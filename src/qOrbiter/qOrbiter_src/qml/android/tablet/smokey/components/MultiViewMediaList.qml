import QtQuick 1.0
import "../components"

Item{
    id:multi_view_list
    height: parent.height
    width: parent.width
    property int itemBuffer:25

    Loader{
        id:viewLoader
    }

    MediaListListView {
        id: media_list
    }
    
    MediaListGridView{
        id:media_grid
        anchors.fill: parent
        model:dataModel
        visible:current_view_type===2
    }
    
    MediaListPathView {
        id: media_path
    }

    states: [
        State {
            name: "list"
            PropertyChanges {
                target: viewLoader
                source:"MediaListListView.qml"
            }
        },
        State {
            name: "grid"
            PropertyChanges {
                target: viewLoader
                source:"MediaListGridView.qml"
            }
        }, State{

        }

    ]
}
