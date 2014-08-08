#include <QFile>
#include <QJsonDocument>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonArray>
#include <QtDebug>
#include <QIODevice>
#include <QJsonParseError>

#include "qquizquestions.h"

QQuizQuestions::QQuizQuestions(QString questionsFile, QObject *parent) :
    QObject(parent)
{
    QFile loadFile(questionsFile);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Couldn't open questions file: " << questionsFile;
    } else {
        QByteArray questionData = loadFile.readAll();
        QJsonParseError jerr;
        QJsonDocument loadDoc(QJsonDocument::fromJson(questionData, &jerr));

        if (jerr.error) {
            qDebug() << "Tried to parse the json questions, but failed miserably" << jerr.errorString();
        } else {
            qDebug() << "Parsed the json questions like a champ";
        }

        QJsonObject json = loadDoc.object();

        QJsonArray questionsArray = json["questions"].toArray();
        for (int i=0; i<questionsArray.size(); i++) {
            QJsonObject questionJson = questionsArray[i].toObject();

            QuizQuestion * question = new QuizQuestion(questionJson["text"].toString(), questionJson["answer"].toString());
            mQuestions.append(question);
        }

        foreach(QuizQuestion* q, mQuestions) {
            qDebug() << q->text();
            qDebug() << q->answer();
        }

        loadFile.close();
    }
}

QList<QuizQuestion *> QQuizQuestions::getAllQuestions() {
    return mQuestions;
}

QList<QuizQuestion *> QQuizQuestions::getRandomQuestions(int count) {
    QList<QuizQuestion *> results;

    if (count > mQuestions.size()) {
        count = mQuestions.size();
    }

    QSet<int> chosen;
    while(chosen.size() < count) {
        int randNumber = qrand() % mQuestions.size();
        chosen.insert(randNumber);
    }

    foreach(int i, chosen) {
        results.append(mQuestions.at(i));
    }

    return results;
}
