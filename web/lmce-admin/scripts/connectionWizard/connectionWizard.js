var last_z_index=1;
var tmpx=0;
var tmpy=0;
var flag_drag=0;
var object_to_drag=null;

if (document.layers) { // Netscape
	document.captureEvents(Event.MOUSEMOVE);
	document.onmousemove = captureMousePosition;
} else if (document.all) { // Internet Explorer
	document.onmousemove = captureMousePosition;
} else if (document.getElementById) { // Netcsape 6
	document.onmousemove = captureMousePosition;
}

xMousePos = 0; // Horizontal position of the mouse on the screen
yMousePos = 0; // Vertical position of the mouse on the screen
pipesArray=new Array('audio','video');

var startX=0;
var startY=0;

globalXFrom=-1;
globalYFrom=-1;
globalFromDevice=-1;
globalToDevice=-1;
globalInput=-1;
globalOutput=-1;
globalPipe='audio';
globalXTo=-1;
globalYTo=-1

audioPipe=new Array();
videoPipe=new Array();

function captureMousePosition(e) {
	if (document.layers) {
		xMousePos = e.pageX;
		yMousePos = e.pageY;
	} else if (document.all) {
		xMousePos = window.event.x+document.body.scrollLeft;
		yMousePos = window.event.y+document.body.scrollTop;
	} else if (document.getElementById) {
		xMousePos = e.pageX;
		yMousePos = e.pageY;
	}
}


function show_layer(val)
{
	bring_to_front(val);
	last_z_index++;
	eval("document.getElementById('"+val+"').style.zIndex=last_z_index;");
	eval("document.getElementById('"+val+"').style.display='';");
}

function hide_layer(val)
{
	eval("document.getElementById('"+val+"').style.display='none';");
}


function assign_res(val)
{
	show_layer('layer_assign');
}

function start_drag(val)
{
  tmpx = xMousePos - document.getElementById(val).offsetLeft;
  tmpy = yMousePos - document.getElementById(val).offsetTop;
	startX=xMousePos;
	startY=yMousePos;
	flag_drag=1;
	object_to_drag=val;
	devID=val.substring(7);

	for(i=0;i<pipesArray.length;i++){
		pipe=pipesArray[i];
		eval(pipe+"_"+devID+".clear()");
		eval("tmpArray="+pipe+"Pipe");
		for(dev in tmpArray){
			if(tmpArray[dev] && tmpArray[dev]['to']==devID){
				eval(pipe+"_"+dev+".clear()");
			}
		}
	}
}

function drag_layer()
{
  window.status = 'flag_drag=' + flag_drag + ', xMousePos=' + xMousePos + ', yMousePos=' + yMousePos + ', tmpx=' + tmpx + ', tmpy=' + tmpy;

	if(flag_drag==1)
	{
	  document.getElementById(object_to_drag).style.left = (xMousePos-tmpx) + "px";
	  document.getElementById(object_to_drag).style.top = (yMousePos-tmpy) + "px";
	}
}

function end_drag(val)
{
	flag_drag=0;
	object_to_drag=null;
	redrawPipes(val);
}

// draw pipes for device and coords from parameters
function drawPipe(device,connectionType,fromX,fromY,toX,toY)
{
	pipeName=connectionType+'_'+device;
	switch (connectionType){
		case 'audio':
			pipeColor='red';
		break;
		case 'video':
			pipeColor='blue';
		break;
	}
	eval(pipeName+".setColor(\""+pipeColor+"\")"); 
  	eval(pipeName+".drawLine("+fromX+", "+fromY+", "+toX+","+toY+")"); 
	eval(pipeName+".paint()");
}

function clearPipe(pipeName)
{
	eval(pipeName+".clear()");
}

// rebuild pipes who doesn't exist in the cookie
// the calls are generated from PHP for missing pipes
function generatePipe(deviceFrom,commandOut,deviceTo,commandIn,connectionType,description)
{
  eval(connectionType+"Pipe["+deviceFrom+"]=new Array();");
  eval(connectionType+"Pipe["+deviceFrom+"]['to']="+deviceTo+";");
  eval(connectionType+"Pipe["+deviceFrom+"]['input']="+commandIn+";");
  eval(connectionType+"Pipe["+deviceFrom+"]['output']="+commandOut+";");
  eval(connectionType+"Pipe["+deviceFrom+"]['description']='"+description+"'");
  
  var coords = calculatePipeCoords(deviceFrom, commandOut, deviceTo, commandIn, connectionType);
  drawPipe(deviceFrom,connectionType,coords.coordXFrom,coords.coordYFrom,coords.coordXTo,coords.coordYTo);
}

