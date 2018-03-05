
;(C)2005 wek   http://www.efton.sk
;free for personal use
;for commercial use, please contact wek@efton.sk

;implementation of original TEA algorithm by Needham&Wheeler for AVR
;

;uses 8 bytes of SRAM (input/output data) and 14 registers
;!!! uses reordered key !!!
;;!!! reordered key !!! k[1]k[0]k[3]k[2]


;6861 cycles
;5E=94 words = 188 bytes, excl. key
;no stack


;key can be easily placed into SRAM - replace lpm xxx,Z+ by ld xxx,Z+
;for devices which don't support lpm xxx,Z+ replace it by lpm; mov xxx,r0


.dseg

ydata:    .byte   4
zdata:    .byte   4

;register definitions
.def		ZL		= r30	
.def		ZH		= r31	
.def        YL      = r28
.def        YH      = r29


.def            tmp0            = r1
.def            tmp1            = r2
.def            tmp2            = r3
.def            tmp3            = r4
.def            tmp4            = r5
.def            tmp5            = r6
.def            tmp6            = r7
.def            tmp7            = r8
.def            tmp8            = r9

.def            sum0            = r16 
.def            sum1            = r17
.def            sum2            = r18
.def            sum3            = r19

.def            RoundCnt        = r20

.cseg

Tea:
    ldi   RoundCnt,2*32
    clr   sum0
    clr   sum1
    clr   sum2
    clr   sum3
    ldi   YL,LOW(zdata)
    ldi   YH,HIGH(zdata)
    ldd   tmp0,Y+0
    ldd   tmp1,Y+1
    ldd   tmp2,Y+2
    ldd   tmp3,Y+3
TeaRound:
    ldi   ZL,LOW(Key*2)
    ldi   ZH,HIGH(Key*2)
    subi  sum0,$47
    sbci  sum1,$86
    sbci  sum2,$C8
    sbci  sum3,$61
TeaSubRound:
    clr   tmp4    ;z>>5 but as (z<<3)>>8 (simultaneously preparing for z<<4)
    lsl   tmp0
    rol   tmp1
    rol   tmp2
    rol   tmp3
    rol   tmp4
    lsl   tmp0
    rol   tmp1
    rol   tmp2
    rol   tmp3
    rol   tmp4
    lsl   tmp0
    rol   tmp1
    rol   tmp2
    rol   tmp3
    rol   tmp4

    lpm   tmp5,Z+   ;!!! reordered key !!! k[1]k[0]k[3]k[2]
    add   tmp5,tmp1   ;z>>5 + key[1]
    lpm   tmp6,Z+
    adc   tmp6,tmp2
    lpm   tmp7,Z+
    adc   tmp7,tmp3
    lpm   tmp8,Z+
    adc   tmp8,tmp4

    lsl   tmp0      ;last shift for z<<4
    rol   tmp1
    rol   tmp2
    rol   tmp3
    lpm   tmp4,Z+   ;z<<4 + key[0]
    add   tmp0,tmp4
    lpm   tmp4,Z+  
    adc   tmp1,tmp4
    lpm   tmp4,Z+  
    adc   tmp2,tmp4
    lpm   tmp4,Z+  
    adc   tmp3,tmp4

    eor   tmp5,tmp0  ;xor the first two parts
    eor   tmp6,tmp1
    eor   tmp7,tmp2
    eor   tmp8,tmp3

    ldd   tmp0,Y+0   ;z+sum
    ldd   tmp1,Y+1
    ldd   tmp2,Y+2
    ldd   tmp3,Y+3
    add   tmp0,sum0
    adc   tmp1,sum1
    adc   tmp2,sum2
    adc   tmp3,sum3
    
    eor   tmp0,tmp5
    eor   tmp1,tmp6
    eor   tmp2,tmp7
    eor   tmp3,tmp8

    sbrc  RoundCnt,0      ;in the first subround increment sum (AVR: decrement by its neg)
    rjmp  TeaRoundX3
    ldi   YL,LOW(ydata)   ;and set pointer to ydata to add the calculated value in tmp (and it remains valid for the next subround, too)
    ldi   YH,HIGH(ydata)
    rjmp  TeaRoundX4
TeaRoundX3: 
    ldi   YL,LOW(zdata)   ;in the second subround only set pointer to zdata
    ldi   YH,HIGH(zdata)
TeaRoundX4: 

    ldd   tmp4,Y+0         ;add the calculated value to y or z
    add   tmp0,tmp4
    std   Y+0,tmp0    
    ldd   tmp4,Y+1
    adc   tmp1,tmp4
    std   Y+1,tmp1
    ldd   tmp4,Y+2
    adc   tmp2,tmp4
    std   Y+2,tmp2
    ldd   tmp4,Y+3
    adc   tmp3,tmp4
    std   Y+3,tmp3

    dec   RoundCnt
    brne  TeaRoundA
    ret    
TeaRoundA:
    sbrs  RoundCnt,0
    rjmp  TeaRound
    rjmp  TeaSubRound



Key:
;     .db   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

    .db   $0f,$1e,$2d,$3c
    .db   $4b,$5a,$69,$78
    .db   $87,$96,$a5,$b4
    .db   $c3,$d2,$e1,$f0
   