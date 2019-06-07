#!/bin/bash
BASE_PATH=$(dirname "${BASH_SOURCE}")
BASE_PATH=$(cd "${BASE_PATH}"; pwd)

# inputs
slcp_dir=$(readlink -f $1)
swath=$2
rlks=$3
alks=$4

dirm=${slcp_dir}/master/IW${swath}
dirf=${slcp_dir}/fine_coreg/IW${swath}
dirg=${slcp_dir}/geom_master/IW${swath}

work_dir=$(pwd)

mkdir s${swath}
cd s${swath}

$BASE_PATH/log_ratio.py -mdir ${dirm} -sdir ${dirf} -gdir ${dirg} -rlks ${rlks} -alks ${alks}
cd $work_dir
