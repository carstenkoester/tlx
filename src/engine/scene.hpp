//
// Copyright 2014 Carsten Koester <carsten@ckoester.net>
//
// This file is part of Foobar.
//
// Foobar is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Foobar is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Foobar. If not, see <http://www.gnu.org/licenses/>.
//
#ifndef _SCENE_HPP
#define _SCENE_HPP

#include <boost/signal.hpp>

#include "exceptions.h"

//
// Class that represents a Scene.
//
// A Scene is a collection of (channel, value) pairs. Unlike many other lighting boards
// and applications, not every scene necessarily contains all channels; channels may
// or may not be part of a scene.
//
// An example scene could be:
//   Ch1:unused  Ch2@50%  Ch3@FF  Ch4@00  Ch5:unused  Ch6@00
// Again, notice the difference between "unused" and "@00" here.
//
//
// This specific implementation of a Vector to store scene information (and will grow the
// vector as required to accomodate the highest channel number), and uses a reserved value
// of -1 to indicate that a channel is not used within a scene. That said, this implementation
// (and the size of the internal storage i.e. the vector) shall be invisible and transparent
// to the user.
//
// Note that in this implementation, a scene will never "shrink", i.e. the size of the
// vector will indicate the highest channel that was ever used in this scene, even if that
// channel is no longer used.
//
class Scene
{
    friend class SceneRepository;

    public:
        enum SceneSignalType
        {
            SCENE_CHANGED,
            SCENE_DELETED
        };
        typedef boost::signal<void (SceneSignalType type, Scene* p_scene)> SceneSignal;
        typedef boost::signals::connection SceneConnection;

        EXCEPTION(NotInSceneException, "Channel is not part of Scene");

        //
        // Constructor and destructor.
        //
        Scene(const unsigned int id);
        ~Scene();

        //
        // Generic accessor function to get this scene's ID
        //
        const unsigned int getId() const;

        //
        // Functions to access individual channels
        //
        // There are two functions to access a channel level. getChannelLevel() will throw an exception if
        // the channel requested is not part of the scene, while getChannelLevelOrDefault() will return a
        // (user-specified) default value.
        //
        const bool channelIsUsed(const unsigned int channel) const;
        const unsigned int getChannelLevel(const unsigned int channel) const;
        const unsigned int getChannelLevelOrDefault(const unsigned int channel,
                                                    const unsigned int default_value) const;
        void setChannelLevel(const unsigned int channel, const unsigned int level);
        void deleteChannel(const unsigned int channel);

        //
        // Notify mechanism
        //
        static SceneConnection connect(const SceneSignal::slot_type& slot);
        static void disconnect(const SceneConnection connection);

    private:
        unsigned int m_id;

        //
        // Change Scene ID. This function is private and shall only be called from friend class SceneRepository.
        // This will ensure that the ID is updated within the repository at the same time.
        //
        void renameScene(const unsigned int new_id);

        std::vector<signed long> m_channels;

        //
        // Notify mechanism
        //
        static SceneSignal scene_signal;
};

//
// Inline functions
//
inline const unsigned int Scene::getId() const
{
    return m_id;
};

inline const bool Scene::channelIsUsed(const unsigned int channel) const
{
    return ((channel < m_channels.size()) && (m_channels[channel] >= 0));
}

inline const unsigned int Scene::getChannelLevel(const unsigned int channel) const
{
    if (channelIsUsed(channel)) {
        return m_channels[channel];
    } else {
        throw NotInSceneException();
    }
}

inline const unsigned int Scene::getChannelLevelOrDefault(const unsigned int channel,
                                                          const unsigned int default_value) const
{
    if (channelIsUsed(channel)) {
        return m_channels[channel];
    } else {
        return default_value;
    }
}

//
// Notify mechanism
//
inline Scene::SceneConnection Scene::connect(const SceneSignal::slot_type& slot)
{
    return scene_signal.connect(slot);
}

inline void Scene::disconnect(const SceneConnection connection)
{
    scene_signal.disconnect(connection);
}
#endif // _SCENE_HPP
