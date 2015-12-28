#ifndef _HAVE_UNIVERSE_H
#define _HAVE_UNIVERSE_H

#include <ola/DmxBuffer.h>
#include <ola/Logging.h>
#include <ola/OlaClientWrapper.h>
#include <ola/Callback.h>

class Universe
{
    public:
        const unsigned int getInternalUniverse() { return m_internal_universe; };
        const unsigned int getOlaUniverse() { return m_ola_universe; };
	void setLevel(const unsigned int channel, const unsigned int level);
	bool sendData();

        /*
         * Functions for managing channels
         */
        static void createUniverses(const unsigned int num_universes, const ola::OlaCallbackClientWrapper & wrapper,
                               const unsigned int universe_offset, const unsigned int dimmers_per_universe);

        inline static Universe * findByInternalNum(const unsigned int internal_universe)
            { return all_universes[internal_universe]; };
        inline const static unsigned int getNumUniverses() { return num_universes; };

    private:
        Universe(const ola::OlaCallbackClientWrapper & wrapper,
                 const unsigned int internal_universe, const unsigned int ola_universe,
                 const unsigned int dimmers_per_universe);

        const ola::OlaCallbackClientWrapper & m_wrapper;
        const unsigned int m_internal_universe;
        const unsigned int m_ola_universe;

        ola::DmxBuffer m_buffer;

        /*
         * all Universes
         */
        static unsigned int num_universes;
        static std::vector<Universe *> all_universes;
};

#endif // _HAVE_UNIVERSE_H
