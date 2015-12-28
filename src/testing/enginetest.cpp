#include <testing/enginetest.h>

#include <logger.h>
#include <engine/exceptions.h>
#include <engine/engine.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <engine/channel.hpp>
#include <engine/scene.hpp>
#include <engine/submaster.hpp>
#include "engine/dimmer.hpp"
#include <engine/universe.hpp>
#include <engine/cue.h>
#include <engine/cuestack.h>
#include <engine/crossfader.h>
#include <engine/scene_repository.hpp>

using std::cout;
using std::cin;
using std::endl;

Engine * e;

void EngineTest::main()
{
  e = new Engine(4, 0, 512, 1024, 48);

  assert(e->getNumChannels() == 1024);

  LOG_INFO("Starting main loop");
}




void EngineTest::unittest()
{
    char crossfader;
    char editor;
    char submaster1;
    char submaster2;

    LOG_INFO("Test1");
    bool exc = false;

    /*
     * Channel patching
     */
    assert(e->getChannel(0).getId() == 0);
    assert(e->getChannel(1000).getId() == 1000);

    e->getChannel(1).addDimmer(*e->m_p_dimmers[10]);
    e->getChannel(1).addDimmer(*e->m_p_dimmers[15]);
    e->getChannel(1).addDimmer(*e->m_p_dimmers[20]);
    e->getChannel(1).removeDimmer(*e->m_p_dimmers[15]);

    exc = false;
    try {
        e->getChannel(1).removeDimmer(*e->m_p_dimmers[15]);
    }
    catch (Dimmer::NotPatchedException ex) {
        LOG_INFO("Exception " << ex.what() << " thrown");
        exc = true;
    }
    assert(exc);

    e->getChannel(2).addDimmer(*e->m_p_dimmers[15]);

    exc = false;
    try {
        e->getChannel(2).addDimmer(*e->m_p_dimmers[20]);
    }
    catch (Dimmer::AlreadyPatchedException ex) {
        LOG_INFO("Exception " << ex.what() << " thrown");
        exc = true;
    }
    assert(exc);

    e->getChannel(2).addDimmer(*e->m_p_dimmers[1000]);

    exc = false;
    try {
        e->getChannel(2).addDimmer(*e->m_p_dimmers[1000]);
    }
    catch (Channel::AlreadyPatchedException ex) {
        LOG_INFO("Exception " << ex.what() << " thrown");
        exc = true;
    }
    assert(exc);

    /*
     * HTP testing
     */

    // Channel not set to anything, should indicate default
    e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 0);

    // Setting first channel (xfader). Channel value should be fader value.
    e->getChannel(1).registerSource(&crossfader, Channel::SOURCE_XFADER, 128); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 128);

    // Setting 2nd source (submaster). Is lower than first, so channel value
    // should remain unchanged.
    e->getChannel(1).registerSource(&submaster1, Channel::SOURCE_SUBMASTER, 64); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 128);

    // Increasing submaster. Now higher, should be new channel value.
    e->getChannel(1).setLevel(&submaster1, 192); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 192);
   
    // Increasing submaster again. 
    e->getChannel(1).setLevel(&submaster1, 255); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 255);

    // Submaster will now be lower than xfader.
    e->getChannel(1).setLevel(&submaster1, 10); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 128);

    // Submaster will win again.
    e->getChannel(1).setLevel(&crossfader, 00); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 10);

    // Both are at zero now.
    e->getChannel(1).setLevel(&submaster1, 00); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 0);

    // Crossfader to 128. Crossfader should be source now.
    e->getChannel(1).setLevel(&crossfader, 128); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 128);
    assert(e->getChannel(1).m_p_active_source == &crossfader);

    // Submaster is 128 too. Source should still be crossfader.
    e->getChannel(1).setLevel(&submaster1, 128); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 128);
    assert(e->getChannel(1).m_p_active_source == &crossfader);

    // Crossfader reduced to 127. Source should now be submaster.
    e->getChannel(1).setLevel(&crossfader, 127); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 128);
    assert(e->getChannel(1).m_p_active_source == &submaster1);

    // Just resubmitting crossfader with same value. Sould not change anything.
    e->getChannel(1).setLevel(&crossfader, 127); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 128);
    assert(e->getChannel(1).m_p_active_source == &submaster1);

    // Submaster also reduced to 127. Now crossfader should be active again.
    e->getChannel(1).setLevel(&submaster1, 127); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 127);
    assert(e->getChannel(1).m_p_active_source == &crossfader);

    // Editor activated. This should override all.
    e->getChannel(1).registerSource(&editor, Channel::SOURCE_EDITOR, 64); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 64);
    assert(e->getChannel(1).m_p_active_source == &editor);

    // Submaster incremented. This shouldn't change anything.
    e->getChannel(1).setLevel(&submaster1, 129); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 64);
    assert(e->getChannel(1).m_p_active_source == &editor);

    // Other submaster registered. Again, shouldn't change anything.
    e->getChannel(1).registerSource(&submaster2, Channel::SOURCE_SUBMASTER, 200); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 64);
    assert(e->getChannel(1).m_p_active_source == &editor);

    // Submaster 2 increased. Shouldn't change anything.
    e->getChannel(1).setLevel(&submaster2, 250); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 64);
    assert(e->getChannel(1).m_p_active_source == &editor);

    // Editor level changed. This should change the channel level.
    e->getChannel(1).setLevel(&editor, 3); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 3);
    assert(e->getChannel(1).m_p_active_source == &editor);

    // Try to patch same input source again. Should throw exception. Levels
    // should be unchanged.
    exc = false;
    try {
        e->getChannel(1).registerSource(&submaster2, Channel::SOURCE_SUBMASTER, 252); e->timerTick();
    }
    catch (Channel::InputSourceAlreadyRegisteredException ex) {
        LOG_INFO("Exception " << ex.what() << " thrown");
        exc = true;
    }
    assert(exc);
    assert(e->getChannel(1).getInputLevel() == 3);
    assert(e->getChannel(1).m_p_active_source == &editor);

    // Unregister override source. Should jump back to HTP source of Sub2.
    e->getChannel(1).unregisterSource(&editor); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 250);
    assert(e->getChannel(1).m_p_active_source == &submaster2);
    
    // Unregister sub1. Nothing should happen because sub is higher.
    e->getChannel(1).unregisterSource(&submaster1); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 250);
    assert(e->getChannel(1).m_p_active_source == &submaster2);

    // Now unregister sub2. We should now go to the level of xfader,
    // 127, which is our only remaining source.
    e->getChannel(1).unregisterSource(&submaster2); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 127);
    assert(e->getChannel(1).m_p_active_source == &crossfader);
    assert(e->getChannel(1).m_inputlevels.size() == 1);

    // Try unregistering sub2 again. Nothing should happen (we
    // haven't got error checking for this scenario.. should we?!);
    e->getChannel(1).unregisterSource(&submaster2); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 127);
    assert(e->getChannel(1).m_p_active_source == &crossfader);
    assert(e->getChannel(1).m_inputlevels.size() == 1);

    // Now unregister the last source. Level should jump back to the
    // default.
    e->getChannel(1).unregisterSource(&crossfader); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 0);
    assert(e->getChannel(1).m_p_active_source == NULL);
    assert(e->getChannel(1).m_inputlevels.size() == 0);

    // Register xfader again...
    e->getChannel(1).registerSource(&crossfader, Channel::SOURCE_XFADER, 100); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 100);
    assert(e->getChannel(1).m_p_active_source == &crossfader);
    assert(e->getChannel(1).m_inputlevels.size() == 1);

    // Register editor, should override
    e->getChannel(1).registerSource(&editor, Channel::SOURCE_EDITOR, 90); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 90);
    assert(e->getChannel(1).m_p_active_source == &editor);
    assert(e->getChannel(1).m_p_override_source == &editor);
    assert(e->getChannel(1).m_inputlevels.size() == 1);

    // Now unregister xfader first. Levels should not change but
    // input level list should be empty.
    e->getChannel(1).unregisterSource(&crossfader); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 90);
    assert(e->getChannel(1).m_p_active_source == &editor);
    assert(e->getChannel(1).m_inputlevels.size() == 0);

    // Now unregister the editor last. Level should go to default.
    e->getChannel(1).unregisterSource(&editor); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 0);
    assert(e->getChannel(1).m_p_active_source == NULL);
    assert(e->getChannel(1).m_inputlevels.size() == 0);

    // Try to set level with some unrecognised source
    e->getChannel(1).setLevel(&editor, 33); e->timerTick();
    assert(e->getChannel(1).getInputLevel() == 0);
    assert(e->getChannel(1).m_p_active_source == NULL);
    assert(e->getChannel(1).m_inputlevels.size() == 0);

    // At this time, all sources unregistered

    /*
     * Test grand master
     */
    e->getChannel(0).registerSource(&submaster1, Channel::SOURCE_SUBMASTER, 255);
    e->getChannel(1).registerSource(&submaster1, Channel::SOURCE_SUBMASTER, 128);
    e->getChannel(2).registerSource(&submaster1, Channel::SOURCE_SUBMASTER, 255);
    e->getChannel(3).registerSource(&submaster1, Channel::SOURCE_SUBMASTER, 128);
    e->getChannel(2).m_nomasters = true;
    e->getChannel(3).m_nomasters = true;
    for (int i = 0; i < 255 ; i++)
    {
        e->m_grand_master.setLevel(i);
        LOG_DEBUG("i is now " << i);
        e->timerTick();
        LOG_DEBUG("i = " << i << ", [0] = " << e->getChannel(0).getOutputLevel() << ", [1] " << e->getChannel(1).getOutputLevel() << ", [2] = " << e->getChannel(2).getOutputLevel() << ", [3] = " << e->getChannel(3).getOutputLevel());

        assert(e->getChannel(0).getOutputLevel() == i);
        assert(e->getChannel(1).getOutputLevel() == i/2);
        assert(e->getChannel(2).getOutputLevel() == 255);
        assert(e->getChannel(3).getOutputLevel() == 128);
    }
    e->m_grand_master.setLevel(255);

    e->getChannel(0).unregisterSource(&submaster1);
    e->getChannel(1).unregisterSource(&submaster1);
    e->getChannel(2).unregisterSource(&submaster1);
    e->getChannel(3).unregisterSource(&submaster1);

    /*
     * Scenes
     */
    Scene scene1(99999);
    for (int i = 0; i < 512; i++)
    {
        assert(scene1.channelIsUsed(i) == false);
    }
    exc = false;
    try {
        unsigned int j = scene1.getChannelLevel(1);
    }
    catch (Scene::NotInSceneException ex) {
        LOG_INFO("Exception " << ex.what() << " thrown");
        exc = true;
    }
    assert(exc);

    scene1.setChannelLevel(0, 255);
    scene1.setChannelLevel(511, 255);
    scene1.setChannelLevel(512, 255);
    assert(scene1.channelIsUsed(0));
    assert(scene1.channelIsUsed(511));
    assert(scene1.channelIsUsed(2000) == false);
    try {
        unsigned int j = scene1.getChannelLevel(515);
    }
    catch (Scene::NotInSceneException ex) {
        LOG_INFO("Exception " << ex.what() << " thrown");
        exc = true;
    }
    assert(exc);
    scene1.setChannelLevel(511, 128);


    /*
     * Scene list related testing
     */
    e->scenes.createScene(3);
    assert(e->scenes.getNumScenes() == 1);
    Scene* it1 = e->scenes.pGetScene(3);
    e->scenes.createScene(2);
    e->scenes.createScene(5);
    assert(e->scenes.getNumScenes() == 3);
    assert(e->scenes.pGetScene(3) == it1);

    exc = false;
    try {
        Scene* it2 = e->scenes.pGetScene(4);
    }
    catch (SceneRepository::DoesNotExistException ex) {
        LOG_INFO("Exception " << ex.what() << " thrown");
        exc = true;
    }
    exc = false;
    try {
        e->scenes.createScene(2);
    }
    catch (SceneRepository::AlreadyDefinedException ex) {
        LOG_INFO("Exception " << ex.what() << " thrown");
        exc = true;
    }
    for (SceneRepository::SceneIterator it3 = e->scenes.begin(); it3 != e->scenes.end(); it3++)
    {
        LOG_INFO("Scene " << it3->second->getId());
    }

