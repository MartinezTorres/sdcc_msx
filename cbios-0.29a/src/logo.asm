; C-BIOS logo ROM
;
; Copyright (c) 2004-2005 Maarten ter Huurne.  All rights reserved.
; Copyright (c) 2004-2005 Albert Beevendorp.  All rights reserved.
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

                include "systemvars.asm"

;
logo_ident:     db      "C-BIOS Logo ROM",$FF

logo_show:
        IF VDP = TMS99X8
                call    $6f

                ld      a,5
                ld      (BAKCLR),a
                ld      (BDRCLR),a
                call    $62

                ld      hl,(NAMBAS)
                ld      bc,768
                ld      a,$00
                call    $56

                ; Set up SCREEN 2 mirrored
                ld      bc,0 +256* 2
                call    $47
                ld      bc,3 +256* 159
                call    $47
                ld      bc,4 +256* 0
                call    $47

                ; Fill the color table
                ld      a,(FORCLR)
                and     15
                rlca
                rlca
                rlca
                rlca
                ld      b,a
                ld      a,(BAKCLR)
                and     15
                or      b
                ld      bc,2048
                ld      hl,(GRPCOL)
                call    $56

                ld      hl,(CGPBAS)
                ld      bc,8 * logo_patoffset
                add     hl,bc
                ex      de,hl
                ld      bc,8 * logo_npatterns
                ld      hl,logo_patterns
                call    $5c

                ld      hl,(CGPBAS)
                ld      bc,8 * 32
                add     hl,bc
                ex      de,hl
                ld      hl,(4)
                add     hl,bc
                ld      bc,8 * 96
                call    $5c

                ld      hl,(GRPCOL)
                ld      bc,8 * logo_patoffset
                add     hl,bc
                ex      de,hl
                ld      bc,8 * logo_ncolors
                ld      hl,logo_colors
                call    $5c

                ld      hl,(GRPCOL)
                ld      de,8 * 32
                add     hl,de
                ld      bc,8 * 96
                ld      a,$f1
                call    $56

                ld      hl,(NAMBAS)
                ld      bc,logo_namoffset
                add     hl,bc
                ex      de,hl
                ld      hl,logo_names
                ld      b,logo_height
plot_logo_nam:
                push    bc
                push    hl
                push    de
                ld      bc,logo_width
                call    $5c
                pop     hl                      ; value of DE
                ld      bc,32
                add     hl,bc
                ex      de,hl
                pop     hl                      ; value of HL
                ld      bc,logo_width
                add     hl,bc
                pop     bc
                djnz    plot_logo_nam

                ret
;
logo_patoffset: equ     128
logo_namoffset: equ     4 *32+ 4                ; Y *32+ 4
;
logo_patterns:
                db      $00,$00,$00,$00,$00,$00,$00,$00
logo_patlength: equ     $ - logo_patterns
                db      $00,$00,$00,$00,$00,$00,$FE,$F8
                db      $00,$00,$FE,$F8,$1F,$7F,$00,$00
                db      $FE,$3F,$00,$00,$00,$00,$00,$00
                db      $00,$00,$00,$00,$00,$00,$00,$00
                db      $1F,$00,$00,$00,$00,$00,$00,$00
                db      $00,$00,$1F,$F8,$FE,$00,$00,$00
                db      $00,$00,$00,$00,$00,$7F,$1F,$F0
                db      $00,$00,$00,$00,$FE,$FC,$F8,$F0
                db      $1F,$3F,$7F,$00,$00,$00,$00,$00
                db      $00,$00,$00,$00,$FE,$FC,$F8,$F0
                db      $00,$F8,$1F,$7F,$00,$00,$00,$00
                db      $00,$00,$00,$00,$00,$00,$00,$00
                db      $00,$1F,$F8,$FE,$00,$00,$00,$00
                db      $00,$00,$00,$00,$00,$7F,$3F,$1F
                db      $F8,$FC,$FE,$00,$00,$00,$00,$00
                db      $00,$00,$00,$00,$7F,$3F,$1F,$F0
                db      $00,$00,$00,$00,$00,$00,$FE,$FE
                db      $1F,$3F,$7F,$7F,$00,$00,$00,$00
                db      $00,$00,$00,$00,$00,$FE,$FE,$FC
                db      $1F,$3F,$7F,$00,$00,$00,$00,$00
                db      $F8,$F8,$F8,$F8,$F0,$F0,$F0,$F0
                db      $1F,$1F,$1F,$1F,$3F,$3F,$3F,$3F
                db      $F8,$F8,$F8,$F8,$00,$00,$00,$00
                db      $F8,$F8,$F0,$F0,$F0,$1F,$1F,$1F
                db      $00,$00,$00,$00,$00,$00,$00,$00
                db      $F8,$F8,$F0,$F0,$F0,$1F,$1F,$1F
                db      $F0,$F0,$F0,$F0,$1F,$1F,$1F,$1F
                db      $3F,$C7,$DF,$00,$00,$F8,$F0,$1F
                db      $00,$00,$00,$00,$00,$00,$00,$00
                db      $00,$00,$00,$00,$F8,$00,$00,$00
                db      $00,$1F,$F8,$FC,$FE,$00,$7F,$3F
                db      $FE,$FE,$FE,$FE,$FC,$FC,$FC,$7C
                db      $F0,$F0,$F0,$F0,$1F,$1F,$1F,$1F
                db      $00,$00,$FC,$F8,$1F,$3F,$7F,$FE
                db      $1F,$00,$00,$00,$F0,$7F,$00,$00
                db      $1F,$FC,$00,$00,$00,$1F,$F0,$F8
                db      $00,$00,$00,$7F,$3F,$1F,$1F,$F0
                db      $FE,$F8,$F0,$1F,$7F,$7C,$FC,$F8
                db      $3F,$3F,$7F,$7F,$7F,$7F,$7F,$7F
                db      $00,$00,$00,$00,$00,$00,$00,$00
                db      $3F,$3F,$7F,$7F,$7F,$7F,$7F,$7F
                db      $1F,$1F,$1F,$1F,$3F,$3F,$3F,$3F
                db      $3F,$7F,$7F,$7F,$00,$00,$00,$00
                db      $1F,$F0,$F8,$F8,$F8,$F8,$F8,$F8
                db      $7C,$3C,$3C,$3C,$C7,$C7,$C7,$C7
                db      $1F,$1E,$E3,$E3,$C7,$C7,$C7,$C7
                db      $FC,$F8,$F0,$F0,$1F,$3F,$3F,$3F
                db      $FC,$FE,$00,$00,$00,$00,$00,$00
                db      $F0,$F8,$F8,$F8,$F8,$F8,$F8,$F8
                db      $F8,$F8,$F8,$F8,$F8,$FE,$7F,$3F
                db      $00,$00,$00,$00,$00,$00,$00,$00
                db      $00,$00,$00,$00,$00,$00,$7F,$1F
                db      $00,$00,$00,$00,$00,$00,$00,$00
                db      $3F,$3F,$3F,$3F,$7E,$7E,$7E,$7E
                db      $F8,$F8,$F8,$F0,$1F,$1F,$3F,$7E
                db      $C7,$C7,$C7,$C7,$F0,$F0,$F0,$F0
                db      $C7,$C7,$C7,$C7,$78,$78,$78,$7C
                db      $3F,$3F,$3F,$3F,$3F,$3F,$1F,$F0
                db      $00,$00,$00,$FE,$FC,$FC,$F8,$F0
                db      $F8,$F8,$F8,$F8,$F0,$F0,$1F,$1F
                db      $1F,$F8,$00,$00,$00,$00,$00,$00
                db      $00,$00,$00,$00,$00,$00,$00,$00
                db      $00,$00,$FE,$00,$00,$00,$00,$00
                db      $F0,$F8,$FC,$7C,$7C,$7C,$7C,$7C
                db      $00,$00,$00,$7F,$7F,$7F,$3F,$3F
                db      $00,$00,$00,$00,$00,$00,$00,$00
                db      $00,$00,$00,$7F,$7F,$7F,$3F,$3F
                db      $7E,$7E,$7E,$7E,$00,$00,$00,$00
                db      $00,$00,$00,$00,$00,$00,$00,$00
                db      $00,$FE,$FC,$1F,$00,$00,$FE,$F0
                db      $FE,$FC,$F8,$F0,$3C,$8F,$1F,$7F
                db      $F0,$F0,$F0,$F0,$9F,$9F,$9F,$9F
                db      $7C,$7E,$7E,$7F,$E3,$3E,$3F,$3F
                db      $F8,$FC,$FE,$00,$7F,$3F,$F0,$3E
                db      $00,$00,$00,$3F,$00,$00,$00,$00
                db      $00,$00,$00,$FC,$00,$00,$00,$FE
                db      $1F,$3F,$7F,$FE,$F8,$F1,$3C,$F0
                db      $3F,$7F,$00,$00,$7F,$00,$00,$00
                db      $00,$00,$00,$FC,$00,$00,$00,$FE
                db      $7C,$FC,$F8,$F8,$1F,$3F,$7F,$00
                db      $3F,$3F,$1F,$1F,$F0,$F0,$F8,$FC
                db      $3F,$1F,$1F,$F0,$F8,$F8,$FC,$FE
                db      $FC,$FE,$00,$00,$00,$00,$00,$00
                db      $00,$00,$00,$7F,$3F,$1F,$F0,$F8
                db      $FE,$00,$00,$00,$00,$00,$00,$00
                db      $00,$00,$7F,$3F,$1F,$F8,$FE,$00
                db      $00,$00,$00,$00,$00,$00,$00,$3F
                db      $00,$00,$00,$00,$00,$FE,$F8,$3F
                db      $F0,$1F,$3F,$7F,$00,$00,$00,$00
                db      $00,$00,$00,$00,$00,$00,$FE,$FC
                db      $F8,$F0,$1F,$3F,$7F,$00,$00,$00
                db      $FC,$FE,$00,$00,$00,$00,$00,$00
                db      $00,$00,$7F,$1F,$F8,$FE,$00,$00
                db      $00,$00,$00,$00,$00,$00,$3F,$FE
                db      $00,$00,$00,$00,$00,$00,$00,$1F
                db      $00,$00,$00,$FE,$F8,$1F,$00,$00
                db      $F0,$1F,$7F,$00,$00,$00,$00,$00
                db      $00,$00,$00,$00,$00,$00,$00,$00
                db      $00,$7F,$7F,$7F,$7F,$7F,$7F,$7F
                db      $7F,$7F,$7F,$7F,$7F,$7F,$7F,$7F
                db      $7F,$00,$00,$00,$00,$00,$00,$00
