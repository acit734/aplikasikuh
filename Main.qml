import QtQuick
import "components/qml/main"
import "components/qml/contents"

Window {
    id: window
    width: 640
    height: 480
    minimumWidth: 640
    minimumHeight: 480
    maximumWidth: 640
    maximumHeight: 480
    visible: true
    title: qsTr("Aplikasikuh")

    ContentContainer {
        id: contentContainer
        content: HomePage{ time: navBar.time }
    }
    NavBar { id: navBar }
    SideBar { target: contentContainer }

    Component.onCompleted: {
        Backend.debug("hai") //qmllint disable unqualified
        console.log("hai")
    }
}
