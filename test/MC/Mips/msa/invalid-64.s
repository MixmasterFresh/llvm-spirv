# Instructions that are invalid
#
# RUN: not llvm-mc %s -triple=mips-unknown-linux -mcpu=mips64r2 -mattr=+msa \
# RUN:     -show-encoding 2>%t1
# RUN: FileCheck %s < %t1

    .set noat
    dlsa    $2, $3, $4, 0   # CHECK: :[[@LINE]]:25: error: expected immediate in range 1 .. 4
    dlsa    $2, $3, $4, 5   # CHECK: :[[@LINE]]:25: error: expected immediate in range 1 .. 4
    insve.b $w25[3], $w9[1] # CHECK: :[[@LINE]]:26: error: expected '0'
    insve.h $w24[2], $w2[1] # CHECK: :[[@LINE]]:26: error: expected '0'
    insve.w $w0[2], $w13[1] # CHECK: :[[@LINE]]:26: error: expected '0'
    insve.d $w3[0], $w18[1] # CHECK: :[[@LINE]]:26: error: expected '0'
    lsa     $2, $3, $4, 0   # CHECK: :[[@LINE]]:25: error: expected immediate in range 1 .. 4
    lsa     $2, $3, $4, 5   # CHECK: :[[@LINE]]:25: error: expected immediate in range 1 .. 4
    sat_s.b $w31, $w31, -1  # CHECK: :[[@LINE]]:25: error: expected 3-bit unsigned immediate
    sat_s.b $w31, $w31, 8   # CHECK: :[[@LINE]]:25: error: expected 3-bit unsigned immediate
    sat_u.b $w31, $w31, -1  # CHECK: :[[@LINE]]:25: error: expected 3-bit unsigned immediate
    sat_u.b $w31, $w31, 8   # CHECK: :[[@LINE]]:25: error: expected 3-bit unsigned immediate
    srari.b $w5, $w25, -1   # CHECK: :[[@LINE]]:24: error: expected 3-bit unsigned immediate
    srari.b $w5, $w25, 8    # CHECK: :[[@LINE]]:24: error: expected 3-bit unsigned immediate
    srlri.b $w18, $w3, -1   # CHECK: :[[@LINE]]:24: error: expected 3-bit unsigned immediate
    srlri.b $w18, $w3, 8    # CHECK: :[[@LINE]]:24: error: expected 3-bit unsigned immediate
