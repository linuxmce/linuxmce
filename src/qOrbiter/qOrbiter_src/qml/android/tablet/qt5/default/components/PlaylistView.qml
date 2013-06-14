import QtQuick 2.0

Item{
id:standard_playlist
height:parent.height
width:scaleX(15)
property alias model:playlist_data.model
property alias delegate:playlist_data.delegate

ListView{
id:playlist_data
height:parent.height
width:parent.width
model:mediaplaylist
}


}
