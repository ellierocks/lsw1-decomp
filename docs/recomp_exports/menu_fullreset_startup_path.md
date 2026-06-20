# `Menu_FullReset` static callsite path

This report uses direct PPC `bl`/`bla` export rows. Callsite order is static instruction layout inside `Menu_FullReset`, not runtime temporal proof.

## Direct callers

- No direct caller was resolved in the exported direct-call graph.

## Direct callees in static callsite order

| # | Callsite | Callee | Address | Label | Evidence |
|---:|---:|---|---:|---|---|
| 1 | `0x800A0870` | `fn_8009CC00` | `0x8009CC00` | generic; separate orchestration inspection | `instruction=0x4BFFC391; direct_target=0x8009CC00` |
| 2 | `0x800A0874` | `fn_8009D23C` | `0x8009D23C` | generic/unrecovered | `instruction=0x4BFFC9C9; direct_target=0x8009D23C` |
| 3 | `0x800A088C` | `fn_80071E0C` | `0x80071E0C` | generic/unrecovered | `instruction=0x4BFD1581; direct_target=0x80071E0C` |
| 4 | `0x800A08E8` | `fn_800B43F0` | `0x800B43F0` | generic/unrecovered | `instruction=0x48013B09; direct_target=0x800B43F0` |
| 5 | `0x800A0910` | `fn_8006DF1C` | `0x8006DF1C` | generic/unrecovered | `instruction=0x4BFCD60D; direct_target=0x8006DF1C` |
| 6 | `0x800A092C` | `fn_8006C748` | `0x8006C748` | generic/unrecovered | `instruction=0x4BFCBE1D; direct_target=0x8006C748` |
| 7 | `0x800A0960` | `fn_80091FC0` | `0x80091FC0` | generic/unrecovered | `instruction=0x4BFF1661; direct_target=0x80091FC0` |
| 8 | `0x800A0978` | `fn_8008EE48` | `0x8008EE48` | generic/unrecovered | `instruction=0x4BFEE4D1; direct_target=0x8008EE48` |
| 9 | `0x800A0984` | `fn_800C9E44` | `0x800C9E44` | generic/unrecovered | `instruction=0x480294C1; direct_target=0x800C9E44` |
| 10 | `0x800A099C` | `fn_8009E6FC` | `0x8009E6FC` | generic/unrecovered | `instruction=0x4BFFDD61; direct_target=0x8009E6FC` |
| 11 | `0x800A09A4` | `fn_800274BC` | `0x800274BC` | generic/unrecovered | `instruction=0x4BF86B19; direct_target=0x800274BC` |
| 12 | `0x800A09BC` | `AXFXSetHooks` | `0x8002765C` | audio-named | `instruction=0x4BF86CA1; direct_target=0x8002765C` |
| 13 | `0x800A09C0` | `fn_80027564` | `0x80027564` | generic/unrecovered | `instruction=0x4BF86BA5; direct_target=0x80027564` |
| 14 | `0x800A09C8` | `NuRndrFootPrints` | `0x80025A70` | Nu rendering/texture-named | `instruction=0x4BF850A9; direct_target=0x80025A70` |
| 15 | `0x800A09E0` | `fn_8014C20C` | `0x8014C20C` | generic/unrecovered | `instruction=0x480AB82D; direct_target=0x8014C20C` |
| 16 | `0x800A09E4` | `fn_800CC970` | `0x800CC970` | generic/unrecovered | `instruction=0x4802BF8D; direct_target=0x800CC970` |
| 17 | `0x800A0A68` | `fn_8009D5D0` | `0x8009D5D0` | generic/unrecovered | `instruction=0x4BFFCB69; direct_target=0x8009D5D0` |
| 18 | `0x800A0A7C` | `fn_8009E6FC` | `0x8009E6FC` | generic/unrecovered | `instruction=0x4BFFDC81; direct_target=0x8009E6FC` |
| 19 | `0x800A0A84` | `fn_800274BC` | `0x800274BC` | generic/unrecovered | `instruction=0x4BF86A39; direct_target=0x800274BC` |
| 20 | `0x800A0A9C` | `AXFXSetHooks` | `0x8002765C` | audio-named | `instruction=0x4BF86BC1; direct_target=0x8002765C` |
| 21 | `0x800A0AA0` | `fn_80027564` | `0x80027564` | generic/unrecovered | `instruction=0x4BF86AC5; direct_target=0x80027564` |
| 22 | `0x800A0AA8` | `NuRndrFootPrints` | `0x80025A70` | Nu rendering/texture-named | `instruction=0x4BF84FC9; direct_target=0x80025A70` |
| 23 | `0x800A0AE0` | `fn_8010CA18` | `0x8010CA18` | generic/unrecovered | `instruction=0x4806BF39; direct_target=0x8010CA18` |
| 24 | `0x800A0BA8` | `fn_800AB9F4` | `0x800AB9F4` | generic/unrecovered | `instruction=0x4800AE4D; direct_target=0x800AB9F4` |
| 25 | `0x800A0BC8` | `fn_800BA87C` | `0x800BA87C` | generic/unrecovered | `instruction=0x48019CB5; direct_target=0x800BA87C` |
| 26 | `0x800A0BCC` | `fn_800ABE30` | `0x800ABE30` | generic/unrecovered | `instruction=0x4800B265; direct_target=0x800ABE30` |
| 27 | `0x800A0BDC` | `fn_80009970` | `0x80009970` | generic/unrecovered | `instruction=0x4BF68D95; direct_target=0x80009970` |
| 28 | `0x800A0C98` | `NuTexAnimProgAssembleEnd` | `0x80007468` | Nu rendering/texture-named | `instruction=0x4BF667D1; direct_target=0x80007468` |
| 29 | `0x800A0CCC` | `fn_8008F968` | `0x8008F968` | generic/unrecovered | `instruction=0x4BFEEC9D; direct_target=0x8008F968` |
| 30 | `0x800A0D18` | `fn_800CA4AC` | `0x800CA4AC` | generic/unrecovered | `instruction=0x48029795; direct_target=0x800CA4AC` |
| 31 | `0x800A0DF0` | `fn_8009D160` | `0x8009D160` | generic/unrecovered | `instruction=0x4BFFC371; direct_target=0x8009D160` |
| 32 | `0x800A0E00` | `fn_800274BC` | `0x800274BC` | generic/unrecovered | `instruction=0x4BF866BD; direct_target=0x800274BC` |
| 33 | `0x800A0E04` | `fn_80027564` | `0x80027564` | generic/unrecovered | `instruction=0x4BF86761; direct_target=0x80027564` |
| 34 | `0x800A0E0C` | `NuRndrFootPrints` | `0x80025A70` | Nu rendering/texture-named | `instruction=0x4BF84C65; direct_target=0x80025A70` |
| 35 | `0x800A0E30` | `fn_800CB8F0` | `0x800CB8F0` | generic/unrecovered | `instruction=0x4802AAC1; direct_target=0x800CB8F0` |
| 36 | `0x800A0E34` | `fn_80063BCC` | `0x80063BCC` | generic/unrecovered | `instruction=0x4BFC2D99; direct_target=0x80063BCC` |
| 37 | `0x800A0E40` | `fn_80045004` | `0x80045004` | generic/unrecovered | `instruction=0x4BFA41C5; direct_target=0x80045004` |
| 38 | `0x800A0E78` | `fn_800C9238` | `0x800C9238` | generic/unrecovered | `instruction=0x480283C1; direct_target=0x800C9238` |
| 39 | `0x800A0ED0` | `fn_800E595C` | `0x800E595C` | generic/unrecovered | `instruction=0x48044A8D; direct_target=0x800E595C` |
| 40 | `0x800A0EE0` | `fn_80044D4C` | `0x80044D4C` | generic/unrecovered | `instruction=0x4BFA3E6D; direct_target=0x80044D4C` |
| 41 | `0x800A0F04` | `fn_800A2370` | `0x800A2370` | generic/unrecovered | `instruction=0x4800146D; direct_target=0x800A2370` |
| 42 | `0x800A0F14` | `fn_80009970` | `0x80009970` | generic/unrecovered | `instruction=0x4BF68A5D; direct_target=0x80009970` |
| 43 | `0x800A0F48` | `fn_8010B5F0` | `0x8010B5F0` | generic/unrecovered | `instruction=0x4806A6A9; direct_target=0x8010B5F0` |
| 44 | `0x800A0F64` | `fn_800B9EF0` | `0x800B9EF0` | generic/unrecovered | `instruction=0x48018F8D; direct_target=0x800B9EF0` |
| 45 | `0x800A0FCC` | `fn_80044D4C` | `0x80044D4C` | generic/unrecovered | `instruction=0x4BFA3D81; direct_target=0x80044D4C` |
| 46 | `0x800A0FD8` | `NuTexAnimSetSignals` | `0x80015D40` | Nu rendering/texture-named | `instruction=0x4BF74D69; direct_target=0x80015D40` |
| 47 | `0x800A0FDC` | `fn_80015EFC` | `0x80015EFC` | generic/unrecovered | `instruction=0x4BF74F21; direct_target=0x80015EFC` |
| 48 | `0x800A0FE4` | `NuTexAnimSetSignals` | `0x80015D40` | Nu rendering/texture-named | `instruction=0x4BF74D5D; direct_target=0x80015D40` |
| 49 | `0x800A0FE8` | `fn_80015EFC` | `0x80015EFC` | generic/unrecovered | `instruction=0x4BF74F15; direct_target=0x80015EFC` |
| 50 | `0x800A0FEC` | `fn_8011D4F8` | `0x8011D4F8` | generic/unrecovered | `instruction=0x4807C50D; direct_target=0x8011D4F8` |
| 51 | `0x800A0FF4` | `fn_800B9E4C` | `0x800B9E4C` | generic/unrecovered | `instruction=0x48018E59; direct_target=0x800B9E4C` |
| 52 | `0x800A1004` | `fn_80044D4C` | `0x80044D4C` | generic/unrecovered | `instruction=0x4BFA3D49; direct_target=0x80044D4C` |
| 53 | `0x800A1008` | `Menu_NavigateForward` | `0x800B44EC` | menu-named | `instruction=0x480134E5; direct_target=0x800B44EC` |
| 54 | `0x800A1034` | `fn_80044D4C` | `0x80044D4C` | generic/unrecovered | `instruction=0x4BFA3D19; direct_target=0x80044D4C` |
| 55 | `0x800A1040` | `fn_80044FD4` | `0x80044FD4` | generic/unrecovered | `instruction=0x4BFA3F95; direct_target=0x80044FD4` |
| 56 | `0x800A104C` | `fn_800AFF58` | `0x800AFF58` | generic/unrecovered | `instruction=0x4800EF0D; direct_target=0x800AFF58` |
| 57 | `0x800A105C` | `fn_80044D4C` | `0x80044D4C` | generic/unrecovered | `instruction=0x4BFA3CF1; direct_target=0x80044D4C` |
| 58 | `0x800A1060` | `fn_800D97F8` | `0x800D97F8` | generic/unrecovered | `instruction=0x48038799; direct_target=0x800D97F8` |
| 59 | `0x800A1074` | `fn_8011E308` | `0x8011E308` | generic/unrecovered | `instruction=0x4807D295; direct_target=0x8011E308` |
| 60 | `0x800A108C` | `fn_800D97BC` | `0x800D97BC` | generic/unrecovered | `instruction=0x48038731; direct_target=0x800D97BC` |
| 61 | `0x800A10FC` | `fn_8006DCD4` | `0x8006DCD4` | generic/unrecovered | `instruction=0x4BFCCBD9; direct_target=0x8006DCD4` |
| 62 | `0x800A112C` | `fn_80044D4C` | `0x80044D4C` | generic/unrecovered | `instruction=0x4BFA3C21; direct_target=0x80044D4C` |
| 63 | `0x800A116C` | `fn_8010CA18` | `0x8010CA18` | generic/unrecovered | `instruction=0x4806B8AD; direct_target=0x8010CA18` |
| 64 | `0x800A1174` | `fn_800B4DF0` | `0x800B4DF0` | generic/unrecovered | `instruction=0x48013C7D; direct_target=0x800B4DF0` |
| 65 | `0x800A1178` | `fn_8010438C` | `0x8010438C` | generic/unrecovered | `instruction=0x48063215; direct_target=0x8010438C` |
| 66 | `0x800A1194` | `fn_8006E184` | `0x8006E184` | generic/unrecovered | `instruction=0x4BFCCFF1; direct_target=0x8006E184` |
| 67 | `0x800A1198` | `fn_80044D4C` | `0x80044D4C` | generic/unrecovered | `instruction=0x4BFA3BB5; direct_target=0x80044D4C` |
| 68 | `0x800A11B0` | `fn_8002CEF4` | `0x8002CEF4` | generic/unrecovered | `instruction=0x4BF8BD45; direct_target=0x8002CEF4` |
| 69 | `0x800A11C4` | `fn_80027D34` | `0x80027D34` | generic/unrecovered | `instruction=0x4BF86B71; direct_target=0x80027D34` |
| 70 | `0x800A11DC` | `fn_80027D34` | `0x80027D34` | generic/unrecovered | `instruction=0x4BF86B59; direct_target=0x80027D34` |
| 71 | `0x800A11F0` | `fn_80027D34` | `0x80027D34` | generic/unrecovered | `instruction=0x4BF86B45; direct_target=0x80027D34` |
| 72 | `0x800A1250` | `fn_800274BC` | `0x800274BC` | generic/unrecovered | `instruction=0x4BF8626D; direct_target=0x800274BC` |
| 73 | `0x800A1254` | `fn_80027564` | `0x80027564` | generic/unrecovered | `instruction=0x4BF86311; direct_target=0x80027564` |
| 74 | `0x800A125C` | `NuRndrFootPrints` | `0x80025A70` | Nu rendering/texture-named | `instruction=0x4BF84815; direct_target=0x80025A70` |
| 75 | `0x800A1274` | `fn_800165B4` | `0x800165B4` | generic/unrecovered | `instruction=0x4BF75341; direct_target=0x800165B4` |
| 76 | `0x800A1280` | `fn_800165B4` | `0x800165B4` | generic/unrecovered | `instruction=0x4BF75335; direct_target=0x800165B4` |
| 77 | `0x800A12B0` | `fn_8009CFF8` | `0x8009CFF8` | generic/unrecovered | `instruction=0x4BFFBD49; direct_target=0x8009CFF8` |
| 78 | `0x800A12B8` | `fn_8011EB18` | `0x8011EB18` | generic/unrecovered | `instruction=0x4807D861; direct_target=0x8011EB18` |
| 79 | `0x800A12BC` | `NuGCutSceneSysPostBackgroundLoad` | `0x8003F12C` | Nu engine-named | `instruction=0x4BF9DE71; direct_target=0x8003F12C` |
| 80 | `0x800A12C8` | `fn_800165B4` | `0x800165B4` | generic/unrecovered | `instruction=0x4BF752ED; direct_target=0x800165B4` |
| 81 | `0x800A12D8` | `fn_80016660` | `0x80016660` | generic/unrecovered | `instruction=0x4BF75389; direct_target=0x80016660` |
| 82 | `0x800A1350` | `fn_800165B4` | `0x800165B4` | generic/unrecovered | `instruction=0x4BF75265; direct_target=0x800165B4` |
| 83 | `0x800A1368` | `fn_80016660` | `0x80016660` | generic/unrecovered | `instruction=0x4BF752F9; direct_target=0x80016660` |
| 84 | `0x800A1370` | `fn_800649FC` | `0x800649FC` | generic/unrecovered | `instruction=0x4BFC368D; direct_target=0x800649FC` |
| 85 | `0x800A137C` | `fn_8010BC08` | `0x8010BC08` | generic/unrecovered | `instruction=0x4806A88D; direct_target=0x8010BC08` |
| 86 | `0x800A13C8` | `fn_8009FB78` | `0x8009FB78` | generic/unrecovered | `instruction=0x4BFFE7B1; direct_target=0x8009FB78` |
| 87 | `0x800A1420` | `fn_800A0474` | `0x800A0474` | generic/unrecovered | `instruction=0x4BFFF055; direct_target=0x800A0474` |
| 88 | `0x800A1428` | `fn_800A02A0` | `0x800A02A0` | generic/unrecovered | `instruction=0x4BFFEE79; direct_target=0x800A02A0` |
| 89 | `0x800A1430` | `fn_8011EC88` | `0x8011EC88` | generic/unrecovered | `instruction=0x4807D859; direct_target=0x8011EC88` |
| 90 | `0x800A1434` | `fn_8009E8B4` | `0x8009E8B4` | generic/unrecovered | `instruction=0x4BFFD481; direct_target=0x8009E8B4` |
| 91 | `0x800A143C` | `fn_800274BC` | `0x800274BC` | generic/unrecovered | `instruction=0x4BF86081; direct_target=0x800274BC` |
| 92 | `0x800A1448` | `fn_8007BF24` | `0x8007BF24` | generic/unrecovered | `instruction=0x4BFDAADD; direct_target=0x8007BF24` |
| 93 | `0x800A144C` | `fn_8013FB08` | `0x8013FB08` | generic/unrecovered | `instruction=0x4809E6BD; direct_target=0x8013FB08` |
| 94 | `0x800A145C` | `AXFXSetHooks` | `0x8002765C` | audio-named | `instruction=0x4BF86201; direct_target=0x8002765C` |
| 95 | `0x800A1460` | `fn_8009E7DC` | `0x8009E7DC` | generic/unrecovered | `instruction=0x4BFFD37D; direct_target=0x8009E7DC` |
| 96 | `0x800A1464` | `NuFileGetEndianSwap` | `0x800D8038` | Nu file/data-named | `instruction=0x48036BD5; direct_target=0x800D8038` |
| 97 | `0x800A1494` | `fn_8009DCE8` | `0x8009DCE8` | generic/unrecovered | `instruction=0x4BFFC855; direct_target=0x8009DCE8` |
| 98 | `0x800A1498` | `fn_80027564` | `0x80027564` | generic/unrecovered | `instruction=0x4BF860CD; direct_target=0x80027564` |
| 99 | `0x800A149C` | `fn_8011E968` | `0x8011E968` | generic/unrecovered | `instruction=0x4807D4CD; direct_target=0x8011E968` |
| 100 | `0x800A14B0` | `fn_80027D34` | `0x80027D34` | generic/unrecovered | `instruction=0x4BF86885; direct_target=0x80027D34` |
| 101 | `0x800A1520` | `fn_800166C4` | `0x800166C4` | generic/unrecovered | `instruction=0x4BF751A5; direct_target=0x800166C4` |
| 102 | `0x800A152C` | `fn_8009CFF8` | `0x8009CFF8` | generic/unrecovered | `instruction=0x4BFFBACD; direct_target=0x8009CFF8` |
| 103 | `0x800A154C` | `fn_8009D238` | `0x8009D238` | generic/unrecovered | `instruction=0x4BFFBCED; direct_target=0x8009D238` |
| 104 | `0x800A1554` | `NuRndrFootPrints` | `0x80025A70` | Nu rendering/texture-named | `instruction=0x4BF8451D; direct_target=0x80025A70` |
| 105 | `0x800A1614` | `DebugMenu_ToggleOption1` | `0x800AFEEC` | menu-named | `instruction=0x4800E8D9; direct_target=0x800AFEEC` |
| 106 | `0x800A1618` | `fn_8009E620` | `0x8009E620` | generic/unrecovered | `instruction=0x4BFFD009; direct_target=0x8009E620` |
| 107 | `0x800A165C` | `fn_8009E6FC` | `0x8009E6FC` | generic/unrecovered | `instruction=0x4BFFD0A1; direct_target=0x8009E6FC` |
| 108 | `0x800A1664` | `fn_800274BC` | `0x800274BC` | generic/unrecovered | `instruction=0x4BF85E59; direct_target=0x800274BC` |
| 109 | `0x800A167C` | `AXFXSetHooks` | `0x8002765C` | audio-named | `instruction=0x4BF85FE1; direct_target=0x8002765C` |
| 110 | `0x800A1680` | `fn_80027564` | `0x80027564` | generic/unrecovered | `instruction=0x4BF85EE5; direct_target=0x80027564` |
| 111 | `0x800A1688` | `NuRndrFootPrints` | `0x80025A70` | Nu rendering/texture-named | `instruction=0x4BF843E9; direct_target=0x80025A70` |
| 112 | `0x800A16A4` | `fn_8009CF2C` | `0x8009CF2C` | generic/unrecovered | `instruction=0x4BFFB889; direct_target=0x8009CF2C` |
| 113 | `0x800A16A8` | `fn_8010B660` | `0x8010B660` | generic/unrecovered | `instruction=0x48069FB9; direct_target=0x8010B660` |
| 114 | `0x800A16B0` | `fn_8011EC3C` | `0x8011EC3C` | generic/unrecovered | `instruction=0x4807D58D; direct_target=0x8011EC3C` |
| 115 | `0x800A16C0` | `fn_80044D8C` | `0x80044D8C` | generic/unrecovered | `instruction=0x4BFA36CD; direct_target=0x80044D8C` |
| 116 | `0x800A16C4` | `fn_80044D4C` | `0x80044D4C` | generic/unrecovered | `instruction=0x4BFA3689; direct_target=0x80044D4C` |
| 117 | `0x800A1700` | `fn_800C910C` | `0x800C910C` | generic/unrecovered | `instruction=0x48027A0D; direct_target=0x800C910C` |
| 118 | `0x800A1708` | `fn_800CCAFC` | `0x800CCAFC` | generic/unrecovered | `instruction=0x4802B3F5; direct_target=0x800CCAFC` |
| 119 | `0x800A1778` | `fn_8006E1EC` | `0x8006E1EC` | generic/unrecovered | `instruction=0x4BFCCA75; direct_target=0x8006E1EC` |
| 120 | `0x800A17AC` | `fn_800D9BB8` | `0x800D9BB8` | generic/unrecovered | `instruction=0x4803840D; direct_target=0x800D9BB8` |
| 121 | `0x800A17D8` | `fn_800A45BC` | `0x800A45BC` | generic/unrecovered | `instruction=0x48002DE5; direct_target=0x800A45BC` |
| 122 | `0x800A1818` | `fn_800A45BC` | `0x800A45BC` | generic/unrecovered | `instruction=0x48002DA5; direct_target=0x800A45BC` |
| 123 | `0x800A183C` | `fn_800A45BC` | `0x800A45BC` | generic/unrecovered | `instruction=0x48002D81; direct_target=0x800A45BC` |
| 124 | `0x800A18B4` | `fn_800AB2B0` | `0x800AB2B0` | generic/unrecovered | `instruction=0x480099FD; direct_target=0x800AB2B0` |
| 125 | `0x800A1954` | `fn_800BA87C` | `0x800BA87C` | generic/unrecovered | `instruction=0x48018F29; direct_target=0x800BA87C` |
| 126 | `0x800A1A84` | `fn_800AB428` | `0x800AB428` | generic/unrecovered | `instruction=0x480099A5; direct_target=0x800AB428` |
| 127 | `0x800A1A88` | `fn_800AB9F4` | `0x800AB9F4` | generic/unrecovered | `instruction=0x48009F6D; direct_target=0x800AB9F4` |
| 128 | `0x800A1A8C` | `fn_80044FD4` | `0x80044FD4` | generic/unrecovered | `instruction=0x4BFA3549; direct_target=0x80044FD4` |
| 129 | `0x800A1A90` | `fn_800D4DAC` | `0x800D4DAC` | generic/unrecovered | `instruction=0x4803331D; direct_target=0x800D4DAC` |
| 130 | `0x800A1AA4` | `NuTexAnimProgAssembleEnd` | `0x80007468` | Nu rendering/texture-named | `instruction=0x4BF659C5; direct_target=0x80007468` |
| 131 | `0x800A1AA8` | `Menu_ResetStack` | `0x80078E40` | menu-named | `instruction=0x4BFD7399; direct_target=0x80078E40` |
| 132 | `0x800A1B08` | `NuHGobjEvalAnim` | `0x800CAC5C` | Nu engine-named | `instruction=0x48029155; direct_target=0x800CAC5C` |
| 133 | `0x800A1B40` | `NuHGobjEvalAnim` | `0x800CAC5C` | Nu engine-named | `instruction=0x4802911D; direct_target=0x800CAC5C` |

