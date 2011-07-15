import QtQuick 1.0



Rectangle {

    id:rectbox

    width: 150
    height: 450
    color: "whitesmoke"
    clip: true


    ListModel {
        id: attributeList

        ListElement {
            name: "Title"
            pk: 1
        }
        ListElement {
            name: "Performer"
            pk: 2
        }
        ListElement {
            name: "Director"
            pk: 1.95
        }
        ListElement {
            name: "Channel"
            pk: 1.95
        }
        ListElement {
            name: "Studio"
            pk: 1.95
        }
        ListElement {
            name: "Year"
            pk: 1.95
        }
        ListElement {
            name: "Rating"
            pk: 1.95
        }

    }

    ListModel {
        id: filterlist

        ListElement {
            name: "stuff"
            pk: 1
        }
        ListElement {
            name: "stuff"
            pk: 2
        }
        ListElement {
            name: "stuff"
            pk: 1.95
        }
        ListElement {
            name: "stuff"
            pk: 1.95
        }
        ListElement {
            name: "stuff"
            pk: 1.95
        }
        ListElement {
            name: "stuff"
            pk: 1.95
        }
        ListElement {
            name: "stuff"
            pk: 1.95
        }

    }

    Component
    {
    id:attributeComponent


    Rectangle {
        id:attributeDelegate
        border.width: 1
        border.color: black
        width: parent.width
        height: 75

        Text {
            id: attributeName
            text: name
            font.pointSize: 14
            anchors.centerIn: parent
              }

        }
    }

    Text {
        id: label
        text: "Please Select Sort"
    }

    ListView
    {
        id:listview
        anchors.fill: parent
        clip: true
        orientation: ListView.Vertical
        model: attributeList
        delegate: attributeComponent

        MouseArea{
            anchors.fill: parent
            onClicked: {
                rectbox.state = "State1"
                listview.model = filterlist   }
             }

    }
    states: [
        State {
            name: "State1"

            PropertyChanges {
                target: rectbox
                width: 250
                height: 480
            }
        }
    ]


}
