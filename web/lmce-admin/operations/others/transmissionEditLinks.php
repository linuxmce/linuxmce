<?php
function transmissionEdit($transmissionADO, $operation, $ID){

	if ($operation!=""){
		$queryLinks='SELECT * FROM Torrent_Links WHERE Link_ID='.$ID.';';
		$LinkInfo=$transmissionADO->Execute($queryLinks);

		if ($LinkInfo->RecordCount()>0){
			$LinkInfo->MoveFirst();
			$Link=$LinkInfo->fetchRow();

			switch ($operation){
				case "delete":
					$queryLinks='DELETE FROM Torrent_Links WHERE Link_ID=?;';
					$DeleteLink=$transmissionADO->Execute($queryLinks, $ID);

					if ($Link['Link_Location']!="" && $Link['Link_Name']!=""){
						exec('sudo -u root /usr/pluto/bin/transmission-settings.sh deleteLink "'.$Link['Link_Location'].DIRECTORY_SEPARATOR.$Link['Link_Name'].'"');
					};
				break;

				case "repair":
					exec('sudo -u root /usr/pluto/bin/transmission-settings.sh createLink "'.$Link['Torrent_Location'].'" "'.$Link['Link_Location'].DIRECTORY_SEPARATOR.$Link['Link_Name'].'"');
				break;

				default:
					//Do Nothing, can't be sure what's been pressed!
			};
		};
	};
}
?>
