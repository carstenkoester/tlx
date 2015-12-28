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
#ifndef _SUBMASTER_HPP
#define _SUBMASTER_HPP

#include <vector>

#include "engine/master.hpp"
#include "engine/scene.hpp"
#include "engine/channel.hpp"
#include "engine/engine.h"

//
// Class that represents a submaster.
//
// A submaster is essentially a fader that can be loaded with a Scene and then fade in or out the entire scene.
// In the future, a Submaster will also be able to control a crossfader.
//
// The Submaster connects to a "Allsubs Master" which acts as an overall master for all Submasters.
//
class Submaster : public Master
{
    public:
        enum SubmasterMode
        {
            MODE_UNDEF,
            MODE_SCENE
            // MODE_CROSSFADER
        };

        //
        // Constructor - create a new submaster.
        //
        Submaster(Engine& engine, const unsigned int id);
        ~Submaster();

        //
        // Accessor functions
        //
        const unsigned int getId() const;
        const SubmasterMode getMode() const;
        void setLevel (const unsigned int level);

        //
        // Scene related functions
        //
        void loadScene(Scene* p_scene);
        void clear();

    private:
        const unsigned int m_id;
        Engine& m_engine;
        SubmasterMode m_mode;
        std::vector<bool> m_channel_registered;

        //
        // Scene related data
        //
        Scene* m_p_scene;

        void updateScene();
        const unsigned int sceneCalcLevel(unsigned int channel) const;

        //
        // Scene signal slot
        //
        void sceneHasChanged(Scene::SceneSignalType type, Scene* p_scene);
        Scene::SceneConnection m_scene_connection;

        //
        // AllSubsMaster signal slot
        //
        void allSubsHasChanged(unsigned int level);
        Master::MasterConnection m_allsubs_connection;
};

//
// Inline functions
//
inline const unsigned int Submaster::getId() const
{
    return m_id;
}

inline const Submaster::SubmasterMode Submaster::getMode() const
{
    return m_mode;
}

//
// Helper functions
//
inline unsigned int const Submaster::sceneCalcLevel(unsigned int channel) const
{
    if (m_engine.getChannel(channel).noMasters())
    {
        return (apply(m_p_scene->getChannelLevel(channel)));
    } else {
        return (m_engine.getAllsubsMaster().apply(apply(m_p_scene->getChannelLevel(channel))));
    }
}
#endif // _SUBMASTER_HPP
