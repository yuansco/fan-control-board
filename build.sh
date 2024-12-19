#!/bin/bash
#
# install gcc:
#
#    sudo apt-get install gcc-arm-none-eabi
#
# build command:
#
#    ./build.sh $BOARD
#
# e.g.  build Core/Board/g070cbt6_evb
#
#    ./build.sh g070cbt6_evb
#
#
# e.g.  build all board
#
#    ./build.sh all
#

############################
# create Build folder
############################

mkdir -p Build
rm -rf ./Build/*

##################################
# Automatic build date and commit
##################################

file="./Core/Frame/build_info.h"

BUILD_INFO_TEXT="
#ifndef __BUILD_INFO_H__
#define __BUILD_INFO_H__

/**
 * commit version and build date
 * 
 * BUILD_COMMIT and BUILD_DATE will update by build.sh, Don't modify its directly.
 */

#define VERSION \"V1.0\"
#define BUILD_DATE \"\"
#define BUILD_COMMIT \"\"

#ifndef BOARDNAME
#define BOARDNAME \"STM32\"
#endif /* BOARDNAME */

#endif /* __BUILD_INFO_H__ */
"

printf "$BUILD_INFO_TEXT" > $file

BUILD_DATE=$(date +%Y.%m.%d)
echo "Update build date $BUILD_DATE to $file"
LINE_NUMBER=$(cat $file |grep '#define BUILD_DATE' -n |cut -d ':' -f 1)
DATE_LINE="#define BUILD_DATE \"$BUILD_DATE\""
sed -i "$LINE_NUMBER s/ *.*/$DATE_LINE/" $file

BUILD_COMMIT=$(git rev-parse --short HEAD)
echo "Update build commit $BUILD_COMMIT to $file"
LINE_NUMBER=$(cat $file |grep '#define BUILD_COMMIT' -n |cut -d ':' -f 1)
COMMIT_LINE="#define BUILD_COMMIT \"$BUILD_COMMIT\""
sed -i "$LINE_NUMBER s/ *.*/$COMMIT_LINE/" $file

############################
# select c files
############################

# include build.mk to select board.c files
# buildmk.py will select board.c files depending on board.h
# list of selected files is in Core/Board/${BOARD}/build.mk


# get board name

if [[ "$1" == "all" ]]
then
    BOARD_LIST=$(ls ./Core/Board -C)
    echo "Target board: $BOARD_LIST"
    BOARD="ALL"
elif [[ "$1" != "" ]]
then
    echo "Target board: $1"
    BOARD=$1
else
    # default board is first folder in board list
    BOARD=$(ls ./Core/Board |head -1)
    echo "Target board: $BOARD (default)"
fi

############################################
# show target project FLASH and SRAM size
############################################

#
# .text  : code and const variables
# .data  : global variables and static variables that are initialized
# .bss   : Uninitialized variables
#
# Flash size = .text + .data
# RAM size = .bss + .data
#

function show() {

    echo ""
    echo -n "Board: $1"
    # elf file
    ELF=$(ls ./Build/$1/*.elf)

    # ld file
    LD=$(ls ./*.ld)

    # check elf file exists
    if [ -e "$ELF" ]
    then
        # calculate Flash and SRAM usage
        size_info=$(arm-none-eabi-size $ELF | awk NR\>1)
        text=$(echo "$size_info" | awk '{print $1}')
        data=$(echo "$size_info" | awk '{print $2}')
        bss=$(echo "$size_info" | awk '{print $3}')
        flash=$(($text + $data))
        sram=$(($bss + $data))

        # get chip Flash and SRAM size
        flash_full=$(cat $LD |grep "FLASH (rx)" |cut -d '=' -f 3 |cut -d 'K' -f 1 |tr -d ' ')
        sram_full=$(cat $LD |grep "RAM (xrw)" |cut -d '=' -f 3 |cut -d 'K' -f 1 |tr -d ' ')
        flash_used=$(($flash*100 / ($flash_full*1024)))
        sram_used=$(($sram*100 / ($sram_full*1024)))

        flash_full_bytes=$(($flash_full*1024))
        sram_full_bytes=$(($sram_full*1024))

        echo 
        printf ".text=%d   .data=%d   .bss=%d (Bytes)\n" "$text" "$data" "$bss"
        printf "FLASH used = %5s/%6s bytes (%2s%%)\n" "$flash" "$flash_full_bytes" "$flash_used"
        printf "SRAM  used = %6s/%5s bytes (%2s%%)\n" "$sram" "$sram_full_bytes" "$sram_used"
    else
        #echo "$ELF not exists."
        echo "Build Fail!"
        exit 0
    fi
}

############################
# build target project
############################

build(){
    # clear old file
    rm -rf ./Build/$1/*.*
    # build build.mk
    #python3 buildmk.py "$1"
    # make build
    make BOARD=$1
}

############################
# build project
############################

if [[ "$BOARD" == "ALL" ]]
then
    # build all board
    for bb in $(ls ./Core/Board)
    do
        echo "Build: $bb"
        build $bb
    done
else
    # build target board
    build $BOARD
fi

############################
# show FLASH and SRAM size
############################

# show FLASH and SRAM size
if [[ "$BOARD" == "ALL" ]]
then
    # show all board
    for bb in $(ls ./Core/Board) 
    do
        show $bb
    done
else
    # show target board
    show $BOARD
fi

exit 0
