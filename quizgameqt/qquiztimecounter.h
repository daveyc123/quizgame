#ifndef QQUIZTIMECOUNTER_H
#define QQUIZTIMECOUNTER_H

#include <QTimer>

class QQuizTimeCounter : public QObject
{
    Q_OBJECT

    Q_PROPERTY(long elapsedTime READ elapsedTime NOTIFY updated)

public:
    QQuizTimeCounter();

    long elapsedTime();

signals:
    void updated(long time);

public slots:
    void start();
    void stop();


private slots:
    void sendUpdate();

private:
    long mStartTime;
    long mEndTime;

    QTimer *timer;

    long getTimeMs();
};

#endif // QQUIZTIMECOUNTER_H
