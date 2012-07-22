#ifndef DOT_H
#define DOT_H

#include <QLabel>

class dot : public QLabel
{
    Q_OBJECT
public:
    explicit dot(QWidget *parent = 0);
    explicit dot(QString, QWidget *parent = 0);
    
signals:
    
public slots:
    void move(int x, int y);
};

#endif // DOT_H
