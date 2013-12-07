import QtQuick 1.1
//phone - default
Item{
    id:navInfo
    width: parent.width
    height: parent.height

    
    Row{
        height: parent.height
        width: parent.width
        spacing:scaleX(2)
        
        StyledText{
            id:pageSplit
            text:"Items per Page: "+ manager.media_pageSeperator
            color:"white"
            fontSize:24
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
        }
        
        //            StyledText{
        //                id:pageNo
        //                text:"Current Page:"+ manager.media_currentPage
        //                color:"white"
        //                fontSize:24
        //            }
        
        StyledText{
            id:itemCount
            text:dataModel.currentItemIndex + "/" + dataModel.totalcells
            color:"white"
            fontSize:24
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
        }


        
//        StyledText{
//            id:letterSort
//            text:"Jump to :"+manager.qs_seek
//            color:"white"
//            fontSize:24
//            MouseArea{
//                anchors.fill: parent
//                onReleased: fileviewscreen.state="alphabets"
//            }
//        }
    }
}
