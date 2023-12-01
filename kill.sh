sudo kill $(ps -ef | grep 'tubekit' | grep -v 'grep' | awk '{print $2}')
