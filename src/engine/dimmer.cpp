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

#include "logger.h"
#include "exceptions.h"

#include "engine/dimmer.hpp"

#include "engine/universe.hpp"
#include "engine/channel.hpp"

static Channel* UNPATCHED = NULL;

Dimmer::Dimmer(Universe& universe, const unsigned int dimmer) :
                   m_universe(universe), m_id(dimmer),
                   m_name_string(universe.getNameString() + "." + boost::lexical_cast<std::string>(dimmer+1)),
                   m_p_patched_channel(UNPATCHED),
                   m_level(0)

{
    m_universe.setLevel(m_id, m_level);
}


void Dimmer::patchToChannel(Channel* p_channel)
{
    if (m_p_patched_channel)
    {
        LOG_ERROR("Error patching channel " << p_channel->getNameString() << " to dimmer " << m_name_string << \
                  ". Dimmer already patched.");
        throw AlreadyPatchedException();
    }
    m_p_patched_channel = p_channel;
    setLevel(m_p_patched_channel->getOutputLevel());
}


void Dimmer::unpatchFromChannel(Channel* p_channel)
{
    if (m_p_patched_channel != p_channel)
    {
        LOG_ERROR("Error unpatching channel " << p_channel->getNameString() << " from dimmer " << m_name_string << \
                  ". Dimmer not patched to this channel.");
        throw NotPatchedException();
    }
    m_p_patched_channel = UNPATCHED;
    setLevel(0);
}


void Dimmer::setLevel(const unsigned int level)
{
    // TODO: - Apply Dimmer Curve
    //       - Enforce Min and Max levels

    m_level = level;
    m_universe.setLevel(m_id, m_level);
}
