import QtQuick 1.1

Rectangle {
    id:rect
    width: childrenRect.width
    height: childrenRect.height
    color: "transparent"
    anchors.centerIn: parent
    border.color: style.button_system_color
    border.width: 1
    clip: true


    Component{
        id:genredelegate
        Item {
            id: fileformatitem
            height: scaleY(8)
            width: scaleX(15)

            Rectangle{
                id: formatrect
                height: scaleY(8)
                width: scaleX(15)
                border.color: "black"
                border.width: 1
                color: status ? style.stageBG : "green"

                MouseArea{
                    anchors.fill: parent
                    z:0
                    onClicked:
                    {
                        rect.destroy()
                    }
                }


                Rectangle{
                   anchors.fill:parent
                   color:"transparent"
                    StyledText {

                        width: scaleX(10)
                        height: parent.height
                        isBold: true
                        id: fileformatcell
                        text: status ? name +" Is selected": name
                        font.pointSize: 12
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        onFocusChanged: {rect.destroy()}
                    }

                    Rectangle{
                        height: 25
                        width: 25
                        border.color: "black"
                        border.width: 1
                        color: status ? "red" : style.stageBG
                        anchors.right: parent.right
                        z:10
                        MouseArea{
                            anchors.fill: parent
                            onClicked:{
                                genrefilter.setSelectionStatus(name)

                            }
                        }
                    }
                }
            }
        }
    }


    ListView{
        id:genrelist
        height: scaleY(80)
        width: scaleX(15)
        model: genrefilter        
        delegate: genredelegate
    }
}


