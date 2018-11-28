; CHPUT routine for C-BIOS
;
; Copyright (c) 2006 Eric Boon.  All rights reserved.
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
; $00A2 CHPUT
; Function : Output character in A to screen
; Input    : A = character code
; Output   : -
; Changes  : -

chput:
                push    hl                      ; save all regs
                push    de
                push    bc
                push    af
                call    H_CHPU                  ; call hook
                ld      a,(SCRMOD)              ; this only works in
                cp      2                       ; screen modes 0 and 1
                jr      nc,chput_exit
                pop     af                      ; restore char to put in A
                push    af
                call    chput_remove_cursor
                pop     af                      ; restore char to put in A
                push    af
                call    chput_decode_char
                call    chput_restore_cursor
                ld      a,(CSRX)                ; CSRX -> TTYPOS
                ld      (TTYPOS),a
chput_exit:
                pop     af
                pop     bc
                pop     de
                pop     hl
                ret

; -- decode character in A
chput_decode_char:                              
                call    cnvchr                  ; Graphic character?
                ret     nc                      ; NC  -> graphic extension hdr
                jr      z,chput_putchar         ; C,Z -> graphic character
                push    af                      ; (C,NZ -> normal char)
                ld      a,(ESCCNT)              ; ESC sequence?
                or      a
                jp      nz,chput_escape
                pop     af
                cp      $20                     ; Control code?
                jr      c,chput_ctrl_search
                cp      127                     ; DEL? 
                jp      z,chput_erase
        
; -- print a normal character and move cursor to next position
chput_putchar:
                call    curs2hl                 ; output character to screen
                call    wrtvrm
                
                ld      hl,(CSRY)               ; h = (CSRX), l = (CSRY)
                ld      a,(LINLEN)
                inc     h
                cp      h
                jr      c,chput_continue_line
                ld      (CSRY),hl
                ret

chput_continue_line:
                ld      de,LINTTB-1             ; make logical line continue
                ld      h,0
                add     hl,de
                xor     a
                ld      (hl),a

                ld      hl,(CSRY)               ; move cursor to start of
                call    chput_ctrl_cr           ; new line
                jp      chput_ctrl_lf

; -- Handle control code
chput_ctrl_search:
                ld      b,12
                ld      hl,chput_ctrl_table
                jp      search_table 

; -- Fill with spaces until next TAB stop
chput_ctrl_tab:
                ld      a,$20
                call    chput_putchar
                ld      a,(CSRX)
                and     7
                cp      1
                jr      nz,chput_ctrl_tab
                ret

; -- Line Feed.
chput_ctrl_lf:
                ld      hl,(CSRY)
                ld      a,(CRTCNT)
                inc     l
                cp      l
                jr      nc,chput_ctrl_lf_done

                push    hl
                call    chput_ctrl_home         ; home cursor
                call    chput_esc_m             ; delete top line (scroll!)
                pop     hl
                dec     l

chput_ctrl_lf_done:
                ld      (CSRY),hl
                ret

; -- Home cursor
chput_ctrl_home:
chput_esc_h:
                ld      hl,$0101
                ld      (CSRY),hl
                ret

; -- Form Feed / Cls
chput_ctrl_ff:  equ     cls
chput_esc_e:    equ     cls
chput_esc_j:    equ     cls

; -- Clear till end of screen
chput_esc_jj:
                call    chput_esc_k             ; clear till end of line
                ld      hl,(CSRY)               ; save current cursor pos
                push    hl
                call    chput_ctrl_cr           ; move to line start
chput_esc_jj_loop:
                ld      a,(CSRY)                ; while no at end of screen
                ld      hl,CRTCNT
                cp      (hl)
                jr      nc,chput_esc_jj_done
                call    chput_ctrl_lf           ;   move to next line
                call    chput_esc_k             ;   clear till end of line
                jr      chput_esc_jj_loop       ; loop
chput_esc_jj_done:
                pop     hl                      ; restore cursor pos
                ld      (CSRY),hl
                ret
                
