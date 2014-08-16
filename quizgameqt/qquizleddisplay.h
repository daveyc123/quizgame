#ifndef QQUIZLEDDISPLAY_H
#define QQUIZLEDDISPLAY_H

#include <QObject>
#include "defines.h"
#include <qquizgamestate.h>
#include <QMutex>

#ifdef BUILD_FOR_PI
#include "thread.h"
#include "led-matrix.h"
#include "led-display.h"
#include "text.h"

#include <assert.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <time.h>
#endif //BUILD_FOR_PI

class QQuizLedDisplay : public QObject
{
    Q_OBJECT
public:
    explicit QQuizLedDisplay(QQuizGameState *gameState, QObject *parent = 0);
    ~QQuizLedDisplay();

private slots:
    void onGameStarted();
    void onGameCountdownStarted();
    void onGameFinished(int rank);
    void onTimerFired(long time);
    void onWrongAnswer();
    void onCorrectAnswer();

private:
    void showBannerText();
    void runCountdown();
    void runCorrectAnswer();
    void runWrongAnswer();
    void runWinner();
    void runLoser(int rank);

private:
    QQuizGameState *mGameState;
    LEDDisplay* mLedDisplay;
    bool mAllowTimerUpdates;
    int mCurrentRank;
    QMutex displayMutex;

};

#endif // QQUIZLEDDISPLAY_H
