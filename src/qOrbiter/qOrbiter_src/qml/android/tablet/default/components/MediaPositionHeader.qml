import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../../../../skins-common/lib/effects"

Rectangle{
    id: pos_label
    anchors.top: fileviewscreen.top
    anchors.horizontalCenter: fileviewscreen.horizontalCenter
    radius:5
    focus:true
    color:style.darkhighlight
    width: scaleX(75)
    height: scaleY(5)
    Row{
        id:label_row
        width: childrenRect.width
        height: scaleY(5)
        anchors.centerIn: pos_label
        spacing: scaleX(1)
        Text {
            id: grid_position_label
            text: qsTr("You are browsing by:") + manager.i_current_mediaType
        }
        Text {
            id: grid_attritbute_label
            text: qsTr("Attribute Type Sort") + manager.q_attributetype_sort
        }
        Text {
            id: page_label
            text: qsTr("Current Page") + manager.media_currentPage
        }
        TextInput{
            id:seperationSetter
            width: page_label.width
            text: manager.media_pageSeperator
            
        }
        Rectangle{
            height: parent.height-1
            width: childrenRect.width + 10
            StyledText{
                text: qsTr("Set")
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    pos_label.forceActiveFocus()
                    if(!seperationSetter.text.match("\d")){
                        manager.setDceGridSep(seperationSetter.text)
                        manager.requestPage(0)
                    }
                    else
                    {
                        seperationSetter.text.color = red
                    }
                }
            }
        }
    }
    
}
