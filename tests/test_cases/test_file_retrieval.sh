#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

SERVER_URL="http://localhost:4221"
TEST_FILE="test.txt"
EXPECTED_CONTENT="$(cat ./tests/data/$TEST_FILE)"
RESPONSE=$(curl -s "$SERVER_URL/files/$TEST_FILE")
TEST_FILE2="index.html"
EXPECTED_CONTENT2="$(cat ./tests/data/$TEST_FILE2)"
RESPONSE2=$(curl -s "$SERVER_URL/files/$TEST_FILE2")

if [[ "$RESPONSE" == "$EXPECTED_CONTENT" && "$RESPONSE2" == "$EXPECTED_CONTENT2" ]]; then
    echo -e "${GREEN}PASS: Both file retrieval tests passed.${NC}"
    exit 0
elif [[ "$RESPONSE" != "$EXPECTED_CONTENT" && "$RESPONSE2" == "$EXPECTED_CONTENT2" ]]; then
    echo -e "${RED}FAIL: First file retrieval test failed.${NC}"
    echo "Expected for $TEST_FILE: $EXPECTED_CONTENT"
    echo "Received for $TEST_FILE: $RESPONSE"
    exit 1
elif [[ "$RESPONSE" == "$EXPECTED_CONTENT" && "$RESPONSE2" != "$EXPECTED_CONTENT2" ]]; then
    echo -e "${RED}FAIL: Second file retrieval test failed.${NC}"
    echo "Expected for $TEST_FILE2: $EXPECTED_CONTENT2"
    echo "Received for $TEST_FILE2: $RESPONSE2"
    exit 1
else
    echo -e "${RED}FAIL: Both file retrieval tests failed.${NC}"
    echo "Expected for $TEST_FILE: $EXPECTED_CONTENT"
    echo "Received for $TEST_FILE: $RESPONSE"
    echo "Expected for $TEST_FILE2: $EXPECTED_CONTENT2"
    echo "Received for $TEST_FILE2: $RESPONSE2"
    exit 1
fi
