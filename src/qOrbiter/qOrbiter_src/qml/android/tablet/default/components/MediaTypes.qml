import QtQuick 1.0

Rectangle {
    id:rect
    width: scaleX(35)
    height: scaleY(60)
    color: "transparent"
    anchors.centerIn: parent
    border.color: style.button_system_color
    border.width: 1
    clip: true

    Component{
        id:mediatypedelegate
        Item {
            id: fileformatitem
            height: scaleY(8)
            width: parent.width

            Rectangle{
                id: formatrect
                height: scaleY(8)
                width: parent.width
                border.color: "black"
                border.width: 1
                color: style.darkhighlight

                MouseArea{
                    anchors.fill: parent
                    z:0
                    onClicked:
                    {
                        loadComponent("NullComponent.qml")
                    }
                }


                Row{
                    height: childrenRect.height
                    width: childrenRect.width

                    Text {
                        width: scaleX(10)
                        height: parent.height
                        id: fileformatcell
                        text: name
                        font.pointSize: 14
                        onFocusChanged: {loadComponent("NullComponent.qml")}

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

                                mediatypefilter.setSelectionStatus(name)
                                fileformatcell.color = status ? "green" : "red"
                                loadComponent("NullComponent.qml")
                            }
                        }
                    }
                }
            }
        }
    }


    ListView{
        id:mediatypelist
        height: parent.height
        width: parent.width
        model: mediatypefilter
        delegate: mediatypedelegate

    }

}