logo_npatterns: equ     ($ - logo_patterns) / logo_patlength
;
logo_colors:
                db      $00,$00,$00,$00,$00,$00,$00,$00
logo_collength: equ     $ - logo_colors
                db      $00,$00,$00,$00,$00,$00,$09,$09
                db      $00,$00,$09,$09,$90,$90,$09,$09
                db      $09,$90,$09,$09,$09,$09,$09,$09
                db      $09,$09,$09,$09,$09,$09,$09,$09
                db      $09,$09,$09,$09,$09,$09,$09,$09
                db      $00,$00,$09,$90,$90,$09,$09,$09
                db      $00,$00,$00,$00,$00,$09,$09,$90
                db      $01,$01,$01,$01,$19,$19,$19,$19
                db      $91,$91,$91,$09,$09,$09,$09,$09
                db      $09,$09,$09,$09,$91,$91,$91,$91
                db      $09,$91,$19,$19,$01,$01,$01,$01
                db      $01,$01,$01,$01,$01,$01,$01,$01
                db      $09,$91,$19,$19,$01,$01,$01,$01
                db      $09,$09,$09,$09,$09,$91,$91,$91
                db      $91,$91,$91,$09,$09,$09,$09,$09
                db      $01,$01,$01,$01,$19,$19,$19,$91
                db      $00,$00,$00,$00,$00,$00,$09,$09
                db      $91,$91,$91,$91,$09,$09,$09,$09
                db      $09,$09,$09,$09,$09,$91,$91,$91
                db      $19,$19,$19,$01,$01,$01,$01,$01
                db      $1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F
                db      $1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F
                db      $F1,$F1,$F1,$F1,$01,$01,$01,$01
                db      $08,$09,$09,$09,$08,$90,$90,$90
                db      $08,$09,$09,$09,$08,$09,$09,$09
                db      $81,$91,$91,$91,$81,$19,$19,$19
                db      $1F,$1F,$1F,$1F,$F1,$F1,$F1,$F1
                db      $1F,$F1,$F1,$0F,$0F,$F1,$F1,$1F
                db      $0F,$0F,$0F,$0F,$01,$01,$01,$01
                db      $0F,$0F,$0F,$0F,$1F,$01,$01,$01
                db      $01,$1F,$F1,$F1,$F1,$0F,$F1,$F1
                db      $1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F
                db      $F1,$F1,$F1,$F1,$1F,$1F,$1F,$1F
                db      $01,$01,$1F,$1F,$F1,$F1,$F1,$F1
                db      $F1,$0F,$0F,$0F,$F1,$1F,$01,$01
                db      $1F,$F1,$0F,$0F,$0F,$F1,$1F,$1F
                db      $01,$01,$01,$1F,$1F,$1F,$1F,$F1
                db      $1F,$1F,$1F,$F1,$F1,$F1,$F1,$F1
                db      $80,$90,$90,$80,$90,$80,$80,$90
                db      $08,$09,$09,$08,$09,$08,$08,$09
                db      $18,$19,$19,$18,$19,$18,$18,$19
                db      $F1,$F1,$F1,$F1,$F1,$F1,$F1,$F1
                db      $1F,$1F,$1F,$1F,$01,$01,$01,$01
                db      $F1,$1F,$1F,$1F,$1F,$1F,$1F,$1F
                db      $1F,$1F,$1F,$1F,$F1,$F1,$F1,$F1
                db      $1F,$1F,$F1,$F1,$F1,$F1,$F1,$F1
                db      $F1,$F1,$F1,$F1,$1F,$1F,$1F,$1F
                db      $1F,$1F,$01,$01,$01,$01,$01,$01
                db      $F1,$F1,$F1,$F1,$F1,$F1,$F1,$F1
                db      $F1,$F1,$F1,$F1,$F1,$F1,$F1,$F1
                db      $01,$01,$01,$01,$01,$01,$0F,$0F
                db      $01,$01,$01,$01,$01,$01,$1F,$1F
                db      $08,$08,$08,$09,$08,$08,$08,$09
                db      $F1,$F1,$E1,$F1,$F1,$E1,$E1,$F1
                db      $1F,$1F,$1E,$1F,$F1,$E1,$E1,$F1
                db      $F1,$F1,$E1,$F1,$1F,$1E,$1E,$1F
                db      $F1,$F1,$E1,$F1,$1F,$1E,$1E,$1F
                db      $1F,$1F,$1E,$1F,$1F,$1E,$1E,$F1
                db      $01,$01,$01,$1F,$1F,$1E,$1E,$1F
                db      $F1,$F1,$E1,$F1,$F1,$E1,$1E,$1F
                db      $F1,$1F,$01,$01,$01,$01,$01,$01
                db      $0F,$0F,$01,$01,$01,$01,$01,$01
                db      $0F,$0F,$1E,$01,$01,$01,$01,$01
                db      $F1,$F1,$E1,$F1,$F1,$E1,$E1,$F1
                db      $08,$08,$08,$80,$80,$80,$80,$80
                db      $08,$08,$08,$08,$08,$08,$08,$08
                db      $01,$01,$01,$18,$18,$18,$18,$18
                db      $E1,$E1,$E1,$E1,$0E,$0E,$0E,$0E
                db      $01,$01,$01,$01,$0E,$0E,$0E,$0E
                db      $01,$1E,$1E,$E1,$0E,$0E,$E1,$E1
                db      $E1,$E1,$E1,$E1,$1E,$E1,$E1,$E1
                db      $1E,$1E,$1E,$1E,$E1,$E1,$E1,$E1
                db      $1E,$1E,$1E,$1E,$1E,$E1,$E1,$E1
                db      $E1,$E1,$E1,$0E,$E1,$E1,$1E,$1E
                db      $01,$01,$01,$1E,$0E,$0E,$0E,$0E
                db      $01,$01,$01,$1E,$0E,$0E,$0E,$E1
                db      $E1,$E1,$E1,$E1,$E1,$E1,$1E,$1E
                db      $1E,$1E,$01,$01,$E1,$0E,$0E,$0E
                db      $01,$01,$01,$1E,$0E,$0E,$0E,$E1
                db      $E1,$E1,$E1,$E1,$1E,$1E,$1E,$01
                db      $80,$80,$80,$80,$08,$08,$08,$08
                db      $18,$18,$18,$81,$81,$81,$81,$81
                db      $08,$08,$00,$00,$00,$00,$00,$00
                db      $08,$08,$08,$81,$81,$81,$18,$18
                db      $81,$08,$08,$08,$08,$08,$08,$08
                db      $01,$01,$18,$18,$18,$81,$81,$08
                db      $01,$01,$01,$01,$01,$01,$01,$18
                db      $01,$01,$01,$01,$01,$18,$18,$81
                db      $18,$81,$81,$81,$08,$08,$08,$08
                db      $08,$08,$08,$08,$08,$08,$81,$81
                db      $81,$81,$18,$18,$18,$01,$01,$01
                db      $08,$08,$00,$00,$00,$00,$00,$00
                db      $08,$08,$80,$80,$08,$08,$00,$00
                db      $08,$08,$08,$08,$08,$08,$80,$08
                db      $08,$08,$08,$08,$08,$08,$08,$08
                db      $08,$08,$08,$80,$80,$08,$00,$00
                db      $84,$08,$08,$00,$00,$00,$00,$00
                db      $04,$00,$00,$00,$00,$00,$00,$00
                db      $00,$04,$04,$04,$04,$04,$04,$04
                db      $04,$04,$04,$04,$04,$04,$04,$04
                db      $04,$00,$00,$00,$00,$00,$00,$00
