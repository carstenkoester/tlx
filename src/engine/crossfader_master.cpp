#include <engine/crossfader_master.h>

void CrossfaderMaster::setLevelX(const unsigned int level)
{
    /* LOG_DEBUG("XFADER-MASTER: Call to SetLevelX level " << level); */
    m_level_x = level;
    if (m_x_or_xy)
    {
        m_x_or_xy->setLevel_noNotify(getPhyLevelX());
    }
    /*
     * Need to determine how we handle this. If we only have one physical fader attached, 
     * do we want to enfore that X and Y run parallel even if the value is set by the
     * using crossfader application? Or do we want the crossfader application to use
     * X and Y separately, and only apply *input* from the physical master to both?
     */
/*
    if (m_mode == MODE_SINGLE)
    {
        m_level_y = MASTER_FULL_LEVEL - m_level_x;
        if (m_y)
        {
            m_y->setLevel_noNotify(getLevelPhyY());
        }
    }
*/
}

void CrossfaderMaster::setLevelY(const unsigned int level)
{
    m_level_y = level;
    /* LOG_DEBUG("XFADER-MASTER: Call to SetLevelY level " << level);
    LOG_DEBUG("XFADER-MASTER: Real level is " << m_level_y << " PHY level is " << getPhyLevelY() << " Reverse is " << m_reversed); */
    if (m_y)
    {
        // LOG_DEBUG("XFADER_MASTER:  - Sending to physical");
        m_y->setLevel_noNotify(getPhyLevelY());
    }
    /*
     * See comment in setLevelX function
     */
/*
    if (m_mode == MODE_SINGLE)
    {
        m_level_x = MASTER_MAY_VALUE - m_level_y;
        if (m_x_or_xy)
        {
            m_x_or_xy->setLevel_noNotify(getLevelPhyX());
        }
    }
*/
}

/*
 * Connect a single (XY) master
 */
void CrossfaderMaster::connectPhy(NotifyingMaster * xy)
{
    assert(m_x_or_xy == NULL);
    assert(m_y == NULL);

    LOG_DEBUG("XFADER-MASTER: Connecting physical controls in SINGLE mode");
    m_mode = MODE_SINGLE;
    m_x_or_xy = xy;
    // FIXME -- should check if m_x_or_xy has a client already
    m_x_or_xy->setClient(this);
    m_x_or_xy->setLevel_noNotify(getPhyLevelX());
}

void CrossfaderMaster::connectPhy(NotifyingMaster * x, NotifyingMaster * y)
{
    assert(m_x_or_xy == NULL);
    assert(m_y == NULL);

    LOG_DEBUG("XFADER-MASTER: Connecting physical controls in SPLIT mode");
    m_mode = MODE_SPLIT;
    m_x_or_xy = x;
    m_y = y;
    // FIXME -- should check if m_x_or_xy or m_y have clients already
    m_x_or_xy->setClient(this);
    m_y->setClient(this);
    m_x_or_xy->setLevel_noNotify(getPhyLevelX());
    m_y->setLevel_noNotify(getPhyLevelY());
}

void CrossfaderMaster::disconnectPhy()
{
    LOG_DEBUG("XFADER-MASTER: Clearing physical controls");
    if (m_mode == MODE_SPLIT)
    {
        assert(m_x_or_xy != NULL);
        assert(m_y != NULL);
        m_x_or_xy->clearClient(this);
        m_y->clearClient(this);
    } else if (m_mode == MODE_SINGLE) {
        assert(m_x_or_xy != NULL);
        assert(m_y == NULL);
        m_x_or_xy->clearClient(this);
    }
    m_x_or_xy = NULL;
    m_y = NULL;
    m_mode = MODE_SPLIT;
}

void CrossfaderMaster::transferPhy(CrossfaderMaster & source, CrossfaderMaster & destination)
{
    LOG_DEBUG("XFADER-MASTER: Transferring physical controls from " << &source << " to " << &destination);
    assert(destination.m_x_or_xy == NULL);
    assert(destination.m_y == NULL);

    destination.m_mode = source.m_mode;
    destination.m_reversed = source.m_reversed;
    destination.m_x_or_xy = source.m_x_or_xy;
    destination.m_y = source.m_y;
    destination.m_level_x = source.m_level_x;
    LOG_DEBUG("TRANSFER setting level y = " << source.m_level_y);
    destination.m_level_y = source.m_level_y;

    if(source.m_x_or_xy != NULL)
    {
        source.m_x_or_xy->clearClient(&source);
        destination.m_x_or_xy->setClient(&destination);
        source.m_x_or_xy = NULL;
    }
    if (source.m_y != NULL)
    {
        source.m_y->clearClient(&source);
        destination.m_y->setClient(&destination);
        source.m_y = NULL;
    }
}
        
void CrossfaderMaster::notifyMasterChanged(NotifyingMaster & master, const unsigned int old_level, const unsigned int new_level)
{
    assert((&master == m_x_or_xy) || (&master == m_y));

    if (&master == m_x_or_xy)
    {
        assert(old_level == getPhyLevelX());
        // LOG_DEBUG("XFADER-MASTER: Received physical master notification change from X Phy. Phy level = " << new_level);

        m_level_x = (m_reversed ? MASTER_FULL_LEVEL - new_level : new_level);
        if (m_mode == MODE_SINGLE)
        {
            m_level_y = MASTER_FULL_LEVEL - m_level_x;
          //  LOG_DEBUG("XFADER-MASTER: Single mode, setting y to " << m_level_y);
        }
    } else if (&master == m_y) {
      //  LOG_DEBUG("XFADER-MASTER: Received physical master notification change from Y Phy. Phy level = " << new_level << " old level was " << getPhyLevelY() << " real is " << m_level_y << " Reversed is " << m_reversed << " THis is " << this << " mode is " << m_mode);
        assert(old_level == getPhyLevelY());
        m_level_y = (m_reversed ? new_level : MASTER_FULL_LEVEL - new_level);
   //     LOG_DEBUG("XFADER_MASTER: Y set to " << m_level_y);
    }
    if (m_client)
    {
//        LOG_DEBUG("XFADER_MASTER:   - Propagating change to Crosfader or Crossfader-Worker.");
        m_client->notifyMasterChanged(master, old_level, new_level);
    }
}
