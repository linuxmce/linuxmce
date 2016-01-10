<?php

$db_handle = mysql_connect('localhost', 'root', '');

$db_name = "pluto_main";
//$db_name = "test_db";

if (!$db_handle)
{
        die("Cannot connect to DB server\n");
}

if (!@mysql_select_db($db_name, $db_handle))
{
        die("Cannot select DB\n");
}

$res = do_query("show tables like 'psc_%_tables'");

// list of repositories
$repo  = array();

while($row = mysql_fetch_array($res, MYSQL_NUM))
{
        list($p, $rname, $t) = split("_", $row[0], 3);
        $repo []= $rname;
}

$res = do_query("show tables");

// list of tables

$tbls = array();

while($row = mysql_fetch_array($res, MYSQL_NUM))
{
        $tbls []= $row[0];
}

foreach ($repo as $rname)
{
        $tbls_in_repo = array();
        $res = do_query("select * from psc_${rname}_tables order by Tablename");
        while($row = mysql_fetch_array($res, MYSQL_ASSOC))
        {
                $tbls_in_repo[$row['Tablename']] = array();
                $tbls_in_repo[$row['Tablename']]['last_psc_id'] = $row['last_psc_id'];
                $tbls_in_repo[$row['Tablename']]['last_psc_batch'] = $row['last_psc_batch'];
        }

		foreach ($tbls_in_repo as $k=>$v)
		{
				$real_psc_id = -1;
				$real_psc_batch = -1;

				if (in_array($k, $tbls))
				{
					$res = do_query("select max(psc_id) as max_id, max(psc_batch) as max_batch from $k");
					if ($row = mysql_fetch_array($res, MYSQL_ASSOC))
					{
						if (isset($row['max_id']))
							$real_psc_id = $row['max_id'];

						if (isset($row['max_batch']))
							$real_psc_batch = $row['max_batch'];
					}
				}
				
				$hist_psc_id = -1;
				$hist_psc_batch = -1;

				if (in_array($k."_pschist", $tbls))
				{
					$res = do_query("select max(psc_id) as max_id, max(psc_batch) as max_batch from ${k}_pschist");
					if ($row = mysql_fetch_array($res, MYSQL_ASSOC))
					{
						if (isset($row['max_id']))
							$hist_psc_id = $row['max_id'];

						if (isset($row['max_batch']))
						$hist_psc_batch = $row['max_batch'];
					}
				}

				$psc_id = max($real_psc_id, $hist_psc_id);
				$psc_batch = max($real_psc_batch, $hist_psc_batch);

				if ($tbls_in_repo[$k]['last_psc_batch'] > 5000)
				{
					$new_batch = ($psc_batch==-1?0:$psc_batch);
					echo "Correcting last_psc_batch for table '$k'  (".$tbls_in_repo[$k]['last_psc_batch']." => $new_batch)  ... " ;
					$update_query = "UPDATE psc_${rname}_tables SET last_psc_batch=$new_batch WHERE Tablename='$k'";
					do_query($update_query);
					echo "done\n" ;
				}


		}
		
}

function do_query($q)
{
        global $db_handle;

        $r = @mysql_query($q, $db_handle);

        if (!$r)
        {
                die("Query failed: $q\n");
        }
        else
                return $r;
}

?>
