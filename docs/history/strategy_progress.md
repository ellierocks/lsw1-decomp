# Strategy progress log (archived)

Per-import progress figures moved out of [strategy](../strategy.md). The current totals are in the
README status block and [SDK import](../sdk_import.md).

## Phase 1 import log

The [first import](sdk_import.md) promotes nine units (six GX, two OS and one
DSP): 12,284 code bytes and 75 functions. Matching progress rises from 2,044
bytes / 23 functions to 14,328 bytes / 98 functions. The existing ProDG linker
now produces a whole image identical to retail, eliminating the baseline's
1,192 differing bytes. The subsequent complete GXAttr import adds 5,044 code
bytes, 640 data bytes and 14 functions: ten SDK units now contribute 17,328
code bytes / 89 functions, and total matching is 19,372 code bytes / 112
functions with 2,084 data bytes. The whole-image SHA-1 remains identical to
retail. The MWCC flags, dead stripping and complete-section verification are
wired. Complete GXLight adds 1,280 code bytes / nine functions and 96 data
bytes: eleven SDK units contribute 18,608 code bytes / 98 functions / 2,144
data bytes, and total matching reaches 20,652 code bytes / 121 functions /
2,180 data bytes. Complete GXPixel adds 1,592 code bytes / 12 functions and 128
data bytes: twelve SDK units contribute 20,200 code bytes / 110 functions / 2,272
data bytes, and total matching reaches 22,244 code bytes / 133 functions / 2,308
data bytes. Complete CARDStat adds 876 code bytes / two functions and no data.
Complete GXFrameBuf adds 2,728 code bytes / 14 functions and proves 424 data
bytes (objdiff credits 16 matched data bytes; see the evidence). Together,
fourteen SDK units contribute 23,804 code bytes / 126 functions / 2,696 complete
data bytes; total matching reaches 25,848 code bytes / 149 functions / 2,324
matched data bytes. Complete OSMemory adds 728 code bytes / six functions and
16 data bytes: fifteen SDK units contribute 24,532 code bytes / 132 functions /
2,712 data bytes, and total matching reaches 26,576 code bytes / 155 functions /
2,340 matched data bytes. The whole image remains identical to retail; remaining
archives and version drift still need work. OSCache is now
promoted as the sixteenth unit (`verify_unit.py` accepts a short all-zero
alignment tail), and the probe's include-path and DTK local-name faults are
fixed; see [SDK import](sdk_import.md).
