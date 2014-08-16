#ifndef QQUIZGAMESTATE_H
#define QQUIZGAMESTATE_H

#include <QObject>
#include <qquizquestions.h>
#include <qquizresults.h>
#include <qquiztimecounter.h>

#define QUESTIONS_PER_GAME 15
#define TIME_PENALTY 7000

class QQuizGameState : public QObject
{
    Q_OBJECT
public:
    explicit QQuizGameState(QQuizQuestions* questions, QQuizResults* results, QObject *parent = 0);

signals:
    void gameCountdownStarted();
    void gameStarted();
    void gameFinished(int rank);
    void newQuestion(QuizQuestion* question, int questionCount);
    void wrongAnswer();
    void correctAnswer();

public slots:
    void startGameCountdown(QString playerName);
    void startGame(QString playerName);
    bool answerCurrentQuestion(QString answer);
    QQuizTimeCounter* timeCounter();
    QQuizResults* results();

public:
    QString currentPlayerName();
    bool inGame();

private:
    void finishGame();

    QQuizQuestions* mGameQuestions;
    QQuizResults* mResults;
    QQuizTimeCounter* mQuizTimeCounter;

    QList<QuizQuestion* > mCurrentQuestions;
    int mCurrentQuestionIndex;
    QString mCurrentPlayerName;

    bool mInGame;
};

#endif // QQUIZGAMESTATE_H
