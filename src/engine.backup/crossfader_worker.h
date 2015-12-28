#ifndef _HAVE_CROSSFADER_WORKER_H
#define _HAVE_CROSSFADER_WORKER_H

#include <memory>

#include "engine/notifying_master.h"
#include "engine/crossfader_master.h"

struct crossfader_channel_t;

class CrossfaderWorker;

typedef std::shared_ptr<CrossfaderWorker> crossfader_worker_ptr;

class CrossfaderWorker : public MasterNotifyClient
{
    friend class Crossfader;

    public:
        CrossfaderWorker(bool is_main);
        ~CrossfaderWorker();

        /*
         * Connect or disconnect physical controls
         *
         * Note that we may or may not have physical controls (or we may have only one).
         *
         * Also, note that always one of the controls is inverted.
         * - Fader X controls the "live" scene and Fader Y controls the "next" scene.
         * - In "normal" mode, Fader X value 255 means Scene X is at full. Fader Y value 255 means that Scene Y is at zero (inverted.)
         * - In "reverse" mode, Fader X value 0 means that Scene X is at full (inverted). Fader Y value 0 means that Scene Y is at zero.
         *
         * This allows us to perform a crossfade by simultaneously moving both faders, and then moving it in the reverse direction for
         * the next crossfade.
         *
         * With only a single fader, that fader would simultaneously control both X and Y, but swap between "normal" and "reverse"
         * mode.
         */
        // FIXME -- the comment above should really go into CrossfaderMaster class

        /*
         * Set level. We expect this to be called ONLY from timer functions.
         */
        void setX(const unsigned int x);
        void setY(const unsigned int y);

        /*
         * Set level, manual faders. Shall be called from the fader
         */
        void notifyMasterChanged(NotifyingMaster & master, const unsigned int old_level, const unsigned int new_level);

        /*
         * Function that performs the actual calculation. This is called
         * by the timer tick, once per worker. The Pre() function is called
         * before iterating all channels, the Post() function after.
         */
        void timerTickPre();
        void timerTickPost();
        void timerTickPerChannel(crossfader_channel_t & channel);

        inline const bool isFinishing() const { return m_fade_finishing; };
        inline const bool isMain() const { return m_is_main; };

    private:
        bool m_is_main;

        //
        // Variables related to timed fades
        //
        bool timed_fade_in_progress;
        unsigned long m_ticks_delay_in;
        unsigned long m_ticks_delay_out;
        unsigned long m_ticks_fade_in;
        unsigned long m_ticks_fade_out;
        unsigned long m_total_ticks_fade_in;
        unsigned long m_total_ticks_fade_out;
        long double m_stepsize_in;
        long double m_stepsize_out;

        //
        // State variables
        //
        bool m_fade_starting;
        bool m_fade_finishing;                   // Finishing. Set to true only between pre and post tick of the last fade.

        CrossfaderMaster m_xy;

        unsigned int m_my_num_channels;          // Temporary variable that is used to count channels we get to work on during tick.
};

#endif // _HAVE_CROSSFADER_WORKER_H
