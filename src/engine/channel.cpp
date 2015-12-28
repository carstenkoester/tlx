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
#include <algorithm>
#include <string>
#include <boost/lexical_cast.hpp>

#include "logger.h"
#include "exceptions.hpp"

#include "engine/channel.hpp"

#include "engine/dimmer.hpp"
#include "engine/master.hpp"


Channel::Channel(Master& grand_master, unsigned int id) :
                     m_id(id), m_grand_master(grand_master),
                     m_default_level(0), m_mergemode(MERGEMODE_HTP),
                     m_nomasters(false), m_nofade(false),
                     m_inputlevel(m_default_level), m_p_override_source(NULL),
                     m_p_active_source(NULL),
                     m_name_string(boost::lexical_cast<std::string>(id+1))
{
}

void Channel::registerSource(const void* p_source, SourceType sourcetype, unsigned int level)
{
    //
    // Register a new input source to this channel.
    //
    // 1.) Check if the source is already registered. That shouldn't happen.
    // 2.) If the new source is the editor, this source becomes the override source.
    //     This will take precedence over all levels, even if it is neither the latest (LTP)
    //     nor the highest (HTP).
    // 3.) Otherwise, if the merge mode is LTP:
    //     - If (another) source is active as override source, then simply discard this update.
    //       The override source retains priority.
    //     - If there is no override source, then apply this update.
    //       Note that we don't really keep track of input sources for LTP mixing. The last update simply wins.
    // 4.) Finally, if the registration is from a normal source in HTP mode, add it to our Level Elements list.
    //     We want to push the Crossfader to the front of the list so that if submasters and crossfader have equal
    //     levels, the crossfader becomes the active source.  
    //
    std::list<SourceLevelPair>::iterator i_inputlevels = m_inputlevels.begin();

    while (i_inputlevels != m_inputlevels.end())
    {
        if (i_inputlevels->p_source == p_source)
        {
            LOG_ERROR("CHANNEL: Error registering source " << p_source << " with channel " << getNameString() << \
                      ". Source already registered");
            throw InputSourceAlreadyRegisteredException();
        } else {
            ++i_inputlevels;
        }
    }

    if (sourcetype == SOURCE_EDITOR)
    {
        //
        // This is the override source.
        //
        m_p_override_source = m_p_active_source = p_source;
        m_inputlevel = level;
        LOG_DEBUG("CHANNEL: Channel " << getNameString() << ": Override source " << p_source << " active.");
    } else if ((m_mergemode == MERGEMODE_LTP) && (m_p_override_source = NULL)) {
        //
        // Merge mode is LTP, and no override source active. Simply process it like an update.
        //
        LOG_DEBUG("CHANNEL: Channel " << getNameString() << ": Registration from LTP source " << p_source);
        m_inputlevel = level;
    } else if (m_mergemode == MERGEMODE_HTP) {
        //
        // Normal source, HTP mode
        //
        SourceLevelPair le;
        le.p_source = p_source;
        le.level = level;

        if (sourcetype == SOURCE_XFADER)
        {
            m_inputlevels.push_front(le);
        } else {
            m_inputlevels.push_back(le);
        }
       
        // Was this the first object we added to the list?
        if (m_inputlevels.size() == 1)
        {
            m_inputlevel = level;
            m_p_active_source = p_source;
        } else {
            // This wasn't the first source. Simply use setLevel to run normal HTP selection mechanism
            setLevel(p_source, level);
        }
    }
}

