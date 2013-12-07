import QtQuick 1.1
//phone - default
Item{
    id:navInfo
    width: parent.width
    height: parent.height*.15
    anchors.top:parent.top
    
    Row{
        height: parent.height
        width: parent.width
        spacing:scaleX(2)
        
        StyledText{
            id:pageSplit
            text:"Items per Page: "+ manager.media_pageSeperator
            color:"white"
            fontSize:24
        }
        
        //            StyledText{
        //                id:pageNo
        //                text:"Current Page:"+ manager.media_currentPage
        //                color:"white"
        //                fontSize:24
        //            }
        
        StyledText{
            id:itemCount
            text:dataModel.currentCells + " of " + dataModel.totalcells
            color:"white"
            fontSize:24
        }
        
        StyledText{
            id:letterSort
            text:"Jump to :"+currentSeekLetter
            color:"white"
            fontSize:24
            MouseArea{
                anchors.fill: parent
                onReleased: fileviewscreen.state="alphabets"
            }
        }
    }
}
