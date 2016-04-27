import QtQuick 2.2

Item {
    objectName: "floorplan_display"
    id:floorplandisplay
    anchors.fill: parent

    property int scaleFactor:floorplanimage.scale
    property alias bg:phil
    property bool useList:true
    property string backgroundColor: "white"

    Component.onCompleted:{
        floorplan_devices.setCurrentPage(1)
    }

    Connections{
        target:manager
        onOrientationChanged:floorplan_devices.setCurrentPage(1)
    }

    function setScaleFactor(factor){
        console.log("Updating scale")
        var amnt = floorplanimage.scale *(factor/100)

        console.log(floorplanimage.scale+amnt)
        floorplanimage.scale = floorplanimage.scale+amnt
    }

    function setCommandType(){
        if(floorplan_devices.getCurrentFloorPlanType()=== 2)
            return lightingCommands
    }

    function placeQmlSprites(){
        console.log("Placing QML Sprites, "+ floorplan_devices.count()+" devices to create.")
        for (var cnt = 0; cnt < floorplan_devices.count(); cnt++){
            var lcl = floorplan_devices.get(cnt)
            console.log(JSON.stringify(lcl, null, "\t"))
            var c
            console.log("QML Creating Sprite")
            c = Qt.createComponent("FpSprite.qml");
            if(c.status === Component.Loading)
            {   console.log("QML Floorplan object Loading")
                finishPlacingSprites(c,lcl.x,lcl.y, lcl.deviceNum, unknown, lcl.type)
            }
            else if (c.status === Component.Error)
            {
                console.log("QML Floorplan object error::"+ c.errorString())
            }
            else if (c.status === Component.Ready)
            {
                console.log("QML Floorplan object Ready!")
                var sprite = c.createObject(floorplanimage, {"x": lcl.x, "y": lcl.y, "deviceNum": lcl.deviceno, "deviceType": lcl.type});
            }
        }

    }

    function placeSprites(x,y, num, state, devtype){
        var i;
        var pX = x; //x point
        var pY = y; //y point
        var c;
        console.log("Creating Sprite")
        c = Qt.createComponent("FpSprite.qml");
        if(c.status === Component.Loading)
        {   console.log("Component Loading")
            finishPlacingSprites(c,pX,pY, num, state, devtype)
        }
        else if (c.status === Component.Error)
        {
            console.log("Component Error"+ c.errorString())
        }
        else if (c.status === Component.Ready)
        {
            console.log("Component Ready!")
            var sprite = c.createObject(floorplanimage, {"x": pX, "y": pY, "deviceNum": num, "deviceType": devtype});
        }
    }

    function finishPlacingSprites(c,x,y,num, state, devtype){
        console.log("Finishing Creation")
        if(c.status === Component.Ready ){
            var sprite = c.createObject(floorplanimage, {"x": x, "y": y, "deviceNum": num, "deviceType": devtype});
        }
    }

    Connections{
        target: floorplan_devices
        onFloorPlanImageChanged: {
            floorplanimage.source = "image://listprovider/floorplan/"+floorplan_devices.currentPage
        }
    }

    Rectangle{
        id:phil
        anchors.fill: parent
        border.color: "black"
        border.width: scaleX(1) *.25
        radius: 5
        anchors.centerIn: parent
        color: backgroundColor
        opacity:.5
    }


    Item{
        id:mainRect
        objectName: "main_rect"
        anchors.fill: parent
        anchors.margins: 15
        height: floorplandisplay.height
        width:floorplandisplay.width

        Flickable{
            id: imageFlick
            anchors.fill: parent
            contentHeight: floorplanimage.sourceSize.height * floorplanimage.scale + 50
            contentWidth: floorplanimage.sourceSize.width * floorplanimage.scale + 50

            PinchArea {
                id: pinchArea
                anchors.fill: parent
                pinch.dragAxis: Pinch.NoDrag

                onPinchStarted: {
                    imageFlick.interactive = false;
                }
                onPinchFinished: {
                    imageFlick.interactive = true;
                    imageFlick.returnToBounds()
                }
                onPinchUpdated: {
                    imageFlick.contentX += pinch.previousCenter.x - pinch.center.x;
                    imageFlick.contentY += pinch.previousCenter.y - pinch.center.y;
                    setScaleFactor((pinch.scale-pinch.previousScale)*3000)
                }

                Image {
                    objectName: "floorplan_image"
                    id: floorplanimage
                    fillMode: Image.PreserveAspectCrop
                    source: ""
                    anchors.centerIn: parent
                    transformOrigin: Item.Center
                    scale: Math.min((mainRect.height-40)/floorplanimage.sourceSize.height,(mainRect.width-40)/floorplanimage.sourceSize.width)

                    Behavior on scale {
                        PropertyAnimation{
                            duration: 350
                            easing.type: Easing.InBounce
                        }
                    }
                }
            }
        }
    }

    Item{
        height: scaleY(10) + 5
        width: scaleX(35)
        anchors.right: parent.right
        visible:floorplanlist.visible
        Text{
            id:fplabel
            text: qsTr("Floorplans")
            height: scaleY(5)

        }

        ListView{
            id:floorplanlist
            anchors { top: parent.top; bottom: parent.bottom; left: parent.left; right: parent.right }
            width: scaleX(35)
//            model:floorplan_pages
            clip:true
            visible: floorplan_pages.count !==0 ? useList : false

            delegate:Rectangle{
                height: scaleY(9)
                width: scaleX(35)
                color: style.accentcolor
                border.color: style.darkhighlight
                Text {
                    id: desc
                    text: m_description
                }
                MouseArea{
                    anchors.fill: parent
                    onPressed:{
                        floorplan_devices.clearAllSelections()
                        floorplan_devices.setCurrentPage(m_page)
                    }
                }
            }
        }
    }

}

