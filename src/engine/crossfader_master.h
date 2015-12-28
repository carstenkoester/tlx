#ifndef _HAVE_CROSSFADER_MASTER_H
#define _HAVE_CROSSFADER_MASTER_H

#include <logger.h>
#include <engine/notifying_master.h>
#include <assert.h>

/*
 * MAJOR FIXME  -- This class needs to be made a lot more robust. When connecting and disconnecting,
 * need to check if we already are connected to something, or if the physical master is connected already.
 *
 * Also, NotifyingMaster should be made more robust.
 *
 * Last not least, we really need to figure out how we want to handle dependencies between number of physical controls
 * and crossfader mode. Should a single control imply that X and Y always must move in parallel? Can we wire two
 * controls (X and Y) to a single CrossfaderMaster in SINGLE mode?
 */
enum crossfader_master_mode_t
{
    MODE_SPLIT,
    MODE_SINGLE
};

class CrossfaderMaster : public NotifyingMaster, public MasterNotifyClient
{
    public:
        CrossfaderMaster() : NotifyingMaster(MASTER_FULL_LEVEL), m_mode(MODE_SPLIT), m_x_or_xy(NULL), m_y(NULL),
                                 m_reversed(false), m_level_x(MASTER_FULL_LEVEL), m_level_y(0) { LOG_DEBUG("XFADER-MASTER: CONSTRUCTOR this object " << this); };
        ~CrossfaderMaster() { LOG_DEBUG("XFADER-MASTER: DESTRUCTOR"); };

        inline const unsigned int getPhyLevelX() const
            { return (m_reversed ? MASTER_FULL_LEVEL - m_level_x : m_level_x); };
        inline const unsigned int getPhyLevelY() const
            { return (m_reversed ? m_level_y : MASTER_FULL_LEVEL - m_level_y); };
        inline const unsigned int getLevelX() const { return m_level_x; };
        inline const unsigned int getLevelY() const { return m_level_y; };
        inline const bool isReversed() const { return m_reversed; };
        inline void toggleReversed() { LOG_DEBUG("XFADER-MASTER: Toggling Reversed"); m_reversed = !m_reversed; m_level_x = MASTER_FULL_LEVEL-m_level_x ;
                                       m_level_y = MASTER_FULL_LEVEL - m_level_y; };

        void setLevelX(const unsigned int level);
        void setLevelY(const unsigned int level);

        void connectPhy(NotifyingMaster * xy);
        void connectPhy(NotifyingMaster * x, NotifyingMaster * y);
        void disconnectPhy();

        const NotifyingMaster * getPhyX() const { return m_x_or_xy; };
        const NotifyingMaster * getPhyY() const { return m_y; };

        void notifyMasterChanged(NotifyingMaster & master, const unsigned int old_level, const unsigned int new_level);

        static void transferPhy(CrossfaderMaster & source, CrossfaderMaster & destination);
    protected:
        crossfader_master_mode_t m_mode;
        bool m_reversed;
        
        NotifyingMaster * m_x_or_xy;    // X, or used as single XY if we are in SINGLE mode
        NotifyingMaster * m_y;

        unsigned int m_level_x;
        unsigned int m_level_y;
};

#endif // _HAVE_CROSSFADER_MASTER_H
