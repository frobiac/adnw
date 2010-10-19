#!/bin/sh

echo "#define VERSIONINFO \"$(git describe HEAD)\"" 
echo "#define BUILDDATE   \"$(date +%y%m%d-%H%M)\"" 

