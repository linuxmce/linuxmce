import QtQuick 2.2

Item {
    //this item can be used to connect incoming dce command to the gui. it takes the signal and acts upon them only if the parent object has focus.
    id:inputController

    signal left();
    signal right();
    signal up();
    signal down();
    signal menu();
    signal backClear();
    signal backPriorMenu();
    signal enterGo();
    signal togglePower();
    signal guide();
    signal powerOn();
    signal powerOff();
    Connections{
        target:manager
        onDceRemoteCommand:{
            if(!parent.activeFocus)
                return;
             console.log( parent.objectName + " is handling key" )

            switch(cmd){
            case 548:menu(); break;
            case 240:backPriorMenu();break;
            case 190:enterGo(); break;
            case 126:guide();break;
            case 363:backClear();break
            case 201:down();break;
            case 202:left();break;
            case 200: up();break;
            case 203:right();break;
            case 194:togglePower();break
            case 193:powerOn();break
            case 192:powerOff();break
            default:console.log("Unhandled dce remote command for " + parent.objectName)
            }
        }
    }

}

