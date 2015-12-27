import QtQuick 2.4
import "../components"
import "../."
StyledScreen {

    Panel{
        headerTitle: qsTr("Manage Drives", "Manager Disc Drives")



        content:Item{
            id:drivesDisplay
            Component.onCompleted: getDriveList()
            function getDriveList(){
                var l1 = (screenparams.getParam(159)).split(",")
                console.log(l1)
                for (var drive in l1){
                    driveList.append({"drive":l1[drive]} )
                }

            }
            ListModel{
                id:driveList
            }

            GenericListModel{
                label:qsTr("%1 Drives ", "Disc Drives").arg(driveList.count)
                anchors.fill: parent
                dataGrid:86
                dataGridLabel:"Discs"
                delegate: Item{
                    height: Style.listViewItemHeight
                    width: parent.width
                    Rectangle{
                        anchors.fill: parent
                        color:Style.appcolor_background_list
                        opacity: Style.appList_opacity
                    }

                    Row{
                        anchors.fill: parent
                        spacing: 2
                        Column{
                            height: parent.height
                            width: parent.width *.25
                            StyledText{
                                fontSize: 22
                                text:desc
                            }
                            StyledText{
                                fontSize: 22
                                text: isRipping ?  qsTr("Copy in progress") : qsTr("No Copy in Progress")
                            }
                        }

                        Item{
                            width: parent.width*.35
                            height: parent.height
                        }
                        StyledButton{
                            fontSize: 22
                            buttonText:qsTr("Copy Disc", "Copy Disc to Linuxmce")

                        }
                        StyledButton{
                            fontSize: 22
                            buttonText:qsTr("Eject Disc", "Eject disc from drive    ")
                            onActivated: manager.ejectDisc(drive)
                        }
                    }
                }
            }

        }
    }
}