logo_ncolors:   equ     ($ - logo_colors) / logo_collength
;
logo_names:
                db      $80,$80,$81,$82,$83,$84,$85,$86,$87,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80
logo_width:     equ     $ - logo_names
                db      $80,$88,$89,$8A,$8B,$8C,$8D,$8E,$8F,$90,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$E3
                db      $91,$92,$93,$94,$8C,$95,$96,$8C,$8C,$8C,$8C,$97,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$E4
                db      $98,$99,$9A,$8C,$8C,$9B,$9C,$9D,$9E,$9F,$A0,$A1,$A2,$A3,$9D,$A4,$A5,$A6,$9D,$9D,$9D,$9D,$8C,$E4
                db      $A7,$A8,$A9,$8C,$8C,$AA,$AB,$8C,$8C,$AC,$AD,$AE,$AF,$8C,$8C,$B0,$B1,$B2,$B3,$B3,$B3,$B4,$8C,$E4
                db      $B5,$B5,$8C,$8C,$8C,$B6,$8C,$8C,$8C,$B7,$B8,$B9,$BA,$8C,$8C,$BB,$BC,$BD,$BE,$BE,$BF,$C0,$8C,$E4
                db      $C1,$C2,$C3,$8C,$8C,$C4,$C5,$C5,$C6,$C7,$C8,$C9,$CA,$CB,$CC,$CD,$CE,$C5,$C5,$C5,$CF,$D0,$8C,$E4
                db      $D1,$C2,$D2,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$E4
                db      $D3,$D4,$D5,$D6,$D7,$D8,$D9,$DA,$DB,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C
                db      "V"
                incbin  "../version.txt"
                db      $E4
                db      $80,$DC,$DD,$DE,$C2,$DF,$E0,$E1,$E2,$E2,$E2,$E2,$E2,$E2,$E2,$E2,$E2,$E2,$E2,$E2,$E2,$E2,$E2,$E5
logo_height:    equ     ($ - logo_names) / logo_width
;
        ENDIF
;
        IF VDP = V9938
                ld      de,$c000
                ld      hl,msx2logodata
                call    unPack

                ld      hl,0
                ld      (BAKCLR),hl
                ld      a,5
                call    $5f

                ld      hl,palette1
                call    setpalette

                call    $41

                ld      hl,256
                ld      (BAKCLR),hl
                call    $62

                ld      a,(RG8SAV+1)
                and     127
                ld      b,a
                ld      c,9
                call    $47

wait_ce_logo:   ld      a,2
                ld      ix,$131
                call    $15f
                bit     0,a
                jr      nz,wait_ce_logo

                push    de
                ld      bc,15
                ld      hl,logo_hmmc
                ldir

                pop     hl
                ld      bc,13
                add     hl,bc
                ld      de,$c000
                ld      a,(de)
                inc     de
                ld      (hl),a
                ex      de,hl

                di
                ld      a,32
                out     ($99),a
                ld      a,128+ 17
                out     ($99),a
                push    hl
                ld      bc,15 *256+ $9b
                ld      hl,logo_hmmc
                otir
                pop     hl
                ld      a,128+ 44
                out     ($99),a
                ld      a,128+ 17
                out     ($99),a
                ei

                ld      b,255
                otir

loop_logo:      ld      a,2
                ld      ix,$131
                call    $15f
                bit     0,a
                jr      z,done_logo

                otir
                jr      loop_logo

done_logo:      ld      bc,32
                ld      de,$c000
                ld      hl,palette1
                ldir

                ld      hl,22 *8
                ld      (GRPACX),hl
                ld      hl,12 *8 +1
                ld      (GRPACY),hl
                ld      a,7
                ld      (FORCLR),a
                ld      a,8
                ld      (LOGOPR),a
                ld      hl,logo_ver
                call    prn_text
                ld      (LOGOPR),a

                call    $44

palette_loop:
                ld      b,16
                ld      de,palette2
                ld      hl,$c000
palette_color:
                ld      a,(de)                  ; change red
                and     240
                ld      c,a
                ld      a,(hl)
                and     240
                cp      c
                jr      z,palette_red_done
                jr      nc,palette_red_down
                add     a,16
                jr      palette_red_done
palette_red_down:
                sub     16
palette_red_done:
                ld      c,a
                ld      a,(hl)
                and     15
                or      c
                ld      (hl),a

                ld      a,(de)
                and     15
                ld      c,a
                ld      a,(hl)
                and     15
                cp      c
                jr      z,palette_blue_done
                jr      nc,palette_blue_down
                inc     a
                jr      palette_blue_done
palette_blue_down:
                dec     a
palette_blue_done:
                ld      c,a
                ld      a,(hl)
                and     240
                or      c
                ld      (hl),a
                inc     de
                inc     hl

                ld      a,(de)
                ld      c,a
                ld      a,(hl)
                cp      c
                jr      z,palette_green_done
                jr      nc,palette_green_down
                inc     a
                jr      palette_green_done
palette_green_down:
                dec     a
palette_green_done:
                ld      (hl),a
                inc     de
                inc     hl
                djnz    palette_color

                ld      hl,$c000
                call    setpalette

                ld      b,6
palette_wait:
                halt
                djnz    palette_wait

                ld      b,32
                ld      de,palette2
                ld      hl,$c000
palette_check:
                ld      a,(de)
                cp      (hl)
                jr      nz,palette_loop
                inc     de
                inc     hl
                djnz    palette_check

                ld      b,9
                ld      hl,glare
glare_loop:
                ld      e,(hl)
                inc     hl
                ld      d,(hl)
                inc     hl
                push    bc
                push    hl
                ex      de,hl
                call    setpalette
                pop     hl
                pop     bc

                halt
                halt

                djnz    glare_loop

                ret

setpalette:     di
                xor     a
                out     ($99),a
                ld      a,128+ 16
                out     ($99),a
                ld      bc,32 *256+ $9a
                otir
                ei
                ret

prn_text:
                ld      a,(SCRMOD)
                cp      5
                jr      nc,prn_text_graph
prn_text_char:
                ld      a,(hl)
                or      a
                ret     z
                call    $a2
                inc     hl
                jr      prn_text_char
prn_text_graph:
                ld      a,(hl)
                or      a
                ret     z
                ld      ix,$0089
                call    $15f
                inc     hl
                jr      prn_text_graph

logo_hmmc:      dw      0
                dw      0
                dw      0
                dw      31
                dw      256
                dw      85
col:            db      0
                db      0
                db      $f0

palette1:       dw      $000,$327,$327,$327,$327,$327,$327,$327
                dw      $327,$327,$327,$327,$327,$327,$327,$327

palette2:       dw      $000,$327,$117,$000,$111,$333,$555,$777
                dw      $674,$563,$573,$572,$672,$772,$470,$270

palette3:       dw      $000,$327,$117,$000,$111,$333,$555,$777
                dw      $674,$563,$573,$572,$672,$772,$470,$777

palette4:       dw      $000,$327,$117,$000,$111,$333,$555,$777
                dw      $674,$563,$573,$572,$672,$772,$777,$270

palette5:       dw      $000,$327,$117,$000,$111,$333,$555,$777
                dw      $674,$563,$573,$572,$672,$777,$470,$270

palette6:       dw      $000,$327,$117,$000,$111,$333,$555,$777
                dw      $674,$563,$573,$572,$777,$772,$470,$270

palette7:       dw      $000,$327,$117,$000,$111,$333,$555,$777
                dw      $674,$563,$573,$777,$672,$772,$470,$270

palette8:       dw      $000,$327,$117,$000,$111,$333,$555,$777
                dw      $674,$563,$777,$572,$672,$772,$470,$270

palette9:       dw      $000,$327,$117,$000,$111,$333,$555,$777
                dw      $777,$563,$573,$572,$672,$772,$470,$270

palette10:      dw      $000,$327,$117,$000,$111,$333,$555,$777
                dw      $674,$777,$573,$572,$672,$772,$470,$270

glare:          dw      palette3,palette4,palette5,palette6
                dw      palette7,palette8,palette9,palette10,palette2
;
logo_ver:
                db      "V"
                incbin  "../version.txt"
                db      0

;
; Bitbuster by Team Bomba
;

;
; In: HL = source
;     DE = destination
;
unPack:
                exx
                ld      bc,128          ; b' = 0 (register loading optimize)
                                        ; c' = bits from bitstream
                exx

unPack_loop:
                exx
                call    getBit
                exx
                jr      c,unPack_outCompress    ; if set, we got LZ77 compression

unPack_outLiteral:
                ldi                     ; copy byte from compressed data to destination
                jr      unPack_loop     ; handle more compressed data

unPack_outCompress:
                ld      a,(hl)          ; get lowest 7 bits of offset, plus the offset
                                        ; extension bit
                inc     hl

                or      a
                jr      z,unPack_outRle ; offset = 0, RLE compression used

unPack_outMatch:
                exx
                ld      e,a
                ld      d,b             ; b' should be always clear when entering this part
                rlca                    ; offset extension bit set?
                jr      nc,unPack_outMatch1     ; no need to get extra bits if carry not set

                call    getBit          ; get offset bit 10
                rl      d
                call    getBit          ; get offset bit 9
                rl      d
                call    getBit          ; get offset bit 8
                rl      d
                call    getBit          ; get offset bit 7

                jr      c,unPack_outMatch1      ; since extension mark already makes bit 7 set
                res     7,e             ; only clear it if the bit should be cleared
unPack_outMatch1:
                inc     de
                call    getGammaValue_0 ; get the match length
                                        ; HL' = length

                push    hl              ; save compressed data pointer
                exx
                push    hl              ; save match length
                push    de              ; save match offset
                exx

                ld      h,d             ; destination in HL
                ld      l,e
                pop     bc              ; load match offset length
                sbc     hl,bc           ; calculate source address
                pop     bc              ; load match length
                ldir

                pop     hl              ; load compressed data pointer
                jr      unPack_loop

unPack_outRle:
                call    getGammaValue
                ret     c               ; HL' = repeat length

                push    hl              ; save compressed data pointer
                exx
                push    hl              ; save repeat length
                exx
                pop     bc              ; load repeat length

                ld      h,d             ; source = destination - 1
                ld      l,e
                dec     hl
                ldir

                pop     hl              ; load compressed data pointer
                jr      unPack_loop
;
getBit:
                sla     c               ; shift out new bit
                ret     nz              ; if remaining value != 0, we're done

                exx
                ld      a,(hl)          ; get 8 bits from the compressed stream
                inc     hl
                exx

                ld      c,a             ; 8 bits in C'
                sla     c               ; shift out new bit
                inc     c               ; set bit 0 so C' will be zero after shifting 8 times
                ret
;
getGammaValue:
                exx                     ; get number of bits used to encode value
getGammaValue_0:
                ld      hl,1            ; initial length
                ld      b,1             ; bitcount

getGammaValue_size:
                call    getBit          ; get more bits
                jr      nc,getGammaValue_sizeEnd        ; if bit is not set, bit length is known
                inc     b               ; increase bitcount
                jr      getGammaValue_size

getGammaValue_bits:
                call    getBit          ; get next bit of value from the compressed stream
                adc     hl,hl           ; insert new bit in HL
getGammaValue_sizeEnd:
                djnz    getGammaValue_bits              ; repeat if more bits to go

getGammaValue_end:
                inc     hl              ; correct HL (was stored as length - 2)
                exx
                ret
