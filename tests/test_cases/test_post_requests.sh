#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

SERVER_URL="http://localhost:4221"
POST_ENDPOINT="files/test_upload.txt"
TEST_DATA="Sample data for POST request."

curl -s -o /dev/null -X POST --data "$TEST_DATA" "$SERVER_URL/$POST_ENDPOINT"

SAVED_CONTENT="$(cat ./tests/data/test_upload.txt)"
if [[ "$SAVED_CONTENT" == "$TEST_DATA" ]]; then
    echo -e "${GREEN}PASS: POST request test passed.${NC}"
    exit 0
else
    echo -e "${RED}FAIL: POST request test failed.${NC}"
    echo "Expected: '$TEST_DATA'"
    echo "Received: '$SAVED_CONTENT'"
    exit 1
fi

rm -rf ./tests/data/test_upload.txt
