#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

SERVER_URL="http://localhost:4221"
EXPECTED_USER_AGENT="Mozilla/5.0 (X11; Linux x86_64; rv:88.0) Gecko/20100101 Firefox/88.0"

print_result() {
    if [ "$1" = "pass" ]; then
        echo -e "${GREEN}PASS: User-Agent Test${NC}"
    else
        echo -e "${RED}FAIL: User-Agent Test${NC}"
    fi
}

RESPONSE=$(curl -s -A "$EXPECTED_USER_AGENT" "$SERVER_URL/user-agent")

USER_AGENT=$(echo "$RESPONSE")

if [ "$USER_AGENT" = "$EXPECTED_USER_AGENT" ]; then
    print_result "pass"
    exit 0
else
    print_result "fail"
    echo "Expected: '$EXPECTED_USER_AGENT'"
    echo "Received: '$USER_AGENT'"
    exit 1
fi
