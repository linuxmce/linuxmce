WorkerScript.onMessage = function(msg)
{
    msg.item.clear();

    if (msg.isPlaying == true)
    {

        msg.item.append({"title":"NowPlaying","subtitle":"Movie","page":"Screen_123.qml"});

        if (msg.subTitle != "")
        {
            msg.item.setProperty(0,"title","Now Playing");
            msg.item.setProperty(0,"subtitle",msg.mainTitle + " \""+msg.subTitle+"\" / " + msg.timecode);
            msg.item.setProperty(0,"page",msg.qs_screen);
        }
        else
        {
            msg.item.setProperty(0,"title","Now Playing");
            msg.item.setProperty(0,"subtitle",msg.mainTitle + " / " + msg.timecode);
            msg.item.setProperty(0,"page",msg.qs_screen);
        }
    }

    msg.item.append({"title":"Lights","page":"Screen_2.qml"});
    msg.item.append({"title":"Media","page":"Screen_3.qml"});
    msg.item.append({"title":"Climate","page":"Screen_4.qml"});
    msg.item.append({"title":"Security","page":"Screen_5.qml"});
    msg.item.append({"title":"Telecom","page":"Screen_6.qml"});

    msg.item.sync(); // Make sure that the list item is updated.

}
