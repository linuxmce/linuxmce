import QtQuick 2.2

Component
{
    id:roomselectioncomponent
    Item {
        id: selectoritem

        //delegate
        Rectangle{
            id:roomdelegate
            height:120
            width: 75
            // color: "transparent"

            Rectangle{
                id:delegateheader
                height: 25
                width: roomdelegate.width
                Text {
                    id: celllabel
                    text: title + ":" + intRoom + ", In EA: " + entertain_area
                }
            }
        }
    }
}
