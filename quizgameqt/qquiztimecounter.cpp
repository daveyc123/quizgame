#include "qquiztimecounter.h"
#include <QDateTime>

QQuizTimeCounter::QQuizTimeCounter()
{

    timer = new QTimer(this);
    timer->setInterval(200);
    connect(timer, SIGNAL(timeout()), this, SLOT(sendUpdate()));
}

long QQuizTimeCounter::getTimeMs()
{
    // Return current system time (used to calculate the speed).
    QDateTime time = QDateTime::currentDateTime();
    return time.toMSecsSinceEpoch();
}

void QQuizTimeCounter::start() {
    mStartTime = getTimeMs();

    timer->start();
}

void QQuizTimeCounter::stop() {
    mEndTime = getTimeMs();

    timer->stop();
}

long QQuizTimeCounter::elapsedTime() {
    return getTimeMs() - mStartTime;
}


void QQuizTimeCounter::sendUpdate() {
    emit updated(elapsedTime());
}
