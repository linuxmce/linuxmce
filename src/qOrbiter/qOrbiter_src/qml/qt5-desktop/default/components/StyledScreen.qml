import QtQuick 2.1

Item {
    property string screenName:"lorem ipsum foo"
    property string screenDescription:"basic description"
    property bool isActive:false
    focus:true

    anchors{
        top:pageLoader.top
        left:pageLoader.left
        right:pageLoader.right
        bottom: pageLoader.bottom
    }
    onActiveFocusChanged: if(activeFocus){
                              uiOn=false
                              isActive=true
                              if(children.length!==0){
                                  console.log("activating first child object for "+screenName)
                                  children[0].forceActiveFocus()
                              }
                          }

}
