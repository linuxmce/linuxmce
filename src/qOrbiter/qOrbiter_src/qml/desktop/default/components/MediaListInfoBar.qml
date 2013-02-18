import QtQuick 1.0
import Qt.labs.shaders 1.0
import AudioVisual 1.0
import "../effects"
import "../components"
import "../animation"
import "../../lib/handlers"
import "../js/ComponentLoader.js" as MyJs


Row{
    id:label_row
    width: childrenRect.width
    height: scaleY(5)
    anchors.centerIn: pos_label
    spacing: scaleX(1)
    StyledText {
        id: grid_position_label
        text: qsTr("You are browsing by:") + manager.i_current_mediaType
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
    }
    
    TextInput{
        id:seperationSetter
        width: page_label.width
        text: manager.media_pageSeperator
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
