
function tabBox(strVariableName){
	this.variableName = strVariableName
	this.position = "relative"
	this.top = -1
	this.left = -1
	this.height = new Number()
	this.width = new Number()
	this.tabHeight = 20
	this.tabSpacing = 6;
	this.tabIndent = 15;
	this.padding = 16
	this.levelFromRoot = 0
	this.borderWidth = 1
	this.borderColor = "#999999"
	//this.backgroundColor = "#f1f1f1"
	this.backgroundColor = "#eeeeee"
	this.backTabColor = "#e5e5e5"
	this.tabTextStyle = "font-family:arial;font-weight:bold;font-size:15"
	
	this.cornerWidth = 0		//must be square
	this.cornerTL = "images/corner1_000000_tl.gif"
	this.cornerTR = "images/corner1_000000_tr.gif"
	this.cornerBL = "images/corner1_000000_bl.gif"
	this.cornerBR = "images/corner1_000000_br.gif"
	
	this.tabSelected = 0
	
	this.isIE = (navigator.appVersion.indexOf('MSIE')>0);

	

	this.tabs = new Array()
	this.HTMLSections = new Array()
	
	this.addTab = tabBoxAddTab
	this.build = tabBoxBuild
	this.writeHTML = tabBoxWriteHTML
	this.clickTab = tabBoxClickTab
}
function tabBoxAddTab(strCaption, intWidth, strJavaScript){
	var intNewIndex = this.tabs.length
	this.tabs[intNewIndex] = new Object()
	this.tabs[intNewIndex].caption = strCaption
	this.tabs[intNewIndex].width = intWidth
	this.tabs[intNewIndex].javaScript = strJavaScript
}
function tabBoxBuild(){

	var strPath = ""
	for(var intCount=0;intCount<this.levelFromRoot;intCount++){
		strPath += "../"
	}

	var strDisplayTemp
	var strCursorTemp
	var strPositionTemp = ""

	//first section
	var strTemp = ""
	if(0 == this.tabSelected){
		strDisplayTemp = ""
	}else{
		strDisplayTemp = "none"
	}
	if(this.top != -1){
		strPositionTemp += ";top:" + this.top
	}
	if(this.left != -1){
		strPositionTemp += ";left:" + this.left
	}
	strTemp += "<div id='" + this.variableName + "Div' style='position:" + this.position + strPositionTemp + ";height:" + this.height + ";width:" + this.width + "'>"
	strTemp += "	<div style='background:" + this.borderColor + ";position:absolute;top:" + this.tabHeight + ";left:0;height:" + (this.height - this.tabHeight) + ";width:" + this.width + "'>"
	strTemp += "		<div style='background:" + this.backgroundColor + ";position:absolute;top:" + this.borderWidth + ";left:" + this.borderWidth + ";height:" + (this.height - this.tabHeight - (2*this.borderWidth)) + ";width:" + (this.width - (2 * this.borderWidth)) + ";overflow:auto'>"
	strTemp += "			<div id='" + this.variableName + "body0Div' style='display:" + strDisplayTemp + ";position:absolute;top:" + this.padding + ";left:" + (this.padding + 10) + ";height:" + (this.height - this.tabHeight - (2*this.borderWidth) - (2 * this.padding)) + ";width:" + (this.width - (2 * this.borderWidth) - (2 * this.padding)) + ";overflow:auto'>"
	this.HTMLSections[0] = strTemp
	
	//middle sections
	for(var intCount=1;intCount<this.tabs.length;intCount++){
		if(intCount == this.tabSelected){
			strDisplayTemp = ""
		}else{
			strDisplayTemp = "none"
		}
		strTemp = ""
		strTemp += "			</div>"
		strTemp += "			<div id='" + this.variableName + "body" + intCount + "Div' style='display:" + strDisplayTemp + ";position:absolute;top:" + this.padding + ";left:" + (this.padding + 10) + ";height:" + (this.height - this.tabHeight - (2*this.borderWidth) - (2 * this.padding)) + ";width:" + (this.width - (2 * this.borderWidth) - (2 * this.padding)) + ";overflow:auto'>"
		this.HTMLSections[intCount] = strTemp
	}
	
	//end section
	strTemp = ""
	strTemp += "			</div>"
	strTemp += "		</div>"
	strTemp += "	</div>"
	
	var intLeftTemp = this.tabIndent
	var intHeightTemp
	var strTabColorTemp
	var strJavaScript
	
	for(var intCount=0;intCount<this.tabs.length;intCount++){
		if(intCount == this.tabSelected){
			strCursorTemp = "default"
			intHeightTemp = this.tabHeight
			strTabColorTemp = this.backgroundColor
		}else{
			if(this.isIE){
				strCursorTemp = "hand"
			}else{
				strCursorTemp = "default"
			}
			intHeightTemp = this.tabHeight - this.borderWidth
			strTabColorTemp = this.backTabColor
		}
		if(this.tabs[intCount].javaScript == ""){
			strJavaScript = this.variableName + ".clickTab(" + intCount + ")"
		}else{
			strJavaScript = this.tabs[intCount].javaScript
		}
		strTemp += "	<div style='background:" + this.borderColor + ";position:absolute;top:0;left:" + intLeftTemp + ";height:" + this.tabHeight + ";width:" + this.tabs[intCount].width + "'>"
		strTemp += "		<div id='" + this.variableName + "Tab" + intCount + "Div' style='background:" + strTabColorTemp + ";position:absolute;top:" + this.borderWidth + ";left:" + this.borderWidth + ";height:" + intHeightTemp + ";width:" + (this.tabs[intCount].width - (2 * this.borderWidth)) + ";text-align:center;cursor:" + strCursorTemp + ";" + this.tabTextStyle + "' onclick=\"JavaScript:" + strJavaScript + "\">"
		strTemp += this.tabs[intCount].caption
		strTemp += "		</div>"
		strTemp += "	</div>"
		intLeftTemp += this.tabs[intCount].width + this.tabSpacing
	}
	if(this.cornerWidth > 0){
		strTemp += "	<IMG style='position:absolute;top:" + this.tabHeight + ";left:0' SRC='" + strPath + this.cornerTL + "' height='" + this.cornerWidth + "' width='" + this.cornerWidth + "' border='0'>"
		strTemp += "	<IMG style='position:absolute;top:" + this.tabHeight + ";left:" + (this.width - this.cornerWidth) + "' SRC='" + strPath + this.cornerTR + "' height='" + this.cornerWidth + "' width='" + this.cornerWidth + "' border='0'>"
		strTemp += "	<IMG style='position:absolute;top:" + (this.height - this.cornerWidth) + ";left:0' SRC='" + strPath + this.cornerBL + "' height='" + this.cornerWidth + "' width='" + this.cornerWidth + "' border='0'>"
		strTemp += "	<IMG style='position:absolute;top:" + (this.height - this.cornerWidth) + ";left:" + (this.width - this.cornerWidth) + "' SRC='" + strPath + this.cornerBR + "' height='" + this.cornerWidth + "' width='" + this.cornerWidth + "' border='0'>"
	}
	strTemp += "</div>"
	
	this.HTMLSections[this.tabs.length] = strTemp
	
	

}
function tabBoxWriteHTML(intIndex){
	document.write(this.HTMLSections[intIndex])
}
function tabBoxClickTab(intIndex){

	this.tabSelected = intIndex

	for(var intCount=0;intCount<intIndex;intCount++){
		objBodyDiv = document.getElementById(this.variableName + "body" + intCount + "Div");
		objBodyDiv.style.display = "none";	
		objTabDiv = document.getElementById(this.variableName + "Tab" + intCount + "Div");
		objTabDiv.style.background = this.backTabColor;
		if(this.isIE){
			objTabDiv.style.cursor = "hand";
			objTabDiv.style.posHeight = (this.tabHeight - this.borderWidth);
		}else{
			objTabDiv.style.cursor = "default";
			objTabDiv.style.height = (this.tabHeight - this.borderWidth);
		}
	}
	
	
	objBodyDiv = document.getElementById(this.variableName + "body" + intIndex + "Div");
	objBodyDiv.style.display = "";	
	objTabDiv = document.getElementById(this.variableName + "Tab" + intIndex + "Div");
	objTabDiv.style.posHeight = this.tabHeight;
	objTabDiv.style.height = this.tabHeight;
	objTabDiv.style.background = this.backgroundColor;
	objTabDiv.style.cursor = "default";
	
	for(var intCount=intIndex+1;intCount<this.tabs.length;intCount++){
		objBodyDiv = document.getElementById(this.variableName + "body" + intCount + "Div");
		objBodyDiv.style.display = "none";	
		objTabDiv = document.getElementById(this.variableName + "Tab" + intCount + "Div");
		objTabDiv.style.background = this.backTabColor;
		if(this.isIE){
			objTabDiv.style.cursor = "hand";
			objTabDiv.style.posHeight = (this.tabHeight - this.borderWidth);
		}else{
			objTabDiv.style.cursor = "default";
			objTabDiv.style.height = (this.tabHeight - this.borderWidth);
		}
	}
	
	
}