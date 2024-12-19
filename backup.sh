#!/bin/bash
#
# Backup source code (backup.sh)
# Version: 1.0
#
# Quick Start:
#
#    ./backup.sh
#

PROJECT_FOLDER="${PWD##*/}"
#echo $PROJECT_FOLDER

# prepare folder
mkdir -p ./Backup

# clean up Build folder
rm -rf ./Build/*

# clean up Backup folder
rm -rf ./Backup/*

cd ..

# get time tag
NOW=$(date +"%Y-%m-%d_%H-%M")

# compress folder to zip file
zip -r ${PROJECT_FOLDER}_$NOW.zip $PROJECT_FOLDER

# move zip file to backup folder
mv ${PROJECT_FOLDER}_$NOW.zip ./$PROJECT_FOLDER/Backup/${PROJECT_FOLDER}_$NOW$TAG.zip

