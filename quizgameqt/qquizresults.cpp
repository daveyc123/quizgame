#include "qquizresults.h"
#include <QFile>
#include <QJsonDocument>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonArray>
#include <QtDebug>
#include <QIODevice>
#include <QJsonParseError>

QQuizResults::QQuizResults(QString resultsFile, QObject *parent) :
    QObject(parent)
{
    QFile loadFile(resultsFile);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Couldn't open results file: " << resultsFile;
    } else {
        QByteArray resultsData = loadFile.readAll();
        QJsonParseError jerr;
        QJsonDocument loadDoc(QJsonDocument::fromJson(resultsData, &jerr));

        if (jerr.error) {
            qDebug() << "Tried to parse the json results, but failed miserably" << jerr.errorString();
        } else {
            qDebug() << "Parsed the json results like a champ";
        }

        QJsonObject json = loadDoc.object();

        QJsonArray resultsArray = json["results"].toArray();
        for (int i=0; i<resultsArray.size(); i++) {
            QJsonObject resultJson = resultsArray[i].toObject();

            QQuizResult *result = new QQuizResult(resultJson["name"].toString(), (long)resultJson["score"].toInt()); // TODO losing some digits
            mResults.append(result);
        }

        foreach(QQuizResult* r, mResults) {
            qDebug() << r->getName();
            qDebug() << r->getScore();
        }

        loadFile.close();
    }
}

void QQuizResults::addResult(QQuizResult *result) {
    //  TODO some smarter sorting
    qDebug() << "Need some smarter sorting in addResult()";
    mResults.append(result);
    serialize(); // save in case we crash
}

QList<QQuizResult*> QQuizResults::getResults() {
    return mResults;
}

void QQuizResults::serialize() {
    qDebug() << "You haven't implement this yet!";
}

void QQuizResults::clearScores() {
    mResults.clear();
    serialize();
}
