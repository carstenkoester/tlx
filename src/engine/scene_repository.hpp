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
#ifndef _SCENE_REPOSITORY_HPP
#define _SCENE_REPOSITORY_HPP

#include <boost/ptr_container/ptr_map.hpp>
#include "exceptions.h"

class Scene;

//
// A repository of scenes.
//
// This is simply a container of Scenes, where each Scene has a unique numerical ID.
//
class SceneRepository
{
    public:
        EXCEPTION(AlreadyDefinedException, "Scene is already defined");
        EXCEPTION(DoesNotExistException, "Scene being modified or deleted does not exist");

        typedef boost::ptr_map<unsigned int,Scene>::iterator SceneIterator;

        bool checkIfSceneExists(const unsigned int) const;
        Scene* pGetScene(const unsigned int);
        void createScene(unsigned int id);
        void renameScene(unsigned int old_id, unsigned int new_id);
        void deleteScene(unsigned int id);
        unsigned int getNumScenes() const;
        SceneIterator begin();
        SceneIterator end();

    private:
        boost::ptr_map<unsigned int,Scene> m_scene_map;
};

//
// Inline functions
//
inline unsigned int SceneRepository::getNumScenes() const
{
    return m_scene_map.size();
}

inline SceneRepository::SceneIterator SceneRepository::begin()
{
    return m_scene_map.begin();
}

inline SceneRepository::SceneIterator SceneRepository::end()
{
    return m_scene_map.end();
}

#endif // _SCENE_REPOSITORY_HPP
