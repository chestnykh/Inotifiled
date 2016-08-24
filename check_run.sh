#!/bin/bash

pid=`pgrep ifiled`


if [ "$pid" != "" ];
then
	echo "ifiled daemon is already running"
fi
