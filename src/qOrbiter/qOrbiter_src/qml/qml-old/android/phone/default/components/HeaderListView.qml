import QtQuick 1.1

Item{
    id:headerListView
    property bool useColumns:listTitle.length !== 0 ? true : false
    property string listTitle:""
    property int itemHeight:scaleY(12)
    property variant columnTitles:[]
    property variant columnSpacing:[]
    property variant returnProperties:[]
    property string headerBgColor:appstyle.headerBgColor
    property int columnCount: columnTitles.length
    property color listBgColor: appstyle.listItemBgColor
    property color listBgActiveColor:appstyle.listItemActiveBgColor
    property color listTextInactiveColor:appstyle.listItemTextInactiveColor
    property color listTextActiveColor:appstyle.listItemTextActiveColor
    property color contentBg:appstyle.contentBgColor
    property alias headerListModel: displayList.model
    property Item delegateType:null
    property string displayProperty:""
    signal activationCallback(variant props);
    property bool isUsingIndex:false

    clip:true

    Rectangle{
        anchors.fill: parent
        id:backing
        color:contentBg
        gradient:appstyle.bgContentGradient
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
            fontSize: scaleY(5)
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


            height:itemHeight
            clip:true

            Rectangle{
                anchors.fill: parent
                color:ms.pressed ? appstyle.listItemPressedBgColor : displayList.currentIndex===index ? listBgActiveColor : "black"
                opacity: .65
                border.color: "white"
                border.width: 1
            }

            StyledText{
                text:isUsingIndex ? displayList.model[index][displayProperty] : model[displayProperty] //displayList.model[index][displayProperty]
                fontSize:manager.isProfile ? scaleY(4) : scaleY(5)
                isBold: true
                color: ms.pressed ? listTextActiveColor : displayList.currentIndex===index ? listTextActiveColor : listTextInactiveColor
                anchors.centerIn: parent

            }

            MouseArea{
                id:ms
                anchors.fill: parent
                onClicked: {
                    displayList.currentIndex=index

                    var obj = new Object;
                    var mdl;

                    if(isUsingIndex){
                        mdl = displayList.model[index]
                    } else {

                        mdl =  model
                    }

                    for(var prop in returnProperties){

                        if(mdl[returnProperties[prop]]!==undefined){
                            if(mdl[returnProperties[prop]]){
                                obj[returnProperties[prop]]=mdl[returnProperties[prop]]
                            }

                        } else {

                        }
                    }
                    activationCallback(obj)
                }
            }
        }

    }


}
