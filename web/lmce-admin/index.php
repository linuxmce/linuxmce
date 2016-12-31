<?php
header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");
header("Cache-Control: no-store, no-cache, must-revalidate");
header("Cache-Control: post-check=0, pre-check=0", false);
header("Pragma: no-cache");

session_name('Web-admin');
session_start();

require('include/config/config.inc.php');
require('include/utils.inc.php');
require('include/template.class.inc.php');

$GLOBALS['server_start_time']=getmicrotime();

$section = @$_REQUEST['section'];

if( !isset($_SESSION['userLoggedIn']) && $section=='authorize' ) {
	
      $authorization=$_REQUEST['state'];
      $client_id=$_GET['client_id'];
      $client_secret=$_GET['client_secret'];
 echo "authorizing:".$authorization;
           if($authorization!=""){
          die('<script>top.location="index.php?section=login&authorize='.$authorization.'&client_id='.$client_id.'&client_secret='.$client_secret.'"</script>');
  }

}

if(!isset($_SESSION['userLoggedIn']) && $section!='' && $section!='login' && $section!='wizard' && $section!='proxySocket' && $section!='orbitersWin'){

	if($section=='createUser'){
		$users=getAssocArray('Users','PK_Users','Username',$dbADO);
		if(count($users)>0){
			unset($_SESSION);
			session_destroy();

			die('<script>top.location="index.php?section=login"</script>');
		}
	}else{
		// invalid session, destroy it and send user to login
		unset($_SESSION);
		session_destroy();
	die('<script>top.location="index.php?section=login"</script>');
	}
}
// if installation ID is 1 (offline installation), check if it doesn't changed
validate_installation((int)@$_SESSION['installationID'],$dbADO);

