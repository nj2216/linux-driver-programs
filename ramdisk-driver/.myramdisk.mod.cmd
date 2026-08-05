savedcmd_myramdisk.mod := printf '%s\n'   myramdisk.o | awk '!x[$$0]++ { print("./"$$0) }' > myramdisk.mod
