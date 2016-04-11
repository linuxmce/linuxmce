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
            floorplantype:7
        }
        ListElement{
            name:"Security"
            modelName:"currentRoomSecurity"
            modelIndex: 4
            floorplantype:1
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
}


