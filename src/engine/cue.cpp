#include <engine/cue.h>

#include <boost/bind.hpp>

float Cue::m_default_time_in = 5.0;
float Cue::m_default_time_out = 5.0;
float Cue::m_default_delay_in = 0.0;
float Cue::m_default_delay_out = 0.0;
float Cue::m_default_dwell_time = CUE_DWELL_TIME_INFINITE;
unsigned  Cue::m_default_jumpto_count = 1;

Cue::CueSignal Cue::cue_signal;

Cue::Cue(const unsigned long id, CueStack & cuestack) :
                m_id(id), m_cuestack(cuestack), m_has_scene(false),
                m_time_in(CUE_TIME_NOT_DEFINED), m_time_out(CUE_TIME_NOT_DEFINED),
                m_delay_in(CUE_TIME_NOT_DEFINED), m_delay_out(CUE_TIME_NOT_DEFINED),
                m_dwell_time(m_default_dwell_time), m_has_jumpto_cue(false),
                m_jumpto_count_configured(0), m_jumpto_count_current(0)
{
    LOG_DEBUG("CONSTRUCTOR: Cue " << m_id);
    m_scene_connection = Scene::connect(boost::bind(&Cue::sceneHasChanged, this, _1, _2));
};

Cue::~Cue()
{
    LOG_DEBUG("DESTRUCTOR: Cue " << m_id);
    Scene::disconnect(m_scene_connection);
    cue_signal(CUE_DELETED, this);
}

void Cue::sceneHasChanged(Scene::SceneSignalType type, Scene* p_scene)
{
    if ((m_has_scene) && (m_p_scene == p_scene))
    {
        if (type == Scene::SCENE_CHANGED)
        {
            LOG_DEBUG("Cue " << m_id << " received change notification from scene " << m_p_scene->getId());
            m_cuestack.notifyCueChanged(this);
        } else { // type == SCENE_DELETED
            LOG_DEBUG("Cue " << m_id << " received delete notification from scene");
            m_has_scene = false;
            m_cuestack.notifyCueChanged(this);
        }
    }
}

/*
 * Accessor functions
 */
void Cue::setName(std::string name)
{
    m_name = name;
    m_cuestack.notifyCueChanged(this);
};

void Cue::loadScene(Scene* p_scene)
{
    m_p_scene = p_scene;
    m_has_scene = true;
    m_cuestack.notifyCueChanged(this);
};

void Cue::clearScene()
{
    m_has_scene = false;
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






boost::signals::connection Cue::connect(const CueSignal::slot_type& slot)
{
    std::cout << " IN Cue Notify Connect" << std::endl;
    return cue_signal.connect(slot);
    std::cout << " DONE IN Cue Notify Connect" << std::endl;
}

void Cue::disconnect(const boost::signals::connection connection)
{
    cue_signal.disconnect(connection);
}
