import QtQuick 2.2
import "../components"
import "../../../../../skins-common/qt5/default/components"

StyledScreen {
    navigation: "../../../../android/phone/qt5/default/components/PhoneNavHome.qml"
    keepHeader: true
    property int rowHeight: 15

    Rectangle {
	anchors.fill: parent
        color: "black"
        opacity: .65
    }
    ListModel{
        id: topMenuItems
        ListElement{
            name:"Lights"
            modelName:"currentRoomLights"
	    modelIndex: 0
            floorplantype:2
        }
        ListElement{
            name:"Media"
            modelName:"currentRoomMedia"
	    modelIndex: 1
            floorplantype:5
        }
        ListElement{
            name:"Climate"
            modelName:"currentRoomClimate"
	    modelIndex: 2
            floorplantype:3
        }
        ListElement{
            name:"Telecom"
            modelName:"currentRoomTelecom"
	    modelIndex: 3
            floorplantype:6
        }
        ListElement{
            name:"Security"
            modelName:"currentRoomSecurity"
	    modelIndex: 4
            floorplantype:4
        }
    }

    property variant scenarioModel: [ currentRoomLights, currentRoomMedia, currentRoomClimate, currentRoomTelecom, currentRoomSecurity ]

    // ListView to contain different scenarios vertically
    ListView {
	id: scenarioView
	anchors.fill: parent
	model: topMenuItems
	orientation: ListView.Vertical
	flickableDirection: Flickable.VerticalFlick
	focus: false
	spacing: 5
	delegate: Row {
	    height: scaleY(rowHeight)
	    width: parent.width
	    anchors.leftMargin:5
	    Item {
		width: scaleX(14)
		height: parent.height

		StyledButton {
		    id: scenarioBt
		    buttonText: name    
		    width: manager.isProfile ? parent.height : scaleX(14)
		    height: manager.isProfile ? scaleX(14) : parent.height
		    transformOrigin: Item.TopRight
		    transform: [Rotation { angle: manager.isProfile ? -90 : 0; }, Translate{ y: manager.isProfile ? scaleY(rowHeight) : 0 } ]
		    hitArea.onReleased:{
			manager.setFloorplanType(floorplantype)
			manager.showfloorplan(floorplantype)
		    }
		}
	    }
	    Item {
		height: parent.height
		width: scaleX(86)
		ListView {
		    height: parent.height
		    width: parent.width
		    model: scenarioModel[modelIndex]
		    orientation: ListView.Horizontal
		    flickableDirection: Flickable.HorizontalFlick
		    spacing: 5
		    clip: true
		    delegate: Item{
			width: manager.isProfile ? scaleY(rowHeight) : scaleY(rowHeight*1.5)
			height: parent.height
			
			StyledButton{
			    id:button
			    anchors.fill: parent
			    buttonText: title
			    textSize: manager.isProfile ? scaleY(3) : scaleY(5)
			    hitArea.onReleased: manager.execGrp(params)
			    
			}
		    }
		}
	    }
	}

    }

/*
    NowPlayingButton{
        id:fs_npButton
        anchors.top: parent.top
        anchors.left: manager.isProfile ? parent.horizontalCenter : parent.left
        anchors.leftMargin: manager.isProfile ? (fs_npButton.width / 2) * -1 : scaleX(2)
    }


    DroidHomeSelector{
        id:home_selector;
        anchors.top: manager.isProfile ? fs_npButton.bottom : parent.top
        anchors.left: manager.isProfile ? stage.left : fs_npButton.right
        anchors.leftMargin: manager.isProfile ? scaleX(15) : scaleX(1)
    }
    ScenarioSelector{
        id:scenarioPopup
    }

    HomeOptions {
        id: advancedrow
    }

    Item{
        id:popExit
        height: scaleY(35)
        width: scaleX(85)
        opacity: 0

        anchors.centerIn: parent

        Behavior on opacity{

            PropertyAnimation{
                duration:500
            }
        }

        Timer{
            id:exitTimer
            interval: 1250
            onTriggered: popExit.opacity = 0
        }

        Rectangle{
            anchors.fill: parent
            color: ms.pressed ? androidSystem.greenStandard : androidSystem.redStandard
        }
        StyledText{
            text:"Click Again to exit"
            font.weight: Font.DemiBold
            color: "White"
            width: parent.width
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            fontSize: scaleY(8)
            anchors.centerIn: parent
        }

        MouseArea{
            id:ms
            anchors.fill: parent
            enabled: parent.opacity != 0
            onReleased: manager.closeOrbiter()
        }

    }
*/
}


