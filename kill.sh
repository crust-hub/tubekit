sudo kill -9 $( ps -ef | grep 'tubekit' | grep -v 'grep' | awk '{print $2}' )
