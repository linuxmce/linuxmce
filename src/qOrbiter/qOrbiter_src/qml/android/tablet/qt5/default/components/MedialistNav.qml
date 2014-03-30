import QtQuick 2.0


Row {
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
