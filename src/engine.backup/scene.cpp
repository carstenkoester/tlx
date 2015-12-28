#include <engine/scene.h>

#include <logger.h>
#include <engine/exceptions.h>
#include <engine/channel.h>

std::list<Scene> Scene::m_scenes;

Scene::Scene(const unsigned int id, const unsigned int total_num_channels) :
                 m_id(id),
                 m_total_num_channels(total_num_channels),
                 m_channels(total_num_channels, CHANNEL_UNUSED)
{
    LOG_DEBUG("CONSTRUCTOR: Scene " << m_id);
}

Scene::~Scene()
{
    std::list<SceneNotifyClient *>::iterator i_p_notify_clients;

    LOG_DEBUG("Destructor Scene " << m_id << " called, notifying " << m_p_notify_clients.size() << " users");
    i_p_notify_clients =  m_p_notify_clients.begin();
    while (i_p_notify_clients != m_p_notify_clients.end())
    {
        (*i_p_notify_clients)->notifySceneDeleted(this, false);
        i_p_notify_clients++;
    }
    m_p_notify_clients.clear();
}

void Scene::resize(const unsigned int total_num_channels)
{
    m_channels.resize(total_num_channels, CHANNEL_UNUSED);
    m_total_num_channels = total_num_channels;
}

void Scene::setChannelLevel(const unsigned int channel, const unsigned int level)
{
    if (channel >= m_total_num_channels) {
        resize(channel + 1);
    }
    m_channels[channel] = level;
    notifyClients();
}

void Scene::deleteChannel(const unsigned int channel)
{
    if (channel < m_total_num_channels) {
        m_channels[channel] = CHANNEL_UNUSED;
    }
    notifyClients();
}

void Scene::addNotifyClient(SceneNotifyClient *p_notify_client)
{
    LOG_DEBUG("Scene " << m_id << " adding notify client");
    m_p_notify_clients.push_back(p_notify_client);
}

void Scene::removeNotifyClient(SceneNotifyClient *p_notify_client)
{
    LOG_DEBUG("Scene " << m_id << " removing notify client");
    m_p_notify_clients.remove(p_notify_client);
}

void Scene::notifyClients()
{
    LOG_DEBUG("Scene " << m_id << " sending notify client change");
    std::list<SceneNotifyClient *>::iterator i_p_notify_clients;

    for (i_p_notify_clients = m_p_notify_clients.begin(); i_p_notify_clients != m_p_notify_clients.end();
         i_p_notify_clients++)
    {
        (*i_p_notify_clients)->notifySceneChanged(this);
    }
}


/*
 * Scene repository management
 */
bool Scene::checkIfSceneExists(const unsigned int id)
{
    std::list<Scene>::iterator i_scenes;
    for (i_scenes = m_scenes.begin(); i_scenes != m_scenes.end(); i_scenes++)
    {
        if (i_scenes->getId() == id)
        {
            return true;
        }
    }
    return false;
}

scene_iterator Scene::findScene(const unsigned int id)
{
    std::list<Scene>::iterator i_scenes;
    for (i_scenes = m_scenes.begin(); i_scenes != m_scenes.end(); i_scenes++)
    {
        if (i_scenes->getId() == id)
        {
            return i_scenes;
        }
    }
    throw SceneDoesNotExistException();
}

bool compareScenes(Scene &s1, Scene &s2)
{
    return (s1.getId() < s2.getId());
}

void Scene::createScene(unsigned int id)
{
    if (checkIfSceneExists(id))
    {
        throw SceneAlreadyDefinedException();
    }
    m_scenes.emplace_back(id, Channel::getNumChannels());
    m_scenes.sort(compareScenes);
}

void Scene::renameScene(unsigned int old_id, unsigned int new_id)
{
    if (checkIfSceneExists(new_id))
    {
        throw SceneAlreadyDefinedException();
    }
    findScene(old_id)->m_id = new_id;
    m_scenes.sort(compareScenes);
}

void Scene::deleteScene(unsigned int id)
{
    if (!checkIfSceneExists(id))
    {
        throw SceneDoesNotExistException();
    }
    m_scenes.erase(findScene(id));
}
