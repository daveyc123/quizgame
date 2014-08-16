#include "qquizgamestate.h"
#include <QtDebug>

QQuizGameState::QQuizGameState(QQuizQuestions* questions, QQuizResults* results, QObject *parent) :
    QObject(parent)
{
    mGameQuestions = questions;
    mResults = results;
    mQuizTimeCounter = new QQuizTimeCounter();
    mInGame = false;
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
    if (!mInGame) {
        return false;
    }

    bool result = true;

    QuizQuestion* currentQuestion = mCurrentQuestions.at(mCurrentQuestionIndex);
    if (QString::compare(currentQuestion->answer(), answer) != 0) {
        mQuizTimeCounter->addElapsedTime(TIME_PENALTY);
        result = false;
    }

    mCurrentQuestionIndex++;

    if (result) {
        emit correctAnswer();
    } else {
        emit wrongAnswer();
    }

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
    int rank = mResults->addResult(&result);

    mInGame = false;
    emit gameFinished(rank);
}

QQuizTimeCounter* QQuizGameState::timeCounter() {
    return mQuizTimeCounter;
}

QQuizResults* QQuizGameState::results() {
    return mResults;
}

QString QQuizGameState::currentPlayerName() {
    return mCurrentPlayerName;
}

void QQuizGameState::startGameCountdown(QString playerName) {
    mCurrentPlayerName = playerName;
    emit gameCountdownStarted();
}

bool QQuizGameState::inGame() {
    return mInGame;
}
