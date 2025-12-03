import QtQuick 2.15
import QtQuick.Controls
import "HomePageSections"

Item {
    property string time

    id: homePageRoot

    Rectangle {
        id: root
        anchors.fill: parent

        ScrollView {
            id: scrollView
            anchors.fill: parent

            ScrollBar.horizontal: ScrollBar {
                parent: parent
                active: false
            }

            ScrollBar.vertical: ScrollBar {
                parent: parent
                height: parent.height
                x: parent.width
            }

            Column {
                Section1 { time: homePageRoot.time}
                Rectangle {
                    width: root.width
                    height: root.height
                    color: "yellow"
                }
            }
        }

        Binding {
          target: scrollView.contentItem
          property: "boundsBehavior"
          value: Flickable.StopAtBounds
        }
    }
}
