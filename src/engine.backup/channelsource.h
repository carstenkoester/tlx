#ifndef _HAVE_CHANNELSOURCE_H
#define _HAVE_CHANNELSOURCE_H

enum sourcetype_t
{
    SOURCE_EDITOR,         // The editor. This will be the override source.
    SOURCE_XFADER,         // Our main "Active->Preset" crossfader
    SOURCE_SUBMASTER       // Submasters.
};


class ChannelSource
{
    public:
       ChannelSource(const sourcetype_t sourcetype_param) : sourcetype(sourcetype_param) {};
       const sourcetype_t sourcetype;
};

#endif // _HAVE_CHANNELSOURCE_H
