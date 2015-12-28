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
#ifndef _CHANNEL_HPP
#define _CHANNEL_HPP

#include <list>
#include <vector>
#include <string>

#include "exceptions.h"

class Dimmer;
class Master;

//
// Class that represents a single channel.
//
// A channel performs input merging, processing (e.g. applying the grand master), and output to Dimmers.
//
// A channel has zero, one or more input sources (such as submasters, an editor, or a crossfader). Prior to
// sending us an input level for the first time, a channel source must "register" for this channel. Once
// registered, the input source can set the channel level. The channel can be configured for HTP or LTP merging;
// in LTP mode, always the latest update is processed; in HTP mode the Channel will keep track of all sources
// and their level, and output the highest source at any moment in time.
//
// A single source (typically the editor) will act as an override source. If this source registers for a channel
// and supplies an input level, the override source will become - and remain - the active source even if it not
// the highest value (in HTP) or if other sources subsequently send newer updates.
//
// Channels have configurable parameters, some of which are stored with the channel but actually evaluated/
// processed in other classes. Configuration parameters include:
// - the merge mode (HTP or LTP),
// - whether or not Masters (Grand Master, All Subs master, ...) should be applied to this channel,
// - whether this channel can be faded, or should always jump to the target value.
//
// TODO - Features that are not yet implemented and may be of interest are:
// - A flash function
// - A solo flash function
//
class Channel
{
    friend class EngineTest;   // FIXME -- is there a better way?

    public:
        //
        // Enum definition for Merge Mode (HTP vs LTP)
        //
        enum MergeMode
        {
            MERGEMODE_HTP,
            MERGEMODE_LTP
        };

        //
        // Enum definition for the different types a source can register as
        //
        enum SourceType
        {
            SOURCE_EDITOR,         // The editor. This will be the override source.
            SOURCE_XFADER,         // Our main "Active->Preset" crossfader
            SOURCE_SUBMASTER       // Submasters.
        };

        EXCEPTION(AlreadyPatchedException, "Attempting to patch Channel to a dimmer it is already patched to");
        EXCEPTION(InputSourceAlreadyRegisteredException, "Same input source attempting to register twice");

        //
        // Constructor - create a new channel.
        //
        Channel(Master& grand_master, const unsigned int id);

        //
        // Accessor functions for reading the identity of this channel, reading the level,
        // and reading/setting configuration parameters.
        //
        const unsigned int getId() const;
        const std::string getNameString() const;
        const unsigned int getInputLevel() const;
        const unsigned int getOutputLevel() const;

        // FIXME - Add remaining accessor functions here to get and set other configurables
        const bool noMasters() const;
        const bool noFade() const;

        //
        // Input/sources - Registering and de-registering sources, setting levels
        //
        void registerSource(const void* p_source, SourceType sourcetype, const unsigned int level);
        void setLevel(const void* p_source, const unsigned int level);
        void unregisterSource(const void* p_source);

        //
        // Output - patching to dimmers.
        //
        void addDimmer(Dimmer& dimmer);
        void updateDimmers();
        void removeDimmer(Dimmer& dimmer);

    private:
        //
        // The channel's ID and configuration parameters.
        //
        const unsigned int m_id;
        const std::string m_name_string;

        const Master& m_grand_master;

        unsigned int m_default_level;
        MergeMode m_mergemode;    // HTP or LTP
        bool m_nomasters;           // Bypass masters
        bool m_nofade;              // Do not fade. Always jump to target level.

        //
        // Input related variables
        //
        // The following struct is used to store (input source, level) pairs
        //
        struct SourceLevelPair
        {
            const void* p_source;
            unsigned int level;
        };
        std::list<SourceLevelPair> m_inputlevels;
        const void* m_p_override_source;
        const void* m_p_active_source;
        unsigned int m_inputlevel;  // Current input level. "input level" means level received from
                                    // input source (crossfader or submaster), before applying
                                    // masters.

        //
        // Output related
        //
        unsigned int m_outputlevel; // Output level. This is after applying grand master.
        std::list<Dimmer *> m_p_dimmers;
};

//
// Inline functions
//
inline const unsigned int Channel::getId() const
{
    return m_id;
}

inline const std::string Channel::getNameString() const
{
    return m_name_string;
}

inline const unsigned int Channel::getInputLevel() const
{
    return m_inputlevel;
};

inline const unsigned int Channel::getOutputLevel() const
{
    return m_outputlevel;
};

inline const bool Channel::noMasters() const
{
    return m_nomasters;
};

inline const bool Channel::noFade() const
{
    return m_nofade;
};

#endif // _CHANNEL_HPP
