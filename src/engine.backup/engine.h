#ifndef _HAVE_ENGINE_H
#define _HAVE_ENGINE_H

#include <vector>
#include <array>
#include <ola/OlaClientWrapper.h>

class EngineTest;

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

    private:
        bool timerTick();

        ola::OlaCallbackClientWrapper m_wrapper;
        ola::io::SelectServer * ss;
};

#endif // _HAVE_ENGINE_H
