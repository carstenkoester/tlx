#ifndef _HAVE_NOTIFYING_MASTER_H
#define _HAVE_NOTIFYING_MASTER_H

#include <logger.h>
#include <engine/master.hpp>
#include <assert.h>

class NotifyingMaster;

class MasterNotifyClient
{
    public:
       virtual void notifyMasterChanged(NotifyingMaster & master, const unsigned int old_level, const unsigned int new_level) =0;
};

class NotifyingMaster : public Master
{
    public:
        NotifyingMaster(unsigned int level) : Master(level), m_client(NULL) {};

        void setClient(MasterNotifyClient * client) { assert(m_client == NULL); m_client = client; LOG_DEBUG("NOTIFYING-MASTER: Setting client"); };
        void clearClient(MasterNotifyClient * client) { assert(m_client == client); m_client = NULL; LOG_DEBUG("NOTIFYING-MASTER: Clearing client"); };

        virtual inline void setLevel (const unsigned int level)
            { unsigned int old_level = m_level; m_level = level;
              // LOG_DEBUG("NOTIFYING-MASTER: Got level change, old " << old_level << " new " << m_level);
              if (m_client) { /* LOG_DEBUG("NOTIFYING-MASTER:  - Notifying client"); */ m_client->notifyMasterChanged(*this, old_level, m_level); }};
        inline void setLevel_noNotify (const unsigned int level) { /* LOG_DEBUG("NOTIFYING-MASTER: Got level change, NO NOTIFY, to " << level); */ m_level = level; };

    protected:
        MasterNotifyClient * m_client;
};

#endif // _HAVE_NOTIFYING_MASTER_H
