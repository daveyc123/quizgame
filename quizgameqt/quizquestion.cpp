#include "quizquestion.h"

QuizQuestion::QuizQuestion(QString text, QString answer, QObject *parent) :
    QObject(parent)
{
    mText = text;
    mAnswer = answer;
}

QString QuizQuestion::getText() {
    return mText;
}

QString QuizQuestion::getAnswer() {
    return mAnswer;
}
