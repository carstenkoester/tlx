#ifndef _HAVE_CUE_NOTIFY_CLIENT_H
#define _HAVE_CUE_NOTIFY_CLIENT_H

class Cue;

class CueNotifyClient
{
    public:
       virtual void notifyCueChanged(const Cue * p_cue) =0;
       virtual void notifyCueDeleted(const Cue * p_cue, const bool unregister) =0;
       virtual void notifyCueDeleted(const Cue * p_cue) { notifyCueDeleted(p_cue, true); };
};

#endif // _HAVE_CUE_NOTIFY_CLIENT
