#include "feedback.h"
#include "QDebug"

feedBack::feedBack()
{
    msgr = new Messenger();
}

void feedBack::doAction(){
    msgr->emitPhotoTaken();

}


