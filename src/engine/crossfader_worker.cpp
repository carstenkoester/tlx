#include <iostream>

#include "engine/crossfader_worker.h"
#include "engine/crossfader_master.h"
#include "engine/crossfader_channel_type.h"

#include "engine/channel.hpp"

using std::cout;
using std::endl;

CrossfaderWorker::CrossfaderWorker(bool is_main) :
                                       m_is_main(is_main), m_fade_starting(false), m_fade_finishing(false),
                                       m_ticks_delay_in(0), m_ticks_delay_out(0), m_stepsize_in(0),
                                       m_stepsize_out(0), m_ticks_fade_in(0), m_ticks_fade_out(0)
{
    LOG_DEBUG("XFADER-WRK: CONSTRUCTOR");
    m_xy.setClient(this);
}

void CrossfaderWorker::notifyMasterChanged(NotifyingMaster & master, const unsigned int old_level, const unsigned int new_level)
{
    // LOG_DEBUG("XFADER-WRK: Master " << &master << "changed level from " << old_level << " to " << new_level);
    //
    // FIXME -- do we need to have code here that aborts a timed fade and makes it a manual fade?
    //
}

CrossfaderWorker::~CrossfaderWorker()
{
    LOG_DEBUG("XFADER-WRK: DESTRUCTOR");
}

void CrossfaderWorker::timerTickPre()
{
    //
    // Once per timer tick, process (reduce by one) the input and output delay
    // values.
    //
    if (m_ticks_delay_in > 0)
    {
        --m_ticks_delay_in;
    }
    if (m_ticks_delay_out > 0)
    {
        --m_ticks_delay_out;
    }

    //
    // Then, if our delay is zero AND we have a stepsize, adjust X/Y accordingly.
    //
    cout << "DO" << m_ticks_delay_out << " FO" << m_ticks_fade_out << " TFO" << m_total_ticks_fade_out;
    if ((m_ticks_delay_out == 0) && (m_ticks_fade_out > 0))
    {
        --m_ticks_fade_out;
        unsigned int new_x = (Master::MASTER_FULL_LEVEL * m_ticks_fade_out / m_total_ticks_fade_out);
        cout << " NX=" << new_x << " BLAH=" << (m_ticks_fade_out / m_total_ticks_fade_out);
        if (new_x != m_xy.getLevelX())
        {
            cout << "*";
            m_xy.setLevelX(new_x);
        } else {
            cout << "-";
        }
    }

    if ((m_ticks_delay_in == 0) && (m_ticks_fade_in > 0))
    {
        --m_ticks_fade_in;
        unsigned int new_y = Master::MASTER_FULL_LEVEL - (Master::MASTER_FULL_LEVEL * m_ticks_fade_in / m_total_ticks_fade_in);
        if (new_y != m_xy.getLevelY())
        {
            cout << "*";
            m_xy.setLevelY(new_y);
        } else {
            cout << "-";
        }
    }

    cout << " X=" << m_xy.getLevelX() << " Y="  << m_xy.getLevelY() << "::: ";
    if (m_xy.getLevelX() == 0 && m_xy.getLevelY() == Master::MASTER_FULL_LEVEL) {
        /* DEBUG */ cout << " -- FINISHING --";
        //
        // Code to be executed when fade is completed:
        //  - Inform parent crossfader?
        //  - Set channels to "inactive" and remove worker?
        m_fade_finishing = true;
    }
    m_my_num_channels = 0;
}

void CrossfaderWorker::timerTickPerChannel(crossfader_channel_t & channel)
{
    m_my_num_channels++;

    if (channel.p_channel->noFade())
    {
        channel.output_sum = channel.target;
    } else {
        channel.output_x = channel.min + ( channel.delta_min_x * m_xy.getLevelX() / Master::MASTER_FULL_LEVEL);
        channel.output_y = channel.min + ( channel.delta_min_y * m_xy.getLevelY() / Master::MASTER_FULL_LEVEL);
        channel.output_sum = std::max(channel.output_x, channel.output_y);
    }
    // TEMP
    if (channel.num < 6)
    {
        cout << channel.num << ":" << (channel.active ? ( channel.output_sum == channel.output_x ? "[X]" : "[Y]" ) : "[-]" ) << channel.output_sum << " ";
    }
    if (m_fade_finishing)
    {
        channel.worker = NULL;
    }
}

void CrossfaderWorker::timerTickPost()
{
    cout << "  - Ch Worked " << m_my_num_channels;
    /*
     * Check if the fade is finshed, either because we reached completion value
     * or because we haven't got any more channels to work on (they may all have
     * been assigned other workers during subsequent cue changes).
     */
/*
// FIXME -- think about this
    if (m_my_num_channels == 0)
    {
        m_fade_finishing = true;
    }
*/
    if ((m_fade_finishing) || (m_my_num_channels == 0))
    {
        LOG_DEBUG("XFADER-WRK: Finishing...");
    } else if ((m_xy.getLevelX() == Master::MASTER_FULL_LEVEL) && (m_xy.getLevelY() == 0)) {
        LOG_DEBUG("XFADER-WRK: Fade is aborting, returning to original position");
        // - Inform parent crossfader?
        // - Reset "noFade" channels?
    }
}

void CrossfaderWorker::setX(const unsigned int x)
{
    LOG_INFO("XFADER-WRK: Setting X to " << x);
    m_xy.setLevelX(x);
}
void CrossfaderWorker::setY(const unsigned int y)
{
    LOG_INFO("XFADER-WRK: Setting Y to " << y);
    m_xy.setLevelY(y);
}

