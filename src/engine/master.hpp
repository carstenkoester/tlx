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
#ifndef _MASTER_HPP
#define _MASTER_HPP

#include <boost/signal.hpp>

//
// This class defines a Master, which is simply a representation of an (imaginary) fader.
//
// A master has a current level and a "full" level that can both be queried, and has the
// ability to notify users (by sending a Boost::signal) when it's value changes.
//
// Note that currenty, a master's level CAN exceed it' full level -- there is no checking done
// to prevent so.
// This may be useful for things such as "speed masters" where we can simply define the middle as
// "full" (100%) so the effective range of the fader is from 0% to 200%.
//
class Master
{
    public:
        static const unsigned int MASTER_FULL_LEVEL = 255;

        typedef boost::signal<void (unsigned int level)> MasterSignal;
        typedef boost::signals::connection MasterConnection;

        //
        // Constructor - generate a new master. Both "level" and "full_level" are optional.
        //
        Master(unsigned int level = MASTER_FULL_LEVEL,
               unsigned int full_level = MASTER_FULL_LEVEL);

        //
        // Accessor functions: Get/set the level, and a couple of shortcuts
        //
        virtual void setLevel (const unsigned int level);
        const unsigned int getLevel() const;
        const unsigned int getFullLevel() const;
        const bool isZero() const;
        const bool isFull() const;

        //
        // Apply the master to a given value (that is, multiply the input
        // value with the ratio of the current master and "full". If our
        // "full" value is at 255 and the master setting is at 192, then
        // the master is 75% up,  so "apply(200)" should return 150.)
        //
        const unsigned int apply(const unsigned int in) const;

        //
        // Notify mechanism
        //
        MasterConnection connect(const MasterSignal::slot_type& slot);
        void disconnect(const MasterConnection connection);

    protected:
        unsigned int m_level;
        unsigned int m_full_level;

        //
        // Notify mechanism
        //
        MasterSignal m_master_signal;
};

inline const unsigned int Master::getLevel() const
{
    return m_level;
};

inline const unsigned int Master::getFullLevel() const
{
    return m_full_level;
};

inline const bool Master::isZero() const
{
    return m_level == 0;
}

inline const bool Master::isFull() const
{
    return m_level == m_full_level;
}

inline const unsigned int Master::apply(const unsigned int in) const
{
    return in * m_level / m_full_level;
}

//
// Notify mechanism
//
inline Master::MasterConnection Master::connect(const MasterSignal::slot_type& slot)
{
    return m_master_signal.connect(slot);
}

inline void Master::disconnect(const MasterConnection connection)
{
    m_master_signal.disconnect(connection);
}
#endif // _MASTER_HPP
