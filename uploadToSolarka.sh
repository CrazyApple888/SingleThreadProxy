#!/bin/bash
scp -r "${PWD}" "$1"@ccfit.nsu.ru:/home/students/"$2"/"$1"/
