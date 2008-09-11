<h1><? echo _("Server logfile"). ", ". _("reversed, 3000 lines"); ?></h1>
<pre>
<? system('tac /opt/pluto/nohup.out | head -3000'); ?>
</pre>
