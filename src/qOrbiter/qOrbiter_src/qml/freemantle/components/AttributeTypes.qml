import QtQuick 1.0



Rectangle {

    id:rectbox

    width: 150
    height: 450
    color: "whitesmoke"
    clip: true
    //Component.onCompleted: listview.model = attributeModel

    ListModel {
        id: attributeModel

        ListElement {
            name: "Title"
            pk: 12
        }
        ListElement {
            name: "Performer"
            pk: 2
        }
        ListElement {
            name: "Director"
            pk: 1
        }
        ListElement {
            name: "Channel"
            pk: 10
        }
        ListElement {
            name: "Studio"
            pk: 17
        }
        ListElement {
            name: "Year"
            pk: 19
        }
        ListElement {
            name: "Rating"
            pk: 18
        }
        ListElement {
            name: "Program"
            pk: 12
        }
        ListElement {
            name: "Rating"
            pk: 18
        }
        ListElement {
            name: "Series"
            pk: 43
        }


        ListElement {
            name: "Performer"
            pk: 2
        }
        ListElement {
            name: "Album"
            pk: 3
        }
        ListElement {
            name: "Track"
            pk: 5
        }
        ListElement {
            name: "Genre"
            pk: 8
        }
        ListElement {
            name: "Title"
            pk: 13
        }

    }
}
