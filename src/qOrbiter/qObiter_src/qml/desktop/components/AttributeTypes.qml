import QtQuick 1.0



Rectangle {

    id:rectbox

    width: childrenRect.width
    height: childrenRect.height
    color: "whitesmoke"
    clip: true
    anchors.centerIn: parent
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


    Rectangle {
               id:rect
               width: 200
               height: 200
               color: style.bgcolor
               anchors.centerIn: parent
               border.color: style.button_system_color
               border.width: 1
               clip: true


             Component{
                   id:attributetypedelegate
                   Item {
                       id:attributetypeitem
                       height: 50
                       width: parent.width

                       Rectangle{
                           id: attributetyperect
                           height: 50
                           width: parent.width
                           border.color: "white"
                           border.width: 1
                           color: status ? "green" : style.alert_bg

                           MouseArea{
                               anchors.fill: parent
                               onPressAndHold: {rectbox.destroy()
                               }

                               onClicked:{
                                  // attributetypemodel.setSelectionStatus(name)

                                         }
                           }
                           Text {
                               id: attributetypecell
                               text: name
                               font.pointSize: 14
                           }


                       }
                   }
               }


            ListView{
                id:attributetypelist
                height: parent.height
                width: parent.width
                model: attributeModel
                delegate: attributetypedelegate

            }

       }



}
