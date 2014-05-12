#!/bin/bash

rm $1-srv
make IPC=$1.c RUN=serverFront.c
mv tp $1-srv

rm $1-clt
make IPC=$1.c RUN=clientUI.c
mv tp $1-clt

rm $1-clt-no-srv
make CLIENT=clientNoSrv.c DB=productDBX.c
mv tp clt-no-srv