#include <engine/cuestack.h>
#include <engine/cue.h>

#include <list>

std::list<CueStack> CueStack::m_cuestacks;

bool CueStack::checkIfCueExists(const unsigned long id)
{
    cue_iterator i_cues;
    for (i_cues = m_cues.begin(); i_cues != m_cues.end(); i_cues++)
    {
        if (i_cues->getId() == id)
        {
            return true;
        }
    }
    return false;
}

cue_iterator CueStack::findCue(const unsigned long id)
{
    cue_iterator i_cues;
    for (i_cues = m_cues.begin(); i_cues != m_cues.end(); i_cues++)
    {
        if (i_cues->getId() == id)
        {
            return i_cues;
        }
    }
    throw CueDoesNotExistException();
}

bool compareCues(Cue &s1, Cue &s2)
{
    return (s1.getId() < s2.getId());
}

void CueStack::createCue(unsigned long id)
{
    if (checkIfCueExists(id))
    {
        throw CueAlreadyDefinedException();
    }
    m_cues.emplace_back(id, *this);
    m_cues.sort(compareCues);
    // TODO: Notify crossfader here -- could be that we inserted a new cue right after the currently active cue
}

void CueStack::renameCue(unsigned long old_id, unsigned long new_id)
{
    if (checkIfCueExists(new_id))
    {
        throw CueAlreadyDefinedException();
    }
    findCue(old_id)->m_id = new_id;
    m_cues.sort(compareCues);
    // TODO: Notify crossfader here -- could be that we inserted a new cue right after the currently active cue
}

void CueStack::deleteCue(unsigned long id)
{
    if (!checkIfCueExists(id))
    {
        throw CueDoesNotExistException();
    }
    m_cues.erase(findCue(id));
    // TODO: Notify crossfader here
}

cue_iterator CueStack::getNextCue(cue_iterator cue)
{
    if (cue->m_has_jumpto_cue) {
       if (cue->m_jumpto_count_current > 0) {
           return cue->m_i_jumpto_cue;
       } else {
           // FIXME -- should we do this here, or completely separate "reading" the next cue versus actually "executing" the next
           // cue (maintaining counters)?
           //
           // And should we maintain the jumpto counts completely outside of the cue, within the cross fader? What happens if
           // multiple cross faders run through the same cue stack??
           cue->m_jumpto_count_current = cue->m_jumpto_count_configured;
       }
    }
    return ++cue;
}

/*
 * Cue Stack
 */
bool CueStack::checkIfCueStackExists(const unsigned int id)
{
    cuestack_iterator i_cuestacks;
    for (i_cuestacks = m_cuestacks.begin(); i_cuestacks != m_cuestacks.end(); i_cuestacks++)
    {
        if (i_cuestacks->getId() == id)
        {
            return true;
        }
    }
    return false;
}

cuestack_iterator CueStack::findCueStack(const unsigned int id)
{
    cuestack_iterator i_cuestacks;
    for (i_cuestacks = m_cuestacks.begin(); i_cuestacks != m_cuestacks.end(); i_cuestacks++)
    {
        if (i_cuestacks->getId() == id)
        {
            return i_cuestacks;
        }
    }
    throw CueStackDoesNotExistException();
}

bool compareCueStacks(CueStack &s1, CueStack &s2)
{
    return (s1.getId() < s2.getId());
}

void CueStack::createCueStack(unsigned int id)
{
    if (checkIfCueStackExists(id))
    {
        throw CueStackAlreadyDefinedException();
    }
    m_cuestacks.emplace_back(id);
    m_cuestacks.sort(compareCueStacks);
}

void CueStack::renameCueStack(unsigned int old_id, unsigned int new_id)
{
    if (checkIfCueStackExists(new_id))
    {
        throw CueStackAlreadyDefinedException();
    }
    findCueStack(old_id)->m_id = new_id;
    m_cuestacks.sort(compareCueStacks);
}

void CueStack::deleteCueStack(unsigned int id)
{
    if (!checkIfCueStackExists(id))
    {
        throw CueStackDoesNotExistException();
    }
    m_cuestacks.erase(findCueStack(id));
    // TODO: Notify crossfader here
}

void CueStack::notifyCueChanged(const Cue * p_cue) const
{
    LOG_INFO("Cue changed notification from cue " << p_cue->getId());
    // TODO: Notify crossfader here
}
