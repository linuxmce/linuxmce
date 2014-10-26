import QtQuick 1.1
import "../components"
import "../js/ComponentLoader.js" as JsLib
import "../../default/lib/handlers"

Item{
    id: mediaParent
    Connections
    {
        target: filedetailsclass
        onShowDetailsChanged: {
            var detailsFile = ''
            switch (manager.i_current_mediaType) {
            case 4: // audio
                detailsFile = 'FileDetails4.qml'
                console.log("Loading audio details")
                break
            case 5: // video
                detailsFile = 'FileDetails5.qml'
                console.log("Loading video details")
                break
            }
            console.log("file",detailsFile,i_current_mediaType)
            if (detailsFile.length) {
                stage.visible = false
                JsLib.loadComponent(mediaParent,detailsFile)
            } else {
                console.log("No details file for media type:" +i_current_mediaType)
                JsLib.loadComponent(mediaParent,'FileDetailsError.qml')
            }
        }
    }


    Rectangle {
        id:stage

        //visible: false
        signal swapStyle()
       anchors.fill: parent
        color: "transparent"
        Clock{
            anchors.right: parent.right
            verticalMirror: true
        }

        // Upper left menu
        MenuBar{
            origin: "top-left"
            items: ListModel {
                ListElement {
                    text: "Home"
                    action: 'setCurrentScreen("Screen_1.qml")'
                }
                ListElement {
                    text: "Back"
                    action: 'goBackGrid()'
                }
            }
        }


        // Build the background
        Image{
            y: scaleY(43.75) // 315
            width: parent.width
            height: scaleY(56.25) // 405
            source: "../img/showcase/lite/showcase_floor.png"
        }
        Image{
            x: scaleX(11.25) // 144
            y: scaleY(80.97) // 583
            width: scaleX(77.58) // 993
            height: scaleY(19.03)  // 137
            source: "../img/showcase/lite/showcase_spot.png"
        }
        Image{
            width: scaleX(36.25) // 464
            height: parent.height
            source: "../img/showcase/showcase_vignette.png"
        }
        Image{
            x: scaleX(63.75) // 816
            width: scaleX(36.25) // 464
            height: parent.height
            source: "../img/showcase/showcase_vignette-r.png"
            //mirror: true // not supported?
        }

        Component {
            id: appDelegate
            Item {
                id: wrapper
                width: scaleX(15.63) // 200;
                height: scaleY(38.2) // 275
                scale: PathView.iconScaling
                z: PathView.iconZ
                //            Rectangle{
                Image {

                    anchors.fill: parent
                    clip: true
                    //                radius:7
                    smooth: true
                    transform: Rotation {
                        origin.x: wrapper.width/2; origin.y: wrapper.height/2
                        axis.x: 0; axis.y: 1; axis.z: 0     // rotate around y-axis
                        angle: PathView.iconAngle
                    }

                    //                Image {
                    id: myIcon
                    //                    smooth: true
                    source:"image://datagridimg/"+id ;
                    //                    anchors.fill: parent
                    fillMode: Image.PreserveAspectCrop;
                    asynchronous: true
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {

                            if(path_view1.currentIndex == index){
                                console.log("clicked",id)
                                manager.setStringParam(4, id)
                            } else {
                                path_view1.currentIndex = index
                            }
                        }
                    }

                }
                //            }

                //            Text {
                //                anchors { top: myIcon.bottom; horizontalCenter: wrapper.horizontalCenter }
                //                text: name
                //                smooth: true
                //                visible: wrapper.PathView.isCurrentItem
                //            }




            }
        }

        PathView {
            id: path_view1
            y: scaleY(55)
            //z: 1
            width: scaleX(80)
            height: scaleY(30)
            //anchors.topMargin: 40
            anchors.horizontalCenter: parent.horizontalCenter
            model:dataModel
            focus: true
            //clip: true

            interactive: true
            Keys.enabled: true
            //anchors.fill: parent
            //offset: 2
            pathItemCount: 16;
            //highlight: appHighlight

            preferredHighlightBegin: 0.462
            preferredHighlightEnd: 0.615
            //highlightRangeMode: PathView.StrictlyEnforceRange
            //highlightMoveDuration: 100
            dragMargin: parent.height/3
            delegate: appDelegate
            path: Path {
                startX: scaleX(4.92)
                startY: path_view1.height/2

                PathPercent { value: 0.0 }
                PathAttribute { name: "iconScaling"; value:  0.8 }
                PathAttribute { name: "iconAngle"; value:  50 }
                PathAttribute { name: "iconZ"; value:  0 }

                PathLine { x: (path_view1.width*0.37); y: path_view1.height/2 }
                PathPercent { value: 0.385 }
                PathAttribute { name: "iconScaling"; value:  0.8 }
                PathAttribute { name: "iconAngle"; value:  50 }
                PathAttribute { name: "iconZ"; value:  10 }

                PathLine { x: path_view1.width/2; y: path_view1.height/2 }
                PathPercent { value: 0.462 }
                PathAttribute { name: "iconScaling"; value:  1.0 }
                PathAttribute { name: "iconAngle"; value:  0 }
                PathAttribute { name: "iconZ"; value:  11 }

                PathLine { x: (path_view1.width*0.63); y: path_view1.height/2 }
                PathPercent { value: 0.538 }
                PathAttribute { name: "iconScaling"; value:  0.8 }
                PathAttribute { name: "iconAngle"; value:  -50 }
                PathAttribute { name: "iconZ"; value:  10 }

                PathLine { x: path_view1.width; y: path_view1.height/2 }
                PathPercent { value: 1.0 }
                PathAttribute { name: "iconScaling"; value:  0.8 }
                PathAttribute { name: "iconAngle"; value:  -50 }
                PathAttribute { name: "iconZ"; value:  0 }

            }
            onCurrentIndexChanged: {
                console.log(dataModel.get(currentIndex,"name"));
                txtName.text = dataModel.get(currentIndex,"name")
            }

        }
        Text{
            id: txtName
            color: "black"
            //width: 400
            font.pixelSize: 30
            font.family: aeonRss.name;
            opacity: .8
            anchors.horizontalCenter: parent.horizontalCenter
            y: scaleY(93)
        }
        //    Rectangle{
        //        border.color: "black"
        //        color: "transparent"
        //        height: 275
        //        width: scaleX(80)
        //        //anchors.verticalCenter: parent.verticalCenter
        //        anchors.centerIn: parent
        //    }

        // Media filter menus

        // Attributes

        Rectangle{
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            //width: scaleX(35.23) // 451
            width: recAttr.width
            height:  scaleY(3.75) // 27
            color: "black"
            opacity: .7
        }

        Rectangle{
            id: recAttr
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            //width: scaleX(35.23) // 451
            width: txtAttr.width+scaleX(1.5)
            height:  scaleY(3.75) // 27
            color: "transparent"
            Text{
                id: txtAttr
                text: "Attributes"
                color: "white"
                font.family: aeonRss.name;
                font.pixelSize: parent.height*.6;
                anchors.centerIn: parent
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        mnuGenre.height=0;
                        mnuFile.height=0;
                        mnuMType.height=0;
                        if (mnuAttr.height) {
                            mnuAttr.height = 0;
                        } else {
                            if ((attribfilter.rowCount()*recAttr.height)<scaleY(50)) {
                                mnuAttr.height = attribfilter.rowCount()*scaleY(3.75);
                            } else {
                                mnuAttr.height = scaleY(50);
                            }
                        }
                    }
                }
            }
        }

        Rectangle{
            id: mnuAttr
            height: 0
            width: recAttr.width*2
            color: "transparent"
            //opacity: .7
            anchors.bottom: recAttr.top
            anchors.left: recAttr.left
            //visible: false
            Behavior on height {
                NumberAnimation {
                    duration: 100
                    easing.type: Easing.InOutQuad
                }
            }

            ListView{
                anchors.fill: parent
                clip: true
                height: scaleY(50)
                model: attribfilter
                delegate: Rectangle{
                    height:  scaleY(3.75) // 27
                    width: mnuAttr.width
                    color: "transparent"
                    Rectangle{
                        anchors.fill: parent
                        color: "black"
                        opacity: .7
                    }
                    Row{
                        anchors.fill: parent
                        spacing: scaleX(.78)
                        anchors.leftMargin: scaleX(.78)
                        Rectangle {
                            id: recCheck
                            anchors.leftMargin: scaleX(.78)
                            width: parent.height*.5
                            height: parent.height*.5
                            anchors.verticalCenter: parent.verticalCenter
                            color: status?"white":"transparent"
                            border.color: "white"
                        }
                        Text{
                            text: name
                            color: "white"
                            font.family: aeonRss.name;
                            font.pixelSize: parent.height*.6;
                            anchors.verticalCenter: parent.verticalCenter
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    var newStatus = attribfilter.setSelectionStatus(name)
                                    recCheck.color = newStatus?"white":"transparent"
                                }
                            }
                        }
                    }
                }
            }
        }

        // Genres

        Rectangle{
            anchors.left: recAttr.right
            anchors.bottom: parent.bottom
            //width: scaleX(35.23) // 451
            width: recGenre.width
            height:  scaleY(3.75) // 27
            color: "black"
            opacity: .7
        }

        Rectangle{
            id: recGenre
            anchors.left: recAttr.right
            anchors.bottom: parent.bottom
            //width: scaleX(35.23) // 451
            width: txtGenre.width+scaleX(1.5)
            height:  scaleY(3.75) // 27
            color: "transparent"
            Text{
                id: txtGenre
                text: "Genre"
                color: "white"
                font.family: aeonRss.name;
                font.pixelSize: parent.height*.6;
                anchors.centerIn: parent
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        mnuAttr.height=0;
                        mnuFile.height=0;
                        mnuMType.height=0;
                        if (mnuGenre.height) {
                            mnuGenre.height = 0;
                        } else {
                            if ((genrefilter.rowCount()*recGenre.height)<scaleY(50)) {
                                mnuGenre.height = genrefilter.rowCount()*scaleY(3.75);
                            } else {
                                mnuGenre.height = scaleY(50);
                            }
                        }
                    }
                }
            }
        }

        Rectangle{
            id: mnuGenre
            height: 0
            width: recGenre.width*2
            color: "transparent"
            //opacity: .7
            anchors.bottom: recGenre.top
            anchors.left: recGenre.left
            //visible: false
            Behavior on height {
                NumberAnimation {
                    duration: 100
                    easing.type: Easing.InOutQuad
                }
            }

            ListView{
                anchors.fill: parent
                clip: true
                height: scaleY(50)
                model: genrefilter
                delegate: Rectangle{
                    height:  recGenre.height
                    width: mnuGenre.width
                    color: "transparent"
                    Rectangle{
                        anchors.fill: parent
                        color: "black"
                        opacity: .7
                    }
                    Row{
                        anchors.fill: parent
                        spacing: scaleX(.78)
                        anchors.leftMargin: scaleX(.78)
                        Rectangle {
                            id: recCheckGenre
                            anchors.leftMargin: scaleX(.78)
                            width: parent.height*.5
                            height: parent.height*.5
                            anchors.verticalCenter: parent.verticalCenter
                            color: status?"white":"transparent"
                            border.color: "white"
                        }
                        Text{
                            text: name
                            color: "white"
                            font.family: aeonRss.name;
                            font.pixelSize: parent.height*.6;
                            anchors.verticalCenter: parent.verticalCenter
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    var newStatus = genrefilter.setSelectionStatus(name)
                                    recCheckGenre.color = newStatus?"white":"transparent"
                                }
                            }
                        }
                    }
                }
            }
        }

        // File formats

        Rectangle{
            anchors.left: recGenre.right
            anchors.bottom: parent.bottom
            //width: scaleX(35.23) // 451
            width: recFile.width
            height:  scaleY(3.75) // 27
            color: "black"
            opacity: .7
        }

        Rectangle{
            id: recFile
            anchors.left: recGenre.right
            anchors.bottom: parent.bottom
            //width: scaleX(35.23) // 451
            width: txtFile.width+scaleX(1.5)
            height:  scaleY(3.75) // 27
            color: "transparent"
            Text{
                id: txtFile
                text: "Resolution"
                color: "white"
                font.family: aeonRss.name;
                font.pixelSize: parent.height*.6;
                anchors.centerIn: parent
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        mnuAttr.height=0;
                        mnuGenre.height=0;
                        mnuMType.height=0;
                        if (mnuFile.height) {
                            mnuFile.height = 0;
                        } else {
                            if ((fileformatmodel.rowCount()*recFile.height)<scaleY(50)) {
                                mnuFile.height = genrefilter.rowCount()*scaleY(3.75);
                            } else {
                                mnuFile.height = scaleY(50);
                            }
                        }
                    }
                }
            }
        }

        Rectangle{
            id: mnuFile
            height: 0
            width: recFile.width*2
            color: "transparent"
            //opacity: .7
            anchors.bottom: recFile.top
            anchors.left: recFile.left
            //visible: false
            Behavior on height {
                NumberAnimation {
                    duration: 100
                    easing.type: Easing.InOutQuad
                }
            }

            ListView{
                anchors.fill: parent
                clip: true
                height: scaleY(50)
                model: fileformatmodel
                delegate: Rectangle{
                    height:  recFile.height
                    width: mnuFile.width
                    color: "transparent"
                    Rectangle{
                        anchors.fill: parent
                        color: "black"
                        opacity: .7
                    }
                    Row{
                        anchors.fill: parent
                        spacing: scaleX(.78)
                        anchors.leftMargin: scaleX(.78)
                        Rectangle {
                            id: recCheckFile
                            anchors.leftMargin: scaleX(.78)
                            width: parent.height*.5
                            height: parent.height*.5
                            anchors.verticalCenter: parent.verticalCenter
                            color: status?"white":"transparent"
                            border.color: "white"
                        }
                        Text{
                            text: name
                            color: "white"
                            font.family: aeonRss.name;
                            font.pixelSize: parent.height*.6;
                            anchors.verticalCenter: parent.verticalCenter
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    var newStatus = fileformatmodel.setSelectionStatus(name)
                                    recCheckFile.color = newStatus?"white":"transparent"
                                }
                            }
                        }
                    }
                }
            }
        }

        // Media types

        Rectangle{
            anchors.left: recFile.right
            anchors.bottom: parent.bottom
            //width: scaleX(35.23) // 451
            width: recMType.width
            height:  scaleY(3.75) // 27
            color: "black"
            opacity: .7
        }

        Rectangle{
            id: recMType
            anchors.left: recFile.right
            anchors.bottom: parent.bottom
            //width: scaleX(35.23) // 451
            width: txtMType.width+scaleX(1.5)
            height:  scaleY(3.75) // 27
            color: "transparent"
            Text{
                id: txtMType
                text: "Media Types"
                color: "white"
                font.family: aeonRss.name;
                font.pixelSize: parent.height*.6;
                anchors.centerIn: parent
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        mnuAttr.height=0;
                        mnuGenre.height=0;
                        mnuFile.height=0;
                        if (mnuMType.height) {
                            mnuMType.height = 0;
                        } else {
                            if ((fileformatmodel.rowCount()*recMType.height)<scaleY(50)) {
                                mnuMType.height = genrefilter.rowCount()*scaleY(3.75);
                            } else {
                                mnuMType.height = scaleY(50);
                            }
                        }
                    }
                }
            }
        }

        Rectangle{
            id: mnuMType
            height: 0
            width: recFile.width*2
            color: "transparent"
            //opacity: .7
            anchors.bottom: recMType.top
            anchors.left: recMType.left
            //visible: false
            Behavior on height {
                NumberAnimation {
                    duration: 100
                    easing.type: Easing.InOutQuad
                }
            }

            ListView{
                anchors.fill: parent
                clip: true
                height: scaleY(50)
                model: mediatypefilter
                delegate: Rectangle{
                    height:  recMType.height
                    width: mnuMType.width
                    color: "transparent"
                    Rectangle{
                        anchors.fill: parent
                        color: "black"
                        opacity: .7
                    }
                    Row{
                        anchors.fill: parent
                        spacing: scaleX(.78)
                        anchors.leftMargin: scaleX(.78)
                        Rectangle {
                            id: recCheckMType
                            anchors.leftMargin: scaleX(.78)
                            width: parent.height*.5
                            height: parent.height*.5
                            anchors.verticalCenter: parent.verticalCenter
                            color: status?"white":"transparent"
                            border.color: "white"
                        }
                        Text{
                            text: name
                            color: "white"
                            font.family: aeonRss.name;
                            font.pixelSize: parent.height*.6;
                            anchors.verticalCenter: parent.verticalCenter
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    var newStatus = mediatypefilter.setSelectionStatus(name)
                                    recCheckMType.color = newStatus?"white":"transparent"
                                }
                            }
                        }
                    }
                }
            }
        }
        Timer{
            interval: 500; running: true; repeat: false
            onTriggered: {
                txtName.text = dataModel.get(0,"name")
            }
        }
    }
}
