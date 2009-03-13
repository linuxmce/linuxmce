var objSensorPositioner = new SensorPositioner("objSensorPositioner");

function SensorPositioner(strVariableName)
{

	this.variableName = strVariableName;
	
	//all numbers are post scaling
	
	//layout
	this.positionerWidthMin = 450;
	this.positionerHeightMin = 450;
	this.positionerWidth = -1;
	this.positionerHeight = -1;
	this.leftMargin = -1;
	this.topMargin = -1;
	
	this.highlightColorText = "yellow";
	this.highlightColorSensor = "#FF0000";
	this.backgroundColor = "#EEEEEE";
	
	//Sensor object
	this.Sensor = new Object();
	
	//input
	this.imageURL = "";
	this.imageWidth = -1;
	this.imageHeight = -1;
	
	this.focusObject = null;
	
	
	this.SensorInstances = new Array();

	this.selectedSensorIndex = -1;
	
	//methods
	this.writeHTML = SensorPositionerWriteHTML;
	this.positionerDoneMoving = SensorPositionerPositionerDoneMoving;
	this.moveSensor = SensorPositionerMoveSensor;
	this.clickSensor = SensorPositionerClickSensor;
	this.getSerial = SensorPositionerGetSerial;
}


function SensorPositionerGetSerial()
{
	var strReturn = "";	
	for(intCount=0; intCount<this.SensorInstances.length; intCount++)
	{
		strReturn += "," + this.SensorInstances[intCount].ID + "," + this.SensorInstances[intCount].SensorX + "," + this.SensorInstances[intCount].SensorY ;
	}
	return strReturn;
}


function SensorInstance()
{

	//input
	this.SensorURL = "";
	this.SensorWidth = -1;
	this.SensorHeight = -1;
	
	this.ID = -1;
	this.Description = "";
	this.Type = -1;

	//this is the stuff we want out
	this.SensorX = 0;
	this.SensorY = 0;
	this.modified = false;
	
	//used internally
	this.SensorXOriginal = 0;
	this.SensorYOriginal = 0;
	
	//this.text = "";

}

function SensorPositionerWriteHTML()
{
	//calculate the margins
	var intWidthTemp = 0;
	var intHeightTemp = 0;
	
	this.positionerWidth = (intWidthTemp > this.positionerWidthMin) ? intWidthTemp : this.positionerWidthMin;
	this.positionerHeight = (intHeightTemp > this.positionerHeightMin) ? intHeightTemp : this.positionerHeightMin;
	
	this.leftMargin = parseInt((this.positionerWidth - this.imageWidth)/2);
	this.topMargin = parseInt((this.positionerHeight - this.imageHeight)/2);
	
	this.leftMargin = 250;
	this.topMargin = 50;

	var objMainLayer = new DynLayer();
	
	var mainHeight = 500; // min:500(for 15), + 30 for each add this.SensorInstances.length
	if (this.SensorInstances.length > 15) mainHeight += 30*(this.SensorInstances.length-15);
	
	objMainLayer.setSize(650,mainHeight); 
	objMainLayer.setBgColor(this.backgroundColor);
	
	
	//add the select list
	for(intCount=0; intCount<this.SensorInstances.length; intCount++)
	{
		objListItem = new DynLayer();
		objListItem.moveTo(40, 30*intCount + 50);
		objListItem.setSize(200,35);
		objListItem.setBgColor((intCount == this.selectedSensorIndex && this.SensorInstances.length > 1) ? this.highlightColorText : "");
		objListItem.setHTML("<table><tr><td>&nbsp;<a href=\"JavaScript:void(null);\" class='normalLink'>" + this.SensorInstances[intCount].Description + "</a></td></tr></table>");
		this.SensorInstances[intCount].Label = objListItem;
		
		objEventListener = new EventListener();
		eval("objEventListener.onmousedown = function(e){objSensorPositioner.clickSensor(" + intCount + ");};")
		objListItem.addEventListener(objEventListener);
		
		objMainLayer.addChild(objListItem);
	}
	
	
	
	
	
	
	
	//add the image and draggable icons
	
	//add a border around the image
	objImageLayer = new DynLayer();
	objImageLayer.moveTo(this.leftMargin-1,this.topMargin-1);
	objImageLayer.setSize(this.imageWidth+2,this.imageHeight+2);
	objImageLayer.setBgColor('#000000');
	objMainLayer.addChild(objImageLayer);
		
	
	objImageLayer = new DynLayer();
	objImageLayer.moveTo(this.leftMargin,this.topMargin);
	objImageLayer.setSize(this.imageWidth,this.imageHeight);
	objImageLayer.setBgColor('#CCCCCC');
	objImageLayer.setHTML("<img src='" + this.imageURL + "' width='" + this.imageWidth + "' height='" + this.imageHeight + "'>");
	objMainLayer.addChild(objImageLayer);
	
	for(var intCount=0;intCount<this.SensorInstances.length;intCount++)
	{
	
		objSensorLayer = new DynLayer();
	
		if(this.SensorInstances[intCount].hasCoordinates)
		{
			objSensorLayer.moveTo(this.leftMargin + this.SensorInstances[intCount].SensorX - 2,this.topMargin + this.SensorInstances[intCount].SensorY - 2);
		}
		else
		{
			objSensorLayer.moveTo(10, 30*intCount + 50);
		}
		
		objSensorLayer.setSize(this.SensorInstances[intCount].SensorWidth + 4,this.SensorInstances[intCount].SensorHeight + 4);
		if(intCount == this.selectedSensorIndex && this.SensorInstances.length > 1)
		{
			objSensorLayer.setHTML("<img src='" + this.SensorInstances[intCount].SensorURL + "' width='" + this.SensorInstances[intCount].SensorWidth + "' height='" + this.SensorInstances[intCount].SensorHeight + "' border='2' style='border-color:" + this.highlightColorSensor + "'>");
		}
		else
		{
			objSensorLayer.setHTML("<table cellpadding='2' cellspacing='0' border='0'><tr><td><img src='" + this.SensorInstances[intCount].SensorURL + "' width='" + this.SensorInstances[intCount].SensorWidth + "' height='" + this.SensorInstances[intCount].SensorHeight + "' border='0'></td></tr></table>");
		}
		
		
		objMainLayer.addChild(objSensorLayer);
		
		this.SensorInstances[intCount].Sensor = objSensorLayer;
		
		DragEvent.enableDragEvents(objSensorLayer);
		DragEvent.setDragBoundary(objSensorLayer);
			
		objEventListener = new EventListener();
		objEventListener.ondragend = function(e)
		{
			objSensorPositioner.positionerDoneMoving();
		}
		eval("objEventListener.onmousedown = function(e){objSensorPositioner.clickSensor(" + intCount + ");};")
		objSensorLayer.addEventListener(objEventListener);
		
		
	}
	
	DynAPI.document.addInflowChild(objMainLayer);

}

