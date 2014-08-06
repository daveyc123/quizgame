#include "qquizuicontroller.h"

#define PAGE_QUESTIONS "questionPage"
#define PAGE_HIGH_SCORE "highScorePage"

/**
 * UI controller to manage the interactions between the model (C++)
 * and the UI (qml). UI should listen to signals emitted by the control
 * to update accordingly.
 *
 * @brief QQuizUIController::QQuizUIController
 * @param parent
 */
QQuizUIController::QQuizUIController(QQuizGameState *gameState, QObject *parent) :
    QObject(parent)
{
    mGameState = gameState;
    setPage(PAGE_HIGH_SCORE);

    QObject::connect(gameState, &QQuizGameState::gameStarted, this, &QQuizUIController::onGameStarted);
    QObject::connect(gameState, &QQuizGameState::newQuestion, this, &QQuizUIController::onNewQuestion);
    QObject::connect(gameState, &QQuizGameState::gameFinished, this, &QQuizUIController::onGameFinished);
}

void QQuizUIController::setPage(QString page) {
    mPage = page;
    emit pageChanged(page);
}

QString QQuizUIController::page() {
    return mPage;
}

void QQuizUIController::startNewGame(QString playerName) {
    mGameState->startGame(playerName);
}

void QQuizUIController::onGameStarted() {
    setPage(PAGE_QUESTIONS);
}

void QQuizUIController::onGameFinished() {
    setPage(PAGE_HIGH_SCORE);
}

void QQuizUIController::annePressed() {
    mGameState->answerCurrentQuestion("Anne");
}

void QQuizUIController::shawnPressed() {
    mGameState->answerCurrentQuestion("Shawn");
}

void QQuizUIController::onNewQuestion(QuizQuestion* question, int questionCount) {
    mCurrentQuestionText = question->text();
    mCurrentQuestionCount = questionCount;
    emit newQuestionText(question->text());
    emit newQuestionCount(QString::number(questionCount));
}

QString QQuizUIController::questionText() {
    return mCurrentQuestionText;
}

QString QQuizUIController::questionCount() {
    return QString::number(mCurrentQuestionCount);
}
