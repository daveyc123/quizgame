import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

Item {
    Image {
         id: background
         anchors.fill: parent
         source: "mrandmrs.jpg"
         fillMode: Image.PreserveAspectCrop
     }

    Text {
        id: highScoresText
        text: "Top Score: " + controller.topName + " (" + controller.topScore + "s)"
        font.bold: true
        font.pointSize: 30
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
    }

    ColumnLayout {
        id: playerNameGroup
        Text {
            id: nameLabel
            text: "New Player: "
            anchors.verticalCenter: playerName.verticalCenter
            anchors.left: parent.left
            font.bold: true
            font.pointSize: 20
        }

        TextField {
            id: playerName
            focus: true
            anchors.left: nameLabel.right
            anchors.leftMargin: 5
            anchors.right: parent.right
            onAccepted: {
                if (playerName.text != null) {
                    controller.startNewGame(playerName.text);
                }
            }
        }

        width: parent.width / 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
    }

//    Button {
//        id: startGameButton
//        text: qsTr("Start Game")
//        onClicked: {
//            if (playerName.text != null) {
//                controller.startNewGame(playerName.text);
//            }
//        }

//        anchors.bottom:
//        anchors.bottomMargin: 10
//        anchors.horizontalCenter: parent.horizontalCenter
//    }

    anchors.fill: parent
}
