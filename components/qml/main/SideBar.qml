import QtQuick 2.15
import QtQuick.Effects

Item {
    required property var target
    property string characters: "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm1234567890"

    id: root
    height: parent.height
    width: 37.5
    z: 2

    Rectangle {
        id: main
        anchors.fill: parent
        color: "#7AE582"

        Item {
            id: sideBarContents
            opacity: 0.0
            anchors.fill: parent

            Text {
                id: titleText
                property string originalText: "Aplikasikuh"
                property string stagedText: ""
                property string generatedText: ""

                x: parent.width/2 - width/2
                y: 25

                text: generatedText
                color: "white"
                font.pixelSize: 20
                font.bold: true

                Timer {
                    id: titleTextRandomizer

                    property real elapsed: 0
                    property string characters: root.characters
                    property real index: 0

                    interval: 10
                    repeat: true
                    running: false

                    onTriggered: {
                        if (index === 11) {
                            running = false
                            return
                        }

                        elapsed += interval
                        if (elapsed >= 100) {
                            elapsed = 0
                            titleText.stagedText += titleText.originalText[index++]
                        }

                        let genText = ""

                        for (let i = 0; i < 11 - index; i++) {
                            genText += characters[Math.floor(Math.random() * 62)]
                        }

                        titleText.generatedText =  titleText.stagedText + genText
                    }
                }
            }

            Column {
                y: 75
                width: sideBarContents.width

                component Selection: Rectangle {
                    id: selectionLabel
                    property string label

                    width: parent.width
                    height: 50
                    color: "transparent"

                    Item {
                        id: upDownBorder
                        opacity: 0.0
                        anchors.fill: parent

                        Behavior on opacity {
                            PropertyAnimation { duration: 200; easing.type: Easing.InOutSine; }
                        }

                        Rectangle {
                            height: 2
                            width: parent.width
                            color: "white"
                        }
                        Rectangle {
                            y: parent.height - height
                            height: 2
                            width: parent.width
                            color: "white"
                        }
                    }

                    Text {
                        id: hoverPointer
                        y: parent.height/2 - height/2
                        x: 17
                        color: "white"
                        font.pixelSize: 15
                    }

                    Text {
                        id: selectionText
                        text: parent.label
                        y: parent.height/2 - height/2
                        x: 30
                        color: "white"
                        font.pixelSize: 15
                    }

                    MouseArea {
                        id: pointerAnimation
                        property string characters: "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm1234567890"

                        hoverEnabled: true
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onContainsMouseChanged: {
                            pointerAnimate.start()
                            if (containsMouse) upDownBorder.opacity = 1.0
                            else upDownBorder.opacity = 0.0
                        }

                        Timer {
                            id: pointerAnimate
                            property real elapsed: 0

                            interval: 10
                            repeat: true
                            running: false
                            
                            onTriggered: {
                                if (elapsed >= 75) {
                                    if (pointerAnimation.containsMouse) hoverPointer.text = ">"
                                    else hoverPointer.text = ""
                                    elapsed = 0
                                    running = false
                                    return
                                }

                                hoverPointer.text = pointerAnimation.characters[Math.floor(Math.random() * 62)]

                                elapsed += interval
                            }
                        }
                    }
                }

                Selection { label: "Home" }
                Selection { label: "Selection 1" }
                Selection { label: "Selection 2" }
                Selection { label: "Selection 3" }
                Selection { label: "Selection 4" }
            }
        }

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
                    PropertyAnimation {
                        target: sideBarContents
                        property: "opacity"
                        to: 1.0
                        duration: 1000
                        easing.type: Easing.InOutExpo
                    }
                    ScriptAction {
                        script: {
                            titleTextRandomizer.start()
                        }
                    }
                }
            }
        }
    }
}
