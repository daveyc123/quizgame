#ifndef QQUIZBUTTONTHREAD_H
#define QQUIZBUTTONTHREAD_H

#include <QObject>
#include <QThread>

class QQuizButtonThread : public QThread
{
    Q_OBJECT
public:
    explicit QQuizButtonThread(QObject *parent = 0);
    ~QQuizButtonThread();

signals:
    void buttonPressed(QString id);

protected:
    void run();

public:
    void addButton(int gpio, QString id);

private:
    void cleanup();
    int pinConfig(int pin, char *attr, char *value);
    void runAdafruitCode();

private:
    QList<int> gpios;
    QList<QString> ids;



};

#endif // QQUIZBUTTONTHREAD_H
