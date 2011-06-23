function tellemCommand(command,remotetype,pkfile)
{
	var self = this;
	var tellemRequest = new self.XMLHttpRequest();
	var postParameter = "command="+command+"&remotetype="+remotetype+"&pk_file="+pkfile;
	tellemRequest.open("POST", "service.php", true);
	tellemRequest.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
	tellemRequest.setRequestHeader("Content-length", postParameter.length);
	tellemRequest.setRequestHeader("Connection", "close");
	tellemRequest.onReadyStateChange = function() {
	if (tellemRequest.readyState != 4) { return; }
        var toldme = tellemRequest.responseText;
   };
   tellemRequest.send(postParameter);
}

function tellemXineStatus(room)
{
   var self = this;
   var tellemRequest = new self.XMLHttpRequest();
   tellemRequest.open("GET", "xinestatus.php?currentRoom=" + room, true);
   tellemRequest.onreadystatechange = function() {
     if (tellemRequest.readyState != 4) { return; }
     var toldme = tellemRequest.responseText;
     document.getElementById('status').innerHTML = toldme;
     /* alert(toldme);	*/
   };
   tellemRequest.send(room);
}