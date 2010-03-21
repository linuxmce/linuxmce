function tellemCommand(command,pkfile)
{
   var self = this;
   var tellemRequest = new self.XMLHttpRequest();
   tellemRequest.open("POST", "service.php", true);
   tellemRequest.onReadyStateChange = function() {
     if (tellemRequest.readyState != 4) { return; }
     var toldme = tellemRequest.responseText;
     alert("hey");
   };
   tellemRequest.send("command=123");
}

function tellemXineStatus(room)
{
   var self = this;
   var tellemRequest = new self.XMLHttpRequest();
   tellemRequest.open("GET", "xinestatus.php", true);
   tellemRequest.onreadystatechange = function() {
     if (tellemRequest.readyState != 4) { return; }
     var toldme = tellemRequest.responseText;
     document.getElementById('status').innerHTML = toldme;
     /* alert(toldme);	*/
   };
   tellemRequest.send(room);
}