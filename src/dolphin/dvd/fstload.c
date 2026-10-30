#include <dolphin.h>
#include <dolphin/os.h>
#include <dolphin/dvd.h>


extern u8 bb2Buf_802F4978[63];

extern u32 lbl_8040A9B8;
extern DVDBB2* bb2_8040A9BC;
extern DVDDiskID* idTmp_8040A9C0;
extern DVDCommandBlock fstloadBlock_802F49B8;

// prototypes
static void cb(s32 result, DVDCommandBlock* block);

static void cb(s32 result, DVDCommandBlock* block) {
    if (result > 0) {
        switch(lbl_8040A9B8) {
        case 0:
            lbl_8040A9B8 = 1;
            DVDReadAbsAsyncForBS(block, bb2_8040A9BC, 0x20, 0x420, cb);
            return;
        case 1:
            lbl_8040A9B8 = 2;
            DVDReadAbsAsyncForBS(block, bb2_8040A9BC->FSTAddress, (bb2_8040A9BC->FSTLength + 0x1F) & 0xFFFFFFE0, bb2_8040A9BC->FSTPosition, cb);
        default:
            return;
        }
    }

    if (result == -1) {
        return;
    } else if (result == -4) {
        lbl_8040A9B8 = 0;
        DVDReset();
        DVDReadDiskID(block, idTmp_8040A9C0, cb);
    }
}

void __fstLoad(void) {
    OSBootInfo* bootInfo;
    DVDDiskID* id;
    u8 idTmpBuf[63];
    s32 state;
    void* arenaHi;


    arenaHi = OSGetArenaHi();
    bootInfo = (void*)OSPhysicalToCached(0);
    idTmp_8040A9C0 = (void*)OSRoundUp32B(idTmpBuf);
    bb2_8040A9BC = (void*)OSRoundUp32B(bb2Buf_802F4978);

    DVDReset();
    DVDReadDiskID(&fstloadBlock_802F49B8, idTmp_8040A9C0, cb);

    while (1) {
        state = DVDGetDriveStatus();
        if (state == 0) {
            break;
        }

        // weird switch that seemingly wont do anything but break out of its own switch. What was this for? Early DVD development pre-hardware?
        switch(state) {
        case DVD_STATE_FATAL_ERROR: break;
        case DVD_STATE_BUSY: break;
        case DVD_STATE_WAITING: break;
        case DVD_STATE_COVER_CLOSED: break;
        case DVD_STATE_NO_DISK: break;
        case DVD_STATE_COVER_OPEN: break;
        case DVD_STATE_MOTOR_STOPPED: break;
        }
    }

    bootInfo->FSTLocation = (void*)bb2_8040A9BC->FSTAddress;
    bootInfo->FSTMaxLength = bb2_8040A9BC->FSTMaxLength;
    id = &bootInfo->DVDDiskID;
    memcpy(id, idTmp_8040A9C0, 0x20);
    OSReport("\n");
    OSReport("  Game Name ... %c%c%c%c\n", id->gameName[0], id->gameName[1], id->gameName[2], id->gameName[3]);
    OSReport("  Company ..... %c%c\n", id->company[0], id->company[1]);
    OSReport("  Disk # ...... %d\n", id->diskNumber);
    OSReport("  Game ver .... %d\n", id->gameVersion);
    OSReport("  Streaming ... %s\n", !(id->streaming) ? "OFF" : "ON");
    OSReport("\n");
    OSSetArenaHi(bb2_8040A9BC->FSTAddress);
}
