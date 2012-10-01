#ifndef FEEDBACK_H
#define FEEDBACK_H
#include "FCam/Action.h"
#include <QObject>
#include <QDebug>

class Messenger : public QObject{
        Q_OBJECT
    public:
        Messenger(){
        }
        void emitPhotoTaken(){
            emit photoTaken();
            qDebug()<<"feedback";
        }
    signals:
        void photoTaken();
};

class feedBack : public FCam::CopyableAction<feedBack>{
public:
    feedBack();
    Messenger* msgr;
    void doAction();
};



#endif // FEEDBACK_H
