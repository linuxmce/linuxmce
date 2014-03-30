import QtQuick 2.0

Item{
    id:headerListView
    property bool useColumns:listTitle.length !== 0 ? true : false
    property string listTitle:""
    property variant columnTitles:[]
    property variant columnSpacing:[]
    property variant returnProperties:[]
    property string headerBgColor:style.headerBgColor
    property int columnCount: columnTitles.length
    property color listBgColor: style.listItemBgColor
    property color listBgActiveColor:style.listItemActiveBgColor
    property color contentBg:style.contentBgColor
    property alias headerListModel: displayList.model
    property Item delegateType:null
    property string displayProperty:""
    signal activationCallback(variant props);

    clip:true

    Rectangle{
        anchors.fill: parent
        id:backing
        color:contentBg
        gradient:style.bgContentGradient
        opacity:bgContentOpacity
    }

    Item{
        id:hdrContainer
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right:parent.right
        height: parent.height*.15

        Rectangle{
            anchors.fill: parent
            id:hdrBg
            color:headerBgColor
            opacity:.75
        }

        StyledText{
            id:hdrTxt
            anchors.centerIn: parent
            text:listTitle
            visible:listTitle !==""
            fontSize: scaleY(8)
            isBold: true
        }

        Row{
            id:colLayout
            anchors{
                left:hdrContainer.left
                right:hdrContainer.right
                top:hdrContainer.top
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
    }


    
    ListView{
        id:displayList
        Component.onCompleted: {displayList.currentIndex=-1}
        clip:true
        anchors{
            top:hdrContainer.bottom
            left:headerListView.left
            right:headerListView.right
            bottom:headerListView.bottom
            margins:5
        }

        delegate:Item{
            id:nocolDelegate
            anchors{
                left:parent.left
                right:parent.right
            }
            height:scaleY(9)
            clip:true
            Rectangle{
                anchors.fill: parent
                color:ms.pressed ? style.listItemPressedBgColor : displayList.currentIndex===index ? listBgActiveColor : "black"
                opacity: .65
                border.color: "white"
                border.width: 1
            }

            StyledText{
                text:model[displayProperty] //displayList.model[index][displayProperty]
                fontSize: scaleY(5)
                isBold: true
                anchors.centerIn: parent

            }

            MouseArea{
                id:ms
                anchors.fill: parent
                onClicked: {
                    displayList.currentIndex=index
                    var obj = new Object;
                    for(var prop in returnProperties){
                        console.log("Checking "+returnProperties[prop])
                        if(model[returnProperties[prop]]!==undefined){

                            console.log(model[returnProperties[prop]])
                            if(model[returnProperties[prop]]){
                                obj[returnProperties[prop]]=model[returnProperties[prop]]
                            }
                        }

                    }
                    activationCallback(obj)
                }
            }
        }

    }


}
