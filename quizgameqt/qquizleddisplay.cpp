#include "qquizleddisplay.h"
#include <QtDebug>

QQuizLedDisplay::QQuizLedDisplay(QQuizGameState *gameState, QObject *parent) :
    QObject(parent)
{
    mGameState = gameState;
    mLedDisplay = new LEDDisplay();

    QObject::connect(gameState, &QQuizGameState::gameStarted, this, &QQuizLedDisplay::onGameStarted);
    QObject::connect(gameState, &QQuizGameState::gameCountdownStarted, this, &QQuizLedDisplay::onGameCountdownStarted);
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
    enableTimerUpdates();
}

void QQuizLedDisplay::onGameCountdownStarted() {
    mAllowTimerUpdates = false;
    mLedDisplay->SetRGB(0xff, 0xff, 0x00);
    mLedDisplay->DisplayString("GOOD", LEDDisplay::kCenter, LEDDisplay::kDownScroll, LEDDisplay::kVariable);
    QTimer::singleShot(1000, this, SLOT(onGameStartedPartTwo()));
}

void QQuizLedDisplay::onGameStartedPartTwo() {
    mLedDisplay->SetRGB(0xff, 0x33, 0x0);
    mLedDisplay->DisplayString("LUCK", LEDDisplay::kCenter, LEDDisplay::kUpScroll, LEDDisplay::kVariable);
    QTimer::singleShot(1000, this, SLOT(onGameStartedPartThree()));
}

void QQuizLedDisplay::onGameStartedPartThree() {
    mLedDisplay->SetRGB(0xff, 0x00, 0x00);
    QString name = mGameState->currentPlayerName();

    mLedDisplay->DisplayString((name != NULL) ? name.toUtf8().constData() : "", LEDDisplay::kCenter, LEDDisplay::kNoScroll, LEDDisplay::kVariable);
}

void QQuizLedDisplay::onGameFinished(int rank) {
    QString text;
    text.sprintf("#%d", rank);
    mLedDisplay->DisplayString(text.toUtf8().constData(), LEDDisplay::kCenter, LEDDisplay::kNoScroll, LEDDisplay::kMono);
    //showBannerText();
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
    mLedDisplay->DisplayX(LEDDisplay::kDupeCenter, LEDDisplay::kNoScroll);
    QTimer::singleShot(1000, this, SLOT(onWrongAnswerPartTwo()));
}

void QQuizLedDisplay::onCorrectAnswer() {
    mAllowTimerUpdates = false;
    mLedDisplay->SetRGB(0x0, 0xff, 0x0);
    mLedDisplay->DisplayCheckmark(LEDDisplay::kDupeCenter, LEDDisplay::kNoScroll);
    QTimer::singleShot(1000, this, SLOT(enableTimerUpdates()));
}

void QQuizLedDisplay::onWrongAnswerPartTwo() {
    mLedDisplay->DisplayString("5s Penalty", LEDDisplay::kLeft, LEDDisplay::kNoScroll, LEDDisplay::kVariable);
    QTimer::singleShot(1000, this, SLOT(enableTimerUpdates()));
}

void QQuizLedDisplay::showBannerText() {
    mLedDisplay->SetRGB(0x0, 0x0, 0xff);

    mLedDisplay->DisplayString("Come play the Anne & Shawn Quiz Game!!! Only $2!!! Win a dinner and a movie night!!!", LEDDisplay::kLeft, LEDDisplay::kLeftScroll, LEDDisplay::kVariable);
}

void QQuizLedDisplay::enableTimerUpdates() {
    mLedDisplay->SetRGB(0x0, 0x0, 0xff);
    mAllowTimerUpdates = true;
}
