; C-BASIC statements
;
; Copyright (c) 2005 BouKiCHi.  All rights reserved.
;
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
;

; part of ROM Basic
                ds      $3193 - $

;-----------------------------
;$3193 - Multiply
; In  : HL,DE
; Out : HL and DAC
; Regs : unknown
; Note : this routine is not correctly yet
multiple:
                push    bc
                push    hl
                pop     bc
multiple_lp:
                dec     de
                ld      a,e
                or      d
                jr      z,multiple_fin
                add     hl,bc
                jr      multiple_lp
multiple_fin:

                pop     bc
                ret


rombas:
                push    hl
                push    af
                ld      hl,rombas_text
                call    print_debug
                ld      a,$20
                out     ($2e),a
                ld      a,d
                out     ($2f),a
                ld      a,e
                out     ($2f),a
                pop     af
                pop     hl
                ret
rombas_text:    db      "ROMBAS",0

                ds      $392e - $
; runloop table

                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy

                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy

                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy

                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy

                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy

                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy

; statement table
                ds      $39de - $
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy

                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy

                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy
                dw      rombas_niy,rombas_niy,rombas_niy

rombas_niy:
                push    hl
                push    af
                ld      hl,rombas_niy_text
                call    print_debug
                pop     af
                pop     hl
                ret

rombas_niy_text:    db      "BASIC statements are not implemented yet",0


