<?php
//will be enabled when I (or someone) gose through this to ensure that we dont break anything
//require_once(getenv('FREEPBX_CONF') ? getenv('FREEPBX_CONF') : '/etc/freepbx.php');

class Dir{
	//agi class handler
	var $agi;
	//inital agi pased variables
	var $agivar;	
	//asterisk manager class handler
	var $ami;
	//pear::db database object handel
	var $db;
	//options of the directory that we are currently working with
	var $dir;
	//the current directory that we are working with
	var $directory;
	//string we are searching for
	var $searchstring;
	//TODO: what is this var for?
	var $vmbasedir='';
	
  //PHP4 comaptibility constructor
  function Dir(){
    $this->__construct();
  }

	//this function is run by php automaticly when the class is initalized
	function __construct(){
		$this->agi=$this->__construct_agi();
		//$this->ami=$this->__construct_ami();
		$this->db=$this->__construct_db();
		//$this->agivars=$this->__construct_inital_vars();
		$this->directory=$this->agivar['dir'];
		$this->dir=$this->__construct_dir_opts();
	}
	
	//get agi handel/inital agi vars, called by __construct()
	function __construct_agi(){
		require_once('phpagi.php');
		$agi=new AGI();
		foreach($agi->request as $key => $value){//strip agi_ prefix from keys
			if(substr($key,0,4)=='agi_'){
				$opts[substr($key,4)]=$value;
			}
		}

		foreach($opts as $key => $value){//get passed in vars
			if(substr($key,0,4)=='arg_'){
				$expld=explode('=',$value);
				$opts[$expld[0]]=$expld[1];
				unset($opts[$key]);
			}
		}
		
		array_shift($_SERVER['argv']);
		foreach($_SERVER['argv'] as $arg){
			$arg=explode('=',$arg);
			//remove leading '--'
			if(substr($arg['0'],0,2) == '--'){$arg['0']=substr($arg['0'],2);}
			$opts[$arg['0']]=isset($arg['1'])?$arg['1']:null;
		}
		$this->agivar=$opts;
		return $agi;
	}
	
	//get ami handel, called by __construct()
	function __construct_ami(){
		require_once('phpagi-asmanager.php');
		$ami=new AGI_AsteriskManager();
		return $ami;
	}	
	
	//get database handle, called by __construct()
  // TODO: hardcoded mysql, deal with sqlite...
  //
	function __construct_db(){
		require_once("DB.php");
		$dsn=array(
			'phptype'  => $this->agi_get_var('AMPDBENGINE'),
			'hostspec' => $this->agi_get_var('AMPDBHOST'),
			'database' => $this->agi_get_var('AMPDBNAME'),
			'username' => $this->agi_get_var('AMPDBUSER'),
			'password' => $this->agi_get_var('AMPDBPASS'),
		);
		$db=DB::connect($dsn);
		return $db;
	}
	
	//get options associated with the current dir
 	// TODO: handle getRow failures
	function __construct_dir_opts(){
		$sql='SELECT * FROM directory_details WHERE ID = ?';
		$row=$this->db->getRow($sql,array($this->directory),DB_FETCHMODE_ASSOC);
		//TODO: Error Checking

		//If any non-defaults (non-zero id) then lookup files
 		//
		if ($row['announcement'] || $row['repeat_recording'] || $row['invalid_recording']) {
			$sql='SELECT id, filename from recordings where id in ('.$row['announcement'].','.$row['repeat_recording'].','.$row['invalid_recording'].')';
			$res=$this->db->getAll($sql,DB_FETCHMODE_ASSOC);
			if(DB::IsError($res)) {
				dbug("FATAL: got error from getAll query",1);
				dbug($res->getDebugInfo());
			}
			$rec_file = array();
			foreach ($res as $entry) {
				//TODO: check if file exists, which means splitting on & and checkking all
				$rec_file[$entry['id']] = $entry['filename'];
			}
			unset($res);
		}
		$row['announcement'] = $row['announcement']&&isset($rec_file[$row['announcement']])?$rec_file[$row['announcement']]:'cdir-please-enter-first-three';
		$row['repeat_recording'] = $row['repeat_recording']&&isset($rec_file[$row['repeat_recording']])?$rec_file[$row['repeat_recording']]:'cdir-sorry-no-entries';
		$row['invalid_recording'] = $row['invalid_recording']&&isset($rec_file[$row['invalid_recording']])?$rec_file[$row['invalid_recording']]:'cdir-transferring-further-assistance';
		return $row;
	}