;
msx2logodata:
                db      $7E,$00,$00,$01,$04,$33,$00,$C1,$40,$00,$00,$FD,$A2,$43,$79,$36
                db      $A9,$99,$00,$07,$7F,$34,$7D,$63,$FB,$10,$3A,$99,$9A,$6A,$BB,$00
                db      $BA,$85,$24,$93,$83,$BF,$61,$7E,$0F,$AA,$AA,$BB,$00,$02,$AA,$A9
                db      $80,$3F,$AE,$C7,$7E,$AB,$7E,$8C,$83,$AC,$80,$7F,$59,$7E,$EF,$80
                db      $1F,$D7,$67,$7E,$7D,$AC,$80,$7F,$5B,$7E,$E2,$AA,$D9,$00,$89,$6A
                db      $80,$2B,$85,$7E,$98,$05,$43,$E8,$72,$BA,$AA,$B5,$FD,$57,$89,$88
                db      $63,$BA,$93,$50,$7A,$F9,$FC,$EF,$66,$F6,$D9,$B6,$F7,$61,$0D,$38
                db      $98,$9A,$80,$1D,$4D,$81,$00,$FB,$1F,$7F,$96,$39,$BF,$F6,$0E,$3C
                db      $91,$88,$80,$D2,$A9,$8E,$37,$01,$F5,$5F,$20,$7F,$94,$99,$F6,$7E
                db      $8D,$80,$9E,$90,$26,$8E,$FE,$7F,$8C,$39,$9B,$E1,$7F,$B9,$6C,$E6
                db      $88,$97,$80,$DF,$FC,$7F,$A4,$99,$EC,$F3,$EB,$1E,$82,$38,$8A,$BB
                db      $80,$5F,$77,$7F,$F2,$63,$FE,$B8,$EA,$F5,$96,$80,$66,$93,$7F,$93
                db      $7F,$41,$FE,$1F,$1C,$80,$AE,$8A,$31,$16,$EF,$7F,$FC,$8C,$FE,$7C
                db      $B3,$01,$89,$89,$F8,$9F,$01,$93,$7F,$CF,$1C,$99,$CC,$00,$57,$E9
                db      $EC,$FE,$7F,$E9,$39,$9C,$7E,$C1,$C9,$93,$7F,$FD,$B4,$FE,$7C,$2B
                db      $66,$00,$4E,$E1,$7C,$36,$14,$7F,$7F,$C1,$B9,$9C,$7F,$8F,$FE,$21
                db      $57,$77,$76,$4E,$F0,$BE,$35,$14,$3F,$7F,$E0,$A5,$FE,$E2,$7C,$56
                db      $7F,$8F,$67,$7F,$E5,$07,$04,$3C,$CC,$7F,$28,$33,$FF,$F3,$7C,$66
                db      $7F,$47,$55,$56,$7F,$F2,$64,$43,$CC,$FE,$FC,$11,$E9,$14,$77,$63
                db      $FF,$FD,$39,$04,$33,$FE,$1D,$3F,$93,$7F,$B5,$3A,$03,$EE,$00,$7F
                db      $33,$7F,$6B,$FE,$2F,$17,$7F,$F5,$94,$FD,$D5,$FD,$B9,$F5,$7F,$EF
                db      $79,$01,$C4,$8A,$77,$09,$E7,$01,$CE,$B0,$3A,$7A,$01,$3F,$63,$7F
                db      $21,$38,$7F,$F3,$CE,$44,$77,$00,$7B,$67,$4B,$57,$94,$51,$97,$5C
                db      $1E,$18,$23,$76,$90,$C7,$11,$AF,$01,$0F,$7F,$C8,$27,$43,$3E,$FE
                db      $A7,$E3,$7F,$B7,$52,$95,$67,$7F,$E7,$7E,$78,$66,$C7,$80,$59,$2B
                db      $23,$79,$00,$FE,$7F,$53,$DD,$00,$96,$D9,$E9,$7A,$C8,$59,$67,$02
                db      $65,$55,$DC,$00,$6E,$38,$76,$63,$7F,$E7,$06,$0F,$39,$19,$46,$56
                db      $A1,$23,$66,$91,$8C,$2B,$7A,$55,$00,$FE,$7F,$38,$04,$43,$8D,$F8
                db      $7F,$92,$56,$DC,$E7,$67,$73,$66,$FE,$9D,$45,$98,$19,$DE,$8F,$48
                db      $67,$99,$E5,$66,$87,$2D,$D3,$FF,$E8,$DD,$FD,$FF,$1D,$3D,$C9,$7F
                db      $D2,$66,$8D,$E3,$5F,$6F,$66,$7F,$31,$EB,$BE,$27,$3C,$0C,$DB,$AD
                db      $CF,$7F,$F8,$69,$3B,$FE,$F8,$B6,$7F,$EB,$59,$F0,$66,$F0,$7F,$8E
                db      $8B,$63,$99,$CB,$18,$8F,$7F,$D8,$23,$43,$3D,$FE,$97,$DC,$FB,$FA
                db      $4B,$F1,$5E,$66,$4E,$66,$7F,$A3,$FE,$D2,$99,$37,$7A,$3B,$97,$E8
                db      $53,$20,$FE,$E4,$BD,$DC,$38,$3B,$FE,$1E,$63,$0F,$DD,$7F,$FA,$19
                db      $DF,$A8,$1F,$7F,$B8,$AE,$D7,$FD,$86,$FF,$38,$F7,$F9,$7E,$66,$A5
                db      $89,$8F,$29,$47,$66,$7F,$ED,$D7,$FE,$CB,$7F,$F0,$5F,$FA,$1A,$66
                db      $36,$A7,$EA,$7B,$A9,$AF,$14,$7F,$DC,$8F,$FF,$91,$57,$7F,$EE,$56
                db      $D8,$7F,$EA,$C9,$FE,$76,$9E,$94,$7F,$7F,$69,$44,$7F,$F2,$4E,$D5
                db      $3E,$73,$7F,$1E,$03,$65,$FE,$D7,$7F,$F8,$0B,$FE,$F9,$3E,$7F,$B7
                db      $73,$7F,$C4,$56,$DE,$A0,$BF,$7F,$4A,$FE,$FE,$4F,$7F,$CE,$D3,$A1
                db      $FD,$7D,$BF,$7F,$29,$3A,$7F,$FC,$1C,$80,$9D,$FB,$E9,$8D,$2E,$1F
                db      $7F,$D3,$F2,$A2,$47,$FD,$97,$94,$35,$7F,$F8,$A9,$38,$FF,$E9,$93
                db      $7F,$FB,$8E,$33,$FB,$F4,$D2,$94,$80,$5A,$FC,$7F,$04,$04,$FF,$7C
                db      $A8,$34,$CE,$9E,$53,$66,$A3,$45,$66,$43,$03,$AF,$18,$69,$2A,$64
                db      $F6,$4F,$00,$44,$46,$64,$7E,$43,$7F,$E4,$FF,$7C,$0C,$36,$FF,$D2
                db      $57,$76,$FF,$3E,$2D,$8F,$77,$97,$AD,$0F,$7F,$DB,$CF,$FE,$81,$66
                db      $E5,$9D,$FB,$7F,$EB,$8F,$C1,$7F,$F9,$F3,$FF,$F2,$48,$E5,$65,$97
                db      $FE,$7C,$A8,$25,$E9,$81,$45,$56,$53,$FE,$2E,$F2,$EA,$CC,$AD,$FC
                db      $11,$FF,$FB,$7E,$7F,$E6,$34,$46,$50,$70,$7F,$44,$27,$56,$55,$EA
                db      $D5,$66,$FE,$3F,$0F,$7F,$F0,$32,$FF,$EF,$FE,$29,$FE,$DD,$53,$99
                db      $B4,$FE,$A6,$F9,$7C,$72,$FF,$F9,$68,$AE,$FF,$FC,$18,$FE,$92,$65
                db      $FE,$47,$FF,$29,$34,$1A,$55,$FE,$1E,$84,$54,$43,$FF,$7E,$E9,$33
                db      $8E,$FF,$CF,$17,$7F,$B7,$65,$36,$E9,$CD,$80,$04,$75,$53,$98,$7B
                db      $7C,$01,$11,$A4,$5B,$FF,$E4,$F7,$FF,$C9,$7F,$EB,$65,$37,$55,$65
                db      $7F,$90,$44,$55,$67,$55,$8F,$98,$14,$65,$FE,$7C,$98,$93,$E1,$7F
                db      $F8,$6F,$8A,$7F,$BF,$CF,$2A,$FC,$B6,$65,$55,$8E,$85,$24,$80,$18
                db      $65,$55,$53,$A5,$CA,$55,$67,$8F,$FE,$92,$AF,$47,$44,$7F,$E4,$AB
                db      $38,$80,$E2,$CD,$7F,$CD,$72,$95,$FD,$A7,$53,$85,$28,$06,$88,$C9
                db      $80,$36,$E1,$EA,$19,$25,$C7,$67,$44,$00,$C3,$94,$4E,$1B,$FF,$F8
                db      $56,$80,$F2,$67,$62,$01,$8C,$E3,$8C,$44,$7E,$52,$64,$D4,$CD,$89
                db      $0A,$BC,$EC,$E0,$55,$8D,$8E,$BB,$9A,$00,$E0,$CF,$30,$7F,$DE,$F3
                db      $00,$A7,$22,$F5,$71,$7F,$55,$7F,$62,$ED,$C4,$80,$45,$E6,$18,$66
                db      $CE,$0C,$0B,$BC,$01,$FC,$7E,$3F,$7F,$11,$38,$80,$5F,$14,$CD,$BA
                db      $39,$43,$0F,$CB,$04,$7F,$0E,$C7,$00,$67,$EA,$7E,$00,$37,$33,$7F
                db      $E4,$DF,$80,$CD,$01,$FC,$37,$7F,$E1,$DB,$80,$83,$E3,$7F,$FB,$6B
                db      $80,$6D,$EE,$7F,$FD,$AD,$80,$38,$3F,$EF,$7F,$B3,$52,$00,$AD,$3A
                db      $8A,$7F,$7F,$F3,$7F,$6B,$80,$3E,$0F,$FF,$7F,$EC,$E7,$80,$E9,$3A
                db      $3F,$FF,$00,$FE,$7F,$7E,$80,$7E,$64,$FF,$96,$FF,$03,$7F,$F8,$53
                db      $80,$A4,$39,$9F,$E9,$7C,$51,$FE,$FC,$17,$7F,$E2,$4A,$80,$BA,$99
                db      $6A,$8F,$EA,$3E,$7E,$EB,$95,$F6,$FF,$7F,$1E,$80,$5E,$28,$FE,$FE
                db      $0F,$7F,$F1,$2F,$81,$65,$80,$C1,$FF,$FE,$1F,$C6,$7E,$7F,$2F,$82
                db      $24,$80,$B8,$BD,$FF,$7E,$96,$FF,$FF,$FC,$FC,$80,$AE,$29,$11,$9A
                db      $FF,$F0,$E6,$3F,$F4,$0D,$FE,$7F,$7D,$04,$29,$83,$38,$AF,$FF,$00
                db      $E3,$63,$F4,$FF,$F4,$14,$81,$CE,$83,$24,$80,$7C,$87,$34,$22,$00
                db      $EE,$7E,$7F,$33,$04,$83,$12,$80,$3E,$41,$33,$40,$00,$00,$FD,$22
                db      $44,$80,$3E,$5F,$7E,$E9,$E5,$80,$63,$80,$E2,$3F,$33,$30,$7E,$A6
                db      $96,$80,$23,$80,$5E,$AA,$7D,$43,$AF,$33,$34,$7E,$FA,$88,$80,$EC
                db      $38,$AA,$4B,$AA,$83,$23,$FA,$AA,$F3,$7E,$FA,$C8,$80,$E7,$88,$16
                db      $83,$4C,$00,$63,$AF,$08,$44,$7E,$FA,$C8,$80,$EA,$83,$53,$00,$83
                db      $34,$44,$7E,$FA,$F8,$80,$EF,$01,$BF,$7E,$EC,$2D,$85,$9E,$01,$8F
                db      $40,$00,$00,$ED,$63,$FB,$FA,$2F,$00,$FF,$F8
