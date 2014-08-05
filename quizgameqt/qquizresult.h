#ifndef QQUIZRESULT_H
#define QQUIZRESULT_H

#include <QObject>

class QQuizResult : public QObject
{
    Q_OBJECT
public:
    explicit QQuizResult(QString name, long score, QObject *parent = 0);

public slots:
    QString getName();
    long getScore();

private:
    QString mName;
    long mScore;


};

#endif // QQUIZRESULT_H
