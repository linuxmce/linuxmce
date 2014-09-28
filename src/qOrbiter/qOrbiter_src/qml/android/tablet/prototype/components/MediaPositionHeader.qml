import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../../../../skins-common/lib/effects"

Item{
    id: pos_label
    anchors.top: fileviewscreen.top
    anchors.horizontalCenter: fileviewscreen.horizontalCenter
    width: scaleX(95)
    height: label_row.height
    focus:true


    Rectangle{
        radius:5
        focus:true
        color:"red"
        opacity: .25
        anchors.fill: parent
    }


    Row{
        id:label_row
        width: childrenRect.width
        height: childrenRect.height
        anchors.centerIn: pos_label
        spacing: scaleX(1)
        StyledText {
            id: grid_position_label
            text: manager.translateMediaType(manager.q_mediaType)
            color:"white"
        }

        StyledText{
            text:"|"
            color:"white"
        }

        StyledText {
            id: grid_attritbute_label
            text:  manager.q_attributetype_sort
            color:"white"
        }
        StyledText {
            id: sub_attritbute_label
            text: " | " + manager.q_subType
            color:"white"
        }
        StyledText {
            id: page_label
            text: qsTr("Current Page:") + manager.media_currentPage
            color:"white"
            visible:dataModel.totalPages !==1

        }

        Rectangle{
            height: parent.height-5
            width: scaleX(15)
            color: "blue"
            opacity:.65
            anchors.verticalCenter: parent.verticalCenter
            StyledText{
                text: qsTr("Set Paging")
                color:"white"
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    pos_label.forceActiveFocus()
                    if(!seperationSetter.text.match("\d")){
                        manager.setDceGridSep(seperationSetter.text)

                        // fileviewscreen.forceActiveFocus()
                    }
                    else
                    {
                        seperationSetter.text.color = red
                    }
                }
            }
        }

        TextInput{
            id:seperationSetter
            width: page_label.width
            text:  manager.media_pageSeperator
            anchors.verticalCenter: parent.verticalCenter
            color:"white"
            font.pointSize: scaleY(3.5)
            onActiveFocusChanged: {
                if(activeFocus)
                { text=""}
                else
                {  manager.media_pageSeperator}
            }
            Keys.onReleased: {
                event.accepted=true
                switch(event.key){
                case Qt.Key_Enter:
                    console.log("enter");
                    break;
                case Qt.Key_Return:
                    seperationSetter.closeSoftwareInputPanel()
                    console.log("return pressed on page split")

                    if(!seperationSetter.text.match("\d")){
                        manager.setDceGridSep(seperationSetter.text)

                        closeSoftwareInputPanel();

                    }
                    else
                    {
                        console.log("no match?")
                        seperationSetter.text.color = red
                    }
                    break;

                }
            }

        }
    }
    
}
