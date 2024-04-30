#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

SERVER_URL="http://localhost:4221"
NON_EXISTENT_FILE="nonexistent.txt"
RESPONSE_CODE=$(curl -s -o /dev/null -w "%{http_code}" "$SERVER_URL/$NON_EXISTENT_FILE")

if [[ "$RESPONSE_CODE" == "404" ]]; then
    echo -e "${GREEN}PASS: 404 response test passed.${NC}"
    exit 0
else
    echo -e "${RED}FAIL: 404 response test failed. Received HTTP status code: $RESPONSE_CODE instead of 404.${NC}"
    exit 1
fi
