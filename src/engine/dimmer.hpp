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
#ifndef _DIMMER_HPP
#define _DIMMER_HPP

#include <string>

#include "exceptions.hpp"

class Channel;
class Universe;

//
// Class that represents a single DMX output, aka dimmer.
//
// The dimmer is associated with a given universe (and "DMX channel" inside that universe)
// when constructed, and that association remains static throughout the dimmer instance's lifetime.
//
// A dimmer can be associated with a Channel object by patching/unpatching. A dimmer that is not patched
// to a channel is set to a default level of zero, and that level cannot be changed. A dimmer can be
// patched to exactly one channel.
//
// Each dimmer has a numerical, zero-based ID, and a name string that is "universe.dimmer" where both
// dimmer and universe a 1-based.
//
// TODO - In the future, a dimmer should support configurable properties such as a minimum/maximum levels
//        or a dimmer curve.
//
class Dimmer
{
    public:
        EXCEPTION(AlreadyPatchedException, "Dimmer already patched to a channel");
        EXCEPTION(NotPatchedException, "Attempting to unpatch dimmer that is not patched");

        //
        // Constructor - create a new Dimmer. Takes a reference to a universe, and the (numerical,
        // zero-based) dimmer ID.
        //
        Dimmer(Universe& universe, const unsigned int dimmer);

        //
        // Accessor functions for reading the identity of this dimmer
        //
        const Universe& getUniverse() const;
        const unsigned int getId() const;
        const std::string getNameString() const;

        //
        // Patch and unpatch from/to channel
        //
        void patchToChannel(Channel* channel);
        void unpatchFromChannel(Channel* channel);
        const Channel* pGetPatchedChannel() const;

        //
        // Set/get this channel's level
        //
	void setLevel(const unsigned int level);
        const unsigned int getLevel() const;

    private:
        Universe& m_universe;
        const unsigned int m_id;
        const std::string m_name_string;

        Channel* m_p_patched_channel;

        unsigned int m_level;
};

//
// Inline functions
//
inline const Universe& Dimmer::getUniverse() const
{
    return m_universe;
};

inline const unsigned int Dimmer::getId() const
{
    return m_id;
};

inline const std::string Dimmer::getNameString() const
{
    return m_name_string;
};

inline const unsigned int Dimmer::getLevel() const
{
    return m_level;
};

inline const Channel* Dimmer::pGetPatchedChannel() const
{
    return m_p_patched_channel;
}

#endif // _DIMMER_HPP
