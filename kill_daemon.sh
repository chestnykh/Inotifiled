#!/bin/bash

pid=`ps aux | grep -E 'ifiled' | head -n 1 | awk '{print $2}'`
kill -9 $pid

