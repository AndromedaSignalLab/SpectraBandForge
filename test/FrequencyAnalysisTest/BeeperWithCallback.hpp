#pragma once
#include <string>
#include <QObject>

class BeeperWithCallback : public QObject
{
    Q_OBJECT
public:
    int open();
    int play();
signals:
    void timech(int x);
};
