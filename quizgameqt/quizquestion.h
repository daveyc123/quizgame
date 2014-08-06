#ifndef QUIZQUESTION_H
#define QUIZQUESTION_H

#include <QObject>

class QuizQuestion : public QObject
{
    Q_OBJECT

public:
    QuizQuestion(QString text, QString answer, QObject *parent = 0);

public:
    QString text();
    QString answer();

private:
    QString mText;
    QString mAnswer;
};

#endif // QUIZQUESTION_H
