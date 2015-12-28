#include <engine/submaster.h>

#include <engine/scene.h>
#include <engine/channel.h>

/*
 * Global stuff
 */
unsigned int Submaster::num_submasters = 0;
std::vector<Submaster *> Submaster::all_submasters;

void Submaster::createSubmasters(const unsigned int num_submasters)
{
    all_submasters.reserve(Submaster::num_submasters + num_submasters);
    for (unsigned int i = 0; i < num_submasters; i++) {
        all_submasters.push_back(new Submaster(Submaster::num_submasters++));
    }
}

AllsubsMaster Submaster::allsubs_master = AllsubsMaster(MASTER_MAX_VALUE);

Submaster::Submaster(const unsigned int submaster) :
                         Master(0),
                         m_submaster(submaster),
                         ChannelSource(SOURCE_SUBMASTER),
                         m_mode(MODE_UNDEF),
                         m_channel_registered(Channel::getNumChannels(), false)
{
};

void Submaster::loadScene(scene_iterator i_scene)
{
    m_mode = MODE_SCENE;
    m_i_scene = i_scene;

    i_scene->addNotifyClient(this);

    FOR_ALL_CHANNELS_INT(i)
    {
        if (m_i_scene->channelIsUsed(i))
        {
            LOG_DEBUG("loadScene Channel " << i << " Level " << calcOutputLevel(i));
            Channel::findByNum(i)->registerSource(this, calcOutputLevel(i));
            m_channel_registered[i] = true;
        }
    }
}

void Submaster::clear(const bool unregister_scene)
{
    if (m_mode == MODE_SCENE && unregister_scene)
    {
        m_i_scene->removeNotifyClient(this);
    }
    m_mode = MODE_UNDEF;

    FOR_ALL_CHANNELS_INT(i)
    {
        if (m_channel_registered[i])
        {
            Channel::findByNum(i)->unregisterSource(this);
            m_channel_registered[i] = false;
        }
    }
}

void Submaster::setLevel(const unsigned int level)
{
    m_level = level;

    if (m_mode == MODE_SCENE)
    {
        FOR_ALL_CHANNELS_INT(i)
        {
            if (m_i_scene->channelIsUsed(i))
            {
                LOG_DEBUG("SetLevel Channel " << i << " Level " << calcOutputLevel(i));
                Channel::findByNum(i)->setLevel(this, calcOutputLevel(i));
            }
        }
    }
}

void Submaster::updateScene()
{
    FOR_ALL_CHANNELS_INT(i)
    {
        if ((m_i_scene->channelIsUsed(i)) && (!m_channel_registered[i]))
        {
            LOG_DEBUG("Scene changed, NEW CHANNEL " << i << " used, registering");
            LOG_DEBUG("SetLevel Channel " << i << " Level " << calcOutputLevel(i));
            Channel::findByNum(i)->registerSource(this, calcOutputLevel(i));
            m_channel_registered[i] = true;
        } else if ((!m_i_scene->channelIsUsed(i)) && (m_channel_registered[i])) {
            LOG_DEBUG("Scene changed, channel " << i << " NO LONGER used, de-registering");
            Channel::findByNum(i)->unregisterSource(this);
            m_channel_registered[i] = false;
        } else if (m_i_scene->channelIsUsed(i)) {
            LOG_DEBUG("Scene changed, channel " << i << " was and is used, re-submitting");
            Channel::findByNum(i)->setLevel(this, calcOutputLevel(i));
        }
    }
}

void Submaster::notifySceneChanged(Scene * p_scene)
{
    assert(m_mode == MODE_SCENE);
    assert(p_scene == &(*m_i_scene));

    LOG_DEBUG("SUB: Notify Scene Changed");

    updateScene();
}

void Submaster::notifySceneDeleted(Scene * p_scene, const bool unregister)
{
    assert(m_mode == MODE_SCENE);
    assert(p_scene == &(*m_i_scene));

    LOG_DEBUG("SUB: Notify Scene Deleted");

    clear(unregister);
}

void Submaster::notifyAllsubsMasterChanged()
{
    if (m_mode == MODE_SCENE)
    {
        updateScene();
    }
}
