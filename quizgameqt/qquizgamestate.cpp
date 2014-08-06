#include "qquizgamestate.h"
#include <QtDebug>

#define TIME_PENALTY 5000

QQuizGameState::QQuizGameState(QQuizQuestions* questions, QQuizResults* results, QObject *parent) :
    QObject(parent)
{
    mGameQuestions = questions;
    mResults = results;
    mQuizTimeCounter = new QQuizTimeCounter();
}

void QQuizGameState::startGame(QString playerName) {
    mCurrentPlayerName = playerName;
    mCurrentQuestions = mGameQuestions->getRandomQuestions(QUESTIONS_PER_GAME);
    mCurrentQuestionIndex = 0;
    mInGame = true;

    mQuizTimeCounter->start();

    emit gameStarted();
    emit newQuestion(mCurrentQuestions.at(mCurrentQuestionIndex), mCurrentQuestionIndex + 1);
}

bool QQuizGameState::answerCurrentQuestion(QString answer) {
    bool result = true;

    QuizQuestion* currentQuestion = mCurrentQuestions.at(mCurrentQuestionIndex);
    if (QString::compare(currentQuestion->answer(), answer) == 0) {
        mQuizTimeCounter->addElapsedTime(TIME_PENALTY);
        result = false;
    }

    mCurrentQuestionIndex++;

    if (mCurrentQuestionIndex == QUESTIONS_PER_GAME) {
        finishGame();
    } else {
        emit newQuestion(mCurrentQuestions.at(mCurrentQuestionIndex), mCurrentQuestionIndex + 1);
    }

    return result;
}

void QQuizGameState::finishGame() {
    mQuizTimeCounter->stop();

    QQuizResult result(mCurrentPlayerName, mQuizTimeCounter->elapsedTime());
    mResults->addResult(&result);

    mInGame = false;
    emit gameFinished();
}

QQuizTimeCounter* QQuizGameState::timeCounter() {
    return mQuizTimeCounter;
}
