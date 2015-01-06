import QtQuick 2.2
import "../../../../../skins-common/qt5/default/components"

Row{
    id:homeNav
    height:scaleY(10)
    visible:manager.currentScreen==="Screen_1.qml"
    spacing: scaleX(2)
    anchors.verticalCenter: parent.verticalCenter
    anchors.leftMargin: 3
    anchors.topMargin: 3
    StyledButton{
        id:roombt
        buttonText: roomList.currentRoom+" "//+"::"+roomList.currentEA
	height: parent.height
	width:  scaleX(18)
        txtObj.color: "antiquewhite"
        txtObj.isBold: false
        hitArea.onReleased: {
	    showRoomSelector();
            console.log(manager.currentScreen)
        }
    }
/*    StyledButton{
        id:nowplayingbt
        buttonText: "Now playing"
	height: parent.height
	width:  scaleX(20)
        txtObj.color: "antiquewhite"
        txtObj.isBold: false
        hitArea.onPressAndHold:{
            manager.showfloorplan(floorplantype)
            manager.setFloorplanType(floorplantype)
        }

        hitArea.onReleased: {
            console.log(manager.currentScreen)
        }
    }
    StyledButton{
        id:menubt
        buttonText: "Menu"
	height: parent.height
	width:  scaleX(18)
        txtObj.color: "antiquewhite"
        txtObj.isBold: false
        hitArea.onPressAndHold:{
            manager.showfloorplan(floorplantype)
            manager.setFloorplanType(floorplantype)
        }

        hitArea.onReleased: {
            console.log(manager.currentScreen)
        }
    }*/
}
