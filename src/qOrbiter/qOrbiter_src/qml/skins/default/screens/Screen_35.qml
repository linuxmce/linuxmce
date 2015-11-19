import QtQuick 2.2
import "../components"
StyledScreen{
    id:securitystatus
    screen:qsTr("Mode Changed")
    screen_params: [
        165 /* PK_HouseMode */,
        166 /* HouseModeTime */,
        167 /* ExitDelay */,
        168 /* Alerts */,
    ]
    property int exitDelay : -1

    Panel{
        headerTitle: "Mode Changed"
        anchors.centerIn: parent

        StyledText {
            id: modeText
            // center
            anchors {
                topMargin: 10
                top: parent.headerRect.bottom
                left: parent.left
                right: parent.right
            }
            textLabel: qsTr("The house mode has been changed to:") + " " + screenparams.getParam(165)
        }

        StyledText {
            id: alertsLabel
            // left aligned
            anchors {
                topMargin: 10
                top: modeText.bottom
                left: parent.left
                right: parent.right
            }
            textLabel: qsTr("Alerts since last change:") + " " + screenparams.getParam(168)
        }
        StyledText {
            id: effectiveText
            anchors {
                bottomMargin: 10
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }
            textLabel: qsTr("Change will be effective in:") + " " + (exitDelay >= 0 ? exitDelay : "...")
        }
    }

    Timer {
        id: countDownTimer
        interval: 1000
        running: true
        repeat: true
        onTriggered: {
            if (exitDelay <= 0) {
                exitDelay = screenparams.getParam(167)
            } else {
                exitDelay = exitDelay - 1
                if (exitDelay <= 0)
                    running = false
            }
        }

    }
}