void Channel::setLevel(const void* p_source, unsigned int level)
{
    // 
    // Process an input level update from a channel:
    // 
    // 1.) If this source is the override source, simply apply the level. No further questions asked.
    // 2.) If no override source is active and our merge mode is LTP, then, again, simply apply the level now.
    //
    // 3.) If none of the above applies, then the update is from a normal channel and our merge mode is HTP.
    //     This is where the Level Elements list comes into play.
    //     In all of these cases, store the level in our elements list. We'll do HTP selection at the same time
    //     (simply because while we're iterating through the list already, it'd be a shame not to look at the level
    //     at the same time), but may or may not utilise that maximum level.
    // 
    // 4.) If no override source is active, then apply the level from step 3.
    //
    if ((p_source == m_p_override_source) && (level != m_inputlevel)) {
        //
        // Source is the override source
        //
#ifdef DEBUG
        LOG_DEBUG("CHANNEL: Override source setting channel " << getNameString() << " level to " << level);
#endif
        m_inputlevel = level;
    } else if ((m_mergemode == MERGEMODE_LTP) && (m_p_override_source = NULL) &&
             (level != m_inputlevel)) {
        //
        // Override is not active and this channel is LTP
        //
#ifdef DEBUG
        LOG_DEBUG("CHANNEL: Normal source setting channel " << getNameString() << " LTP level to " << level);
#endif
        m_inputlevel = level;
    } else {
        //
        // Normal source, merge mode HTP
        //
#ifdef DEBUG
        // We use this just to complain if we're getting an update from an unknown source.
        bool found = false;
#endif

        int loop_max = -1;
        const void* p_loop_max_source = NULL;

        // Iterate through the element list.
        std::list<SourceLevelPair>::iterator i_inputlevels;
        for (i_inputlevels = m_inputlevels.begin(); i_inputlevels != m_inputlevels.end();
             ++i_inputlevels)
        {
            if (i_inputlevels->p_source == p_source)
            {
                // Found the source the update is from.
#ifdef DEBUG
                // LOG_DEBUG("CHANNEL: Channel " << getNameString() << " Source " << i_inputlevels->p_source << " setting level to " << level);
                found = true;
#endif
                i_inputlevels->level = level;
            }

            // Track the maximum level
            if ((int) i_inputlevels->level > loop_max)
            {
                loop_max = i_inputlevels->level;
                p_loop_max_source = i_inputlevels->p_source;
            }
        }
        if ((loop_max > -1) && (m_p_override_source == NULL)) {
            m_p_active_source = p_loop_max_source;
            m_inputlevel = loop_max;
        }
#ifdef DEBUG
        if (!found)
        {
            LOG_ERROR ("CHANNEL: " << getNameString() << " level set from unknown source " << p_source << ", ignoring!");
        }
#endif
    }
}

void Channel::unregisterSource(const void* p_source)
{
    //
    // Unregister a source.
    // 
    // 1.) If the channel mode is HTP and it isn't the override source, remove it from the input
    //     elements list.
    // 2.) If the source was the override source, reset m_override_source to NULL.
    // 3.) If the channel mode is LTP, we're done. If the channel mode is HTP and no override source
    //     is active, execute HTP selection.
    //
    if (p_source == m_p_override_source)
    {
        LOG_DEBUG("CHANNEL: Channel " << getNameString() << " removing override source");
        m_p_override_source = NULL;
        m_p_active_source = NULL;    // This is relevant only for HTP but doesn't harm LTP
    }
    if (m_mergemode == MERGEMODE_HTP) {
        std::list<SourceLevelPair>::iterator i_inputlevels = m_inputlevels.begin();
        signed int loop_max = -1;
        const void* p_loop_max_source = NULL;

        // Iterate through the element list.
        while (i_inputlevels != m_inputlevels.end())
        {
            if (i_inputlevels->p_source == p_source)
            {
                i_inputlevels = m_inputlevels.erase(i_inputlevels);
            } else {
                // Track the maximum level
                if ((int) i_inputlevels->level > loop_max)
                {
                    loop_max = i_inputlevels->level;
                    p_loop_max_source = i_inputlevels->p_source;
                }
                ++i_inputlevels;
            }

        }
        if ((loop_max > -1) && (m_p_override_source == NULL))
        {
            m_p_active_source = p_loop_max_source;
            m_inputlevel = loop_max;
        } else if ((loop_max == -1) && (m_p_override_source == NULL)) {
	    m_p_active_source = NULL;
	    m_inputlevel = m_default_level;
        }
    }
}

void Channel::addDimmer(Dimmer& dimmer)
{
    // Check if we are already patched to the same dimmer. If so,
    // throw exception.
    std::list<Dimmer *>::iterator findIter = std::find(m_p_dimmers.begin(),
        m_p_dimmers.end(), &dimmer);
    if (findIter != m_p_dimmers.end())
    {
        LOG_ERROR("CHANNEL: Error patching channel " << getNameString() << " to dimmer " << dimmer.getNameString() << \
                  ". Channel already patched to same dimmer.");
        throw AlreadyPatchedException();
    }

    m_p_dimmers.push_back(&dimmer);
    dimmer.patchToChannel(this);
}

void Channel::updateDimmers()
{
    std::list<Dimmer*>::iterator i_p_dimmers;

    if (m_nomasters) {
        m_outputlevel = m_inputlevel;
    } else {
        m_outputlevel = m_grand_master.apply(m_inputlevel);
    }

    for (i_p_dimmers = m_p_dimmers.begin(); i_p_dimmers != m_p_dimmers.end(); 
         ++i_p_dimmers)
    {
        (*i_p_dimmers)->setLevel(m_outputlevel);
    }
}

void Channel::removeDimmer(Dimmer& dimmer)
{
    //
    // No real need to perform any checking here. If we weren't patched to this dimmer,
    // the dimmer's unpatch function will complain.
    //
    dimmer.unpatchFromChannel(this);
    m_p_dimmers.remove(&dimmer);
}
