#!/usr/bin/env bash
# Verification for fn_800203AC (0x800203AC)
#
# Steps:
#   1. Write matching C in the appropriate src/<module>/fn_800203AC.c
#   2. Add the object to configure.py under the nuanim module
#   3. Reconfigure and build:
#
#        python3 configure.py && ninja
#
#   4. Check the objdiff report:
#
#        ninja build/GL5E4F/report.json
#        objdiff-cli report generate --config objdiff.json
#
#      Open build/GL5E4F/report.html in a browser for the visual diff,
#      or run objdiff-cli directly:
#
#        objdiff-cli diff \
#          --base build/GL5E4F/obj/auto_01_800034A0_text.o \
#          --target build/GL5E4F/obj/nuanim/fn_800203AC.o \
#          --symbol fn_800203AC \
#          --output /tmp/fn_800203AC_diff.png
#
#   5. Screenshot the objdiff UI showing 100% match and save to:
#      tasks/fn_800203AC/match_screenshot.png
#
# Troubleshooting:
#   - Stack mismatch? Check prologue stwu / epilogue addi sp
#   - Instruction mismatch? Verify register assignment matches
#   - Size mismatch? Check for extra padding or missing code
#
echo "Run: ninja build/GL5E4F/report.json"
