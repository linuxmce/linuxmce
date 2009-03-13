
	function popup(strURL, intLevel, strWindowName, intWidth, intHeight){
		if(intLevel == null){
			intLevel = 1;
		}
		var intX = 150 + intLevel * 50;
		var intY = intX;
		if(strWindowName == null){
			strWindowName = "childWindow" + intLevel;
		}
		if(intWidth == null){
			intWidth = 900;
		}
		if(intHeight == null){
			intHeight = 500;
		}
		
		if(inString(strURL, "?")){
			strURL += "&";
		}else{
			strURL += "?";
		}
		
		strURL += "popuplevel=" + intLevel;
		var objPopupWindow = window.open(strURL,strWindowName,"width=" + intWidth + ",height=" + intHeight + ",top=" + intY + ",left=" + intX + ",resizable=1,scrollbars=1,menubar=0");
		objPopupWindow.focus();
	}

	
	
	
	function stripInitialChars(str){
		str = new String(str)
		var intLength = str.length;
		for(var intCount=0; intCount < intLength; intCount++){
			if(!isNaN(str.substring(intCount,intLength))){
				return str.substring(intCount,intLength);
				break;
			}
		};
	};
	
	
	function inString(strOriginal, strCharMatch){
		//this function obviously need to be rewritten using regular expressions
		var blnFound = false;
		for (i=0; i < strOriginal.length; i++) {
			if (strOriginal.charAt(i) == strCharMatch) {
				blnFound = true;
			}
		}
		return blnFound;
	}

	
	
	function selectBoxPopulate(objSelectBox, strSerial){
		objSelectBox.options.length = 0;
		aPairs = strSerial.split("|");
		for(var intCount=0; intCount < aPairs.length; intCount++){
			if(aPairs[intCount] != ""){
				aPair = aPairs[intCount].split(",");
				objSelectBox.options[objSelectBox.options.length] = new Option(aPair[1], aPair[0]);
			}
		}	
	}

	function selectOption(objSelectBox,strValue){
		//this function receives a select box and a value
		//it will select the last option in the select box that has the given value
		var intIndex = -1
		for(var intCount=0;intCount<objSelectBox.options.length;intCount++){
			if(objSelectBox.options[intCount].value == strValue){
				intIndex = intCount
			}
		}
		if(intIndex > -1){
			objSelectBox.selectedIndex = intIndex
		}
	}
	
	
