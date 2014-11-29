import QtQuick 1.1

import AudioVisual 1.0
import "../effects"
import "../components"
import "../animation"
import "../../../skins-common/lib/handlers"
import "../js/ComponentLoader.js" as MyJs


Row{
    id:label_row
    width: visible ? childrenRect.width : 0
    height: visible ? scaleY(5) :0
    anchors.centerIn: parent
    spacing: scaleX(1)
    StyledText {
        id: grid_position_label
        text: qsTr("You are browsing by:") + manager.q_mediaType
        font.pixelSize: 14
        color: "aliceblue"
    }
    StyledText {
        id: grid_subtype
        text: qsTr("Sorted by subtype:  ") + manager.q_subType
        color: "aliceblue"
    }
    StyledText {
        id: grid_attritbute_label
        text: qsTr("Sorted by Attribute:  ") + manager.q_attributetype_sort
        color: "aliceblue"
    }
    StyledText {
        id: page_label
        text: qsTr("Current Page") + manager.media_currentPage
        color: "aliceblue"
        visible: manager.media_currentPage !==0
    }
    
    TextInput{
        id:seperationSetter
        width: page_label.width
        height: parent.height
        text: manager.media_pageSeperator
        font.pixelSize: scaleY(2)
        onFocusChanged: activeFocus ? text="" : text=manager.media_pageSeperator
        color: "aliceblue"
        Keys.onEnterPressed: {
            if (!seperationSetter.text.match("/D"))
            {
                manager.setDceGridSep(seperationSetter.text)
                manager.requestPage(manager.media_currentPage)
            }
        }
    }


    
}
