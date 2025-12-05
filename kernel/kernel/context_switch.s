.global context_switch
.type context_switch, @function

# void context_switch(struct context *old, struct context *new)
context_switch:
    pusha                   # Save registers

    mov 8(%esp), %eax       # arg1 = old
    test %eax, %eax
    jz 1f
    mov %esp, (%eax)        # old->esp = ESP
1:

    mov 12(%esp), %eax      # arg2 = new
    mov (%eax), %esp        # ESP = new->esp

    popa                    # Restore registers
    ret
