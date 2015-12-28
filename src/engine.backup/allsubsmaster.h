#ifndef _HAVE_ALLSUBSMASTER_H
#define _HAVE_ALLSUBSMASTER_H

#include <engine/master.h>

class AllsubsMaster : public Master
{
    public:
        AllsubsMaster(unsigned int level) : Master (level) {};

        void setLevel (const unsigned int level);
};

#endif // _HAVE_ALLSUBSMASTER_H
