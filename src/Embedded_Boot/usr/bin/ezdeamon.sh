#!/bin/ash

log_file="/tmp/log.ezdaemon"
wls_bk="/tmp/wireless.orig"
wls_save="/tmp/wireless.save"

#traps
#1,6,15 - import wireless.bak and exit
#6 - err or timeout in received string
#0 - normal exit
trap "echo You\'re trying to Control-C me" 2
#trap "uci import wireless -f $wls_bk; uci commit; echo "`date` - Trap signal 1, 6 or 15 - Importing back conf file"; exit" 6
#trap "uci commit; sync; echo "`date` - Trap signal 0 - Commiting changes"; exit" 0


echo "`date` - ===EzDaemon Started===." >> $log_file
###
read -t 120 get_status
###
if [[ "$get_status" != "GET_STATUS" ]]; then 
    echo "`date` - Timeout or incorrect get status ($get_status)" >> $log_file
    exit 6
fi

wls_off="`grep "disabled" $wls_bk | tr -d "'" | cut -d " " -f 3`"
echo "`date` - wireless.wl0.disabled=$wls_off" >> $log_file

wls_mac="`nvram show 2>/dev/null | grep -i "wl0_hwaddr" | cut -d "=" -f 2`"
echo "`date` - wl0_hwaddr=$wls_mac" >> $log_file

wls_encr_type="`grep -i "encryption" $wls_bk | tr -d "'" | cut -d " " -f 3`"
if [[ "$wls_encr_type" == "none" ]]; then
    echo "`date` - No wireless encryption set" >> $log_file
    has_key="0"
else
    echo "`date` - Wireless encryption set to: $wls_encr_type" >> $log_file
    has_key="1"
fi
###
echo "GET_STATUS $wls_mac $has_key" | tee $log_file
###
if [[ "$has_key" == "1" ]]; then
#####################################################
###       Wireless is already configurated        ###
#####################################################
    echo "`date` - (1) Wireless has_key = $has_key" >> $log_file
    ###
    read -t 30 get_encryption wls_pass
    ###
    if [[ "$get_encryption" != "GET_ENCRYPTION" ]] || [[ -z $wls_pass ]]; then
        echo "`date` - Timeout or incorrect get encryption ($get_encryption - $wls_pass)" >> $log_file
        exit 6
    fi
    
    wls_key="`grep -i "key" $wls_bk | tr -d "'" | cut -d " " -f 3`"
    wls_ssid="`grep -i "ssid" $wls_bk | tr -d "'" | cut -d " " -f 3`"
    wls_encr_type="`grep -i "ssid" $wls_bk | tr -d "'" | cut -d " " -f 3`"
    wls_key="`grep -i "key" $wls_bk | tr -d "'" | cut -d " " -f 3`"
    #if [[ "$wls_key" == "1" ]]; then
    if [[ "$wls_encr_type" == "wep" ]]; then
        #encry type = wep
        wep_wls_key1="`grep -i "key1" $wls_bk | tr -d "'" | cut -d " " -f 3`"
        if [[ $(lenght $wep_wls_key1) eq 10 ]]; then
            #wep 40
            wls_encr_type="wep40"
            wep_wls_key2="`grep -i "key2" $wls_bk | tr -d "'" | cut -d " " -f 3`"
            wep_wls_key3="`grep -i "key2" $wls_bk | tr -d "'" | cut -d " " -f 3`"
            wep_wls_key4="`grep -i "key3" $wls_bk | tr -d "'" | cut -d " " -f 3`"
            wls_key="$wep_wls_key1|$wep_wls_key2|$wep_wls_key3|$wep_wls_key4"
        else
            #wep128
            wls_encr_type="wep128"
            wls_key="$wep_wls_key1"
        fi   
    fi
    #encrypt
    echo "$wls_ssid $wls_encr_type $wls_key" > $wls_save
    send_encryption=$(/usr/bin/xorcrypt $wls_save $wls_pass | uuencode -)
    ###
    echo "`date` - GET_ENCRYPTION $wls_ssid $wls_encr_type $wls_key" >> $log_file
    echo "GET_ENCRYPTION $send_encryption" | tee $log_file
    ###
    read -t 30 exit_ezsetup
    ###
    if [[ "$exit_ezsetup" != "EXIT_EZSETUP" ]]; then
        echo "`date` - Timeout or incorrect exit_ezsetup ($exit_ezsetup)" >> $log_file
        exit 6
    else
        echo "EXIT_EZSETUP" | tee $log_file
        #import & commit orig settings
        uci import wireless -f $wls_bk
        uci commit
        sync
        exit 1
    fi
else
#####################################################
###         Wireless is not configurated          ###
#####################################################
    #has_key==0
    echo "`date` - (0) Wireless has_key = $has_key" >> $log_file
    ###
    read -t 30 setup_ecnryption wls_encr_type wls_pass wls_ssid pc_mac
    ###
    if [[ "$setup_encryption" != "SETUP_ENCRYPTION" || -z "$wls_encr_type" || -z "$wls_pass" || -z "$wls_ssid" || -z "$pc_mac" ]]; then
        echo "`date` - Timeout or incorrect setup encryption ($setup_encryption - $wls_encr_type - $wls_pass - $wls_ssid - $pc_mac)" >> $log_file
        exit 6
    fi
    echo "`date` - Received SETUP_ENCRYPTION: $setup_encryption - $wls_encr_type - $wls_pass - $wls_ssid - $pc_mac " >> $log_file
    
    Cfg=$(uci show wireless|grep device=wl0|cut -d. -f2)
    #enable wifi    
    uci set wireless.wl0.disabled=0
    #broadcast ssid
    uci set wireless.$Cfg.hidden=0
    #set ssid
    uci set wireless.$Cfg.ssid="$wls_ssid"
    #set encryption & save it
    case "$wls_encr_type" in
        wep40)
                #gen 40bit key
                textkeys=$(wepkeygen "$wls_pass" | sed s/':'//g)
                keycount=1
                for curkey in $textkeys; do
                     case $keycount in
                        1) wep40_wls_key1=$curkey;;
                        2) wep40_wls_key2=$curkey;;
                        3) wep40_wls_key3=$curkey;;
                        4) wep40_wls_key4=$curkey
                        break;;
                     esac
                let "keycount+=1"
                done
                
                uci set wireless.$Cfg.encryption=wep
                uci set wireless.$Cfg.key=1
                uci set wireless.$Cfg.key1=$wep40_wls_key1
                uci set wireless.$Cfg.key2=$wep40_wls_key2
                uci set wireless.$Cfg.key3=$wep40_wls_key3
                uci set wireless.$Cfg.key4=$wep40_wls_key4

                #echo "$wls_ssid $wls_encr_type $wep40_wls_key1|$wep40_wls_key2|$wep40_wls_key3|$wep40_wls_key4" | tee $log_file > $wls_save
                echo "SETUP_ENCRYPTION $wep40_wls_key1|$wep40_wls_key2|$wep40_wls_key3|$wep40_wls_key4" | tee $log_file 
            ;;
        wep128)
                #gen 128bit key
                $wep128_wls_key1=$(/usr/bin/wepkeygen -s "$wls_pass" | /
                        awk 'BEGIN { count=0 }; { total[count]=$1, count+=1; } END {print total[0] ":" total[1] ":" total[2] ":" total[3]}' | /
                        cut -d ':' -f 0-13 | sed s/':'//g)
                uci set wireless.$Cfg.encryption=wep
                uci set wireless.$Cfg.key=1
                uci set wireless.$Cfg.key1=$wep128_wls_key1

                #echo "$wls_ssid $wls_encr_type $wep128_wls_key1" | tee $log_file > $wls_save
                echo "SETUP_ENCRYPTION $wep128_wls_key1" | tee $log_file
            ;;
        wpa)
                uci set wireless.$Cfg.encryption=psk
                uci set wireless.$Cfg.key=$wls_pass

                #echo "$wls_ssid $wls_encr_type $wls_pass" | tee $log_file > $wls_save
                echo "SETUP_ENCRYPTION $wls_pass" | tee $log_file
            ;;
        wpa2)
                uci set wireless.$Cfg.encryption=psk2
                uci set wireless.$Cfg.key=$wls_pass

                #echo "$wls_ssid $wls_encr_type $wls_pass" | tee $log_file > $wls_save
                echo "$wls_pass" | tee $log_file
            ;;

       wpa+wpa2)
                uci set wireless.$Cfg.encryption="psk+psk2"
                uci set wireless.$Cfg.key=$wls_pass

                #echo "$wls_ssid $wls_encr_type $wls_pass" | tee $log_file > $wls_save
                echo "SETUP_ENCRYPTION $wls_pass" | tee $log_file
            ;;
       none)
                uci set wireless.$Cfg.encryption="none"

                echo "SETUP_ENCRYPTION $wls_ssid $wls_encr_type $wls_pass" | tee $log_file > $wls_save
            ;;
       *)
                uci set wireless.$Cfg.encryption="none"

                echo "$wls_ssid $wls_encr_type $wls_pass" | tee $log_file > $wls_save
                echo "SETUP_ENCRYPTION  "| tee $log_file
            ;;
    esac
    #encrypt
    send_encryption=$(/usr/bin/xorcrypt $wls_save $wls_pass | uuencode -)
    #send wireless config settings
    ###
    echo "SETUP_ENCRYPTION $send_encryption" | tee $log_file
    ###
    #setup wl0
    uci commit
    sync
    exit 0
fi


