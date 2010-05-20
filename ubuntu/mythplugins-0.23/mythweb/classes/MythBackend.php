<?php
/**
 *
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/classes/MythBackend.php $
 * @date        $Date: 2010-03-01 14:20:56 -0800 (Mon, 01 Mar 2010) $
 * @version     $Revision: 23637 $
 * @author      $Author: cpinkham $
 * @license     GPL
 *
 * @package     MythTV
 *
/**/

class MythBackend {

// MYTH_PROTO_VERSION is defined in libmyth in mythtv/libs/libmyth/mythcontext.h
// and should be the current MythTV protocol version.
    static $protocol_version        = 56;

// The character string used by the backend to separate records
    static $backend_separator       = '[]:[]';

// NUMPROGRAMLINES is defined in mythtv/libs/libmythtv/programinfo.h and is
// the number of items in a ProgramInfo QStringList group used by
// ProgramInfo::ToSringList and ProgramInfo::FromStringList.
    static $program_line_number     = 47;

    private $fp                     = null;
    private $connected              = false;
    private $host                   = '127.0.0.1';
    private $ip                     = '127.0.0.1';
    private $port                   = null;
    private $port_http              = null;

    static function find($host = null, $port = null) {
        static $Backends = array();

    // Looking for the master backend?
        if (is_null($host)) {
            $host = setting('MasterServerIP');
            $port = setting('MasterServerPort');
            if (!$host || !$port)
                trigger_error("MasterServerIP or MasterServerPort not found! You may"
                            ."need to check your settings.php file or re-run mythtv-setup",
                            FATAL);
        }

        if (!isset($Backend[$host][$port]))
            $Backend[$host][$port] = new MythBackend($host, $port);
        return $Backend[$host][$port];
    }

    function __construct($host, $port = null) {
        $this->host         = $host;
        $this->ip           = _or(setting('BackendServerIP', $this->host), $host);
        $this->port         = _or($port, _or(setting('BackendServerPort', $this->host), 6543));
        $this->port_http    = _or(setting('BackendStatusPort', $this->host), _or(setting('BackendStatusPort'), 6544));
    }

    function __destruct() {
        $this->disconnect();
    }

    private function connect() {
        if ($this->fp)
            return;
        $this->fp = @fsockopen($this->ip, $this->port, $errno, $errstr, 25);
        if (!$this->fp)
            custom_error("Unable to connect to the master backend at {$this->ip}:{$this->port}".(($this->host == $this->ip)?'':" (hostname: {$this->host})").".\nIs it running?");
        socket_set_timeout($this->fp, 30);
        $this->checkProtocolVersion();
        $this->announce();
    }

    private function disconnect() {
        if (!$this->fp)
            return;
        $this->sendCommand('DONE');
        fclose($this->fp);
    }

    private function checkProtocolVersion() {
    // Allow overriding this check
        if ($_SERVER['ignore_proto'] == true )
            return true;

        if (   time() - $_SESSION['backend'][$this->host]['proto_version']['last_check_time'] < 60*60*2
            && $_SESSION['backend'][$this->host]['proto_version']['last_check_version'] == MythBackend::$protocol_version )
            return true;

        $response = $this->sendCommand('MYTH_PROTO_VERSION '.MythBackend::$protocol_version);
        $_SESSION['backend'][$this->host]['proto_version']['last_check_version'] = @$response[1];

        if ($response[0] == 'ACCEPT') {
            $_SESSION['backend'][$this->host]['proto_version']['last_check_time'] = time();
            return true;
        }

        if ($response[0] == 'REJECT')
            trigger_error("Incompatible protocol version (mythweb=" . MythBackend::$protocol_version . ", backend=" . @$response[1] . ")");
        else
            trigger_error("Unexpected response to MYTH_PROTO_VERSION '".MythBackend::$protocol_version."': ".print_r($response, true));
        return false;
    }

