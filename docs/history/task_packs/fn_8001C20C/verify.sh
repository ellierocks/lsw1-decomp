#!/usr/bin/env bash
# Verification for fn_8001C20C (0x8001C20C)
#
# Steps:
#   1. Write matching C in the appropriate src/<module>/fn_8001C20C.c
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
#          --target build/GL5E4F/obj/nuanim/fn_8001C20C.o \
#          --symbol fn_8001C20C \
#          --output /tmp/fn_8001C20C_diff.png
#
#   5. Screenshot the objdiff UI showing 100% match and save to:
#      tasks/fn_8001C20C/match_screenshot.png
#
# Troubleshooting:
#   - Stack mismatch? Check prologue stwu / epilogue addi sp
#   - Instruction mismatch? Verify register assignment matches
#   - Size mismatch? Check for extra padding or missing code
#
echo "Run: ninja build/GL5E4F/report.json"
