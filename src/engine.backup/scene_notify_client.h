#ifndef _HAVE_SCENE_NOTIFY_CLIENT_H
#define _HAVE_SCENE_NOTIFY_CLIENT_H

class Scene;

class SceneNotifyClient
{
    public:
       virtual void notifySceneChanged(Scene* p_scene) =0;
       virtual void notifySceneDeleted(Scene* p_scene, const bool unregister) =0;
       virtual void notifySceneDeleted(Scene* p_scene) { notifySceneDeleted(p_scene, true); };
};

#endif // _HAVE_SCENE_NOTIFY_CLIENT_H
