; Scan code tables US International keyboard for C-BIOS
;
; Copyright (c) 2008 Eric Boon.  All rights reserved.
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
; -------------------------------------
; scan code tables
scode_tbl:
                db      "01234567"                      ;00
                db      "89-=",$5C,"[];"                ;01
                db      "'`,./",$00,"ab"                ;02
                db      "cdefghij"                      ;03
                db      "klmnopqr"                      ;04
                db      "stuvwxyz"                      ;05

scode_tbl_shift:
                db      ")!@#$%^&"                      ;00
                db      "*(_+|{}:"                      ;01
                db      $22,"~<>?",$00,"AB"             ;02
                db      "CDEFGHIJ"                      ;03
                db      "KLMNOPQR"                      ;04
                db      "STUVWXYZ"                      ;05

scode_tbl_graph:
                db      $09,$AC,$AB,$BA,$EF,$BD,$F4,$FB ;00
                db      $EC,$07,$17,$F1,$1E,$01,$0D,$06 ;01
                db      $05,$BB,$F3,$F2,$1D,$00,$C4,$11 ;02
                db      $BC,$C7,$CD,$14,$15,$13,$DC,$C6 ;03
                db      $DD,$C8,$0B,$1B,$C2,$DB,$CC,$18 ;04
                db      $D2,$12,$C0,$1A,$CF,$1C,$19,$0F ;05

scode_tbl_shift_graph:
                db      $0A,$00,$FD,$FC,$00,$00,$F5,$00 ;00
                db      $00,$08,$1F,$F0,$16,$02,$0E,$04 ;01
                db      $03,$F7,$AE,$AF,$F6,$00,$FE,$00 ;02
                db      $FA,$C1,$CE,$D4,$10,$D6,$DF,$CA ;03
                db      $DE,$C9,$0C,$D3,$C3,$D7,$CB,$A9 ;04
                db      $D1,$00,$C5,$D5,$D0,$F9,$AA,$F8 ;05

scode_tbl_code:
                db      $EB,$9F,$D9,$BF,$9B,$98,$E0,$E1 ;00
                db		$E7,$87,$EE,$E9,$00,$ED,$DA,$B7 ;01
                db		$B9,$E5,$86,$A6,$A7,$00,$84,$97 ;02
                db		$8D,$8B,$8C,$94,$81,$B1,$A1,$91 ;03
                db		$B3,$B5,$E6,$A4,$A2,$A3,$83,$93 ;04
                db		$89,$96,$82,$95,$88,$8A,$A0,$85 ;05
                
scode_tbl_shift_code:
                db      $D8,$AD,$9E,$BE,$9C,$9D,$00,$00 ;00
                db		$E2,$80,$00,$00,$00,$E8,$EA,$B6 ;01
                db		$B8,$E4,$8F,$00,$A8,$00,$8E,$00 ;02
                db		$00,$00,$00,$99,$9A,$B0,$00,$92 ;03
                db		$B2,$B4,$00,$A5,$00,$E3,$00,$00 ;04
                db		$00,$00,$90,$00,$00,$00,$00,$00 ;05

; vim:ts=8:expandtab:filetype=z8a:syntax=z8a: