import QtQuick 2.1
import "../components"
Item{
    id:navHeader
    height: scaleY(0)
    state:"hidden"
    anchors{
        top:content.top
        left:parent.left
        right:parent.right
    }

    GradientFiller{
        fillColor: "black"
        opacity: .65
    }
    
    Rectangle{
        id:homeAnchor
        color:"white"
        width: height
        anchors{
            top:parent.top
            left:parent.left
            bottom:parent.bottom
            margins: 5
        }
    }
    StyledText{
        id:mediatypeLabel
        anchors{
            top:parent.top
            left: attribLabel.right
            bottom:parent.bottom
            margins: 15
        }
        color:"white"
        text: qsTr("Mediatype:")+manager.q_mediaType
    }
    
    StyledText{
        id:attribLabel
        anchors{
            top:parent.top
            left: homeAnchor.right
            bottom:parent.bottom
            margins: 15
        }
        color:"white"
        text: qsTr("Attribute:")+ manager.q_attributetype_sort
        
    }
    
    
    StyledText{
        id:subTypeLabel
        anchors{
            top:parent.top
            left: mediatypeLabel.right
            bottom:parent.bottom
            margins: 15
        }
        color:"white"
        text: qsTr("Genre(s):")+ manager.q_attribute_genres
    }
    
    StyledText{
        id:pkAttributeLabel
        anchors{
            top:parent.top
            left: subTypeLabel.right
            bottom:parent.bottom
            margins: 10
        }
        color:"white"
        text: qsTr("Pk Attribute")+ manager.q_pk_attribute
    }
    StyledText{
        id:lastViewedLabel
        anchors{
            top:parent.top
            left: pkAttributeLabel.right
            bottom:parent.bottom
            margins: 10
        }
        color:"white"
        text: qsTr("Last Viewed")+ manager.q_last_viewed
    }
    
    StyledText{
        id:pkUsersLabel
        anchors{
            top:parent.top
            left: lastViewedLabel.right
            bottom:parent.bottom
            margins: 10
        }
        color:"white"
        text: qsTr("Users")+ manager.q_pk_users
    }
    StyledText{
        id:fileFormatLabel
        anchors{
            top:parent.top
            left: pkUsersLabel.right
            bottom:parent.bottom
            margins: 10
        }
        color:"white"
        text: qsTr("File Format: ")+ manager.q_fileFormat
    }
    StyledText{
        id:subLabel
        anchors{
            top:parent.top
            left: fileFormatLabel.right
            bottom:parent.bottom
            margins: 15
        }
        color:"white"
        text: qsTr("Subtype(s):")+ manager.q_subType
    }

    states: [
        State {
            name: ""
            PropertyChanges {
                target: navHeader
                height:0
            }
        },
        State {
            name: "hidden"
            extend: ""
            PropertyChanges {
                target: navHeader
                height:scaleY(0)
            }

        },
        State {
            name: "open"
            extend:""
            PropertyChanges {
                target: navHeader
                height:scaleY(8)
            }
        }
    ]
}
