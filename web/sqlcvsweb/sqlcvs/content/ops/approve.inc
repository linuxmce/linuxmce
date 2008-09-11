<h1><? echo _("Batch approval") ?></h1>
<h3><? echo _("Repository").": ".$_POST['repository'].", "._("Batch ID").": ".$_POST['batchid']; ?></h3>
<div class="pre">
<pre>
<?
$scall = '/usr/bin/sudo /usr/sbin/chroot /opt/pluto/chroot sqlCVS -h "127.0.0.1" -u "root" -p "" -D "pluto_main" -P "3306" -r "'.$_POST['repository'].'" -t "" -U "'.$_POST['credentials'].'" -n -b "'.$_POST['batchid'].'" approve';
# print $scall."\n";
system($scall);

# system("/opt/pluto/viewbatch.sh constants 21");

?>
</pre>
</div>
