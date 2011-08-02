import QtQuick 1.0

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
               id:fileformatdelegate
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

                       Row{
                           height: childrenRect.height
                           width: childrenRect.width
                           spacing: 85

                           Text {
                               width: 75
                               id: fileformatcell
                               text: name
                               font.pointSize: 14
                               onFocusChanged: {rect.destroy()}
                               MouseArea{
                                         anchors.fill: fileformatcell
                                         onPressAndHold:
                                         {
                                             rect.destroy()
                                         }
                                    }
                                }

                           Rectangle{
                               height: 25
                               width: 25
                               border.color: "black"
                               border.width: 1
                               color: status ? "green" : "red"
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked:{
                                        console.log(fileformatmodel.setSelectionStatus(name))
                                        fileformatlist.currentIndex=index
                                              }
                                        }
                                    }

                             }


                       }
                   }
               }



        ListView{
            id:fileformatlist
            height: parent.height
            width: parent.width
            model: fileformatmodel
            delegate: fileformatdelegate
            focus:true

           onCurrentIndexChanged: {
               console.log("Model Changed " + currentIndex + "highlighted")

           }
        }

   }


