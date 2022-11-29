#!/bin/bash - 
#===============================================================================
#
#          FILE: check_run.sh
# 
#         USAGE: ./check_run.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: YOUR NAME (), 
#  ORGANIZATION: 
#       CREATED: 10/27/2015 12:28
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

fmnc=`screen -ls | grep -P '[0-9]+\.lsong2'|wc -l`
#echo $fmnc,`ps -df|grep ScaleBox`
if [ $fmnc -lt 1 ]; then
        echo `date`,"Restarting"
        (cd /home/netscale/FMNC_server/ScaleBox/src; screen -S lsong2 -dm sudo /home/netscale/FMNC_server/ScaleBox/src/ScaleBox /home/netscale/FMNC_server/ScaleBox/src/fmnc-open.xml )
else
        echo `date`,"Running"
fi


