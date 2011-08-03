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
                     id:attributeDelegate
                     Item {
                         id: fileformatitem
                         height: 50
                         width: parent.width

                         Rectangle{
                             id: formatrect
                             height: 50
                             width: parent.width
                             border.color: "black"
                             border.width: 1
                             color: style.bgcolor

                             MouseArea{
                                       anchors.fill: parent
                                       z:0
                                       onClicked:
                                       {
                                           rect.destroy()
                                       }
                                  }


                             Row{
                                 height: childrenRect.height
                                 width: childrenRect.width
                                 spacing: 85


                                 Text {
                                     width: 75
                                     height: parent.height
                                     id: fileformatcell
                                     text: name
                                     font.pointSize: 14
                                     onFocusChanged: {rect.destroy()}

                                      }

                                 Rectangle{
                                     height: 25
                                     width: 25
                                     border.color: "black"
                                     border.width: 1
                                     color: status ? "green" : "red"
                                     z:10
                                      MouseArea{
                                          anchors.fill: parent
                                          onClicked:{
                                              cindex=desc
                                              attributeModel.setSelectionStatus(name)
                                              fileformatcell.color = status ? "green" : "red"

                                                    }
                                              }
                                          }

                                   }


                             }
                         }
                     }



            ListView{
                id:attributetypelist
                height: parent.height
                width: parent.width
                model: attributeModel
                delegate: attributeDelegate

            }

       }



}
