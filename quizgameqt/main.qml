import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0

ApplicationWindow {
    width: 360
    height: 360
    visible: true

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: highScorePage
    }

    Component {
        id: highScorePage
        Item {
            HighScores {}
            anchors.fill: parent
        }
    }

    Component {
        id: questionPage
        Question {}
    }

    Connections {
        target: controller
         // don't use eval, eh? well it converts an id to an object pretty nicely. if you read this and know a better way, hit me up at david.a.cummings@gmail.com
        onPageChanged: stackView.push(eval(controller.page))
    }

}
