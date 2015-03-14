// import QtQuick 2.0 // to target S60 5th Edition or Maemo 5
import QtQuick 2.3
import "."
Item {
   anchors.fill: parent
    id:splashPage
    anchors.centerIn: parent
    signal setupStart(string x, string y)
    signal splashLoaded()


    onWidthChanged: console.log( "SmallUI"+height)

    Component.onCompleted: console.log("smalluiLoaded"+manager.appHeight)

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
        height:Style.appNavigation_panelHeight

//        Rectangle{
//            id:welcome_container
//            color:Style.appcolor_background
//            anchors.fill: parent
//            opacity: .15
//        }

        Text {
            id: welcome
            text: qsTr(" Welcome to LinuxMCE")
            font.pointSize: Style.appFontSize_title
            anchors.left:parent.left
            anchors.top: parent.top
            color:"white"
        }
        Text {
            id: welcome_select
            text: qsTr("Please Choose an orbiter")
            font.pointSize: Style.appFontSize_description
            font.weight: Font.Light
            anchors{
                left:parent.left
                bottom:parent.bottom
            }
            color:"white"
        }
        Text {
            id: ui_name
            text: "SmallUI"
            font.pointSize: Style.appFontSize_description
            font.weight: Font.Light
            anchors.right:optionIcon.left
            anchors.verticalCenter: parent.verticalCenter
            color:"white"
        }
        Image{
            id:optionIcon
            source:"images/options.png"
            height: parent.height /2
            fillMode: Image.PreserveAspectFit
            anchors{
                right:parent.right
                verticalCenter: parent.verticalCenter
            }
        }
    }

    //    StatusRow {
    //        id: statusRow
    //    }



    ConnectionBox {
        id: connectionBox
    }


    ExistingOrbiters {
        id:existing_orbiters

    }

    NewOrbiterButton {
        id:newOrbiterButton
        width:parent.width *.85
    }

}




