import QtQuick 1.0
import com.nokia.android 1.1

Component{


    Button{
        id:lightingdelegate
        text: title
        height: scaleY(style.stdbuttonh)
        width: scaleX(15)
        checkable:false
        checked:false
    }

//    Item{
//        //important!! these need to be set on an imported component otherwise its appears all wrong!
//        height: style.stdbuttonh
//        width: style.stdbuttonw

//        /*
//        //this image displays a second button-image beneath the buttons
//        //from the Climate-, Lighting-, Media-, Security- and Telecom-Rows;
//        //if it has no purpose it can be deleted
//        */
//        Image {
//            id: overlay
//            fillMode: Image.PreserveAspectFit
//            source: "../img/ui3/linuxmcewidebutton.png"
//            height: parent.height
//            width: parent.width
//        }
//        Image {
//            id: overlay_hover
//            fillMode: Image.PreserveAspectFit
//            source: "../img/ui3/linuxmcewidebuttongrn.png"
//            height: parent.height
//            width: parent.width
//            visible:false
//        }


//        ImgButton
//        {       id:delegateButton
//            anchors.top: parent.top
//            anchors.topMargin: scaleY(2)
//            color:"transparent"
//            height: style.stdbuttonh
//            width: style.stdbuttonw
//            buttontext: title
//            buttontextbold: true
//            buttontextitalic: true
//            buttonopacity: .85


//            MouseArea{
//                anchors.fill: delegateButton
//                onClicked: manager.execGrp(params)
//                onPressed: overlay_hover.visible = true
//                onReleased: overlay_hover.visible= false
//                hoverEnabled: true

//                onEntered: {
//                    delegateButton.buttontextcolor = "white"
//                    delegateButton.buttonopacity = 1

//                }
//                onExited: {
//                    delegateButton.buttontextcolor = "black"
//                    delegateButton.buttonopacity = .85

//                }
//            }

//        }
//        /*
//        Text {
//            id: daslabel
//            text: label
//            wrapMode: "WrapAtWordBoundaryOrAnywhere"
//            anchors.centerIn: delegateButton
//            anchors.fill: delegateButton
//            z:2
//            horizontalAlignment: Text.AlignHCenter
//            verticalAlignment: Text.AlignVCenter
//            font.family: "Droid Sans"
//        }
//        */
//    }
}

