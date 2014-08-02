#ifndef QQUIZQUESTIONS_H
#define QQUIZQUESTIONS_H

#include <QObject>
#include <quizquestion.h>

class QQuizQuestions : public QObject
{
    Q_OBJECT

public:
    QQuizQuestions(QString questionsFile, QObject *parent = 0);

public:
    QList<QuizQuestion *> getAllQuestions();
    QList<QuizQuestion *> getRandomQuestions(int count);

private:
    QList<QuizQuestion *> mQuestions;
};

#endif // QQUIZQUESTIONS_H
