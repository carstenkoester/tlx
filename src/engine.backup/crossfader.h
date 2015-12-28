#ifndef _HAVE_CROSSFADER_H
#define _HAVE_CROSSFADER_H

#include <memory>

#include "engine/scene.h"
#include "engine/scene_notify_client.h"
#include "engine/cue_notify_client.h"
#include "engine/cuestack.h"
#include "engine/channel.h"
#include "engine/master.h"
#include "engine/notifying_master.h"
#include "engine/crossfader_master.h"
#include "engine/cue.h"
#include "engine/channelsource.h"
#include "engine/crossfader_channel_type.h"
#include "engine/crossfader_worker.h"

/*
 * Crossfader implementation
 *
 * The main ingredients are as follows:
 *
 * 1.) A Crossfader() class. A crossfader is a device that will iterate through a cue stack and fade from one cue (the X cue or
 *     "live" cue to the next (the Y cue or "next" cue).
 *
 *     Typically there would be one crossfader per application. In the future, however, there may be multiple independent crossfaders,
 *     possibly assigned to submasters. A design goal of this application is to implement chasers (aka "effects") simply as an independent
 *     crossfader instance.
 *
 * 2.) A CrossfaderWorker class. This sort of represents a fade-in-progress. A CrossfaderWorker may be associated with two physical
 *     Master (fader) controls, with one physical control, or with no physical controls at all.
 *
 *     Typically there would be one CrossfaderWorker, and that would be associated with the main X/Y controls and the main Crossfader
 *     instance of our application. This would be sufficient for crossfading through one cue stack when all channels fade simultaneously.
 *
 *     However, we do want to implement two additional options,
 *     - We want crossfades to be able to overlap, i.e. (for different channels) be able to start a new fade before the previous
 *       one has completed, and
 *     - eventually we want to support time groups (i.e. associate different fade times for different channels within the same cue).
 *     Both of these options would result in multiple CrossfaderWorker objects going on at the same time.
 *
 */

class Crossfader : public ChannelSource, public MasterNotifyClient, public SceneNotifyClient
{
    public:
        Crossfader();
        ~Crossfader();

        /*
         * Cue stack and Cue management
         */
        void loadCueStack(cuestack_iterator cuestack);
        void loadCueX(cue_iterator cue, bool initial_load = true);
        void loadCueY(cue_iterator cue);

        /*
         * Notification methods to be called when one of our associated cues changed
         */
        void cueChangedNotification();
        void cueDeleteNotification();

        void notifySceneChanged(Scene * p_scene);
        void notifySceneDeleted(Scene* p_scene, bool unregister);

        /*
         * Set level, manual faders. Shall be called from the fader
         */
        void notifyMasterChanged(NotifyingMaster & master, const unsigned int old_level, const unsigned int new_level);

        //
        // Automatic cue crossfade
        //
        void go();

    // friend engine
        void timerTick();

        /*
         * These are static members defining our primary X/Y crossfader, known as X1/X1. (Should there be additional
         * crossfaders, they will be named X2/Y2 etc.
         *
         * Note that these refers to the physical controls. Even in the absence of physical controls, we might
         * be running multiple Crossfader objects simultaneously, eventually perhaps with a submaster attached as combined
         * XY fader, or in the background with no physical fader at all.
         */
        static NotifyingMaster * x1;
        static NotifyingMaster * y1;
        static CrossfaderMaster * x1y1;
        static Master * speed;
        static Master * master;               // Level master, comparible to Grand Master or AllSubs master but applies to xfader display
        static Crossfader * main_xy;

        inline static Crossfader * getMainXY() { return main_xy; };
        static void createMainXY();

        void cueHasChanged(Cue::cueSignalType type, unsigned int cuestack, unsigned long int cue);

    private:
        /*
         * Data structures owned by each Crossfader instance
         */
        bool m_has_cuestack;                // Do we have a cue stack?
        bool m_has_cue_x;                   // Do we have an X cue?
        bool m_has_cue_y;                   // Do we have a Y cue?
        bool m_has_scene_x;                 // Do we have an X scene (other than the empty scene)?
        bool m_has_scene_y;                 // Do we have a Y scene?
        cuestack_iterator m_cuestack;       // Iterator to cuestack. May be undefined/invalid if m_has_cuestack=false.
        cue_iterator m_cue_x;               // Iterator to X cue. As above.
        cue_iterator m_cue_y;               // Iterator to Y cue. As above.
        scene_iterator m_scene_x;           // Iterator to X scene. If our associated cue has no scene, this shall be the empty scene.
        scene_iterator m_scene_y;           // As above.

        std::list<crossfader_worker_ptr> m_workers;

        /*
         * Physical controls. We pass this on to one of our worker threads.
         */
        CrossfaderMaster m_xy;
        Master * m_levelmaster;

        bool m_fade_in_progress;

        std::vector<crossfader_channel_t> m_channels;

        boost::signals::connection connection_cue;

        /*
         * Internal helper functions
         */
        void startFade(bool manual);
        void updateScenes();


        /*
         * Scene with all-zero channels. We use this in place of an actual scene for cues that have no scene.
         */
        static std::list<Scene> empty_scene;
        static scene_iterator empty_scene_iterator;
};

#endif // _HAVE_CROSSFADER_H
