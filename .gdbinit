define target remote
target extended-remote $arg0
end

define target hookpost-remote

mon tpwr enable
shell bash -c 'sleep 0.05s'
#mon swdp_scan
#attach 1

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

end