// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import "../../lib/handlers"

Rectangle {
    id:filterView
    width: manager.appWidth
    height: scaleY(12)
    color: "transparent"
    state: "inactive"
    property alias currentFilterModel:filterList.model

    onCurrentFilterModelChanged: filterList.count===0 ? state = "inactive" : state = "active"

    Rectangle{
        anchors.fill: parent
        color: "black"
        opacity: .25
    }

    ListView{
        id:filterList
        orientation: ListView.Horizontal

        height: filterView.height
        width: filterView.width
        clip: true
        spacing:scaleX(2)
        delegate: Rectangle{
            height: scaleY(10)
            width: scaleX(10)
            color: "transparent"

            Image {
                source: "../img/icons/Video.png"
                anchors.fill: parent
            }

            StyledText{
                text:name
                anchors.centerIn: parent
                font.bold: true
                font.pointSize: scaleY(3)

            }
            AttributeFilterHandler{onClicked: {filterView.state = "inactive";filterList.model=""}}
        }
    }

    states: [
        State {
            name: "active"
            PropertyChanges {
                target: filterView
                visible:true
            }
            AnchorChanges{
                anchors.bottom: undefined
                anchors.top: parent.top
            }
        },
        State {
            name: "inactive"
            PropertyChanges {
                target: filterView
                visible:false
            }
            AnchorChanges{
                anchors.top: undefined
                anchors.bottom: parent.top
            }
        }
    ]

}
