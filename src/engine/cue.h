#ifndef _HAVE_CUE_H
#define _HAVE_CUE_H

#include <string>

#include <iostream>

#include <engine/scene.hpp>
#include <engine/cuestack.h>

#define CUE_TIME_NOT_DEFINED -1
#define CUE_DWELL_TIME_INFINITE -1
#define CUE_DEFAULT_JUMPTO_COUNT 1

class Cue
{

    friend class CueStack;

    public:
        enum CueSignalType
        {
            CUE_CHANGED,
            CUE_DELETED
        };
        typedef boost::signal<void (CueSignalType type, Cue * cue)> CueSignal;

        Cue(const unsigned long id, CueStack& cuestack);
        ~Cue();
        const unsigned long getId() const { return m_id; };

        /*
         * Accessor functions
         */
        void setName(std::string name);
        void loadScene(Scene* p_scene);
        void setTimeIn(float time_in);
        void setTimeOut(float time_out);
        void setDelayIn(float delay_in);
        void setDelayOut(float delay_out);
        void clearScene();
        void clearTimeIn();
        void clearTimeOut();
        void clearDelayIn();
        void clearDelayOut();
        std::string getName() const { return m_name; };
        inline bool hasScene() const { return m_has_scene; };
        inline Scene* getScene() const { return m_p_scene; };
        inline const float getTimeIn() const { return (m_time_in == CUE_TIME_NOT_DEFINED ? m_default_time_in : m_time_in); };
        inline const float getTimeOut() const { return (m_time_out == CUE_TIME_NOT_DEFINED ? m_default_time_out : m_time_out); };
        inline const float getDelayIn() const { return (m_delay_in == CUE_TIME_NOT_DEFINED ? m_default_delay_in : m_delay_in); };
        inline const float getDelayOut() const { return (m_delay_out == CUE_TIME_NOT_DEFINED ? m_default_delay_out : m_delay_out); };
        inline const bool hasTimeIn() const { return (m_time_in != CUE_TIME_NOT_DEFINED); };
        inline const bool hasTimeOut() const { return (m_time_out != CUE_TIME_NOT_DEFINED); };
        inline const bool hasDelayIn() const { return (m_delay_in != CUE_TIME_NOT_DEFINED); };
        inline const bool hasDelayOut() const { return (m_delay_out != CUE_TIME_NOT_DEFINED); };

        //
        // Scene signal slot -- we're the slot (receiver)
        //
        void sceneHasChanged(Scene::SceneSignalType type, Scene* p_scene);
        Scene::SceneConnection m_scene_connection;

        /*
         * Cue change / delete notification
         */
        static boost::signals::connection connect(const CueSignal::slot_type& slot);
        static void disconnect(const boost::signals::connection connection);

    private:

        unsigned long m_id;        // Identifier
        const CueStack & m_cuestack;
        std::string m_name;
        bool m_has_scene;
        Scene* m_p_scene;
        float m_time_in;
        float m_time_out;
        float m_delay_in;
        float m_delay_out;
        float m_dwell_time;
        bool m_has_jumpto_cue;
        cue_iterator m_i_jumpto_cue;
        unsigned int m_jumpto_count_configured;
        unsigned int m_jumpto_count_current;

        static CueSignal cue_signal;

        static float m_default_time_in;
        static float m_default_time_out;
        static float m_default_delay_in;
        static float m_default_delay_out;
        static float m_default_dwell_time;
        static unsigned m_default_jumpto_count;
};

#endif // _HAVE_CUE_H
