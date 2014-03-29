import QtQuick 2.0

Item{
    id:headerListView
    property bool useColumns:title.length !== 0 ? true : false
    property string title:""
    property variant columnTitles:[]
    property variant columnSpacing:[]
    property int columnCount: columnTitles.length
    property color listBgColor: "green"
    property variant headerListModel:[]

    Rectangle{
        anchors.fill: parent
        id:backing
        color:listBgColor
    }

    StyledText{
        anchors{
            top:parent.top
            left:parent.left
        }
        text:title
        visible:title !==""
    }

    Row{
        id:colLayout
        anchors{
            left:headerListView.left
            right:headerListView.right
            top:headerListView.top
        }

        Repeater{
            model:columnTitles
            delegate: Item{

                StyledText{
                    text:model.data
                }
            }
        }
    }
    
    ListView{
        id:displayList
        model:headerListModel

    }


}
