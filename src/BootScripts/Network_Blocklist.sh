#!/bin/bash
# written by benjamin cathey on 2008.07.30
#
# used to get dshield blocklist and implement using apf, csf or iptables

#program selection - please choose only one

#ban="csf"
#ban="iptables"
#ban="apf"
ban="lmce"

#logfile

log="/var/log/pluto/blocklistlog"

touch $log

#program paths

apf="/etc/apf/apf"
csf="/etc/csf/csf.pl"
iptables="/sbin/iptables"

#contact email

email="name@domain.com"

echo "`date` - DShield Update begun on `hostname`" >> $log
echo "------------------------------------------------------------------------------------------" >> $log
echo "" >> $log

# get list
echo "`date` - downloading new blocklist" >> $log
echo "" >> $log
wget -O /etc/block.txt http://feeds.dshield.org/block.txt

# parse new list
echo "`date` - adding new blocks" >> $log
echo "------------------------------------------------------------------------------------------" >> $log
echo "" >> $log
blocklist=$(cat /etc/block.txt |awk '/^[0-9]/'|awk '{print $1"/"$3}'|sort -n)
if [ $ban == "iptables" ]; then
	$iptables -F BLOCKLIST
	$iptables -X BLOCKLIST
	$iptables -D INPUT -j BLOCKLIST
	$iptables -N BLOCKLIST
	$iptables -I INPUT -j BLOCKLIST
elif [ $ban == "lmce" ]; then
	/usr/pluto/bin/Network_Firewall.sh -L BLOCKLIST -A Flush
	/usr/pluto/bin/Network_Firewall.sh -L Rule -H local -A addchain -C BLOCKLIST -P chain-ipv4
fi
for ip in $blocklist
do
    if [ $ban == "csf" ]; then
      echo "`date` - running 'csf -d $ip'" >> $log
      $csf -d $ip
    elif [ $ban == "iptables" ]; then
      echo "`date` running 'iptables -I BLOCKLIST -s $ip -j DROP'" >> $log
      $iptables -I BLOCKLIST -s $ip -j DROP
    elif [ $ban == "apf" ]; then
      echo "`date` - running 'apf -d $ip {DSHIELD_`date +%Y.%m.%d`}'" >> $log
      $apf -d $ip {DSHIELD_`date +%Y.%m.%d`}
   elif [ $ban == "lmce" ]; then
	
	echo "`date` - running 'lmce -d $ip {DSHIELD_`date +%Y.%m.%d`}'" >> $log
	/usr/pluto/bin/Network_Firewall.sh -L BLOCKLIST -H local -A add -P all-ipv4 -C BLOCKLIST -R $ip -T DROP
    fi
done

cat $log 
#|mail -s "DShield updated on `date +%Y.%m.%d` on `hostname`" $email
rm -f $log
