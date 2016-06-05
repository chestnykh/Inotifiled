#!/bin/bash
n=`cat include/handle_events.h | grep -E '#define Ntf'`
if [ "$n" == "#define Ntf" ];
then 
	ntf=$(cat npollfd.data)
	sed 's/#define Ntf/#define Ntf '$ntf'/g' -i include/handle_events.h
fi