;
        ENDIF
;
        IF VDP = V9958
                call    $17a
                rla
                ret     c

                ld      a,$80
                call    $17d
                ; MSX2+ logo version

                ld      de,$c000
                ld      hl,msx2logodata_1
                call    unPack_1

                ld      hl,0
                ld      (BAKCLR),hl
                ld      a,5
                call    $5f

                ld      hl,palette1_1
                call    setpalette_1

                call    $41

                ld      hl,256
                ld      (BAKCLR),hl
                call    $62

                ld      a,(RG8SAV+1)
                and     127
                ld      b,a
                ld      c,9
                call    $47

wait_ce_logo_1: ld      a,2
                ld      ix,$131
                call    $15f
                bit     0,a
                jr      nz,wait_ce_logo_1

                push    de
                ld      bc,15
                ld      hl,logo_hmmc_1
                ldir

                pop     hl
                ld      bc,13
                add     hl,bc
                ld      de,$c000
                ld      a,(de)
                inc     de
                ld      (hl),a
                ex      de,hl

                di
                ld      a,32
                out     ($99),a
                ld      a,128+ 17
                out     ($99),a
                push    hl
                ld      bc,15 *256+ $9b
                ld      hl,logo_hmmc_1
                otir
                pop     hl
                ld      a,128+ 44
                out     ($99),a
                ld      a,128+ 17
                out     ($99),a
                ei

                ld      b,255
                otir

loop_logo_1:    ld      a,2
                ld      ix,$131
                call    $15f
                bit     0,a
                jr      z,done_logo_1

                otir
                jr      loop_logo_1

done_logo_1:    ld      bc,32
                ld      de,$c000
                ld      hl,palette1_1
                ldir

                ld      hl,22 *8
                ld      (GRPACX),hl
                ld      hl,12 *8 +1
                ld      (GRPACY),hl
                ld      a,7
                ld      (FORCLR),a
                ld      a,8
                ld      (LOGOPR),a
                ld      hl,logo_ver_1
                call    prn_text_1
                ld      (LOGOPR),a

                call    $44

palette_loop_1:
                ld      b,16
                ld      de,palette2_1
                ld      hl,$c000
palette_color_1:
                ld      a,(de)                  ; change red
                and     240
                ld      c,a
                ld      a,(hl)
                and     240
                cp      c
                jr      z,palette_red_done_1
                jr      nc,palette_red_down_1
                add     a,16
                jr      palette_red_done_1
palette_red_down_1:
                sub     16
palette_red_done_1:
                ld      c,a
                ld      a,(hl)
                and     15
                or      c
                ld      (hl),a

                ld      a,(de)
                and     15
                ld      c,a
                ld      a,(hl)
                and     15
                cp      c
                jr      z,palette_blue_done_1
                jr      nc,palette_blue_down_1
                inc     a
                jr      palette_blue_done_1
palette_blue_down_1:
                dec     a
palette_blue_done_1:
                ld      c,a
                ld      a,(hl)
                and     240
                or      c
                ld      (hl),a
                inc     de
                inc     hl

                ld      a,(de)
                ld      c,a
                ld      a,(hl)
                cp      c
                jr      z,palette_green_done_1
                jr      nc,palette_green_down_1
                inc     a
                jr      palette_green_done_1
palette_green_down_1:
                dec     a
palette_green_done_1:
                ld      (hl),a
                inc     de
                inc     hl
                djnz    palette_color_1

                ld      hl,$c000
                call    setpalette_1

                ld      b,6
palette_wait_1:
                halt
                djnz    palette_wait_1

                ld      b,32
                ld      de,palette2_1
                ld      hl,$c000
palette_check_1:
                ld      a,(de)
                cp      (hl)
                jr      nz,palette_loop_1
                inc     de
                inc     hl
                djnz    palette_check_1

                ld      b,9
                ld      hl,glare_1
glare_loop_1:
                ld      e,(hl)
                inc     hl
                ld      d,(hl)
                inc     hl
                push    bc
                push    hl
                ex      de,hl
                call    setpalette_1
                pop     hl
                pop     bc

                halt
                halt

                djnz    glare_loop_1

                ret

setpalette_1:   di
                xor     a
                out     ($99),a
                ld      a,128+ 16
                out     ($99),a
                ld      bc,32 *256+ $9a
                otir
                ei
                ret

prn_text_1:
                ld      a,(SCRMOD)
                cp      5
                jr      nc,prn_text_graph_1
prn_text_char_1:
                ld      a,(hl)
                or      a
                ret     z
                call    $a2
                inc     hl
                jr      prn_text_char_1
prn_text_graph_1:
                ld      a,(hl)
                or      a
                ret     z
                ld      ix,$0089
                call    $15f
                inc     hl
                jr      prn_text_graph_1

logo_hmmc_1:    dw      0
                dw      0
                dw      0
                dw      31
                dw      256
                dw      85
col_1:          db      0
                db      0
                db      $f0

palette1_1:     dw      $000,$327,$327,$327,$327,$327,$327,$327
                dw      $327,$327,$327,$327,$327,$327,$327,$327

palette2_1:     dw      $000,$327,$117,$000,$111,$333,$555,$777
                dw      $674,$563,$573,$572,$672,$772,$470,$270

palette3_1:     dw      $000,$327,$117,$000,$111,$333,$555,$777
                dw      $674,$563,$573,$572,$672,$772,$470,$777

palette4_1:     dw      $000,$327,$117,$000,$111,$333,$555,$777
                dw      $674,$563,$573,$572,$672,$772,$777,$270

palette5_1:     dw      $000,$327,$117,$000,$111,$333,$555,$777
                dw      $674,$563,$573,$572,$672,$777,$470,$270

palette6_1:     dw      $000,$327,$117,$000,$111,$333,$555,$777
                dw      $674,$563,$573,$572,$777,$772,$470,$270

palette7_1:     dw      $000,$327,$117,$000,$111,$333,$555,$777
                dw      $674,$563,$573,$777,$672,$772,$470,$270

palette8_1:     dw      $000,$327,$117,$000,$111,$333,$555,$777
                dw      $674,$563,$777,$572,$672,$772,$470,$270

palette9_1:     dw      $000,$327,$117,$000,$111,$333,$555,$777
                dw      $777,$563,$573,$572,$672,$772,$470,$270

palette10_1:    dw      $000,$327,$117,$000,$111,$333,$555,$777
                dw      $674,$777,$573,$572,$672,$772,$470,$270

