import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
import QtMultimedia 5.0

Item {
    anchors.fill: parent

    Rectangle {
        anchors.fill: parent
        color: "#FFFFFF"
    }

    Text {
        id: questionCount
        text: qsTr("Question ") + controller.questionCount + "/10"
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 20
        anchors.topMargin: 20
        font.family: "Rockwell"
        font.pointSize: 40
        font.capitalization: Font.AllUppercase
        color: "#000000"
    }

    Text {
        id: timerLeft
        text: controller.timerText.toString().split(".")[0]

        anchors.right: timerPeriod.right
        anchors.top: parent.top
        anchors.rightMargin: 5
        anchors.topMargin: 23

        font.family: "Segment7"
        font.pointSize: 50
        color: "#000000"
    }

    Text {
        id: timerPeriod
        text: "."

        anchors.right: timerRight.left
        anchors.top: parent.top
        anchors.rightMargin: 5
        anchors.topMargin: 23

        font.family: "DS-Digital"
        font.pointSize: 50
        color: "#000000"
    }

    Text {
        id: timerRight
        text: controller.timerText.toString().split(".")[1]

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 20
        anchors.topMargin: 23

        font.family: "Segment7"
        font.pointSize: 50
        color: "#000000"
    }

    Text {
        id: questionText
        text: controller.questionText
        wrapMode: Text.WordWrap
        width: parent.width * 0.9
        horizontalAlignment: Text.AlignHCenter
        anchors.centerIn: parent

        font.pointSize: 75
        font.family: "Rockwell"
        color: "#000000"
    }

    Image {
        id: mrsImage
        source: "mrs.png"
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.bottomMargin: 20
        anchors.leftMargin: 20

    }

    Image {
        id: mrImage
        source: "mr.png"
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.bottomMargin: 20
        anchors.rightMargin: 20
    }

    SoundEffect {
         id: correctAnswerSoundEffect
         source: "file:///home/pi/quizgamenfs/quizgameqt/ff-strike3.wav"
         muted: false
         volume: 1.0
         onPlayingChanged: {console.log("In theory I'm playing")}
         onLoadedChanged: {console.log("I'm loaded")}
    }

    FontLoader {
        source: "rockwell.ttf"
    }

    FontLoader {
        source: "timer2.otf"
    }

    FontLoader {
        source: "timer.ttf"
    }

    Connections {
        target: controller
        onNewQuestionText: {
            correctAnswerSoundEffect.play();
        }
    }

//    Audio {
//         id: correctAnswerSoundEffect
//         source: "file://Users/dcummings/Downloads/ff-strike3.wav"
//         muted: false
//         loops: SoundEffect.Infinite
//    }


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
