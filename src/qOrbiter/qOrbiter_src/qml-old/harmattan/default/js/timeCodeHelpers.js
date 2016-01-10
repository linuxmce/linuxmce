function timecodeToSeconds(timecode)
{
    var timeElements = timecode.split(":");
    if (timeElements.length==3)
    {
        // HH:MM:SS
        var sec = parseInt(timeElements[2]);
        var min = parseInt(timeElements[1]*60);
        var hr = parseInt(timeElements[0]*3600);
        return hr+min+sec;
    }
    else if (timeElements.length==2)
    {
        // MM:SS
        var sec = parseInt(timeElements[1]);
        var min = parseInt(timeElements[0]*60);
        return min+sec;
    }
    else
    {
        var sec = parseInt(timeElements[0]);
        return sec;
    }
}

function secondsToTimecode(seconds)
{
    var d = Number(seconds);
    var h = Math.floor(d / 3600);
    var m = Math.floor(d % 3600 / 60);
    var s = Math.floor(d % 3600 % 60);

    return ((h > 0 ? h + ":" : "") + (m > 0 ? (h > 0 && m < 10 ? "0" : "") + m + ":" : "0:") + (s < 10 ? "0" : "") + s);

}
