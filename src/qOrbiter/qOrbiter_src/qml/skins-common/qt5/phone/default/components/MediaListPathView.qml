import QtQuick 2.2
import "../components"

PathView{
    id:media_path
    anchors.fill: parent
    model: manager.getDataGridModel("MediaFile", 63)
    visible:current_view_type===3
}