	//get a channel varibale	
	function agi_get_var($var){
		global $agi_cache;
		if (isset($agi_cache[$var])) {
			return $agi_cache[$var];
		}
		$ret=$this->agi->get_variable($var);
		if($ret['result']==1){
			$result=$ret['data'];
			$agi_cache[$var] = $result;
			return $result;
		}else{
			return '';
		}
	}

	// Return null on nothing pressed, false on error, otherwise the key
	// TODO: make it so you can pass in an array:
	//
	function getKeypress($filename, $pressables='', $timeout=2000){
		if (!is_array($filename)) {
			$filename = array($filename);
		}
		foreach ($filename as $chunk) {
			$ret=is_int($chunk)?$this->agi->say_number($chunk,$pressables):$this->agi->stream_file($chunk,$pressables);
			if(!empty($ret['result'])) {break;}
		}
		if(empty($ret['result'])){
			$ret=$this->agi->wait_for_digit($timeout);
		}
		switch ($ret['result']) {
			case 0:
				return null;
			case -1:
				return false;
			default:
				return chr($ret['result']);
		}
	}
	
	function readContact($con, $keys='#'){
		switch($con['audio']){
			case 'vm':
				$vm_dir = $this->agi->database_get('AMPUSER',$con['dial'].'/voicemail');
				$vm_dir = $vm_dir['data'];
				dbug('got directory ' . $vm_dir . ' for user ' . $con['dial']);
				//check to see if we have a greet.* and play it. otherwise, fallback to spelling the name

				if ($vm_dir && $vm_dir != 'novm') {
					if (!$this->vmbasedir) {
						$this->vmbasedir = $this->agi_get_var('ASTSPOOLDIR').'/voicemail/';
					}
					
					$dir=scandir($this->vmbasedir.$vm_dir.'/'.$con['dial']);
					foreach($dir as $file){
						dbug("looking for vm file $file using: ".basename($file),6);
						if(substr($file,0,5) == 'greet' && is_file($this->vmbasedir.$vm_dir.'/'.$con['dial'].'/'.$file)){
							$ret=$this->agi->stream_file($this->vmbasedir.$vm_dir.'/'.$con['dial'].'/greet',$keys);
							if ($ret['result']) {
								$ret['result']=chr($ret['result']);
							}
							break 2;	
						}	
					}
				}
				//fallthough if not successfull
			case 'spell':
				foreach(str_split(strtolower($con['name']),1) as $char){
					dbug('saying '.$char.' from string '.strtolower($con['name']));
					switch(true){
						case ctype_alpha($char):
							$ret = $this->agi->evaluate('SAY ALPHA '.$char.' '.$keys);
							dbug("returned from SAY ALPHA with code/result {$ret['code']}/{$ret['result']}",6);
							break;
						case ctype_digit($char):
							$ret = $this->agi->say_digits($char, $keys);
							break;
						case ctype_space($char)://pause
							$ret = $this->agi->wait_for_digit(750);
							break;					
					}
					if(trim($ret['result'])) {
						$ret['result'] = chr($ret['result']);
						break;
					}
				}
				break;
				//TODO: BUG: hardcoded to Flite, needs to either check what is there or be configurable
			case 'tts':
				$ret=$this->agi->exec('Flite "'.$con['name'].'",'.$keys);
				if ($ret['result']){$ret['result']=chr($ret['result']);}
				break;
			default:
				if(is_numeric($con['audio'])){
					$sql='SELECT filename from recordings where id = ?';
					$rec=$this->db->getOne($sql, array($con['audio']));
					dbug("got record id: {$con['audio']} file(s): $rec");
					if($rec){
						$rec=explode('&',$rec);
						foreach($rec as $r){
							$ret=$this->agi->stream_file($r,$keys);
							if(trim($ret['result'])){$ret['result']=chr($ret['result']);break;}
						}
					} else {
						//TODO: handle error
						dbug("ERROR: unknown/undefined sound file");
					}
				}
				break; 
			}
			return $ret;
		}
	
