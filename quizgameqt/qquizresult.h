#ifndef QQUIZRESULT_H
#define QQUIZRESULT_H

#include <QObject>

class QQuizResult : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(long score READ score NOTIFY scoreChanged)

public:
    explicit QQuizResult(QString name, long score, QObject *parent = 0);

public slots:
    QString name();
    long score();

signals:
    void nameChanged();
    void scoreChanged();

private:
    QString mName;
    long mScore;


};

#endif // QQUIZRESULT_H
