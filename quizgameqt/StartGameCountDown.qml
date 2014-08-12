import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
import QtMultimedia 5.0

Item {
    anchors.fill: parent

    property int counter: 3

    Rectangle {
        anchors.fill: parent
        color: "#FFFFFF"
    }

    Timer {
        id: timer
        interval: 1000
        running: false
        repeat: true
        onTriggered: {
            if (--counter == 0) {
                timer.running = false;
                controller.countDownFinished();
            }
        }
    }

    Text {
        id: countDownText
        text: counter
        anchors.centerIn: parent
        font.pointSize: 300
        font.bold: true
        font.capitalization: Font.AllUppercase
        color: "#000000"
    }

    Connections {
        target: controller
        onShowCountDown: {
            counter = 3;
            timer.running = true;
        }
    }

}
