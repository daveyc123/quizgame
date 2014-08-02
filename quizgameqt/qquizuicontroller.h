#ifndef QQUIZUICONTROLLER_H
#define QQUIZUICONTROLLER_H

#include <QObject>

class QQuizUIController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString page READ page NOTIFY pageChanged)

public:
    QQuizUIController(QObject *parent = 0);

    QString page();

signals:
    void pageChanged(QString page);

private:
    void setPage(QString page);

    QString mPage;

};

#endif // QQUIZUICONTROLLER_H
