#include <engine/engine.h>
#include <engine/channel.hpp>
#include <logger.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <ola/Logging.h>

#include <engine/universe.hpp>
#include <engine/dimmer.hpp>
#include <engine/submaster.hpp>
#include <engine/crossfader.h>

unsigned int Engine::timerInterval = DEFAULT_TIMER_INTERVAL;

Engine::Engine(const unsigned int num_universes, const unsigned int universe_offset,
               const unsigned int dimmers_per_universe,
               const unsigned int num_channels, const unsigned int num_submasters) :
                   m_grand_master()
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
     */
    m_p_universes.reserve(num_universes);
    m_p_dimmers.reserve(num_universes * dimmers_per_universe);
    m_p_channels.reserve(num_channels);
    m_p_submasters.reserve(num_submasters);

    for (unsigned int i = 0; i < num_universes; ++i) {
        Universe* u = new Universe(m_wrapper, i, universe_offset + i);
        m_p_universes.push_back(u);
        for (unsigned int j = 0; j < dimmers_per_universe; j++)
        {
            m_p_dimmers.push_back(new Dimmer(*u, j));
        }
    }
    for (unsigned int i = 0; i < num_channels; ++i) {
        m_p_channels.push_back(new Channel(m_grand_master, i));
    }
    for (unsigned int i = 0; i < num_submasters; i++) {
        m_p_submasters.push_back(new Submaster(*this, i));
    }

    Crossfader::createMainXY(*this);
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

    for (int i = 0; i < m_p_channels.size() ; i++) {
        m_p_channels[i]->updateDimmers();
    }
    for (int i = 0; i < m_p_universes.size() ; i++) {
        m_p_universes[i]->sendData();
    }

    boost::posix_time::ptime time_end(boost::posix_time::microsec_clock::local_time());
    boost::posix_time::time_duration duration(time_end - time_start);
#ifdef DEBUG
//    LOG_DEBUG("Timer Tick finished. Duration was " << duration);
#endif
    return true;
}
