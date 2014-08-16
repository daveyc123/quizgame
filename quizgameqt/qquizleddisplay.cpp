#include "qquizleddisplay.h"
#include <QtDebug>
#include <QtConcurrent/QtConcurrent>

QQuizLedDisplay::QQuizLedDisplay(QQuizGameState *gameState, QObject *parent) :
    QObject(parent)
{
    mGameState = gameState;
    mLedDisplay = new LEDDisplay();

    QObject::connect(gameState, &QQuizGameState::gameCountdownStarted, this, &QQuizLedDisplay::onGameCountdownStarted);
    QObject::connect(gameState, &QQuizGameState::gameStarted, this, &QQuizLedDisplay::onGameStarted);
    QObject::connect(gameState->timeCounter(), &QQuizTimeCounter::updated, this, &QQuizLedDisplay::onTimerFired);
    QObject::connect(gameState, &QQuizGameState::wrongAnswer, this, &QQuizLedDisplay::onWrongAnswer);
    QObject::connect(gameState, &QQuizGameState::correctAnswer, this, &QQuizLedDisplay::onCorrectAnswer);
    QObject::connect(gameState, &QQuizGameState::gameFinished, this, &QQuizLedDisplay::onGameFinished);

    showBannerText();
}

QQuizLedDisplay::~QQuizLedDisplay() {
    delete mLedDisplay;
}

void QQuizLedDisplay::onGameCountdownStarted() {
   QtConcurrent::run(this, &QQuizLedDisplay::runCountdown);
}

void QQuizLedDisplay::onGameStarted() {
    // nothing?
}

void QQuizLedDisplay::onWrongAnswer() {
    QtConcurrent::run(this, &QQuizLedDisplay::runWrongAnswer);
}

void QQuizLedDisplay::onCorrectAnswer() {
   QtConcurrent::run(this, &QQuizLedDisplay::runCorrectAnswer);
}

void QQuizLedDisplay::onTimerFired(long time) {
    if (displayMutex.tryLock()) {
        mLedDisplay->SetRGB(0x0, 0x0, 0xff);
        mLedDisplay->DisplayTimer((unsigned)time);
        displayMutex.unlock();
    }
}

void QQuizLedDisplay::onGameFinished(int rank) {
    if (rank == 1) {
        QtConcurrent::run(this, &QQuizLedDisplay::runWinner);
    } else {
        QtConcurrent::run(this, &QQuizLedDisplay::runLoser, rank);
    }
}

void QQuizLedDisplay::runCountdown() {
    displayMutex.lock();

    mLedDisplay->SetRGB(0xFF, 0x0, 0x0);
    mLedDisplay->DisplayString("GOOD", LEDDisplay::kCenter, LEDDisplay::kDownScroll, LEDDisplay::kVariable);
    usleep(600000);


    mLedDisplay->SetRGB(0x0, 0xFF, 0x0);
    mLedDisplay->DisplayString("LUCK", LEDDisplay::kCenter, LEDDisplay::kUpScroll, LEDDisplay::kVariable);
    usleep(600000);

    QString name;
    name.sprintf("                %s!", mGameState->currentPlayerName().toUtf8().constData());
    mLedDisplay->SetRGB(0x0, 0x0, 0xFF);
    mLedDisplay->DisplayString((name != NULL) ? name.toUtf8().constData() : "", LEDDisplay::kLeft, LEDDisplay::kLeftScroll, LEDDisplay::kVariable);
    usleep(1800000);

    displayMutex.unlock();
}

void QQuizLedDisplay::runWrongAnswer() {
    if (displayMutex.tryLock(100)) {
        mLedDisplay->SetRGB(0xff, 0x0, 0x0);
        mLedDisplay->DisplayX(LEDDisplay::kDupeCenter, LEDDisplay::kNoScroll);
        usleep(1000000);
        displayMutex.unlock();
    }
}

void QQuizLedDisplay::runCorrectAnswer() {
    if (displayMutex.tryLock(100)) {
        mLedDisplay->SetRGB(0x0, 0xff, 0x0);
        mLedDisplay->DisplayCheckmark(LEDDisplay::kDupeCenter, LEDDisplay::kNoScroll);
        usleep(1000000);
        displayMutex.unlock();
    }
}

void QQuizLedDisplay::runLoser(int rank) {
    displayMutex.lock();

    mLedDisplay->SetRGB(0xff, 0xff, 0x00);
    QString text;
    text.sprintf("YOU");
    mLedDisplay->DisplayString(text.toUtf8().constData(), LEDDisplay::kCenter, LEDDisplay::kNoScroll, LEDDisplay::kVariable);
    usleep(1500000);

    text.sprintf("FINISHED");
    mLedDisplay->DisplayString(text.toUtf8().constData(), LEDDisplay::kCenter, LEDDisplay::kNoScroll, LEDDisplay::kVariable);
    usleep(1500000);

    text.sprintf("# %d", rank);
    mLedDisplay->DisplayString(text.toUtf8().constData(), LEDDisplay::kCenter, LEDDisplay::kNoScroll, LEDDisplay::kMono);
    usleep(1500000);

    displayMutex.unlock();

    showBannerText();
}

void QQuizLedDisplay::runWinner() {
    displayMutex.lock();
    const char* str = "Top Score!";
    unsigned str_width;
    Font font("Oswald-Bold.ttf", 15);
    RGBCanvas* canvases[2];
    Pen pen;
    unsigned char r = 0xff;
    unsigned char g = 0xff;
    unsigned char b = 0;
    int i = 0;

    str_width = font.GetWidth(str);
    pen.x = mLedDisplay->GetWidth() / 2 - str_width / 2;
    pen.y = mLedDisplay->GetHeight() - 2;
    canvases[0] = new RGBCanvas(mLedDisplay->GetWidth(), mLedDisplay->GetHeight());
    canvases[1] = new RGBCanvas(mLedDisplay->GetWidth(), mLedDisplay->GetHeight());
    font.PaintString(str, canvases[0], &pen, r, g, b);
    canvases[1]->Fill(r, g, b);
    font.PaintString(str, canvases[1], &pen, 0, 0, 0);

    while(i<25) { // 5s
      mLedDisplay->SwapCanvas(canvases[i++ % 2], LEDDisplay::kLeft, LEDDisplay::kNoScroll, false);
      usleep(200000);
    }

    displayMutex.unlock();

    showBannerText();
}

void QQuizLedDisplay::showBannerText() {
    displayMutex.lock();
    mLedDisplay->SetRGB(0x0, 0x0, 0xff);
    mLedDisplay->DisplayString("                    Play the Anne & Shawn Quiz Game!!! Only $2!!! Win a dinner and a movie night!!!", LEDDisplay::kLeft, LEDDisplay::kLeftScroll, LEDDisplay::kVariable);
    displayMutex.unlock();
}

