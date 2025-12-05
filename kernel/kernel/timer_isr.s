.global timer_isr_stub
.extern timer_handler

timer_isr_stub:
    pusha
    call timer_handler
    popa
    iret
