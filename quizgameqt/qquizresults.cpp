#include "qquizresults.h"
#include <QFile>
#include <QJsonDocument>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonArray>
#include <QtDebug>
#include <QIODevice>
#include <QJsonParseError>
#include <QtDebug>

QQuizResults::QQuizResults(QString resultsFile, QObject *parent) :
    QObject(parent)
{
    mResultsFile = resultsFile;

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
            qDebug() << "--------------";
            qDebug() << "Found result:" << result->name() << result->score();
            addResult(result, false); // should be serialized in sort order, but in case they've been tweaked use addResult to get proper sorting
            foreach (QQuizResult *realResult, mResults) {
             qDebug() << "List result:" << realResult->name() << realResult->score();
            }

            qDebug() << "--------------";
        }

        loadFile.close();
    }
}

void QQuizResults::addResult(QQuizResult *result, bool shouldSerialize) {
    if (mResults.size() == 0) {
        mResults.append(result);
    } else {
        for (int i=0; i<mResults.size(); i++) {
            if (i==0 && mResults[i]->score() > result->score()) {
                mResults.insert(0, result);
                break;
            } else if (i==(mResults.size() - 1)) {
                mResults.append(result);
                break;
            } else if (result->score() > mResults[i]->score() && result->score() < mResults[i+1]->score()) {
                mResults.insert(i+1, result);
                break;
            }
        }
    }

    if (shouldSerialize) {
        serialize(); // save in case we crash
    }
}

void QQuizResults::addResult(QQuizResult *result) {
   addResult(result, true);
}

QList<QQuizResult*> QQuizResults::getResults() {
    return mResults;
}

void QQuizResults::serialize() {

    QJsonObject jsonRoot;
    QJsonArray jsonResults;

    foreach(QQuizResult* r, mResults) {
        QJsonObject jsonResult;
        jsonResult.insert("name", r->name());
        QJsonValue score((int)r->score());
        jsonResult.insert("score", score);
        jsonResults.append(jsonResult);
    }
    jsonRoot.insert("results", jsonResults);

    QFile saveFile(mResultsFile);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Couldn't open the save file for writing";
        return;
    } else {
        QJsonDocument saveDoc(jsonRoot);
        saveFile.write(saveDoc.toJson());
        saveFile.close();
    }
}

void QQuizResults::clearScores() {
    mResults.clear();
    serialize();
}
