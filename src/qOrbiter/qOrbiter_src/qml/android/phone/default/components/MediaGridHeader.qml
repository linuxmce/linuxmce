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
        StyledButton{
            buttonText: "A-Z"
            width: scaleX(15)
            onActivated: {
                pageLoader.item.toggleAlphabet()
            }
        }

        StyledText{
            text: "Seeking to :"+pageLoader.item.currentSeekLetter
            width: scaleX(15)

        }

        
        StyledText{
            id:itemCount
            text:dataModel.currentItemIndex + "/" + dataModel.totalcells
            color:"white"
            fontSize:24           
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
