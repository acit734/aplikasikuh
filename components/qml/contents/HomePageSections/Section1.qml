import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Effects
import QtQuick.Controls

Rectangle {
    property string time
    id: sec1Root
    width: root.width
    height: root.height
    color: "white"

    Item {
        anchors.centerIn: parent

        ColumnLayout {
            x: -width/2
            y: -height/2
            spacing: 10
            Item {
                width: 200
                height: 200
                Layout.alignment: Qt.AlignHCenter

                Rectangle {
                    x: parent.width/2 - width/2
                    y: parent.height/2 - height/2
                    width: 100
                    height: 100
                    radius: 50
                    border.color: "black"
                    border.width: 6
                    color: "transparent"

                    transform: [
                        Rotation {
                            id: rot1Circ3
                            origin { x: 50; y: 50 }
                            axis { x: 1; y: 0; z: 0 }
                            angle: 0
                            PropertyAnimation on angle {
                                to: rot1Circ3.angle + 360
                                running: true
                                duration: Math.floor(Math.random()*(3000-1000+1))+1000
                                loops: Animation.Infinite
                                onStopped: {
                                    duration = Math.floor(Math.random()*(3000-1000+1))+1000
                                }
                            }
                        },
                        Rotation {
                            id: rot2Circ3
                            origin { x: 50; y: 50 }
                            axis { x: 0; y: 0; z: 1 }
                            angle: 0
                            PropertyAnimation on angle {
                                to: rot2Circ3.angle + 360
                                running: true
                                duration: Math.floor(Math.random()*(3000-1000+1))+1000
                                loops: Animation.Infinite
                                onStopped: {
                                    duration = Math.floor(Math.random()*(3000-1000+1))+1000
                                }
                            }
                        }
                    ]
                }
                Rectangle {
                    x: parent.width/2 - width/2
                    y: parent.height/2 - height/2
                    width: 150
                    height: 150
                    radius: 75
                    border.color: "black"
                    border.width: 6
                    color: "transparent"

                    transform: [
                        Rotation {
                            id: rot1Circ2
                            origin { x: 75; y: 75}
                            axis { x: 1; y: 0; z: 0 }
                            angle: 0
                            PropertyAnimation on angle {
                                to: rot1Circ2.angle + 360
                                running: true
                                duration: Math.floor(Math.random()*(3000-1000+1))+1000
                                loops: Animation.Infinite
                                onStopped: {
                                    duration = Math.floor(Math.random()*(3000-1000+1))+1000
                                }
                            }
                        },
                        Rotation {
                            id: rot2Circ2
                            origin { x: 75; y: 75 }
                            axis { x: 0; y: 0; z: 1 }
                            angle: 0
                            PropertyAnimation on angle {
                                to: rot2Circ2.angle + 360
                                running: true
                                duration: Math.floor(Math.random()*(3000-1000+1))+1000
                                loops: Animation.Infinite
                                onStopped: {
                                    duration = Math.floor(Math.random()*(3000-1000+1))+1000
                                }
                            }
                        }
                    ]
                }
                Rectangle {
                    width: 200
                    height: 200
                    radius: 100
                    border.color: "black"
                    border.width: 6
                    color: "transparent"

                    transform: [
                        Rotation {
                            id: rot1Circ1
                            origin { x: 100; y: 100 }
                            axis { x: 1; y: 0; z: 0 }
                            angle: 0
                            PropertyAnimation on angle {
                                to: rot1Circ1.angle + 360
                                running: true
                                duration: Math.floor(Math.random()*(3000-1000+1))+1000
                                loops: Animation.Infinite
                                onStopped: {
                                    duration = Math.floor(Math.random()*(3000-1000+1))+1000
                                }
                            }
                        },
                        Rotation {
                            id: rot2Circ1
                            origin { x: 100; y: 100 }
                            axis { x: 0; y: 0; z: 1 }
                            angle: 0
                            PropertyAnimation on angle {
                                to: rot2Circ1.angle + 360
                                running: true
                                duration: Math.floor(Math.random()*(3000-1000+1))+1000
                                loops: Animation.Infinite
                                onStopped: {
                                    duration = Math.floor(Math.random()*(3000-1000+1))+1000
                                }
                            }
                        }
                    ]
                }

                SequentialAnimation on y {
                    loops: Animation.Infinite
                    PropertyAnimation {
                        from: 0
                        to: -20
                        duration: 5000
                        easing.type: Easing.InOutSine
                    }
                    PropertyAnimation {
                        from: -20
                        to: 0
                        duration: 5000
                        easing.type: Easing.InOutSine
                    }
                }
            }
            Text {
                property string timeSection
                text: "Selamat " + timeSection
                Layout.alignment: Qt.AlignHCenter
                font.pixelSize: 40
                font.bold: true

                Component.onCompleted: {
                    let timeHour = Number(sec1Root.time.split(':')[0])
                    if (4 <= timeHour && timeHour < 12) timeSection = "Pagi"
                    else if (12 <= timeHour && timeHour < 15) timeSection = "Siang"
                    else if (15 <= timeHour && timeHour < 18) timeSection = "Sore"
                    else if (18 <= timeHour && timeHour < 24 || 0 <= timeHour && timeHour < 4) timeSection = "Malam"
                }
            }
        }
    }
}
