; Scan code tables JIS (japanese) keybaord for C-BIOS
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
; scan code tableS
scode_tbl:
; Japanese
                db      "01234567"                      ;00
                db      "89-^",$5C,"@[;"                ;01 ($5C = yen)
                db      ":],./_ab"                      ;02
                db      "cdefghij"                      ;03
                db      "klmnopqr"                      ;04
                db      "stuvwxyz"                      ;05

scode_tbl_shift:
                db      "0!",$22,"#$%&'"                ;00 ($22 = quote)
                db      "()=~|`{+"                      ;01
                db      "*}<>?_AB"                      ;02
                db      "CDEFGHIJ"                      ;03
                db      "KLMNOPQR"                      ;04
                db      "STUVWXYZ"                      ;05

scode_tbl_graph:
                db      $0F,$07,$01,$02,$03,$04,$05,$06 ;00
                db      $0D,$E0,$17,$00,$09,$00,$84,$82 ;01
                db      $81,$85,$1F,$1D,$80,$83,$00,$1B ;02
                db      $1A,$14,$18,$15,$13,$0A,$16,$00 ;03
                db      $00,$1E,$0B,$00,$00,$10,$00,$12 ;04
                db      $0C,$19,$00,$11,$00,$1C,$08,$00 ;05

; TODO SHIFT + GRAPH is undefined in the red book. Same keycodes
;      as with shift or no key entered? (all $00)
scode_tbl_shift_graph:
                db      $0F,$07,$01,$02,$03,$04,$05,$06 ;00
                db      $0D,$E0,$17,$00,$09,$00,$84,$82 ;01
                db      $81,$85,$1F,$1D,$80,$83,$00,$1B ;02
                db      $1A,$14,$18,$15,$13,$0A,$16,$00 ;03
                db      $00,$1E,$0B,$00,$00,$10,$00,$12 ;04
                db      $0C,$19,$00,$11,$00,$1C,$08,$00 ;05

scode_tbl_kana:
                db      $FC,$E7,$EC,$91,$93,$94,$95,$F4 ;00
                db		$F5,$F6,$EE,$ED,$B0,$DE,$DF,$FA ;01
                db		$99,$F1,$E8,$F9,$F2,$FB,$E1,$9A ;02
                db		$9F,$9C,$92,$EA,$97,$98,$E6,$EF ;03
                db		$E9,$F8,$F3,$F0,$F7,$9E,$E0,$9D ;04
                db		$E4,$96,$E5,$EB,$E3,$9B,$FD,$E2 ;05
                
scode_tbl_caps_kana:
                db      $DC,$C7,$CC,$B1,$B3,$B4,$B5,$D4 ;00
                db		$D5,$D6,$CE,$CD,$B0,$DE,$DF,$DA ;01
                db		$B9,$D1,$C8,$D9,$D2,$DB,$C1,$BA ;02
                db		$BF,$BC,$B2,$CA,$B7,$B8,$C6,$CF ;03
                db		$C9,$D8,$D3,$D0,$D7,$BE,$C0,$BD ;04
                db		$C4,$B6,$C5,$CB,$C3,$BB,$DD,$C2 ;05
                
scode_tbl_shift_kana:
                db      $86,$00,$00,$87,$89,$8A,$8B,$8C ;00
                db		$8D,$8E,$00,$00,$00,$00,$A2,$00 ;01
                db		$00,$83,$84,$81,$85,$00,$00,$00 ;02
                db		$00,$00,$88,$00,$00,$00,$00,$00 ;03
                db		$00,$00,$00,$00,$00,$00,$00,$00 ;04
                db		$00,$00,$00,$00,$00,$00,$00,$8F ;05

                
scode_tbl_shift_caps_kana:
                db      $A6,$00,$00,$A7,$A9,$AA,$AB,$AC ;00
                db		$AD,$AE,$00,$00,$00,$00,$A2,$00 ;01
                db		$00,$A3,$A4,$A1,$A5,$00,$00,$00 ;02
                db		$00,$00,$A8,$00,$00,$00,$00,$00 ;03
                db		$00,$00,$00,$00,$00,$00,$00,$00 ;04
                db		$00,$00,$00,$00,$00,$00,$00,$AF ;05


; vim:ts=8:expandtab:filetype=z8a:syntax=z8a: