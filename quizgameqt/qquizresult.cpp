#include "qquizresult.h"

QQuizResult::QQuizResult(QString name, long score, QObject *parent) :
    QObject(parent)
{
    mName = name;
    mScore = score;
}

QString QQuizResult::getName() {
    return mName;
}

long QQuizResult::getScore() {
    return mScore;
}
