#include "qquizuicontroller.h"

/**
 * UI controller to manage the interactions between the model (C++)
 * and the UI (qml). UI should listen to signals emitted by the control
 * to update accordingly.
 *
 * @brief QQuizUIController::QQuizUIController
 * @param parent
 */
QQuizUIController::QQuizUIController(QObject *parent) :
    QObject(parent)
{
    setPage("questionPage");
}

void QQuizUIController::setPage(QString page) {
    mPage = page;
    emit pageChanged(page);
}

QString QQuizUIController::page() {
    return mPage;
}
