import QtQuick 2.2
import "."

Item {
    id:setup_screen_root

    property alias title:step_label.text
    property alias contentItem:content_area.sourceComponent
    property alias listmodel:list.model
    property variant trackedProperty
    property bool custom:false
    signal selectionMade(int id, string choice)
    clip:true
    Rectangle{
        id:setup_hdr
        color:enabled ? style.appcolor_background_medium :" grey"
        radius:5
        width:parent.width
        anchors{
            top:parent.top
           }
        height: style.appNavigation_panelHeight
    }

    Text{
        id:step_label
        anchors.fill: setup_hdr
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        MouseArea{
            anchors.fill: parent
            onClicked: console.log("wtf")
        }
    }
    Image{
        id:next_step
        source:"images/vertical_arrow.png"
        height: setup_hdr.height*.65
        fillMode: Image.PreserveAspectFit
        rotation: -90
        opacity: ms.enabled ? 1 : .45
        anchors{
            right: setup_hdr.right
            rightMargin:scaleX(2)
            verticalCenter: setup_hdr.verticalCenter
        }
        MouseArea{
            id:ms
            enabled: trackedProperty!==-1
            anchors.fill: parent
            onClicked: selectionSteps.incrementCurrentIndex()
        }
    }

    Image{
        id:last_step
        source:"images/vertical_arrow.png"
        height: setup_hdr.height*.65
        fillMode: Image.PreserveAspectFit
        rotation: 90
        anchors{
            left: setup_hdr.left
            leftMargin:scaleX(2)
            verticalCenter: setup_hdr.verticalCenter
        }

        MouseArea{
            anchors.fill: parent
            onClicked: selectionSteps.decrementCurrentIndex()
        }
    }

    GenericSplashList{
        id:list
        label:qsTr("%1 %2").arg(modelCount).arg(modelCount<2?"Selection":"Selections")
        visible: !custom
        enabled: visible
        anchors{
            top:setup_hdr.bottom
            left:parent.left
            bottom:parent.bottom
            right:parent.right
        }
        delegate: Item{
            width: setup_hdr.width
            height: style.listViewItemHeight
            Rectangle{
                anchors.fill: parent
                color:trackedProperty===index ? "green" :"lightgrey"
            }
            Text {
                id: roomText
                text: dataTitle
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 12
                color: trackedProperty===index ? "white" : "black"
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    selectionMade(index, dataTitle)
                    trackedProperty=index
                }
            }
        }
    }

    Loader{
        id:content_area
        visible:custom
        enabled: visible
        anchors{
            top:setup_hdr.bottom
            left:parent.left
            bottom:parent.bottom
            right:parent.right
            bottomMargin: 5
        }
    }


}

