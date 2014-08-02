TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp \
    qquiztimecounter.cpp \
    qquizquestions.cpp \
    quizquestion.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    qquiztimecounter.h \
    qquizquestions.h \
    quizquestion.h

OTHER_FILES += \
    questions.json


