; C-BASIC(minibas) main ROM
;
; Copyright (c) 2005 BouKiCHi.  All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions
; are met:
; 1. Redistributions of source code must retain the above copyright
;    notice, this list of conditions and the following disclaimer.
; 2. Redistributions in binary form must reproduce the above copyright
;    notice, this list of conditions and the following disclaimer in the
;    documentation and/or other materials provided with the distribution.
;
; THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
; IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
; OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
; IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
; INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
; NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
; DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
; THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
; THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;

CHGMOD:         equ     $005f
CHPUT:          equ     $00A2
CHGET:          equ     $009F
ENASLT:         equ     $0024

EXPTBL:         equ     $FCC1
RSLREG:         equ     $0138

WORK:           equ     $E000
MEMPTR:         equ     WORK
INCMD:          equ     WORK+$100
MEMMGR:         equ     WORK+$800

;------------------------
; Memory structure info
; MEMMGR+0h : type id
; MEMMGR+1h : a pointer to string
; MEMMGR+3h : an address of value

; -----------------------
; ROM Header
                org     $4000
                db      "AB"
                dw      bas_main
                dw      $0000
                dw      $0000
                dw      $0000
                dw      $0000
                dw      $0000
                dw      $0000

; The start point of the code

bas_main:
                ld      a,1
                call    CHGMOD          ; change to screen 1
                call    make_ramslot    ; make a value to RAM

                ld      hl,$8000
                call    ENASLT

; form here : Page0 = BIOS ,Page 1 = THIS, Page 2,3 = RAM

                call    init_bas

                ld      hl,str_startmsg
                call    prn_text

                ld      hl,str_lf
                call    prn_text

                jp      bas_loop

;------------------------
; init_bas
; The routine initialises all parameters
; Changes : ALL
init_bas:
                ld      hl,MEMMGR
                ld      (MEMPTR),hl
                ld      a,$80
                ld      (hl),a

                ld      hl,bas_code
                ld      de,$8000
                ld      bc,bas_code_end - bas_code
                ldir                    ; transmit to ram

                ret

;------------------------
; bas_loop
; The routine does loop mainly
; Changes : ALL
bas_loop:
                ld      hl,str_okprompt
                call    prn_text
                ld      hl,INCMD
                call    get_string
                ld      hl,INCMD
                call    strupr
                ld      hl,INCMD
                call    parser
                jr      bas_loop

;------------------------
; get_string
; The routine is for getting string a line
; Changes : AF,HL
;
get_string:
                ld      b,$00
gs_lp:
                call    CHGET
                cp      $08
                jr      z,gs_get_back
                cp      $0d
                jr      z,gs_end

                call    CHPUT
                ld      (hl),a
                inc     hl
                inc     b
                jr      gs_lp
gs_end:
                call    CHPUT
                ld      a,$0a
                call    CHPUT

                xor     a
                ld      (hl),a
                ret
gs_get_back:
                xor     a
                cp      b
                jr      z,gs_lp
                dec     hl
                dec     b
                ld      a,$08
                call    CHPUT
                ld      a,$20
                call    CHPUT
                ld      a,$08
                call    CHPUT
                jr      gs_lp


;-----------------------
; parser
; The routine is parser of the BASIC
; Changes: unknown
parser:
                call    isdigit
                jr      z,single_cmd
                ld      hl,str_linecmd
                call    prn_text
                ret
single_cmd:
                call    search_cmd
                ret

;-----------------------
; search_cmd
; The routine searches command to execute and executes actually
; Changes: unknown

search_cmd:
                ld      hl,data_icmd
                ld      b,$00
scmd_lp:
                ld      de,INCMD
                ld      a,(hl)
                cp      $00
                jr      nz,scmd_chk
                inc     hl
                ld      a,(hl)
                cp      $00
                ret     z
                dec     hl
scmd_chk:
                call    scmd_get_sl
                jr      z,scmd_eql
                inc     hl
                inc     hl
                inc     b
                jr      scmd_lp
scmd_eql:
                ld      hl,data_cmdexe
                xor     a
                or      b
                jr      z,scmd_jmp
scmd_eql_lp:
                inc     hl
                inc     hl
                dec     b
                jr      nz,scmd_eql_lp
scmd_jmp:
                ld      a,(hl)
                inc     hl
                ld      h,(hl)
                ld      l,a
                push    hl
                ret

scmd_get_sl:
                push    hl
                ld      a,(hl)
                inc     hl
                ld      h,(hl)
                ld      l,a
                call    strcmp
                pop     hl
                ret


;-----------------------
; strcmp
; Compares string HL with DE until HL is NUL
; Out : ZF = 1 when it is equal
; Changes: AF,HL,DE

strcmp:
                xor  a
                cp   (hl)
                jr   z,scmp_eq
                ld   a,(de)
                cp   (hl)
                jr   nz,scmp_neq
                cp   $00
                ret  z
                inc  hl
                inc  de
                jr   strcmp
scmp_neq:
                ld      a,$ff
                or      a
scmp_eq:
                ret

;-----------------------
; ifasval
; Checks the input is action of assigning the value
; In  : DE = head of input string , HL = the name of the value
; Out : AF = operand or token, AF == 0xff when it failed
; Changes: AF,HL,DE

ifasval:
                xor  a
                cp   (hl)
                jr   z,ifas_eq
                ld   a,(de)

                cp   '='
                jr   z,ifas_eq
                cp   '$'
                jr   z,ifas_eq

                cp   (hl)
                jr   nz,scmp_neq
                cp   $00
                ret  z
                inc  hl
                inc  de
                jr   ifasval
ifas_neq:
                ld      a,$ff
                or      a
ifas_eq:
                ret

;-----------------------
; disp_niy
; The routine says "not implemented yet"
; Changes: unknown
disp_niy:
                ld      hl,str_niy
                call    prn_text
                ret

;-----------------------
; disp_list
; The routine displays BASIC list
; Changes: unknown
disp_list:
                ld      ix,$8001
                call    list_bas
                ret



;-----------------------
; isdigit
; Checks digit or not
; In : HL = pointer to character to check
; Out: ZF = 1 when (HL) is digit
; Changes: unknown
isdigit:
                ld      a,(HL)
                cp      '0'
                jr      z,isdigit_true
                jr      c,isdigit_false
                cp      '9'
                jr      nc,isdigit_false
isdigit_true:
                ld      a,$ff
                or      a
                ret
isdigit_false:
                xor     a
                ret

;-----------------------
; strupr
; Changes string to upper case
; In : HL = a pointer to string
; Changes: unknown
strupr:
                ld      a,(HL)
                cp      'a'
                jr      z,supr_toupr
                jr      c,supr_skip
                cp      'z'
                jr      nc,supr_skip
supr_toupr:
                sub     $20
                ld      (HL),a
supr_skip:
                cp      $00
                ret     z
                inc     hl
                jr      strupr

;-----------------------
; get_value
; The routine gets a value from memory managed area
; In : HL = a pointer of string
; Out: DE = a pointer to value, status = A
; Changes : Unknown

get_value:
                ld      de,MEMMGR
loop_get_value:
                ld      a,(de)
                cp      $80
                ret     z

                inc     de
                push    hl
                push    de
                ex      de,hl
                ld      a,(hl)
                inc     hl
                ld      h,(hl)
                ld      l,a
                call    str_compr
                jr      z,read_value
                pop     de
                pop     hl
                inc     de
                inc     de
                inc     de
                inc     de
                jr      loop_get_value
read_value:
                pop     hl ; a simple trick
                pop     de
                ld      a,(hl)
                inc     hl
                ld      h,(hl)
                ld      l,a
                xor     a
                ret


;-----------------------
; str_compr
; The routine compares string HL with DE
; The strings have to be terminated by NUL($00)
; In : HL,DE = string
; Out: Z -> the same : NZ -> not the same
; Changes : HL,DE
str_compr:
                ld      a,(de)
                cp      (hl)
                ret     nz
                cp      $00
                ret     z
                inc     hl
                inc     de
                jr      str_compr


;------------------------
; list_bas
; for displaying internal code as a list
; Changes : ALL

list_bas:
                ld      b,(ix) ; ptr of next_line
                inc     ix
                ld      c,(ix)
                inc     ix
                ld      a,b
                or      c
                ret     z
line_start:
                call    read_num        ; line number
                call    disp_dec_nz
                ld      a,' '
                call    CHPUT