; -- Carriage return
chput_ctrl_cr:
                ld      a,1
                ld      (CSRX),a
                ret

; -- Escape
chput_ctrl_esc:
                ld      a,$FF
                ld      (ESCCNT),a
                ret

; -- Cursor right
chput_ctrl_right:
                ld      a,(CSRX)
                ld      hl,LINLEN
                cp      (hl)
                jr      nc,chput_ctrl_right_next
                inc     a
                jr      chput_right_left_ok
chput_ctrl_right_next:
                ld      a,(CSRY)
                ld      hl,CRTCNT
                cp      (hl)
                ret     nc
                inc     a
                ld      (CSRY),a
                jr      chput_ctrl_cr

; -- Cursor left
chput_ctrl_bs:
chput_ctrl_left:
                ld      a,(CSRX)
                dec     a
                jr      nz,chput_right_left_ok
                ld      a,(CSRY)
                dec     a
                ret     z
                ld      (CSRY),a
                ld      a,(LINLEN)
chput_right_left_ok:
                ld      (CSRX),a
                ret
                
; -- Cursor up
chput_ctrl_up:
chput_esc_a:
                ld      a,(CSRY)
                dec     a
                ret     z
                ld      (CSRY),a
                ret

; -- Cursor down
chput_ctrl_down:
chput_esc_b:
                ld      a,(CSRY)
                ld      hl,CRTCNT
                cp      (hl)
                ret     nc
                inc     a
                ld      (CSRY),a
                ret

; -- Handle ESC mode (ESCCNT in A and != 0)
chput_escape:
                ld      b,a                     ; b := (ESCCNT)
                inc     a                       ; (ESCCNT) == -1 ?
                jr      nz,chput_escape_1
                ld      (ESCCNT),a
                pop     af                        ; restore character in A 
                ld      b,15                    ; search in table
                ld      hl,chput_esc_table
                jp      search_table

chput_escape_1: ; ----------------------------
                pop     af
                djnz    chput_escape_2
                
                ; -- ESCCNT == 1: 'ESC x <n>'
                ld      c,0                     ; CSTYLE/CSRSW := 0
                jr      chput_esc_xy

chput_escape_2: ; ----------------------------
                djnz    chput_escape_3

                ; -- ESCCNT == 2: 'ESC y <n>'
                ld      c,1                     ; CSTYLE/CSRSW := 1

chput_esc_xy:
                cp      '4'
                jr      z,chput_esc_xy_4
                cp      '5'
                jr      z,chput_esc_xy_5
                jr      chput_escape_reset
chput_esc_xy_4:
                ld      a,c
                ld      (CSTYLE),a
                jr      chput_escape_reset
chput_esc_xy_5:
                ld      a,c
                ld      (CSRSW),a
                jr      chput_escape_reset

chput_escape_3: ; ----------------------------
                djnz    chput_escape_4

                ; -- ESCCNT == 3: 'ESC Y <n> <m>'
                ld      b,$1F
                sub     b
                ld      (CSRX),a
                jr      chput_escape_reset

chput_escape_4: ; ----------------------------
                djnz    chput_escape_reset

                ; -- ESCCNT == 4: 'ESC Y <n>'
                ld      b,$1F
                sub     b
                ld      (CSRY),a
                ld      a,3
                jr      chput_escape_set

; -- ESCCNT := 1
chput_esc_x:
                ld      a,1
                jr      chput_escape_set

; -- ESCCNT := 2
chput_esc_y:
                ld      a,2
                jr      chput_escape_set

; -- ESCCNT := 4
chput_esc_yy:
                ld      a,4
                jr      chput_escape_set

chput_escape_reset:
                xor     a
chput_escape_set:
                ld      (ESCCNT),a
                ret

; -- Cursor right, no wrap
chput_esc_c:
                ld      a,(CSRX)
                ld      hl,LINLEN
                cp      (hl)
                ret     nc
                inc     a
                ld      (CSRX),a
                ret

; -- Cursor left, no wrap
chput_esc_d:
                ld      a,(CSRX)
                dec     a
                ret     z
                ld      (CSRX),a
                ret

; -- clear line
chput_esc_l:
                call    chput_ctrl_cr

; -- Clear till end of line
chput_esc_k:
                ld      hl,LINTTB-1             ; update LINTTB
                ld      a,(CSRY)
                ld      e,a
                ld      d,0
                add     hl,de
                ; a != 0, which is OK
                ld      (hl),a
                
                ld      a,(LINLEN)
                inc     a                       ; because CSRX is 1-based
                ld      hl,CSRX
                sub     (hl)
                ld      c,a
                ld      b,0
                ld      a,32
                call    curs2hl
                jp      filvrm
                
; -- Insert line
chput_esc_ll:
                call    chput_ctrl_cr           ; move to start of line
                ld      hl,(CSRY)               ; save current cursor pos
                push    hl
                ld      b,l
                ld      a,(CRTCNT)
                ld      (CSRY),a
                sub     b
                ld      b,a
                inc     b
                ld      a,(CSRY)
                jr      chput_esc_ll_loop_end

chput_esc_ll_loop:
                call    curs2hl
                ex      de,hl
                dec     a
                ld      (CSRY),a
                call    curs2hl
                call    chput_copy_line
chput_esc_ll_loop_end:
                djnz    chput_esc_ll_loop

                pop     hl                      ; restore cursor position
                ld      (CSRY),hl
                ld      h,0
                ld      a,(CRTCNT)              ; update LINTTB
                ld      d,a                     ; DE := (CRTCNT)
                ld      e,0
                sub     l                       ; BC := (CRTCNT) - (CSRY) - 1
                dec     a
                ld      c,a
                ld      b,0
                ld      hl,LINTTB-1             ; DE := LINTTB + (CRTCNT)
                add     hl,de
                ex      de,hl
                ld      h,d                     ; HL := DE - 1
                ld      l,e
                dec     hl                     
                lddr
                jp      chput_esc_k

; -- Delete line (and scroll rest up)
chput_esc_m:
                call    chput_ctrl_cr           ; move to start of line
                ld      hl,(CSRY)
                push    hl                      ; save cursor pos
                ld      b,l
                ld      a,(CRTCNT)
                sub     b
                ld      b,a
                inc     b
                ld      a,(CSRY)
                jr      chput_esc_m_loop_end

chput_esc_m_loop:
                call    curs2hl                 ;   Copy 1 line:
                ex      de,hl                   ;     de = dest in VRAM
                inc     a                       ;     next line
                ld      (CSRY),a
                call    curs2hl                 ;     hl = src in VRAM
                call    chput_copy_line
chput_esc_m_loop_end:
                djnz    chput_esc_m_loop        ; endloop

                call    chput_esc_k             ; clear till end of line
                pop     hl                      ; restore cursor position
                ld      (CSRY),hl

                ld      h,0                     ; update LINTTB
                ld      a,(CRTCNT)              ; BC := (CRTCNT) - (CRSY) - 1
                sub     l                       
                dec     a
                ld      c,a
                ld      b,0
                ld      de,LINTTB-1             ; DE := LINTTB + (CSRY)
                add     hl,de
                ld      d,h
                ld      e,l
                inc     hl                      ; HL := DE + 1
                ldir
                ret

; -- Copy line: from HL to DE
chput_copy_line:
                push    af
                push    bc
                ld      b,0
                ld      a,(LINLEN)
                ld      c,a
                
        IF MODEL_MSX != MODEL_MSX1
                cp      41
                jr      c,chput_copy_line_2
                ld      c,40
                call    chput_copy_line_copy
                ld      a,(LINLEN)
                sub     40
                ld      c,a
chput_copy_line_2:
        ENDIF

                call    chput_copy_line_copy
                pop     bc
                pop     af
                ret

chput_copy_line_copy:
                push    hl
                push    de
                push    bc
                ld      de,LINWRK
                call    ldirmv
                pop     bc
                pop     de
                push    de
                push    bc
                ld      hl,LINWRK
                call    ldirvm
                pop     bc
                pop     hl
                add     hl,bc
                ex      de,hl
                pop     hl
                add     hl,bc
                ret

