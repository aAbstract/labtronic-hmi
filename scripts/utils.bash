#!/bin/bash

alias qrn='make && ./build/bin/main'
alias cqrn='make clean && make && ./build/bin/main'
alias lvi='python ./scripts/LVGLImage.py'
alias emu_spi='gcc -x c -D EXEC ./src/platforms/linux_socket.h -lm -o soc && ./soc && rm ./soc'
alias dbg_emu_spi='gcc -x c -D EXEC ./src/platforms/linux_socket.h -lm -o soc -g'
