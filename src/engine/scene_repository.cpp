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
#include "engine/scene_repository.hpp"

#include "engine/scene.hpp"

bool SceneRepository::checkIfSceneExists(const unsigned int id) const
{
    return (!(m_scene_map.find(id) == m_scene_map.end()));
}

Scene* SceneRepository::pGetScene(unsigned int id)
{
    SceneIterator it = m_scene_map.find(id);
    if (it == m_scene_map.end())
    {
        throw DoesNotExistException();
    } else {
        return it->second;
    }
}

void SceneRepository::createScene(unsigned int id)
{
    if (checkIfSceneExists(id))
    {
        throw AlreadyDefinedException();
    }
    m_scene_map.insert(id, new Scene(id));
}

void SceneRepository::renameScene(unsigned int old_id, unsigned int new_id)
{
    if (checkIfSceneExists(new_id))
    {
        throw AlreadyDefinedException();
    }

    SceneIterator it = m_scene_map.find(old_id);
    if (it == m_scene_map.end())
    {
        throw DoesNotExistException();
    }
    boost::ptr_map<unsigned int,Scene>::auto_type ptr = m_scene_map.release(it);
    Scene* s = ptr.release();
    s->renameScene(new_id);
    m_scene_map.insert(new_id, s);
}

void SceneRepository::deleteScene(unsigned int id)
{
    if (!checkIfSceneExists(id))
    {
        throw DoesNotExistException();
    }
    m_scene_map.erase(id);
}
