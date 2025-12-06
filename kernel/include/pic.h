#ifndef PIC_H
#define PIC_H

void pic_remap(void);
void pic_unmask_timer(void);
void pic_send_eoi(int irq);

#endif

