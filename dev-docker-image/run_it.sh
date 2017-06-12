#/bin/bash
docker run -d --network mshielddemo_default -p 80:80 -p 443:443 mshield-reverse-proxy-dev /bin/bash
