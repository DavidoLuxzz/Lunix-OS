; use no includes
; every function here will just be a redirection to a kernel system function
; NOTE: this will only work for ring 0 programs (drivers, system extensions...)
    section .text
; just a tool function for firing a system call.
; will be useful when trying to system call from C-code
    global  __syscall
__syscall:
    int 0x80
    ret

; every program has an entry point. when a program is calling a function it goes first thru
; redirection, and then it does its job. as a 3rd party program, you cannot know every time
; where is which function located in memory, so to not change redirection addresses every time
; when OS gets recompiled you can call a standard function called __Lunix_fn_sync.
; __Lunix_fn_sync collets information that kernel sends to program at the runtime.
; It does a job synchronizing it to a programs local variables.
; This information and data include system function addresses. So it gets much easier to call
; a system function when using the given standard "library"
; problem:
; - THIS FUNCTION IS NOT SUPPORTED UNTIL A PROPER SYSTEM ISN'T MADE
    global  __Lunix_fn_sync
__Lunix_fn_sync:
    ret

