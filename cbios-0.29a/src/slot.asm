;-------------------------------------
; 000Ch RDSLT
; Reads a value from an address in another slot.
; Input:   A  = slot ID: E000SSPP
;          HL = address to read
; Output:  A  = value read
;          Interrupts disabled.
; Changes: F, C, DE
rdslt:
                push    bc
                push    hl
                push    af
                ld      d,a             ; init D in case call is not made
                and     a               ; expanded slot?
                di
                call    m,select_subslot
                pop     af
                pop     hl

                push    hl              ; HL = address
                push    de              ; D = slot ID, E = saved SSL
                push    hl              ; HL = address
                push    af              ; A = slot ID
                ld      a,h
                rlca
                rlca
                and     $03
                ld      l,a             ; L = page number

                ld      b,a
                ld      a,$FC
                call    rdsft
                ld      e,a             ; E = mask (shifted)
                ld      b,l             ; B = page number
                pop     af              ; A = slot ID
                and     $03
                call    rdsft
                ld      b,a             ; B = primary slot (shifted)
                in      a,(PSL_STAT)
                ld      d,a             ; D = primary slot select for restore
                and     e
                or      b               ; A = primary slot select for read
                pop     hl              ; HL = address

                call    rdprim
                ld      a,e
                pop     de              ; D = slot ID, E = saved SSL
                push    af              ; A = value read
                bit     7,d             ; expanded slot?
                call    nz,restore_subslot
                pop     af              ; A = value read
                pop     hl              ; HL = address
                pop     bc
                ret

rdsft:
                inc     b
                dec     b
                ret     z
rdsft_lp:
                rlca
                rlca
                djnz    rdsft_lp
                ret

;-------------------------------------
; $0014 WRSLT
; Writes a value to an address in another slot.
; Input:   A  = slot ID: E000SSPP
;          HL = address to write
;          E  = value to write
; Output:  Interrupts disabled.
; Changes: AF, BC, D
wrslt:
                push    hl
                ld      d,a             ; D = slot ID
                push    de
                and     a               ; expanded slot?
                di
                call    m,select_subslot
                pop     bc              ; B = slot ID, C = data
                pop     hl
                push    de              ; D = slot ID, E = saved SSL

                push    hl              ; HL = address
                ld      a,h
                rlca
                rlca
                and     $03
                ld      l,a             ; L = page number

                ld      b,a             ; B = page number
                ld      a,$FC
                call    rdsft
                ld      e,a             ; E = mask (shifted)

                ld      b,l             ; B = page number
                ld      a,d
                and     $03             ; A = 000000PP
                call    rdsft
                ld      b,a             ; B = primary slot (shifted)
                in      a,(PSL_STAT)
                ld      d,a             ; D = primary slot select for restore
                and     e
                or      b               ; A = primary slot select for write
                pop     hl              ; HL = address
                ld      e,c             ; E = data
                call    wrprim

                pop     de              ; D = slot ID, E = saved SSL
                push    hl              ; HL = address
                bit     7,d             ; expanded slot?
                call    nz,restore_subslot
                pop     hl
                ret

;-------------------------------------
; $001C CALSLT
; Function : Executes inter-slot call.
; Input    : IY - High byte with input for A in RDSLT
;            IX - The address that will be called
; Remark   : Variables can never be given in alternative registers
;            of the Z-80 or IX and IY

calslt:
                ex      af,af'
                exx

; Select secondary slot of target:
; Note: This approach fails if target is in page 0 of slot 0.1, 0.2 or 0.3.
; TODO: Put slot 0 specific routine in page 3, on the stack if necessary.
                di
                push    iy
                pop     af              ; A = slot ID: E000SSPP
                push    ix
                pop     hl              ; HL = address to call
                ld      d,a             ; init D in case call is not made
                and     a               ; expanded slot?
                call    m,select_subslot
                push    de              ; D = slot ID, E = saved SSL