//XXXX
//    e->scenes.getSceneList().erase(it1);

    e->scenes.renameScene(3, 9);
    for (SceneRepository::SceneIterator it3 = e->scenes.begin(); it3 != e->scenes.end(); it3++)
    // for (SceneIterator it3 = e->scenes.getSceneList().begin(); it3 != Scene::getSceneList().end(); it3++)
    {
        LOG_INFO("Scene " << it3->second->getId());
    }
    e->scenes.pGetScene(9)->setChannelLevel(0, 64);
    e->getAllsubsMaster().setLevel(255);
    e->m_grand_master.setLevel(255);


    /*
     * Submaster with scene testing
     */
    e->getChannel(0).addDimmer(*e->m_p_dimmers[0]);
    e->getChannel(0).addDimmer(*e->m_p_dimmers[1]);
    e->getChannel(0).addDimmer(*e->m_p_dimmers[2]);
    e->getChannel(511).addDimmer(*e->m_p_dimmers[3]);
    e->getChannel(511).addDimmer(*e->m_p_dimmers[4]);
    e->getChannel(511).addDimmer(*e->m_p_dimmers[5]);
    e->getChannel(1023).addDimmer(*e->m_p_dimmers[6]);
    e->getChannel(1023).addDimmer(*e->m_p_dimmers[7]);
    e->getChannel(1023).addDimmer(*e->m_p_dimmers[8]);

    // Should have UNDEF mode
    assert(e->m_p_submasters[0]->getMode() == Submaster::MODE_UNDEF);

    it1->setChannelLevel(0,255);
    it1->setChannelLevel(511,128);

    // Load scene. Should now have SCENE mode.
    LOG_INFO("Loading Scene");
    e->m_p_submasters[0]->loadScene(it1);
    assert(e->m_p_submasters[0]->getMode() == Submaster::MODE_SCENE);
    LOG_INFO("Setting Channel Level in Scene");
    it1->setChannelLevel(1023, 255);

    LOG_INFO("Setting Submaster level to 50%");
    e->m_p_submasters[0]->setLevel(128);
    e->timerTick();

    LOG_INFO("Just asserting channel input level");
    assert(e->getChannel(0).getInputLevel() == 128);
    e->timerTick();

    LOG_INFO("Setting allsubs level to 50%");
    e->getAllsubsMaster().setLevel(128);
    assert(e->getChannel(0).getInputLevel() == 64 );
    e->timerTick();

    LOG_INFO("Creating new scene 0");
    e->scenes.createScene(0);
    LOG_INFO("Creating new scene 99");
    e->scenes.createScene(99);
    e->timerTick();

    LOG_INFO("Removing scene 9");
    e->scenes.deleteScene(9);
    e->timerTick();

    LOG_INFO("Final scene list");
    for (SceneRepository::SceneIterator it3 = e->scenes.begin(); it3 != e->scenes.end(); it3++)
    // for (SceneIterator it3 = e->scenes.getSceneList().begin(); it3 != Scene::getSceneList().end(); it3++)
    {
        LOG_INFO("Scene " << it3->second->getId());
    }


    /*
     * Cue stack testing
     */
    assert(CueStack::getNumCueStacks() == 0);
    CueStack::createCueStack(1);
    assert(CueStack::getNumCueStacks() == 1);
    CueStack::findCueStack(1)->createCue(1);
    CueStack::findCueStack(1)->createCue(3);
    CueStack::findCueStack(1)->createCue(2);
    assert(CueStack::getNumCueStacks() == 1);
    assert(CueStack::findCueStack(1)->getNumCues() == 3);

    CueStack::findCueStack(1)->findCue(2)->loadScene(e->scenes.pGetScene(2));
    e->scenes.pGetScene(2)->setChannelLevel(33,33);