// deviceFrom is the key to the pipe array
// commandOut is .output
// deviceTo is .to
// commandIn is .input
// connectionType is 'audio' or 'video'
function calculatePipeCoords(deviceFrom, commandOut, deviceTo, commandIn, connectionType) {
  var from = document.getElementById('device_' + deviceFrom);
  var to = document.getElementById('device_' + deviceTo);
  xContainer = from.offsetLeft;
  yContainer = from.offsetTop;
  xContainerTo = to.offsetLeft;
  yContainerTo = to.offsetTop;
	
  var fromCmd = document.getElementById('out_' + deviceFrom + '_cmd_' + commandOut);
  var toCmd = document.getElementById('in_' + deviceTo + '_cmd_' + commandIn);
  fromX = fromCmd.offsetLeft;
  fromY = fromCmd.offsetTop;
  toX = toCmd.offsetLeft;
  toY = toCmd.offsetTop;

  yHook=getYHook(connectionType);
  var retval = {
    coordXFrom: xContainer+fromX+110,
    coordYFrom: yContainer+fromY+yHook,
    coordXTo: xContainerTo+toX,
    coordYTo: yContainerTo+toY+yHook
  }

  return retval;
}

function disabletext(e){
	return false
}

function reEnable(){
	return true
}

function bring_to_front(val)
{
	last_z_index++;
	eval("document.getElementById('"+val+"').style.zIndex=last_z_index;");
	for(i=0;i<layersArray.length;i++){
		eval("document.getElementById('head_"+layersArray[i]+"').style.background='red';");
	}


	//if the browser is IE4+
	document.onselectstart=new Function ("return false")
	
	//if the browser is NS6
	if (window.sidebar){
		document.onmousedown=disabletext
		document.onclick=reEnable
	}	
}

function setPipeVariables()
{
	eval(globalPipe+"Pipe["+globalFromDevice+"]=new Array();");
	eval(globalPipe+"Pipe["+globalFromDevice+"]['to']="+globalToDevice+";");
	eval(globalPipe+"Pipe["+globalFromDevice+"]['input']="+globalInput+";");
	eval(globalPipe+"Pipe["+globalFromDevice+"]['output']="+globalOutput+";");
	//	eval(globalPipe+"Pipe["+globalFromDevice+"]['coords']='"+globalXFrom+","+globalYFrom+","+globalXTo+","+globalYTo+"';");
	eval(globalPipe+"Pipe["+globalFromDevice+"]['description']='From "+document.getElementById('fromMessage').innerHTML+" to "+document.getElementById('toMessage').innerHTML+"';");
	switch(globalPipe){
		case 'audio':
			setMessage('Audio Pipe set',0);
		break;
		case 'video':
			setMessage('Video Pipe set',0);
		break;
	}
	clearNotifications();
}

// grab input and outputs for devices and
// create a graphical pipe and display it if it's a valid one
function setPipe(device,command,prefix,itemName)
{
  var dev = document.getElementById('device_' + device);
  var item = document.getElementById(prefix + device + '_cmd_' + command);
  xContainer = dev.offsetLeft;
  yContainer = dev.offsetTop;
  xOffsetItem = item.offsetLeft;
  yOffsetItem = item.offsetTop;

	yHook=getYHook(globalPipe);
	if(prefix=='in_'){
		globalXTo=xContainer+xOffsetItem;
		globalYTo=yContainer+yOffsetItem+yHook;
		if(globalToDevice!=-1){
			setMessage('Invalid pipe from input to input',1);
		}
		globalToDevice=device;		
		globalInput=command;
		document.getElementById('toMessage').innerHTML=itemName;
		
	}else{
		globalXFrom=xContainer+xOffsetItem+110;
		globalYFrom=yContainer+yOffsetItem+yHook;
		if(globalFromDevice!=-1){
			setMessage('Invalid pipe from output to output',1);
		}		
		globalFromDevice=device;
		globalOutput=command;
		document.getElementById('fromMessage').innerHTML=itemName;
	}

	if(globalYTo!=-1 && globalXFrom!=-1){
		if(globalFromDevice==globalToDevice){
			setMessage('You selected input and output for the same device',1);
		}else{
			existAlready=eval("("+globalPipe+"Pipe["+globalFromDevice+"]!=null)?1:0");
			if(existAlready){
				if(eval("confirm('The "+globalPipe+" pipe for device "+globalToDevice+" exist. Do you want to delete it to create a new one?')")){
					eval("clearPipe('"+globalPipe+"_'+"+globalFromDevice+")");
					setPipeVariables();
					drawPipe(globalFromDevice,globalPipe,globalXFrom,globalYFrom,globalXTo,globalYTo);
				}
			}else{
				setPipeVariables();
				drawPipe(globalFromDevice,globalPipe,globalXFrom,globalYFrom,globalXTo,globalYTo);
			}
			
			globalXFrom=-1;
			globalYFrom=-1;
			globalXTo=-1;
			globalYTo=-1
			globalFromDevice=-1;
			globalToDevice=-1;
		}
	}
}

// remove the notification from top right box
function clearNotifications()
{
	document.getElementById('toMessage').innerHTML='';
	document.getElementById('fromMessage').innerHTML='';
}

// call redraw connectors for every pipe type
function redrawPipes(val)
{
	deviceID=val.substring(7);
	redrawConnectors('audio',audioPipe,deviceID);
	redrawConnectors('video',videoPipe,deviceID);
}

