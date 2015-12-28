#ifndef _HAVE_CUESTACK_H
#define _HAVE_CUESTACK_H

#include <logger.h>
#include <engine/exceptions.h>

#include <list>
#include <exception>

class Cue;
class CueStack;

typedef std::list<Cue>::iterator cue_iterator;
typedef std::list<CueStack>::iterator cuestack_iterator;

class CueAlreadyDefinedException: public std::exception
{
    public:
        virtual const char* what() const throw()
        {
            return "Cue already exists";
        }
};
class CueDoesNotExistException: public std::exception
{
    public:
        virtual const char* what() const throw()
        {
            return "Cue does not exist";
        }
};
class CueStackAlreadyDefinedException: public std::exception
{
    public:
        virtual const char* what() const throw()
        {
            return "Cue Stack already exists";
        }
};
class CueStackDoesNotExistException: public std::exception
{
    public:
        virtual const char* what() const throw()
        {
            return "Cue Stack does not exist";
        }
};


class CueStack
{
    public:
        CueStack(const unsigned int id) : m_id(id), m_loop(false) { LOG_DEBUG("CONSTRUCTOR: CueStack " << m_id); };
        ~CueStack() { LOG_DEBUG("DESTRUCTOR: CueStack " << m_id); };
        inline const unsigned int getId() const { return m_id; };
        inline const bool getLoop() const { return m_loop; };
        inline void setLoop(bool loop) { m_loop = loop; };

        cue_iterator findCue(const unsigned long id);
        bool checkIfCueExists(const unsigned long id);
        void createCue(unsigned long id);
        void renameCue(unsigned long old_id, unsigned long new_id);
        void deleteCue(unsigned long id);
        inline unsigned int getNumCues() { return m_cues.size(); };
        inline cue_iterator cueListBegin() { return m_cues.begin(); };
        inline cue_iterator cueListEnd() { return m_cues.end(); };
        cue_iterator getNextCue(cue_iterator cue);

        /*
         * Notification from our cues
         */
        void notifyCueChanged(const Cue * p_cue) const;

        /*
         * Now this is for managing all cue stacks. A lot of
         * duplication going on here. Should define some sort of
         * wrapper container for this, and use it as
         * Scene repository as well as cue stack and list of cue stacks.
         */
        static cuestack_iterator findCueStack(const unsigned int);
        static bool checkIfCueStackExists(const unsigned int);
        static void createCueStack(unsigned int id);
        static void renameCueStack(unsigned int old_id, unsigned int new_id);
        static void deleteCueStack(unsigned int id);
        static inline unsigned int getNumCueStacks() { return m_cuestacks.size(); };
        static inline cuestack_iterator cueStackListBegin() { return m_cuestacks.begin(); };
        static inline cuestack_iterator cueStackListEnd() { return m_cuestacks.end(); };

    private:
        unsigned int m_id;

        bool m_loop;
        std::list<Cue> m_cues;
        static std::list<CueStack> m_cuestacks;
};

#endif // _HAVE_CUESTACK_H
