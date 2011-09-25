import QtQuick 1.0
import "../components"
Stage{
    id:securitypanel

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
                        delegatetext.color = "white"; secrect.color = style.darkhighlight
                    }
                    onExited: {
                        delegatetext.color = "black"; secrect.color = "whitesmoke"
                    }

                    onClicked: {
                        setHouseMode(mode, securitynumbers.pass)
                        console.log(securitynumbers)
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

    Text {
        id: securitypanellabel
        anchors.bottom: container.top
        text: "Security Panel"
        font.family: "Droid Sans"
        font.bold: false
        font.pointSize: 15
    }
//main layout
    ShadowRect{
        id:container       
        rectcolor: style.highlight1
        anchors.centerIn: parent
        rectheight: childrenRect.height
        rectwidth: contentrow.width

            Row{
                id:contentrow
                height: scaleY(45)
                width: scaleX(55)
                spacing: scaleX(2)
                anchors.centerIn: container
                HomeButton{height: 75; smooth: true}
//numberpad
                SecurityPad{id:securitynumbers; width: scaleX(20)}

                Rectangle {
                    id: rectangle4
                    width: 200
                    height: 402
                    color: "#a7b8c4"
                    radius: 16
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