//    e->scenes.deleteScene(2);

    CueStack::deleteCueStack(1);

    /*
     * Unpatch all
     */
    e->getChannel(0).removeDimmer(*e->m_p_dimmers[0]);
    e->getChannel(0).removeDimmer(*e->m_p_dimmers[1]);
    e->getChannel(0).removeDimmer(*e->m_p_dimmers[2]);
    e->getChannel(511).removeDimmer(*e->m_p_dimmers[3]);
    e->getChannel(511).removeDimmer(*e->m_p_dimmers[4]);
    e->getChannel(511).removeDimmer(*e->m_p_dimmers[5]);
    e->getChannel(1023).removeDimmer(*e->m_p_dimmers[6]);
    e->getChannel(1023).removeDimmer(*e->m_p_dimmers[7]);
    e->getChannel(1023).removeDimmer(*e->m_p_dimmers[8]);

    e->getChannel(1).removeDimmer(*e->m_p_dimmers[10]);
    e->getChannel(1).removeDimmer(*e->m_p_dimmers[20]);
    e->getChannel(2).removeDimmer(*e->m_p_dimmers[15]);
    e->getChannel(2).removeDimmer(*e->m_p_dimmers[1000]);

    LOG_INFO("Cleaning up scenes...");

    e->scenes.deleteScene(0);
    e->scenes.deleteScene(2);
    e->scenes.deleteScene(5);
    e->scenes.deleteScene(99);

    e->getAllsubsMaster().setLevel(255);
    e->m_grand_master.setLevel(255);

    // Patch channels 1-1
    LOG_INFO("Making channels 1-1 patch");
    for (int i=0; i < 1024; i++)
    {
        e->getChannel(i).addDimmer(*e->m_p_dimmers[i]);
    }
    e->scenes.createScene(0);
    e->scenes.createScene(1);
    e->scenes.createScene(2);
    
    e->scenes.pGetScene(0)->setChannelLevel(0, 255);
    e->scenes.pGetScene(0)->setChannelLevel(1, 0);
    e->scenes.pGetScene(0)->setChannelLevel(2, 255);
    e->scenes.pGetScene(0)->setChannelLevel(3, 127);
    e->scenes.pGetScene(0)->setChannelLevel(4, 255);
    e->scenes.pGetScene(0)->setChannelLevel(5, 127);

    e->scenes.pGetScene(1)->setChannelLevel(0, 0);
    e->scenes.pGetScene(1)->setChannelLevel(1, 255);
    e->scenes.pGetScene(1)->setChannelLevel(2, 255);
    e->scenes.pGetScene(1)->setChannelLevel(3, 255);
    e->scenes.pGetScene(1)->setChannelLevel(4, 127);
    e->scenes.pGetScene(1)->setChannelLevel(5, 0);

    e->scenes.pGetScene(2)->setChannelLevel(0, 128);
    e->scenes.pGetScene(2)->setChannelLevel(1, 128);
    e->scenes.pGetScene(2)->setChannelLevel(2, 128);
    e->scenes.pGetScene(2)->setChannelLevel(3, 128);
    e->scenes.pGetScene(2)->setChannelLevel(4, 255);
    e->scenes.pGetScene(2)->setChannelLevel(5, 255);

    CueStack::createCueStack(0);
    CueStack::findCueStack(0)->createCue(0);
    CueStack::findCueStack(0)->createCue(1);
    CueStack::findCueStack(0)->createCue(2);
    CueStack::findCueStack(0)->findCue(0)->loadScene(e->scenes.pGetScene(0));
    CueStack::findCueStack(0)->findCue(1)->loadScene(e->scenes.pGetScene(1));
