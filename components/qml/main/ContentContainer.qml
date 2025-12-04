import QtQuick 2.15
import QtQuick.Controls

Item {
    required property var content
    property bool disableScroll: false

    id: root
    x: 37.5
    y: 37.5
    width: parent.width - 37.5
    height: parent.height - 37.5

    Rectangle {
        anchors.fill: parent
        ScrollView {
            id: container
            anchors.fill: parent

            ScrollBar.horizontal: ScrollBar {
                policy: ScrollBar.AlwaysOff
            }

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AlwaysOff
            }
        }

        Binding {
            target: container.contentItem
            property: "boundsBehavior"
            value: Flickable.StopAtBounds
        }
    }

    onContentChanged: {
        container.contentChildren = [
            content
        ]
    }

    onDisableScrollChanged: {
        if (disableScroll) {
            container.contentItem.interactive = false
        } else {
            container.contentItem.interactive = true
        }
    }
}
