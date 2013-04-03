import QtQuick 1.1

Rectangle{
    color: "transparent"
    height: style.stdbuttonw //RayBe
    width: style.stdbuttonw //RayBe
    property alias buttontext: buttonLabel.text
    property alias buttontextcolor: buttonLabel.color
    property alias buttontextfontsize: buttonLabel.font.pointSize
    property alias buttontextbold: buttonLabel.font.bold
    property alias buttontextitalic: buttonLabel.font.italic
    property alias buttontextzindex: buttonLabel.z
    property alias buttonsqradius:  buttonBase.radius


    Rectangle {
        id:buttonBase
        opacity: 0.85
        //color: "green" //RayBe
        //color:style.button_system_color //RayBe
        height: parent.height
        width: parent.width
        //border.width: 2 //RayBe
        //border.color: style.highlight1 //RayBe
        //radius: 5 //RayBe

        Image {
            id: buttonmainimage
            source: "../img/buttons/ButtonMain.png"
            anchors.fill: parent
        }

        Text {
            id: buttonLabel
            //x: 50 //RayBe
            //y: 50 //RayBe
            //width: -5 //RayBe was -1
            //height: 0
            font.family: didactGothic.name//RayBe
            font.pixelSize: parent.height/3.1 //RayBe
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            anchors.fill: parent
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            // elide: "ElideRight"
            color:"white"
            opacity: 1
            z:1
            style: Text.Sunken; styleColor: "grey" // added by RayBe
            textFormat: Text.RichText // added by RayBe
            smooth: true
        }

        MouseArea{
            anchors.fill: buttonLabel
            hoverEnabled: true
            onEntered: {
                buttonBase.opacity = .55
                //buttonBase.color = style.button_system_color_hover //RayBe
                //buttonLabel.color = style.lighttext //RayBe
                //buttonLabel.font.capitalization = Font.AllUppercase //RayBe
            }

            onExited: {
                buttonBase.opacity = .85
                //buttonBase.color = style.button_system_color //RayBe
                //buttonLabel.font.capitalization = Font.Normal //RayBe
                //buttonLabel.color = "black" //RayBe
            }
        }
    }
}
