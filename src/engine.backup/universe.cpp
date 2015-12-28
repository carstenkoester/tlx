#include <engine/universe.h>
#include <engine/dimmer.h>

/*
 * Global stuff
 */
unsigned int Universe::num_universes = 0;
std::vector<Universe *> Universe::all_universes;

void Universe::createUniverses(const unsigned int num_universes, const ola::OlaCallbackClientWrapper & wrapper,
                               const unsigned int universe_offset, const unsigned int dimmers_per_universe)
{
    all_universes.reserve(Universe::num_universes + num_universes);
    for (unsigned int i = 0; i < num_universes; i++) {
        all_universes.push_back(new Universe(wrapper, Universe::num_universes, Universe::num_universes+universe_offset,
                                             dimmers_per_universe));
        Universe::num_universes++;
    }
}


/*
 * per-Instance functions
 */
Universe::Universe(const ola::OlaCallbackClientWrapper & wrapper,
                   const unsigned int internal_universe, const unsigned int ola_universe,
                   const unsigned int dimmers_per_universe) :
                             m_wrapper(wrapper),
                             m_internal_universe(internal_universe),
                             m_ola_universe(ola_universe)
{
    m_buffer.Blackout();

    Dimmer::createDimmers(dimmers_per_universe, *this);
}

void Universe::setLevel(const unsigned int channel, const unsigned int level)
{
    m_buffer.SetChannel(channel, level);
}

bool Universe::sendData()
{
    m_wrapper.GetClient()->SendDmx(m_ola_universe, m_buffer);
}


