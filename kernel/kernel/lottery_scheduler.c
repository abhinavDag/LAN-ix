// kernel/kernel/lottery_scheduler.c
#include "../include/lottery.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../include/random.h"

#define DEFAULT_TICKETS 10U

enum proc_state_local { UNUSED = 0 };

extern void context_switch(struct context *old_ctx, struct context *new_ctx);
extern struct pcb *get_current_pcb(void);
extern void set_current_pcb(struct pcb *p);
extern uint32_t external_entropy_u32(void); /* optional */
extern const char *kernel_cmdline_str(void); /* optional */

static struct pcb *proc_table = NULL;
static struct pcb *last_added = NULL;
static uint32_t total_tickets = 0;

static inline uint64_t rdtsc64(void) {
    uint32_t a, d;
    __asm__ volatile("rdtsc" : "=a"(a), "=d"(d));
    return ((uint64_t)d << 32) | a;
}

void lottery_add_process(struct pcb *p) {
    if (!p) return;
    p->next = NULL;
    if (!proc_table) proc_table = p;
    else last_added->next = p;
    last_added = p;
    if (p->tickets == 0) p->tickets = DEFAULT_TICKETS;
    total_tickets += p->tickets;
}

void lottery_remove_process(struct pcb *p) {
    if (!p || !proc_table) return;
    struct pcb *cur = proc_table, *prev = NULL;
    while (cur) {
        if (cur == p) break;
        prev = cur; cur = cur->next;
    }
    if (!cur) return;
    if (prev) prev->next = cur->next;
    else proc_table = cur->next;
    if (last_added == cur) last_added = prev;
    if (cur->tickets <= total_tickets) total_tickets -= cur->tickets;
    else total_tickets = 0;
    cur->next = NULL;
}

struct pcb *lottery_find_by_pid(int pid) {
    for (struct pcb *p = proc_table; p; p = p->next) if (p->pid == pid) return p;
    return NULL;
}

int lottery_set_tickets(int pid, uint32_t tickets) {
    struct pcb *p = lottery_find_by_pid(pid);
    if (!p) return -1;
    if (tickets == 0) tickets = 1;
    total_tickets -= p->tickets;
    p->tickets = tickets;
    total_tickets += p->tickets;
    return 0;
}

int sys_set_tickets(uint32_t tickets) {
    struct pcb *me = get_current_pcb();
    if (!me) return -1;
    return lottery_set_tickets(me->pid, tickets);
}

static struct pcb *lottery_pick(void) {
    if (total_tickets == 0) return NULL;
    uint32_t winning = rng_get_range(total_tickets) + 1;
    uint32_t acc = 0;
    for (struct pcb *p = proc_table; p; p = p->next) {
        if (p->state != PROC_RUNNABLE && p->state != PROC_RUNNING) continue;
        acc += p->tickets;
        if (acc >= winning) return p;
    }
    return NULL;
}

void lottery_schedule_tick(void) {
    struct pcb *current = get_current_pcb();
    struct pcb *next = lottery_pick();
    if (!next) return;
    if (current == next) return;
    struct context *old_ctx = current ? current->ctx : NULL;
    struct context *new_ctx = next->ctx;
    if (current) current->state = PROC_RUNNABLE;
    next->state = PROC_RUNNING;
    set_current_pcb(next);
    context_switch(old_ctx, new_ctx);
}

void lottery_schedule_yield(void) {
    struct pcb *current = get_current_pcb();
    struct pcb *next = lottery_pick();
    if (!next || next == current) return;
    struct context *old_ctx = current ? current->ctx : NULL;
    struct context *new_ctx = next->ctx;
    if (current) current->state = PROC_RUNNABLE;
    next->state = PROC_RUNNING;
    set_current_pcb(next);
    context_switch(old_ctx, new_ctx);
}

void lottery_parse_cmdline_tickets(void) {
    const char *cmd = kernel_cmdline_str();
    if (!cmd) return;
    const char *p = cmd;
    const char *found = NULL;
    while (*p) {
        if (*p == 't') {
            const char *s = p;
            const char *needle = "tickets=";
            const char *n = needle;
            while (*s && *n && *s == *n) { s++; n++; }
            if (*n == '\0') { found = s; break; }
        }
        p++;
    }
    if (!found) return;
    p = found;
    while (*p && *p != '\0') {
        while (*p == ' ' || *p == ',') p++;
        if (!*p) break;
        int pid = 0; bool have_digit = false;
        while (*p >= '0' && *p <= '9') { pid = pid*10 + (*p - '0'); p++; have_digit = true; }
        if (!have_digit) { while (*p && *p != ',') p++; continue; }
        if (*p != ':') continue;
        p++;
        uint32_t tickets = 0; bool have_t = false;
        while (*p >= '0' && *p <= '9') { tickets = tickets*10 + (*p - '0'); p++; have_t = true; }
        if (have_t) lottery_set_tickets(pid, tickets);
        while (*p && *p != ',') p++;
    }
}

void lottery_init(void) {
    proc_table = NULL;
    last_added = NULL;
    total_tickets = 0;
    lottery_parse_cmdline_tickets();
}
