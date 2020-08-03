#!../../bin/darwin-x86/Clock1

## You may have to change Clock1 to something else
## everywhere it appears in this file

< envPaths

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/trackerlv.dbd"
#Clock1_registerRecordDeviceDriver pdbbase
trackerlv_registerRecordDeviceDriver pdbbase

## Load record instances
dbLoadRecords("db/biTracker.db","user=odell")

cd "${TOP}/iocBoot/${IOC}"
iocInit

## Start any sequence programs
#seq sncxxx,"user=odell"
