#ifndef _HAVE_SCENE_H
#define _HAVE_SCENE_H

#include <logger.h>
#include <engine/exceptions.h>

#include <list>
#include <exception>

#include <engine/scene_notify_client.h>
#include <boost/signal.hpp>

#define CHANNEL_UNUSED -1

class Scene;

typedef std::list<Scene>::iterator scene_iterator;

class SceneAlreadyDefinedException: public std::exception
{
    public:
        virtual const char* what() const throw()
        {
            return "Scene already exists";
        }
};
class SceneDoesNotExistException: public std::exception
{
    public:
        virtual const char* what() const throw()
        {
            return "Scene does not exist";
        }
};


class Scene
{
    typedef boost::signal<void, bool> SceneSignal;

    public:
        Scene(const unsigned int id, unsigned int total_num_channels);
        ~Scene();
        const unsigned int getId() const { return m_id; };

        inline const unsigned int getTotalNumChannels() const { return m_total_num_channels; };
        inline const bool channelIsUsed(const unsigned int channel) const
            { return ((channel < m_total_num_channels) && (m_channels[channel] >= 0)); }
        inline const unsigned int getChannelLevel(const unsigned int channel) const
            { if ((channel < m_total_num_channels) && (m_channels[channel] >= 0)) {
                return m_channels[channel];
            } else {
                throw ChannelNotInSceneException();
            } }
        inline const unsigned int getChannelLevelIfUsed(const unsigned int channel, const unsigned int default_value) const
            { if ((channel < m_total_num_channels) && (m_channels[channel] >= 0)) {
                return m_channels[channel];
            } else {
                return default_value;
            } }
        void setChannelLevel(const unsigned int channel, const unsigned int level);
        void deleteChannel(const unsigned int channel);

        /*
         * Notify mechanism
         */
        void addNotifyClient(SceneNotifyClient *p_notify_client);
        void removeNotifyClient(SceneNotifyClient *p_notify_client);

        /*
         * Scene repository management
         */
        static scene_iterator findScene(const unsigned int);
        static bool checkIfSceneExists(const unsigned int);
        static void createScene(unsigned int id);
        static void renameScene(unsigned int old_id, unsigned int new_id);
        static void deleteScene(unsigned int id);
        inline static unsigned int getNumScenes() { return m_scenes.size(); };
        // static const std::list<Scene>& getSceneList() { return m_scenes; };
        inline static scene_iterator sceneListBegin() { return m_scenes.begin(); };
        inline static scene_iterator sceneListEnd() { return m_scenes.end(); };

    private:
        /*
         * This implementation stores the scene as a list of *signed long* integers (so we have a larger data type
         * than "unsigned int") and uses a negative level to indicate that a channel is not part of the scene.
         *
         * A different implementation would be to make this a list of (channel, level) tuples. Time will tell if one
         * has any advantages over the other.
         *
         */
        unsigned int m_id;

        void resize(const unsigned int total_num_channels);

        std::vector<signed long> m_channels;
        unsigned int m_total_num_channels;

        std::list<SceneNotifyClient *> m_p_notify_clients;
        void notifyClients();

        /*
         * Scene repository
         */
        static std::list<Scene> m_scenes;
};

#endif // _HAVE_SCENE_H
