#include "qquizuicontroller.h"

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
    setPage("questionPage");
}

void QQuizUIController::setPage(QString page) {
    mPage = page;
    emit pageChanged(page);
}

void QQuizUIController::setQuestionPage() {
    setPage("highScorePage");
}

QString QQuizUIController::page() {
    return mPage;
}

void QQuizUIController::startNewGame() {
    mGameState->startGame();
}
