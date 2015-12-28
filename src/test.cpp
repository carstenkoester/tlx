#include <engine/engine.h>
#include <logger.h>

#include <engine/cue.h>
#include <set>

#include <testing/enginetest.h>

int main(int, char *[]) {
  DOMConfigurator::configure("cklx-logging.xml");

/* Test here */

/*  std::set<Cue *> cues;

  Cue * c;

  c = new Cue(1); */

  EngineTest t;
  t.main();
  t.unittest();
}
    