; Calculate primary slot select value:
                ld      a,d             ; A = slot ID: E000SSPP
                and     $03
                ld      b,a             ; B = primary slot
                ld      c,$FC           ; C = mask
                ; Calculate page that contains call address.
                push    ix
                pop     af              ; A = high byte call address
                rlca
                rlca
                and     $03             ; A = page
                ; Shift B and C page*2 positions to the left.
                add     a,a
                jr      z,calslt_sh2
calslt_sh1:
                rlc     b
                rlc     c
                dec     a
                jr      nz,calslt_sh1
calslt_sh2:

; Select primary slot of target and perform call:
                ld      hl,calslt_restore
                push    hl
                in      a,(PSL_STAT)
                push    af
                and     c               ; C = mask (shifted)
                or      b               ; B = primary slot (shifted)
                exx
                jp      clprim

calslt_restore:
                ex      af,af'
                exx

; Restore secondary slot:
                di
                pop     de              ; D = slot ID, E = saved SSL
                bit     7,d             ; expanded slot?
                call    nz,restore_subslot

; Done:
                ex      af,af'
                exx
                ret

;-------------------------------------
; $0024 ENASLT
; Selects a slot in the page specified by an address.
; Input:   A  = slot ID: ExxxSSPP
;               E  = expanded flag
;               SS = secondary slot number (only if expanded)
;               PP = primary slot number
;          HL = address inside the page to change
; Output:  Interrupts disabled.
; Changes: AF, BC, DE

enaslt:

; A=(A >> 6)&0x3
                di
                push    hl

                ld      l,a             ; L = ExxxSSPP

                and     $03             ; A = 000000PP
                ld      b,a
                ld      a,$AB
psl_dup_lp:
                add     a,$55
                dec     b
                jp      p,psl_dup_lp

                ld      d,a             ; D = PP PP PP PP

                ld      a,h
                rlca
                rlca
                and     $03

                ld      h,a             ; H = page number (0-3)

                ld      b,a

                ld      a,$C0
page_msk_lp:
                rlca
                rlca
                dec     b
                jp      p,page_msk_lp

                ld      e,a             ; E = page mask (00 00 00 11 << page)
                cpl
                ld      c,a             ; C = page mask complement

                ld      a,d
                and     e
                ld      b,a             ; B = 00 00 00 PP << page

                ld      a,l
                and     a
                jp      p,chg_psl

;SSL-Change
                rrca
                rrca
                and     $03             ; A = 000000SS

                push    hl
                push    bc

                ld      b,a
                ld      a,$AB
ssl_dup_lp:
                add     a,$55
                dec     b
                jp      p,ssl_dup_lp

                and     e
                ld      b,a             ; B = 00 00 00 SS << page

                ld      a,d
                and     $C0
                ld      h,a

                in      a,(PSL_STAT)
                ld      l,a
                and     $C0
                or      h
                out     (PSL_STAT),a

                ld      a,(SSL_REGS)
                cpl
                and     c               ; preserve other pages
                or      b
                ld      c,a
                ld      (SSL_REGS),a

                ld      a,l
                out     (PSL_STAT),a

                ; (SLTTBL + PP) <- RegC

                ld      hl,SLTTBL

                ld      a,d
                and     $03             ; A = 000000PP

                add     a,l
                ld      l,a             ; L = L + A

                ld      a,h
                adc     a,0
                ld      h,a             ; H = H + Cy

                ld      a,c
                ld      (hl),a

                pop     bc
                pop     hl

chg_psl:
                in      a,(PSL_STAT)
                and     c
                or      b
                out     (PSL_STAT),a

                pop     hl
                ret