//    CueStack::findCueStack(0)->findCue(2)->loadScene(e->scenes.pGetScene(2));

    Crossfader::main_xy->loadCueStack(CueStack::findCueStack(0));
    Crossfader::main_xy->loadCueX(CueStack::findCueStack(0)->findCue(0));
//    Crossfader::main_xy->loadCueY(CueStack::findCueStack(0)->findCue(1));

    std::string input;

    e->timerTick();
    LOG_INFO("Scene 0 (FF 00 FF 50 FF 50) loaded. Going to fade to Scene 2 (00 FF FF FF 50 00).");
    cout << "Please press enter";
    getline(cin, input); 
    for (int i = 255 ; i >= 0; i--)
    {
        Crossfader::x1->setLevel(i);
        Crossfader::y1->setLevel(i);
        // Crossfader::main_xy->setLevel(FADER_X, i);
        // Crossfader::main_xy->setLevel(FADER_Y, 255-i);
        e->timerTick();
    }

    LOG_INFO("Scene 2 (00 FF FF FF 50 00) reached. Going to fade to Scene 3; this is an empty scene and no output should change.");
    cout << "Please press enter";
    getline(cin, input); 
    // This one should be reversed; move upwards
    for (int i = 0 ; i <= 255; i++)
    {
        Crossfader::x1->setLevel(i);
        Crossfader::y1->setLevel(i);
        // Crossfader::main_xy->setLevel(FADER_X, i);
        // Crossfader::main_xy->setLevel(FADER_Y, 255-i);
        e->timerTick();
    }
    LOG_INFO("Fading again. Nothing should happen as we are at the end of the cue list.");
    cout << "Please press enter";
    getline(cin, input); 
    // Normal orientation again
    for (int i = 255 ; i >= 0; i--)
    {
        Crossfader::x1->setLevel(i);
        Crossfader::y1->setLevel(i);
        // Crossfader::main_xy->setLevel(FADER_X, i);
        // Crossfader::main_xy->setLevel(FADER_Y, 255-i);
        e->timerTick();
    }

    LOG_INFO("Starting again, with loop setting");

    CueStack::findCueStack(0)->setLoop(true);
    Crossfader::main_xy->loadCueX(CueStack::findCueStack(0)->findCue(0));
    e->timerTick();
    LOG_INFO("Now loading scene 2 into cue 2.");
    cout << "Please press enter";
    getline(cin, input); 
    CueStack::findCueStack(0)->findCue(2)->loadScene(e->scenes.pGetScene(2));
    LOG_INFO("Scene 0 (FF 00 FF 50 FF 50) loaded. Going to fade to Scene 1 (00 FF FF FF 50 00).");
    cout << "Please press enter";
    getline(cin, input); 
    // Reversed again
    for (int i = 0; i <= 255; i++)
    {
        Crossfader::x1->setLevel(i);
        Crossfader::y1->setLevel(i);
        // Crossfader::main_xy->setLevel(FADER_X, i);
        // Crossfader::main_xy->setLevel(FADER_Y, 255-i);
        e->timerTick();
    }
    e->timerTick();
    LOG_INFO("Scene 1 (00 FF FF FF 50 00) reached. Going to fade to Scene 2 now (50 50 50 50 FF FF)");
    cout << "Please press enter";
    getline(cin, input); 
    // Normal
    for (int i = 255 ; i >= 0; i--)
    {
        Crossfader::x1->setLevel(i);
        Crossfader::y1->setLevel(i);
        // Crossfader::main_xy->setLevel(FADER_X, i);
        // Crossfader::main_xy->setLevel(FADER_Y, 255-i);
        e->timerTick();
    }
    LOG_INFO("Scene 2 reached. Fading again, due to loop this should go back to Scene 0.");
    cout << "Please press enter";
    getline(cin, input); 
    // Reversed
    for (int i = 0; i <= 255; i++)
    {
        Crossfader::x1->setLevel(i);
        Crossfader::y1->setLevel(i);
        // Crossfader::main_xy->setLevel(FADER_X, i);
        // Crossfader::main_xy->setLevel(FADER_Y, 255-i);
        e->timerTick();
    }


    // Now trying again but skipping cue
    Crossfader::main_xy->loadCueX(CueStack::findCueStack(0)->findCue(0));
    Crossfader::main_xy->loadCueY(CueStack::findCueStack(0)->findCue(2));
    e->timerTick();
    LOG_INFO("Scene 0 loaded again. Going DIRECTLY to Cue 2.");
    cout << "Please press enter";
    getline(cin, input); 
    // Normal
    for (int i = 255 ; i >= 0; i--)
    {
        Crossfader::x1->setLevel(i);
        Crossfader::y1->setLevel(i);
        // Crossfader::main_xy->setLevel(FADER_X, i);
        // Crossfader::main_xy->setLevel(FADER_Y, 255-i);
        e->timerTick();
    }

    LOG_INFO("Now half-fading from cue 0 to 1");
    Crossfader::main_xy->loadCueX(CueStack::findCueStack(0)->findCue(0));
    e->timerTick();
    cout << "Please press enter";
    getline(cin, input); 
    // Reversed
    for (int i = 0 ; i < 127 ; i++)
    {
        Crossfader::x1->setLevel(i);
        Crossfader::y1->setLevel(i);
        // Crossfader::main_xy->setLevel(FADER_X, i);
        // Crossfader::main_xy->setLevel(FADER_Y, 255-i);
        e->timerTick();
    }
    cout << "Please press enter";
    getline(cin, input); 
    Crossfader::main_xy->loadCueY(CueStack::findCueStack(0)->findCue(2));
    e->timerTick();
    cout << "Please press enter";
    getline(cin, input); 
    for (int i = 127 ; i <= 255; i++)
    {
        Crossfader::x1->setLevel(i);
        Crossfader::y1->setLevel(i);
        // Crossfader::main_xy->setLevel(FADER_X, i);
        // Crossfader::main_xy->setLevel(FADER_Y, 255-i);
        e->timerTick();
    }

    LOG_INFO("Automatic/timed fade from cue 0 to 1");
    Crossfader::main_xy->loadCueX(CueStack::findCueStack(0)->findCue(0));
    CueStack::findCueStack(0)->findCue(1)->setTimeIn(0);
    CueStack::findCueStack(0)->findCue(1)->setTimeOut(0);
    CueStack::findCueStack(0)->findCue(1)->setDelayIn(5.0);
    e->timerTick();
    cout << "Please press enter";
    getline(cin, input); 
    // Reversed
    Crossfader::main_xy->go();
    for (int i = 0 ; i < 500 ; i++)
    {
        e->timerTick();
    }

    // engine->Run();
}
