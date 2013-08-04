import QtQuick 1.1

Item{
    id:homeScreenItems
    height: info_panel.height / 2
    width: info_panel.width *.55

    Row{
        height: parent.height
        width: parent.width
        spacing:scaleX(2)

        StyledButton{
            buttonText.text: "Power"
        }
        StyledButton{
            buttonText.text: "Sleeping Menu"
        }

    }

}
