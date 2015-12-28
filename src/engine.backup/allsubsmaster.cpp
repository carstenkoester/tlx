#include <engine/allsubsmaster.h>

#include <engine/master.h>
#include <engine/submaster.h>

void AllsubsMaster::setLevel (const unsigned int level)
{
    Master::setLevel(level);

    for (int i = 0; i < Submaster::getNumSubmasters() ; i++) {
        Submaster::findByNum(i)->notifyAllsubsMasterChanged();
    }    
}
