<?
require_once('g/session_start.php');

$last_page = $_SESSION['last_page'];
$this_page = $_SESSION['this_page'];
require_once('customer_setup/globals.php');
$_SESSION['last_page'] = $last_page;
$_SESSION['this_page'] = $this_page;
?>

function CollapsableItem(strVariableName, strText, blnExpanded){
	
	this.variableName = strVariableName;
	this.text = strText;
	
	this.showDiv = new Object();
	this.hideDiv = new Object();
	
	this.expanded = blnExpanded;
	

	this.writeHTML = CollapsableItemWriteHTML;
	this.attachHTML = CollapsableItemAttachHTML;
	
	this.collapse = CollapsableItemCollapse;
	this.expand = CollapsableItemExpand;
	
	this.attached = false;

};


function CollapsableItemWriteHTML(intCount){

	strHTML = "";
	
	if(intCount == 0){
		strHTML += "<div id='divHide" + this.variableName + "'";
		
		if(this.expanded){
			strHTML += " style='display:none'>";
		}else{
			strHTML += " style='display:'>";
		};
		strHTML += "		<table width='100%'>";
		strHTML += "			<tr bgcolor='#D1D9EA'>";
		strHTML += "				<td><table cellpadding='2' cellspacing='0' border='0'><tr><td valign='bottom'><a href='JavaScript:" + this.variableName + ".expand()'><IMG SRC='/<?= APPLICATION_PATH ?>/inc/images/gbtn_expand.gif' border='0'></a></td><td valign='bottom'>&nbsp;<a href='JavaScript:" + this.variableName + ".expand()' class='normalLinkBold'>" + this.text + "</a></td></tr></table></td>";
		strHTML += "			</tr>";
		strHTML += "		</table>";
		strHTML += "	</div>";
		strHTML += "	<div id='divShow" + this.variableName + "'";
		if(this.expanded){
			strHTML += " style='display:'>";
		}else{
			strHTML += " style='display:none'>";
		};
		strHTML += "		<table width='100%'>";
		strHTML += "			<tr bgcolor='#D1D9EA'>";
		strHTML += "				<td colspan='2'><table cellpadding='2' cellspacing='0' border='0'><tr><td valign='bottom'><a href='JavaScript:" + this.variableName + ".collapse()'><IMG SRC='/<?= APPLICATION_PATH ?>/inc/images/gbtn_collapse.gif' border='0'></a></td><td valign='bottom'>&nbsp;<a href='JavaScript:" + this.variableName + ".collapse()' class='normalLinkBold'>" + this.text + "</a></td></tr></table></td>";
		strHTML += "			</tr>";
		strHTML += "			<tr>";
		strHTML += "				<td>&nbsp;</td>";
		strHTML += "				<td width='100%'>";

	};
	
	if(intCount == 1){
		strHTML += "			<br></td>";
		strHTML += "		</tr>";
		strHTML += "	</table>";
		strHTML += "</div>";
	};
	

	
	document.write(strHTML);

};


function CollapsableItemAttachHTML(){
	this.showDiv = document.getElementById("divShow" + this.variableName);
	this.hideDiv = document.getElementById("divHide" + this.variableName);
	this.attached = true;
};


function CollapsableItemCollapse(){
	if(!this.attached){this.attachHTML();};
	this.showDiv.style.display = "none";
	this.hideDiv.style.display = "";
	this.expanded = false;
};

function CollapsableItemExpand(){
	if(!this.attached){this.attachHTML();};
	this.showDiv.style.display = "";
	this.hideDiv.style.display = "none";
	this.expanded = true;
};





