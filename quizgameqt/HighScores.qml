import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

Item {
    Rectangle {
        anchors.fill: parent
        color: "#FFFFFF"
    }

//    ListModel {
//        id: myModel
//        ListElement {
//            rank: 1
//            name: "Bill Smith"
//            number: "555 3264"
//        }
//        ListElement {
//            rank: 2
//            name: "John Brown"
//            number: "555 8426"
//        }
//        ListElement {
//            rank: 3
//            name: "Sam Wise"
//            number: "555 0473"
//        }
//    }

    Text {
        id: highScoresText
        text: "High Scores"

        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
    }

    ListView {
        anchors.top: highScoresText.bottom
        //anchors.bottom: parent.bottom
        anchors.topMargin: 10
        anchors.bottom: playerNameGroup.top
        anchors.left: parent.left
        anchors.right: parent.right

        model: resultsModel
        delegate: Text {
            text: name + ": " + parseInt(score)
        }
    }

    ColumnLayout {
        id: playerNameGroup
        Text {
            id: nameLabel
            text: "New Player: "
            anchors.verticalCenter: playerName.verticalCenter
            anchors.left: parent.left
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
        anchors.bottomMargin: 10
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
