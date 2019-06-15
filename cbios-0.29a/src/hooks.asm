; C-BIOS hook declarations
;
; Copyright (c) 2002-2003 BouKiCHi.  All rights reserved.
; Copyright (c) 2003 Reikan.  All rights reserved.
; Copyright (c) 2004 Maarten ter Huurne.  All rights reserved.
; Copyright (c) 2004 Manuel Bilderbeek.  All rights reserved.
; Copyright (c) 2004-2006 Joost Yervante Damad.  All rights reserved.
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

; called at start of interrupt subroutine, before it has been checked if
; the interrupt was from VDP; used by e.g. RS232
H_KEYI:         equ     $FD9A

; called at start of interrupt subroutine, when it is clear that
; the interrupt is from the VDP
H_TIMI:         equ     $FD9F

; called at start of CHPUT(00A2)
H_CHPU:         equ     $FDA4

; called at start of the subroutine drawing the cursor
H_DSPC:         equ     $FDA9

; called at start of the subroutine that removes the cursor
H_ERAC:         equ     $FDAE

; called at start of DSPFNK(00CF)
H_DSPF:         equ     $FDB3

; called at start of ERAFNK(00CC)
H_ERAF:         equ     $FDB8

; called at start of TOTEXT(00D2)
H_TOTE:         equ     $FDBD

; called at start of CHGET(009F)
H_CHGE:         equ     $FDC2

; called at start of the subroutine that fills the pattern-table
; can be used to override the default ASCII patterns
H_INIP:         equ     $FDC7

; called at part of subroutine that decodes combined keystrokes
; like with CTRL/CODE/GRAPH/SHIFT to an ASCII code; can be used
; to override the working of the keyboard
H_KEYC:         equ     $FDCC

; called at start of the subroutine that decodes single keystrokes;
; can be used to override the working of the keyboard
H_KYEA:         equ     $FDD1

; called at start of NMI interrupt subroutine
H_NMI:          equ     $FDD6

; called at start of PINLIN(00AE)
H_PINL:         equ     $FDDB

; called at start of QINLIN(00B4)
H_QINL:         equ     $FDE0

; called at start of INLIN(00B1)
H_INLI:         equ     $FDE5

; BASIC interpreter hook
H_ONGO:         equ     $FDEA

; implementation hook for DSKO$
H_DSKO:         equ     $FDEF

; implementation hook for SET
H_SETS:         equ     $FDF4

; implementation hook for NAME
H_NAME:         equ     $FDF9

; implementation hook for KILL
H_KILL:         equ     $FDFE

; implementation hook for IPL
H_IPL:          equ     $FE03

; implementation hook for COPY
H_COPY:         equ     $FE08

; implementation hook for CMD
H_CMD:          equ     $FE0D

; implementation hook for DSKF
H_DSKF:         equ     $FE12

; implementation hook for DSKI$
H_DSKI:         equ     $FE17

; implementation hook for ATTR$
H_ATTR:         equ     $FE1C

; implementation hook for LSET
H_LSET:         equ     $FE21

; implementation hook for RSET
H_RSET:         equ     $FE26

; implementation hook for FIELD
H_FIEL:         equ     $FE2B

; implementation hook for MKI$
H_MKIS:         equ     $FE30

; implementation hook for MKS$
H_MKSS:         equ     $FE35

; implementation hook for MKD$
H_MKDS:         equ     $FE3A

; implementation hook for CVI
H_CVI:          equ     $FE3F

; implementation hook for CVS
H_CVS:          equ     $FE44

; implementation hook for CVD
H_CVD:          equ     $FE49

; called when looking up the value of PTRFIL(F864) for DISKBASIC
H_GETP:         equ     $FE4E

; called when PTRFIL(F864) is being given a new value
H_SETF:         equ     $FE53

; called when an OPEN statement was issued without a FOR-part
; part of DISKBASIC
H_NOFO:         equ     $FE58

; called for an operation for file-buffer 0, in DISKBASIC
H_NULO:         equ     $FE5D

; called from DISKBASIC for a call with file-buffer not 0
H_NTFL:         equ     $FE62

; called when doing a MERGE command for disks
H_MERG:         equ     $FE67

; called when doing a SAVE commands for disks
H_SAVE:         equ     $FE6C

; called when doing a BSAVE command for disks
H_BINS:         equ     $FE71

; called when doing a BLOAD command for disks
H_BINL:         equ     $FE76

; implementation hook for FILES
H_FILE:         equ     $FE7B

;  DISKBASIC hook
H_DGET:         equ     $FE80

;  DISKBASIC hook
H_FILO:         equ     $FE85

;  DISKBASIC hook
H_INDS:         equ     $FE8A

; DISKBASIC entry for selecting the previous disk station for disk IO
H_RSLF:         equ     $FE8F

; DISKBASIC entry for remembering the current disk station
H_SAVD:         equ     $FE94

