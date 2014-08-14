#include "qquizuicontroller.h"
#include <QThread>
#include <QtDebug>
#include "defines.h"
#include <QThread>

#define PAGE_QUESTIONS "questionPage"
#define PAGE_HIGH_SCORE "highScorePage"
#define PAGE_START_GAME_COUNT_DOWN "startGameCountDownPage"

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

    emit pageChanged(page);
}

QString QQuizUIController::page() {
    return mPage;
}

void QQuizUIController::startNewGame(QString playerName) {
    mPlayerName = playerName;
    setPage(PAGE_START_GAME_COUNT_DOWN);
    emit showCountDown();
}

void QQuizUIController::countDownFinished() {
    mGameState->startGame(mPlayerName);
}

void QQuizUIController::onGameStarted() {
    setPage(PAGE_QUESTIONS);
}

void QQuizUIController::onGameFinished() {
    QList<QQuizResult *> results = mGameState->results()->getResults();
    if (results.size() > 0) {
        mTopScore.sprintf("%3.1f", ((float)results.at(0)->score())/1000.0);
        mTopName = results.at(0)->name();
        emit newTopScore(mTopScore);
        emit newTopName(mTopName);
    }
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

QString QQuizUIController::topScore() {
    return mTopScore;
}

QString QQuizUIController::topName() {
    return mTopName;
}

void QQuizUIController::onButtonPressed(QString id) {
    if (id.compare(id, "red") == 0) {
        annePressed();
    } else if (id.compare(id, "green") == 0) {
        shawnPressed();
    }
}
