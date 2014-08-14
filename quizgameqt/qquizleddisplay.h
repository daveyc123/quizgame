#ifndef QQUIZLEDDISPLAY_H
#define QQUIZLEDDISPLAY_H

#include <QObject>
#include "defines.h"
#include <qquizgamestate.h>

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
    void onNewQuestion(QuizQuestion* question, int questionCount);
    void onGameFinished();
    void onTimerFired(long time);
    void onWrongAnswer();
    void onCorrectAnswer();
    void enableTimerUpdates();
    void onWrongAnswerPartTwo();

private:
    void showBannerText();

private:
    QQuizGameState *mGameState;
    LEDDisplay* mLedDisplay;
    bool mAllowTimerUpdates;

};

#endif // QQUIZLEDDISPLAY_H
