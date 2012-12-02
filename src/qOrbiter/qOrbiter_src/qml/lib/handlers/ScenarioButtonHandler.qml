import QtQuick 1.0

MouseArea{
    id:delegateHit
    anchors.fill: parent
    onClicked: {
        manager.execGrp(params);
        generic_model_display.state = "inactive"
    }

}
