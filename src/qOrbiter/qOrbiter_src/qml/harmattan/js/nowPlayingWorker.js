WorkerScript.onMessage = function(msg)
{
        msg.item.clear();
        msg.item.append({"title":"fooby"})
        if (msg.dcenowplaying.qs_subtitle != "")
        {
            msg.item.setProperty(0,"title","Now Playing");
            msg.item.setProperty(0,"subtitle",msg.dcenowplaying.qs_mainTitle + " \""+msg.dcenowplaying.qs_subTitle+"\" / " + msg.dcenowplaying.timecode);
            msg.item.setProperty(0,"page",msg.dcenowplaying.qs_screen);
        }
        else
        {
            msg.item.setProperty(0,"title","Now Playing");
            msg.item.setProperty(0,"subtitle",msg.dcenowplaying.qs_mainTitle + " / " + msg.dcenowplaying.timecode);
            msg.item.setProperty(0,"page",msg.dcenowplaying.qs_screen);
        }
        msg.item.sync(); // Make sure that the list item is updated.
}
