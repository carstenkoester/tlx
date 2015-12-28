bin_PROGRAMS = test
test_LDADD = $(OLA_LIBS) -lapr-1 -laprutil-1 -llog4cxx -lboost_signals
AM_CPPFLAGS = -I/usr/local/include -DDEBUG -std=c++0x
test_SOURCES = engine/engine.cpp engine/universe.cpp engine/dimmer.cpp engine/channel.cpp engine/scene.cpp engine/scene_repository.cpp engine/master.cpp engine/submaster.cpp engine/cue.cpp engine/cuestack.cpp engine/crossfader_master.cpp engine/crossfader_worker.cpp engine/crossfader.cpp testing/enginetest.cpp test.cpp
