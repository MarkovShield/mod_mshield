#!/bin/sh
# $Id: rebuild.sh 117 2009-08-06 15:00:50Z droethli $
# Rebuilds mod_mshield for development.

function findbinary() {
	defloc="$1"
	envloc="$2"
	base=$(basename "$defloc")
	pathloc=$(type -p "$base")
	for loc in "$envloc" "$pathloc" "$defloc"; do
		if [ -x "$loc" ]; then
			echo "$loc"
			return
		fi
	done
	echo "$0: $base not found" >&2
	exit -1
}

apxs=`findbinary /opt/applic/httpd/bin/apxs "$APXS"`

make APXS="$apxs" APXSFLAGS="-c -i -a -Wc,-O0 -Wc,-Wall -Wc,-DMOD_MSHIELD_SESSION_COUNT=10 -Wc,-DMOD_MSHIELD_COOKIESTORE_COUNT=30"

