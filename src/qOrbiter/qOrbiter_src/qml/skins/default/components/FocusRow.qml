import QtQuick 2.2
import org.linuxmce.enums 1.0
import "../components"
import "../."

Row{
    id:optionContainer
    spacing:5
    focus:true
    property int currentIndex:-1
    onActiveFocusChanged: if(activeFocus) currentIndex=0; else currentIndex = -1
    onCurrentIndexChanged: {
        if(currentIndex==-1)
            return;

        optionContainer.children[currentIndex].forceActiveFocus()
    }

    Keys.onLeftPressed: moveLeft()
    Keys.onRightPressed: moveRight()
    Keys.onEscapePressed: escapeRow()
    Keys.onDownPressed: escapeRow()
    Keys.onUpPressed: escapeRow()
    signal escapeRow()
    signal boundary()
    function moveRight(){
        if(currentIndex < optionContainer.children.length-1) {
            currentIndex++
            boundary()
        }
        else
            currentIndex=0
    }
    
    function moveLeft(){
        if(currentIndex==0){
            currentIndex=optionContainer.children.length-1
            boundary()
        } else {
            currentIndex--
        }
    }
    

    

    
}
