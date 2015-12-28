#include <boost/bind.hpp>
#include <iostream>
#include <algorithm>
#include <boost/bind.hpp>

#include <engine/crossfader.h>

#include <engine/master.hpp>
#include <engine/engine.h>
#include <engine/channel.hpp>

using std::cout;
using std::endl;

using namespace boost;

#define FOR_ALL_CHANNELS(x) for (int x = 0; x < m_num_channels; ++x)

/*
 * Define all the static elements
 */
NotifyingMaster * Crossfader::x1;
NotifyingMaster * Crossfader::y1;
Master * Crossfader::speed;
Master * Crossfader::master;
Crossfader * Crossfader::main_xy;

Scene* Crossfader::p_empty_scene = new Scene(0);

/*
 * Per-Instance methods
 */
Crossfader::Crossfader(Engine& engine) :
    m_has_cuestack(false), m_has_cue_x(false), m_has_cue_y(false),
    m_has_scene_x(false), m_has_scene_y(false),
    m_levelmaster(master), m_channels(engine.getNumChannels()),
    m_num_channels(engine.getNumChannels())
{
    m_xy.setClient(this);
    FOR_ALL_CHANNELS(i)
    {
        m_channels[i].num = i;
        m_channels[i].p_channel = &(engine.getChannel(i));
    }

    connection_cue = Cue::connect(boost::bind(&Crossfader::cueHasChanged, this, _1, _2));
    m_scene_connection = Scene::connect(boost::bind(&Crossfader::sceneHasChanged, this, _1, _2));
};

Crossfader::~Crossfader()
{
    LOG_DEBUG("XFADER: Destructor");
    Scene::disconnect(m_scene_connection);
    Cue::disconnect(connection_cue);
}

void Crossfader::cueHasChanged(Cue::CueSignalType type, Cue* cue)
{
//FIXME - Check cue stack ID, stop running fades, etc etc
    if (type == Cue::CUE_DELETED)
    {
        LOG_DEBUG("CUE HAS BEEN DELETED Cue " << cue);
        if (m_has_cue_x)
        {
            if (cue == &(*m_cue_x))
            {
                LOG_INFO("XFADER: Cue X deleted.");
                m_has_cue_x = false;
                m_has_scene_x = false;
            }
        }
        if (m_has_cue_y)
        {
            if (cue == &(*m_cue_y))
            {
                LOG_INFO("XFADER: Cue Y deleted.");
                m_has_cue_x = false;
                m_has_scene_x = false;
            }
        }
    } else {
        LOG_DEBUG("CUE HAS CHANGED Cue " << cue);
    }

}

void Crossfader::loadCueStack(cuestack_iterator cuestack)
{
    LOG_DEBUG("XFADER: Crossfader assigning cuestack...");

    FOR_ALL_CHANNELS(i)
    {
        m_channels[i].p_channel->registerSource(this, Channel::SOURCE_XFADER, 0);
        m_channels[i].active = false;
        m_channels[i].output_sum = 0;
        m_channels[i].worker = NULL;
    }

    // FIXME -- need to "unregister" from previous cues or so?
    m_has_cuestack = true;
    m_cuestack = cuestack;

    m_has_cue_x = false;
    m_has_cue_y = false;
}

