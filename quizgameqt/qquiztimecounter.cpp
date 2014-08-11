#include "qquiztimecounter.h"
#include <QDateTime>

QQuizTimeCounter::QQuizTimeCounter()
{
    timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(sendUpdate()));
}

long QQuizTimeCounter::getTimeMs()
{
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

void QQuizTimeCounter::addElapsedTime(long time) {
    mStartTime = mStartTime - time;
}
