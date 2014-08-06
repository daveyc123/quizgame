#include "quizquestion.h"

QuizQuestion::QuizQuestion(QString text, QString answer, QObject *parent) :
    QObject(parent)
{
    mText = text;
    mAnswer = answer;
}

QString QuizQuestion::text() {
    return mText;
}

QString QuizQuestion::answer() {
    return mAnswer;
}
