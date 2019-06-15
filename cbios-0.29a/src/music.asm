; C-BIOS MSX-MUSIC ROM
;
; Copyright (c) 2004 Albert Beevendorp.  All rights reserved.
; Copyright (c) 2004 BouKiCHi.  All rights reserved.
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

                org     $4000

                db      "AB"
                dw      0               ; init
                dw      0               ; statement
                dw      0               ; device
                dw      0               ; basic text
                dw      0,0,0

                ds      8,0
                db      "APRLOPLL"


;--------------------------------
; FM BIOS

; $4110 WRTOPL
                ds      $4110 - $
                jp      wrtopl

; $4113 INIOPL
                ds      $4113 - $
                jp      iniopl

; $4116 MSTART
                ds      $4116 - $
                jp      mstart

; $4119 MSTOP
                ds      $4119 - $
                jp      mstop

; $411C RDDATA
                ds      $411C - $
                jp      rddata

; $411F OPLDRV
                ds      $411F - $
                jp      opldrv

; $4122 TSTBGM
                ds      $4122 - $
                jp      tstbgm

;--------------------------------
; $4110 WRTOPL
; Writes data to OPLL register.
wrtopl:
                push    hl
                push    af
                ld      hl,wrtopl_text
                call    print_debug
                pop     af
                pop     hl
                ret
wrtopl_text:    db      "music: WRTOPL ($4410) called",0

;--------------------------------
; $4113 INIOPL
iniopl:
                push    hl
                push    af
                ld      hl,iniopl_text
                call    print_debug
                pop     af
                pop     hl
                ret
iniopl_text:    db      "music: INIOPL ($4113) called",0

;--------------------------------
; $4116 MSTART
mstart:
                push    hl
                push    af
                ld      hl,mstart_text
                call    print_debug
                pop     af
                pop     hl
                ret
mstart_text:    db      "music: MSTART ($4116) called",0

;--------------------------------
; $4119 MSTOP
mstop:
                push    hl
                push    af
                ld      hl,mstop_text
                call    print_debug
                pop     af
                pop     hl
                ret
mstop_text:     db      "music: MSTOP ($4119) called",0

;--------------------------------
; $411C RDDATA
rddata:
                push    hl
                push    af
                ld      hl,rddata_text
                call    print_debug
                pop     af
                pop     hl
                ret
rddata_text:    db      "music: RDDATA ($411C) called",0

;--------------------------------
; $411F OPLDRV
opldrv:
                push    hl
                push    af
                ld      hl,opldrv_text
                call    print_debug
                pop     af
                pop     hl
                ret
opldrv_text:    db      "music: OPLDRV ($411F) called",0

;--------------------------------
; $4122 TSTBGM
tstbgm:
                push    hl
                push    af
                ld      hl,tstbgm_text
                call    print_debug
                pop     af
                pop     hl
                ret
tstbgm_text:    db      "music: TSTBGM ($4122) called",0

;--------------------------------

                include "hardware.asm"
                include "debug.asm"

;--------------------------------
                ds      $8000 - $,$FF

