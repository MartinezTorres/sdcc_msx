; C-BIOS Disk ROM - based on WD2793 FDC
;
; Copyright (c) 2004 Albert Beevendorp.  All rights reserved.
; Copyright (c) 2005 Maarten ter Huurne.  All rights reserved.
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

                include "hooks.asm"
                include "systemvars.asm"
                include "hardware.asm"

calslt:         equ     $001C
enaslt:         equ     $0024
chput:          equ     $00A2

; Disk Transfer Area.
DTA_ADDR:       equ     $F23D

; For each page, the slot in which RAM is located.
RAM_PAGE0:      equ     $F341
RAM_PAGE1:      equ     $F342
RAM_PAGE2:      equ     $F343
RAM_PAGE3:      equ     $F344

; BDOS entry point.
BDOS_ENTRY:     equ     $F37D

; Actual place where the BDOS inter slot call is made.
; The entry point is just 3 bytes, inter slot call requires 5.
H_BDOS:         equ     $F331

; Number of drives on interface.
NUM_DRIVES:     equ     1

                org     $4000

                db      "AB"
                dw      init            ; init
                dw      0               ; statement
                dw      0               ; device
                dw      0               ; basic text
                dw      0,0,0

; $4010 DSKIO
                ds      $4010 - $,$FF
                jp      dskio

; $4013 DSKCHG
                ds      $4013 - $
                jp      dskchg

; $4016 GETDPB
                ds      $4016 - $
                jp      getdpb

; $4019 CHOICE
                ds      $4019 - $
                jp      choice

; $401C DSKFMT
                ds      $401C - $
                jp      dskfmt

; $401F LOC_DS - stop motor of drives connected to this interface
                ds      $401F - $
                jp      loc_ds

; $4022 BASIC
                ds      $4022 - $
                jp      basic

                scf
; $4026 FORMAT
                ds      $4026 - $
                jp      format

; $4029 DSKSTP - stop motor of drives on all interfaces
                ds      $4029 - $
                jp      dskstp

; $402D DSKSLT
                ds      $402D - $,$00
                jp      dskslt

;--------------------------------
init:
                ld      hl,init_text
                call    print_debug

; Init variables:

                ; Init RAM_PAGEx.
                ; Note: Should this happen in disk ROM or main ROM?
                ; We assume that the same slot that is chosen by the main ROM
                ; to hold the system vars provides RAM in all pages.
                ; This is not true for all MSX models, but it is for most.
                ; TODO: Search for RAM for each page separately.
                in      a,(PSL_STAT)
                and     $C0
                rlca
                rlca
                ld      c,a             ; C = 000000PP
                ld      a,(SSL_REGS)
                cpl
                and     $C0
                rrca
                rrca
                rrca
                rrca                    ; A = 0000SS00
                or      c               ; A = 0000SSPP
                ld      b,0
                ld      hl,EXPTBL
                add     hl,bc
                or      (hl)            ; A = E000SSPP
                ld      hl,RAM_PAGE0
                ld      (hl),a
                inc     hl
                ld      (hl),a
                inc     hl
                ld      (hl),a
                inc     hl
                ld      (hl),a

                ; Init DRVINF.
                ; TODO: Cooperate with other disk ROMs.
                ld      hl,DRVINF
                ld      (hl),NUM_DRIVES
                inc     hl
                push    hl
                call    dskslt
                pop     hl
                ld      (hl),a

                ; Init DTA.
                call    resetdta

; Setup hooks:

                call    dskslt
                ; BDOS
                ld      de,bdos
                ld      hl,H_BDOS
                call    init_sethook
                ; PHYDIO
                ld      de,phydio
                ld      hl,H_PHYD
                call    init_sethook
                ; FORMAT
                ld      de,format
                ld      hl,H_FORM
                call    init_sethook
                ; boot loader
                ld      de,boot
                ld      hl,H_RUNC
                call    init_sethook

                ; Setup BDOS entry point.
                ld      a,$C3           ; jp
                ld      hl,H_BDOS
                ld      (BDOS_ENTRY),a
                ld      (BDOS_ENTRY + 1),hl

                ; Init megarom mapper.
                ld      a,1
                ld      ($6000),a

                ret

init_sethook:
                ld      (hl),$F7        ; rst $30
                inc     hl
                ld      (hl),a          ; slot ID
                inc     hl
                ld      (hl),e          ; addr low
                inc     hl
                ld      (hl),d          ; addr high
                inc     hl
                ld      (hl),$C9        ; ret
                ret

init_text:      db      "C-DISK is initializing",0

;--------------------------------
boot:
                ld      hl,boot_text
                call    print_debug

                ld      hl,$C000        ; address to load to
                ld      de,$0000        ; boot sector
                ld      bc,$01F9        ; 1 sector, 720K disk
                xor     a               ; drive 0, read (NC)
                call    dskio
                ret     c               ; error -> abort boot

                ; TODO: Perform sanity checks on boot sector contents?

                ; TODO: Put RAM in page 0 and set up minimal call environment.

                ; TODO: Since the bootsector routine checks CF, I assume it is
                ;       called two times: first time with CF reset, second
                ;       time with CF set. But I don't know the difference
                ;       between the two.
                ;and     a
                ;call    $C01E
                ld      hl,0            ; a pointer will be written here
                ld      de,0            ; ???
                ld      a,0             ; ???
                scf
                call    $C01E

                ret

boot_text:      db      "C-DISK booting",0

;--------------------------------
; DSKIO
; Input:   F  = NC to read, C to write
;          A  = Drive number (0=A:)
;          B  = Number of sectors to transfer
;          C  = Media descriptor
;          DE = Logical sector number
;          HL = Transfer address
;
; Output:  F  = NC if successful, C if error
;          A  = Error code if error
;               0 = Write protected
;               2 = Not ready
;               4 = Data (CRC) error
;               6 = Seek error
;               8 = Record not found
;              10 = Write fault
;              12 = Other errors
;          B  = Always the number of sectors transferred
; NOTE: This routine is still stubbed
dskio:
                call    dskio_debug

                ; Check whether the drive exists.
                push    af
                cp      NUM_DRIVES
                jr      c,dskio_drive_ok
                pop     af
                ld      a,12
                scf
                ret
dskio_drive_ok:
                pop     af

                ; Read or write?
                jp      c,dskio_write

dskio_read_loop:
                push    de
                push    bc
                call    load_sector
                pop     bc
                pop     de
                inc     de              ; next sector
                djnz    dskio_read_loop
                and     a               ; CF = 0
                call    dskio_done
                ret

; Load 1 sector.
; Input:   HL = address to load to
;          DE = sector number
; Output:  HL = updated address to load to
; Changes: AF, DE, BC
; TODO: Loading will fail if a sector is loaded across a page boundary.
;       Probably the only decent way to fix this is loading to a RAM buffer
;       and LDIR-ing in two steps.
load_sector:
                ld      a,h
                cp      $40
                jr      c,load_sector_skip
                cp      $80
                jr      c,load_sector_page1
load_sector_skip:
                ex      de,hl           ; DE = address to load to
                add     hl,hl           ; HL = sectornr * 2
                ld      b,l
                add     hl,hl
                add     hl,hl
                add     hl,hl
                ld      a,h
                add     a,2
                ld      ($6000),a       ; page nr
                ld      a,b
                and     $1F
                ld      b,a
                ld      c,0             ; BC = offset in page
                ld      hl,$6000
                add     hl,bc
                ld      bc,$0200        ; 512 bytes per sector
                ldir
                ex      de,hl           ; HL = updated address to load to
                ld      a,1
                ld      ($6000),a
                ret

; Load 1 sector to an address in page 1.
load_sector_page1:
                ; TODO: Determine slot currently active in page 2.
                ld      a,(RAM_PAGE2)   ; RAM slot
                push    af
                ; Select disk ROM in page 2.
                push    hl
                push    de
                call    dskslt
                push    af
                ld      h,$80
                call    enaslt
                ; MegaROM bank 0 and 1.
                xor     a
                ld      ($8000),a
                inc     a
                ld      ($A000),a
                pop     af
                pop     de
                pop     hl
                ; Call routine which runs in page 2.
                call    load_sector_page1_high + $4000
                ; Restore slot in page 2.
                pop     af
                push    hl
                ld      h,$80
                call    enaslt
                pop     hl
                ret

load_sector_page1_high:
                push    af              ; A = disk ROM slot
                push    hl
                push    de
                ; Select RAM in page 1.
                ; Note that this will only allow loading into the primary
                ; mapper; does the MSX disk ROM have the same limitation?
                ; If not, how does it know which slot to load to?
                ld      a,(RAM_PAGE1)
                ld      h,$40
                call    enaslt
                pop     hl              ; HL = sector number
                pop     de              ; DE = address to load to
                add     hl,hl           ; HL = sectornr * 2
                ld      b,l
                add     hl,hl
                add     hl,hl
                add     hl,hl
                ld      a,h
                add     a,2
                ld      ($A000),a       ; page nr
                ld      a,b
                and     $1F
                ld      b,a
                ld      c,0             ; BC = offset in page
                ld      hl,$A000
                add     hl,bc
                ld      bc,$0200        ; 512 bytes per sector
                ldir
                ex      de,hl           ; HL = updated address to load to
                ld      a,1
                ld      ($A000),a
                ; Restore disk ROM in page 1.
                pop     af
                push    hl
                ld      h,$40
                call    enaslt
                pop     hl
                ret

dskio_write:
                ; write protect error
                xor     a
                scf
                call    dskio_done
                ret

dskio_debug:
                push    hl
                push    af
                ld      a,$23           ; ASCII mode
                out     (DBG_CTRL),a
                ld      hl,dskio_text_1
                call    print_debug_asciiz
                pop     af
                push    af
                ld      hl,dskio_text_wr
                jr      c,dskio_debug_write
                ld      hl,dskio_text_rd
dskio_debug_write:
                call    print_debug_asciiz
                ld      hl,dskio_text_2
                call    print_debug_asciiz
                ld      l,e
                ld      h,d
                call    print_debug_hexword
                ld      hl,dskio_text_3
                call    print_debug_asciiz
                ld      a,b
                call    print_debug_hexbyte
                ld      hl,dskio_text_4
                call    print_debug_asciiz
                pop     af
                pop     hl
                push    hl
                push    af
                call    print_debug_hexword
                ld      hl,dskio_text_5
                call    print_debug_asciiz
                ld      a,c
                call    print_debug_hexbyte
                ld      a,$00           ; flush
                out     (DBG_CTRL),a
                pop     af
                pop     hl
                ret
dskio_text_1:   db      "disk: ",0
dskio_text_rd:  db      "READ",0
dskio_text_wr:  db      "WRITE",0
dskio_text_2:   db      " sectors: first $",0
dskio_text_3:   db      ", num $",0
dskio_text_4:   db      ", to $",0
dskio_text_5:   db      ", media $",0

;--------------------------------
; DSKCHG
; Input:   A  = Drive number (0=A:)
;          B  = Media Descriptor
;          C  = Media Descriptor
;          HL = Base address of DPB
;
; Output:  F  = NC if successful, C if error
;          A  = Error code if error
;               0 = Write protected
;               2 = Not ready
;               4 = Data (CRC) error
;               6 = Seek error
;               8 = Record not found
;              10 = Write fault
;              12 = Other errors
;          B  = Disk Change state if successful
;              -1 = Disk changed
;               0 = Unknown
;               1 = Disk unchanged
; Note:    If the disk has been changed or may have been changed (unknown)
;          read the bootsector or the first FAT sectoe for a disk media
;          descriptor and transfer a new DPB as with GETDPB.
; NOTE: This routine is still stubbed
dskchg:
                push    hl
                push    af
                ld      hl,dskchg_text
                call    print_debug
                pop     af
                pop     hl
                ld      b,0             ; unknown whether changed or not
                ret
dskchg_text:    db      "disk: DSKCHG ($4013) called",0

;--------------------------------
; GETDPB
; Input:   A  = Drive number (0=A:)
;          B  = First byte of FAT (media descriptor)
;          C  = Media descriptor
;          HL = Base address of DPB
;
; Output:  HL = DPB filled in
; Note:    DPB consists of:
;          Name    Offset    Size    Description
;          --------------------------------------------------
;          MEDIA      $00       1    Media type ($F8 - $FF)
;          SECSIZE    $01       2    Sector size (must be 2^n)
;          DIRMSK     $03       1    (SECSIZ / 32 - 1)
;          DIRSHFT    $04       1    Number of one bits in DIRMSK
;          CLUSMSK    $05       1    (Sectors per cluster - 1)
;          CLUSSHFT   $06       1    (Number of one bits in CLUSMSK) - 1
;          FIRFAT     $07       2    Logical sector number of first FAT
;          FATCNT     $09       1    Number of FATs
;          MAXENT     $0A       1    Number of root directory entries
;          FIRREC     $0B       2    Logical sector number of first data
;          MAXCLUS    $0D       2    (Number of clusters) + 1
;                                    This excludes the number of reserved,
;                                    FAT and root directory sectors.
;          FATSIZ     $0F       1    Number of sectors used for FAT
;          FIRDIR     $10       2    Logical sector number of first directory
; NOTE: This routine is still stubbed
getdpb:
                push    hl
                push    af
                ld      hl,getdpb_text
                call    print_debug
                pop     af
                pop     hl
                ret
getdpb_text:    db      "disk: GETDPB ($4016) called",0

;--------------------------------
; CHOICE
; Output:  HL = Address of ASCIIz string containing the text with choices
;               for DSKFMT. If there are no choices (only one format sup-
;               ported) HL=0
choice:
                ld      hl,choice_text
                ret
choice_text:    db      13,10,"1 - Single sided, 80 tracks"
                db      13,10,"2 - Double sided, 80 tracks"
                db      13,10,0

;--------------------------------
; DSKFMT
; Input:   A  = Choice specified by user: 1-9. See CHOICE
;          D  = Drive number (0=A:)
;          BC = Length of work area
;          HL = Base address of work area
;
; Output:  F  = NC if successful, C if error
;          A  = Error code if error
;               0 = Write protected
;               2 = Not ready
;               4 = Data (CRC) error
;               6 = Seek error
;               8 = Record not found
;              10 = Write fault
;              12 = Bad parameter
;              14 = Insufficient memory
;              16 = Other errors
; Note:    Also write MSX bootsector at sector 0, clears all FATs (media
;          descriptor ar first byte, $FF at the second/third byte and
;          rest filled with $00) and clears the root directory (full $00).
; NOTE: This routine is still stubbed
dskfmt:
                push    hl
                push    af
                ld      hl,dskfmt_text
                call    print_debug
                pop     af
                pop     hl
                scf                     ; error, because we didn't format
                ret
dskfmt_text:    db      "disk: DSKFMT ($401C) called",0

;--------------------------------
; LOC_DS
; Note:    Stop motor for all drives on THIS interface.
loc_ds:
                push    hl
                push    af
                ld      hl,loc_ds_text
                call    print_debug
                pop     af
                pop     hl
                ret
loc_ds_text:    db      "disk: LOC_DS ($401F) called",0

