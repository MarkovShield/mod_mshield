#!/bin/bash
make clean > /dev/null 2>&1
apxs -c -Wc,-O0 -Wc,-Wall -Wc,-DMOD_MSHIELD_SESSION_COUNT=10 -Wc,-DMOD_MSHIELD_COOKIESTORE_COUNT=30 mod_mshield.c
make clean > /dev/null 2>&1