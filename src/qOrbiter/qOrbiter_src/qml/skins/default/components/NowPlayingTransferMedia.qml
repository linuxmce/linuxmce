import QtQuick 2.2
import org.linuxmce.enums 1.0
import org.linuxmce.screens 1.0
import QtGraphicalEffects 1.0
import enums.media 1.0
import QtQuick.Controls 1.1
import "../."
GenericPopup{
    id:transferMedia
    title:qsTr("Select Entertain Area for %1").arg(dcenowplaying.mediatitle)
    content: Item{
        anchors.fill: parent
        ListModel{id:eaListing}
        Component.onCompleted: {

            var inData= roomList.eaList()

            for(var i = 0; i < inData.length; i++){
                var t = inData[i]

                eaListing.append({
                                     "name":t.ea_name,
                                     "room":t.room,
                                     "ea_number":t.ea_number
                                 }
                                 )
            }
        }

        GenericListModel{
            label:modelCount
            anchors.fill: parent
            model:eaListing
            delegate: LargeStyledButton{
                height: 50
                buttonText:name
                onActivated: mediaHelper.moveMediaStream(String(ea_number), dcenowplaying.getStreamID())
            }
        }

    }
}
