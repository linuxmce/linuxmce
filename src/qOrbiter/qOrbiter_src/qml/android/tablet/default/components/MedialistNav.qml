import QtQuick 1.1
import "../../../../skins-common/lib/handlers"

Flickable{
    height:scaleY(7)
    width:scaleX(69)
    anchors.verticalCenter: parent.verticalCenter
    contentWidth: btnRow.width
    clip:true

    Row {
        id:btnRow
        width: childrenRect.width
        height: scaleY(7)
        spacing: scaleX(1)

        StyledButton{
            buttonText: "Attribute"
            onActivated: {attributeSelector.currentModel=attribfilter; }
        }

        StyledButton{
            buttonText: "Genre"
            onActivated: {attributeSelector.currentModel=genrefilter; }
        }

        StyledButton{
            buttonText: "MediaType"
            onActivated: {attributeSelector.currentModel=mediatypefilter; }
        }

        StyledButton{
            buttonText: "Resolution"
            onActivated: {attributeSelector.currentModel=fileformatmodel; }
        }
        StyledButton{
            buttonText: "Sources"
            // onActivated: {attributeSelector.currentModel=undefined; }
        }

        StyledButton{
            buttonText: "Users"
            // onActivated: {attributeSelector.currentModel=undefined; }
        }

        StyledButton{
            buttonText: "Play All"
            onActivated:  manager.playMedia("!G"+iPK_Device)
        }
    }
}


