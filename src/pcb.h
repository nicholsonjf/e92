struct pcb
{
    int pid;
    struct stream *streams[32];
};

extern struct pcb *currentPCB;