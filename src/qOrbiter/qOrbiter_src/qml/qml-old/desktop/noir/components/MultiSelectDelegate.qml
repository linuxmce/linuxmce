import QtQuick 1.1

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
              color: "transparent"

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
                               setSelectionStatus(name)
                               fileformatcell.color = status ? "green" : "red"

                                     }
                               }
                           }

                    }


              }
          }
      }
