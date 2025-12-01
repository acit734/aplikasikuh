import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Effects

Item {
    required property var target

    id: root
    height: parent.height
    width: 37.5
    z: 2

    Rectangle {
        id: main
        anchors.fill: parent
        color: "#7AE582"

        Rectangle {
            id: button
            x: 9.75
            y: 9.75
            width: 17.5
            height: 17.5
            color: "transparent"

            border.color: "white"
            border.width: 3
            radius: 17.5

            Behavior on scale {
                PropertyAnimation {
                    easing.type: Easing.InOutQuint
                }
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onContainsMouseChanged: {
                    if (containsMouse) parent.scale = 1.4
                    else parent.scale = 1.0
                }
                onClicked: {
                    enabled = false
                    openSidebarAnimation.start()
                }
            }
        }
    }

    MultiEffect {
        anchors.fill: main
        source: main
        shadowEnabled: true
        shadowColor: "black"
    }

    Item {
        property var target: root.target

        id: hoverEffect
        opacity: 0.0
        x: target.x
        y: target.y
        width: target.width
        height: target.height
        z: 0

        Component.onCompleted: {
            hoverEffect.parent = root.parent
        }

        Behavior on opacity {
            PropertyAnimation { duration: 1000 }
        }

        MultiEffect {
            anchors.fill: parent
            source: root.target
            blurEnabled: true
            blurMax: 64
            blur: 1.0
        }
        Rectangle {
            anchors.fill: parent
            color: "black"
            opacity: 0.4
        }
    }

    SequentialAnimation {
        id: openSidebarAnimation

        PropertyAnimation {
            target: button
            property: "y"
            to: main.height/2 - button.height/2
            duration: 1000
            easing.type: Easing.InOutQuint
        }
        ParallelAnimation {
            PropertyAnimation {
                target: button
                property: "scale"
                to: 1.5
                duration: 1000
                easing.type: Easing.InBack
            }
            PropertyAnimation {
                target: button
                property: "opacity"
                to: 0.0
                duration: 1000
                easing.type: Easing.InExpo
            }
            SequentialAnimation {
                PauseAnimation {
                    duration: 800
                }
                ParallelAnimation {
                    PropertyAnimation {
                        target: hoverEffect
                        property: "opacity"
                        to: 1.0
                    }
                    PropertyAnimation {
                        target: root
                        property: "width"
                        to: 200
                        duration: 1000
                        easing.type: Easing.OutExpo
                    }
                }
            }
        }
    }
}
