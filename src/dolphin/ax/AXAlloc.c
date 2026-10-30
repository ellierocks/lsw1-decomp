#include <dolphin.h>
#include <dolphin/ax.h>

#include "__ax.h"

static AXVPB* __AXStackHead[AX_PRIORITY_STACKS];
static AXVPB* __AXStackTail[AX_PRIORITY_STACKS];

extern AXVPB* __AXCallbackStack;

static u32 __AXCheckStacks(void) {
    u32 i;
    u32 voices;
    AXVPB* voice;

    voices = 0;
    for (i = 0; i < 32; i++) {
        voice = __AXStackHead[i];
        while (voice != 0) {
            voices++;
            if (voices > 64) {
                return 0;
            }

            voice = voice->next;
        }
    }

    return 1;
}

AXVPB* __AXGetStackHead(u32 priority) {
    ASSERTLINE(97, priority < AX_PRIORITY_STACKS);
    return __AXStackHead[priority];
}

void __AXServiceCallbackStack(void) {
    AXVPB* p;

    for (p = __AXPopCallbackStack(); p; p = __AXPopCallbackStack()) {
        if (p->priority != 0) {
            if (p->callback) {
                p->callback(p);
            }
    
            __AXRemoveFromStack(p);
            __AXPushFreeStack(p);
        }
    }
}



void __AXAllocInit(void) {
#ifdef DEBUG
    OSReport("Initializing AXAlloc code module\n");
#endif
    __AXInitVoiceStacks();
}

void __AXAllocQuit(void) {
#ifdef DEBUG
    OSReport("Shutting down AXAlloc code module\n");
#endif
    __AXInitVoiceStacks();
}

void __AXPushFreeStack(AXVPB* p) {
    ASSERTLINE(168, p->priority);
    p->next = __AXStackHead[0];
    __AXStackHead[0] = p;
    p->priority = 0;
}



void __AXPushCallbackStack(AXVPB* p) {
    p->next1 = __AXCallbackStack;
    __AXCallbackStack = p;
}

AXVPB* __AXPopCallbackStack(void) {
    AXVPB* p;

    p = (void*)(u32)&__AXCallbackStack[0];
    if (p) {
        __AXCallbackStack = p->next1;
    }
    return p;
}

void __AXRemoveFromStack(AXVPB* p) {
    u32 i;
    AXVPB* head;
    AXVPB* tail;

    ASSERTLINE(219, p->priority);

    i = p->priority;
    head = __AXStackHead[i];
    tail = __AXStackTail[i];
    if (head == tail) {
        __AXStackHead[i] = __AXStackTail[i] = 0;
        return;
    }

    if (p == head) {
        __AXStackHead[i] = p->next;
        __AXStackHead[i]->prev = 0;
        return;
    }

    if (p == tail) {
        __AXStackTail[i] = p->prev;
        __AXStackTail[i]->next = 0;
        return;
    }

    head = p->prev;
    tail = p->next;
    head->next = tail;
    tail->prev = head;
}





void AXFreeVoice(AXVPB* p) {
    BOOL old;
    
    ASSERTLINE(322, p);
    ASSERTMSGLINE(326, p->priority != 0, "Calling AXFreeVoice() for voice that is already free\n");

    old = OSDisableInterrupts();
    __AXRemoveFromStack(p);
    if (p->pb.state == 1) {
        p->depop = 1;
    }
    __AXSetPBDefault(p);
    __AXPushFreeStack(p);

    ASSERTMSGLINE(343, __AXCheckStacks() != 0, "Voice list is trashed!\n");
    OSRestoreInterrupts(old);
}

AXVPB* AXAcquireVoice(u32 priority, void (*callback)(void*), u32 userContext) {
    BOOL old;
    AXVPB* p;
    u32 i;

    ASSERTLINE(361, priority);
    ASSERTLINE(362, priority < AX_PRIORITY_STACKS);

    old = OSDisableInterrupts();
    p = __AXPopFreeStack();
    if (p == 0) {
        for (i = 1; i < priority; i++) {
            p = __AXPopStackFromBottom(i);
            if (p) {
                if (p->pb.state == 1) {
                    p->depop = 1;
                }
                if (p->callback != 0) {
                    p->callback(p);
                }
                break;
            }
        }
    }

    if (p) {
        __AXPushStackHead(p, priority);
        p->callback = callback;
        p->userContext = userContext;
        __AXSetPBDefault(p);
    }

    ASSERTMSGLINE(410, __AXCheckStacks() != 0, "Voice list is trashed!\n");
    OSRestoreInterrupts(old);
    return p;
}