switch ($section) {
		case 'index';
		@$_SESSION['lastLeftFrameSrc']='';
		@$_SESSION['lastRightFrameSrc']='';

		die('<script>self.location=\'index.php\'</script>');
		exit();
	break;
	case 'login':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/login.php');
	    login($output,$dbADO);
	break;
	case 'userHome':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/userHome.php');
	    userHome($output,$dbADO);
	break;
	case 'deviceTemplates':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		$output->setHelpSrc('/wiki/index.php/List_Page');
	    include_once('operations/deviceTemplates.php');
	    deviceTemplates($output,$dbADO);
	break;
	case 'irCodes':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/irCodes.php');
	    irCodes($output,$dbADO,$mediadbADO);
	break;
	case 'addManufacturer':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/manufacturers/addManufacturer.php');
	    addManufacturer($output,$dbADO);
	break;
	case 'editMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
		$output->setHelpSrc('/wiki/index.php/Edit_Page');
	    include_once('operations/editMasterDevice.php');
	    editMasterDevice($output,$dbADO);
	break;
	case 'addObjectToMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/objects/addObjectToMasterDevice.php');
	    addObjectToMasterDevice($output,$dbADO);
	break;
	case 'deleteObjectFromMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/objects/deleteObjectFromMasterDevice.php');
	    deleteObjectFromMasterDevice($output,$dbADO);
	break;
	case 'addControlledViaToMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/controlledVia/addControlledViaToMasterDevice.php');
	    addControlledViaToMasterDevice($output,$dbADO);
	break;
	case 'editControlledViaToMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/controlledVia/editControlledViaToMasterDevice.php');
	    editControlledViaToMasterDevice($output,$dbADO);
	break;
	case 'deleteControlledViaFromMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/controlledVia/deleteControlledViaFromMasterDevice.php');
	    deleteControlledViaFromMasterDevice($output,$dbADO);
	break;
	case 'deletePipeFromDeviceTemplateControlledVia':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/controlledVia/deletePipeFromDeviceTemplateControlledVia.php');
	    deletePipeFromDeviceTemplateControlledVia($output,$dbADO);
	break;
	case 'deletePipeFromDeviceTemplateControlledViaCategory':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/controlledVia/deletePipeFromDeviceTemplateControlledViaCategory.php');
	    deletePipeFromDeviceTemplateControlledViaCategory($output,$dbADO);
	break;
	case 'addControlledViaCategoryToMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/controlledVia/addControlledViaCategoryToMasterDevice.php');
	    addControlledViaCategoryToMasterDevice($output,$dbADO);
	break;
	case 'editControlledViaCategoryToMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/controlledVia/editControlledViaCategoryToMasterDevice.php');
	    editControlledViaCategoryToMasterDevice($output,$dbADO);
	break;
	case 'deleteControlledViaCategoryFromMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/controlledVia/deleteControlledViaCategoryFromMasterDevice.php');
	    deleteControlledViaCategoryFromMasterDevice($output,$dbADO);
	break;
	case 'addPackageToMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/addPackageToMasterDevice.php');
	    addPackageToMasterDevice($output,$dbADO);
	break;
	case 'addPackage':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/addPackage.php');
	    addPackage($output,$dbADO);
	break;
	case 'packagesList':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		$output->setHelpSrc('/wiki/index.php/Packages');
	    include_once('operations/packages/packagesList.php');
	    packagesList($output,$dbADO);
	break;
	case 'addDistro':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/addDistro.php');
	    addDistro($output,$dbADO);
	break;
	case 'distroList':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/packages/distroList.php');
	    distroList($output,$dbADO);
	break;
	case 'addOS':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/addOS.php');
	    addOS($output,$dbADO);
	break;
	case 'osList':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/packages/osList.php');
	    osList($output,$dbADO);
	break;
	case 'editRepositorySource':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/editRepositorySource.php');
	    editRepositorySource($output,$dbADO);
	break;
	case 'repositorySourceList':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/packages/repositorySourceList.php');
	    repositorySourceList($output,$dbADO);
	break;
	case 'addDirectory':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/addDirectory.php');
	    addDirectory($output,$dbADO);
	break;	
	case 'addDirectoryFile':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/addDirectoryFile.php');
	    addDirectoryFile($output,$dbADO);
	break;	
	case 'editDirectoryFile':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/editDirectoryFile.php');
	    editDirectoryFile($output,$dbADO);
	break;
	case 'deleteDirectoryFile':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/deleteDirectoryFile.php');
	    deleteDirectoryFile($output,$dbADO);
	break;
		
	case 'editPackageToMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/editPackageToMasterDevice.php');
	    editPackageToMasterDevice($output,$dbADO);
	break;	
	case 'deletePackageFromMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/packages/deletePackageFromMasterDevice.php');
	    deletePackageFromMasterDevice($output,$dbADO);
	break;	
	case 'addParameter':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/parameters/addParameter.php');
	    addParameter($output,$dbADO);
	break;	
	case 'editDeviceData':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/parameters/editDeviceData.php');
	    editDeviceData($output,$dbADO);
	break;	
	case 'addParameterToMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/parameters/addParameterToMasterDevice.php');
	    addParameterToMasterDevice($output,$dbADO);
	break;	
	case 'deleteParameterFromMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/parameters/deleteParameterFromMasterDevice.php');
	    deleteParameterFromMasterDevice($output,$dbADO);
	break;	
	//event list
	case 'deleteEventFromMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/events/deleteEventFromMasterDevice.php');
	    deleteEventFromMasterDevice($output,$dbADO);
	break;	
	case 'createEvent':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/events/createEvent.php');
	    createEvent($output,$dbADO);
	break;	
	case 'createEventCategory':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/events/createEventCategory.php');
	    createEventCategory($output,$dbADO);
	break;
	case 'editEvent':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/events/editEvent.php');
	    editEvent($output,$dbADO);
	break;	
	case 'editEventParameterFromEvent':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/events/editEventParameterFromEvent.php');
	    editEventParameterFromEvent($output,$dbADO);
	break;
	case 'deleteEventParameterFromEvent':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/events/deleteEventParameterFromEvent.php');
	    deleteEventParameterFromEvent($output,$dbADO);
	break;
	case 'createEventParameter':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/events/createEventParameter.php');
	    createEventParameter($output,$dbADO);
	break;	
	//commands
	case 'addCommandGroup':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/commands/addCommandGroup.php');
	    addCommandGroup($output,$dbADO);	    
	break;	
	case 'editCommandGroupFromMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/commands/editCommandGroupFromMasterDevice.php');
	    editCommandGroupFromMasterDevice($output,$dbADO);	    
	break;	
	case 'addCommand':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/commands/addCommand.php');
	    addCommand($output,$dbADO);	    
	break;
	case 'editCommand':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/commands/editCommand.php');
	    editCommand($output,$dbADO);	    
	break;
	case 'deleteParameterFromCommand':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/commands/deleteParameterFromCommand.php');
	    deleteParameterFromCommand($output,$dbADO);	    
	break;	
	case 'deleteCommandFromMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/commands/deleteCommandFromMasterDevice.php');
	    deleteCommandFromMasterDevice($output,$dbADO);	    
	break;	
	case 'createParameterForCommand':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/commands/createParameterForCommand.php');
	    createParameterForCommand($output,$dbADO);	    
	break;	
	case 'editParameterFromCommand':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/commands/editParameterFromCommand.php');
	    editParameterFromCommand($output,$dbADO);	    
	break;	
	//device params
	
	case 'editDeviceParams':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/editDeviceParams.php');
	    editDeviceParams($output,$dbADO);	    
	break;	
		
	case 'myDevices'://used by myDevicesFrames
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/myDevices.php');
	    myDevices($output,$dbADO);	    
	break;	
	case 'addMyDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/addMyDevice.php');
	    addMyDevice($output,$dbADO);	    
	break;
	case 'deleteMyDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/deleteMyDevice.php');
	    deleteMyDevice($output,$dbADO);	    
	break;
	case 'devices':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/devices.php');
	    $output->setHelpSrc('/wiki/index.php/Devices');
	    devices($output,$dbADO);
	break;
	case 'devicesAV':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/devicesAV.php');
	    devicesAV($output,$dbADO);
	break;
		
	//deviceCategory
	case 'editDeviceCategory':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/deviceCategory/editDeviceCategory.php');
	    editDeviceCategory($output,$dbADO);	    
	break;
	case 'deleteDeviceCategory':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/deviceCategory/deleteDeviceCategory.php');
	    deleteDeviceCategory($output,$dbADO);	    
	break;
	case 'deleteDeviceRelatedFromDeviceParams':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/deleteDeviceRelatedFromDeviceParams.php');
	    deleteDeviceRelatedFromDeviceParams($output,$dbADO);	    
	break;
	case 'deleteDevicePipeFromDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/deleteDevicePipeFromDevice.php');
	    deleteDevicePipeFromDevice($output,$dbADO);	    
	break;
	case 'deleteDeviceFromDeviceGroup':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/deleteDeviceFromDeviceGroup.php');
	    deleteDeviceFromDeviceGroup($output,$dbADO);	    
	break;	
	case 'createDeviceGroup':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/createDeviceGroup.php');
	    createDeviceGroup($output,$dbADO);	    
	break;
	case 'leftMenu':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/leftMenu.php');
	    leftMenu($output,$dbADO);	    
	break;
	//rooms
	case 'rooms':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		$output->setHelpSrc('/wiki/index.php/Rooms');
	    include_once('operations/rooms/rooms.php');
	    rooms($output,$dbADO);	    
	break;	
	case 'deleteRoomFromInstallation':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/rooms/deleteRoomFromInstallation.php');
	    deleteRoomFromInstallation($output,$dbADO);	    
	break;	
	case 'entertainArea':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/entertainArea/entertainArea.php');
	    entertainArea($output,$dbADO);	    
	break;
	case 'deleteRoomFromEntertainArea':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/entertainArea/deleteRoomFromEntertainArea.php');
	    deleteRoomFromEntertainArea($output,$dbADO);	    
	break;	 
	//users_settings
        case 'emailSetup':
                $output = new Template($dbADO);
                $output->setTemplateFileType('large');
            include_once('operations/users_settings/emailSetup.php');
            emailSetup($output,$dbADO);
        break;
	case 'installationSettings':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');

	    include_once('operations/users_settings/installationSettings.php');
	    installationSettings($output,$dbADO);	    
	break;
	case 'createInstallation':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/users_settings/createInstallation.php');
	    createInstallation($output,$dbADO);	    
	break;
	case 'users':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		$output->setHelpSrc('/wiki/index.php/Users');
	    include_once('operations/users_settings/users.php');
	    users($output,$dbADO);	    
	break;
	case 'createUser':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/users_settings/createUser.php');
	    createUser($output,$dbADO);	    
	break;
	case 'userChangePassword':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/users_settings/userChangePassword.php');
	    userChangePassword($output,$dbADO);	    
	break;
	case 'userChangePIN':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/users_settings/userChangePIN.php');
	    userChangePIN($output,$dbADO);	    
	break;
        case 'userChangeVPNPassword':
        $output = new Template($dbADO);
        $output->setTemplateFileType('small');
        include_once('operations/users_settings/userChangeVPNPassword.php');
        userChangeVPNPassword($output,$dbADO);
        break;
	
	//orbiters
	case 'orbiters':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/orbiters/orbiters.php');
	    $output->setHelpSrc('/wiki/index.php/Orbiters');
	    orbiters($output,$dbADO);	    
	break;
	case 'wizardOrbiters':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		$output->setHelpSrc('/wiki/index.php/Orbiters');
	    include_once('operations/myDevices/orbiters/wizardOrbiters.php');
	    wizardOrbiters($output,$dbADO);	    
	break;
        case 'setupWebOrbiter';
                $output = new Template($dbADO);
                $output->setTemplateFileType('large');
            include_once('operations/myDevices/orbiters/setupWebOrbiter.php');
            $output->setHelpSrc('/wiki/index.php/Orbiters');
            setupWebOrbiter($output,$dbADO);
        break;

	//categories
	case 'manageCategories':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/categories/manageCategories.php');
	    manageCategories($output,$dbADO);
	break;
	case 'deleteCategory':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/categories/deleteCategory.php');
	    deleteCategory($output,$dbADO);	    
	break;
	case 'editCategory':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/categories/editCategory.php');
	    editCategory($output,$dbADO);	    
	break;
	case 'addCategory':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/categories/addCategory.php');
	    addCategory($output,$dbADO);	    
	break;
	
	//my scenarios
	case 'myScenarios':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myScenarios/myScenarios.php');
	    $output->setHelpSrc('/wiki/index.php/Scenarios');
	    myScenarios($output,$dbADO);
	break;
	case 'mediaScenarios':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		$output->setHelpSrc('/wiki/index.php/Media_Scenarios');
	    include_once('operations/myScenarios/mediaScenarios.php');
	    mediaScenarios($output,$dbADO);
	break;
	
	case 'editCommandGroup':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myScenarios/editCommandGroup.php');
	    editCommandGroup($output,$dbADO);
	break;
	case 'lightingScenarios':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		$output->setHelpSrc('/wiki/index.php/Lighting_Scenarios');
	    include_once('operations/myScenarios/lightingScenarios.php');
	    lightingScenarios($output,$dbADO);
	break;
	case 'climateScenarios':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		$output->setHelpSrc('/wiki/index.php/Climate_Scenarios');
	    include_once('operations/myScenarios/climateScenarios.php');
	    climateScenarios($output,$dbADO);
	break;	
	case 'irrigationScenarios':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myScenarios/irrigationScenarios.php');
	    irrigationScenarios($output,$dbADO);
	break;	
	case 'addIrrigationScenario':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myScenarios/addIrrigationScenario.php');
	    addIrrigationScenario($output,$dbADO);
	break;	
	case 'editIrrigationScenario':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myScenarios/editIrrigationScenario.php');
	    editIrrigationScenario($output,$dbADO);
	break;	
	
	case 'wizardScenarios':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myScenarios/wizardScenarios.php');
	    wizardScenarios($output,$dbADO);
	break;
	case 'deleteCommandFromCommandGroup_Command':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myScenarios/deleteCommandFromCommandGroup_Command.php');
	    deleteCommandFromCommandGroup_Command($output,$dbADO);
	break;
	case 'addCommandGroupAsScenario':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myScenarios/addCommandGroupAsScenario.php');
	    addCommandGroupAsScenario($output,$dbADO);	    
	break;	
	// infrared section
	case 'addInfraredGroup':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/infrared/addInfraredGroup.php');
	    addInfraredGroup($output,$dbADO);	    
	break;	
	case 'editInfraredGroupFromMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/infrared/editInfraredGroupFromMasterDevice.php');
	    editInfraredGroupFromMasterDevice($output,$dbADO);	    
	break;	
	case 'deleteInfraredCommandFromMasterDevice':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/infrared/deleteInfraredCommandFromMasterDevice.php');
	    deleteInfraredCommandFromMasterDevice($output,$dbADO);	    
	break;
	case 'bootSequence':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/bootSequence.php');
	    $output->setHelpSrc('/wiki/index.php/Boot_Sequence');
	    bootSequence($output,$dbADO);	    
	break;
	//plugin space
	case 'customPage':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
		$page=cleanString($_POST['page'],50);
		switch ($page) {
			case 'testXX':
				include_once('operations/pluginPages/pageXX.php');
	    		pageXX($output,$dbADO);	    				
			break;
			
			case 'testXX1':
				include_once('operations/pluginPages/pageXX1.php');
	    		pageXX1($output,$dbADO);	    				
			break;
		
			default:
			break;
		}
	break;
	case 'mediaBrowser';
	    include_once('operations/mediaBrowser/mediaBrowserFrameset.php');
	    mediaBrowserFrameset();	    
	break;		
	case 'mediaBrowserFrameset';
		$output = new Template($dbADO);	
		$output->setTemplateFileType('homeWithFrames');
		$output->setLeftFrameSrc('index.php?section=leftMediaBrowser');
		$output->setRightFrameSrc('index.php?section=mainMediaBrowser');
		$output->setBody('');		
		$output->setScriptCalendar('null');
		$output->setTitle(APPLICATION_NAME);			
  		$output->output();  		
	break;
	case 'leftMediaBrowser':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/mediaBrowser/leftMediaBrowser.php');
	    leftMediaBrowser($output,$dbADO);	    
	break;
	case 'mainMediaBrowser':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/mainMediaBrowser.php');
	    $output->setHelpSrc('/wiki/index.php/Files&Media');
	    mainMediaBrowser($output,$mediadbADO,$dbADO);	    
	break;
	case 'editMediaFile':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/editMediaFile.php');
	    editMediaFile($output,$mediadbADO,$dbADO);	    
	break;
	//ADDED BY PAUL MUMBY FOR RECURSIVE TAGGING CODE
	case 'editMediaTagsRecursive':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/editMediaTagsRecursive.php');
	    editMediaTagsRecursive($output,$mediadbADO,$dbADO);	    
	break;
	case 'editDirectoryAttributes':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/editDirectoryAttributes.php');
	    editDirectoryAttributes($output,$mediadbADO,$dbADO);	    
	break;
	case 'internetRadio':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/internet_radio.php');
	    internet_radio($output,$mediadbADO,$dbADO);	    
	break;
	case 'internetRadioAdd':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/internet_radio_add.php');
	    internet_radio_add($output,$mediadbADO,$dbADO);	    
	break;
	case 'playlists':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/playlists.php');
	    playlists($output,$mediadbADO);	    
	break;
	case 'editPlaylist':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/editPlaylist.php');
	    editPlaylist($output,$mediadbADO,$dbADO);	    
	break;
	case 'editPlaylistAdvanced':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/editPlaylistAdvanced.php');
	    editPlaylistAdvanced($output,$mediadbADO,$dbADO);	    
	break;
	case 'exportPlaylist':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
		include_once('operations/mediaBrowser/exportPlaylist.php');
		exportPlaylist($output,$mediadbADO,$dbADO);
	break;
	case 'powerFile':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/powerFile.php');
	    powerFile($output,$mediadbADO,$dbADO);	    
	break;
	case 'discAttributes':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/discAttributes.php');
	    discAttributes($output,$mediadbADO,$dbADO);	    
	break;	
	case 'ripStatus':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/mediaBrowser/ripStatus.php');
	    ripStatus($output,$mediadbADO,$dbADO);	    
	break;	

	
	case 'mediaFilesSync';
	    include_once('operations/mediaBrowser/mediaFilesSync.php');
	    mediaFilesSync();	    
	break;		
	case 'mediaFilesSyncFrameset';
		$output = new Template($dbADO);	
		$output->setTemplateFileType('homeWithFrames');
		$output->setLeftFrameSrc('index.php?section=leftMediaFilesSync');
		$output->setRightFrameSrc('index.php?section=mainMediaFilesSync');
		$output->setBody('');		
		$output->setScriptCalendar('null');
		$output->setTitle(APPLICATION_NAME);			
  		$output->output();  		
	break;
	case 'leftMediaFilesSync':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/mediaBrowser/leftMediaFilesSync.php');
	    leftMediaFilesSync($output,$mediadbADO,$dbADO);	    
	break;
	case 'mainMediaFilesSync':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/mainMediaFilesSync.php');
	    mainMediaFilesSync($output,$mediadbADO,$dbADO);	    
	break;
	
	case 'floorplanWizard':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/floorplan/floorplanWizard.php');
	    $output->setHelpSrc('/support/index.php?section=document&docID=137');
	    floorplanWizard($output,$dbADO);
	break;
	case 'uploadFloorplan':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/floorplan/uploadFloorplan.php');
	    $output->setHelpSrc('/wiki/index.php/Floorplans');
	    uploadFloorplan($output,$dbADO);
	break;
	case 'respondToEvents':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/events/respondToEvents.php');
	    $output->setHelpSrc('/wiki/index.php/Respond_to_Events');
	    respondToEvents($output,$dbADO);
	break;
	case 'editRespondToEvent':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/events/editRespondToEvent.php');
	    $output->setHelpSrc('/wiki/index.php/Respond_to_Events');
	    editRespondToEvent($output,$dbADO);
	break;
	case 'timedEvents':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/events/timedEvents.php');
	    $output->setHelpSrc('/wiki/index.php/Timed_Events');
	    timedEvents($output,$dbADO);
	break;
	case 'editTimedEvent':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/events/editTimedEvent.php');
	    $output->setHelpSrc('/wiki/index.php/Timed_Events');
	    editTimedEvent($output,$dbADO);
	break;
	case 'advancedEvents':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/events/advancedEvents.php');
	    $output->setHelpSrc('/wiki/index.php/Events');
	    advancedEvents($output,$dbADO);
	break;
	case 'createAdvancedEvent':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/events/createAdvancedEvent.php');
	    createAdvancedEvent($output,$dbADO);
	break;	
	case 'editCriteria':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/events/editCriteria.php');
	    editCriteria($output,$dbADO);
	break;
	case 'editEventCommands':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/events/editEventCommands.php');
	    editEventCommands($output,$dbADO);
	break;
	
	case 'securitySettings':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/securitySettings.php');
	    $output->setHelpSrc('/wiki/index.php/Active_sensors');
	    securitySettings($output,$dbADO,$securityADO);
	break;
	case 'outsideAccess':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/security/outsideAccess.php');
	    $output->setHelpSrc('/wiki/index.php/Outside_Access');
	    outsideAccess($output,$dbADO);
	break;
	
	case 'videoLinks':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/videoLinks.php');
	    $output->setHelpSrc('/wiki/index.php/Security-Video_Links');
	    videoLinks($output,$dbADO);
	break;
	case 'cellphoneNotifications':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/orbiters/cellphoneNotifications.php');
	    $output->setHelpSrc('/wiki/index.php/Notifications');
	    cellphoneNotifications($output,$dbADO);
	break;
	case 'securityScenarios':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myScenarios/securityScenarios.php');
	    $output->setHelpSrc('/wiki/index.php/Security');
	    securityScenarios($output,$dbADO);
	break;
	case 'deviceTemplatePicker':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
		$output->setHelpSrc('/wiki/index.php/List_Page');
	    include_once('operations/deviceTemplatePicker.php');
	    deviceTemplatePicker($output,$dbADO);
	break;
	case 'rubyCodes':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/rubyCodes.php');
	    $output->setHelpSrc('/wiki/index.php/Infrared_Codes');
	    rubyCodes($output,$dbADO,$mediadbADO);
	break;
	case 'avWizard':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/avWizard.php');
	    avWizard($output,$dbADO);
	break;
	case 'wizard';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/steps.php');
	    steps($output,$dbADO);
	break;
	
	case 'alertTypes';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/security/alertTypes.php');
	    $output->setHelpSrc('/wiki/index.php/Settings_By_Alert_Type');
	    alertTypes($output,$securityADO,$dbADO);
	break;
	case 'alertsLog';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/security/alertsLog.php');
	    $output->setHelpSrc('/wiki/index.php/Alert_Logs');
	    alertsLog($output,$securityADO,$dbADO);
	break;
	case 'modeChangesLog';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/security/modeChangesLog.php');
	    $output->setHelpSrc('/wiki/index.php/Mode_Change_Logs');
	    modeChangesLog($output,$securityADO,$dbADO);
	break;
	case 'securityStatus';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/security/securityStatus.php');
	    $output->setHelpSrc('/wiki/index.php/Status');
	    securityStatus($output,$dbADO);
	break;
	case 'telecomScenarios';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myScenarios/telecomScenarios.php');
	    $output->setHelpSrc('/wiki/index.php/Telecom');
	    telecomScenarios($output,$dbADO);
	break;
	//start webfilter &proxy
	case 'proxy';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/webfilter_proxy/proxy.php');
	    //$output->setHelpSrc('/wiki/index.php/proxy_Settings');
	    proxySettings($output,$dbADO);
	break;
	case 'webfilter';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/webfilter_proxy/webfilter.php');
	    //$output->setHelpSrc('/wiki/index.php/webfilter_Settings');
	    webfilterSettings($output,$dbADO);
	break;
	//end of webfilter & proxy
	case 'networkSettings';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/network/networkSettings.php');
	    $output->setHelpSrc('/wiki/index.php/Network_Settings');
	    networkSettings($output,$dbADO);
	break;
        case 'advancedSettings';
            $output = new Template($dbADO);
            $output->setTemplateFileType('large');
            include_once('operations/network/advancedSettings.php');
            $output->setHelpSrc('/wiki/index.php/Advanced_Settings');
            advancedSettings($output,$dbADO);
        break;
	case 'DHCPLeases';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/network/DHCPLeases.php');
	    $output->setHelpSrc('');
	    DHCPLeases($output,$dbADO);
	break;
	case 'firewall';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/network/firewall.php');
	    $output->setHelpSrc('/wiki/index.php/Firewall_Rules');
	    firewall($output,$dbADO);
	break;
        case 'dynamicDNS';
                $output=new Template($dbADO);
                $output->setTemplateFileType('large');
                include_once('operations/network/dynamicDNS.php');
                $output->setHelpSrc('');
                dynamicDNS($output,$dbADO);
        break;
	case 'usersOrbiters';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/users_settings/usersOrbiters.php');
	    $output->setHelpSrc('/wiki/index.php/Privacy_Settings');
	    usersOrbiters($output,$dbADO);
	break;
	case 'usersRooms';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/users_settings/usersRooms.php');
	    $output->setHelpSrc('/wiki/index.php/Privacy_Settings');
	    usersRooms($output,$dbADO);
	break;
	case 'dceDeviceData';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/dce/dceDeviceData.php');
	    $output->setHelpSrc('/wiki/index.php/DCE');
	    dceDeviceData($output,$dbADO);
	break;
	case 'dceEvents';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/dce/dceEvents.php');
	    $output->setHelpSrc('/wiki/index.php/DCE');
	    dceEvents($output,$dbADO);
	break;
	case 'dceCommands';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/dce/dceCommands.php');
	    $output->setHelpSrc('/wiki/index.php/DCE');
	    dceCommands($output,$dbADO);
	break;
	case 'addCommandCategory';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/commands/addCommandCategory.php');
	    addCommandCategory($output,$dbADO);
	break;
	case 'editCommandCategory';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/commands/editCommandCategory.php');
	    editCommandCategory($output,$dbADO);
	break;
	//Automation
	case 'deviceStatus';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/automation/deviceStatus.php');
	    $output->setHelpSrc('/wiki/index.php/Device_Status');
	    deviceStatus($output,$dbADO);
	break;
	case 'advancedZWave';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/automation/advancedZWave.php');
	    $output->setHelpSrc('/wiki/index.php/Advanced_ZWave');
	    advancedZWave($output,$dbADO);
	break;
	case 'executeScenarios';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/automation/executeScenarios.php');
	    $output->setHelpSrc('/wiki/index.php/Execute_Scenarios');
	    executeScenarios($output,$dbADO);
	break;
        case 'viewDatalog';
                $output = new Template($dbADO);
                $output->setTemplateFileType('large');
            include_once('operations/datalog/viewDatalog.php');
            $output->setHelpSrc('/wiki/index.php/DataLogger');
            viewDatalog($output,$dbADO);
        break;
        case 'datalogDetails';
                $output = new Template($dbADO);
                $output->setTemplateFileType('large');
            include_once('operations/datalog/details.php');
            $output->setHelpSrc('/wiki/index.php/DataLogger');
            datalogDetails($output,$dbADO);
        break;
        case 'energyMonitor';
                $output = new Template($dbADO);
                $output->setTemplateFileType('large');
            include_once('operations/datalog/energyMonitor.php');
            $output->setHelpSrc('/wiki/index.php/energyMonitor');
            energyMonitor($output,$dbADO);
        break;
	case 'thermostatSchedules';
		$output = new Template($dbADO);
               	$output->setTemplateFileType('large');
            include_once('operations/thermostat/editSchedules.php');
            $output->setHelpSrc('/wiki/index.php/thermostat_schedules');
            editSchedules($output,$dbADO);
	break;
	case 'pickScenario';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myScenarios/pickScenario.php');
	    pickScenario($output,$dbADO);
	break;
	
	case 'viewCameras';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/security/viewCameras.php');
	    $output->setHelpSrc('/wiki/index.php/View_Cameras');
	    viewCameras($output,$dbADO);
	break;
	
	case 'phoneLines';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		@include($GLOBALS['globalConfigPath'].'asteriskDB.inc.php');
		@include($GLOBALS['globalConfigPath'].'telecom.inc.php');
	    include_once('operations/phones/phoneLines.php');
	    $output->setHelpSrc('/wiki/index.php/Phone_Lines');
	    phoneLines($output,$asteriskADO,$dbADO,$telecomADO);
	break;
	case 'fax';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		@include($GLOBALS['globalConfigPath'].'asteriskDB.inc.php');
		@include($GLOBALS['globalConfigPath'].'telecom.inc.php');
	    include_once('operations/phones/fax.php');
	    $output->setHelpSrc('/wiki/index.php/fax');
	    fax($output,$asteriskADO,$dbADO,$telecomADO);
	break;
	case 'cdrviewer';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		include_once('operations/phones/cdrviewer.php');
	    $output->setHelpSrc('/wiki/index.php/cdrviewer');
	    cdrviewer($output);
	break;
	case 'phoneExtensions';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/phones/phoneExtensions.php');
	    $output->setHelpSrc('/wiki/index.php/Phone_Extensions');
	    phoneExtensions($output,$dbADO);
	break;
	case 'callRouting';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		@include($GLOBALS['globalConfigPath'].'asteriskDB.inc.php');
		@include($GLOBALS['globalConfigPath'].'telecom.inc.php');       			
	    include_once('operations/phones/callRouting.php');
	    callRouting($output,$dbADO,$asteriskADO,$telecomADO);
	break;
	case 'priorityCallers';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		@include($GLOBALS['globalConfigPath'].'telecom.inc.php');       			
	    include_once('operations/phones/priorityCallers.php');
	    priorityCallers($output,$dbADO,$telecomADO);
	break;
	case 'callersForMe';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		@include($GLOBALS['globalConfigPath'].'telecom.inc.php');       			
	    include_once('operations/phones/callersForMe.php');
	    callersForMe($output,$dbADO,$telecomADO);
	break;
	
	case 'addSoftware';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/others/addSoftware.php');
	    $output->setHelpSrc('/wiki/index.php/Add_Software');
	    addSoftware($output,$dbADO);
	break;	
	case 'learnCode';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/infrared/learnCode.php');
	    learnCode($output,$dbADO);
	break;	
	// changed name to preserve compatibility 
	case 'newIRCode';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/infrared/learnCode.php');
	    learnCode($output,$dbADO);
	break;	
	
	case 'mobileScenarios';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myScenarios/mobileScenarios.php');
	    mobileScenarios($output,$dbADO);
	break;	
	case 'sleepingScenarios';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myScenarios/sleepingScenarios.php');
	    sleepingScenarios($output,$dbADO);
	break;	
	case 'editDHCP';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/deviceTemplate/editDHCP.php');
	    editDHCP($output,$dbADO);
	break;	
	case 'infraredCommands';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/infrared/infraredCommands.php');
	    infraredCommands($output,$dbADO);
	break;	
	case 'eibDevices';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		include($GLOBALS['globalConfigPath'].'eibDB.inc.php');
	    include_once('operations/myDevices/eibDevices.php');
	    eibDevices($output,$dbADO,$eibADO);
	break;	
	case 'basicInfo';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/others/basicInfo.php');
	    basicInfo($output,$dbADO);
	break;	
	
	case 'softwareVersions';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/others/softwareVersions.php');
	    softwareVersions($output,$dbADO);
	break;	
	case 'commandsPicker';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/commands/commandsPicker.php');
	    commandsPicker($output,$dbADO);
	break;	
	case 'core';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/core.php');
	    core($output,$dbADO);
	break;	
	case 'tunerConfig';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/tunerConfig.php');
	    tunerConfig($output,$dbADO);
	break;	
	case 'deleteCriticalDevice';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/deleteCriticalDevice.php');
	    deleteCriticalDevice($output,$dbADO);
	break;
	case 'scenarioWizard';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myScenarios/scenarioWizard.php');
	    scenarioWizard($output,$dbADO);
	break;
	case 'restart';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/others/restart.php');
	    restart($output,$dbADO);
	break;
	case 'connectionWizard';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/connectionWizard.php');
	    connectionWizard($output,$dbADO);
	break;
	case 'genericSerialDevices';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/genericSerialDevices.php');
	    genericSerialDevices($output,$dbADO);
	break;
	case 'orbitersWin';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/orbiters/orbitersWin.php');
	    orbitersWin($output,$dbADO);
	break;
	case 'newRubyCode';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/infrared/newRubyCode.php');
	    newRubyCode($output,$dbADO);
	break;	
	case 'sendCommand';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/sendCommand.php');
	    sendCommand($output,$dbADO);
	break;	
	case 'sortScenarios';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myScenarios/sortScenarios.php');
	    sortScenarios($output,$dbADO);
	break;	
	case 'userPic';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/users_settings/userPic.php');
	    userPic($output,$dbADO);
	break;	
	case 'mythSettings';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/mythSettings.php');
	    mythSettings($output,$dbADO);
	break;	
	case 'errorLog';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/logs/errorLog.php');
	    errorLog($output,$dbADO);
	break;	
	case 'fullLog';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/logs/fullLog.php');
	    fullLog($output,$dbADO);
	break;		
	case 'followLog';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/logs/followLog.php');
	    followLog($output,$dbADO);
	break;	
	case 'wapSettings';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		$output->setHelpSrc('http://wiki.linuxmce.org/index.php/WAP/GPRS_Settings');
	    include_once('operations/network/wapSettings.php');
	    wapSettings($output,$dbADO);
	break;	
	case 'zones';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/zones.php');
	    zones($output,$dbADO);
	break;	
	case 'editCode';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/infrared/editCode.php');
	    editCode($output,$dbADO);
	break;	
	case 'moveFile';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/mediaBrowser/moveFile.php');
	    moveFile($output,$mediadbADO);
	break;	
	case 'restrictAccess';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/users_settings/restrictAccess.php');
	    restrictAccess($output,$dbADO);
	break;	
	case 'mediaDirectors';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/mediaDirectors.php');
	    mediaDirectors($output,$dbADO);
	break;	
	case 'editPipes';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/editPipes.php');
	    editPipes($output,$dbADO);
	break;	
	case 'editDeviceDeviceData';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/editDeviceDeviceData.php');
	    editDeviceDeviceData($output,$dbADO);
	break;	
	case 'mediaRemotes';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myScenarios/mediaRemotes.php');
	    mediaRemotes($output,$dbADO);
	break;	
	case 'setResolution';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/setResolution.php');
	    setResolution($output,$dbADO);
	break;	
	case 'setupDisklessMD';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/setupDisklessMD.php');
	    setupDisklessMD($output,$dbADO);
	break;	
	case 'editComputingApplications';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/editComputingApplications.php');
	    editComputingApplications($output,$dbADO,$mediadbADO);
	break;	
	case 'editQuickStartTemplates';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/editQuickStartTemplates.php');
	    editQuickStartTemplates($output,$dbADO);
	break;	
	case 'editQuickStartCategory';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/editQuickStartCategory.php');
	    editQuickStartCategory($output,$dbADO);
	break;	
	case 'editQuickStartTemplate';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/editQuickStartTemplate.php');
	    editQuickStartTemplate($output,$dbADO);
	break;	
	case 'addQuickStartTemplate';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/addQuickStartTemplate.php');
	    addQuickStartTemplate($output,$dbADO);
	break;	
	
	case 'sqlcvs_update';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/sqlCVS/sqlcvs_update.php');
	    sqlcvs_update($output,$dbADO);
	break;	
	case 'sqlcvs_checkin';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/sqlCVS/sqlcvs_checkin.php');
	    sqlcvs_checkin($output,$dbADO);
	break;	
	case 'sqlcvs_diff';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/sqlCVS/sqlcvs_diff.php');
	    sqlcvs_diff($output,$dbADO);
	break;	
	case 'sqlcvs_view_batch';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/sqlCVS/sqlcvs_view_batch.php');
	    sqlcvs_view_batch($output,$dbADO);
	break;	
	case 'customBackground';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/orbiters/customBackground.php');
	    customBackground($output,$dbADO);
	break;	
	case 'mediaProviders';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/mediaProviders.php');
	    mediaProviders($output,$mediadbADO,$dbADO);
	break;
	case 'bookmarks';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/bookmarks.php');
	    bookmarks($output,$mediadbADO,$dbADO);
	break;
	case 'addModel';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/infrared/addModel.php');
	    addModel($output,$dbADO,$mediadbADO);
	break;
	case 'displayCode';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/infrared/displayCode.php');
	    displayCode($output,$dbADO);
	break;
	case 'testCode';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/infrared/testCode.php');
	    testCode($output,$dbADO);
	break;
	case 'orbiterRegenInProgress';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/orbiters/orbiterRegenInProgress.php');
	    orbiterRegenInProgress($output,$dbADO);
	break;
	case 'remoteOrbiter';
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/orbiters/remoteOrbiter.php');
	    remoteOrbiter($output,$dbADO);
	break;
	case 'proxySocket':
    	$output = new Template($dbADO);
       	$output->setTemplateFileType('small');
		include_once('operations/myDevices/orbiters/proxySocket.php');
	    proxySocket($output,$dbADO);
	break;
	
	case 'phoneBook':
    	$output = new Template($dbADO);
       	$output->setTemplateFileType('large');
		@include($GLOBALS['globalConfigPath'].'telecom.inc.php');       	
		include_once('operations/telecom/phoneBook.php');
	    phoneBook($output,$telecomADO);
	break;
	case 'addPhoneNumber':
    	$output = new Template($dbADO);
       	$output->setTemplateFileType('large');
		@include($GLOBALS['globalConfigPath'].'telecom.inc.php');       	
		include_once('operations/telecom/addPhoneNumber.php');
	    addPhoneNumber($output,$telecomADO);
	break;
	case 'editPhoneNumber':
    	$output = new Template($dbADO);
       	$output->setTemplateFileType('large');
		@include($GLOBALS['globalConfigPath'].'telecom.inc.php');       	
		include_once('operations/telecom/editPhoneNumber.php');
	    editPhoneNumber($output,$telecomADO);
	break;
	case 'importContacts':
	    $output = new Template($dbADO);
	    $output->setTemplateFileType('large');
	    @include($GLOBALS['globalConfigPath'].'telecom.inc.php');
	    include_once('operations/telecom/importContacts.php');
	    importContacts($output,$telecomADO);
	break;
	case 'editContact':
    	$output = new Template($dbADO);
       	$output->setTemplateFileType('large');
		@include($GLOBALS['globalConfigPath'].'telecom.inc.php');       	
		include_once('operations/telecom/editContact.php');
	    editContact($output,$telecomADO);
	break;
	case 'help':
    	$output = new Template($dbADO);
       	$output->setTemplateFileType('large');
		include_once('operations/myDevices/help.php');
	    help($output,$dbADO);
	break;
	case 'error_message':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('error_message.php');
	    error_message($output);	    
	break;	
	case 'incomingCallsSettings':
    	$output = new Template($dbADO);
       	$output->setTemplateFileType('large');
		@include($GLOBALS['globalConfigPath'].'telecom.inc.php');       	
		include_once('operations/phones/incomingCallsSettings.php');
		@include($GLOBALS['globalConfigPath'].'asteriskDB.inc.php');		
	    incomingCallsSettings($output,$dbADO,$telecomADO,$asteriskADO);
	break;
	case 'translationItems':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('translation_tools/translationItems.php');
	    translationItems($output,$dbADO);	    
	break;	
	case 'editTranslationItem':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('translation_tools/editTranslationItem.php');
	    editTranslationItem($output,$dbADO);	    
	break;	
	case 'addTranslationItem':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('translation_tools/addTranslationItem.php');
	    addTranslationItem($output,$dbADO);	    
	break;	
	case 'voicemail':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/telecom/voicemail.php');
	    voicemail($output,$dbADO);
	break;	
	case 'keyboardLayout':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/others/keyboardLayout.php');
	    keyboardLayout($output,$dbADO);	    
	break;	
	case 'setTimezone':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/users_settings/setTimezone.php');
	    setTimezone($output,$dbADO);	    
	break;	
	case 'setLoglevels':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/users_settings/setLoglevels.php');
	    setLoglevels($output,$dbADO);            
	break;
	case 'deviceCategoryDeviceData':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/dce/deviceCategoryDeviceData.php');
	    deviceCategoryDeviceData($output,$dbADO);	    
	break;	
	case 'checkAmazon':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/checkAmazon.php');
	    checkAmazon($output,$mediadbADO,$dbADO);	    
	break;
	
	case 'searchAmazon':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/mediaBrowser/searchAmazon.php');
	    searchAmazon($output,$mediadbADO,$dbADO);	    
	break;	
	case 'grabAmazonAttributes':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/mediaBrowser/grabAmazonAttributes.php');
	    grabAmazonAttributes($output,$mediadbADO,$dbADO);	    
	break;	
	case 'checkTMDB':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/checkTMDB.php');
	    initialSetup($fileID,$mediadbADO,$dbADO,$output);	 
	break;
	case 'tmdbResults':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/checkTMDB.php');
	    tmdbResults($output,$mediadbADO,$dbADO);
		break;
	case 'tmdbSaved':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/checkTMDB.php');
	    tmdbSaveData($output,$mediadbADO,$dbADO);	 	 
	break;
	case 'checkIMDB':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/checkIMDB.php');
	    checkIMDB($output,$mediadbADO,$dbADO);	
	break;
	case 'checkTVDB':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/checkTVDB.php');
	    initialSetup($fileID,$mediadbADO,$dbADO,$output);	    
	break;		
	case 'searchtvDB':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/searchTVDB.php');
	    searchTVDB($output,$mediadbADO,$dbADO);	    
	break;
	case 'tvdbAttributes':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/tvdbAttributes.php');
	    tvdbAttributes($output,$mediadbADO,$dbADO, $fileID, $episodeInfo);	    
	break;			
	case 'tvdbSeries':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/tvdbUtils.php');
	    tvdbSeries($output,$mediadbADO,$dbADO);	    
	break;
	case 'tvdbBatch':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/tvdbbatch.php');
	    tvdbBatch($output,$mediadbADO,$dbADO);	    
	break;
	case 'tvdbBatchResult':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/tvdbbatch.php');
	    tvdbBatchResult($output,$mediadbADO,$dbADO);	    
	break;
	case 'tvdbSpecSave':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/tvdbUtils.php');
	    tvdbSpecSave($output,$mediadbADO,$dbADO);    
	break;
	case 'backup':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/others/backup.php');
	    backup($output,$dbADO);	    
	break;	
	case 'restoreChildDevices':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/restoreChildDevices.php');
	    restoreChildDevices($output,$dbADO);	    
	break;	
	case 'resyncCodes':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/resyncCodes.php');
	    resyncCodes($output,$dbADO);	    
	break;	
	case 'coverArt':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/coverArt.php');
	    coverArt($output,$mediadbADO);	    
	break;	
	case 'matchCoverArt':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/matchCoverArt.php');
	    matchCoverArt($output,$mediadbADO);	    
	break;	
	case 'unknownDevices':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/myDevices/unknownDevices.php');
	    unknownDevices($output,$dbADO);	    
	break;	
	case 'tvLineUp':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/tvLineUp.php');
	    tvLineUp($output,$dbADO,$mediadbADO);	    
	break;	
	case 'editDeviceControlledVia':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/editDeviceControlledVia.php');
	    editDeviceControlledVia($output,$dbADO);	    
	break;	
	case 'editModules':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/editModules.php');
	    editModules($output,$dbADO);	    
	break;	
	case 'editCameraRelated':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/myDevices/editCameraRelated.php');
	    editCameraRelated($output,$dbADO);	    
	break;	
	case 'raid':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/raid/raid.php');
	    raid($output,$dbADO);
	break;
	case 'raidDrives':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/raid/raidDrives.php');
	    raidDrives($output,$dbADO);
	break;	
	case 'editLongAttribute':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/editLongAttribute.php');
	    editLongAttribute($output,$mediadbADO);	    
	break;
	case 'rippingStatus':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/rippingStatus.php');
	    rippingStatus($output,$dbADO,$mediadbADO);	    
	break;	
	case 'raidFormatStatus':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/raid/raidFormatStatus.php');
	    raidFormatStatus($output,$dbADO);	    
	break;	
	case 'updateErrors':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/others/updateErrors.php');
	    updateErrors($output,$dbADO);	    
	break;	
	case 'screenSaver':
	    include_once('operations/mediaBrowser/screenSaver.php');
		screenSaver($output,$mediadbADO,$dbADO);	    
	break;
	case 'screenSaverFrameset';
		$output = new Template($dbADO);	
		$output->setTemplateFileType('homeWithFrames');
		$output->setLeftFrameSrc('index.php?section=leftScreenSaver');
		$output->setRightFrameSrc('index.php?section=mainScreenSaver&path=/home/public/data/pictures');
		$output->setBody('');		
		$output->setScriptCalendar('null');
		$output->setTitle(APPLICATION_NAME);			
  		$output->output();  		
	break;
	case 'leftScreenSaver':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/mediaBrowser/leftScreenSaver.php');
	    leftScreenSaver($output,$mediadbADO,$dbADO);	    
	break;
	case 'mainScreenSaver':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/mainScreenSaver.php');
	    mainScreenSaver($output,$mediadbADO,$dbADO);	    
	break;
        case 'photoBrowser':
             $output = new Template($dbADO);
             $output->setTemplateFileType('large');
             include_once('operations/mediaBrowser/photoBrowser.php');
             photoBrowser($output,$mediadbADO,$dbADO);
        break;
	case 'vdr':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/others/vdr.php');
	    vdr($output,$dbADO);	    
	break;		
	case 'commMethod':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/deviceTemplate/commMethod.php');
	    commMethod($output,$dbADO);	    
	break;		
	case 'upnpServer':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/others/upnpServer.php');
	    upnpServer($output,$dbADO);	    
	break;		
	case 'transmission-daemon':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
		include_once('operations/others/transmissionDaemon.php');
		transmissionDaemon($output,$dbADO,$transmissionADO);
		//transmissionDaemon($output,$dbADO,"");
	break;
	case 'transmissionLink':
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
		include_once('operations/others/transmissionLink.php');
		transmissionLink($output,$dbADO,$transmissionADO);
		//transmissionLink($output,$dbADO,"");
	break;
	case 'slimServer':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/others/slimServer.php');
	    slimServer($output,$dbADO);	    
	break;		
        case 'printingSystem':
        $output = new Template($dbADO);
        $output->setTemplateFileType('large');
        include_once('operations/others/printingSystem.php');
		printingSystem($output,$dbADO);
        break;
	case 'searchMedia':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/mediaBrowser/searchMedia.php');
	    searchMedia($output,$mediadbADO);
	break;
	case 'filePicker';
		$output = new Template($dbADO);
		$output->setTemplateFileType('small');
	    include_once('operations/mediaBrowser/filePicker.php');
	    filePicker($output,$mediadbADO,$dbADO);
	break;
	case 'installWizardList':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/packages/installWizardList.php');
	    installWizardList($output,$dbADO);
	break;
       case 'authorizeApp':
		$output= new Template($dbADO);
		$output->setTemplateFileType('small');
		include_once('../api/authorize.php');
		doAuth($output,$dbADO );
	break;
	 case 'cloudServices':
		$output= new Template($dbADO);
		$output->setTemplateFileType('large');
		echo $_GET['action'];
		
		 if($_GET['action']=="complete"){
				$svc = $_GET['service'];
			include_once('operations/cloudServices/'.$svc.'Complete.php');
				completeAuth($mediadbADO, $dbADO, $output);
		} else if($_GET['action']=="auth"){
			unset($_GET['action']);
				$svc = $_GET['service'];
			include_once('operations/cloudServices/'.$svc.'Authorize.php');
				initAuth($mediadbADO, $dbADO, $output);
		}  else {
			include_once('operations/cloudServices/serviceList.php');
		init_services($mediadbADO, $dbADO, $output);
		}		
	break;
	 case 'googleFiles':
		$output= new Template($dbADO);
		$output->setTemplateFileType('large');
		include_once('operations/cloudServices/includes/googleFiles.php');
		showGoogleDriveFiles($mediadbADO, $dbADO, $output);
		 break;
	 case 'energyGraph':

		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/datalog/energyGraph.php');
        break;
	 case 'sipConf':
		$output = new Template($dbADO);
		$output->setTemplateFileType('large');
	    include_once('operations/telecom/sipConf.php');
	    
		@include($GLOBALS['globalConfigPath'].'telecom.inc.php');    	
		@include($GLOBALS['globalConfigPath'].'asteriskDB.inc.php');	    
	    sipConf($output,$asteriskADO,$dbADO,$telecomADO);
        break;
	case '';
		$output = new Template($dbADO);	

		$output->setTemplateFileType('homeWithFrames');

		$output->setLeftFrameSrc('index.php?section=wizard&rightSection=login');
		$output->setRightFrameSrc('index.php?section=userHome');

		$output->setBody('');

		$output->setScriptCalendar('null');
		$output->setTitle(APPLICATION_NAME);
  		$output->output();
  	break;
	default:
		$output = new Template($dbADO);

		$output->setTemplateFileType('home');

		$output->setBody('Not a valid section');


		$output->setTitle(APPLICATION_NAME);
  		$output->output();

	break;
}
?>
