#!/bin/bash

# Test script for linear regression server

echo "Starting server on port 8080..."
cd build
./server 8080 &
SERVER_PID=$!

# Wait for server to start
sleep 2

echo -e "\n=== Test 1: Perfect linear relationship (y = 2x) ==="
curl -X POST http://localhost:8080/linear-regression \
  -H "Content-Type: application/json" \
  -d '{"points": [{"x": 1, "y": 2}, {"x": 2, "y": 4}, {"x": 3, "y": 6}]}'

echo -e "\n\n=== Test 2: Linear with noise ==="
curl -X POST http://localhost:8080/linear-regression \
  -H "Content-Type: application/json" \
  -d '{"points": [{"x": 1, "y": 2.1}, {"x": 2, "y": 3.9}, {"x": 3, "y": 6.2}, {"x": 4, "y": 8.1}, {"x": 5, "y": 9.8}]}'

echo -e "\n\n=== Test 3: Negative values ==="
curl -X POST http://localhost:8080/linear-regression \
  -H "Content-Type: application/json" \
  -d '{"points": [{"x": -2, "y": -4}, {"x": -1, "y": -2}, {"x": 0, "y": 0}, {"x": 1, "y": 2}, {"x": 2, "y": 4}]}'

echo -e "\n\n=== Test 4: Error handling (only one point) ==="
curl -X POST http://localhost:8080/linear-regression \
  -H "Content-Type: application/json" \
  -d '{"points": [{"x": 1, "y": 2}]}'

echo -e "\n\n=== Test 5: Home page ==="
curl http://localhost:8080/

echo -e "\n\nStopping server..."
kill $SERVER_PID

echo "Tests complete!"
