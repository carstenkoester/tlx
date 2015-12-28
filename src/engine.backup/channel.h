#ifndef _HAVE_CHANNEL_H
#define _HAVE_CHANNEL_H

#include <list>
#include <vector>

class Dimmer;
class Master;
class ChannelSource;

struct LevelElement
{
    const ChannelSource * p_source;
    unsigned int level;
};

enum mergemode_t
{
    MERGEMODE_HTP,
    MERGEMODE_LTP
};

class Channel;
typedef std::vector<Channel *>::iterator channel_iterator;
#define FOR_ALL_CHANNELS_ITERATOR(x) for (channel_iterator x = Channel::channelsBegin(); x != Channel::channelsEnd(); ++x)
#define FOR_ALL_CHANNELS_INT(x) for (int x = 0; x < Channel::getNumChannels(); ++x)

class Channel
{
    friend class EngineTest;

    public:
        inline const unsigned int getNum() const { return m_num; }
        inline const unsigned int getInputLevel() const { return m_inputlevel; };
        inline const unsigned int getOutputLevel() const { return m_outputlevel; };

        /*
         * Get channel properties
         */
        inline const bool noMasters() const { return m_nomasters; };
        inline const bool noFade() const { return m_nofade; };

        /*
         * Patching functions
         */
        void addDimmer(Dimmer *p_dimmer);
        void removeDimmer(Dimmer *p_dimmer);

        /*
         * Input/sources: Registering and de-registering sources, setting levels
         */
        void setLevel(const ChannelSource * p_source, const unsigned int level);
        void registerSource(const ChannelSource * p_source, const unsigned int level);
        void unregisterSource(const ChannelSource * p_source);

        /*
         * Push output to dimmers
         */
        void updateDimmers();

// FIXME        bool m_flash;               // Flash active.

        /*
         * Grand Master. This is static, as there is only one Grand Master.
         */
        inline static Master * getGrandMaster() { return &grand_master; };

        /*
         * Functions for managing channels
         */
        static void createChannels(const unsigned int num_channels);
        inline static Channel * findByNum(const unsigned int channel) { return all_channels[channel]; };
        inline const static unsigned int getNumChannels() { return num_channels; };
        inline static channel_iterator channelsBegin() { return all_channels.begin(); };
        inline static channel_iterator channelsEnd() { return all_channels.end(); };

    private:
        /*
         * per-Channel properties
         */
        Channel(const unsigned int m_num);

        unsigned int m_inputlevel;  // Current input level. "input level" means level received from
                                    // input source (crossfader or submaster), before applying
                                    // masters.
        unsigned int m_outputlevel; // Output level. This is after applying grand master, blackout, ...
        unsigned int m_num;

        unsigned int m_default_level;
        mergemode_t m_mergemode;
        bool m_nomasters;           // bypass masters
        bool m_nofade;              // Do not fade. Always jump to target level.
// FIXME        bool m_nosolo;              // Don't blackout this channel even when "solo" is active.

        std::list<LevelElement> m_inputlevels;
        std::list<Dimmer *> m_p_dimmers;

        const ChannelSource * m_p_override_source;
        const ChannelSource * m_p_active_source;

        static Master grand_master;

        /*
         * all Channels
         */
        static unsigned int num_channels;
        static std::vector<Channel *> all_channels;
};

#endif // _HAVE_CHANNEL_H
