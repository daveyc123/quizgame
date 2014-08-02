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
        initialItem: questionPage
    }

    Component {
        id: highScorePage
        Item {
            HighScores {}
        }
    }

    Component {
        id: questionPage
        Question {}
    }

    Connections {
        target: controller
        onPageChanged: stackView.push(controller.page)
    }

}
