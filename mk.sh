#!/bin/bash

SEM="$2"

if [ -z "$2" ]; then
	SEM=semSysV
fi

SEM="$SEM".c

rm $1-srv
make IPC=$1.c SEM="$SEM" RUN=serverFront.c DB=productDB.c
mv tp $1-srv

rm $1-clt
make IPC=$1.c SEM="$SEM" RUN=clientUI.c DB=productDB.c
mv tp $1-clt

rm clt-no-srv
make CLIENT=clientNoSrv.c RUN=clientUI.c DB=productDBX.c
mv tp clt-no-srv