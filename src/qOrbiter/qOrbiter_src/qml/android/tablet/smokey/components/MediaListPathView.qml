import QtQuick 1.0
import "../components"

PathView{
    id:media_path
    anchors.fill: parent
    model:dataModel
    visible:current_view_type===3
}