void Crossfader::loadCueX(cue_iterator cue, bool initial_load)
{
    // FIXME -- Do we need some mechanism here to verify the cue is from the correct cue stack?
    LOG_DEBUG("XFADER: Crossfader loading cue " << cue->getId() << " to X");

    m_cue_x = cue;
    m_has_cue_x = true;

    // FIXME - This will decrement jumpto count. We should decrement when we start the fade (or when we finish it?),
    //         not when we load the cue.
    m_cue_y = m_cuestack->getNextCue(m_cue_x);  
    m_has_cue_y = !(m_cue_y == m_cuestack->cueListEnd());

    if (!m_has_cue_y)
    {
        LOG_INFO("XFADER: Last cue reached");
        if (m_cuestack->getLoop())
        {
            LOG_INFO("XFADER: - Looping");
            m_cue_y = m_cuestack->cueListBegin();
            m_has_cue_y = true;
        } else {
            m_has_cue_y = false;
        }
    }

    //
    // Now see if we have scenes
    //
    if (m_cue_x->hasScene())
    {
        m_p_scene_x = m_cue_x->getScene();
        m_has_scene_x = true;
    } else {
        m_p_scene_x = p_empty_scene;
        m_has_scene_x = false;
    }
    if (m_has_cue_y)
    {
        if (m_cue_y->hasScene())
        {
            m_p_scene_y = m_cue_y->getScene();
            m_has_scene_y = true;
        } else {
            m_p_scene_y = p_empty_scene;
            m_has_scene_y = false;
        }
    } else {
        m_p_scene_y = p_empty_scene;
        m_has_scene_y = false;
    }
    
    /*
     * If this is an initial load (triggered by an external "load" command, as opposed to
     * tracking through the cue stack), do some extra work:
     *
     * Reposition the X/Y controls
     * Stop all ongoing fades (FIXME)
     * Reset all jump counts in the cue stack (FIXME)
     */
    LOG_INFO("XFADER: Initial load? " << initial_load);
    if (initial_load)
    {
        /*
         * For an initial load, stop all active fades and assign the X cue to the output.
         */
        FOR_ALL_CHANNELS(i)
        {
            m_channels[i].active = false;
            m_channels[i].output_sum = m_p_scene_x->getChannelLevelOrDefault(i, 0);
            m_channels[i].worker = NULL;
        }

        for (std::list<crossfader_worker_ptr>::iterator it = m_workers.begin(); it != m_workers.end(); ++it)
        {
            if ((*it)->isMain())
            {
                LOG_DEBUG("XFADER: MAIN worker ABORTING, claiming back controls");
                CrossfaderMaster::transferPhy((*it)->m_xy, m_xy);
                m_fade_in_progress = false;
            } else {
                LOG_DEBUG("XFADER: Worker other than main worker aborting");
            }
            it = m_workers.erase(it);
        }
        m_xy.setLevelX(Master::MASTER_FULL_LEVEL);
        m_xy.setLevelY(0);
    }

    LOG_INFO("XFADER: LoadCueX");
    LOG_INFO("XFADER: - Cue 1 has cue? " << m_has_cue_x << " -- cue number << " << m_cue_x->getId());
    LOG_INFO("XFADER: - Cue 2 has cue? " << m_has_cue_y << " -- cue number << " << m_cue_y->getId());

    /*
     * Calculate the values we're fading to. For an initial load with no fade in progress this wouldn't
     * strictly be necessary, but comes in useful if we have a non-initial load or are re-loading while a
     * fade is in progress.
     */
    updateScenes();
}

void Crossfader::loadCueY(cue_iterator cue)
{
    m_has_cue_y = true;
    m_cue_y = cue;

    if (m_cue_y->hasScene())
    {
        m_p_scene_y = m_cue_y->getScene();
        m_has_scene_y = true;
    } else {
        m_p_scene_y = p_empty_scene;
        m_has_scene_y = false;
    }

    LOG_INFO("XFADER: LoadCueY");
    LOG_INFO("XFADER: - Cue 1 has cue? " << m_has_cue_x << " -- cue number << " << m_cue_x->getId());
    LOG_INFO("XFADER: - Cue 2 has cue? " << m_has_cue_y << " -- cue number << " << m_cue_y->getId());

    /*
     * Calculate the values we're fading to. For an initial load with no fade in progress this wouldn't
     * strictly be necessary, but comes in useful if we have a non-initial load or are re-loading while a
     * fade is in progress.
     */
    updateScenes();
}

