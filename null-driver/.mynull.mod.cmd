savedcmd_mynull.mod := printf '%s\n'   mynull.o | awk '!x[$$0]++ { print("./"$$0) }' > mynull.mod
