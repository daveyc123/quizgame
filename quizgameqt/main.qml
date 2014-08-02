import QtQuick 2.2
import QtQuick.Controls 1.1

ApplicationWindow {
    width: 360
    height: 360
    visible: true

    Text {
        anchors.centerIn: parent
        text: parseInt(quizTimeCounter.elapsedTime)
    }

    Button {
        text: "Reset clock"
        onClicked: quizTimeCounter.start()
    }

    Button {
        text: "Stop clock"
        onClicked: quizTimeCounter.stop();
    }
}