// redraw all graphical pipes based on the arrays with data for a specific pipe type and device
function redrawConnectors(connectorType,pipeArray,deviceID)
{
		for(dev in pipeArray){
		  var info = pipeArray[dev];
		  if(info && (dev==deviceID || info.to == deviceID)) {
		    // there are pipes that start or end from this device
		    var coords = calculatePipeCoords(dev, info.output, info.to, info.input, connectorType);
		    drawPipe(dev, connectorType, coords.coordXFrom, coords.coordYFrom, coords.coordXTo,coords.coordYTo);
		  }
		}
}

// category 0 is normal message, 1 mean error and is displayed in red
function setMessage(message,category)
{
	if(category==1){
		document.getElementById('generalMessage').innerHTML='<span class="err">'+message+'</span>';
	}else{
		document.getElementById('generalMessage').innerHTML=message;
	}
}

// set which pipe will be drawn
function setPipeType()
{
	for(i=0;i<document.forms[0].pipeType.length;i++){
		if(document.forms[0].pipeType[i].checked){
			globalPipe=document.forms[0].pipeType[i].value;
			break;
		}
	}
}

// save all positions and pipes
function savePositions()
{
	toSave='';
	var len = layersArray.length;
	for (var key=0; key<len; ++key){
	  var dev = layersArray[key];
	  var thisLayer = document.getElementById('device_' + dev);
	  xCoord = thisLayer.offsetLeft + "px";
	  yCoord = thisLayer.offsetTop + "px";
	  audioCoords = getPipeCookieValue(audioPipe[dev], dev, 'audio');
	  videoCoords = getPipeCookieValue(videoPipe[dev], dev, 'video');
	  toSave += ';' + [dev, xCoord, yCoord, audioCoords, videoCoords].join(';');
	}
	document.forms[0].devicesCoords.value=toSave;
	document.connectionWizard.action.value='setCookie';
	document.connectionWizard.submit();
}

function getPipeCookieValue(pipe, deviceFrom, connectionType)
{
  var retval = "none";
  if (pipe) {
    var coords = calculatePipeCoords(deviceFrom, pipe.output, pipe.to, pipe.input, connectionType);
    var coordsString = [coords.coordXFrom, coords.coordYFrom, coords.coordXTo, coords.coordYTo].join(',');
    retval = [pipe.to, coordsString, pipe.input, pipe.output, pipe.description].join(':');
  }

  return retval;
}

// display info toolbar with checkboxes to be able to delete existing pipes
function showInfoToolbar(device)
{
  var toolbar = document.getElementById('deleteToolbar');
  toolbar.style.left = (xMousePos-10) + "px";
  toolbar.style.top = (yMousePos-10) + "px";
	for(i=0;i<pipesArray.length;i++){
		hasPipe=eval("("+pipesArray[i]+"Pipe["+device+"]!=null)?1:0");	
		if(hasPipe==1){
			eval("toDevice="+pipesArray[i]+"Pipe["+device+"]['to']");
			eval("deviceInput="+pipesArray[i]+"Pipe["+device+"]['input']");
			eval("deviceOutput="+pipesArray[i]+"Pipe["+device+"]['output']");
			//eval("document.getElementById('del_"+pipesArray[i]+"_text').innerHTML=\'from "+deviceOutput+" to "+deviceInput+" on "+toDevice+"\'");
			eval("document.getElementById('del_"+pipesArray[i]+"_text').innerHTML="+pipesArray[i]+"Pipe["+device+"]['description'];");
			eval("document.getElementById('del_"+pipesArray[i]+"').checked=true;");
			eval("document.getElementById('del_"+pipesArray[i]+"').value="+device);
		}else{
			eval("document.getElementById('del_"+pipesArray[i]+"_text').innerHTML=\'not used\'");
			eval("document.getElementById('del_"+pipesArray[i]+"').checked=false;");
			eval("document.getElementById('del_"+pipesArray[i]+"').value=1");
		}
	}
	toolbar.style.display='';	
}

// delete the graphical pipe (not the arrays used for rebuild)
function removePipe(val)
{
	if(val.value!=1){
		pipeToDel=val.id.substr(4);
		clearPipe(pipeToDel+'_'+val.value);
		eval(pipeToDel+"Pipe["+val.value+"]=null");
	}
}

// functions used to check if an element has other elements on it
function mouseLeaves (element, evt) {
	if (typeof evt.toElement != 'undefined' && typeof element.contains != 'undefined') {
		return !element.contains(evt.toElement);
	}
	else if (typeof evt.relatedTarget != 'undefined' && evt.relatedTarget) {
		return !contains(element, evt.relatedTarget);
	}
}

function contains (container, containee) {
  while (containee) {
    if (container == containee) {
      return true;
    }
    containee = containee.parentNode;
  }
  return false;
}

function getYHook(pipeType)
{
	switch(pipeType){
		case 'audio':
			yHook=8;
		break;
		case 'video':
			yHook=12;
		break;
	}
	return yHook;

}