; implementation hook for LOC
H_LOC:          equ     $FE99

; implementation hook for LOF
G_LOF:          equ     $FE9E

; called when doing EOF for a disk in DISKBASIC
H_EOF:          equ     $FEA3

; implementation hook for FPOS
H_FPOS:         equ     $FEA8

; DISKBASIC hook
H_BAKU:         equ     $FEAD

; called when BASIC interpreter is decoding the device part of a filename
H_PARD:         equ     $FEB2

; called when BASIC interpreter finds a file without a device part
H_NODE:         equ     $FEB7

; DISKBASIC hook
H_POSD:         equ     $FEBC

; called when searching a device by name
H_DEVN:         equ     $FEC1

; BASIC interpreter hook
H_GEND:         equ     $FEC6

; Called when clearing variables during the preparation of a RUN statement.
; Also used by the disk ROM to start boot sequence.
H_RUNC:         equ     $FECB

; called when doing CLEAR
H_CLEA:         equ     $FED0

; BASIC interpreter hook
H_LOPD:         equ     $FED5

; BASIC interpreter hook; called at stack error
H_STKE:         equ     $FEDA

; called at the start of ISFLIO(014A)
H_ISFL:         equ     $FEDF

; called at the start of OUTDO(0018)
H_OUTD:         equ     $FEE4

; BASIC interpreter hook
H_CRDO:         equ     $FEE9

; BASIC interpreter hook
H_DSKC:         equ     $FEEE

; called at the end of a BASIC program
H_PRGE:         equ     $FEF8

; BASIC interpreter hook
H_ERRP:         equ     $FEFD

; BASIC interpreter hook
H_ERRF:         equ     $FF02

; BASIC interpreter hook
H_READ:         equ     $FF07

; BASIC interpreter hook
H_MAIN:         equ     $FF0C

; called when executing a BASIC statement in direct mode
H_DIRD:         equ     $FF11

; BASIC interpreter hook
H_FINI:         equ     $FF16

; BASIC interpreter hook
H_FINE:         equ     $FF1B

; called while encoding a just typed BASIC statement
H_CRUN:         equ     $FF20

; called while encoding a just typed BASIC statement
H_CRUS:         equ     $FF25

; called when a keyword has been found while encoding a just typed
; BASIC statement
H_ISRE:         equ     $FF2A

; called when a function has been found while encoding a just typed
; BASIC statement
H_NTFN:         equ     $FF2F

; called when a non-keyword has been found while encoding a just
; typed BASIC statement
H_NOTR:         equ     $FF34

; BASIC interpreter hook
H_SNGF:         equ     $FF39

; BASIC interpreter hook
H_NEWS:         equ     $FF3E

; BASIC interpreter hook
H_GONE:         equ     $FF43

; called at start of CHRGTR(0010)
H_CHRG:         equ     $FF48

; BASIC interpreter hook
H_RETU:         equ     $FF4D

; BASIC interpreter hook
H_PRTF:         equ     $FF52

; BASIC interpreter hook
H_COMP:         equ     $FF57

; BASIC interpreter hook
H_FINP:         equ     $FF5C

; BASIC interpreter hook
H_TRMN:         equ     $FF61

; BASIC interpreter hook
H_FRME:         equ     $FF66

; BASIC interpreter hook
H_NTPL:         equ     $FF6B

; called when calculating the value of an expression in BASIC
H_EVAL:         equ     $FF70

; BASIC interpreter hook
H_OKNO:         equ     $FF75

; BASIC interpreter hook
H_FING:         equ     $FF7A

; called when setting a value to a substring with MID$
H_ISMI:         equ     $FF7F

; called when executing the WIDTH statement
H_WIDT:         equ     $FF84

; called when executing the LIST statement
H_LIST:         equ     $FF89

; BASIC interpreter hook
H_BUFL:         equ     $FF8E

; BASIC interpreter hook
H_FRQI:         equ     $FF93

; BASIC interpreter hook
H_SCNE:         equ     $FF98

; BASIC interpreter hook
H_FRET:         equ     $FF9D

; called when looking up a variable in BASIC
H_PTRG:         equ     $FFA2

; called from within PHYDIO(0144), to allow its implementation
H_PHYD:         equ     $FFA7

; called from within FORMAT(147), to allow its implementation
H_FORM:         equ     $FFAC

; called form the error-handling routine of the BASIC interpreter
H_ERRO:         equ     $FFB1

; called at start of LPTOUT(00A5)
H_LPTO:         equ     $FFB6

; called at start of LPTSTT(00A8)
H_LPTS:         equ     $FFBB

; called when executing SCREEN
H_SCRE:         equ     $FFC0

; called when executing PLAY
H_PLAY:         equ     $FFC5

; allows for installation of expansion devices that contain extra OS subroutines
H_BEXT:         equ     $FFCA


; vim:ts=8:expandtab:filetype=z8a:syntax=z8a:
