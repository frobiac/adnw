;(C)2005 wek   http://www.efton.sk
;free for personal use
;for commercial use, please contact wek@efton.sk

;implementation of XTEA algorithm by Needham&Wheeler for AVR
;

;6347 cycles excl. ret
;$67 words = $CE bytes = 206 bytes of code FLASH, including 16 bytes of key

;uses 8 bytes of SRAM (input/output data) and 10 registers

;key can be easily placed into SRAM - replace lpm xxx,Z+ by ld xxx,Z+
;for devices which don't support lpm xxx,Z+ replace it by lpm; mov xxx,r0
;the 4 registers used by sum0..sum3 can be spared by moving sum to SRAM with 3 modifications:
;  - when getting the pointer to key, load the appropriate byte from sum into tmp4
;  - the increment of sum should be done moving bytes from and to SRAM (e.g. in tmp4)
;  - when adding up sum to key, an additional register is necessary (so the spare is 3 registers only...)



.dseg

ydata:    .byte   4
zdata:    .byte   4


;register definitions
.def		ZL		= r30	
.def		ZH		= r31	

.def            tmp0            = r16
.def            tmp1            = r17
.def            tmp2            = r18
.def            tmp3            = r19
.def            tmp4            = r20

.def            sum0            = r21
.def            sum1            = r22
.def            sum2            = r23
.def            sum3            = r24

.def            RoundCnt        = r25

.cseg

Tea:
    ldi   RoundCnt,2*32
    clr   sum0
    clr   sum1
    clr   sum2
    clr   sum3
    ldi   ZL,LOW(zdata)
    ldi   ZH,HIGH(zdata)
    ldd   tmp0,Z+0
    ldd   tmp1,Z+1
    ldd   tmp2,Z+2
    ldd   tmp3,Z+3
TeaRound:
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

    lsl   tmp0  ;the last step for z<<4 and simultaneously the xor
    eor   tmp0,tmp1
    rol   tmp1
    eor   tmp1,tmp2
    rol   tmp2
    eor   tmp2,tmp3
    rol   tmp3
    eor   tmp3,tmp4

    ldd   tmp4,Z+0   ;now add z
    add   tmp0,tmp4
    ldd   tmp4,Z+1
    adc   tmp1,tmp4
    ldd   tmp4,Z+2
    adc   tmp2,tmp4
    ldd   tmp4,Z+3
    adc   tmp3,tmp4

    ldi   ZL,LOW(Key*2)    ;now calculate key pointer depending on which subround is it
    ldi   ZH,HIGH(Key*2)
    sbrc  RoundCnt,0
    rjmp  TeaRoundX1
    sbrc  sum0,0
    adiw  ZL,4
    sbrc  sum0,1
    adiw  ZL,8
    rjmp  TeaRoundX2
TeaRoundX1:
    sbrc  sum1,3              ;sum>>11 = (sum>>8)>>3
    adiw  ZL,4
    sbrc  sum1,4
    adiw  ZL,8
TeaRoundX2:
    
    lpm   tmp4,Z+          ;(tmp) xor (sum+k[ptr])
    add   tmp4,sum0
    eor   tmp0,tmp4
    lpm   tmp4,Z+
    adc   tmp4,sum1
    eor   tmp1,tmp4
    lpm   tmp4,Z+
    adc   tmp4,sum2
    eor   tmp2,tmp4
    lpm   tmp4,Z+
    adc   tmp4,sum3
    eor   tmp3,tmp4

    sbrc  RoundCnt,0      ;in the first subround increment sum (AVR: decrement by its neg)
    rjmp  TeaRoundX3
    subi  sum0,$47
    sbci  sum1,$86
    sbci  sum2,$C8
    sbci  sum3,$61
    ldi   ZL,LOW(ydata)   ;and set pointer to ydata to add the calculated value in tmp (and it remains valid for the next subround, too)
    ldi   ZH,HIGH(ydata)
    rjmp  TeaRoundX4
TeaRoundX3: 
    ldi   ZL,LOW(zdata)   ;in the second subround only set pointer to zdata
    ldi   ZH,HIGH(zdata)
TeaRoundX4: 

    ldd   tmp4,Z+0         ;add the calculated value to y or z
    add   tmp0,tmp4
    std   Z+0,tmp0    
    ldd   tmp4,Z+1
    adc   tmp1,tmp4
    std   Z+1,tmp1
    ldd   tmp4,Z+2
    adc   tmp2,tmp4
    std   Z+2,tmp2
    ldd   tmp4,Z+3
    adc   tmp3,tmp4
    std   Z+3,tmp3

    dec   RoundCnt
    brne  TeaRoundA
    ret    
TeaRoundA:
    rjmp  TeaRound


Key:
    .db   $4b,$5a,$69,$78
    .db   $0f,$1e,$2d,$3c
    .db   $c3,$d2,$e1,$f0
    .db   $87,$96,$a5,$b4
    
;   0x78695a4b 0x3c2d1e0f 0xf0e1d2c3 0xb4a59687 