; INLIN/PINLIN/QINLIN routines for C-BIOS
;
; Copyright (c) 2007 Eric Boon.  All rights reserved.
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
;--------------------------------
; $00AE PINLIN
; Function : Stores in the specified buffer the character codes input
;           until the return key or STOP key is pressed
; Output   : HL - for the starting address of the buffer -1
;            C-flag set when it ends with the STOP key
; Registers: All
; TODO: call H_PINL
pinlin:
                call    H_PINL
                ld      a,(AUTFLG)              ; If AUTO is active
                and     a
                jp      z,inlin                 ; then start line input
                ld      a,1                     ; else set cursor
                ld      (CSRX),a                ;    to left border first
                jp      inlin                   ;    and then start line input

;--------------------------------
; $00B4 QINLIN
; Function : Prints a questionmark and one space and continues with INLIN
; Output   : HL - for the starting address of the buffer -1
;            C-flag set when it ends with the STOP key
; Registers: All
qinlin_prompt:
                db      "? ",0
qinlin:
                call    H_QINL
                ld      hl,qinlin_prompt
                call    prn_text
                ; continue with inlin

;--------------------------------
; $00B1 INLIN
; Function : Main line input routine
; Output   : HL - for the starting address of the buffer -1
;            C-flag set when it ends with the STOP key
; Registers: All

inlin:
                ld      hl,(CSRX)               ; loads CSRX and CSRY
                ld      (FSTPOS),hl             ; save in FSTPOS

                ld      de,LINTTB-2             ; break logical line
                ld      h,0                     ; above cursor pos
                ld      a,l
                add     hl,de
                ld      (hl),a
inlin_loop:
                call    chget                   ; get a character from the kbd

                cp      $7F
                jp      z,inlin_del
                cp      $20
                jr      nc,inlin_printable

                ld      b,20
                ld      hl,inlin_table
                call    jump_table
                
                xor     a                       ; we just put out a ctrl char
                ld      (INSFLG),a              ; switch insert mode off
                ld      (CSTYLE),a
                jr      inlin_loop

inlin_printable:                                ; else...
                push    af
                ld      a,(INSFLG)
                and     a
                call    nz,inlin_insert
                pop     af
                rst     $18
                jr      inlin
; ----------------------------------------------
inlin_insert:
                call    chput_remove_cursor
                ld      hl,(CSRY)               ; save cursorpos
                ld      (TEMP2),hl

                ld      a,' '                   ; oldchar = space
                ld      (TEMP),a

inlin_insert_loop:                              ; REPEAT
                call    curs2hl                 ;   get char under curpos
                call    rdvrm

                cp      ' '                     ;   IF is space
                jr      nz,inlin_insert_cont

                ld      hl,(CSRY)               ;   AND at end of line
                ld      a,(LINLEN)
                cp      h
                jr      nz,inlin_insert_cont1

                ld      h,0                     ;   AND logical line does
                ld      de,LINTTB-1             ;     not continue
                add     hl,de
                ld      a,(hl)
                or      a
                jr      z,inlin_insert_cont1

                ld      a,(TEMP)                ;   THEN
                call    curs2hl
                call    wrtvrm                  ;     put old char
                ld      hl,(TEMP2)              ;     restore cursor pos
                ld      (CSRY),hl
                ret
                jp      chput_restore_cursor    ;     and exit

inlin_insert_cont1:
                ld      a,' '
inlin_insert_cont:
                push    af                      ;   ELSE
                ld      a,(TEMP)                ;     put old char
                rst     $18
                pop     af
                ld      (TEMP),a                ;   oldchar = character read
                jr      inlin_insert_loop       ; ENDREP

; ----------------------------------------------
inlin_wback:
                ret

; ----------------------------------------------
inlin_break:
                scf                             ; C
                pop     hl                      ; do not return to INLIN
                ret                             ; but to caller of INLIN

; ----------------------------------------------
inlin_clear:
                ret

; ----------------------------------------------
inlin_wfwd:
                ret

; ----------------------------------------------
inlin_bs:
                ret

; ----------------------------------------------
inlin_cr:
                ret

; ----------------------------------------------
inlin_end:
                xor     a                       ; NZ, NC
                pop     hl                      ; do not return to INLIN
                ret                             ; but to caller of INLIN

; ----------------------------------------------
inlin_ins:
                ret

; ----------------------------------------------
inlin_clrlin:
                ret

; -- ESCAPE
inlin_esc:
                ret                             ; Do nothing

; -- DELETE
inlin_del:
                ret

; -- Jump table. Control chars not handled in one of the routines above
;    are simply forwarded to OUTDO
inlin_table:
                dw      $0018                   ; @
                dw      $0018                   ; A - 
                dw      inlin_wback             ; B word back
                dw      inlin_break             ; C stop, abort, quit
                dw      $0018                   ; D
                dw      inlin_clear             ; E: clear to end of line
                dw      inlin_wfwd              ; F: word fwd
                dw      $0018                   ; G
                dw      inlin_bs                ; H BACKSP: erase char left
                dw      $0018                   ; I
                dw      $0018                   ; J
                dw      $0018                   ; K
                dw      $0018                   ; L
                dw      inlin_cr                ; M ENTER : confirm, yes, ok
                dw      inlin_end               ; N to end of line
                dw      $0018                   ; O
                dw      $0018                   ; P
                dw      $0018                   ; Q
                dw      inlin_ins               ; R INSERT: toggle insert mode
                dw      $0018                   ; S
                dw      $0018                   ; T
                dw      inlin_clrlin            ; U clear line
                dw      $0018                   ; V
                dw      $0018                   ; W
                dw      $0018                   ; X
                dw      $0018                   ; Y
                dw      $0018                   ; Z
                dw      inlin_esc               ; ESCAPE: ignore
                dw      $0018                   ; (28)
                dw      $0018                   ; (29)
                dw      $0018                   ; (30)
                dw      $0018                   ; (31)

; vim:ts=8:expandtab:filetype=z8a:syntax=z8a:
