var touch_enabled = false;

function cursorWait() {
	var waiting = document.getElementById("waiting");
	waiting.style.position = 'absolute';
	waiting.style.left = '100';
	waiting.style.top = '100';
	waiting.style.display = 'block';
}

function cursorDone() {
	var waiting = document.getElementById("waiting");
	waiting.style.display = "none";
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
		cursorWait();
		var xRelative = event.pageX - $("#screen").position().left;
		var yRelative = event.pageY - $("#screen").position().top;

		DoCmd("TOUCH " + xRelative + "x" + yRelative, DeviceNumber);
	}
}

function DoCmd(Cmd, DeviceNumber)
{
	var Img = new Image();
	Img.src = "weborbiter_image.php?device_id=" + encodeURIComponent(DeviceNumber) + "&action=cmd&cmd=" + encodeURIComponent(Cmd)
		+ "&rand=" + Math.floor(Math.random() * 90000 + 10000);

	$("#screen").attr("src", Img.src);

	delete Img;
}

function PageLoaded(DeviceNumber)
{
	touch_enabled = true;
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
				DoCmd("IMAGE", DeviceNumber);
			}
		}
	);
	$(window).oneTime(1000, "RefreshImage", function() {RefreshImage(DeviceNumber)});
	cursorDone();
}
