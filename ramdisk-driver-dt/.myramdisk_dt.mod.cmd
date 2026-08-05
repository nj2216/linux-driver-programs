savedcmd_myramdisk_dt.mod := printf '%s\n'   myramdisk_dt.o | awk '!x[$$0]++ { print("./"$$0) }' > myramdisk_dt.mod