void Crossfader::startFade(bool manual)
{
    if (!m_has_cue_y)
    {
        LOG_INFO("XFADER: About to start fade, but nothing to fade to. Ignoring.");
        return;
    }

    LOG_DEBUG("FADER xy address is " << &m_xy);
    crossfader_worker_ptr cw(new CrossfaderWorker(true));
    CrossfaderMaster::transferPhy(m_xy, cw->m_xy);
    m_workers.push_back(cw);
    if (!manual)
    {
        if (!m_has_cue_y)
        {
            LOG_INFO("XFADER: Wanting to start automatic fade but nothing to fade to");
        }
        cw->m_ticks_delay_in = m_cue_y->getDelayIn() * 1000 / Engine::timerInterval;
        cw->m_ticks_delay_out = m_cue_y->getDelayOut() * 1000 / Engine::timerInterval;
        cw->m_ticks_fade_in = cw->m_total_ticks_fade_in = m_cue_y->getTimeIn() * 1000 / Engine::timerInterval;
        cw->m_ticks_fade_out = cw->m_total_ticks_fade_out = m_cue_y->getTimeOut() * 1000 / Engine::timerInterval;
        if (cw->m_total_ticks_fade_in == 0)
        {
            cw->m_ticks_fade_in = cw->m_total_ticks_fade_in = 1;
        }
        if (cw->m_total_ticks_fade_out == 0)
        {
            cw->m_ticks_fade_out = cw->m_total_ticks_fade_out = 1;
        }

        cw->m_stepsize_in = m_cue_y->getTimeIn() * 1000 / Engine::timerInterval / Master::MASTER_FULL_LEVEL;
        cw->m_stepsize_out = m_cue_y->getTimeOut() * 1000 / Engine::timerInterval / Master::MASTER_FULL_LEVEL;
        LOG_DEBUG("XFADER: Setting worker's ticksDIn = " << cw->m_ticks_delay_in << " ticksDOut = " << cw->m_ticks_delay_out << " stepIn " << cw->m_stepsize_in << " stepOut " << cw->m_stepsize_out << " ticksFIn = " << cw->m_ticks_fade_in << " ticksFOut = " << cw->m_ticks_fade_out);
    }
    FOR_ALL_CHANNELS(i)
    {
        m_channels[i].start = m_channels[i].output_sum;  // This line may have to move if there is a fade already in progress
        m_channels[i].active = (m_p_scene_y->channelIsUsed(i));
        if (m_channels[i].active) {
            //
            // FIXME -- If this is NOT a manual fade but automatic fade, deal with fade times, step sizes, and time groups
            //
            m_channels[i].target = m_p_scene_y->getChannelLevelOrDefault(m_channels[i].num, 0);
            m_channels[i].worker = cw;
            m_channels[i].min = std::min(m_channels[i].start, m_channels[i].target);
            m_channels[i].delta_min_x = m_channels[i].start - m_channels[i].min;
            m_channels[i].delta_min_y = m_channels[i].target - m_channels[i].min;
        }
    }
    m_fade_in_progress = true;
}

void Crossfader::updateScenes()
{
    //
    // One of our scenes has changed. Re-calculate the values.
    //

    // If there is no fade in progress, simply re-apply X
    if (!m_fade_in_progress)
    {
        LOG_DEBUG("XFADER: Updating scenes, no fade in progress. Simply replacing start value.");

        // FIXME -- this also will get more complicated once we support multiple concurrent fades.
        FOR_ALL_CHANNELS(i)
        {
            m_channels[i].output_sum = m_p_scene_x->getChannelLevelOrDefault(i, 0);
        }
    } else {
        LOG_DEBUG("XFADER: Updating scenes, fade IS IN PROGRESS. Recalculating target values.");
        //
        // FIXME -- This assumes that there is only a single worker, which is the main worker.
        //          That is true AS LONG AS THERE IS ONLY ONE SIMULTANEOUS FADE. Should this
        //          evolve further into different workers fading different channels at different speeds, we'll need
        //          to think more about wich worker controls which channel and how we need to re-assign this.
        //          Probably in that case we'll need some sort of association between the worker and the scene they're
        //          working towards.
        crossfader_worker_ptr cw = NULL;
    
        std::list<crossfader_worker_ptr>::const_iterator it = m_workers.begin();
        if (it != m_workers.end())
        {
            cw = (*it);
        } else {
            LOG_INFO("updateScenes, has no workers. Maybe because there is no fade in progress.");
        }
    
        FOR_ALL_CHANNELS(i)
        {
            m_channels[i].active = (m_p_scene_y->channelIsUsed(i));
            if (m_channels[i].active) {
                //
                // FIXME -- If this is NOT a manual fade but automatic fade, deal with fade times, step sizes, and time groups
                //

                m_channels[i].target = m_p_scene_y->getChannelLevelOrDefault(m_channels[i].num, 0);
                m_channels[i].worker = cw;
                m_channels[i].min = std::min(m_channels[i].start, m_channels[i].target);
                m_channels[i].delta_min_x = m_channels[i].start - m_channels[i].min;
                m_channels[i].delta_min_y = m_channels[i].target - m_channels[i].min;
            } else {
                m_channels[i].worker = NULL;
            }
        }
    }
}

void Crossfader::go()
{
    LOG_INFO("XFADER: Go");
    startFade(false);
}

void Crossfader::notifyMasterChanged(NotifyingMaster & master, const unsigned int old_level, const unsigned int new_level)
{
    // LOG_DEBUG("XFADER: Master " << &master << "changed level from " << old_level << " to " << new_level);
    // LOG_DEBUG(" - LevelX " << m_xy.getLevelX() << " LevelY " << m_xy.getLevelY());

    if (!m_fade_in_progress &&
           ((m_xy.getLevelX() < Master::MASTER_FULL_LEVEL) || (m_xy.getLevelY() > 0)))
    {
        /*
         * We are MANUALLY starting a fade.
         */
        /* DEBUG */ cout << " -- STARTING --";
        // Code to be executed when starting fade here...
        //  - decrement jump-to counters?
        //  - instantly set "noFade" channels?
        //  - Inform parent Crossfader?
        //  - Potentially start timer for next fade?
        // ...
        startFade(true);
    }
}

