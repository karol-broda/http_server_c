#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;36m'
NC='\033[0m'

echo -e "${BLUE}Starting Full Test Suite for HTTP Server${NC}"

cd "$(dirname "$0")/.."

if [ ! -f "./build/bin/http_server" ]; then
    echo -e "${RED}Build failed, stopping tests.${NC}"
    exit 1
fi

echo -e "${BLUE}Starting the server...${NC}"
./build/bin/http_server -d ./tests/data -p 4221 &
SERVER_PID=$!

sleep 2

total_tests=0
passed_tests=0

echo -e "${BLUE}Running test cases...${NC}"
if ./tests/test_cases/test_post_requests.sh; then
    ((passed_tests+=1))
fi
((total_tests+=1))

if ./tests/test_cases/test_file_retrieval.sh; then
    ((passed_tests+=1))
fi
((total_tests+=1))

if ./tests/test_cases/test_404_responses.sh; then
    ((passed_tests+=1))
fi
((total_tests+=1))

if ./tests/test_cases/test_user_agent.sh; then
    ((passed_tests+=1))
fi
((total_tests+=1))

echo -e "${BLUE}Stopping the server...${NC}"
kill $SERVER_PID

wait $SERVER_PID 2>/dev/null

echo -e "${GREEN}All tests completed.${NC}"

echo "Passed $passed_tests/$total_tests tests successfully."
