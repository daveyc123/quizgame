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
    QCommandLineOption questionsOption(QStringList() << "q" << "questions", QCoreApplication::translate("main", "Specifiy a json file containing the list of questions."), QCoreApplication::translate("main", "questions"));
    parser.addOption(questionsOption);

    QCommandLineOption resultsOption(QStringList() << "r" << "results", QCoreApplication::translate("main", "Specifiy a json file containing the list of previous results (high scores)."), QCoreApplication::translate("main", "results"));
    parser.addOption(resultsOption);

    parser.process(app);
    QString questionsFile = parser.value(questionsOption);
    QString resultsFile = parser.value(resultsOption);

    // Seed random
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    QQuizQuestions quizQuestions(questionsFile);
    QQuizResults quizResults(resultsFile);
    QQuizGameState quizGameState(&quizQuestions, &quizResults);
    QQuizUIController quizUIController(&quizGameState);

    QQmlApplicationEngine engine;
    quizUIController.setQmlContext(engine.rootContext());
    engine.rootContext()->setContextProperty("controller", &quizUIController);

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    return app.exec();
}
