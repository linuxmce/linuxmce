import QtQuick 2.1

Item {
    property string screenName:"lorem ipsum foo"
    property string screenDescription:"basic description"
    property bool isActive:false
    property Item focusTarget:undefined
    focus:true

    onFocusTargetChanged: {
        if(focusTarget && !focusTarget.activeFocus){
            console.log("focus target updated, activating.")
            focusTarget.forceActiveFocus()
        }
    }

    anchors{
        top:pageLoader.top
        left:pageLoader.left
        right:pageLoader.right
        bottom: pageLoader.bottom
    }
    onActiveFocusChanged: if(activeFocus && !focusTarget.activeFocus){
                              uiOn=false
                              isActive=true
                              if(focusTarget){
                                  console.log("using focus target")
                                  focusTarget.forceActiveFocus()
                              } else if(children.length!==0 ){
                                  console.log("activating first child object for "+screenName)
                                  children[0].forceActiveFocus()
                              }


                          }
}


