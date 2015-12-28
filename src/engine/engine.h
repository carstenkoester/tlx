#ifndef _HAVE_ENGINE_H
#define _HAVE_ENGINE_H

#include <vector>
#include <array>
#include <ola/OlaClientWrapper.h>

#include "engine/master.hpp"
#include "engine/scene_repository.hpp"

class EngineTest;

class Submaster;
class Universe;
class Dimmer;
class Channel;

// #define FOR_ALL_CHANNELS_INT(x) for (int x = 0; x < m_engine.getNumChannels(); ++x)

#define DEFAULT_TIMER_INTERVAL 25               // 1000 msec/25 = 40 ticks per second
class Engine
{
    friend class EngineTest;

    public:
        Engine(const unsigned int num_universes, const unsigned int universe_offset,
               const unsigned int dimmers_per_universe,
               const unsigned int num_channels, const unsigned int num_submasters);
        void Run();

        static unsigned int timerInterval;

        const Dimmer& getDimmer(unsigned int id) const;
        // const Channel& getChannel(unsigned int id) const;
        Channel& getChannel(unsigned int id) const;
        const unsigned int getNumChannels() const;
        Master& getAllsubsMaster();
        
        SceneRepository scenes;

    private:
        ///
        /// Data associated with the application
        ///
        bool timerTick();

        ola::OlaCallbackClientWrapper m_wrapper;
        ola::io::SelectServer * ss;

        Master m_grand_master;
        Master m_allsubs_master;

        std::vector<Universe *> m_p_universes;
        std::vector<Dimmer *> m_p_dimmers;
        std::vector<Channel *> m_p_channels;
        std::vector<Submaster *> m_p_submasters;
};

inline const Dimmer& Engine::getDimmer(unsigned int id) const
{
    return *m_p_dimmers[id];
}

/* inline const Channel& Engine::getChannel(unsigned int id) const
{
    return *m_p_channels[id];
} */

inline Channel& Engine::getChannel(unsigned int id) const
{
    return *m_p_channels[id];
}

inline const unsigned int Engine::getNumChannels() const
{
    return m_p_channels.size();
}

inline Master& Engine::getAllsubsMaster()
{
    return m_allsubs_master;
}

#endif // _HAVE_ENGINE_H
