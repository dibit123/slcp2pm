#!/bin/bash

#set environment variables here
export INSAR_ZERODOP_SCR=$(readlink -f script)
export INSAR_ZERODOP_BIN=$(readlink -f src)

export PATH=$INSAR_ZERODOP_SCR:$INSAR_ZERODOP_BIN:$PATH
