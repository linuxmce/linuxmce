import QtQuick 2.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../../../skins-common/lib/handlers"

Item {
    id:fileviewscreen
    width: manager.appWidth
    height: manager.appHeight
    clip: true
    focus:true
    Component.onCompleted: {
        forceActiveFocus()
    }

    Keys.onReleased: {
        event.accepted=true
        switch(event.key){
        case Qt.Key_Back:
        case Qt.Key_MediaPrevious:
	    if (!manager.goBackGrid())
                event.accepted=false
            break;
        case Qt.Key_Menu:

            break;
        case Qt.Key_M:
            if(selector.state === "hidden")
                selector.state ="atrselect"
            else
            selector.state ="hidden"
            break;
        default:
            console.log(event.key)
            break
        }
    }


    Connections
    {
        target: filedetailsclass
        onShowDetailsChanged:
        {
            loadComponent("FileDetails.qml")

        }
    }




    Component
    {
        id: contactDelegateList
        Item {
            width: scaleX(95)
            height: scaleY(15)

            Rectangle {
                id: background
                color: mouseclick.pressed ? "orange":"black"
                anchors.fill: parent
                opacity: mouseclick.pressed ? 1 : .75
            }

            Image  {
                id: imagerect;
                fillMode: Image.PreserveAspectCrop
                source:path !=="" ? "http://"+m_ipAddress+"/lmce-admin/MediaImage.php?type=img&val="+path : ""
                anchors.fill: parent
            }
            Rectangle{
                id:mask
                anchors.fill: parent
                color:"black"
                opacity:.65
            }

            StyledText {
                text: name;
                font.pixelSize: scaleY(3.5);
                color: "White" ;
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                width: parent.width
                height: parent.height
                font.weight: Font.Light
            }


            MediaListClickHandler{
                id:mouseclick
            }

        }
    }


    ListView {
        id: list_view1
        width: scaleX(95)
        height:scaleY(90)
        model: manager.getDataGridModel("MediaFile", 63)
        delegate: contactDelegateList
        clip: true
        cacheBuffer: 15
        anchors.left: parent.left
        anchors.leftMargin: scaleX(2)
        anchors.top: fileviewscreen.top

    }

    Text{
        id:spaceholder
        text:manager.dceResponse
        font.pixelSize: scaleY(3)
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        color: "silver"

    }

    AttributeSelector {
        id:selector

    }

    GenericAttributeFilter{
        id:filterTarget
        anchors.centerIn: parent
    }
}
