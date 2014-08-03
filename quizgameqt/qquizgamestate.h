#ifndef QQUIZGAMESTATE_H
#define QQUIZGAMESTATE_H

#include <QObject>
#include <qquizquestions.h>

#define QUESTIONS_PER_GAME 10

class QQuizGameState : public QObject
{
    Q_OBJECT
public:
    explicit QQuizGameState(QQuizQuestions* questions, QObject *parent = 0);

signals:
    void gameStarted();

public slots:
    void startGame();

private:
    QQuizQuestions* mGameQuestions;

    QList<QuizQuestion* > mCurrentQuestions;
    int mCurrentQuestionIndex;



};

#endif // QQUIZGAMESTATE_H
