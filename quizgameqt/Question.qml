import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
import QtMultimedia 5.0

Item {
    anchors.fill: parent

    Rectangle {
        anchors.fill: parent
        color: "#061275"
    }

    FontLoader {
        source: "jeopardy.ttf"
    }

    FontLoader {
        source: "timer2.otf"
    }

    FontLoader {
        source: "timer.ttf"
    }

    Text {
        id: questionCount
        text: qsTr("Question ") + controller.questionCount
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 5
        anchors.topMargin: 5
        font.family: "Korinna"
        font.pointSize: 40
        font.capitalization: Font.AllUppercase
        color: "#FFFFFF"
    }

    Text {
        id: timerLeft
        text: controller.timerText.toString().split(".")[0]

        anchors.right: timerPeriod.right
        anchors.top: parent.top
        anchors.rightMargin: 5
        anchors.topMargin: 8

        font.family: "Segment7"
        font.pointSize: 50
        color: "#FFFFFF"
    }

    Text {
        id: timerPeriod
        text: "."

        anchors.right: timerRight.left
        anchors.top: parent.top
        anchors.rightMargin: 5
        anchors.topMargin: 8

        font.family: "DS-Digital"
        font.pointSize: 50
        color: "#FFFFFF"
    }

    Text {
        id: timerRight
        text: controller.timerText.toString().split(".")[1]

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 5
        anchors.topMargin: 8

        font.family: "Segment7"
        font.pointSize: 50
        color: "#FFFFFF"
    }

    Text {
        id: questionText
        text: controller.questionText
        wrapMode: Text.WordWrap
        width: parent.width * 0.9
        horizontalAlignment: Text.AlignHCenter
        anchors.centerIn: parent

        font.capitalization: Font.AllUppercase
        font.family: "Korinna"
        font.pointSize: 40
        color: "#FFFFFF"
    }

    Button {
        text: "Anne"
        onClicked: {correctAnswerSoundEffect.play(); controller.annePressed(); console.log(correctAnswerSoundEffect.status == SoundEffect.Ready); console.log(correctAnswerSoundEffect.volume)}
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

    SoundEffect {
         id: correctAnswerSoundEffect
         source: "file:///Users/dcummings/Downloads/ff-zero2.wav"
         muted: false
         loops: SoundEffect.Infinite
         onPlayingChanged: {console.log("In theory I'm playing")}
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
