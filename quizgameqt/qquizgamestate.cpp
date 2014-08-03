#include "qquizgamestate.h"

QQuizGameState::QQuizGameState(QQuizQuestions* questions, QObject *parent) :
    QObject(parent)
{
    mGameQuestions = questions;
}

void QQuizGameState::startGame() {
    mCurrentQuestions = mGameQuestions->getRandomQuestions(QUESTIONS_PER_GAME);
    mCurrentQuestionIndex = 0;

    emit gameStarted();
}
