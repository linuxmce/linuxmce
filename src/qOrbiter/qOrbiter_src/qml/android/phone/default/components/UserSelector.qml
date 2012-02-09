import QtQuick 1.0

Rectangle {
    id:usermenu
    height: 100

    HomeButtonDelegate{id:userdelegate}
    Image {
        id: bg
        source: "../img/bkg.png"
        anchors.fill: lightingScenarios
    }
    ListView{
        id: lightingScenarios
        width: 200
        height: userList.childrenRect.height +25
        model: userList
        spacing:25
        orientation:ListView.Vertical
        delegate:  userdelegate
        interactive: true
        clip:true

    }
}