## Position of `fn_8009CC00`

- It is direct callee **#1 of 133** at static callsite `0x800A0870`.
- No direct callee precedes it in this function's exported callsite layout.
- The remaining direct callee callsites are later in static layout. Their labels are: Nu engine-named (3), Nu file/data-named (1), Nu rendering/texture-named (10), audio-named (4), generic/unrecovered (111), menu-named (3).
- This does not prove that `fn_8009CC00` runs first at runtime or that every later static call is executed.

## Data/string references

- `0x801d2dd0	lbl_801D2DD0	Menu_FullReset	0x800a0940	addr	addi`
- `0x801d68e0	lbl_801D68E0	Menu_FullReset	0x800a0b0c	addr	addi`
- `0x801d68e0	lbl_801D68E0	Menu_FullReset	0x800a0c00	addr	addi`
- `0x801d68e0	lbl_801D68E0	Menu_FullReset	0x800a0c40	addr	addi`
- `0x801d68e0	lbl_801D68E0	Menu_FullReset	0x800a1718	addr	addi`
- `0x801d68e0	lbl_801D68E0	Menu_FullReset	0x800a1790	addr	addi`
- `0x801d68e0	lbl_801D68E0	Menu_FullReset	0x800a197c	addr	addi`
- `0x801d68e0	lbl_801D68E0	Menu_FullReset	0x800a197c	addr	addi`
- `0x801d68e0	lbl_801D68E0	Menu_FullReset	0x800a1acc	addr	addi`
- `0x803e04d0	lbl_803E04D0	Menu_FullReset	0x800a19c8	addr	addi`
- No string-reference row named `Menu_FullReset` was found in the inspected string-rename table.

