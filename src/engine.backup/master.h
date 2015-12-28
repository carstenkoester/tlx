#ifndef _HAVE_MASTER_H
#define _HAVE_MASTER_H

#define MASTER_MAX_VALUE 255

#include <logger.h>

class Master
{
    public:
        Master(unsigned int level) : m_level(level) {};

        inline const unsigned int getLevel() const { return m_level; };
        inline virtual void setLevel (const unsigned int level) { m_level = level; };

    protected:
        unsigned int m_level;
};

#endif // _HAVE_MASTER_H
