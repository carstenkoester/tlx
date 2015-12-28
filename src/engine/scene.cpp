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
#include <logger.h>

#include <engine/scene.hpp>

#include <engine/channel.hpp>

const static signed long CHANNEL_UNUSED = -1;
Scene::SceneSignal Scene::scene_signal;


Scene::Scene(const unsigned int id) :
                 m_id(id),
                 m_channels(0, CHANNEL_UNUSED)
{
    LOG_DEBUG("SCENE: Constructor Scene " << m_id);
}

Scene::~Scene()
{
    LOG_DEBUG("SCENE: Destructor Scene " << m_id);
    scene_signal(SCENE_DELETED, this);
}


void Scene::setChannelLevel(const unsigned int channel, const unsigned int level)
{
    if (channel >= m_channels.size()) {
        m_channels.resize(channel + 1, CHANNEL_UNUSED);
    }
    m_channels[channel] = level;
    scene_signal(SCENE_CHANGED, this);
}

void Scene::deleteChannel(const unsigned int channel)
{
    if (channel < m_channels.size()) {
        m_channels[channel] = CHANNEL_UNUSED;
    }
    scene_signal(SCENE_CHANGED, this);
}

void Scene::renameScene(unsigned int new_id)
{
    m_id = new_id;
    scene_signal(SCENE_CHANGED, this);
}
