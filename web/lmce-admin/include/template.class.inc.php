<?php 
 /*
 Site page builder for template file provided.
 
 The file should have:
 	{CssFile},{MetaTags},{Title}
 	{ScriptValidateSrc},{ScriptCalendar},{JavaScripts},{ScriptInBody}
 	{Body},{NavigationMenu},{RightMenu},{LeftMenu},{BottomMenu},{Logout}

 
 */
 
 /*Changelog:
 
 V1.0 - 20040724
 	- remove any project references
 	- rename methods and variables with nameOfMethod
 V1.0.1 - 20041005
 	- load template file without a case structure
 	
 */
 class Template
 {
  var $content;
  
  var $templateType="home";
  
  var $defaultMetaTags = '';
  var $defaultTitle='<-mkr_t_name_mixed->';
  var $defaultCssFile   = "include/styles/main.css";
  
  var $defaultScriptValidateSrc = 'scripts/formValidation.js';
  var $defaultScriptCalendar = '';
  
  var $defaultScriptInHead = '';
  
  var $metaTags		 = '';
  var $title     = '';
  var $cssFile   = '';
  
  
  var $scriptValidateSrc = '';
  var $scriptAnotherJS=''; //can be a list of js files,separated with commas [,]    
  var $scriptCalendar = '';
  var $scriptInBody;
  var $scriptInHead;
  
  var $navigationMenu=''; 
  
  var $loginForm   = '';
  
  var $leftMenu   = '';
  var $rightMenu  = '';
  
  var $topMenu = '';
  
  var $bottomMenu = '';
  var $HelpSrc= "/wiki/index.php/<-mkr_t_name_mixed->_Admin_Website";
  var $body = "";
  
  var $leftFrameSrc = 'index.php?section=wizard';
  var $rightFrameSrc = 'index.php?section=deviceTemplates';
  
  var $reloadLeftFrame=true;
  var $MenuTitle = '';
  var $PageTitle = '';
  
  var $dbADO;
  function Template($myDbADO) {
  	$this->dbADO=$myDbADO;
  }
  
  
function setTemplateFileType($type) { 	
	$this->templateType = $type;
	switch($type){
		case 'homeWithFrames':
			if(@$_REQUEST['section']=='mediaFilesSyncFrameset' || @$_REQUEST['section']=='mediaBrowserFrameset'){
				$_SESSION['lastLeftFrameSrc']='';
				$_SESSION['lastRightFrameSrc']='';
			}
		break;
		case 'large':
			if(@$_REQUEST['section']!='checkAmazon' && @$_REQUEST['section']!='orbitersWin'){
				if(@$_REQUEST['section']!=''){
					@$_SESSION['lastRightFrameSrc']='index.php?'.$_SERVER['QUERY_STRING'];
				}
			}
		break;
		case 'small':
			if(@$_REQUEST['section']=='leftMenu' || @$_REQUEST['section']=='wizard' || @$_REQUEST['section']=='leftMediaBrowser'){
				@$_SESSION['lastLeftFrameSrc']='index.php?'.$_SERVER['QUERY_STRING'];
			}
		break;
		
	}
	
	if (file_exists(APPROOT."include/templates/template.$type.tpl.html")) {
		$this->content = implode('',file(APPROOT."include/templates/template.$type.tpl.html"));
	} else {
		if (file_exists(APPROOT."include/templates/template.home.tpl.html")) {
			$this->content = implode('',file(APPROOT."include/templates/template.home.tpl.html"));  		
		} else {
			die('Please provide at least a include/templates/template.home.tpl.html file to be used as a template!');
		}
	}
	
}		
  
  //setting functions (if 'null' [not the NULL constant, but the 'null' string] is the parameter, the script will replace with nothing;
  //if empty string '' is passed, the default value will be used
  
  function setTitle($myTitle) {
  	$this->title = $myTitle;
  }
  
  function setTopMenu($myTopMenu) {
  	$this->topMenu = $myTopMenu;
  }
  
  function setMetaTags($myMetaTags) {
  	$this->metaTags = $myMetaTags;
  }
  
  function setCssFile($myCss) {
  	$this->cssFile = $myCss;
  }

     
  function setScriptInBody($myScriptInBody) {
  		$this->scriptInBody = $myScriptInBody;
  }
  
  function setScriptInHead($myScriptInHead) {
  		$this->scriptInHead = $myScriptInHead;
  }
  
  function setScriptValidateSrc($myScriptValidateSrc) {
  	$this->scriptValidateSrc = $myScriptSrc;
  }
  
  function setScriptCalendar($myScript) {
  	$this->scriptCalendar = $myScript;
  }

  function setLeftFrameSrc($myLeftFrameSrc) {
	// use $this->lastLeftFrameSrc is available
  	$this->leftFrameSrc = (@$_SESSION['lastLeftFrameSrc']!='')?$_SESSION['lastLeftFrameSrc']:$myLeftFrameSrc;
  	@$_SESSION['lastLeftFrameSrc']=$myLeftFrameSrc;
  }
  
  function setRightFrameSrc($myRightFrameSrc) {
  	// use $this->lastRightFrameSrc
  	$this->rightFrameSrc = (@$_SESSION['lastRightFrameSrc']!='')?$_SESSION['lastRightFrameSrc']:$myRightFrameSrc;
  	@$_SESSION['lastRightFrameSrc']=$myRightFrameSrc;
  }
  
  function setScriptAnotherJS($myScript) {
  	$this->scriptAnotherJS = $myScript;
  }
  
  function setHelpSrc($myHelpSrc) {
  	$this->HelpSrc = $myHelpSrc;
  }
  
  function setNavigationMenu($new_menuNavigation) {  	 
  	if (count($new_menuNavigation)>0) {
  		if (is_array(@$new_menuNavigation[0])) {  			
  			$menuString = '
				<span>';
					 
				 	reset($new_menuNavigation[0]);
				 	$i=1;
				 	$menuString.='<a href="index.php?section=userHome" >Home</a> &gt; ';
				 	
			  	 	foreach($new_menuNavigation[0] as $key=>$value){
			  	 		$valueArray = explode(":!:",$value);
			  	 		if (trim($value[1])!='') {
			  	 			$menuString.= '<a href="'.$valueArray[1].'">'.$valueArray[0].'</a>';
			  	 		} else {
			  	 			$menuString.= '<span>'.$valueArray[0].'</span>';
			  	 		}
			  	 		if ($i<count($new_menuNavigation[0])) {
			  	 			$menuString.=" &gt; ";
			  	 		} else {
			  	 			break;
			  	 		}
			  	 		$i++;
			  	 	}				  	 	
			  	  		  	 
		 		
		        $menuString.= '</span>';		 
		        
  		} else {
  			$menuString = '
				<span>';
					 if (count($new_menuNavigation)>0) {		  	 	
					 	reset($new_menuNavigation);
					 	$i=1;
					 	$menuString.='<a href="index.php?section=userHome">Home</a> &gt; ';
				  	 	foreach($new_menuNavigation as $key=>$value){
				  	 		if (trim($value)!='') {
				  	 			$menuString.= '<a href="'.$value.'">'.$key.'</a>';
				  	 		} else {
				  	 			$menuString.= '<span>'.$key.'</span>';
				  	 		}
				  	 		if ($i<count($new_menuNavigation)) {
				  	 			$menuString.=" &gt; ";
				  	 		} else {
				  	 			break;
				  	 		}
				  	 		$i++;
				  	 	}				  	 	
				  	 } 		  	 
		 		
		        $menuString.= '</span>';		  	
  		}
  	} else {
  		$menuString='';
  	}
  	
  	  //$menuString = $menuString."&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;".strftime("%A %d %B %Y,%H:%M");
      $this->navigationMenu=$menuString;
  }
  
  
  function setLoginForm($myLoginForm) {
  	$this->loginForm = $myLoginForm;
  }
  
  function setLeftMenu($myLeftMenu) {
  	$this->leftMenu = $myLeftMenu;
  }
  
  function setRightMenu($myRightMenu) {
  	$this->rightMenu = $myRightMenu;
  }
  
  function setBottomMenu($myBottomMenu) {
  	$this->bottomMenu = $myBottomMenu;
  }
      
  function setReloadLeftFrame($reloadLeftFrame) {
  	$this->reloadLeftFrame = $reloadLeftFrame;
  }
  
  function setMenuTitle($MenuTitle) {
  	$this->MenuTitle = $MenuTitle;
  }

  function setPageTitle($PageTitle) {
  	$this->PageTitle = $PageTitle;
  }

  function setBody($myBody) {
  	$this->body = $myBody;
  	if($this->reloadLeftFrame==true && $this->templateType=='large'){
  		$this->body.='<script>
  			try{
	  			var queryStr=top.treeframe.location.search.substring(9,top.treeframe.location.search.length);
	  			if(queryStr.substr(0,8)!=\'leftMenu\' && queryStr.substr(0,6)!=\'wizard\' && queryStr!=\'\'){
	  				top.treeframe.location=\'index.php?section=wizard\';
	  			}
	  		}catch(e){
	  			// do nothing, it\'s not in frame
	  		}  			
  			</script>';
  	}

  }
  
  //display functions
  function displayTitle(){
	  	if ($this->title=='null') {
	 		$this->title='';   
	  	} elseif (strlen(trim($this->title))==0) {
	  		$this->title=$this->defaultTitle;
	  	}
  		$this->content = str_replace("{Title}", $this->title, $this->content);
  }
  
  function displayMetaTags(){  	
	  	if ($this->metaTags=='null') {
	 		$this->metaTags='';   
	  	} elseif (strlen(trim($this->metaTags))==0) {
	  		$this->metaTags=$this->defaultMetaTags;
	  	}
    $this->content = str_replace("{MetaTags}", $this->metaTags, $this->content);
  }
  
  function displayCssFile(){  	  
	  	if ($this->cssFile=='null') {
	 		$this->cssFile='';   	 		
	  	} elseif (strlen(trim($this->cssFile))==0) {	  		
	  		$this->cssFile=$this->defaultCssFile;
	  	}
	  	
		if ($this->cssFile!='') {
   			$this->content = str_replace("{CssFile}", "<link rel='stylesheet' type='text/css' href='".$this->cssFile."'>", $this->content); 
		}else{
			$this->content = str_replace("{CssFile}", '', $this->content); 
		}
  }

  //------------------------------------------------------------------------------------------
  function displayScriptAnotherJS() {
  	$scriptsPaths = explode(",",$this->scriptAnotherJS);
  	$replacementText='';
  	if (count($scriptsPaths)>0) {
  		foreach ($scriptsPaths as $scriptPath) {
  			if (strlen(trim($scriptPath))>0) {
  				$replacementText.='<script src="'.$scriptPath.'" type="text/javascript" language="JavaScript"></script>'."\n";
  			}
  		}
  	}
   $this->content = str_replace("{JavaScripts}",$replacementText,$this->content);
  }
   
 
   
  function displayScriptValidateSrc(){
  	
	  	if ($this->scriptValidateSrc=='null') {
	 		$this->scriptValidateSrc='';   
	  	} elseif (strlen(trim($this->scriptValidateSrc))==0) {
	  		$this->scriptValidateSrc=$this->defaultScriptValidateSrc;
	  	}
	  	
	  	if ($this->scriptValidateSrc!='') {	
    		$this->content = str_replace("{ScriptValidateSrc}",'<script src="'.$this->scriptValidateSrc.'" type="text/javascript" language="JavaScript"></script>'."\n",$this->content);
	  	} else {
	  		$this->content = str_replace("{ScriptValidateSrc}",'',$this->content);
	  	}
  }

  function displayScriptCalendar() {
  	
	  	if ($this->scriptCalendar=='null') {
	 		$this->scriptCalendar='';   
	  	} elseif (strlen(trim($this->scriptCalendar))==0) {
	  		$this->scriptCalendar=$this->defaultScriptCalendar;
	  	}
	  	
   	$this->content = str_replace("{ScriptCalendar}", $this->scriptCalendar,$this->content);
  }
  
  function displayLeftFrameSrc(){  	
	  	if ($this->leftFrameSrc=='null') {
	 		$this->leftFrameSrc='';   
	  	} elseif (strlen(trim($this->leftFrameSrc))==0) {
	  		$this->leftFrameSrc=$this->leftFrameSrc;
	  	}
    $this->content = str_replace("{LeftFrame}", $this->leftFrameSrc, $this->content);
  }
  
  function displayRightFrameSrc(){  	
	  	if ($this->rightFrameSrc=='null') {
	 		$this->rightFrameSrc='';   
	  	} elseif (strlen(trim($this->rightFrameSrc))==0) {
	  		$this->rightFrameSrc=$this->rightFrameSrc;
	  	}
    $this->content = str_replace("{RightFrame}", $this->rightFrameSrc, $this->content);
  }
  
  
  function displayScriptInBody(){
  	//no default values ;)  		
    $this->content = str_replace("{ScriptInBody}",$this->scriptInBody,$this->content);
  }

  function displayScriptInHead() {
  	
	  	if ($this->scriptInHead=='null') {
	 		$this->scriptInHead='';   
	  	} elseif (strlen(trim($this->scriptInHead))==0) {
	  		$this->scriptInHead=$this->defaultScriptInHead;
	  	}
	  	
   	$this->content = str_replace("{ScriptInHead}", $this->scriptInHead,$this->content);
  }
  
   function displayHelpSrc(){
  	global $defaultHelpUrl,$wikiHost;
  	if($this->HelpSrc=='')
  		$this->HelpSrc=$defaultHelpUrl;
  	$this->HelpSrc=str_replace('/wiki/',$wikiHost,$this->HelpSrc);
	$this->content = str_replace("{HelpSrc}", $this->HelpSrc, $this->content);
  }
  
   //------------------------------------------------------------------------------------------  
	
  function displayMenuNavigation() {
  	if ($this->navigationMenu=='null') {
		$this->navigationMenu='';   
	}
    $this->content=str_replace('{NavigationMenu}', $this->navigationMenu,$this->content);
  }

  function displayLoginForm(){   
	if ($this->loginForm=='null') {
		$this->loginForm='';   
	}
	
    $this->content = str_replace("{LoginForm}", $this->loginForm, $this->content);
  }
  
  function displayLeftMenu(){   
	if ($this->leftMenu=='null') {
		$this->leftMenu='';   
	}
	
    $this->content = str_replace("{LeftMenu}", $this->leftMenu, $this->content);
  }
  
  function displayRightMenu(){
	if ($this->rightMenu=='null') {
		$this->rigthMenu='';   
	}
	
	$this->content = str_replace("{RightMenu}", $this->rightMenu, $this->content);
  }
  
  function displayTopMenu(){
	if (@$_SESSION['userLoggedIn']==true) {
	
	if($this->templateType!='index')
		$topMenu = generateTopMenu(1,(int)@$_SESSION['installationID'],$this->dbADO);

	} else {
		$topMenu='';
	}
	$this->content = str_replace("{TopMenu}", $topMenu, $this->content);
	$this->content = str_replace("{MenuTitle}", $this->MenuTitle, $this->content );
	$this->content = str_replace("{PageTitle}", $this->PageTitle,$this->content);
  }
  
  function displayBottomMenu(){
  	if ($this->bottomMenu=='null') {
		$this->bottomMenu='';   
	} 
	
    $this->content = str_replace("{BottomMenu}", $this->bottomMenu, $this->content);
  }
  
  function displayBody(){
   $this->content = str_replace("{Body}", $this->body, $this->content);
  }

  function displayLogout() {
  	if (@$_SESSION['customerIsLogged']==true) {
  		$this->content = str_replace("{Logout}", '<br /><span><b>'.$_SESSION['customerName'].'</b></span><br /><a href="index.php?section=customerLoginLogout&action=logout">Logout</a><br />', $this->content);
  	} elseif (@$_SESSION['adminIsLogged']==true) {
  		$this->content = str_replace("{Logout}", '<br /><span><b>'.$_SESSION['adminName'].'</b></span><br /><a href="index.php?section=adminLoginLogout&action=logout">Logout</a><br />', $this->content);
  	} else {
  		$this->content = str_replace("{Logout}", '', $this->content);
  	}
  }
  
  
  function displayContent(){
  	
	$GLOBALS['server_end_time']=getmicrotime();			
	parse_str($_SERVER['QUERY_STRING']);
	if(isset($section) && $GLOBALS['save_loading_time']!=''){
		$loadingTime=round(($GLOBALS['server_end_time']-$GLOBALS['server_start_time']),3);
		$loadingTimeToLog=$section."\t".$loadingTime."\t".$_SERVER['QUERY_STRING'];
		if($GLOBALS['display_loading_time']==1){
			$this->content.='<p class="normaltext">'.$loadingTime.' s';
		}
		writeFile($GLOBALS['save_loading_time'].'loadingTimes',$loadingTimeToLog."\n",'a');
	}
	
	echo $this->content;
  }

  function output() {
  	// todo: remove
  	
	if(isset($_SESSION['cssFile'])){
		$this->setCssFile($_SESSION['cssFile']);
	}
  	
  	
  	$this->displayTitle();
   	$this->displayMetaTags();
   	$this->displayCssFile();
   
   	$this->displayScriptCalendar();
   	  
   	$this->displayScriptValidateSrc();
   	$this->displayScriptAnotherJS();
   	$this->displayScriptInHead();
   	
   	$this->displayLeftFrameSrc();
   	$this->displayRightFrameSrc();
   	
   	$this->displayScriptInBody();
   	
	$this->displayMenuNavigation();      	
	$this->displayLogout();
	$this->displayLoginForm();
   	$this->displayLeftMenu();
   	$this->displayRightMenu();
   	$this->displayTopMenu();
   	$this->displayHelpSrc();
   	$this->displayBottomMenu();
   	$this->displayBody();
   
   	$this->displayContent();
  }
  
  
 }
?>