	function search($key,$count=0){
		if($key == ''){return false;}//requre search term

		if(strstr($key,'0') !== false) {
			debug("user pressed 0 - bailing out");
			$this->bail();
		}

		//the regex in the query will match the searchstring at the beging of the string or after a space
		$num= array('1','2','3','4','5','6','7','8','9','0','#');
		$alph=array("[ \s@,-\!/+=\.']",'[abcABC]','[defDEF]','[ghiGHI]','[jklJKL]','[mnoMNO]','[pqrsPQRS]','[tuvTUV]','[wxyzWXYZ]','','');
		$this->searchstring=$this->db->escapeSimple(str_replace($num,$alph,$key));
		dbug("search string for regex: {$this->searchstring}");

		//TODO: check db results for errors and fail gracefully

		$vtable = '(SELECT DISTINCT a.audio, IF(a.name != "",a.name,b.name) name, IF(a.dial != "",a.dial,b.extension) dial FROM directory_entries a LEFT JOIN users b ON a.foreign_id = b.extension WHERE id = "'.$this->directory.'") v';
		if($count==1){
			$sql="SELECT COUNT(*) FROM $vtable WHERE name REGEXP \"(^| ){$this->searchstring}\"";
			$res=$this->db->getOne($sql);
			if(DB::IsError($res)) {
				dbug("FATAL: got error from COUNT(*) query");
				dbug($res->getDebugInfo());
			}
			dbug("Found $res possible matches from $key");
		}else{
			$sql="SELECT * FROM $vtable WHERE name REGEXP \"(^| ){$this->searchstring}\"";
			$res=$this->db->getAll($sql,DB_FETCHMODE_ASSOC);
			if(DB::IsError($res)) {
				dbug("FATAL: got error from getAll query");
				dbug($res->getDebugInfo());
			} else {
				dbug("Found the following matches:");
				foreach ($res as $ent) {
					dbug("name: {$ent['name']}, audio: {$ent['audio']}, dial: {$ent['dial']}");
				}
			}
		}
		return $res;
	}

	function bail() {
		//do something if we are exiting due to to many tries
		//
		dbug("User pressed zero, passing back recording of {$this->dir['invalid_recording']}");
		$this->agi->set_variable('DIR_INVALID_RECORDING',$this->dir['invalid_recording']);
		if($this->agi_get_var('IVR_CONTEXT')){
			$this->agi->set_extension('retivr');
		}else{
	 		$dest = explode(',',$this->dir['invalid_destination']);
			$this->agi->set_variable('DIR_INVALID_CONTEXT',$dest['0']);
			$this->agi->set_variable('DIR_INVALID_EXTEN',$dest['1']);
			$this->agi->set_variable('DIR_INVALID_PRI',$dest['2']);
			$this->agi->set_extension('invalid');
		}
		$this->agi->set_priority('1');
		exit;
	}
}

/* PHP4 Compatibility functions */

if (!function_exists('file_put_contents')) {
  function file_put_contents($filename, $data, $flags='', $context=null) {
    $option = $flags == FILE_APPEND ? 'a' : 'w';
    if ($context !== null) {
      $fd = @fopen($filename, $option);
    } else {
      $fd = @fopen($filename, $option, false, $context);
    }
    if (!$fd) {
      return false;
    }
    if (is_array($data)) {
      $data = implode('',$data);
    } else if (is_object($data)) {
      $data = print_r($data,true);
    }
    $bytes = fwrite($fd,$data);
    fclose($fd);

    return $bytes;
  }
}
if (!function_exists('scandir')) {
	function scandir($path,$sort=0) {
		$fh = opendir($path);
		$list = array();
		while(false !== ($filename = readdir($fh))) {
			$list[] = $filename;
		}
		closedir($fh);
    /* Not really needed here
		if ($sort) {
			sort($list);
		} else {
			rsort($list);
		}
    */
		return $list;
	}
}

// non-utf8 version for php4
if(!function_exists('str_split')) {
  function str_split($string, $split_length = 1) {
    $array = explode("\r\n", chunk_split($string, $split_length));
    array_pop($array);
    return $array;
  }
}

?>