;--------------------------------
; BASIC
; Note:    Warmboots to BASIC.
; NOTE: This routine is still stubbed
basic:
                push    hl
                push    af
                ld      hl,basic_text
                call    print_debug
                pop     af
                pop     hl
                ret
basic_text:     db      "disk: BASIC ($4022) called",0

;--------------------------------
; FORMAT
; Note:    Like CALL FORMAT, FORMAT (DOS) and BIOS routine $0147.
;          Display CHOICE, wait for input and do DSKFMT.
; NOTE: This routine is still stubbed
format:
                push    hl
                push    af
                ld      hl,format_text
                call    print_debug
                pop     af
                pop     hl
                ret
format_text:    db      "disk: FORMAT ($4026) called",0

;--------------------------------
; DSKSTP
; Note:    Stop motor for all drives on all interfaces. Interslot-calls
;          LOC_DS for all detected interfaces.
; NOTE: This routine is still stubbed
dskstp:
                push    hl
                push    af
                ld      hl,dskstp_text
                call    print_debug
                pop     af
                pop     hl
                ret
dskstp_text:    db      "disk: DSKSTP ($4029) called",0

;--------------------------------
; $402D DSKSLT
; Calculate slot ID for disk ROM slot.
; Output:  A = slot ID
; Changes: F, HL, BC
; TODO: Old description said this:
;       Output:  Address $F348 keeps the slot where the DISK-ROM is found.
dskslt:
                ; TODO: Calculate this dynamically.
                ld      a,$8F           ; slot 3.3
                ret

;--------------------------------
; PHYDIO
phydio:
                ; TODO: Support multiple disk ROMs.
                jp      dskio

;--------------------------------
; BDOS
bdos:
                ; Note: none of the BDOS functions uses A as an input.
                ld      a,c
                cp      $31
                jr      nc,bdos_illfunc
                push    hl
                ld      hl,bdos_table
                add     a,a
                add     a,l
                ld      l,a
                ld      a,0
                adc     a,h
                ld      h,a
                ld      a,(hl)
                inc     hl
                ld      h,(hl)
                ld      l,a
                ex      (sp),hl
                ret                     ; jump to address from table

bdos_illfunc:
                ; Invalid function number.
                ; Note: I couldn't find a specification anywhere of the proper
                ;       behaviour in this case, so I'll return a typical error
                ;       value.
                ld      a,$FF
                ld      l,a
                ret

bdos_table:
                dw      bdos_print      ; $00
                dw      bdos_print      ; $01
                dw      bdos_conout     ; $02
                dw      bdos_print      ; $03
                dw      bdos_print      ; $04
                dw      bdos_print      ; $05
                dw      bdos_print      ; $06
                dw      bdos_print      ; $07
                dw      bdos_print      ; $08
                dw      bdos_strout     ; $09
                dw      bdos_print      ; $0A
                dw      bdos_print      ; $0B
                dw      bdos_print      ; $0C
                dw      bdos_dskrst     ; $0D
                dw      bdos_print      ; $0E
                dw      bdos_print      ; $0F
                dw      bdos_print      ; $10
                dw      bdos_print      ; $11
                dw      bdos_print      ; $12
                dw      bdos_print      ; $13
                dw      bdos_print      ; $14
                dw      bdos_print      ; $15
                dw      bdos_print      ; $16
                dw      bdos_print      ; $17
                dw      bdos_print      ; $18
                dw      bdos_curdrv     ; $19
                dw      bdos_setdta     ; $1A
                dw      bdos_print      ; $1B
                dw      bdos_print      ; $1C
                dw      bdos_print      ; $1D
                dw      bdos_print      ; $1E
                dw      bdos_print      ; $1F
                dw      bdos_print      ; $20
                dw      bdos_print      ; $21
                dw      bdos_print      ; $22
                dw      bdos_print      ; $23
                dw      bdos_print      ; $24
                dw      bdos_print      ; $25
                dw      bdos_print      ; $26
                dw      bdos_print      ; $27
                dw      bdos_print      ; $28
                dw      bdos_print      ; $29
                dw      bdos_print      ; $2A
                dw      bdos_print      ; $2B
                dw      bdos_print      ; $2C
                dw      bdos_print      ; $2D
                dw      bdos_verify     ; $2E
                dw      bdos_rdabs      ; $2F
                dw      bdos_print      ; $30

