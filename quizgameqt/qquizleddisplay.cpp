#include "qquizleddisplay.h"
#include <QtDebug>

QQuizLedDisplay::QQuizLedDisplay(QQuizGameState *gameState, QObject *parent) :
    QObject(parent)
{
    mGameState = gameState;
    mLedDisplay = new LEDDisplay();

    QObject::connect(gameState, &QQuizGameState::gameStarted, this, &QQuizLedDisplay::onGameStarted);
    QObject::connect(gameState, &QQuizGameState::newQuestion, this, &QQuizLedDisplay::onNewQuestion);
    QObject::connect(gameState, &QQuizGameState::gameFinished, this, &QQuizLedDisplay::onGameFinished);
    QObject::connect(gameState, &QQuizGameState::wrongAnswer, this, &QQuizLedDisplay::onWrongAnswer);
    QObject::connect(gameState, &QQuizGameState::correctAnswer, this, &QQuizLedDisplay::onCorrectAnswer);
    QObject::connect(gameState->timeCounter(), &QQuizTimeCounter::updated, this, &QQuizLedDisplay::onTimerFired);

    showBannerText();
}

QQuizLedDisplay::~QQuizLedDisplay() {
    delete mLedDisplay;
}

void QQuizLedDisplay::onGameStarted() {
    mAllowTimerUpdates = true;
    mLedDisplay->SetRGB(0, 0xff, 0xff);
}

void QQuizLedDisplay::onGameFinished() {
    mLedDisplay->DisplayClear();
    mAllowTimerUpdates = true;
    showBannerText();
}

void QQuizLedDisplay::onTimerFired(long time) {
    if (mAllowTimerUpdates) {
        mLedDisplay->DisplayTimer((unsigned)time);
    }
}

void QQuizLedDisplay::onNewQuestion(QuizQuestion *question, int questionCount) {

}

void QQuizLedDisplay::onWrongAnswer() {
    mAllowTimerUpdates = false;
    mLedDisplay->SetRGB(0xff, 0x0, 0x0);
    QTimer::singleShot(500, this, SLOT(onWrongAnswerPartTwo()));
}

void QQuizLedDisplay::onCorrectAnswer() {
    mAllowTimerUpdates = false;
    mLedDisplay->SetRGB(0x0, 0xff, 0x0);
    mLedDisplay->DisplayCheckmark(LEDDisplay::kDupeCenter, LEDDisplay::kNoScroll);
    QTimer::singleShot(1000, this, SLOT(enableTimerUpdates()));
}

void QQuizLedDisplay::onWrongAnswerPartTwo() {
    mLedDisplay->DisplayString("5s Penalty", LEDDisplay::kLeft, LEDDisplay::kNoScroll, LEDDisplay::kVariable);
    QTimer::singleShot(500, this, SLOT(enableTimerUpdates()));
}

void QQuizLedDisplay::showBannerText() {
    mLedDisplay->SetRGB(0x0, 0x0, 0xff);

    mLedDisplay->DisplayString("Come play the Anne & Shawn Quiz Game!!! Only $2!!! Win a dinner and a movie night!!!", LEDDisplay::kLeft, LEDDisplay::kLeftScroll, LEDDisplay::kVariable);
}

void QQuizLedDisplay::enableTimerUpdates() {
    mLedDisplay->SetRGB(0x0, 0x0, 0xff);
    mAllowTimerUpdates = true;
}
