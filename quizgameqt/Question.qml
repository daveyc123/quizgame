import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0

Item {
    anchors.fill: parent

    Rectangle {
        anchors.fill: parent
        color: "#FFFFFF"
    }

    Text {
        id: questionCount
        text: qsTr("Question 1")
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 5
        anchors.topMargin: 5
    }

    Text {
        id: timer
        text: parseInt(quizTimeCounter.elapsedTime)

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 5
        anchors.topMargin: 5
    }

    Text {
        id: questionText
        text: qsTr("Have ever had the feeling there's a zamp in the lamp?")
        wrapMode: Text.WordWrap
        width: parent.width * 0.9
        horizontalAlignment: Text.AlignHCenter
        anchors.centerIn: parent
    }

//    Button {
//        text: "Reset clock"
//        onClicked: quizTimeCounter.start()
//        anchors.left: parent.left
//        anchors.bottom : parent.bottom;
//    }

//    Button {
//        text: "Stop clock"
//        onClicked: quizTimeCounter.stop();
//        anchors.right: parent.right;
//        anchors.top : parent.bottom;
//    }

    Button {
        text: "Stop clock"
        onClicked: controller.setPage("highScorePage");
        anchors.left: parent.left
        anchors.bottom : parent.bottom
    }


}
