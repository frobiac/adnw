#!/bin/sh

echo "#define VERSIONINFO \"$(git describe HEAD)\"" 
echo "#define BUILDDATE   \"$(date +%Y%m%d-%H%M)\"" 

