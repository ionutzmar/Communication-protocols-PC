#!/bin/bash

SPEED=20
DELAY=200
LOSS=10
REORDER1=10
CORRUPT=10
FILENAME=fileX

sudo rm recv_$FILENAME &>/dev/null

killall -9 link &>/dev/null
killall -9 recv &>/dev/null
killall -9 send &>/dev/null

./link_emulator/link speed=$SPEED delay=$DELAY loss=$LOSS corrupt=$CORRUPT reorder=$REORDER &> /dev/null &
sleep 1
./recv &
sleep 1

time ./send $FILENAME $SPEED $DELAY

echo "[SCRIPT] Finished transfer, checking files: $FILENAME recv_$FILENAME"
diff $FILENAME recv_$FILENAME
