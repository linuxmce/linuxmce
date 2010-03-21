function tellemCommand(command,pkfile)
{
   var self = this;
   var tellemRequest = new self.XMLHttpRequest();
   tellemRequest.open("POST", "service.php", true);
   tellemRequest.onReadyStateChange = function() {
     if (tellemRequest.readyState != 4) { return; }
     var toldme = tellemRequest.responseText;
   };
   tellemRequest.send("command="+command+"&pkfile"+pkfile);
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