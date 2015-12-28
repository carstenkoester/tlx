#ifndef _HAVE_DIMMER_H
#define _HAVE_DIMMER_H

#include <string>

#include <engine/universe.h>

class Channel;

class Dimmer
{
    public:
        const Universe & get_Universe() const { return m_universe; };
        const unsigned int getUniverse() const { return m_universe.getInternalUniverse(); };
        const unsigned int getDimmer() const { return m_dimmer; };
        const std::string getNameString() const { return m_name_string; };

        void patchToChannel(Channel * p_channel);
        void unpatchFromChannel(Channel * p_channel);

	void setLevel(const unsigned int level);
        inline const unsigned int getLevel() const { return m_level; };

        static void createDimmers(const unsigned int num_dimmers, Universe & universe);
        inline static Dimmer * findByNum(const unsigned int dimmer) { return all_dimmers[dimmer]; };
        inline const static unsigned int getNumDimmers() { return num_dimmers; };


    private:
        Dimmer(Universe & universe, const unsigned int dimmer);

        Universe & m_universe;
        const unsigned int m_dimmer;

        Channel * m_p_patchedChannel;
        unsigned int m_level;
        std::string m_name_string;

        /*
         * all Dimmers
         */
        static unsigned int num_dimmers;
        static std::vector<Dimmer *> all_dimmers;
};

#endif // _HAVE_DIMMER_H
