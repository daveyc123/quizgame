#ifndef QQUIZUICONTROLLER_H
#define QQUIZUICONTROLLER_H

#include <QObject>
#include <qquizgamestate.h>
#include <QString>
#include <qquiztimecounter.h>
#include <qquizbuttonthread.h>
#include <QQmlContext>

class QQuizUIController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString page READ page NOTIFY pageChanged)
    Q_PROPERTY(QString questionText READ questionText NOTIFY newQuestionText)
    Q_PROPERTY(QString questionCount READ questionCount NOTIFY newQuestionCount)
    Q_PROPERTY(QString timerText READ timerText NOTIFY newTimerText)
    Q_PROPERTY(QString topScore READ topScore NOTIFY newTopScore)
    Q_PROPERTY(QString topName READ topName NOTIFY newTopName)

public:
    QQuizUIController(QQuizGameState *gameState, QObject *parent = 0);
    ~QQuizUIController();

public slots:
    QString page();

    void setPage(QString page);
    void startNewGame(QString playerName);
    void countDownFinished();
    QString questionText();
    QString questionCount();
    QString timerText();
    QString topName();
    QString topScore();

    void shawnPressed();
    void annePressed();

private slots:
    void onGameStarted();
    void onNewQuestion(QuizQuestion* question, int questionCount);
    void onGameFinished();
    void onTimerFired(long time);
    void onButtonPressed(QString id);


signals:
    void pageChanged(QString page);
    void showCountDown();
    void newQuestionText(QString questionText);
    void newQuestionCount(QString questionCount);
    void newTimerText(QString timerText);
    void newTopScore(QString topScore);
    void newTopName(QString topName);

private:
    QString mPage;
    QQuizGameState* mGameState;
    QString mCurrentQuestionText;
    int mCurrentQuestionCount;
    QString mTimerText;
    QString mTopScore;
    QString mTopName;
    QString mPlayerName;

    QQuizButtonThread* mButtonThread;
};

#endif // QQUIZUICONTROLLER_H