    private function announce() {
        $response = $this->sendCommand('ANN Monitor '.hostname.' 2' );
        if ($response == 'OK')
            return true;
        return false;
    }

    public function setTimezone() {
        if (!is_string($_SESSION['backend']['timezone']['value']) || time() - $_SESSION['backend']['timezone']['last_check_time'] > 60*60*24) {
            $response = $this->sendCommand('QUERY_TIME_ZONE');
            $timezone = str_replace(' ', '_', $response[0]);
            $_SESSION['backend']['timezone']['value']           = $timezone;
            $_SESSION['backend']['timezone']['last_check_time'] = time();
        }

        if (!@date_default_timezone_set($_SESSION['backend']['timezone']['value'])) {
            $attempted_value = $_SESSION['backend']['timezone']['value'];
            unset($_SESSION['backend']['timezone']);
            trigger_error('Failed to set php timezone to '.$attempted_value.(is_array($response) ? ' Response from backend was '.print_r($response, true) : ''));
        }
    }

    public function sendCommand($command = null) {
        $this->connect();
    // The format should be <length + whitespace to 8 total bytes><data>
        if (is_array($command))
            $command = implode(MythBackend::$backend_separator, $command);
        $command = strlen($command) . str_repeat(' ', 8 - strlen(strlen($command))) . $command;
        fputs($this->fp, $command);
        return $this->receiveData();
    }

    public function receiveData($timeout = 30) {
        $this->connect();
        stream_set_timeout($this->fp, $timeout);

    // Read the response header to find out how much data we'll be grabbing
        $length = rtrim(fread($this->fp, 8));

    // Read and return any data that was returned
        $response = '';
        while ($length > 0) {
            $data = fread($this->fp, min(8192, $length));
            if (strlen($data) < 1)
                break; // EOF
            $response .= $data;
            $length -= strlen($data);
        }
        $response = explode(MythBackend::$backend_separator, $response);
        if (count($response) == 1)
            return $response[0];
        if (count($response) == 0)
            return false;
        return $response;
    }

    public function listenForEvent($event, $timeout = 120) {
        $endtime = time() + $timeout;
        do {
            $response = $this->receiveData();
        } while ($response[1] != $event && $endtime < time());
        if ($response[1] == $event)
            return $response;
        return false;
    }

    public function queryProgramRows($query = null, $offset = 1) {
        $records = $this->sendCommand($query);
    // Parse the records, starting at the offset point
        $row = 0;
        $col = 0;
        $count = count($records);
        for($i = $offset; $i < $count; $i++) {
            $rows[$row][$col] = $records[$i];
        // Every $NUMPROGRAMLINES fields (0 through ($NUMPROGRAMLINES-1)) means
        // a new row.  Please note that this changes between myth versions
            if ($col == (MythBackend::$program_line_number - 1)) {
                $col = 0;
                $row++;
            }
        // Otherwise, just increment the column
            else
                $col++;
        }
    // Lastly, grab the offset data (if there is any)
        for ($i=0; $i < $offset; $i++) {
            $rows['offset'][$i] = $recs[$i];
        }
    // Return the data
        return $rows;
    }

/**
 * Tell the backend to reschedule a particular record entry.  If the change
 * isn't specific to a single record entry (e.g. channel or record type
 * priorities), then use 0.  I don't think mythweb should need it, but if you
 * need to indicate every record rule is affected, then use -1.
/**/
    public function rescheduleRecording($recordid = -1) {
        $this->sendCommand('RESCHEDULE_RECORDINGS '.$recordid);
        if ($this->listenForEvent('SCHEDULE_CHANGE'))
            return true;
        return false;
    }

    public function httpRequest($path, $args = array()) {
        $url = "http://{$this->ip}:{$this->port_http}/Myth/{$path}?";
        foreach ($args as $key => $value)
            $url .= $key.'='.urlencode($value).'&';
        return @file_get_contents($url);
    }
}
