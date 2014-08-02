import QtQuick 2.0
import QtQuick.Controls 1.1

Item {
    Rectangle {
        width: parent.width
        height: parent.height
        color: "#FFFFFF"
    }

    ListModel {
        id: myModel
        ListElement {
            name: "Bill Smith"
            number: "555 3264"
        }
        ListElement {
            name: "John Brown"
            number: "555 8426"
        }
        ListElement {
            name: "Sam Wise"
            number: "555 0473"
        }
    }

    ListView {
        width: 180; height: 200

        model: myModel
        delegate: Text {
            text: name + ": " + number
        }
    }


}