glare_1:        dw      palette3_1,palette4_1,palette5_1,palette6_1
                dw      palette7_1,palette8_1,palette9_1,palette10_1,palette2_1
;
logo_ver_1:
                db      "V"
                incbin  "../version.txt"
                db      0

;
; Bitbuster by Team Bomba
;

;
; In: HL = source
;     DE = destination
;
unPack_1:
                exx
                ld      bc,128          ; b' = 0 (register loading optimize)
                                        ; c' = bits from bitstream
                exx

unPack_loop_1:
                exx
                call    getBit_1
                exx
                jr      c,unPack_outCompress_1  ; if set, we got LZ77 compression

unPack_outLiteral_1:
                ldi                     ; copy byte from compressed data to destination
                jr      unPack_loop_1   ; handle more compressed data

unPack_outCompress_1:
                ld      a,(hl)          ; get lowest 7 bits of offset, plus the offset
                                        ; extension bit
                inc     hl

                or      a
                jr      z,unPack_outRle_1       ; offset = 0, RLE compression used

unPack_outMatch_1:
                exx
                ld      e,a
                ld      d,b             ; b' should be always clear when entering this part
                rlca                    ; offset extension bit set?
                jr      nc,unPack_outMatch1_1   ; no need to get extra bits if carry not set

                call    getBit_1        ; get offset bit 10
                rl      d
                call    getBit_1        ; get offset bit 9
                rl      d
                call    getBit_1        ; get offset bit 8
                rl      d
                call    getBit_1        ; get offset bit 7

                jr      c,unPack_outMatch1_1    ; since extension mark already makes bit 7 set
                res     7,e             ; only clear it if the bit should be cleared
unPack_outMatch1_1:
                inc     de
                call    getGammaValue_0_1       ; get the match length
                                        ; HL' = length

                push    hl              ; save compressed data pointer
                exx
                push    hl              ; save match length
                push    de              ; save match offset
                exx

                ld      h,d             ; destination in HL
                ld      l,e
                pop     bc              ; load match offset length
                sbc     hl,bc           ; calculate source address
                pop     bc              ; load match length
                ldir

                pop     hl              ; load compressed data pointer
                jr      unPack_loop_1

unPack_outRle_1:
                call    getGammaValue_1
                ret     c               ; HL' = repeat length

                push    hl              ; save compressed data pointer
                exx
                push    hl              ; save repeat length
                exx
                pop     bc              ; load repeat length

                ld      h,d             ; source = destination - 1
                ld      l,e
                dec     hl
                ldir

                pop     hl              ; load compressed data pointer
                jr      unPack_loop_1
;
getBit_1:
                sla     c               ; shift out new bit
                ret     nz              ; if remaining value != 0, we're done

                exx
                ld      a,(hl)          ; get 8 bits from the compressed stream
                inc     hl
                exx

                ld      c,a             ; 8 bits in C'
                sla     c               ; shift out new bit
                inc     c               ; set bit 0 so C' will be zero after shifting 8 times
                ret
;
getGammaValue_1:
                exx                     ; get number of bits used to encode value
getGammaValue_0_1:
                ld      hl,1            ; initial length
                ld      b,1             ; bitcount

getGammaValue_size_1:
                call    getBit_1        ; get more bits
                jr      nc,getGammaValue_sizeEnd_1      ; if bit is not set, bit length is known
                inc     b               ; increase bitcount
                jr      getGammaValue_size_1

getGammaValue_bits_1:
                call    getBit_1        ; get next bit of value from the compressed stream
                adc     hl,hl           ; insert new bit in HL
getGammaValue_sizeEnd_1:
                djnz    getGammaValue_bits_1            ; repeat if more bits to go

getGammaValue_end_1:
                inc     hl              ; correct HL (was stored as length - 2)
                exx
                ret
