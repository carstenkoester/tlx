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
#include <boost/bind.hpp>
#include <logger.h>

#include <engine/submaster.hpp>

#include "engine/channel.hpp"
#include "engine/engine.h"

//
// Shortcut macro that will be used throughout the functions
//
#define FOR_ALL_CHANNELS(x) for (int x = 0; x < m_engine.getNumChannels(); ++x)

Submaster::Submaster(Engine& engine, const unsigned int id) :
                         Master(),
                         m_engine(engine),
                         m_id(id),
                         m_mode(MODE_UNDEF),
                         m_channel_registered(engine.getNumChannels(), false)
{
    m_scene_connection = Scene::connect(boost::bind(&Submaster::sceneHasChanged, this, _1, _2));
    m_allsubs_connection = m_engine.getAllsubsMaster().connect(boost::bind(&Submaster::allSubsHasChanged, this, _1));
};

Submaster::~Submaster()
{
    m_engine.getAllsubsMaster().disconnect(m_allsubs_connection);
    Scene::disconnect(m_scene_connection);
}

void Submaster::setLevel(const unsigned int level)
{
    m_level = level;

    if (m_mode == MODE_SCENE)
    {
        FOR_ALL_CHANNELS(i)
        {
            if (m_p_scene->channelIsUsed(i))
            {
                m_engine.getChannel(i).setLevel(this, sceneCalcLevel(i));
            }
        }
    }
}

void Submaster::loadScene(Scene* p_scene)
{
    m_mode = MODE_SCENE;
    m_p_scene = p_scene;

    FOR_ALL_CHANNELS(i)
    {
        if (m_p_scene->channelIsUsed(i))
        {
            m_engine.getChannel(i).registerSource(this, Channel::SOURCE_SUBMASTER, sceneCalcLevel(i));
            m_channel_registered[i] = true;
        }
    }
}

void Submaster::clear()
{
    m_mode = MODE_UNDEF;

    FOR_ALL_CHANNELS(i)
    {
        if (m_channel_registered[i])
        {
            m_engine.getChannel(i).unregisterSource(this);
            m_channel_registered[i] = false;
        }
    }
}

void Submaster::updateScene()
{
    FOR_ALL_CHANNELS(i)
    {
        if ((m_p_scene->channelIsUsed(i)) && (!m_channel_registered[i]))
        {
            m_engine.getChannel(i).registerSource(this, Channel::SOURCE_SUBMASTER, sceneCalcLevel(i));
            m_channel_registered[i] = true;
        } else if ((!m_p_scene->channelIsUsed(i)) && (m_channel_registered[i])) {
            m_engine.getChannel(i).unregisterSource(this);
            m_channel_registered[i] = false;
        } else if (m_p_scene->channelIsUsed(i)) {
            m_engine.getChannel(i).setLevel(this, sceneCalcLevel(i));
        }
    }
}

void Submaster::sceneHasChanged(Scene::SceneSignalType type, Scene* p_scene)
{
    if ((m_mode == MODE_SCENE) && (m_p_scene == p_scene))
    {
        if (type == Scene::SCENE_CHANGED)
        {
            LOG_DEBUG("SUBMASTER: Notify Scene Changed");
            updateScene();
        } else { // type == SCENE_DELETED
            LOG_DEBUG("SUBMASTER: Notify Scene Deleted");
            clear();
        }
    }
}

void Submaster::allSubsHasChanged(unsigned int level)
{
    //
    // Note that the "level" which we receive as a parameter is the AllSubsMaster's level --
    // this is NOT our own level.
    // We'll simply call setLevel with *our own* level, which will refresh the output and
    // pull in the Allsubs master on the way.
    //
    setLevel(m_level);
}
