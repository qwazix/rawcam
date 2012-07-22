#include "dot.h"

dot::dot(QWidget *parent) :
    QLabel(parent)
{

}

dot::dot(QString, QWidget *parent):
    QLabel(parent)
{

}


void dot::move(int x, int y){
    QLabel::move(x, y);
}
