import QtQuick 2.2
import org.linuxmce.screeninfo 1.0
import QtGraphicalEffects 1.0
Item{
    id:connectionVars
    property alias router:routerInput.text
    property string location:""
    signal valueSet();
    width: parent.width *.65
    height: parent.height *.35
    anchors.horizontalCenter: parent.horizontalCenter

    Rectangle{
        id:sink
        anchors{
            left:parent.left
            right:parent.right
            bottom:parent.bottom
            top:locationText.bottom
        }
        visible:false
        color:"darkgrey"
        radius: 5
    }

    InnerShadow{
        source: sink
        anchors.fill: sink
        radius: 6.0
        samples: 16
        horizontalOffset: -3
        verticalOffset: 3
        color: "black"
    }

    Text{
        id:locationText
        text:qsTr("Location: %1").arg(location)
        anchors{
            top:parent.top
            left:parent.left
            margins: 5
        }
        color:"white"
    }

    Rectangle{
        anchors.fill: routerInput
        radius:5
    }


    Text {
        id:host_label
        text: qsTr("Host:")
        font.pixelSize: theme.appFontSize_title
        color:theme.apptext_color_active
        font.family: myFont.name
       // font.weight: Font.Light
        height: sink.height
       anchors.verticalCenter: sink.verticalCenter
       anchors.verticalCenterOffset: 5
       anchors.left:parent.left
       anchors.leftMargin:5

    }
    

    TextInput {
        id: routerInput
       // width: scaleX(17) * theme.dpRatio
        color: "#030000"

        height: host_label.paintedHeight
        clip: true
        text: orbiterWindow.router
        font.pixelSize:theme.appFontSize_list
        font.family: myFont.name

        anchors{
            verticalCenter: sink.verticalCenter
            left:host_label.right
            right:connectbutton.left
            margins: 8
        }
    }
    

    Rectangle {
        id: connectbutton
        height: routerInput.height
        width: theme.appButtonWidth*theme.dpRatio
        radius:5
        color:theme.appbutton_confirm_color

        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.verticalCenter: sink.verticalCenter

        Text {
            id: name
            anchors.centerIn: parent
            text: qsTr("Go!")
            font.pixelSize: theme.appFontSize_list
            verticalAlignment: Text.AlignTop
            font.bold: true
            font.family: myFont.name
            color:theme.apptext_color_active
        }

        MouseArea{
            hoverEnabled: true
            onEntered: parent.color="green"
            onExited: parent.color="red"
            anchors.fill: parent
            onClicked: { orbiterWindow.qmlSetupLmce(orbiterWindow.deviceno, routerInput.text) ;valueSet() }
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
