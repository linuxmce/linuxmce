import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs


Rectangle {
    id:fileviewscreen
    width: style.orbiterW
    height: style.orbiterH
    color: "transparent"
    clip: true

    Image {
        id: gb
        source: "../images/mediatime.png"
    }
    Connections
    {
        target: filedetailsclass
        onShowDetailsChanged:
        {
            MyJs.createFileDetails(gmediaType)
        }
    }

        Component
        {
            id: contactDelegate

            Rectangle
            {
                id:mainItem
                width: scaleX(20);
                height: scaleY(20)
                color: "transparent"

                MouseArea{
                    anchors.fill: mainItem
                    hoverEnabled: true
                    onEntered: {
                        mainItem.color = style.button_system_color_hover
                        mainItem.scale = 1.25
                        mainItem.z = 10

                    }
                    onExited: {
                        mainItem.color = style.button_system_color
                        mainItem.scale = 1
                        mainItem.z = 1
                    }
                }

                Rectangle
                {
                    id:frame

                    width: scaleX(19);
                    height: scaleY(19)
                    anchors.centerIn: mainItem
                    clip:true
                    color: "transparent"

                    MouseArea
                    {
                        anchors.fill: frame
                        onClicked: setStringParam(4, id)
                    }

                    BorderImage {
                        id: borderimg
                        horizontalTileMode: BorderImage.Repeat
                        source: "../images/drpshadow.png"
                        anchors.fill: imagerect
                        anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
                        border { left: 10; top: 10; right: 10; bottom: 10 }
                        //smooth: true
                    }

                    Image
                    {
                        id: imagerect;
                        source:"image://datagridimg/"+id ;
                        height: scaleY(18);
                        width: scaleX(18);
                        anchors.centerIn: parent;
                        fillMode: Image.PreserveAspectCrop
                        opacity: 1
                        asynchronous: true
                        smooth: true
                    }
                    Rectangle{
                        id:textmask
                        color: "grey"
                       anchors.fill:celllabel
                        opacity: .5
                    }

                    Text
                    {
                        id:celllabel
                        text: name;
                        font.pointSize: 12;
                        color: "white" ;
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"

                        width: imagerect.width
                        font.bold: true
                        anchors.top: imagerect.top
                        anchors.horizontalCenter: imagerect.horizontalCenter
                    }
                }
            }
        }

        Component
        {
            id: contactDelegateList
            Item {
                width: list_view1.width;
                height: 120
                Rectangle {
                    id: background
                    x: 2; y: 2; width: parent.width - x*2; height: parent.height - y*2
                    color: "floralwhite"
                    border.color: "black"
                    radius: 5
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: setStringParam(4, id)
                }
                Row {
                    id: topLayout
                    x: 10; y: 10; height: imagerect.height; width: parent.width
                    spacing: 10

                    Image
                    {
                        id: imagerect;
                        source:"image://datagridimg/"+id ;
                        height: 100;
                        width: 156;
                        fillMode: Image.PreserveAspectFit;
                    }

                    Column {
                        width: background.width - imagerect.width - 20;
                        height: imagerect.height
                        spacing:5
                        Text {
                            text: name;
                            opacity: 1;
                            font.pointSize: 12;
                            color: "black" ;
                            wrapMode: "WrapAtWordBoundaryOrAnywhere"
                            //anchors.fill: parent
                            font.bold: true
                        }
                    }

                }
            }
        }


        MultiStateFileDisplay{id:grid_view1}

        ListView{
            id:alphalist
            height: grid_view1.height
            width: scaleX(5)
            clip:true
            anchors.left: grid_view1.right
            model:alphabetlist

            delegate:
                Rectangle {
                id:alphabetrect
                height: scaleY(5)
                width: scaleX(4)
                color: "transparent"
                clip:false
                Text {
                    id: test
                    text: name
                    font.pixelSize: 18
                    anchors.centerIn: parent
                }
                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {                       
                        alphabetrect.scale = 1.5
                    }
                    onExited: {

                        alphabetrect.scale = 1
                    }
                    onClicked: setSeekLetter(name)
                }
            }
        }




        Row
        {
            height: childrenRect.height
            anchors.topMargin: 0
            width: childrenRect.width
            anchors.top: grid_view1.bottom
            anchors.horizontalCenter: parent.horizontalCenter

            ButtonSq
            {
                height: style.stdbuttonh
                width: style.stdbuttonw
                buttontext: "Go Back"
                buttontextbold: true
                MouseArea
                {
                    anchors.fill:parent
                    onClicked: goBackGrid()
                }
            }
            AttributeSelector {}
        }
        ListModel{
            id:alphabetlist

            ListElement{
                name:"A"
            }

            ListElement{
                name:"B"
            }

            ListElement{
                name:"C"
            }

            ListElement{
                name:"D"
            }

            ListElement{
                name:"E"
            }

            ListElement{
                name:"F"
            }

            ListElement{
                name:"G"
            }

            ListElement{
                name:"H"
            }

            ListElement{
                name:"I"
            }

            ListElement{
                name:"J"
            }

            ListElement{
                name:"K"
            }

            ListElement{
                name:"L"
            }

            ListElement{
                name:"M"
            }

            ListElement{
                name:"O"
            }

            ListElement{
                name:"P"
            }

            ListElement{
                name:"Q"
            }

            ListElement{
                name:"R"
            }

            ListElement{
                name:"S"
            }

            ListElement{
                name:"T"
            }

            ListElement{
                name:"U"
            }

            ListElement{
                name:"V"
            }

            ListElement{
                name:"W"
            }

            ListElement{
                name:"X"
            }

            ListElement{
                name:"Y"
            }

            ListElement{
                name:"Z"
            }
        }
    }

