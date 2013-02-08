#!/bin/bash

sed -ri '/syntax on/ s/^"//; /set background/ s/^"//; /if has\("autocmd"\)/,/^$/ s/^"//; /set showcmd/ s/^"//; /set showmatch/ s/^"//' /etc/vim/vimrc
