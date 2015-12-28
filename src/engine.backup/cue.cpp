#include <engine/cue.h>

float Cue::m_default_time_in = 5.0;
float Cue::m_default_time_out = 5.0;
float Cue::m_default_delay_in = 0.0;
float Cue::m_default_delay_out = 0.0;
float Cue::m_default_dwell_time = CUE_DWELL_TIME_INFINITE;
unsigned  Cue::m_default_jumpto_count = 1;

Cue::cueSignal Cue::cue_signal;

void Cue::notifySceneDeleted(Scene* p_scene, const bool unregister)
{
    LOG_DEBUG("Cue " << m_id << " received delete notification from scene");
    m_has_scene = false;
    m_cuestack.notifyCueChanged(this);
};


void Cue::notifySceneChanged(Scene* p_scene)
{
    LOG_DEBUG("Cue " << m_id << " received change notification from scene " << m_i_scene->getId());
    m_cuestack.notifyCueChanged(this);
};


Cue::~Cue()
{
    LOG_DEBUG("DESTRUCTOR: Cue " << m_id);
    if (m_has_scene)
    {
        LOG_DEBUG(" - Removing us from Scene " << m_i_scene->getId() << " notification list");
        m_i_scene->removeNotifyClient(this);
        m_has_scene = false;
    }
    cue_signal(CUE_DELETED, m_id, m_id);
}


/*
 * Accessor functions
 */
void Cue::setName(std::string name)
{
    m_name = name;
    m_cuestack.notifyCueChanged(this);
};

void Cue::loadScene(scene_iterator i_scene)
{
    m_i_scene = i_scene;
    m_has_scene = true;
    m_i_scene->addNotifyClient(this);
    m_cuestack.notifyCueChanged(this);
};

void Cue::clearScene()
{
    m_has_scene = false;
    m_i_scene->removeNotifyClient(this);
    m_cuestack.notifyCueChanged(this);
};

void Cue::setTimeIn(float time_in)
{
    m_time_in = time_in;
    m_cuestack.notifyCueChanged(this);
};

void Cue::setTimeOut(float time_out)
{
    m_time_out = time_out;
    m_cuestack.notifyCueChanged(this);
};

void Cue::setDelayIn(float delay_in)
{
    m_delay_in = delay_in;
    m_cuestack.notifyCueChanged(this);
};

void Cue::setDelayOut(float delay_out)
{
    m_delay_out = delay_out;
    m_cuestack.notifyCueChanged(this);
};

void Cue::clearTimeIn()
{
    m_time_in = CUE_TIME_NOT_DEFINED;
    m_cuestack.notifyCueChanged(this);
};
void Cue::clearTimeOut()
{
    m_time_in = CUE_TIME_NOT_DEFINED;
    m_cuestack.notifyCueChanged(this);
};
void Cue::clearDelayIn()
{
    m_time_in = CUE_TIME_NOT_DEFINED;
    m_cuestack.notifyCueChanged(this);
};
void Cue::clearDelayOut()
{
    m_time_in = CUE_TIME_NOT_DEFINED;
    m_cuestack.notifyCueChanged(this);
};






boost::signals::connection Cue::notifyConnect(const cueSignal::slot_type& slot)
{
    std::cout << " IN Cue Notify Connect" << std::endl;
    return cue_signal.connect(slot);
    std::cout << " DONE IN Cue Notify Connect" << std::endl;
}

void Cue::notifyDisconnect(const boost::signals::connection connection)
{
    cue_signal.disconnect(connection);
}
