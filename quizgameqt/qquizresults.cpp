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
            addResult(result, false); // should be serialized in sort order, but in case they've been tweaked use addResult to get proper sorting
        }

        loadFile.close();
    }
}

int QQuizResults::addResult(QQuizResult *result, bool shouldSerialize) {
    if (result->name().compare("winner") == 0) {
        return 1;
    } else if (result->name().compare("loser") == 0) {
        return 2;
    }
    int rank = 0;
    if (mResults.size() == 0) {
        mResults.append(result);
        rank = 1;
    } else {
        for (int i=0; i<mResults.size(); i++) {
            if (i==0 && mResults[i]->score() > result->score()) {
                mResults.insert(0, result);
                rank = 1;
                break;
            } else if (i==(mResults.size() - 1)) {
                mResults.append(result);
                rank = mResults.size();
                break;
            } else if (result->score() > mResults[i]->score() && result->score() < mResults[i+1]->score()) {
                mResults.insert(i+1, result);
                rank = i + 2;
                break;
            }
        }
    }

    if (shouldSerialize) {
        serialize(); // save in case we crash
    }

    return rank;
}

int QQuizResults::addResult(QQuizResult *result) {
   return addResult(result, true);
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
