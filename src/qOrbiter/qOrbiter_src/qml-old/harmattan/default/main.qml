import QtQuick 1.1
import com.nokia.meego 1.0

PageStackorbiterWindow {
    id: apporbiterWindow
    initialPage: Splash { }

    platformStyle: defaultStyle;
    property bool enableSwipe: true;

    PageStackorbiterWindowStyle { id: defaultStyle; }
    PageStackorbiterWindowStyle {
        id: customStyle;
        background: "image://theme/meegotouch-wallpaper-portrait";
        backgroundFillMode: Image.PreserveAspectCrop
    }

    function screenchange(screenname)
    {
        var component = Qt.createComponent("../screens/"+screenname);
        if (component.status == Component.Ready)
        {
            if (screenname == "Screen_70.qml")
            {
                theme.inverted=true;
            }
            else if (screenname == "Screen_47.qml")
            {
                theme.inverted=true;
            }
            else if (screenname == "Screen_47_details.qml")
            {
                theme.inverted=true;
            }
            else
            {
                theme.inverted=false;
            }
            pageStack.push(component);
        }
        else
            console.log("Unable to load screen: "+component.errorString());
    }
}


//Item {
//     id: item
//     signal close()
//     signal changeScreen(string s)
//     property string locationinfo: "standby"

//    // Style {id:style}
//    width: style.orbiterW;
//    height: style.orbiterH;

//     function screenchange(screenname )
//     {
//         if(pageLoader.source = "../screens/"+screenname)
//         {
//             console.log("Command to change to:" + screenname+ " was successfull")
//         }
//         else
//         {
//             console.log("Command to change to:" + screename + " failed!")
//         }
//     }

//     Loader {
//         id:pageLoader
//         objectName: "loadbot"
//         source: "Splash.qml"
//         onLoaded: {

//             console.log("Screen Changed:" + pageLoader.source)

//            }
//         }
// }
