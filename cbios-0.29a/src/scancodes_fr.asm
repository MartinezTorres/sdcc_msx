; Scan code tables FR (french) keyboard for C-BIOS
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
                db      $85,"&",$82,$22,"'(",$BF,$8A    ;00
                db      "!",$87,")-<",$00,"$m"          ;01
                db      $97,"#;:=",$00,"qb"             ;02
                db      "cdefghij"                      ;03
                db      "kl,nopar"                      ;04
                db      "stuvzxyw"                      ;05

scode_tbl_shift:
                db      "01234567"                      ;00
                db      "89",$F8,"_>",$00,"*M"          ;01
                db      "%",$9C,"./+",$00,"QB"          ;02
                db      "CDEFGHIJ"                      ;03
                db      "KL?NOPAR"                      ;04
                db      "STUVZXYW"                      ;05

scode_tbl_graph:
                db      $09,$AC,$AB,$BA,$BB,$EF,$F4,$FB ;00
                db      $EC,$07,$01,$17,$AE,$00,$0D,$06 ;01
                db      $05,$BB,$F6,$1E,$F1,$00,$C4,$11 ;02
                db      $BC,$C7,$CD,$14,$15,$13,$DC,$C6 ;03
                db      $DD,$C8,$0B,$1B,$C2,$DB,$CC,$18 ;04
                db      $D2,$12,$C0,$1A,$CF,$1C,$19,$0F ;05

scode_tbl_shift_graph:
                db      $0A,$16,$FD,$FC,$F7,$00,$F5,$00 ;00
                db      $00,$08,$02,$1F,$AF,$00,$0E,$04 ;01
                db      $03,$00,$00,$1D,$F0,$00,$FE,$00 ;02
                db      $FA,$C1,$CE,$D4,$10,$D6,$DF,$CA ;03
                db      $DE,$C9,$0C,$D3,$C3,$D7,$CB,$A9 ;04
                db      $D1,$D9,$C5,$D5,$D0,$F9,$AA,$00 ;05

scode_tbl_code:
                db      $EB,$7C,$40,$E0,$60,$7B,$5E,$EE ;00
                db		$E7,$E9,$7D,$ED,$F3,$00,$9B,$B7 ;01
                db		$B9,$E5,$86,$A6,$A7,$00,$84,$E1 ;02
                db		$8D,$8B,$8C,$94,$81,$B1,$A1,$91 ;03
                db		$B3,$B5,$E6,$A4,$A2,$A3,$83,$93 ;04
                db		$89,$96,$98,$95,$88,$9F,$A0,$DA ;05
                
scode_tbl_shift_code:
                db      $D8,$AD,$90,$9E,$00,$5B,$BE,$7E ;00
                db		$E2,$80,$5D,$E8,$F2,$00,$00,$B6 ;01
                db		$B8,$E4,$8F,$5C,$00,$00,$8E,$00 ;02
                db		$00,$00,$00,$99,$9A,$B0,$00,$92 ;03
                db		$B2,$B4,$A8,$A5,$00,$E3,$00,$00 ;04
                db		$00,$00,$00,$00,$00,$00,$9D,$EA ;05

; vim:ts=8:expandtab:filetype=z8a:syntax=z8a: