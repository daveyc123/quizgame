#ifndef QQUIZGAMESTATE_H
#define QQUIZGAMESTATE_H

#include <QObject>
#include <qquizquestions.h>
#include <qquizresults.h>

#define QUESTIONS_PER_GAME 10

class QQuizGameState : public QObject
{
    Q_OBJECT
public:
    explicit QQuizGameState(QQuizQuestions* questions, QQuizResults* results, QObject *parent = 0);

signals:
    void gameStarted();
    void gameFinished();
    void newQuestion(QuizQuestion* question, int questionCount);

public slots:
    void startGame(QString playerName);
    bool answerCurrentQuestion(QString answer);

private:
    void finishGame();

    QQuizQuestions* mGameQuestions;
    QQuizResults* mResults;

    QList<QuizQuestion* > mCurrentQuestions;
    int mCurrentQuestionIndex;
    QString mCurrentPlayerName;

    bool mInGame;
};

#endif // QQUIZGAMESTATE_H
