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
#include <string>
#include <boost/lexical_cast.hpp>

#include "engine/universe.hpp"

Universe::Universe(const ola::OlaCallbackClientWrapper & wrapper,
                   const unsigned int internal_universe, const unsigned int ola_universe) :
                       m_wrapper(wrapper),
                       m_internal_id(internal_universe),
                       m_ola_id(ola_universe),
                       m_name_string(boost::lexical_cast<std::string>(internal_universe+1))
{
    m_buffer.Blackout();
}

void Universe::setLevel(const unsigned int channel, const unsigned int level)
{
    m_buffer.SetChannel(channel, level);
}

void Universe::sendData()
{
    m_wrapper.GetClient()->SendDmx(m_ola_id, m_buffer);
}
