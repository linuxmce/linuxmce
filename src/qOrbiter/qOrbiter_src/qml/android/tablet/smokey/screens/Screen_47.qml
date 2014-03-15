import QtQuick 1.1
import "../components"
//"http://192.168.80.1/lmce-admin/qOrbiterGenerator.php?id=20818"
Item {
    id:files_view_screen
    height: pageLoader.height
    width:manager.appWidth
    state:"viewing"
    property int current_view_type:1
    property int depth:0
    focus:true
    Keys.onReleased: {
        switch(event.key){
        case Qt.Key_MediaPrevious:
        case Qt.Key_Back:
	    if (!manager.goBackGrid())
                event.accepted=false
            break;
        }
    }

    Component.onCompleted: {
        if(manager.q_mediaType===5 ){
         manager.setGridStatus(false)
        }
        setNavigation("MedialistNav.qml")
        hideInfoPanel();

        forceActiveFocus()
    }
  //  state: manager.i_current_mediaType === 5 ? "selection" : "viewing"
    Connections
    {
        target: filedetailsclass
        onShowDetailsChanged:
        {
            files_view_screen.state="detail"
        }
    }
    MultiViewMediaList{
        id:media_view
        anchors.centerIn: parent
    }

    GridView{
        id:typeSelection
        anchors.centerIn: parent
        height:parent.height / 2
        width: parent.width *.85
        model:mediatypefilter
        cellWidth:scaleX(25)
        cellHeight:scaleY(15)
        delegate:
            Item{
            height: childrenRect.height
            width: childrenRect.width

            StyledButton{
              buttonText: name
             textSize: 48
              hitArea.onReleased:{ mediatypefilter.setSelectionStatus(name); files_view_screen.state="viewing"}
            }
        }
    }

    StyledButton{
        id:fileBtn
        buttonText: "Filename Sort"
        height: childrenRect.height+10
       textSize:scaleY(4)
        anchors.top: typeSelection.bottom
        anchors.horizontalCenter: typeSelection.horizontalCenter
        width: typeSelection.width
        onActivated: {
            attribfilter.setSelectionStatus("File");
            files_view_screen.state="viewing"
        }
    }

    MediaListProgressBar{
        id:progress_bar
        anchors.bottom: bottomOptions.top
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Item{
        id:bottomOptions
        height:scaleY(8)
        anchors{
            left:parent.left
            right:parent.right
            bottom:parent.bottom
        }
        Rectangle{
            anchors.fill: parent
            color:"black"
            opacity: .65
        }

        Row{
            anchors{
                left:parent.left
                right:parent.horizontalCenter
                bottom:parent.bottom
                bottomMargin: 5
            }
            StyledButton{
                id:viewBtn
                buttonText: "View"
                height: childrenRect.height+10
               textSize:scaleY(4)

//                onActivated: {
//                    attribfilter.setSelectionStatus("File");
//                    files_view_screen.state="viewing"
//                }
            }

        }
    }

    Loader{
        id:file_details_loader
        height: parent.height
        width: parent.width
        anchors.left: files_view_screen.right
        onStatusChanged: {
            if(file_details_loader.status===Loader.Error){
                file_details_loader.source = "../components/GenericFileDetails.qml"
            }
        }
    }

    states: [
        State {
            name: "selection"
            PropertyChanges {
                target: media_view
                visible:false
            }
            PropertyChanges {
                target: progress_bar
                visible:false
            }
            PropertyChanges {
                target: typeSelection
                visible:true
            }
            PropertyChanges {
                target: fileBtn
                visible:true
            }
            AnchorChanges{
                target: file_details_loader
                anchors.left: parent.right
            }
            StateChangeScript{
                script: {manager.resetModelAttributes; mediatypefilter.resetStates(); attribfilter.resetStates(); }
            }
        //("5", "", "", "", "1,2", "", "13", "", "2", "")
        //("5", "1", "", "", "1,2", "", "12", "", "2", "")
        //("5", "1", "", "", "1,2", "", "52", "", "2", "7691")
        //("5", "1", "", "", "1,2", "", "12", "", "2", "")
        },
        State {
            name: "viewing"
            PropertyChanges {
                target: progress_bar
                visible:true
            }
            PropertyChanges {
                target: media_view
                visible:true
            }
            PropertyChanges {
                target: fileBtn
                visible:false
            }
            PropertyChanges{
                target:typeSelection
                visible:false
            }
            AnchorChanges{
                target: file_details_loader
                anchors.left: parent.right
            }
        },
        State {
            name: "detail"
            PropertyChanges {
                target: progress_bar
                visible:false
            }
            PropertyChanges {
                target: media_view
                visible:false
            }
            PropertyChanges{
                target:typeSelection
                visible:false
            }
            AnchorChanges{
                target: file_details_loader
                anchors.left: parent.left
            }
            PropertyChanges {
                target: file_details_loader
                source:"../components/FileDetails_"+manager.i_current_mediaType+".qml"
            }
        }
    ]



}
