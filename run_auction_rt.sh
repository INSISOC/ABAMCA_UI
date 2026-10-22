#!/bin/bash
set -e  # Exit immediately if any command fails

echo "=============================================="
echo "     Running Scenario Generator + Auction     "
echo "=============================================="

echo ""
echo "=== Step 1: Enter scenario_generator ==="
cd scenario_generator

echo "=== Step 2: Compile scenario generator ==="
source build_rt.sh

echo "=== Step 3: Execute simulation: ./bin/abamca ==="
./bin/abamca

# Check that the simulation produced the expected file
if [ ! -f data_auction.txt ]; then
    echo "ERROR: data_auction.txt was NOT produced by abamca!"
    exit 1
fi

echo "Simulation completed. File generated: scenario_generator/data_auction.txt"
echo ""

echo "=== Step 4: Enter auction folder ==="
cd ../auction

echo "=== Step 5: Remove previous data_auction.txt (if exists) ==="
rm -f data_auction.txt

echo "=== Step 6: Copy new data_auction.txt from scenario_generator ==="
cp ../scenario_generator/data_auction.txt .

echo "File copied successfully."
echo ""

echo "=== Step 7: Compile auction ==="
make all

echo ""
echo "=============================================="
echo "      All Tasks Completed Successfully         "
echo "=============================================="