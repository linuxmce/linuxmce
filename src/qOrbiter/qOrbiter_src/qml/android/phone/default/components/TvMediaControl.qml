import QtQuick 1.0
import "../../../../skins-common/lib/handlers"
Item{
    anchors.fill: parent

    Row{
        height:parent.height
        width: parent.width
        spacing: scaleY(2)

        StyledButton{
            buttonText: "Info"
            onActivated: pageLoader.item.children[0].state="INFO"
        }
        StyledButton{
            buttonText: "Digits"
            onActivated: pageLoader.item.children[0].state="NUMBERS"
        }

        StyledButton{
            buttonText: "EPG"
            onActivated: pageLoader.item.children[0].state="GRID"
        }
        StyledButton{
            buttonText: "Remote"
            onActivated: pageLoader.item.children[0].state="REMOTE"
        }
        StyledButton{
            buttonText: "Advanced"
            onActivated: pageLoader.item.children[0].state="advanced"
        }
    }

}


