  <?
  // include our OAuth2 Server object
  //http://ballfam.dyndns-home.com/lmce-admin/index.php?section=authorizeApp&state=google
  //http://ballfam.dyndns-home.com/api/token.php

  function doAuth($output,$dbADO){
  
  $urlArray;
foreach($_GET as $key => $value) {
   $urlArray[] =  $key."=".$value;  
}
$urlString = implode("&", $urlArray);
var_dump($urlString);

  require_once __DIR__.'/server.php';
  $request = OAuth2\Request::createFromGlobals();
  $response = new OAuth2\Response();

  // validate the authorize request
  if (!$server->validateAuthorizeRequest($request, $response)) {
      $response->send();
      die;
  }
  // display an authorization form
  if (empty($_POST)) {
  
    exit('

  <h2>Welcome to the Linuxmce API! </h2>

  Please login to authorize this<br>
  app using you Linuxmce web admin credentials

  <div id="login_form">
      <form name="f1" method="post" action="index.php?section=authorizeApp'.$urlString.'" id="login_form">    
      <input type="hidden" name="state" value="'.$_GET['state'].' id="state" >
	<input type="submit" name="login" value="Authorize" style="font-size:18px;" />
      </form> 
  </div>');
  }

  // print the authorization code if the user has authorized your client
  $is_authorized = true; //($_POST['authorized'] === 'yes');
  $server->handleAuthorizeRequest($request, $response, $is_authorized, $_SESSION['userID']);
  if ($is_authorized) {
    // this is only here so that you get to see your code in the cURL request. Otherwise, we'd redirect back to the client
    $code = substr($response->getHttpHeader('Location'), strpos($response->getHttpHeader('Location'), 'code=')+5, 40);
    echo("SUCCESS! Authorization Code: $code");
  }
  $response->send();   

  $output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
  $output->setPageTitle($TEXT_BOOKMARKS_CONST);
  $output->setNavigationMenu(array($TEXT_BOOKMARKS_CONST=>'index.php?section=bookmarks&type='.$type));
	  $output->setScriptCalendar('null');
		  $output->setBody($out);
	  $output->setTitle("Authorize External Application");
	  $output->output();
  }

  ?>