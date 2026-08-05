savedcmd_myzero.mod := printf '%s\n'   myzero.o | awk '!x[$$0]++ { print("./"$$0) }' > myzero.mod
