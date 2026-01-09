import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

pragma ComponentBehavior: Bound

Item {
    id: root
    width: parent.width
    height: parent.height
    z: 3

    Rectangle {
        id: rootRect
        anchors.fill: parent
        color: "black"

        Item {
            id: transitionScreen
            width: parent.width
            height: parent.height

            component TransitionBlock: Rectangle {
                id: self
                property string type
                property int rowId: 0
                property int columnId: 0

                ParallelAnimation {
                    id: fadeOutAnimation
                    PropertyAnimation {
                        target: self
                        property: "color"
                        to: "transparent"
                        duration: 100
                        easing.type: Easing.Linear
                    }
                    SequentialAnimation {
                        PauseAnimation {
                            duration: 10
                        }
                        ScriptAction {
                            script: {
                                for (let i = 1; i < children.length; ++i) children[i].kill()
                                if ((rowId === Math.ceil(transitionScreen.width/50) - 1) && (columnId === Math.ceil(transitionScreen.height/50) - 1)) {
                                    parent.requestDestroy()
                                }
                            }
                        }
                    }
                }

                function kill() {
                    blockMouseArea.enabled = false
                    fadeOutAnimation.start()
                }

                function requestDestroy() {
                    if (parent && typeof parent.requestDestroy === "function") { //qmllint disable
                        parent.requestDestroy() //qmllint disable
                    } else {
                        self.destroy()
                    }
                }

                color: "black"
                width: 50
                height: 50
                x: 0
                y: 0

                MouseArea {
                    id: blockMouseArea
                    anchors.fill: parent
                    enabled: true
                }

                Component.onCompleted: {
                    if (type === "top") {
                        if (rowId < Math.ceil(transitionScreen.width/50)) {
                            Qt.callLater(() => {
                                transitionBlockComponent.createObject(self, {
                                    rowId: rowId + 1,
                                    columnId: columnId,
                                    x: self.width,
                                    y: self.y,
                                    type: "top"
                                })
                            })
                            Qt.callLater(() => {
                                transitionBlockComponent.createObject(self, {
                                    rowId: rowId,
                                    columnId: columnId + 1,
                                    x: columnId >=1 ? self.x : 0,
                                    y: self.height,
                                    type: "below"
                                })
                            })
                        }
                    } else if (type === "below") {
                        if (columnId < Math.ceil(transitionScreen.height/50)) {
                            Qt.callLater(() => {
                                transitionBlockComponent.createObject(self, {
                                    rowId: rowId,
                                    columnId: columnId + 1,
                                    x: self.x,
                                    y: self.height,
                                    type: "below"
                                })
                            })
                        }
                    }
                }
            }

            Component {
                id: transitionBlockComponent
                TransitionBlock {}
            }

            TransitionBlock {
                id: mainBlock
                type: "top"
            }
        }

        Item {
            id: usernameFirstLogin
            enabled: false
            opacity: 0
            scale: 0.8
            x: parent.width/2
            y: parent.height/2 - 20
            
            ColumnLayout {
                id: inputColumn
                x: -width/2
                y: -height/2

                Text {
                    id: inputTitle
                    text: "Enter Username:"
                    color: "white"
                    font.pixelSize: 30
                    Layout.alignment: Qt.AlignHCenter
                }

                Rectangle {
                    id: inputRect
                    property string username: ""
                    property string storageLink: ""
                    Layout.alignment: Qt.AlignHCenter

                    Timer {
                        id: changeTitle
                        property int length
                        property int elapsed: 0
                        property int elapsedCount: 0
                        property string stagedString
                        property string targetString: "Enter Storage Link:"
                        property string randomChar: "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm1234567890 "

                        running: false
                        repeat: true
                        interval: 10

                        Component.onCompleted: {
                            length = inputTitle.text.length
                        }

                        onTriggered: {
                            if (targetString === stagedString) {
                                running = false
                                inputField.readOnly = false
                                inputField.maximumLength = 32767
                                inputField.echoMode = TextInput.PasswordEchoOnEdit
                                return
                            }

                            elapsed += interval

                            if (elapsed >= 30) {
                                elapsed = 0
                                if (length < targetString.length) length++
                                else {
                                    stagedString += targetString[elapsedCount++]
                                }
                            }
                            
                            let processed = stagedString
                            for (let i = elapsedCount; i < length; i++) {
                                processed += randomChar[Math.floor(Math.random() * (randomChar.length - 0))]
                            }

                            inputTitle.text = processed
                        }
                    }

                    TextField {
                        id: inputField
                        focus: true
                        anchors.fill: parent
                        font.pixelSize: 20
                        color: "white"
                        maximumLength: 10

                        background: Rectangle {
                            color: "black"
                        }
                        
                        Text {
                            id: inputtedUsernameText
                            visible: false
                            font.pixelSize: 20
                        }

                        onTextChanged: {
                            inputtedUsernameText.text = text
                            inputRect.width = inputtedUsernameText.width + 20
                            inputRect.height = inputtedUsernameText.height + 5
                            inputRect.x = inputColumn.width/2 - inputRect.width/2
                        }

                        onAccepted: {
                            if (inputRect.username === "") {
                                inputRect.username = text
                                text = ""
                                inputField.readOnly = true
                                changeTitle.start()
                            } else {
                                inputRect.storageLink = text
                                firstTimeCloseAnimation.start()
                                Backend.openingAnimation.insertUsername(inputRect.username, inputRect.storageLink); //qmllint disable
                            }
                        }
                    }
                }
            }
        }

        Item {
            id: startScreen
            anchors.fill: parent

            Text {
                id: startLetter
                text: "Δ"
                anchors.centerIn: parent
                color: "white"
                font.pixelSize: 200
                transform: Rotation {
                    id: startLetterRotation
                    axis {x: 0; y: 1; z: 0}
                    origin.x: startLetter.width/2
                    origin.y: startLetter.height/2
                    angle: -90
                }
            }

            Text {
                id: usernameText

                property string username

                text: ""
                color: "white"
                font.pixelSize: 30
                opacity: 0.0
                x: parent.width/2 - width/2
                y: parent.height/2 - height/2

                Timer {
                    id: usernameTextScramble

                    property int elapsed: 0
                    property int elapsedCount: 0
                    property string stagedText: ""
                    property string characters: "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm1234567890"

                    running: false
                    repeat: true
                    interval: 5
                    onTriggered: {
                        if (elapsed >= 50) {
                            if (stagedText === usernameText.username) {
                                running = false
                                return
                            }
                            stagedText += usernameText.username[elapsedCount++]
                            elapsed = 0
                        }

                        elapsed += interval

                        let processedText = stagedText

                        for (let i = elapsedCount; i < usernameText.username.length; ++i) {
                            processedText += characters[Math.floor(Math.random() * characters.length)]
                        }

                        usernameText.text = processedText
                    }
                }
            }
        }
        
        ParallelAnimation {
            id: startAnimation
            PropertyAnimation {
                target: startLetterRotation
                property: "angle"
                to: 0
                duration: 500
                easing.type: Easing.OutCubic
            }
            SequentialAnimation {
                PauseAnimation {
                    duration: 300
                }
                PropertyAnimation {
                    target: startLetterRotation
                    property: "angle"
                    to: 90
                    duration: 1000
                    easing.type: Easing.InCubic
                }
                ScriptAction {
                    script: {
                        if (!Backend.openingAnimation.checkUsernameAvailability()) { //qmllint disable
                            usernameFirstLogin.enabled = true
                            firstTimeOpenAnimation.start()
                        } else {
                            usernameText.username = Backend.openingAnimation.getUsername()
                            continueAnimation.start()
                        }
                    }
                }
            }
        }

        ParallelAnimation {
            id: firstTimeOpenAnimation

            PropertyAnimation {
                target: usernameFirstLogin
                properties: "opacity, scale"
                to: 1
                duration: 500
                easing.type: Easing.OutSine
            }
            PropertyAnimation {
                target: usernameFirstLogin
                property: "y"
                to: usernameFirstLogin.y + 20
                duration: 500
                easing.type: Easing.OutSine
            }
        }

        ParallelAnimation {
            id: firstTimeCloseAnimation

            PropertyAnimation {
                target: usernameFirstLogin
                property: "opacity"
                to: 0
                duration: 500
                easing.type: Easing.OutSine
            }
            PropertyAnimation {
                target: usernameFirstLogin
                property: "scale"
                to: 0.8
                duration: 500
                easing.type: Easing.OutSine
            }
            PropertyAnimation {
                target: usernameFirstLogin
                property: "y"
                to: usernameFirstLogin.y - 20
                duration: 500
                easing.type: Easing.OutSine
            }
            SequentialAnimation {
                PauseAnimation {
                    duration: 500
                }
                ScriptAction {
                    script: {
                        usernameText.username = Backend.openingAnimation.getUsername()
                        continueAnimation.start()
                    }
                }
            }
        }

        ParallelAnimation {
            id: continueAnimation
            ScriptAction {
                script: {
                    startLetter.text = "Selamat Datang"
                    startLetterRotation.angle = -90
                    startLetter.font.pixelSize = 50
                }
            }
            PropertyAnimation {
                target: startLetterRotation
                property: "angle"
                to: 0
                duration: 1000
                easing.type: Easing.OutCubic
            }
            SequentialAnimation {
                PauseAnimation {
                    duration: 100
                }
                ParallelAnimation {
                    PropertyAnimation {
                        target: startLetter
                        property: "scale"
                        to: 1.2
                        duration: 1000
                        easing.type: Easing.InOutSine
                    }
                    PropertyAnimation {
                        target: startLetter
                        property: "opacity"
                        to: 0
                        duration: 1000
                        easing.type: Easing.InSine
                    }
                    SequentialAnimation {
                        PauseAnimation {
                            duration: 500
                        }
                        ParallelAnimation{
                            PropertyAnimation {
                                target: usernameText
                                property: "opacity"
                                to: 1
                                duration: 500
                                easing.type: Easing.InSine
                            }
                            ScriptAction {
                                script: {
                                    usernameTextScramble.start()
                                }
                            }
                            SequentialAnimation {
                                PauseAnimation {
                                    duration: 1500
                                }
                                ParallelAnimation {
                                    ScriptAction {
                                        script: {
                                            rootRect.color = "transparent"
                                            mainBlock.kill()
                                        }
                                    }
                                    PropertyAnimation {
                                        target: usernameText
                                        property: "opacity"
                                        to: 0.0
                                        duration: 500
                                        easing.type: Easing.InOutSine
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        Component.onCompleted: {
            startAnimation.start()
        }
    }
}