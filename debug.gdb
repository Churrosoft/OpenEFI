shell bash -c 'for f in /dev/serial/by-id/usb-Black_Magic_Debug_Black_Magic_Probe_*-if00; do [ -e "$f" ] && echo "target extended-remote $f" || echo -e "echo \033[1;31mDEBUGGER IS NOT CONNECTED\033[0m\\n\nquit"; break; done > /tmp/gdb-conn-source'
source /tmp/gdb-conn-source

mon connect_rst enable
# mon tpwr enable
shell bash -c 'sleep 0.05s'
mon swdp_scan
attach 1

# print demangled symbols
set print asm-demangle on

# set backtrace limit to not have infinite backtrace loops
set backtrace limit 32

# detect unhandled exceptions, hard faults and panics
break DefaultHandler
break HardFault
break rust_begin_unwind

# *try* to stop at the user entry point (it might be gone due to inlining)
break main

load