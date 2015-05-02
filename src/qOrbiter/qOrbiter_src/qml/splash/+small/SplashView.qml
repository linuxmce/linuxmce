// import QtQuick 2.2 // to target S60 5th Edition or Maemo 5
import QtQuick 2.2
import QtGraphicalEffects 1.0
import "."
Item {
   anchors.fill: parent
    id:splashPage

    signal setupStart(string x, string y)
    signal splashLoaded()

    Component{
        id:options
        OptionMenu{
            innerBoxWidth:parent.width
            innerBoxHeight:scaleY(75)
        }
    }

    Connections{
        target: window
        onShowList:{

            if(existing_orbiters.count === 0){
                screenChange("NewOrbiterSetup.qml")
            }
            else{
                existing_orbiters.state="showing"
                newOrbiterButton.state="showing"
                connectionBox.state="hidden"
            }
        }
        onPageChanged:screenchange(qmlPage)
    }

    Item{
        id:topContainer
        anchors{
            top:parent.top
            left:parent.left
            right:parent.right
        }
        height:parent.height *.08

        Rectangle{
            id:welcome_container
            color:theme.appcolor_background
            anchors.fill: parent
            opacity: .15
        }

        Text {
            id: welcome
            text: qsTr(" Welcome to LinuxMCE")
            font.pixelSize: theme.appFontSize_header
            anchors.left:parent.left
            anchors.horizontalCenter: parent.horizontalCenter
            color:"white"
        }

        Text {
            id: ui_name
            text: "smallUI"
            font.pixelSize: theme.appFontSize_description
           // font.weight: Font.Light
            anchors.right: optionIcon.left
            anchors.verticalCenter: optionIcon.verticalCenter
            color:"white"
        }
        Image{
            id:optionIcon
            source:"images/options.png"
            visible: true
            anchors{
                right:parent.right
                verticalCenter: parent.verticalCenter
            }
            MouseArea{
                anchors.fill: parent
                onClicked:{options.createObject(splashPage) }
            }
        }

    }

    Text {
        id: welcome_select
        text: qsTr("Please Choose an orbiter")
        font.pixelSize: theme.appFontSize_description
       anchors.top: topContainer.bottom
        color:"white"
    }

//    StatusRow {
//        id: statusRow
//    }



    Rectangle {
        width: parent.width
        height: scaleY(20)
        opacity: 1
        color: "transparent"

    }
    ConnectionBox {
        id: connectionBox
    }



    ExistingOrbiters {
        id:existing_orbiters
        width:parent.width
        anchors{
            top:welcome_select.bottom
            bottom:newOrbiterButton.top
        }

    }

    NewOrbiterButton {
        id:newOrbiterButton
    }

}




