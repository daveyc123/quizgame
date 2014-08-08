#include "qquizresult.h"

QQuizResult::QQuizResult(QString name, long score, QObject *parent) :
    QObject(parent)
{
    mName = name;
    mScore = score;
}

QString QQuizResult::name() {
    return mName;
}

long QQuizResult::score() {
    return mScore;
}
