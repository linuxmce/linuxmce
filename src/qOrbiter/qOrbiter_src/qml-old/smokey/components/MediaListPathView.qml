import QtQuick 1.0
import "../components"

PathView{
    id:media_path
    anchors.fill: parent
    model: manager.getDataGridModel("MediaFile", 63)
    visible:current_view_type===3
}