;
msx2logodata_1:
                db      $7E,$00,$00,$01,$04,$33,$00,$C1,$40,$00,$00,$FD,$A2,$43,$79,$36
                db      $A9,$99,$00,$07,$7F,$34,$7D,$63,$FB,$10,$3A,$99,$9A,$6A,$BB,$00
                db      $BA,$85,$24,$93,$83,$BF,$61,$7E,$0F,$AA,$AA,$BB,$00,$02,$AA,$A9
                db      $80,$3F,$AE,$C7,$7E,$AB,$7E,$8C,$83,$AC,$80,$7F,$59,$7E,$EF,$80
                db      $1F,$D7,$67,$7E,$7D,$AC,$80,$7F,$5B,$7E,$E2,$AA,$D9,$00,$89,$6A
                db      $80,$2B,$85,$7E,$98,$05,$43,$E8,$72,$BA,$AA,$B5,$FD,$57,$89,$88
                db      $63,$BA,$93,$50,$7A,$F9,$FC,$EF,$66,$F6,$D9,$B6,$F7,$61,$0D,$38
                db      $98,$9A,$80,$1D,$4D,$81,$00,$FB,$1F,$7F,$96,$39,$BF,$F6,$0E,$3C
                db      $91,$88,$80,$D2,$A9,$8E,$37,$01,$F5,$5F,$20,$7F,$94,$99,$F6,$7E
                db      $8D,$80,$9E,$90,$26,$8E,$FE,$7F,$8C,$39,$9B,$E1,$7F,$B9,$6C,$E6
                db      $88,$97,$80,$DF,$FC,$7F,$A4,$99,$EC,$F3,$EB,$1E,$82,$38,$8A,$BB
                db      $80,$5F,$77,$7F,$F2,$63,$FE,$B8,$EA,$F5,$96,$80,$66,$93,$7F,$93
                db      $7F,$41,$FE,$1F,$1C,$80,$AE,$8A,$31,$16,$EF,$7F,$FC,$8C,$FE,$7C
                db      $B3,$01,$89,$89,$F8,$9F,$01,$93,$7F,$CF,$1C,$99,$CC,$00,$57,$E9
                db      $EC,$FE,$7F,$E9,$39,$9C,$7E,$C1,$C9,$93,$7F,$FD,$B4,$FE,$7C,$2B
                db      $66,$00,$4E,$E1,$7C,$36,$14,$7F,$7F,$C1,$B9,$9C,$7F,$8F,$FE,$21
                db      $57,$77,$76,$4E,$F0,$BE,$35,$14,$3F,$7F,$E0,$A5,$FE,$E2,$7C,$56
                db      $7F,$8F,$67,$7F,$E5,$07,$04,$3C,$CC,$7F,$28,$33,$FF,$F3,$7C,$66
                db      $7F,$47,$55,$56,$7F,$F2,$64,$43,$CC,$FE,$FC,$11,$E9,$14,$77,$63
                db      $FF,$FD,$39,$04,$33,$FE,$1D,$3F,$93,$7F,$B5,$3A,$03,$EE,$00,$7F
                db      $33,$7F,$6B,$FE,$2F,$17,$7F,$F5,$94,$FD,$D5,$FD,$B9,$F5,$7F,$EF
                db      $79,$01,$C4,$8A,$77,$09,$E7,$01,$CE,$B0,$3A,$7A,$01,$3F,$63,$7F
                db      $21,$38,$7F,$F3,$CE,$44,$77,$00,$7B,$67,$4B,$57,$94,$51,$97,$5C
                db      $1E,$18,$23,$76,$90,$C7,$11,$AF,$01,$0F,$7F,$C8,$27,$43,$3E,$FE
                db      $A7,$E3,$7F,$B7,$52,$95,$67,$7F,$E7,$7E,$78,$66,$C7,$80,$59,$2B
                db      $23,$79,$00,$FE,$7F,$53,$DD,$00,$96,$D9,$E9,$7A,$C8,$59,$67,$02
                db      $65,$55,$DC,$00,$6E,$38,$76,$63,$7F,$E7,$06,$0F,$39,$19,$46,$56
                db      $A1,$23,$66,$91,$8C,$2B,$7A,$55,$00,$FE,$7F,$38,$04,$43,$8D,$F8
                db      $7F,$92,$56,$DC,$E7,$67,$73,$66,$FE,$9D,$45,$98,$19,$DE,$8F,$48
                db      $67,$99,$E5,$66,$87,$2D,$D3,$FF,$E8,$DD,$FD,$FF,$1D,$3D,$C9,$7F
                db      $D2,$66,$8D,$E3,$5F,$6F,$66,$7F,$31,$EB,$BE,$27,$3C,$0C,$DB,$AD
                db      $CF,$7F,$F8,$69,$3B,$FE,$F8,$B6,$7F,$EB,$59,$F0,$66,$F0,$7F,$8E
                db      $8B,$63,$99,$CB,$18,$8F,$7F,$D8,$23,$43,$3D,$FE,$97,$DC,$FB,$FA
                db      $4B,$F1,$5E,$66,$4E,$66,$7F,$A3,$FE,$D2,$99,$37,$7A,$3B,$97,$E8
                db      $53,$20,$FE,$E4,$BD,$DC,$38,$3B,$FE,$1E,$63,$0F,$DD,$7F,$FA,$19
                db      $DF,$A8,$1F,$7F,$B8,$AE,$D7,$FD,$86,$FF,$38,$F7,$F9,$7E,$66,$A5
                db      $89,$8F,$29,$47,$66,$7F,$ED,$D7,$FE,$CB,$7F,$F0,$5F,$FA,$1A,$66
                db      $36,$A7,$EA,$7B,$A9,$AF,$14,$7F,$DC,$8F,$FF,$91,$57,$7F,$EE,$56
                db      $D8,$7F,$EA,$C9,$FE,$76,$9E,$94,$7F,$7F,$69,$44,$7F,$F2,$4E,$D5
                db      $3E,$73,$7F,$1E,$03,$65,$FE,$D7,$7F,$F8,$0B,$FE,$F9,$3E,$7F,$B7
                db      $73,$7F,$C4,$56,$DE,$A0,$BF,$7F,$4A,$FE,$FE,$4F,$7F,$CE,$D3,$A1
                db      $FD,$7D,$BF,$7F,$29,$3A,$7F,$FC,$1C,$80,$9D,$FB,$E9,$8D,$2E,$1F
                db      $7F,$D3,$F2,$A2,$47,$FD,$97,$94,$35,$7F,$F8,$A9,$38,$FF,$E9,$93
                db      $7F,$FB,$8E,$33,$FB,$F4,$D2,$94,$80,$5A,$FC,$7F,$04,$04,$FF,$7C
                db      $A8,$34,$CE,$9E,$53,$66,$A3,$45,$66,$43,$03,$AF,$18,$69,$2A,$64
                db      $F6,$4F,$00,$44,$46,$64,$7E,$43,$7F,$E4,$FF,$7C,$0C,$36,$FF,$D2
                db      $57,$76,$FF,$3E,$2D,$8F,$77,$97,$AD,$0F,$7F,$DB,$CF,$FE,$81,$66
                db      $E5,$9D,$FB,$7F,$EB,$8F,$C1,$7F,$F9,$F3,$FF,$F2,$48,$E5,$65,$97
                db      $FE,$7C,$A8,$25,$E9,$81,$45,$56,$53,$FE,$2E,$F2,$EA,$CC,$AD,$FC
                db      $11,$FF,$FB,$7E,$7F,$E6,$34,$46,$50,$70,$7F,$44,$27,$56,$55,$EA
                db      $D5,$66,$FE,$3F,$0F,$7F,$F0,$32,$FF,$EF,$FE,$29,$FE,$DD,$53,$99
                db      $B4,$FE,$A6,$F9,$7C,$72,$FF,$F9,$68,$AE,$FF,$FC,$18,$FE,$92,$65
                db      $FE,$47,$FF,$29,$34,$1A,$55,$FE,$1E,$84,$54,$43,$FF,$7E,$E9,$33
                db      $8E,$FF,$CF,$17,$7F,$B7,$65,$36,$E9,$CD,$80,$04,$75,$53,$98,$7B
                db      $7C,$01,$11,$A4,$5B,$FF,$E4,$F7,$FF,$C9,$7F,$EB,$65,$37,$55,$65
                db      $7F,$90,$44,$55,$67,$55,$8F,$98,$14,$65,$FE,$7C,$98,$93,$E1,$7F
                db      $F8,$6F,$8A,$7F,$BF,$CF,$2A,$FC,$B6,$65,$55,$8E,$85,$24,$80,$18
                db      $65,$55,$53,$A5,$CA,$55,$67,$8F,$FE,$92,$AF,$47,$44,$7F,$E4,$AB
                db      $38,$80,$E2,$CD,$7F,$CD,$72,$95,$FD,$A7,$53,$85,$28,$06,$88,$C9
                db      $80,$36,$E1,$EA,$19,$25,$C7,$67,$44,$00,$C3,$94,$4E,$1B,$FF,$F8
                db      $56,$80,$F2,$67,$62,$01,$8C,$E3,$8C,$44,$7E,$52,$64,$D4,$CD,$89
                db      $0A,$BC,$EC,$E0,$55,$8D,$8E,$BB,$9A,$00,$E0,$CF,$30,$7F,$DE,$F3
                db      $00,$A7,$22,$F5,$71,$7F,$55,$7F,$62,$ED,$C4,$80,$45,$E6,$18,$66
                db      $CE,$0C,$0B,$BC,$01,$FC,$7E,$3F,$7F,$11,$38,$80,$5F,$14,$CD,$BA
                db      $39,$43,$0F,$CB,$04,$7F,$0E,$C7,$00,$67,$EA,$7E,$00,$37,$33,$7F
                db      $E4,$DF,$80,$CD,$01,$FC,$37,$7F,$E1,$DB,$80,$83,$E3,$7F,$FB,$6B
                db      $80,$6D,$EE,$7F,$FD,$AD,$80,$38,$3F,$EF,$7F,$B3,$52,$00,$AD,$3A
                db      $8A,$7F,$7F,$F3,$7F,$6B,$80,$3E,$0F,$FF,$7F,$EC,$E7,$80,$E9,$3A
                db      $3F,$FF,$00,$FE,$7F,$7E,$80,$7E,$64,$FF,$96,$FF,$03,$7F,$F8,$53
                db      $80,$A4,$39,$9F,$E9,$7C,$51,$FE,$FC,$17,$7F,$E2,$4A,$80,$BA,$99
                db      $6A,$8F,$EA,$3E,$7E,$EB,$95,$F6,$FF,$7F,$1E,$80,$5E,$28,$FE,$FE
                db      $0F,$7F,$F1,$2F,$81,$65,$80,$C1,$FF,$FE,$1F,$C6,$7E,$7F,$2F,$82
                db      $24,$80,$B8,$BD,$FF,$7E,$96,$FF,$FF,$FC,$FC,$80,$AE,$29,$11,$9A
                db      $FF,$F0,$E6,$3F,$F4,$0D,$FE,$7F,$7D,$04,$29,$83,$38,$AF,$FF,$00
                db      $E3,$63,$F4,$FF,$F4,$14,$81,$CE,$83,$24,$80,$7C,$87,$34,$22,$00
                db      $EE,$7E,$7F,$33,$04,$83,$12,$80,$3E,$41,$33,$40,$00,$00,$FD,$22
                db      $44,$80,$3E,$5F,$7E,$E9,$E5,$80,$63,$80,$E2,$3F,$33,$30,$7E,$A6
                db      $96,$80,$23,$80,$5E,$AA,$7D,$43,$AF,$33,$34,$7E,$FA,$88,$80,$EC
                db      $38,$AA,$4B,$AA,$83,$23,$FA,$AA,$F3,$7E,$FA,$C8,$80,$E7,$88,$16
                db      $83,$4C,$00,$63,$AF,$08,$44,$7E,$FA,$C8,$80,$EA,$83,$53,$00,$83
                db      $34,$44,$7E,$FA,$F8,$80,$EF,$01,$BF,$7E,$EC,$2D,$85,$9E,$01,$8F
                db      $40,$00,$00,$ED,$63,$FB,$FA,$2F,$00,$FF,$F8
;
        ENDIF
;
                ds      $c000 - $,$ff
