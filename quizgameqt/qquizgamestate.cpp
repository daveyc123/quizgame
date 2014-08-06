#include "qquizgamestate.h"
#include <QtDebug>

QQuizGameState::QQuizGameState(QQuizQuestions* questions, QQuizResults* results, QObject *parent) :
    QObject(parent)
{
    mGameQuestions = questions;
    mResults = results;
}

void QQuizGameState::startGame(QString playerName) {
    mCurrentPlayerName = playerName;
    mCurrentQuestions = mGameQuestions->getRandomQuestions(QUESTIONS_PER_GAME);
    mCurrentQuestionIndex = 0;
    mInGame = true;

    emit gameStarted();
    emit newQuestion(mCurrentQuestions.at(mCurrentQuestionIndex), mCurrentQuestionIndex + 1);
}

bool QQuizGameState::answerCurrentQuestion(QString answer) {
    bool result = true;

    QuizQuestion* currentQuestion = mCurrentQuestions.at(mCurrentQuestionIndex);
    if (QString::compare(currentQuestion->answer(), answer) == 0) {
        // add the delay to the timer
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
    // stop the timer

    qDebug() << "Timer code isn't being inserted into the result";
    QQuizResult result(mCurrentPlayerName, 10); // TODO get the proper value from the time
    mResults->addResult(&result);

    mInGame = false;
    emit gameFinished();
}
