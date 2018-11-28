; C-BIOS debug routines
; These routines should not be used in release builds of C-BIOS, but they can
; be useful for developers and testers.
;
; Copyright (c) 2004 Maarten ter Huurne.  All rights reserved.
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
; Print string to openMSX debug device as a separate line.
; Input:   HL = address of zero-terminated string
; Changes: HL, AF
print_debug:
                ld      a,$23
                out     (DBG_CTRL),a
                call    print_debug_asciiz
                ld      a,$00
                out     (DBG_CTRL),a
                ret

;--------------------------------
; Print string to openMSX debug device  as part of an existing line.
; Input:   HL = address of zero-terminated string
; Changes: HL, AF
print_debug_asciiz:
                ld      a,(hl)
                inc     hl
                or      a
                ret     z
                out     (DBG_DATA),a
                jr      print_debug_asciiz

;--------------------------------
; Print nibble in hexidecimal format as part of an existing line.
; Input:   A = nibble to print (bit 7-4 are ignored)
; Changes: AF
print_debug_hexnibble:
                and     $0F
                cp      10
                jr      nc,print_debug_hexnibble_letter
                add     a,'0'
                out     (DBG_DATA),a
                ret
print_debug_hexnibble_letter:
                add     a,'A' - 10
                out     (DBG_DATA),a
                ret

;--------------------------------
; Print byte in hexidecimal format as part of an existing line.
; Input:   A = byte to print
; Changes: AF
print_debug_hexbyte:
                push    af
                rrca
                rrca
                rrca
                rrca
                call    print_debug_hexnibble
                pop     af
                call    print_debug_hexnibble
                ret

;--------------------------------
; Print word in hexidecimal format as part of an existing line.
; Input:   HL = word to print
; Changes: HL, AF
print_debug_hexword:
                ld      a,h
                call    print_debug_hexbyte
                ld      a,l
                call    print_debug_hexbyte
                ret
