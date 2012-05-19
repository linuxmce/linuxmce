// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1


Rectangle {
    id:scenarios
    width: appW
    height: scaleX(8)
    color: "grey"
    border.color: "white"
    border.width: 1

    Row{
        id:modelRow
        height: scenarios.height
        width: appW
    }

}
