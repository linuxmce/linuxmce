import QtQuick 1.1
import "../../../../skins-common/lib/handlers"

Row {
    width: childrenRect.width
    height: scaleY(7)
    spacing: scaleX(1)

    StyledButton{
        buttonText.text: "Attribute"
        onActivated: {attributeSelector.currentModel=attribfilter; }
    }

    StyledButton{
        buttonText.text: "Genre"
        onActivated: {attributeSelector.currentModel=genrefilter; }
    }

    StyledButton{
        buttonText.text: "MediaType"
        onActivated: {attributeSelector.currentModel=mediatypefilter; }
    }

    StyledButton{
        buttonText.text: "Resolution"
        onActivated: {attributeSelector.currentModel=fileformatmodel; }
    }
    StyledButton{
        buttonText.text: "Sources"
        // onActivated: {attributeSelector.currentModel=undefined; }
    }

    StyledButton{
        buttonText.text: "Users"
        // onActivated: {attributeSelector.currentModel=undefined; }
    }

    StyledButton{
        buttonText.text: "Play All"
        onActivated:  manager.playMedia("!G"+iPK_Device)
    }
}
