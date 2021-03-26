#ifndef _MYPCB_H
#define _MYPCB_H

struct pcb
{
    int pid;
    Stream *streams[32];
};

extern struct pcb *currentPCB;

#endif /* ifndef _MYPCB_H */