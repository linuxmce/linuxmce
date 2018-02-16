import QtQuick 2.1
import org.linuxmce.enums 1.0

Item{
    id:ftr

    width: qmlroot.width
    height:scaleY(14)
    visible:false
    focus:true
    anchors{
        bottom:qmlroot.bottom
    }
    property bool isActive: false
    property Item focusedItem:null

    onActiveFocusChanged:{
        if(activeFocus){
            isActive=true
            scenarioList.forceActiveFocus()
            console.log("footer gained focus and assigned to scenarios.")
        }
        else{

            console.log("Footer lost active focus")
        }
    }
    //    Component.onCompleted: ftr.state = "hidden"



    Connections{
        target: manager
        onRoomChanged:{

        }
        onDceRemoteCommand:{
            if(focusedItem)
                focusedItem.processRemoteCommand(cmd, name)
            else
                console.log("no focused item set!")
        }
    }

    GradientFiller {
        opacity: .75
    }

    Item{
        id:npOptions
        width: (dcenowplaying.b_mediaPlaying || dceplayer.mediaPlaying) ? scaleX(8) :0
        anchors{
            top:parent.top
            bottom:parent.bottom
            margins: 5
            left:parent.left
        }
    }

    ListView{
        id:scenarioList
        height: parent.height / 2
        anchors{
            top:parent.top
            left:npOptions.right
            right:parent.right
            margins: 5
        }
        function nextItem(){
            if(currentIndex < count-1){
                currentIndex++
            } else {
                currentIndex=0
            }
            scenarioList.currentItem.forceActiveFocus()
        }
        function lastItem(){
            if(currentIndex!==0){
                currentIndex--
            }else {
                currentIndex=count-1
            }
            scenarioList.currentItem.forceActiveFocus()
        }

        property int itemWidth:(scenarioList.width)/6
        property int lastIndex:0
        spacing:scaleX(1)
        orientation: ListView.Horizontal

        onActiveFocusChanged:{

            console.log("Active focus on Scenario listmodel is " + activeFocus);
            if(activeFocus){
                scenarioList.currentIndex=lastIndex
            } else {
                lastIndex = scenarioList.currentIndex
                scenarioList.currentIndex=-1
            }
        }
        function processRemoteCommand(command, ident){
            console.log("ScenarioListItem::Processing command #"+command+"-"+ident)

            switch(command){
            case RemoteCommands.MoveLeft:
                console.log("ScenarioListItem::Moving left")
                scenarioList.nextItem()
                break;

            case RemoteCommands.MoveRight:
                console.log("ScenarioListItem::moving right")
                scenarioList.lastItem()
                break;
            case RemoteCommands.BackClearEntry:
                uiOn=false;
                break;

            default:
                console.log("ScenarioListitem::Failedto process command #"+command+"-"+ident)
                return;

            }
        }

        clip:false
        focus:true
        model:scenarios
        delegate: Item{
            id:scenarioParent
            height: parent.height
            width:scenarioList.itemWidth
            focus:true
            property bool active:index===scenarioList.currentIndex
            onActiveChanged: {
                if(active){
                    console.log(name+ " activated, focusing")
                    scenarioParent.forceActiveFocus()
                    focusedItem = scenarioParent
                    if(manager.currentScreen!=="Screen_47.qml"){
                        if(modelName==="currentRoomLights")
                            submodel.model = currentRoomLights
                        else if(modelName==="currentRoomMedia")
                            submodel.model = currentRoomMedia
                        else if(modelName==="currentRoomClimate")
                            submodel.model = currentRoomClimate
                        else if(modelName==="currentRoomTelecom")
                            submodel.model=currentRoomTelecom
                        else if(modelName==="currentRoomSecurity")
                            submodel.model= currentRoomSecurity
                        else if (modelName==="advancedMenu")
                            submodel.model=advancedMenu
                    }else if (manager.currentScreen==="Screen_47"){

                        if(name==="Attribute")
                            console.log("attribute selected")
                    }
                    submodel.currentIndex = -1
                }
            }

            onActiveFocusChanged: {
                if(activeFocus){
                    console.log("processing focus")
                    focusedItem = scenarioParent
                    if(manager.currentScreen!=="Screen_47.qml"){
                        if(modelName==="currentRoomLights")
                            submodel.model = currentRoomLights
                        else if(modelName==="currentRoomMedia")
                            submodel.model = currentRoomMedia
                        else if(modelName==="currentRoomClimate")
                            submodel.model = currentRoomClimate
                        else if(modelName==="currentRoomTelecom")
                            submodel.model=currentRoomTelecom
                        else if(modelName==="currentRoomSecurity")
                            submodel.model= currentRoomSecurity
                        else if (modelName==="advancedMenu")
                            submodel.model=advancedMenu
                    }else if (manager.currentScreen==="Screen_47"){

                        if(name==="Attribute")
                            console.log("attribute selected")
                    }
                    submodel.currentIndex = -1
                }

            }
            function processRemoteCommand(command, ident){
                console.log("ScenarioParent::Processing command #"+command+"-"+ident)
                switch(command){
                case RemoteCommands.MoveDown:

                    if(submodel.currentIndex !==0 && submodel.count !==0){
                        console.log("subModel::moving down ")
                        submodel.decrementCurrentIndex()
                    } else{
                        console.log("activating meta row")
                        metarow.forceActiveFocus()
                        submodel.currentIndex = -1
                    }

                    break;
                case RemoteCommands.MoveUp:
                    console.log("subModel::move up")
                    submodel.incrementCurrentIndex()
                    break;
                case RemoteCommands.EnterGo:

                pressed()

                    break;
                case RemoteCommands.MoveLeft:
                    scenarioList.nextItem()
                    break;

                case RemoteCommands.MoveRight:
                    scenarioList.lastItem()
                    break;

                default:
                    return;

                }
            }

            Rectangle{
                anchors.fill: parent
                radius:5
                opacity: .75
                color: scenarioList.currentIndex===index ? appstyle.listItemActiveBgColor : appstyle.listItemBgColor
            }


            StyledText{
                anchors.centerIn: parent
                text:name
                font.pixelSize:headerText
                font.bold: true
                font.capitalization: Font.SmallCaps
                color:appstyle.lighttext
            }

            Rectangle{
                anchors.fill: parent
                gradient: appstyle.buttonGradient
                radius:5
                opacity: .65
            }

            Keys.onTabPressed: {
                swapFocus();ftr.state="hidden"
            }
            Keys.onDownPressed: {
                if(submodel.currentIndex !==0  && submodel.count !==0){
                    submodel.decrementCurrentIndex()
                }
                else{
                    metarow.forceActiveFocus()
                    submodel.currentIndex = -1
                }
            }

            Keys.onUpPressed: submodel.incrementCurrentIndex()
            Keys.onEnterPressed: { pressed() }
            Keys.onPressed: {
                hideUiTimer.restart()
                console.log("Pressed-"+manager.dumpKey(event.key))
                if(event.key === Qt.Key_Enter || event.key===Qt.Key_Return )
                {
                    if(submodel.currentIndex !==-1){
                        pressed()
                        scenarioList.currentIndex=-1

                    }else{
                        manager.setCurrentScreen("Screen_"+(index+2)+".qml")
                    }
                    swapFocus()
                }
            }
            signal pressed()


            ListView{
                id:submodel
                height:scenarioList.currentIndex === index ?scaleY(85) : 0
                width: parent.width
                focus: true
                rotation: 180
                orientation:ListView.Vertical
                keyNavigationWraps: true
                clip:true
                anchors.bottom: parent.top
                anchors.bottomMargin: 10
                spacing:scaleY(1)
                delegate: Item{
                    rotation: 180
                    height:label.paintedHeight
                    width: scenarioList.itemWidth


                    GradientFiller{
                        fillColor: submodel.currentIndex === index ? "black" : appstyle.button_system_color
                        opacity: submodel.currentIndex === index ? .75 : .55
                        grpRadius: 5
                    }

                    StyledText {
                        id:label
                        color:appstyle.lighttext
                        text: title
                        font.bold: submodel.currentIndex === index
                        width:parent.width
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        anchors.centerIn: parent
                        font.pointSize: submodel.currentIndex === index ? headerText : mediumText

                    }
                    Connections{
                        target:scenarioParent
                        onPressed:{

                            if(submodel.currentIndex === index && submodel.model !==advancedMenu){
                                console.log("executing cmd grp")
                                manager.execGrp(params)
                                showMenu(false)
                            }else{
                                // console.log(submodel.model[index].params)
                                if(submodel.currentIndex!==index){
                                    //console.log(advancedMenu.get(index).params)
                                    return;
                                }
                                switch(params){
                                case 1:
                                    manager.quickReload()
                                    break;
                                case 2:
                                    manager.regenOrbiter(manager.iPK_Device)
                                    break;
                                case 3:
                                    console.log("Show Power!")
                                    break;
                                case 5:
                                    manager.setCurrentScreen("Screen_44.qml")
                                    break;
                                case 7:
                                    manager.exitApp()
                                    break;
                                default:
                                    console.log(params)
                                    break;
                                }
                            }
                        }
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked:{

                            if(submodel.model !==advancedMenu)
                            {
                                manager.execGrp(params)
                                scenarioList.currentIndex = -1
                                //                                swapFocus()
                                ftr.state = "hidden"
                            }
                            else{
                                switch(params){
                                case 1:
                                    manager.quickReload()
                                    break;
                                case 2:
                                    manager.regenOrbiter(manager.iPK_Device)
                                    break;
                                case 3:
                                    console.log("Show Power!")
                                    break;
                                case 5:
                                    manager.setCurrentScreen("Screen_44.qml")
                                    break;
                                case 7:
                                    manager.exitApp()
                                    break;
                                default:
                                    console.log(params)
                                    break;
                                }
                            }
                        }
                    }
                }

                Behavior on height {
                    PropertyAnimation{
                        duration: 500
                    }

                }
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {scenarioList.currentIndex = index; forceActiveFocus()}
            }

        }


    }
    FocusRow{
        id:metarow
        height: parent.height/2
        anchors{
            top:scenarioList.bottom
            right:parent.right
            left:npOptions.right
            bottom:parent.bottom
            margins: 5
        }

        onActiveFocusChanged: {
            if(activeFocus){
                focusedItem = metarow
            }
        }

        Keys.onUpPressed: scenarioList.forceActiveFocus()

        FocusButton{
            text:manager.sPK_User
            fontSize: 22
            height: parent.height
        }

        FocusButton{
            text:roomList.currentRoom+"::"+roomList.currentEA
            fontSize: 22
            height: parent.height
            width:textObj.paintedWidth
            onSelect:{ overLay.source = "RoomSelector.qml"; ftr.state = "hidden" }

        }
    }
    Clock{
        id:timekeeper
        anchors{
            top:metarow.top
            bottom: metarow.bottom
            right:metarow.right
        }

    }

    Keys.onTabPressed:swapFocus()


    states: [
        State {
            name: "hidden"
            extend: ""
            when:!uiOn
            PropertyChanges {
                target: ftr
                isActive:false
                visible:true
                focusedItem:null
            }

            AnchorChanges{
                target:ftr
                anchors.bottom: undefined
                anchors.top: qmlroot.bottom
            }
        },
        State {
            name: "showing"
            when:uiOn
            extend:""
            PropertyChanges {
                target: ftr
                isActive:true
                visible:true
            }

            AnchorChanges{
                target:ftr
                anchors.top: undefined
                anchors.bottom: qmlroot.bottom
            }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"

            AnchorAnimation{
                duration: appstyle.globalAnimationSpeed
                easing.type: appstyle.globalAnimationEasing

            }
        }
    ]

}