; -- Erase
chput_erase:
                ld      a,(CSRX)
                cp      1
                ret     z
                ld      a,32
                call    chput_putchar
                jp      chput_ctrl_left

; -- disable cursor
chput_remove_cursor:
                ld      a,(CSRSW)               ; Cursor visible?
                cp      1
                ret     nz
                ld      a,(SCRMOD)              ; Are we in text mode?
                cp      2
                ret     nc

                ld      a,(CURSAV)              ; get saved character
                call    curs2hl                 ; and drop it at the
                jp      wrtvrm

; -- enable cursor
chput_restore_cursor:
                ld      a,(CSRSW)               ; Cursor visible?
                cp      1
                ret     nz
                ld      a,(SCRMOD)
                cp      2
                ret     nc

                call    curs2hl                 ; get character at cursor
                call    rdvrm                   ; and store at CURSAV
                ld      (CURSAV),a
                
                and     a                       ; reset carry
                ld      d,0                     ; de := 8 * a
                ld      e,a
                rl      e
                rl      d
                rl      e
                rl      d
                rl      e
                rl      d
                xor     a                       ; get pattern table address
                ld      hl,SCRMOD
                cp      (hl)
                jr      nz,chput_restore_cursor_t32

                ld      hl,(TXTCGP)
                jr      chput_restore_cursor_getpattern

chput_restore_cursor_t32:
                ld      hl,(T32CGP)

chput_restore_cursor_getpattern:
                push    hl
                add     hl,de                   ; add offset of character
                ld      de,LINWRK               ; copy pattern to LINWRK
                ld      bc,8
                call    ldirmv

                ld      a,(CSTYLE)              ; depending on CSTYLE
                cp      0
                jr      nz,chput_restore_cursor_ins
                ld      hl,LINWRK               ; invert the complete pattern
                ld      b,8
                jr      chput_restore_cursor_invert
chput_restore_cursor_ins:
                ld      hl,LINWRK+6             ; or only the lower 2 lines
                ld      b,2

chput_restore_cursor_invert:
                ld      a,(hl)                  ; invert!
                cpl
                ld      (hl),a
                inc     hl
                djnz    chput_restore_cursor_invert
                pop     hl                      ; copy inverted pattern to
                ld      de,255*8                ; pattern 255
                add     hl,de
                ex      de,hl
                ld      hl,LINWRK
                ld      bc,8
                call    ldirvm

                call    curs2hl                 ; place char 255 at cursor pos
                ld      a,255
                jp      wrtvrm

; -- Control character search table
chput_ctrl_table:
                db      7
                dw      beep ; chput_ctrl_beep
                db      8
                dw      chput_ctrl_bs
                db      9
                dw      chput_ctrl_tab
                db      10
                dw      chput_ctrl_lf
                db      11
                dw      chput_ctrl_home
                db      12
                dw      chput_ctrl_ff
                db      13
                dw      chput_ctrl_cr
                db      27
                dw      chput_ctrl_esc
                db      28
                dw      chput_ctrl_right
                db      29
                dw      chput_ctrl_left
                db      30
                dw      chput_ctrl_up
                db      31
                dw      chput_ctrl_down

; -- Escape character search table
chput_esc_table:
                db      'j'
                dw      chput_esc_j
                db      'E'
                dw      chput_esc_e
                db      'K'
                dw      chput_esc_k
                db      'J'
                dw      chput_esc_jj
                db      'l'
                dw      chput_esc_l
                db      'L'
                dw      chput_esc_ll
                db      'M'
                dw      chput_esc_m
                db      'Y'
                dw      chput_esc_yy
                db      'A'
                dw      chput_esc_a
                db      'B'
                dw      chput_esc_b
                db      'C'
                dw      chput_esc_c
                db      'D'
                dw      chput_esc_d
                db      'H'
                dw      chput_esc_h
                db      'x'
                dw      chput_esc_x
                db      'y'
                dw      chput_esc_y

; vim:ts=8:expandtab:filetype=z8a:syntax=z8a:
