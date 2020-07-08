#!/bin/sh

# ========================================
# Build Unix
# ========================================

# $1 must be the target, so either Rouage or RouageServer
/usr/local/bin/scons --jobs=2 --dbg=1 $1