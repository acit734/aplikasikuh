import QtQuick 2.15
import "HomePageSections"

Column {
    id: root
    
    function beginSection1Animation() {
        section1.startOpeningAnimation()
    }

    property real rWidth: 640 - 37.5
    property real rHeight: 480 - 37.5

    property string time
    Section1 {
        id: section1
        time: parent.time
        width: root.rWidth
        height: root.rHeight
    }
    Rectangle {
        width: root.rWidth
        height: root.rHeight
        color: "yellow"
    }
}
