import QtQuick 1.1
import "../components"
/*!
 *\brief Security Panel
 *
 *\ingroup qml_desktop_default
 *This screen allows you to set the desired security mode.
 */
Stage{
    id:securitypanel

    BorderImage {
        id: borderimg
        horizontalTileMode: BorderImage.Repeat
        source: "../img/icons/drpshadow.png"
        anchors.fill: container
        anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
        border { left: 10; top: 10; right: 10; bottom: 10 }
        smooth: true
    }

    ListModel{
        id:securityModes

        ListElement{
            name: "Un-Armed - Home"
            image: "security/Home.png"
            mode:1
        }

        ListElement{
            name: "Armed - Away"
            image: "security/Away.png"
            mode:2
        }

        ListElement{
            name: "At Home Secure"
            image: "security/HomeSecure.png"
            mode:3
        }

        ListElement{
            name: "Sleeping"
            image: "security/Away.png"
            mode:4
        }

        ListElement{
            name: "Entertaining"
            image: "security/Entertaining.png"
            mode:5
        }

        ListElement{
            name: "Extended Away"
            image: "security/Sleeping.png"
            mode:6
        }
    }

    //delegate
    Component {
        id: securityDelegate
        Item{
            height: childrenRect.height
            width: childrenRect.width
            clip: true

            Rectangle{
                id:secrect
                height:75
                width: securitylist.width
                color: "whitesmoke"
                opacity: .5
                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {
                        delegatetext.color = "white"; secrect.color = skinstyle.darkhighlight
                    }
                    onExited: {
                        delegatetext.color = "black"; secrect.color = "whitesmoke"
                    }

                    onClicked: {
                        manager.setHouseMode(mode, securitynumbers.pass)
                    }
                }
            }

            Text {
                id: delegatetext
                text: name
                anchors.centerIn: secrect
                font.pointSize: 12
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
            }
        }
    }


    //main layout
    Rectangle{
        id:container
        color: skinstyle.lightHighlight
        anchors.centerIn: parent
        height: childrenRect.height + scaleY(1)
        width: contentrow.width + scaleX(1)
        radius: 2.5

        Text {
            id: securitypanellabel
            anchors.bottom: container.top
            text: "Security Panel"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }

        Row{
            id:contentrow
            height: securitynumbers.height
            width: scaleX(55)
            spacing: scaleX(5)
            anchors.centerIn: container
            HomeButton{smooth: true}
            //numberpad
            SecurityPad{id:securitynumbers; }

            Rectangle {
                id: rectangle4
                width: scaleX(15)
                height: scaleY(45)
                color: "#a7b8c4"
                radius: 2.5
                clip: true

                ListView
                {
                    id:securitylist
                    height: parent.height
                    width: parent.width
                    clip: true
                    model: securityModes
                    orientation: ListView.Vertical
                    delegate: securityDelegate
                }
            }
        }

    }
}

