import QtQuick 2.0


Row {
    width: childrenRect.width
    height: scaleY(7)
    spacing: scaleX(1)

    StyledButton{
        label: "Attribute"
        onActivated: {attributeSelector.currentModel=attribfilter; }
    }

    StyledButton{
        label: "Genre"
        onActivated: {attributeSelector.currentModel=genrefilter; }
    }

    StyledButton{
        label: "MediaType"
        onActivated: {attributeSelector.currentModel=mediatypefilter; }
    }

    StyledButton{
        label: "Resolution"
        onActivated: {attributeSelector.currentModel=fileformatmodel; }
    }
    StyledButton{
        label: "Sources"
        // onActivated: {attributeSelector.currentModel=undefined; }
    }

    StyledButton{
        label: "Users"
        // onActivated: {attributeSelector.currentModel=undefined; }
    }

    StyledButton{
        label: "Play All"
        onActivated:  manager.playMedia("!G"+iPK_Device)
    }
}