line_loop:
        ld      a,(ix)
                inc     ix
                cp      $20
                jp      nc,put_code
; special code (A < $20)
                cp      $11
                jp      nc,disp_num1
                cp      $0f
                jp      z,disp_num2
                cp      $00
                jp      z,line_end

                jr      line_loop

put_code:
                ld      d,a
                ld      c,a
                ld      b,0
                ld      hl,table_code
                add     hl,bc
                add     hl,bc
                ld      a,(hl)
                inc     hl
                ld      h,(hl)
                ld      l,a

                or      h
                jr      nz,prn_code
                ld      a,d
                call    CHPUT
                jp      line_loop
prn_code:

                call    prn_text
                jp      line_loop

disp_num1:
                add     a,'0'-$11
                call    CHPUT
                jp      line_loop
disp_num2:
                call    read_num
                call    disp_dec_nz
                jp      line_loop

line_end:
                ld      hl,str_lf
                call    prn_text
                jp      list_bas


read_num:
                ld      l,(ix)
                inc     ix
                ld      h,(ix)
                inc     ix
                ret


;-----------------------------
dispdec_lf:
                ld      d,0
                call    disp_dec
                ld      hl,str_lf
                call    prn_text
                ret

;---------------------------
;disp_dec_nz
; The routine displays decimal without zero
; This is a wrapper of disp_dec

disp_dec_nz:
                ld      d,0

;---------------------------
; display decimal
; The routine displays decimal
; In : HL = value , D  = zero flag
; Changes: unknown
disp_dec:
                ld      bc,$d8f0 ; -10000
                call    nega_hl
                call    put_dec
                ld      bc,$fc18 ; -1000
                call    nega_hl
                call    put_dec

                ld      bc,$ff9c ; -100
                call    nega_hl
                call    put_dec

                ld      bc,$fff6 ; -10
                call    nega_hl
                call    put_dec

                ld      a,l
                add     a,'0'
                call    CHPUT
                ret

put_dec:
                or      a
                jr      z,skip_flag
                ld      d,1
skip_flag:
                bit     0,d
                ret     z
                add     a,'0'
                jp      CHPUT




nega_hl:
                xor     a
nega_lp:
                add     hl,bc
                inc     a
                jr      c,nega_lp
                sbc     hl,bc
                dec     a
                ret


;table_dec_neg
;                dw      $d8f0 ; -10000
;                dw      $fc18 ; -1000
;                dw      $ff9c ; -100
;                dw      $fff6 ; -10


;------------------------
;make_ramslot
;The routine makes a value of RAM slot

make_ramslot:
                call    RSLREG
                rlca
                rlca
                and     $03
                ld      c,a
                ld      b,0
                ld      ix,EXPTBL
                add     ix,bc
                ld      e,a
                ld      a,(ix)
                and     $80
                jr      z,no_exp
                or      e
                ld      e,a
                inc     ix
                inc     ix
                inc     ix
                inc     ix
                ld      a,(ix)
                rrca
                rrca
                rrca
                rrca
                and     $0c
                or      e
                ret
no_exp:
                ld      a,e
                ret

;------------------------
prn_text:
prn_str_disp:
                ld      a,(hl)
                or      a
                jp      z,nul_term
                call    CHPUT
                inc     hl
                jr      prn_str_disp
nul_term:
                ret


; internal code table
                include "basic_tables.asm"

str_startmsg:
                db      "C-BASIC ver 0.02 (050607)",$0d,$0a
                db      "Copyright (C) BouKiCHi",$0d,$0a,$00

str_okprompt:
                db      "Ok",$0d,$0a,$00

str_linecmd:
                db      "recognized Line num",$0d,$0a,$00


str_niy:
                db      "Not implemented yet",$0d,$0a,$00

str_lf:
                db      $0d,$0a,$00

; Internal command table
data_icmd:
                dw      icmd_list
                dw      icmd_vlist
                dw      $0000

data_cmdexe:
                dw      disp_list
                dw      disp_niy
                dw      $0000


;---------------------------
; list of internal commands
icmd_list:
                db      "LIST",$00
icmd_vlist:
                db      "VLIST",$00


;---------------------------
; test.bas(for test)
;
bas_code:
                incbin  "test.bas"
bas_code_end:

;end
        ds $8000-$