## Nearby symbols

- `fn_8009FB78` (`0x8009FB78`), `fn_800A02A0` (`0x800A02A0`), `fn_800A039C` (`0x800A039C`), `fn_800A0474` (`0x800A0474`), `Menu_FullReset` (`0x800A0844`), `fn_800A1B50` (`0x800A1B50`), `fn_800A1BD4` (`0x800A1BD4`), `Menu_CleanupAndReset` (`0x800A1C18`), `fn_800A1C70` (`0x800A1C70`).

## Assessment

`Menu_FullReset` is a named symbol and directly calls other menu-named functions (`Menu_InitDefinitions`, `Menu_NavigateForward`, `Menu_ResetStack`) as well as Nu and audio-named functions. That is name and direct-edge evidence for a menu-related path, but it does not distinguish frontend reset, game reset, menu initialization, or level restart.

`fn_8009CC00` can be described only as a direct callee on this static `Menu_FullReset` path. A menu reset/init helper label is not accepted or justified without resolving its caller conditions, arguments, and generic callees.

## Confidence and limitations

- HIGH: direct edge rows and their PPC instruction evidence.
- MED: indexed data-xref rows copied below are existing reference evidence.
- LOW: menu-family interpretation from names.
- Direct edges omit indirect calls and do not prove runtime order, entrypoint status, or semantic role.
