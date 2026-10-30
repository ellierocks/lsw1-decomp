#include <dolphin/card.h>

#include "__card.h"

extern CARDControl lbl_8030E320[2];

// prototypes
static void CARDDeleteDeleteCallback(s32 chan, s32 result);

static void CARDDeleteDeleteCallback(s32 chan, s32 result) {
    CARDControl* card;
    CARDCallback callback;

    card = &lbl_8030E320[chan];
    callback = card->apiCallback;
    card->apiCallback = NULL;

    if (result < 0)
        goto error;

    result = __CARDFreeBlock(chan, card->startBlock, callback);
    if (result < 0)
        goto error;
    return;

error:
    __CARDPutControlBlock(card, result);
    if (callback)
        callback(chan, result);
}

s32 CARDDeleteAsync(s32 chan, const char* fileName, CARDCallback callback) {
    CARDControl* card;
    s32 fileNo;
    s32 result;
    CARDDir* dir;
    CARDDir* ent;

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0)
        return result;
    result = __CARDGetFileNo(card, fileName, &fileNo);
    if (result < 0)
        return __CARDPutControlBlock(card, result);
    if (__CARDIsOpened(card, fileNo))
        return __CARDPutControlBlock(card, CARD_RESULT_BUSY);

    dir = __CARDGetDirBlock(card);
    ent = &dir[fileNo];
    card->startBlock = ent->startBlock;
    memset(ent, 0xff, sizeof(CARDDir));

    card->apiCallback = callback ? callback : __CARDDefaultApiCallback;
    result = __CARDUpdateDir(chan, CARDDeleteDeleteCallback);
    if (result < 0)
        __CARDPutControlBlock(card, result);
    return result;
}
