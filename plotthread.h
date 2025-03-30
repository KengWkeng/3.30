#ifndef PLOTTHREAD_H
#define PLOTTHREAD_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QDebug>
#include <QVector>
#include <qcustomplot.h>

class plotThread : public QObject
{
    Q_OBJECT
public:
    explicit plotThread(QObject *parent = nullptr);

signals:
};

#endif // PLOTTHREAD_H
