#ifndef QQUIZRESULTS_H
#define QQUIZRESULTS_H

#include <QObject>
#include <qquizresult.h>

class QQuizResults : public QObject
{
    Q_OBJECT
public:
    explicit QQuizResults(QString resultsFile, QObject *parent = 0);

public slots:
    QList<QQuizResult*> getResults();
    void clearScores();

public:
    void addResult(QQuizResult *);

private:
    void serialize();
    void addResult(QQuizResult *, bool);
private:
    QList<QQuizResult*> mResults;
    QString mResultsFile;


};

#endif // QQUIZRESULTS_H
