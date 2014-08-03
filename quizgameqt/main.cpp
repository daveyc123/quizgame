#include <QApplication>
#include <QQmlApplicationEngine>
#include <qquiztimecounter.h>
#include <QQmlContext>
#include <qquizquestions.h>
#include <QCommandLineParser>
#include <QtDebug>
#include <qquizuicontroller.h>
#include <QTime>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("quizgame");
    app.setApplicationDisplayName("Quiz Game");
    app.setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::translate("main", "Amazing quiz game!."));
    QCommandLineOption questionsOption(QStringList() << "q" << "questions", QCoreApplication::translate("main", "Specifiy a json file containing the list of questions."), QCoreApplication::translate("main", "address"));
    parser.addOption(questionsOption);

    parser.process(app);
    QString questionsFile = parser.value(questionsOption);

    // Seed random
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    QQuizTimeCounter quizTimeCounter;
    QQuizQuestions quizQuestions(questionsFile);
    QQuizGameState quizGameState(&quizQuestions);
    QQuizUIController quizUIController(&quizGameState);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("quizTimeCounter", &quizTimeCounter);
    engine.rootContext()->setContextProperty("controller", &quizUIController);

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    return app.exec();
}
