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
#ifndef _UNIVERSE_HPP
#define _UNIVERSE_HPP

#include <string>

#include <ola/DmxBuffer.h>
#include <ola/OlaClientWrapper.h>

//
// Class that represents a DMX Universe.
//
// The universe is essentially a buffer of (max.) 512 channel values. These values can
// be set individually by the "setLevel" method, and sent to the underlying OLA DMX universe
// by the "sendData" method.
//
// Each universe has a numerical, zero-based ID and a name string *which is one-based*.
// When creating an universe, we can specify an offset towards the OLA DMX universe, so that
// e.g. this application's universe 0 can be mapped to OLA universe 4.
//
class Universe
{
    public:
        //
        // Constructor - create a new universe
        //
        Universe(const ola::OlaCallbackClientWrapper & wrapper,
                 const unsigned int internal_universe, const unsigned int ola_universe);

        //
        // Return information about this universe
        //
        const unsigned int getInternalId() const;
        const unsigned int getOlaId() const;
        const std::string getNameString() const;

        //
        // Universe management. Set the actual output level of a channel
        // (note that this referse to "channel" in the OLA sense, i.e. a channel 0-511 within this
        // universe, not a Channel object as used elsewhere in this application), and actually send
        // the data to OLA.
        //
	void setLevel(const unsigned int channel, const unsigned int level);
	void sendData();

    private:
        const ola::OlaCallbackClientWrapper & m_wrapper;
        const unsigned int m_internal_id;
        const unsigned int m_ola_id;
        const std::string m_name_string;
        ola::DmxBuffer m_buffer;
};

//
// Inline functions
//
inline const unsigned int Universe::getInternalId() const
{
    return m_internal_id;
};

inline const unsigned int Universe::getOlaId() const
{
    return m_ola_id;
};

inline const std::string Universe::getNameString() const
{
    return m_name_string;
}

#endif // _UNIVERSE_HPP
