// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id:genericlist
    height: 200
    width: 200
    border.color: "orange"
    border.width: 1
    clip: false
    color:"transparent"

Component{
  id:delegatemenu
  
  Item{
        id:generic_item
        //important!! these need to be set on an imported component otherwise its appears all wrong!
        height:  delegatemenu.height
        width: delegatemenu.width

        Rectangle{
           
            width: 200
            height: 55
            border.color: "white"
            border.width: 1
            color:"transparent"

            Text {
                id: generic_label
                text:  username
                color: "white"
                font.pixelSize: 11
                anchors.centerIn: parent
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                   setCurrentUser(index+1)
                   currentuser
                    genericlist.destroy()
                }
            }
        }
    }
}
    Image {
        id: bg
        source: "../img/bkg.png"
        anchors.fill: genericview
    }
    ListView{
        id: genericview
        width: 200
        height: genericview.childrenRect.height +25
        
        model: userList
        spacing:56
        orientation:ListView.Vertical
        delegate:  delegatemenu
        interactive: true
        clip:true

    }
}