void Crossfader::timerTick()
{
    if (!m_has_cuestack)
    {
        return;
    }

    //
    // If a fade is NOT in progress and we get faders in opposite "full" value (as they would be at the end
    // of a fade), reverse them and use this as a new starting position.
    //
    if (!m_fade_in_progress &&
           ((m_xy.getLevelX() == 0) && (m_xy.getLevelY() == Master::MASTER_FULL_LEVEL)))
    {
        m_xy.toggleReversed();
    }

    /*
     * Call the pre function of all workers
     */
    for (std::list<crossfader_worker_ptr>::const_iterator it = m_workers.begin(); it != m_workers.end(); ++it)
    {
        (*it)->timerTickPre();
    }

    /*
     * Call the worker for each channel
     */
    int y = 0; //DEBUG
    int z = 0; //DEBUG
    FOR_ALL_CHANNELS(i)
    {
        if (m_channels[i].worker)
        {
            ++y; //DEBUG
            m_channels[i].worker->timerTickPerChannel(m_channels[i]);
        }
        if (m_channels[i].active) z++; // DEBUG
        if (m_channels[i].p_channel->noMasters())
        {
            m_channels[i].p_channel->setLevel(this, m_channels[i].output_sum);
        } else {
            m_channels[i].p_channel->setLevel(this, m_channels[i].output_sum * Master::MASTER_FULL_LEVEL / m_levelmaster->getLevel());
        }
    }

    /*
     * Call the post function of all workers
     */
    for (std::list<crossfader_worker_ptr>::iterator it = m_workers.begin(); it != m_workers.end(); ++it)
    {
        (*it)->timerTickPost();
        cout << " - Ch w/ worker: " << y << " Ch active " << z << endl;
        /*
         * If the worker was finishing:
         *
         * - Destroy it
         * - If the worker was the main worker:
         *   - reverse the physical controls;
         *   - load the next cue
         */
        if ((*it)->isFinishing())
        {
            if ((*it)->isMain())
            {
                LOG_DEBUG("XFADER: MAIN worker finishing, claiming back controls");
                CrossfaderMaster::transferPhy((*it)->m_xy, m_xy);
                m_xy.toggleReversed();
                m_fade_in_progress = false;
                loadCueX(m_cue_y, false);
            } else {
                LOG_DEBUG("XFADER: Worker other than main worker has finished. To be implemented.");
            }
            // In any case, remove the worker from the list. This will destroy the worker as it was allocated by shared_ptr.
            it = m_workers.erase(it);
        }
    }
}

void Crossfader::sceneHasChanged(Scene::SceneSignalType type, Scene* p_scene)
{
    if (type == Scene::SCENE_CHANGED)
    {
        if (((m_has_scene_x) && (p_scene == m_p_scene_x)) ||
            ((m_has_scene_y) && (p_scene == m_p_scene_y)))
        {
            updateScenes();
        }
    } else { // type == SCENE_DELETED
        if ((m_has_scene_x) && (p_scene == m_p_scene_x)) {
            m_p_scene_x = p_empty_scene;
            m_has_scene_x = false;
            updateScenes();
        } else if ((m_has_scene_y) && (p_scene == m_p_scene_y)) {
            m_p_scene_y = p_empty_scene;
            m_has_scene_y = false;
            updateScenes();
        }
    }
}

// TODO
// Handlers for cue changes / cue deletion.
// Deleting the X cue shall simply clear the fader.
// Deleting the Y cue shall re-load the fader with the current X cue and let the magic work it's way to see if there is a new
//   Y cue. (Or do we really want to get into seeing what the story is if a cue is reloaded mid-fade?)
// Updating a cue shall do nothing really...
// Inserting a cue right after our X cue shall make that new cue the new Y cue
// 

void Crossfader::createMainXY(Engine& engine)
{
    x1 = new NotifyingMaster(Master::MASTER_FULL_LEVEL);
    y1 = new NotifyingMaster(Master::MASTER_FULL_LEVEL);
    speed = new Master(Master::MASTER_FULL_LEVEL / 2);
    master = new Master(Master::MASTER_FULL_LEVEL);
    main_xy = new Crossfader(engine);
    main_xy->m_xy.connectPhy(x1, y1);
}
