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
        text: qsTr("Question ") + controller.questionCount
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
        text: controller.questionText
        wrapMode: Text.WordWrap
        width: parent.width * 0.9
        horizontalAlignment: Text.AlignHCenter
        anchors.centerIn: parent
    }

    Button {
        text: "Anne"
        onClicked: controller.annePressed();
        anchors.bottom : parent.bottom
        anchors.left: parent.left
        anchors.bottomMargin: 20
        anchors.leftMargin: 20
    }

    Button {
        text: "Shawn"
        onClicked: controller.shawnPressed();
        anchors.right: parent.right;
        anchors.bottom : parent.bottom;
        anchors.bottomMargin: 20
        anchors.rightMargin: 20
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

//    Button {
//        text: "Stop clock"
//        onClicked: controller.setPage("highScorePage");
//        anchors.left: parent.left
//        anchors.bottom : parent.bottom
//    }


}