;--------------------------------
; Select subslot.
; Input:   A  = slot ID: E000SSPP
;          HL = address which specifies page to select
;               (actually, only the highest 2 bits of H are relevant)
; Output:  D  = slot ID (same as input)
;          E  = original value of secondary slot select register
;          SLTTBL[slot] = new value of secondary slot select register
; Changes: AF, HL, BC
; Note:    Interrupts must be disabled before calling this routine.
select_subslot:
                ; Select primary slot of target in page 3.
                ; Note: Stack is unavailable until primary slot is restored.
                ld      d,a             ; D = E000SSPP
                rrca
                rrca
                ld      e,a             ; E = PPE000SS
                and     $C0
                ld      l,a             ; L = PP000000
                in      a,(PSL_STAT)
                ld      c,a             ; C = saved PSL
                and     $3F
                or      l
                out     (PSL_STAT),a
                ; Shift mask and subslot according to page.
                ld      a,e             ; A = PPE000SS
                and     $03
                ld      l,a             ; L = subslot
                ld      a,h             ; A = high byte of address
                ld      h,$03           ; H = mask
                jr      select_subslot_next
select_subslot_lp:
                add     hl,hl           ; Shift 2 bits to the left.
                add     hl,hl
select_subslot_next:
                sub     $40             ; Subtract 1 page.
                jr      nc,select_subslot_lp
                ld      a,h
                cpl
                ld      h,a
                ; Select secondary slot of target.
                ld      a,(SSL_REGS)
                cpl
                ld      e,a             ; E = saved SSL
                and     h               ; H = mask (shifted)
                or      l               ; L = subslot (shifted)
                ld      (SSL_REGS),a
                ld      l,a             ; L = value written to SSL_REGS
                ; Restore original primary slot in page 3.
                ld      a,c
                out     (PSL_STAT),a
                ; Update SLTTBL.
                ld      a,d
                and     $03             ; A = 000000SS
                ld      c,a
                ld      b,0
                ld      a,l             ; A = value written to SSL_REGS
                ld      hl,SLTTBL
                add     hl,bc
                ld      (hl),a
                ret

;--------------------------------
; Restore subslot, companion routine to select_subslot.
; Input:   D  = slot ID: E000SSPP
;          E  = original value of secondary slot select register
; Output:  SLTTBL[slot] = original value of secondary slot select register
; Changes: AF, HL, BC
; Note:    Interrupts must be disabled before calling this routine.
restore_subslot:
                ; Select primary slot of target in page 3.
                ; Note: Stack is unavailable until primary slot is restored.
                ld      a,d
                rrca
                rrca
                and     $C0
                ld      b,a             ; B = PP000000
                in      a,(PSL_STAT)
                ld      c,a             ; C = saved PSL
                and     $3F
                or      b
                out     (PSL_STAT),a
                ; Restore secondary slot.
                ld      a,e
                ld      (SSL_REGS),a
                ; Restore original primary slot in page 3.
                ld      a,c
                out     (PSL_STAT),a
                ; Update SLTTBL.
                ld      a,d
                and     $03             ; A = 000000SS
                ld      c,a
                ld      b,0
                ld      hl,SLTTBL
                add     hl,bc
                ld      (hl),e
                ret

;--------------------------------

m_rdprim:
                out     (PSL_STAT),a
                ld      e,(hl)
                jr      m_wrprm1
m_wrprim:
                out     (PSL_STAT),a
                ld      (hl),e
m_wrprm1:
                ld      a,d
                out     (PSL_STAT),a
                ret
m_clprim:
                out     (PSL_STAT),a
                ex      af,af'
                call    cl_jp
                ex      af,af'
                pop     af
                out     (PSL_STAT),a
                ex      af,af'
                ret
m_cl_jp:
                jp      (ix)
m_prim_end:
                nop

rdprim:         equ     $F380
wrprim:         equ     rdprim+(m_wrprim-m_rdprim)
clprim:         equ     rdprim+(m_clprim-m_rdprim)
cl_jp:          equ     rdprim+(m_cl_jp-m_rdprim)

; vim:ts=8:expandtab:filetype=z8a:syntax=z8a:
