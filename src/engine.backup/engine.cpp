#include <engine/engine.h>
#include <engine/channel.h>
#include <logger.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <engine/universe.h>
#include <engine/dimmer.h>
#include <engine/submaster.h>
#include <engine/crossfader.h>

unsigned int Engine::timerInterval = DEFAULT_TIMER_INTERVAL;

Engine::Engine(const unsigned int num_universes, const unsigned int universe_offset,
               const unsigned int dimmers_per_universe,
               const unsigned int num_channels, const unsigned int num_submasters)
{
    /*
     * OLA related initialisation
     */
    ola::InitLogging(ola::OLA_LOG_WARN, ola::OLA_LOG_STDERR);
    m_wrapper.Setup();
    ss = Engine::m_wrapper.GetSelectServer();
    ss->RegisterRepeatingTimeout(timerInterval, ola::NewCallback(this, &Engine::timerTick));

    /*
     * Initialise universes, dimmers, channels and submasters
     *
     * Dimmers will indirectly be created by creating the universes.
     */
    Universe::createUniverses(num_universes, m_wrapper, universe_offset, dimmers_per_universe);
    Channel::createChannels(num_channels);
    Submaster::createSubmasters(num_submasters);
    Crossfader::createMainXY();
}

void Engine::Run()
{
    ss->Run();
}

bool Engine::timerTick()
{
#ifdef DEBUG
//    LOG_DEBUG("Timer Tick running");
#endif
    boost::posix_time::ptime time_start(boost::posix_time::microsec_clock::local_time());

    /*
     * FIXME: These really could be moved into helper functions associated
     *        with the class (i.e. Channel::updateAllDimmers());
     */
    Crossfader::main_xy->timerTick();

    for (int i = 0; i < Channel::getNumChannels() ; i++) {
        Channel::findByNum(i)->updateDimmers();
    }
    for (int i = 0; i < Universe::getNumUniverses() ; i++) {
        Universe::findByInternalNum(i)->sendData();
    }

    boost::posix_time::ptime time_end(boost::posix_time::microsec_clock::local_time());
    boost::posix_time::time_duration duration(time_end - time_start);
#ifdef DEBUG
//    LOG_DEBUG("Timer Tick finished. Duration was " << duration);
#endif
    return true;
}
