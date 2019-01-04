#!/bin/bash

if [ "$1" = "s" ] 
then
  echo "Server Mode"
  kill `pidof Server` >/dev/null 2>&1
  if [ "$2" = "1" ]
  then
    ./bin/Server 7735 ./bin/recv.bin 0.05 >./s1_1
  fi
  if [ "$2" = "2" ]
  then
    ./bin/Server 7735 ./bin/recv.bin 0.05 >./s2_1
  fi
  if [ "$2" = "3" ]
  then
    ./bin/Server 7735 ./bin/recv.bin 0.01 & >./s2_1
    ./bin/Server 7735 ./bin/recv.bin 0.02 & >./s2_2
    ./bin/Server 7736 ./bin/recv.bin 0.03 & >./s2_3
    ./bin/Server 7737 ./bin/recv.bin 0.04 & >./s2_4
    ./bin/Server 7738 ./bin/recv.bin 0.05 & >./s2_5
    ./bin/Server 7739 ./bin/recv.bin 0.06 & >./s2_6
    ./bin/Server 7740 ./bin/recv.bin 0.07 & >./s2_7
    ./bin/Server 7741 ./bin/recv.bin 0.08 & >./s2_8
    ./bin/Server 7742 ./bin/recv.bin 0.09 & >./s2_9
    ./bin/Server 7743 ./bin/recv.bin 0.10 >./s2_10
  fi
  kill `pidof Server` >/dev/null 2>&1
fi

SAddr="152.46.18.44"

if [ "$1" = "c" ]
then
  kill `pidof Client` >/dev/null 2>&1
  echo "Client Mode"
  if [ "$2" = "1" ]
  then
    ./bin/Client $SAddr 7735 ./bin/send.bin 1 500 -n 5 >./t1_1
    ./bin/Client $SAddr 7735 ./bin/send.bin 2 500 -n 5 >./t1_2
    ./bin/Client $SAddr 7735 ./bin/send.bin 4 500 -n 5 >./t1_4
    ./bin/Client $SAddr 7735 ./bin/send.bin 8 500 -n 5 >./t1_8
    ./bin/Client $SAddr 7735 ./bin/send.bin 16 500 -n 5 >./t1_16
    ./bin/Client $SAddr 7735 ./bin/send.bin 32 500 -n 5 >./t1_32
    ./bin/Client $SAddr 7735 ./bin/send.bin 64 500 -n 5 >./t1_64
    ./bin/Client $SAddr 7735 ./bin/send.bin 128 500 -n 5 >./t1_128
    ./bin/Client $SAddr 7735 ./bin/send.bin 256 500 -n 5 >./t1_256
    ./bin/Client $SAddr 7735 ./bin/send.bin 512 500 -n 5 >./t1_512
    ./bin/Client $SAddr 7735 ./bin/send.bin 1024 500 -n 5 >./t1_1024
  fi
  if [ "$2" = "2" ]
  then
    ./bin/Client $SAddr 7735 ./bin/send.bin 64 100 -n 5 >./t2_1
    ./bin/Client $SAddr 7735 ./bin/send.bin 64 200 -n 5 >./t2_2
    ./bin/Client $SAddr 7735 ./bin/send.bin 64 300 -n 5 >./t2_3
    ./bin/Client $SAddr 7735 ./bin/send.bin 64 400 -n 5 >./t2_4
    ./bin/Client $SAddr 7735 ./bin/send.bin 64 500 -n 5 >./t2_5
    ./bin/Client $SAddr 7735 ./bin/send.bin 64 600 -n 5 >./t2_6
    ./bin/Client $SAddr 7735 ./bin/send.bin 64 700 -n 5 >./t2_7
    ./bin/Client $SAddr 7735 ./bin/send.bin 64 800 -n 5 >./t2_8
    ./bin/Client $SAddr 7735 ./bin/send.bin 64 900 -n 5 >./t2_9
    ./bin/Client $SAddr 7735 ./bin/send.bin 64 1000 -n 5 >./t2_10
  fi
  if [ "$2" = "3" ]
  then
    ./bin/Client $SAddr 7735 ./bin/send.bin 64 500 -n 5 >./t3_1
    ./bin/Client $SAddr 7736 ./bin/send.bin 64 500 -n 5 >./t3_2
    ./bin/Client $SAddr 7737 ./bin/send.bin 64 500 -n 5 >./t3_3
    ./bin/Client $SAddr 7738 ./bin/send.bin 64 500 -n 5 >./t3_4
    ./bin/Client $SAddr 7739 ./bin/send.bin 64 500 -n 5 >./t3_5
    ./bin/Client $SAddr 7740 ./bin/send.bin 64 500 -n 5 >./t3_6
    ./bin/Client $SAddr 7741 ./bin/send.bin 64 500 -n 5 >./t3_7
    ./bin/Client $SAddr 7742 ./bin/send.bin 64 500 -n 5 >./t3_8
    ./bin/Client $SAddr 7743 ./bin/send.bin 64 500 -n 5 >./t3_9
    ./bin/Client $SAddr 7744 ./bin/send.bin 64 500 -n 5 >./t3_10
  fi
fi
if [ "$1" = "f" ]
then
  dd if=/dev/urandom of=./bin/send.bin count=1024 bs=1024
fi

kill `pidof Client` >/dev/null 2>&1
kill `pidof Server` >/dev/null 2>&1

wait
