# Mac-to-GC Symbol Review: Top 200 Candidates

**Do not auto-apply.** Each entry requires manual verification.

## Candidate Table

| # | Conf. | Pri | Mac Name | Mac Addr | Source | GC Name | GC Addr | Match Type | Notes |
|---|--------|-----|----------|----------|--------|---------|---------|------------|-------|
| 1 | HIGH | P1 | `NuDatFileLoadBuffer` | 0x1e2574 | lsw2_ppc | `NuDatFileLoadBuffer` | 0x80004630 | exact_name | Exact name match |
| 2 | HIGH | P1 | `NuDatFileOpen` | 0x1e1104 | lsw2_ppc | `NuDatFileOpen` | 0x80004CF8 | exact_name | Exact name match |
| 3 | HIGH | P1 | `NuDatOpenEx` | 0x1e1f44 | lsw2_ppc | `NuDatOpenEx` | 0x80004F6C | exact_name | Exact name match |
| 4 | HIGH | P1 | `NuFileBeginBlkRead` | 0x1e17e0 | lsw2_ppc | `NuFileBeginBlkRead` | 0x80004390 | exact_name | Exact name match |
| 5 | HIGH | P1 | `NuFileClose` | 0x1e0088 | lsw2_ppc | `NuFileClose` | 0x80003C34 | exact_name | Exact name match |
| 6 | HIGH | P1 | `NuFileLoadBuffer` | 0x1e2724 | lsw2_ppc | `NuFileLoadBuffer` | 0x80003DBC | exact_name | Exact name match |
| 7 | HIGH | P1 | `NuFileOpen` | 0x1e1d3c | lsw2_ppc | `NuFileOpen` | 0x800035F0 | exact_name | Exact name match |
| 8 | HIGH | P1 | `NuFilePos` | 0x1e0210 | lsw2_ppc | `NuFilePos` | 0x800039BC | exact_name | Exact name match |
| 9 | HIGH | P1 | `NuPPLoadBuffer` | 0x264038 | lsw2_ppc | `NuPPLoadBuffer` | 0x800082A4 | exact_name | Exact name match |
| 10 | HIGH | P1 | `NuSpecialFind` | 0x1bbdac | lsw2_ppc | `NuSpecialFind` | 0x80012D00 | exact_name | Exact name match |
| 11 | HIGH | P1 | `NuSpecialFindMulti` | 0x1bc024 | lsw2_ppc | `NuSpecialFindMulti` | 0x80012E94 | exact_name | Exact name match |
| 12 | MEDIUM | P2 | `_Z10NuFileOpenPc12nufilemode_e` | 0x1a0d0 | lsw1_demo_ppc | `NuFileOpen` | 0x800035F0 | demangled_partial | Demangled base 'NuFileOpenPc12nufilemode' overlaps with NuFileOpen |
| 13 | MEDIUM | P2 | `_Z11NuFileClosei` | 0x1a470 | lsw1_demo_ppc | `NuFileClose` | 0x80003C34 | demangled_partial | Demangled base 'NuFileClosei' overlaps with NuFileClose |
| 14 | MEDIUM | P2 | `_Z11NuMemFreeFnPvPci` | 0x31bb0 | lsw1_demo_ppc | `NuMemFree` | 0x8000726C | demangled_partial | Demangled base 'NuMemFreeFnPvPci' overlaps with NuMemFree |
| 15 | MEDIUM | P2 | `_Z12NuMemAllocFniPci` | 0x318b0 | lsw1_demo_ppc | `NuMemAlloc` | 0x80006F74 | demangled_partial | Demangled base 'NuMemAllocFniPci' overlaps with NuMemAlloc |
| 16 | MEDIUM | P2 | `_Z13NuDatFileOpenP10nudathdr_sPc12nufilemode_e` | 0x1bec0 | lsw1_demo_ppc | `NuDatFileOpen` | 0x80004CF8 | demangled_partial | Demangled base 'NuDatFileOpenP10nudathdr' overlaps with NuDatFileOpen |
| 17 | MEDIUM | P2 | `_Z13NuFileOpenDirPc` | 0x15ef90 | lsw1_demo_ppc | `NuFileOpen` | 0x800035F0 | demangled_partial | Demangled base 'NuFileOpenDirPc' overlaps with NuFileOpen |
| 18 | MEDIUM | P2 | `_Z13NuSpecialFindP8nugscn_sP12nuhspecial_sPci` | 0xb99f0 | lsw1_demo_ppc | `NuSpecialFind` | 0x80012D00 | demangled_partial | Demangled base 'NuSpecialFindP8nugscn' overlaps with NuSpecialFind |
| 19 | MEDIUM | P2 | `_Z14NuFileCloseDiri` | 0x15f0b0 | lsw1_demo_ppc | `NuFileClose` | 0x80003C34 | demangled_partial | Demangled base 'NuFileCloseDiri' overlaps with NuFileClose |
| 20 | MEDIUM | P2 | `_Z14NuFileOpenSizei` | 0x1a060 | lsw1_demo_ppc | `NuFileOpen` | 0x800035F0 | demangled_partial | Demangled base 'NuFileOpenSizei' overlaps with NuFileOpen |
| 21 | MEDIUM | P2 | `_Z14NuPPLoadBufferiPvi` | 0x132ca0 | lsw1_demo_ppc | `NuPPLoadBuffer` | 0x800082A4 | demangled_partial | Demangled base 'NuPPLoadBufferiPvi' overlaps with NuPPLoadBuffer |
| 22 | MEDIUM | P2 | `_Z16NuFileLoadBufferPcPvi` | 0x1ae80 | lsw1_demo_ppc | `NuFileLoadBuffer` | 0x80003DBC | demangled_partial | Demangled base 'NuFileLoadBufferPcPvi' overlaps with NuFileLoadBuffer |
| 23 | MEDIUM | P2 | `_Z17CreditBufferAllocP9variptr_uS0_i` | 0x1322e0 | lsw1_demo_ppc | `CreditBufferAlloc` | 0x8010BDC0 | demangled_partial | Demangled base 'CreditBufferAllocP9variptr' overlaps with CreditBufferAlloc |
| 24 | MEDIUM | P2 | `_Z17NuDatFileOpenSizei` | 0x1c150 | lsw1_demo_ppc | `NuDatFileOpen` | 0x80004CF8 | demangled_partial | Demangled base 'NuDatFileOpenSizei' overlaps with NuDatFileOpen |
| 25 | MEDIUM | P2 | `_Z18NuFileBeginBlkReadii` | 0x1b400 | lsw1_demo_ppc | `NuFileBeginBlkRead` | 0x80004390 | demangled_partial | Demangled base 'NuFileBeginBlkReadii' overlaps with NuFileBeginBlkRead |
| 26 | MEDIUM | P2 | `_Z18NuFileLoadBufferVPPcP9variptr_uS1_` | 0x1b020 | lsw1_demo_ppc | `NuFileLoadBuffer` | 0x80003DBC | demangled_partial | Demangled base 'NuFileLoadBufferVPPcP9variptr' overlaps with NuFileLoadBuffer |
| 27 | MEDIUM | P2 | `_Z18NuSpecialFindMultiP8nugscn_sP12nuhspecial_sPcii` | 0xb9ba0 | lsw1_demo_ppc | `NuSpecialFind` | 0x80012D00 | demangled_partial | Demangled base 'NuSpecialFindMultiP8nugscn' overlaps with NuSpecialFind |
| 28 | MEDIUM | P2 | `_Z19NuDatFileLoadBufferP10nudathdr_sPcPvi` | 0x1b850 | lsw1_demo_ppc | `NuDatFileLoadBuffer` | 0x80004630 | demangled_partial | Demangled base 'NuDatFileLoadBufferP10nudathdr' overlaps with NuDatFileLoadBuffer |
| 29 | MEDIUM | P2 | `_Z21NuAnimCurve2CalcValExP14nuanimcurve2_sP12nuanimtime_s16NUANIMKEYTYPES_e` | 0x1282f0 | lsw1_demo_ppc | `NuAnimCurve2CalcVal` | 0x80016E28 | demangled_partial | Demangled base 'NuAnimCurve2CalcValExP14nuanimcurve2' overlaps with NuAnimCurve2CalcVal |
| 30 | MEDIUM | P2 | `_Z22NuDatFileLoadBufferLSNP10nudathdr_sPcPvi` | 0x1b660 | lsw1_demo_ppc | `NuDatFileLoadBuffer` | 0x80004630 | demangled_partial | Demangled base 'NuDatFileLoadBufferLSNP10nudathdr' overlaps with NuDatFileLoadBuffer |
| 31 | MEDIUM | P2 | `_Z9NuFilePosi` | 0x1ab30 | lsw1_demo_ppc | `NuFilePos` | 0x800039BC | demangled_partial | Demangled base 'NuFilePosi' overlaps with NuFilePos |
| 32 | MEDIUM | P3 | `NuAnimCurve2CalcValEx` | 0x1743d0 | lsw2_ppc | `NuAnimCurve2CalcVal` | 0x80016E28 | lsw2_superstr | GC 'NuAnimCurve2CalcVal' is substring of Mac 'NuAnimCurve2CalcValEx' |
| 33 | MEDIUM | P3 | `NuDatFileOpenSize` | 0x1df110 | lsw2_ppc | `NuDatFileOpen` | 0x80004CF8 | lsw2_superstr | GC 'NuDatFileOpen' is substring of Mac 'NuDatFileOpenSize' |
| 34 | MEDIUM | P3 | `NuDatOpen` | 0x1e22dc | lsw2_ppc | `NuDatOpenEx` | 0x80004F6C | lsw2_substr | Base 'NuDatOpen' is substring of GC 'NuDatOpenEx' |
| 35 | MEDIUM | P3 | `NuFileCloseDir` | 0x265434 | lsw2_ppc | `NuFileClose` | 0x80003C34 | lsw2_superstr | GC 'NuFileClose' is substring of Mac 'NuFileCloseDir' |
| 36 | MEDIUM | P3 | `NuFileLoad` | 0x1e2400 | lsw2_ppc | `NuFileLoadBuffer` | 0x80003DBC | lsw2_substr | Base 'NuFileLoad' is substring of GC 'NuFileLoadBuffer' |
| 37 | MEDIUM | P3 | `NuFileLoadBufferVP` | 0x1e288c | lsw2_ppc | `NuFileLoadBuffer` | 0x80003DBC | lsw2_superstr | GC 'NuFileLoadBuffer' is substring of Mac 'NuFileLoadBufferVP' |
| 38 | MEDIUM | P3 | `NuFileOpenDir` | 0x265354 | lsw2_ppc | `NuFileOpen` | 0x800035F0 | lsw2_superstr | GC 'NuFileOpen' is substring of Mac 'NuFileOpenDir' |
| 39 | MEDIUM | P3 | `NuFileOpenSize` | 0x1e08cc | lsw2_ppc | `NuFileOpen` | 0x800035F0 | lsw2_superstr | GC 'NuFileOpen' is substring of Mac 'NuFileOpenSize' |
| 40 | MEDIUM | P3 | `NuMemAllocFn` | 0x166830 | lsw2_ppc | `NuMemAlloc` | 0x80006F74 | lsw2_superstr | GC 'NuMemAlloc' is substring of Mac 'NuMemAllocFn' |
| 41 | MEDIUM | P3 | `NuMemFreeFn` | 0x166478 | lsw2_ppc | `NuMemFree` | 0x8000726C | lsw2_superstr | GC 'NuMemFree' is substring of Mac 'NuMemFreeFn' |
| 42 | MEDIUM | P3 | `NuSpecialFindMultiWC` | 0x1bbea4 | lsw2_ppc | `NuSpecialFind` | 0x80012D00 | lsw2_superstr | GC 'NuSpecialFind' is substring of Mac 'NuSpecialFindMultiWC' |
| 43 | MEDIUM | P3 | `_Z10NuMemAlloci` | 0x166340 | lsw2_ppc | `NuMemAlloc` | 0x80006F74 | lsw2_superstr | GC 'NuMemAlloc' is substring of Mac 'NuMemAlloci' |
| 44 | MEDIUM | P3 | `_Z25NuSpecialFindByPlatformIDP8nugscn_sP12nuhspecial_si` | 0x13481c | lsw2_ppc | `NuSpecialFind` | 0x80012D00 | lsw2_superstr | GC 'NuSpecialFind' is substring of Mac 'NuSpecialFindByPlatformIDP8nugscn' |
| 45 | MEDIUM | P3 | `_Z9NuMemFreePv` | 0x166520 | lsw2_ppc | `NuMemFree` | 0x8000726C | lsw2_superstr | GC 'NuMemFree' is substring of Mac 'NuMemFreePv' |
| 46 | LOW | P5 | `AISysGetPathPos2` | 0x5cd4 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 47 | LOW | P5 | `AITriggerSetAddTrigger` | 0x5f44 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 48 | LOW | P5 | `AITriggerSetCreate` | 0x55d0 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 49 | LOW | P5 | `AITriggerSetSysAddPair` | 0x5dd4 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 50 | LOW | P5 | `AITriggerSetSysCreate` | 0x5598 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 51 | LOW | P5 | `AITriggerSetSysProcess` | 0x5628 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 52 | LOW | P5 | `AITriggerSetSysReset` | 0x550c | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 53 | LOW | P5 | `AITriggerSysAutoSetUp` | 0x60f4 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 54 | LOW | P5 | `AddBoltType` | 0x647c | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 55 | LOW | P5 | `BoltDebris` | 0x67cc | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 56 | LOW | P5 | `BoltHitParts` | 0x6ee0 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 57 | LOW | P5 | `BoltReflect` | 0x6944 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 58 | LOW | P5 | `EndBolt` | 0x6d88 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 59 | LOW | P5 | `FindBolt` | 0x64ec | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 60 | LOW | P5 | `FindIncomingBolt` | 0x6c04 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 61 | LOW | P5 | `GetBoltTypeByID` | 0x65cc | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 62 | LOW | P5 | `GetBoltTypeIDByCreature` | 0x66b0 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 63 | LOW | P5 | `GetBoltTypeIDByName` | 0x6608 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 64 | LOW | P5 | `InitBoltTypes` | 0x62a0 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 65 | LOW | P5 | `ResetBoltTypes` | 0x6430 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 66 | LOW | P5 | `ResetBolts` | 0x6440 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 67 | LOW | P5 | `_GLOBAL__D_g_ApplicationRun` | 0x32a0 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 68 | LOW | P5 | `_GLOBAL__I_g_ApplicationRun` | 0x3290 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 69 | LOW | P5 | `_Z10Initializev` | 0x2960 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 70 | LOW | P5 | `_Z11BoltRayCastP6bolt_sP7nuvec_sS2_f` | 0x6a58 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 71 | LOW | P5 | `_Z11MacGameMainv` | 0x25e0 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 72 | LOW | P5 | `_Z12BoltHitForceP6bolt_sP7nuvec_sS2_S2_fPh` | 0x6e00 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 73 | LOW | P5 | `_Z12OneShotTimerP16__EventLoopTimerPv` | 0x28b0 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 74 | LOW | P5 | `_Z13OrientateBoltP6bolt_s` | 0x6a00 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 75 | LOW | P5 | `_Z13starwars_mainiPPc` | 0x32b0 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 76 | LOW | P5 | `_Z14IsBuildExpiredsss` | 0x2cdc | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 77 | LOW | P5 | `_Z14ShowSplashFadev` | 0x2530 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 78 | LOW | P5 | `_Z15AppEventHandlerP25OpaqueEventHandlerCallRefP14OpaqueEventRefPv` | 0x2d50 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 79 | LOW | P5 | `_Z15MakeBoltPosListP6bolt_sP7nuvec_s` | 0x6b58 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 80 | LOW | P5 | `_Z17CreatePixelFormathPP7GDevicelllhi` | 0x42c0 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 81 | LOW | P5 | `_Z19EndBolt_EwokTorpedoP6bolt_s` | 0x6988 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 82 | LOW | P5 | `_Z21EndBolt_PhotonTorpedoP6bolt_s` | 0x6990 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 83 | LOW | P5 | `_Z24updateBolt_PhotonTorpedoP6bolt_s` | 0x70e0 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 84 | LOW | P5 | `_Z41__static_initialization_and_destruction_0ii` | 0x31ec | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 85 | LOW | P5 | `_Z7MacMainiPPc` | 0x2900 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 86 | LOW | P5 | `_Z7dprintfPKcz` | 0x3180 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 87 | LOW | P5 | `_ZN10MacDisplay10GetNthModeEiRiS0_S0_` | 0x3f60 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 88 | LOW | P5 | `_ZN10MacDisplay10InitializeEv` | 0x3240 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 89 | LOW | P5 | `_ZN10MacDisplay11GetCardTypeEv` | 0x4060 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 90 | LOW | P5 | `_ZN10MacDisplay11GetDeviceIDEv` | 0x4020 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 91 | LOW | P5 | `_ZN10MacDisplay11GetGLVendorEv` | 0x40a0 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 92 | LOW | P5 | `_ZN10MacDisplay11GetMainPortEv` | 0x3790 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 93 | LOW | P5 | `_ZN10MacDisplay11GetNumModesEv` | 0x3f20 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 94 | LOW | P5 | `_ZN10MacDisplay12InWindowModeEv` | 0x3730 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 95 | LOW | P5 | `_ZN10MacDisplay12IsWindowModeEv` | 0x3920 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 96 | LOW | P5 | `_ZN10MacDisplay12SetupDisplayEii` | 0x3530 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 97 | LOW | P5 | `_ZN10MacDisplay13GetGLRendererEv` | 0x40f0 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 98 | LOW | P5 | `_ZN10MacDisplay13GetMainWindowEv` | 0x3760 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 99 | LOW | P5 | `_ZN10MacDisplay13GlobalToLocalER5Point` | 0x3aa0 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 100 | LOW | P5 | `_ZN10MacDisplay13LocalToGlobalER5Point` | 0x3a30 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 101 | LOW | P5 | `_ZN10MacDisplay13PointInWindowE5Point` | 0x3b10 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 102 | LOW | P5 | `_ZN10MacDisplay14GetCurrentModeERiS0_` | 0x3d00 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 103 | LOW | P5 | `_ZN10MacDisplay14GetCurrentModeERiS0_S0_` | 0x3db0 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 104 | LOW | P5 | `_ZN10MacDisplay14GetDescriptionEv` | 0x3fd0 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 105 | LOW | P5 | `_ZN10MacDisplay14ReleaseContextERP16OpaqueContextRef` | 0x4640 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 106 | LOW | P5 | `_ZN10MacDisplay14ReleaseDisplayEv` | 0x36a0 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 107 | LOW | P5 | `_ZN10MacDisplay15GetDeviceHandleEv` | 0x37e0 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 108 | LOW | P5 | `_ZN10MacDisplay15GetGLExtensionsEv` | 0x4140 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 109 | LOW | P5 | `_ZN10MacDisplay18GetMaxTextureUnitsEv` | 0x4240 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 110 | LOW | P5 | `_ZN10MacDisplay18GetVideoMemoryInfoERlS0_` | 0x41f0 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 111 | LOW | P5 | `_ZN10MacDisplay19CenterRectInDisplayER7MacRect` | 0x3b60 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 112 | LOW | P5 | `_ZN10MacDisplay19CreateScreenContextEihRh` | 0x43c0 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 113 | LOW | P5 | `_ZN10MacDisplay19GetCustomResolutionERiS0_` | 0x39c0 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 114 | LOW | P5 | `_ZN10MacDisplay19UseCustomResolutionEv` | 0x3960 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 115 | LOW | P5 | `_ZN10MacDisplay22IsGLExtensionSupportedEPKc` | 0x4190 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 116 | LOW | P5 | `_ZN10MacDisplay23GetPCPixelShaderVersionEv` | 0x4280 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 117 | LOW | P5 | `_ZN10MacDisplay6FadeInEf` | 0x3bb0 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 118 | LOW | P5 | `_ZN10MacDisplay7FadeOutEf` | 0x3c40 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 119 | LOW | P5 | `_ZN10MacDisplay7SetModeEiii` | 0x3e20 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 120 | LOW | P5 | `_ZN10MacGlobals10LockSystemEv` | 0x31a0 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 121 | LOW | P5 | `_ZN10MacGlobals12UnlockSystemEv` | 0x31d0 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 122 | LOW | P5 | `_ZN10MacGlobals14IsSystemLockedEv` | 0x3210 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 123 | LOW | P5 | `_ZN14MacPreferences10GetBooleanEPKci` | 0x2ed0 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 124 | LOW | P5 | `_ZN14MacPreferences10GetIntegerEPKci` | 0x2d60 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 125 | LOW | P5 | `_ZN14MacPreferences10PutBooleanEPKci` | 0x2f10 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 126 | LOW | P5 | `_ZN14MacPreferences10PutIntegerEPKci` | 0x2dc0 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 127 | LOW | P5 | `_ZN14MacPreferences11SynchronizeEv` | 0x3140 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 128 | LOW | P5 | `_ZN14MacPreferences7GetRectEPKcR6CGRect` | 0x2f40 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 129 | LOW | P5 | `_ZN14MacPreferences7PutRectEPKcRK6CGRect` | 0x3050 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 130 | LOW | P5 | `_ZN14MacPreferences8GetFloatEPKcf` | 0x2e10 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 131 | LOW | P5 | `_ZN14MacPreferences8PutFloatEPKcf` | 0x2e70 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 132 | LOW | P5 | `_ZN14MacPreferences9GetStringEPKcPciS0_` | 0x2be0 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 133 | LOW | P5 | `_ZN14MacPreferences9PutStringEPKcS0_` | 0x2cd0 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 134 | LOW | P5 | `__darwin_gcc3_preregister_frame_info` | 0x2bc0 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 135 | LOW | P5 | `_call_mod_init_funcs` | 0x2474 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 136 | LOW | P5 | `_call_objcInit` | 0x2714 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 137 | LOW | P5 | `_dyld_func_lookup` | 0x2510 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 138 | LOW | P5 | `_dyld_init_check` | 0x24bc | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 139 | LOW | P5 | `_start` | 0x22f4 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 140 | LOW | P5 | `crt_basename` | 0x267c | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 141 | LOW | P5 | `crt_strbeginswith` | 0x26b8 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 142 | LOW | P5 | `darwin_unwind_dyld_add_image_hook` | 0x2938 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 143 | LOW | P5 | `darwin_unwind_dyld_remove_image_hook` | 0x2a20 | lsw2_ppc | `—NO GC MATCH—` | — | lsw2_proposal | LSW2-only name (non-Nu2), potential new GC symbol |
| 144 | LOW | P5 | `dyld_stub_binding_helper` | 0x24f8 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |
| 145 | LOW | P5 | `main` | 0x2b20 | lsw1_demo_ppc | `—NO GC MATCH—` | — | lsw1_proposal | LSW1-only name, no corresponding GC named function |

---

## Summary

- Total candidates: 145, Top 200 shown
- HIGH: 11, MEDIUM: 34, LOW: 100

### By Source
- lsw1_demo_ppc: 80
- lsw2_ppc: 65

### By Subsystem
- AISys: 1
- Credit: 1
- Nu2: 22
- NuAnim: 1
- NuDat: 4
- NuFile: 9
- NuMem: 2
- NuPP: 1
- NuSpecial: 2
- Other: 102

### By Match Type
- demangled_partial: 20
- exact_name: 11
- lsw1_proposal: 60
- lsw2_proposal: 40
- lsw2_substr: 2
- lsw2_superstr: 12

### Note: All 11 HIGH exact-name matches already present in GC symbols.txt
No changes needed for those.
