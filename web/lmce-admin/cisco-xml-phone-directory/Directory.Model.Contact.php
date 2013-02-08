<?php 

require_once 'DB.php';

// select c.Name, t.Description, p.DialAs FROM Contact c, PhoneType t, PhoneNumber p WHERE c.Name LIKE '%' AND p.FK_Contact = c.PK_Contact AND p.FK_PhoneType = PK_PhoneType ORDER BY c.Name;

/**
 * Directory.ContactModel.php - Model allowing interaction with
 * the Pluto Phone Book
 * @author	Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * @package	1.0
 * @version	1.0
 ***************************************************************/
class Contacts {

	/**
	 * $this->db - the PEAR::DB database handle.
	 *
	 * @var	object	$db
	 */
	var $db;

	/**
	 * numEntriesMatching - Return # of entries matching a
	 * string.
	 *
	 * @param	string	$searchString The Search Query
	 *
	 * @return	integer	# of entries that match.
	 */
	function numEntriesMatching($searchString) {
	
		return $this->db->getOne("SELECT COUNT(*) FROM Contacts WHERE Name LIKE '%$searchString'");
	
	}

	/**
	 * dialMatchingRange - Return Numbers Matching a Range
	 * 
	 * @param	string	$range	Regexplet to match
	 * 
	 * @return	array	array of contact objects
	 */
	function dialMatchingRange($range) {
	
		return $this->db->getAll("select c.Name, t.Description, p.DialAs FROM Contact c, PhoneType t, PhoneNumber p WHERE c.Name REGEXP '^[$range]' AND p.FK_Contact = c.PK_Contact AND p.FK_PhoneType = PK_PhoneType ORDER BY c.Name",array(),DB_FETCHMODE_OBJECT);	
	
	}

	/**
	 * Contacts - The Constructor.
	 *
	 * @param	string	$host	DB Host 'localhost'
	 *
	 * @param	string	$user	DB User	'root'
	 *
	 * @param	string	$pass	DB Password ''
	 *
	 * @return	mixed	A Contact object, or Pear::Error
	 */
	function Contacts($host = 'localhost', $user = 'root',
			 $pass = '') {	

		$connect_res = $this->_connect($host,$user,$pass);

		if (PEAR::isError($connect_res)) {
			return $connect_res;
		} else {
			// fall through...
		}
	
		$this->db = $connect_res;

	}

	/**
	 * _connect() - Private method to connect to database. 
	 * returns the database handle.
	 * 
	 * @param	string	$host	Database Host.
	 * 
	 * @param	string	$user	Database User.
	 * 
	 * @param	string	$pass	Database Password.
	 * 
	 * @return	mixed	PEAR::DB handle or PEAR:Error.  
	 */
	function _connect($host,$user,$pass) {
	
		$dsn = array(
			'phptype' 	=> 'mysql',
			'username'	=> $user,
			'password'	=> $pass,
			'hostspec'	=> $host,
			'database'	=> 'pluto_telecom'
		);
	
		$options = array(
			'debug'		=> 2,
			'portability'	=> DB_PORTABILITY_ALL
		);

		return DB::connect($dsn,$options);

	}
}

?>
