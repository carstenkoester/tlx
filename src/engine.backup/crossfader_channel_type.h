#include <engine/crossfader_worker.h>

/*
 * This struct is used to store information about a single channel which is part of a crossfade.
 *
 * There is one such data structure per Channel per active Crossfader instance.
 *
 * Note that within this structure, X and Y refer to the value of a given channel within the scene associated with the fader.
 */

#include "engine/channel.h"

struct crossfader_channel_t
{
    unsigned int num;                      // This channel's number
    Channel * p_channel;                   // Pointer to the channel. Storing this here will ever so slightly speed up
                                           // accessing channel properties such as noFade or noMasters.      
    bool active;                           // Is this channel active in a fade? If not, other values are irrelevant/undefined.
    crossfader_worker_ptr worker;          // Pointer to the worker which is responsible for this channel
    unsigned int start;                    // Value the channel had when we started the fade.
    unsigned int target;                   // Value after the fade
    unsigned int min;                      // Minimum of (start, target)
    unsigned int delta_min_x;              // Difference between min and X   - Used for dipless mode 1
    unsigned int delta_min_y;              // Difference between min and Y   - Used for dipless mode 1 and 2

    bool fade_in_progress;                 // Is this channel currently fading?
    bool fade_waiting_or_in_progress;      // Waiting to fade, or in progress? (False means fade complete).

    unsigned int output_x;                 // Current output level of X, Y respectively
    unsigned int output_y;
    unsigned int output_dipfill;           // Current level of (min(x,y) * (1-delta(x,y)). Used in dipless mode 2.  // FIXME - to be coded

    unsigned int output_sum;               // The merged (i.e. max(current_x,current_y)) output.
};
