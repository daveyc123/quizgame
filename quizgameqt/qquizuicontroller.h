#ifndef QQUIZUICONTROLLER_H
#define QQUIZUICONTROLLER_H

#include <QObject>
#include <qquizgamestate.h>

class QQuizUIController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString page READ page NOTIFY pageChanged)

public:
    QQuizUIController(QQuizGameState *gameState, QObject *parent = 0);

public slots:
    QString page();

    void setPage(QString page);

    void setQuestionPage();
    void startNewGame();

signals:
    void pageChanged(QString page);

private:
    QString mPage;
    QQuizGameState* mGameState;

};

#endif // QQUIZUICONTROLLER_H
