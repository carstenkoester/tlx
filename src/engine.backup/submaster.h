#ifndef _HAVE_SUBMASTER_H
#define _HAVE_SUBMASTER_H

#include <vector>

#include <engine/master.h>
#include <engine/channelsource.h>
#include <engine/scene_notify_client.h>
#include <engine/scene.h>
#include <engine/channel.h>
#include <engine/allsubsmaster.h>

enum submastermode
{
    MODE_UNDEF,
    MODE_SCENE,
    MODE_CROSSFADER
};

class Channel;

class Submaster : public Master, public ChannelSource, public SceneNotifyClient
{
    public:
        inline const submastermode getMode() const { return m_mode; };

        void notifySceneChanged(Scene * p_scene);
        void notifySceneDeleted(Scene * p_scene, const bool unregister);
        void notifyAllsubsMasterChanged();

        void setLevel (const unsigned int level);

        void loadScene(scene_iterator i_scene);
        void updateScene();
        void clear(const bool unregister = true);
        void clear() { clear(true); };

        /*
         * The AllSubs master -- static, as there is only one
         */
        inline static AllsubsMaster * getAllsubsMaster() { return &allsubs_master; };

        /*
         * Functions for managing submasters
         */
        static void createSubmasters(const unsigned int num_universes);

        inline static Submaster * findByNum(const unsigned int submaster) { return all_submasters[submaster]; };
        inline const static unsigned int getNumSubmasters() { return num_submasters; };

    private:
        Submaster(const unsigned int submaster);
        submastermode m_mode;
        const unsigned int m_submaster;   // my ID

        std::vector<bool> m_channel_registered;
        scene_iterator m_i_scene;

        /*
         * The AllSubs master -- static, as there is only one
         */
        static AllsubsMaster allsubs_master;

        /*
         * all Submasters. The term "All submasters" is not to be confused with the one "All Subs" master...
         */
        static unsigned int num_submasters;
        static std::vector<Submaster *> all_submasters;

        /*
         * Helper functions
         */
        inline unsigned int const calcOutputLevel(unsigned int channel)
        {
            if (Channel::findByNum(channel)->noMasters())
            {
                return (m_i_scene->getChannelLevel(channel) * getLevel() / MASTER_MAX_VALUE);
            } else {
                return (m_i_scene->getChannelLevel(channel) * getLevel() / MASTER_MAX_VALUE *
                        allsubs_master.getLevel() / MASTER_MAX_VALUE);
            }
        }

};

#endif // _HAVE_SUBMASTER_H