bdos_print:
                ld      a,$23           ; ASCII mode
                out     (DBG_CTRL),a
                ld      hl,bdos_text
                call    print_debug_asciiz
                ld      a,c
                call    print_debug_hexbyte
                ld      a,$00           ; flush
                out     (DBG_CTRL),a
                ret
bdos_text:      db      "disk: BDOS ($F37D/$0005) called, function $",0

;--------------------------------
; BDOS $02: CONOUT
; Print character on stdout.
; TODO: Support printer echo.
; TODO: Check CTRL-C and other key combos (see function $0B in DOS2 docs).
; Input: E = character to print
bdos_conout:
                push    iy
                push    ix
                ld      ix,chput
                ld      iy,(EXPTBL - 1)
                ld      a,e
                call    calslt
                pop     ix
                pop     iy
                ret

;--------------------------------
; BDOS $09: STROUT
; Print string on stdout.
; Input: DE = address of string, string is terminated by "$" character
bdos_strout:
                ld      a,(de)
                cp      '$'
                ret     z
                push    de
                ld      e,a
                call    bdos_conout
                pop     de
                inc     de
                jr      bdos_strout

;--------------------------------
; BDOS $0D: DSKRST
; Flush internal buffers and reset DTA.
bdos_dskrst:
                ; TODO: Flush internal buffers.
                ;       (we don't have any buffers yet)
                call    resetdta
                ret

;--------------------------------
; BDOS $19: CURDRV
; Gets the current drive.
; Output: drive (0=A)
bdos_curdrv:
                ; TODO: Keep the current drive in a sysvar.
                xor     a
                ld      l,a
                ret

;--------------------------------
; BDOS $1A: SETDTA
; Set Disk Transfer Area.
; Input: DE = new DTA address
resetdta:
                ld      de,$0080
bdos_setdta:
                ld      (DTA_ADDR),de
                ret

;--------------------------------
; BDOS $2E: VERIFY
; Set/reset verify flag.
; The DOS2 docs say it is optional to implement the verify feature.
; That means we will not implement it, because:
; - most people will be using C-BIOS with disk images rather than real disks
; - verification is very slow
; - verification doesn't really add a lot of protection
; Input: E = new flag state (E=0: disabled, E!=0: enabled)
bdos_verify:
                ret

;--------------------------------
; BDOS $2F: RDABS
; Read sectors.
; Input:  DE = number of first sector to read
;         L  = drive (0=A)
;         H  = number of sectors to read
; Output: A = error code (0=OK)
bdos_rdabs:
                ld      a,l
                ld      b,h
                ld      c,$F9           ; TODO: Retrieve media ID from disk.
                ld      hl,(DTA_ADDR)
                and     a               ; CF = 0
                call    phydio
                jr      c,bdos_rdabs_error
                xor     a
                ret

bdos_rdabs_error:
                ; TODO: Find out how to translate PHYDIO errors to BDOS errors.
                inc     a
                ret

; Input:   F  = NC to read, C to write
;          A  = Drive number (0=A:)
;          B  = Number of sectors to transfer
;          C  = Media descriptor
;          DE = Logical sector number
;          HL = Transfer address


;--------------------------------

                include "debug.asm"

;--------------------------------

; The purpose of this routine is having a fixed address to put breakpoints on.
; I expect that having a break point just after loading will be very useful
; when debugging the disk ROM.
                ds      $7F00 - $,$FF
dskio_done:
                nop
                ret

                ds      $8000 - $,$FF
