#!/bin/bash

#DB

#Create config setup file
Create_Config='
#!/bin/bash

#. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

#. /usr/pluto/bin/Network_Parameters.sh
#. /usr/pluto/bin/LockUtils.sh

#create the config file
Create_Config() {
program=$1
echo $2
if [ $2 = "global" ]; then
        conf_file=$program.conf
else
        if [ $program = "dansguardian" ]; then
                conf_file=$program$2.conf
        else
                conf_file=$2.conf
        fi
fi
if [ $program = "dansguardian" ]; then
        sectie="webfilter"
elif [ $program = "squid" ]; then
        sectie="proxy"
fi
echo /etc/$program/$conf_file
if ! BlacklistConfFiles /etc/$program/$conf_file; then
echo "file not blacklisted"
echo > /etc/$program/$conf_file
SQL="SELECT var, val FROM webfilter_proxy WHERE conf='$sectie' AND conf_type='$conf'";
R=$(RunSQL "$SQL")

for webfilter_proxy in $R; do

        echo $(Field "1" "$webfilter_proxy") = $(Field "2" "$webfilter_proxy") >> /etc/$program/$conf_file

done

fi
}

Pass_vars_to_Create_Config() {
program=$1
conf=$2
declare -p conf | grep -q '^declare \-a' && conf_array=yes || conf_array=no
if [ $conf_array == no ]; then
        Create_Config $program $conf
else
        for i in "${conf[@]}"
        do
                Create_Config $program $i
        done
fi
}

if [[ -z $1 ]]; then
echo "none"
echo "none allconfig files"
program[0]="webfilter"
program[1]="proxy"
else
program=$1
fi
declare -p program | grep -q '^declare \-a' && prog_array=yes || prog_array=no
if [ $prog_array = "no" ]; then
        if [ $program = "webfilter" ]; then
                if [[ -z $2 ]]; then
                        SQL='SELECT val FROM webfilter_proxy WHERE conf="webfilter" AND conf_type="global" AND var="filtergroups"';
                        group=$(RunSQL "$SQL")
                        conf[0]="global"
                        conf[1]="f0"
                        confincr=2
                        for ((i = 1; i <= $group; i++)); do
                                conf[$confincr]=f$i
                                ((confincr++))
                        done

                        echo "array config webfilter"
                else
                        conf=$2
                        echo $2
                fi
                program="dansguardian"
        elif [ $program = "proxy" ]; then
                if [[ -z $2 ]]; then
                        #als meerdere config files worden moet hier een array gecreerd worden
                        conf="global"
                else
                        conf=$2
                        echo $2
                fi

                program="squid"
        fi
        Pass_vars_to_Create_Config $program $conf
elif [ $prog_array = "yes" ]; then
        for i in "${program[@]}"
         do
                if [ $i = "webfilter" ]; then
                        if [[ -z $2 ]]; then
                                SQL=\'SELECT val FROM webfilter_proxy WHERE conf="webfilter" AND conf_type="global" AND var="filtergroups"\';
                                group=$(RunSQL "$SQL")
                                conf[0]="global"
                                conf[1]="f0"
                                confincr=2
                                for ((i = 1; i <= $group; i++)); do
                                        conf[$confincr]=f$i
                                        ((confincr++))
                                done

                                echo "array config webfilter"
                        else
                                conf=$2
                                echo $2
                        fi
                        program="dansguardian"
                elif [ $i = "proxy" ]; then
                        unset conf
                        if [[ -z $2 ]]; then
                                #als meerdere config files worden moet hier een array gecreerd worden
                                conf="global"
                         else
                                conf=$2
                                echo $2
                        fi
                        program="squid"
                fi
                Pass_vars_to_Create_Config $program $conf
        done
fi
'

echo $Create_Config >/usr/pluto/bin/webfilter_proxy_conf.sh

Alias="
		Alias /dgblocknotice    /var/www/lmce-admin/operations/webfilter_proxy/webfilter/blocknotice.php
         <Directory /var/www/lmce-admin/operations/webfilter_proxy/webfilter>
        php_admin_flag safe_mode off
        </Directory>
"

exit 0

