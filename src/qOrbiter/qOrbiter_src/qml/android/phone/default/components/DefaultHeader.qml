import QtQuick 1.1
//phone - default
Item{
    id:hdr
    anchors{
        left:parent.left
        top:parent.top
        right:parent.right
    }
    height: manager.b_orientation ? scaleY(8) : scaleY(12)

    Rectangle{
        anchors.fill: parent
        color: "black"
        opacity: .75
    }


    Row{
        id:hdrLayout
        anchors{
            left:parent.left
            right:optionCol.left
            top:parent.top
            bottom:parent.bottom
        }
        Clock{

        }
    }

    Column{
        id:optionCol
        width: manager.b_orientation ? scaleX(5) : scaleX(8)
        spacing:2
        anchors{
            right: parent.right
            top:parent.top
            bottom:parent.bottom
        }

        Rectangle{
            color: "green"
            height: hdr.height /4
            width: height
        }
        Rectangle{
            color: "green"
            height: hdr.height /4
            width: height
        }
        Rectangle{
            color: "green"
            height: hdr.height /4
            width: height
        }
    }
}