function SensorPositionerPositionerDoneMoving()
{
	objSensor = this.SensorInstances[this.selectedSensorIndex];

	var intX = objSensor.Sensor.x - this.leftMargin + 1;
	var intY = objSensor.Sensor.y - this.topMargin + 1;

	if((intX < 0 || intY < 0 || intX > this.imageWidth-20 || intY > this.imageHeight-20))
	{
		objSensor.Sensor.moveTo(10, 30*this.selectedSensorIndex + 50);
		this.SensorInstances[this.selectedSensorIndex].SensorX = -1;
		this.SensorInstances[this.selectedSensorIndex].SensorY = -1;
	}
	else
	{
		this.SensorInstances[this.selectedSensorIndex].SensorX = intX;
		this.SensorInstances[this.selectedSensorIndex].SensorY = intY;
	}
	
	if (this.focusObject != null) this.focusObject.focus();
}

function SensorPositionerMoveSensor(intIndex,strX, strY)
{
	this.SensorInstances[intIndex].SensorX = parseInt(strX,10);
	this.SensorInstances[intIndex].SensorY = parseInt(strY,10);
	this.SensorInstances[intIndex].SensorXOriginal = this.SensorX;
	this.SensorInstances[intIndex].SensorYOriginal = this.SensorY;
	this.SensorInstances[intIndex].Sensor.moveTo(this.leftMargin + this.SensorInstances[intIndex].SensorX,this.topMargin + this.SensorInstances[intIndex].SensorY);
}

function SensorPositionerClickSensor(intIndex)
{
	if(this.selectedSensorIndex != -1)
	{
		//SensorTextChanged();
	}
	if(intIndex != this.selectedSensorIndex)
	{
		if(this.selectedSensorIndex != -1)
		{
			this.SensorInstances[this.selectedSensorIndex].Sensor.setHTML("<table cellpadding='2' cellspacing='0' border='0'><tr><td><img src='" + this.SensorInstances[this.selectedSensorIndex].SensorURL + "' width='" + this.SensorInstances[this.selectedSensorIndex].SensorWidth + "' height='" + this.SensorInstances[this.selectedSensorIndex].SensorHeight + "' border='0'></td></tr></table>");
			this.SensorInstances[this.selectedSensorIndex].Label.setBgColor("");
		}
		this.selectedSensorIndex = intIndex;
		this.SensorInstances[intIndex].Sensor.setHTML("<img src='" + this.SensorInstances[intIndex].SensorURL + "' width='" + this.SensorInstances[intIndex].SensorWidth + "' height='" + this.SensorInstances[intIndex].SensorHeight + "' border='2' style='border-color:" + this.highlightColorSensor + "'>");
		this.SensorInstances[intIndex].Label.setBgColor(this.highlightColorText);
	}
}