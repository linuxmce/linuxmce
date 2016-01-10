<?php
require('include/config/config.inc.php');
require('include/utils.inc.php');
Header( "Content-type: text/xml");
echo "<?xml version='1.0'?>";
?>

<CiscoIPPhoneMenu>
  <Title>SERVICES</Title>
  <Prompt>Make Your Selection...</Prompt>
  <MenuItem>
    <Name>Orbiter</Name>
    <URL>http://<?php getCoreIP($dbADO) ?>/lmce-admin/index.php?section=proxySocket&amp;command=XML</URL>
  </MenuItem>
</CiscoIPPhoneMenu>
