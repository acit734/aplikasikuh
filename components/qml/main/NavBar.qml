import QtQuick 2.15
import QtQuick.Effects

Item {
    property string time: Qt.formatTime(new Date(), "hh:mm:ss")

    id: root
    width: parent.width
    height: 37.5
    z: 2
    Rectangle {
        id: mainBar
        anchors.fill: parent
        color: "#7AE582"

        Row {
            spacing: 5
            x: parent.width - width - 10
            y: parent.height/2 - height/2
            LayoutMirroring.enabled: true

            Text {
                text: "Aplikasikuh"
                font.pixelSize: 15
                color: "white"
            }

            Text {
                text: "|"
                font.weight: 15
                font.pixelSize: 15
                color: "white"
            }

            Text {
                text: root.time
                color: "white"
                font.pixelSize: 15
            }
        }
    }
    Timer {
        interval: 1000
        running: true
        repeat: true
        onTriggered: {
            root.time = Qt.formatTime(new Date(), "hh:mm:ss")
        }
    }

    MultiEffect {
        anchors.fill: mainBar
        source: mainBar
        shadowEnabled: true
        shadowColor: "black"
    }

}
