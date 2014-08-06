#ifndef QQUIZUICONTROLLER_H
#define QQUIZUICONTROLLER_H

#include <QObject>
#include <qquizgamestate.h>
#include <QString>
#include <qquiztimecounter.h>

class QQuizUIController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString page READ page NOTIFY pageChanged)
    Q_PROPERTY(QString questionText READ questionText NOTIFY newQuestionText)
    Q_PROPERTY(QString questionCount READ questionCount NOTIFY newQuestionCount)
    Q_PROPERTY(QString timerText READ timerText NOTIFY newTimerText)

public:
    QQuizUIController(QQuizGameState *gameState, QObject *parent = 0);

public slots:
    QString page();

    void setPage(QString page);
    void startNewGame(QString playerName);
    QString questionText();
    QString questionCount();
    QString timerText();

    void shawnPressed();
    void annePressed();

private slots:
    void onGameStarted();
    void onNewQuestion(QuizQuestion* question, int questionCount);
    void onGameFinished();
    void onTimerFired(long time);

signals:
    void pageChanged(QString page);
    void newQuestionText(QString questionText);
    void newQuestionCount(QString questionCount);
    void newTimerText(QString timerText);

private:
    QString mPage;
    QQuizGameState* mGameState;
    QString mCurrentQuestionText;
    int mCurrentQuestionCount;
    QString mTimerText;

};

#endif // QQUIZUICONTROLLER_H
