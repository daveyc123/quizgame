#ifndef QQUIZUICONTROLLER_H
#define QQUIZUICONTROLLER_H

#include <QObject>
#include <qquizgamestate.h>
#include <QString>

class QQuizUIController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString page READ page NOTIFY pageChanged)
    Q_PROPERTY(QString questionText READ questionText NOTIFY newQuestionText)
    Q_PROPERTY(QString questionCount READ questionCount NOTIFY newQuestionCount)

public:
    QQuizUIController(QQuizGameState *gameState, QObject *parent = 0);

public slots:
    QString page();

    void setPage(QString page);
    void startNewGame(QString playerName);
    QString questionText();
    QString questionCount();
    void shawnPressed();
    void annePressed();

private slots:
    void onGameStarted();
    void onNewQuestion(QuizQuestion* question, int questionCount);
    void onGameFinished();

signals:
    void pageChanged(QString page);
    void newQuestionText(QString questionText);
    void newQuestionCount(QString questionCount);

private:
    QString mPage;
    QQuizGameState* mGameState;
    QString mCurrentQuestionText;
    int mCurrentQuestionCount;

};

#endif // QQUIZUICONTROLLER_H
