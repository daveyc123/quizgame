#include "qquizuicontroller.h"
#include <QThread>
#include <QtDebug>

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

    mButtonThread = new QQuizButtonThread();
    mButtonThread->addButton(27, "green");
    mButtonThread->addButton(11, "red");

    QObject::connect(gameState, &QQuizGameState::gameStarted, this, &QQuizUIController::onGameStarted);
    QObject::connect(gameState, &QQuizGameState::newQuestion, this, &QQuizUIController::onNewQuestion);
    QObject::connect(gameState, &QQuizGameState::gameFinished, this, &QQuizUIController::onGameFinished);
    QObject::connect(gameState->timeCounter(), &QQuizTimeCounter::updated, this, &QQuizUIController::onTimerFired);
    QObject::connect(mButtonThread, &QQuizButtonThread::buttonPressed, this, &QQuizUIController::onButtonPressed);
    mButtonThread->start(QThread::HighPriority);
}

QQuizUIController::~QQuizUIController() {
    delete mButtonThread;
}

void QQuizUIController::setPage(QString page) {
    mPage = page;
    if (page.compare(PAGE_HIGH_SCORE) == 0) {
        if (mQmlContext != NULL) {
            QList<QObject*> dataList;
            foreach(QQuizResult* r, mGameState->results()->getResults()) {
                // If I pass in r, this doesn't seem to work (segfaults). All the examples I've seen do a copy of the data
                // so perhaps the qml context is responsible for cleaning up the items
                dataList.append(new QQuizResult(r->name(), r->score()));
            }

            mQmlContext->setContextProperty("resultsModel", QVariant::fromValue(dataList));
        }
    }

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

void QQuizUIController::onTimerFired(long time) {
    mTimerText.sprintf("%3.1f", ((float)time)/1000.0);
    emit newTimerText(mTimerText);
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

QString QQuizUIController::timerText() {
    return mTimerText;
}

void QQuizUIController::onButtonPressed(QString id) {
    if (id.compare(id, "green") == 0) {
        annePressed();
    } else if (id.compare(id, "red") == 0) {
        shawnPressed();
    }
}

void QQuizUIController::setQmlContext(QQmlContext *context) {
    mQmlContext = context;
    setPage(PAGE_HIGH_SCORE);
}
