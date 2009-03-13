------------------------------------------------------
Cisco XML Directory Patch for Pluto/LinuxMCE Web Admin
------------------------------------------------------

== Synopsis ==

This set of files adds a working Directory for your Cisco IP phone. It has been tested on my Cisco 7970 phone, but does not use any additional features, so it should work on any of the other Cisco IP phones that support XML services, such as the Cisco 7960. 

It adds the following Features:

* Directory button now works!
* Works with Contacts from the Pluto phone directory
* Paginates them based on # of entries in phone book
* Shows the phone type.

Nice and simple.

== Installing this Patch ==

* Unpack the files to the pluto-admin directory. (/var/www/pluto-admin/)
* edit the file /tftpboot/SEPDefault7970.cnf.xml (it just needs to be the default xml for your cisco phone, adjust as needed.)
** Find the <directoryURL></directoryURL> tags, and place '''http://192.168.80.1/pluto-admin/directory.php''' between them.
* Reboot the phone by pressing the settings button, and pressing the following button sequence '''**#**'''

Once the phone boots, try the directory by pressing the Directory button!

If there are any problems, let me know --~~~~

Thom Cherryhomes - UNIT-E
thom.cherryhomes at gmail.com


