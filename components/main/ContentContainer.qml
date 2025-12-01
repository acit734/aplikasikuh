import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Effects

Item {
    required property var content

    x: 37.5
    y: 37.5
    width: parent.width - 37.5
    height: parent.height - 37.5

    Rectangle {
        id: container
        anchors.fill: parent
        color: "white"
    }

    Component.onCompleted: {
        content.parent = container
        content.anchors.fill = container
    }
}
