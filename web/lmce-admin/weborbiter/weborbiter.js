var touch_enabled = false;
var waiting_offset = {top: 100, left: 100};
var use_wait_cursor = true;
var DeviceNumer = undefined;

function cursorWait() {
	if (!use_wait_cursor)
		return;
	$("#waiting").show();
	$("#waiting").offset(waiting_offset);
}

function cursorDone() {
	if (!use_wait_cursor)
		return;
	$("#waiting").hide();
}

function findPosX(obj)
{
	var curleft = 0;
	if (obj.offsetParent)
	{
		while (obj.offsetParent)
		{
			curleft += obj.offsetLeft
			obj = obj.offsetParent;
		}
	}
	else if (obj.x)
		curleft += obj.x;
	return curleft;
}

function findPosY(obj)
{
	var curtop = 0;
	if (obj.offsetParent)
	{
		while (obj.offsetParent)
		{
			curtop += obj.offsetTop
			obj = obj.offsetParent;
		}
	}
	else if (obj.y)
		curtop += obj.y;
	return curtop;
}

function sendTouch(event)
{
	if (touch_enabled)
	{
		var xRelative = event.pageX - $("#screen").position().left;
		var yRelative = event.pageY - $("#screen").position().top;

		DoCmd("TOUCH " + xRelative + "x" + yRelative);
	}
}

function DoCmd(Cmd)
{
	StopRefreshTimer();
	cursorWait();

	var url = 'weborbiter_command.php';
	var data = 'device_id=' + encodeURIComponent(DeviceNumber) + "&cmd=" + encodeURIComponent(Cmd);

	$.get(url, data, LoadImage);
}

function StartRefreshTimer()
{
	StopRefreshTimer();
	$(window).oneTime(1000, "RefreshImage", RefreshImage);
}

function StopRefreshTimer()
{
	$(window).stopTime("RefreshImage");
}

function LoadImage()
{
	StopRefreshTimer();
	cursorWait();

	var url = "weborbiter_image.php?device_id=" + encodeURIComponent(DeviceNumber) + "&rand=" + Math.floor(Math.random() * 90000 + 10000);
	$("#screen").attr("src", url);
}

function PageLoaded(theDeviceNumber)
{
	var offset_top = ($("#screen").height() - $("#waiting").height()) / 2;
	var offset_left = ($("#screen").width() - $("#waiting").width()) / 2;
	waiting_offset = { top: offset_top, left: offset_left };
	touch_enabled = true;
	DeviceNumber = theDeviceNumber;
	StartRefreshTimer();

	$("#screen").load(OnPageLoaded);
	$("#screen").error(OnPageLoadError);

	$(document).keydown(sendKey);
}

function OnPageLoaded()
{
	cursorDone();
	StartRefreshTimer();
}

function OnPageLoadError()
{
	StartRefreshTimer();
}

function RefreshImage()
{
	var url = 'weborbiter.php';
	var data = 'device_id=' + encodeURIComponent(DeviceNumber) + "&action=anynews";
	$.ajax({
		url: url,
		data: data,
		cache: false,
		success: OnRefreshSuccess,
		error: OnRefreshError
	});
}

function OnRefreshSuccess(data)
{
	if (data == "yes")
		LoadImage();
	else
		StartRefreshTimer();
}

function OnRefreshError()
{
	StartRefreshTimer();
}
