var touch_enabled = false;
var waiting_offset = {top: 100, left: 100};
var use_wait_cursor = true;

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

function sendTouch(event, DeviceNumber)
{
	if (touch_enabled)
	{
		var xRelative = event.pageX - $("#screen").position().left;
		var yRelative = event.pageY - $("#screen").position().top;

		DoCmd("TOUCH " + xRelative + "x" + yRelative, DeviceNumber);
	}
}

function DoCmd(Cmd, DeviceNumber)
{
	cursorWait();

	var url = 'weborbiter_command.php';
	var data = 'device_id=' + encodeURIComponent(DeviceNumber) + "&cmd=" + encodeURIComponent(Cmd);

	$.get(url, data,
		function(data) {
			LoadImage(DeviceNumber);
		}
	);
}

function LoadImage(DeviceNumber)
{
	cursorWait();

	var Img = new Image();
	$(Img).load(
		function() {
			$("#screen").attr("src", Img.src);
			cursorDone();
			delete Img;
		}
	);
	Img.src = "weborbiter_image.php?device_id=" + encodeURIComponent(DeviceNumber)
		+ "&rand=" + Math.floor(Math.random() * 90000 + 10000);
}

function PageLoaded(DeviceNumber)
{
	touch_enabled = true;
	offset_top = ($(window).height() - $("#waiting").height()) / 2;
	offset_left = ($(window).width() - $("#waiting").width()) / 2;
	waiting_offset = { top: offset_top, left: offset_left };
	$(window).oneTime(1000, "RefreshImage", function() {RefreshImage(DeviceNumber)});
}

function RefreshImage(DeviceNumber)
{
	var url = 'weborbiter.php';
	var data = 'device_id=' + encodeURIComponent(DeviceNumber) + "&action=anynews";
	$.get(url, data,
		function(data)
		{
			if (data == "yes")
			{
				LoadImage(DeviceNumber);
			}
		}
	);
	$(window).oneTime(1000, "RefreshImage", function() {RefreshImage(DeviceNumber)});
}
