		; --- PT3 REPLAYER WORKING ON ROM ---
		; --- Can be assembled with asMSX ---
		; --- ROM version: MSX-KUN        ---
		; --- THIS FILE MUST BE COMPILED IN RAM ---

		; --- PT3 WORKAREA [self-modifying code patched] ---

	.area	_DATA

PT3_SETUP:		.ds	1	;set bit0 to 1, if you want to play without looping
					;bit7 is set each time, when loop point is passed
PT3_MODADDR:		.ds	2
PT3_CrPsPtr:		.ds	2
PT3_SAMPTRS:		.ds	2
PT3_OrnPtrs:		.ds	2
PT3_PDSP:		.ds	2
PT3_CSP:		.ds	2
PT3_PSP:		.ds	2
PT3_PrNote:		.ds	1
PT3_PrSlide:		.ds	2
PT3_AdInPtA:		.ds	2
PT3_AdInPtB:		.ds	2
PT3_AdInPtC:		.ds	2
PT3_LPosPtr:		.ds	2
PT3_PatsPtr:		.ds	2
PT3_Delay:		.ds	1
PT3_AddToEn:		.ds	1
PT3_Env_Del:		.ds	1
PT3_ESldAdd:		.ds	2

VARS:

ChanA:			.ds	29			;CHNPRM_Size
ChanB:			.ds	29			;CHNPRM_Size
ChanC:			.ds	29			;CHNPRM_Size

;GlobalVars
DelyCnt:		.ds	1
CurESld:		.ds	2
CurEDel:		.ds	1
Ns_Base_AddToNs:
Ns_Base:		.ds	1
AddToNs:		.ds	1

AYREGS:
VT_:			.ds	14
EnvBase:		.ds	2
VAR0END:		.ds	240


		; --- PT3 REPLAYER WORKING ON ROM ---
		; --- Can be assembled with asMSX ---
		; --- ROM version: MSX-KUN        ---
		; --- asMSX version: SapphiRe     ---

; Based on MSX version of PT3 by Dioniso
;
; This version of the replayer uses a fixed volume and note table, if you need a 
; different note table you can copy it from TABLES.TXT file, distributed with the
; original PT3 distribution. This version also allows the use of PT3 commands.
;
; PLAY and PSG WRITE routines seperated to allow independent calls
;
; ROM LENGTH: 1528 bytes
; RAM LENGTH:  382 bytes

	.area	_CODE

		; --- CONSTANT VALUES DEFINITION ---

;ChannelsVars
;struc	CHNPRM
;reset group
CHNPRM_PsInOr	.equ 0	;RESB 1
CHNPRM_PsInSm	.equ 1	;RESB 1
CHNPRM_CrAmSl	.equ 2	;RESB 1
CHNPRM_CrNsSl	.equ 3	;RESB 1
CHNPRM_CrEnSl	.equ 4	;RESB 1
CHNPRM_TSlCnt	.equ 5	;RESB 1
CHNPRM_CrTnSl	.equ 6	;RESW 1
CHNPRM_TnAcc	.equ 8	;RESW 1
CHNPRM_COnOff	.equ 10	;RESB 1
;reset group

CHNPRM_OnOffD	.equ 11	;RESB 1

;IX for PTDECOD here (+12)
CHNPRM_OffOnD	.equ 12	;RESB 1
CHNPRM_OrnPtr	.equ 13	;RESW 1
CHNPRM_SamPtr	.equ 15	;RESW 1
CHNPRM_NNtSkp	.equ 17	;RESB 1
CHNPRM_Note	.equ 18	;RESB 1
CHNPRM_SlToNt	.equ 19	;RESB 1
CHNPRM_Env_En	.equ 20	;RESB 1
CHNPRM_Flags	.equ 21	;RESB 1
 ;Enabled - 0,SimpleGliss - 2
CHNPRM_TnSlDl	.equ 22	;RESB 1
CHNPRM_TSlStp	.equ 23	;RESW 1
CHNPRM_TnDelt	.equ 25	;RESW 1
CHNPRM_NtSkCn	.equ 27	;RESB 1
CHNPRM_Volume	.equ 28	;RESB 1
CHNPRM_Size	.equ 29	;RESB 1
;endstruc

;struc	AR
AR_TonA		.equ 0	;RESW 1
AR_TonB		.equ 2	;RESW 1
AR_TonC		.equ 4	;RESW 1
AR_Noise	.equ 6	;RESB 1
AR_Mixer	.equ 7	;RESB 1
AR_AmplA	.equ 8	;RESB 1
AR_AmplB	.equ 9	;RESB 1
AR_AmplC	.equ 10	;RESB 1
AR_Env		.equ 11	;RESW 1
AR_EnvTp	.equ 13	;RESB 1
;endstruc

		; --- CODE STARTS HERE ---

CHECKLP:	
		LD	HL,#PT3_SETUP
		SET	7,(HL)
		BIT	0,(HL)
		RET	Z
		POP	HL
		LD	HL,#DelyCnt
		INC	(HL)
		LD	HL,#(ChanA+CHNPRM_NtSkCn)
		INC	(HL)
PT3_MUTE:	XOR	A
		LD	H,A
		LD	L,A
		LD	(AYREGS+AR_AmplA),A
		LD	(AYREGS+AR_AmplB),HL
		JP	ROUT_A0

PT3_INIT:	;HL - AddressOfModule - 100
		LD (PT3_MODADDR),HL
		PUSH HL
		LD DE,#100
		ADD HL,DE
		LD A,(HL)
		LD (PT3_Delay),A
		PUSH HL
		POP IX
		ADD HL,DE
		LD (PT3_CrPsPtr),HL
		LD E,+2(IX)
		ADD HL,DE
		INC HL
		LD (PT3_LPosPtr),HL
		POP DE
		LD L,+103-100(IX)
		LD H,+104-100(IX)
		ADD HL,DE
		LD (PT3_PatsPtr),HL
		LD HL,#169
		ADD HL,DE
		LD (PT3_OrnPtrs),HL
		LD HL,#105
		ADD HL,DE
		LD (PT3_SAMPTRS),HL
		LD HL,#PT3_SETUP
		RES 7,(HL)

		; --- CREATE PT3 VOLUME TABLE (c) Ivan Roshin, adapted by SapphiRe ---
		ld	hl,#0x11
		ld	d,h
		ld	e,h
		ld	IX,#(VT_+16)
		ld	b,#15
__INITV1:	
		push	hl
		add	hl,de
		ex	de,hl
		sbc	hl,hl
		ld	c,b
		ld	b,#16
__INITV2:	ld	a,l
		rla
		ld	a,h
		adc	a,#0
		ld	(ix),a
		inc	ix
		add	hl,de
		djnz	__INITV2
		pop	hl
		ld	a,e
		cp	#0x77
		jr	nz,__INITV3
		inc	e
__INITV3:	ld	b,c
		djnz	__INITV1

		; --- INITIALIZE PT3 VARIABLES ---
		XOR A
		LD HL,#VARS
		LD (HL),A
		LD DE,#(VARS+1)
		LD BC,#(VAR0END-VARS-1)
		LDIR

		INC A
		LD (DelyCnt),A
		LD HL,#0xF001 ;H - CHNPRM_Volume, L - CHNPRM_NtSkCn
		LD (ChanA+CHNPRM_NtSkCn),HL
		LD (ChanB+CHNPRM_NtSkCn),HL
		LD (ChanC+CHNPRM_NtSkCn),HL

		LD HL,#EMPTYSAMORN
		LD (PT3_AdInPtA),HL ;ptr to zero
		LD (ChanA+CHNPRM_OrnPtr),HL ;ornament 0 is "0,1,0"
		LD (ChanB+CHNPRM_OrnPtr),HL ;in all versions from
		LD (ChanC+CHNPRM_OrnPtr),HL ;3.xx to 3.6x and VTII

		LD (ChanA+CHNPRM_SamPtr),HL ;S1 There is no default
		LD (ChanB+CHNPRM_SamPtr),HL ;S2 sample in PT3, so, you
		LD (ChanC+CHNPRM_SamPtr),HL ;S3 can comment S1,2,3; see
					    ;also EMPTYSAMORN comment
		RET

		;pattern decoder
PD_OrSm:	LD +(CHNPRM_Env_En-12)(IX),#0
		CALL SETORN
		LD A,(BC)
		INC BC
		RRCA

PD_SAM:		ADD A,A
PD_SAM_:	LD E,A
		LD D,#0
		LD HL,(PT3_SAMPTRS)
		ADD HL,DE
		LD E,(HL)
		INC HL
		LD D,(HL)
		LD HL,(PT3_MODADDR)
		ADD HL,DE
		LD +(CHNPRM_SamPtr-12)(IX),L
		LD +(CHNPRM_SamPtr+1-12)(IX),H
		JR PD_LOOP

PD_VOL:		RLCA
		RLCA
		RLCA
		RLCA
		LD +(CHNPRM_Volume-12)(IX),A
		JR PD_LP2
	
PD_EOff:	LD +(CHNPRM_Env_En-12)(IX),A
		LD +(CHNPRM_PsInOr-12)(IX),A
		JR PD_LP2

PD_SorE:	DEC A
		JR NZ,PD_ENV
		LD A,(BC)
		INC BC
		LD +(CHNPRM_NNtSkp-12)(IX),A
		JR PD_LP2

PD_ENV:		CALL SETENV
		JR PD_LP2

PD_ORN:		CALL SETORN
		JR PD_LOOP
       
PD_ESAM:	LD +(CHNPRM_Env_En-12)(IX),A
		LD +(CHNPRM_PsInOr-12)(IX),A
		CALL NZ,SETENV
		LD A,(BC)
		INC BC
		JR PD_SAM_

PTDECOD:	LD A,+(CHNPRM_Note-12)(IX)
		LD (PT3_PrNote),A
		LD L,+(CHNPRM_CrTnSl-12)(IX)
		LD H,+(CHNPRM_CrTnSl+1-12)(IX)
		LD (PT3_PrSlide),HL

PD_LOOP:	LD DE,#0x2010
PD_LP2:		LD A,(BC)
		INC BC
		ADD A,E
		JR C,PD_OrSm
		ADD A,D
		JR Z,PD_FIN
		JR C,PD_SAM
		ADD A,E
		JR Z,PD_REL
		JR C,PD_VOL
		ADD A,E
		JR Z,PD_EOff
		JR C,PD_SorE
		ADD A,#96
		JR C,PD_NOTE
		ADD A,E
		JR C,PD_ORN
		ADD A,D
		JR C,PD_NOIS
		ADD A,E
		JR C,PD_ESAM
		ADD A,A
		LD E,A
;		LD HL,#((SPCCOMS+0xDF20)%0xFFFF)	; Adapted from original Speccy version (saves 6 bytes)
		LD HL,#(SPCCOMS+0xDF20)	; Adapted from original Speccy version (saves 6 bytes)
		ADD HL,DE
		LD E,(HL)
		INC HL
		LD D,(HL)
		PUSH DE
		JR PD_LOOP

PD_NOIS:	LD (Ns_Base),A
		JR PD_LP2

PD_REL:		RES 0,+(CHNPRM_Flags-12)(IX)
		JR PD_RES
	
PD_NOTE:	LD +(CHNPRM_Note-12)(IX),A
		SET 0,+(CHNPRM_Flags-12)(IX)
		XOR A

PD_RES:		LD (PT3_PDSP),SP
		LD SP,IX
		LD H,A
		LD L,A
		PUSH HL
		PUSH HL
		PUSH HL
		PUSH HL
		PUSH HL
		PUSH HL
		LD SP,(PT3_PDSP)

PD_FIN:		LD A,+(CHNPRM_NNtSkp-12)(IX)
		LD +(CHNPRM_NtSkCn-12)(IX),A
		RET

C_PORTM:	RES 2,+(CHNPRM_Flags-12)(IX)
		LD A,(BC)
		INC BC
		;SKIP PRECALCULATED TONE DELTA [BECAUSE
		;CANNOT BE RIGHT AFTER PT3 COMPILATION]
		INC BC
		INC BC
		LD +(CHNPRM_TnSlDl-12)(IX),A
		LD +(CHNPRM_TSlCnt-12)(IX),A
		LD DE,#NT_
		LD A,+(CHNPRM_Note-12)(IX)
		LD +(CHNPRM_SlToNt-12)(IX),A
		ADD A,A
		LD L,A
		LD H,#0
		ADD HL,DE
		LD A,(HL)
		INC HL
		LD H,(HL)
		LD L,A
		PUSH HL
		LD A,(PT3_PrNote)
		LD +(CHNPRM_Note-12)(IX),A
		ADD A,A
		LD L,A
		LD H,#0
		ADD HL,DE
		LD E,(HL)
		INC HL
		LD D,(HL)
		POP HL
		SBC HL,DE
		LD +(CHNPRM_TnDelt-12)(IX),L
		LD +(CHNPRM_TnDelt+1-12)(IX),H
		LD DE,(PT3_PrSlide)
		LD +(CHNPRM_CrTnSl-12)(IX),E
		LD +(CHNPRM_CrTnSl+1-12)(IX),D
		LD A,(BC) ;SIGNED TONE STEP
		INC BC
		EX AF,AF'
		LD A,(BC)
		INC BC
		AND A
		JR Z,__NOSIG
		EX DE,HL
__NOSIG:	SBC HL,DE
		JP P,SET_STP
		CPL
		EX AF,AF'
		NEG
		EX AF,AF'
SET_STP:	LD +(CHNPRM_TSlStp+1-12)(IX),A
		EX AF,AF'
		LD +(CHNPRM_TSlStp-12)(IX),A
		LD +(CHNPRM_COnOff-12)(IX),#0
		RET

C_GLISS:	SET 2,+(CHNPRM_Flags-12)(IX)
		LD A,(BC)
		INC BC
		LD +(CHNPRM_TnSlDl-12)(IX),A
		LD +(CHNPRM_TSlCnt-12)(IX),A
		LD A,(BC)
		INC BC
		EX AF,AF'
		LD A,(BC)
		INC BC
		JR SET_STP

C_SMPOS:	LD A,(BC)
		INC BC
		LD +(CHNPRM_PsInSm-12)(IX),A
		RET

C_ORPOS:	LD A,(BC)
		INC BC
		LD +(CHNPRM_PsInOr-12)(IX),A
		RET

C_VIBRT:	LD A,(BC)
		INC BC
		LD +(CHNPRM_OnOffD-12)(IX),A
		LD +(CHNPRM_COnOff-12)(IX),A
		LD A,(BC)
		INC BC
		LD +(CHNPRM_OffOnD-12)(IX),A
		XOR A
		LD +(CHNPRM_TSlCnt-12)(IX),A
		LD +(CHNPRM_CrTnSl-12)(IX),A
		LD +(CHNPRM_CrTnSl+1-12)(IX),A
		RET

C_ENGLS:	LD A,(BC)
		INC BC
		LD (PT3_Env_Del),A
		LD (CurEDel),A
		LD A,(BC)
		INC BC
		LD L,A
		LD A,(BC)
		INC BC
		LD H,A
		LD (PT3_ESldAdd),HL
		RET

C_DELAY:	LD A,(BC)
		INC BC
		LD (PT3_Delay),A
		RET
	
SETENV:		LD +(CHNPRM_Env_En-12)(IX),E
		LD (AYREGS+AR_EnvTp),A
		LD A,(BC)
		INC BC
		LD H,A
		LD A,(BC)
		INC BC
		LD L,A
		LD (EnvBase),HL
		XOR A
		LD +(CHNPRM_PsInOr-12)(IX),A
		LD (CurEDel),A
		LD H,A
		LD L,A
		LD (CurESld),HL
C_NOP:		RET

SETORN:		ADD A,A
		LD E,A
		LD D,#0
		LD +(CHNPRM_PsInOr-12)(IX),D
		LD HL,(PT3_OrnPtrs)
		ADD HL,DE
		LD E,(HL)
		INC HL
		LD D,(HL)
		LD HL,(PT3_MODADDR)
		ADD HL,DE
		LD +(CHNPRM_OrnPtr-12)(IX),L
		LD +(CHNPRM_OrnPtr+1-12)(IX),H
		RET

		;ALL 16 ADDRESSES TO PROTECT FROM BROKEN PT3 MODULES
SPCCOMS:	.dw C_NOP
		.dw C_GLISS
		.dw C_PORTM
		.dw C_SMPOS
		.dw C_ORPOS
		.dw C_VIBRT
		.dw C_NOP
		.dw C_NOP
		.dw C_ENGLS
		.dw C_DELAY
		.dw C_NOP
		.dw C_NOP
		.dw C_NOP
		.dw C_NOP
		.dw C_NOP
		.dw C_NOP

CHREGS:		XOR A
		LD (AYREGS+AR_AmplC),A
		BIT 0,+CHNPRM_Flags(IX)
		PUSH HL
		JP Z,__CH_EXIT
		LD (PT3_CSP),SP
		LD L,+CHNPRM_OrnPtr(IX)
		LD H,+CHNPRM_OrnPtr+1(IX)
		LD SP,HL
		POP DE
		LD H,A
		LD A,+CHNPRM_PsInOr(IX)
		LD L,A
		ADD HL,SP
		INC A
		CP D
		JR C,__CH_ORPS
		LD A,E
__CH_ORPS:	LD +CHNPRM_PsInOr(IX),A
		LD A,+CHNPRM_Note(IX)
		ADD A,(HL)
		JP P,__CH_NTP
		XOR A
__CH_NTP:	CP #96
		JR C,__CH_NOK
		LD A,#95
__CH_NOK:	ADD A,A
		EX AF,AF'
		LD L,+CHNPRM_SamPtr(IX)
		LD H,+CHNPRM_SamPtr+1(IX)
		LD SP,HL
		POP DE
		LD H,#0
		LD A,+CHNPRM_PsInSm(IX)
		LD B,A
		ADD A,A
		ADD A,A
		LD L,A
		ADD HL,SP
		LD SP,HL
		LD A,B
		INC A
		CP D
		JR C,__CH_SMPS
		LD A,E
__CH_SMPS:	LD +CHNPRM_PsInSm(IX),A
		POP BC
		POP HL
		LD E,+CHNPRM_TnAcc(IX)
		LD D,+CHNPRM_TnAcc+1(IX)
		ADD HL,DE
		BIT 6,B
		JR Z,__CH_NOAC
		LD +CHNPRM_TnAcc(IX),L
		LD +CHNPRM_TnAcc+1(IX),H
__CH_NOAC:	EX DE,HL
		EX AF,AF'
		LD L,A
		LD H,#0
		LD SP,#NT_
		ADD HL,SP
		LD SP,HL
		POP HL
		ADD HL,DE
		LD E,+CHNPRM_CrTnSl(IX)
		LD D,+CHNPRM_CrTnSl+1(IX)
		ADD HL,DE
		LD SP,(PT3_CSP)
		EX (SP),HL
		XOR A
		OR +CHNPRM_TSlCnt(IX)
		JR Z,__CH_AMP
		DEC +CHNPRM_TSlCnt(IX)
		JR NZ,__CH_AMP
		LD A,+CHNPRM_TnSlDl(IX)
		LD +CHNPRM_TSlCnt(IX),A
		LD L,+CHNPRM_TSlStp(IX)
		LD H,+CHNPRM_TSlStp+1(IX)
		LD A,H
		ADD HL,DE
		LD +CHNPRM_CrTnSl(IX),L
		LD +CHNPRM_CrTnSl+1(IX),H
		BIT 2,+CHNPRM_Flags(IX)
		JR NZ,__CH_AMP
		LD E,+CHNPRM_TnDelt(IX)
		LD D,+CHNPRM_TnDelt+1(IX)
		AND A
		JR Z,__CH_STPP
		EX DE,HL
__CH_STPP:	SBC HL,DE
		JP M,__CH_AMP
		LD A,+CHNPRM_SlToNt(IX)
		LD +CHNPRM_Note(IX),A
		XOR A
		LD +CHNPRM_TSlCnt(IX),A
		LD +CHNPRM_CrTnSl(IX),A
		LD +CHNPRM_CrTnSl+1(IX),A
__CH_AMP:	LD A,+CHNPRM_CrAmSl(IX)
		BIT 7,C
		JR Z,__CH_NOAM
		BIT 6,C
		JR Z,__CH_AMIN
		CP #15
		JR Z,__CH_NOAM
		INC A
		JR __CH_SVAM
__CH_AMIN:	CP #-15
		JR Z,__CH_NOAM
		DEC A
__CH_SVAM:	LD +CHNPRM_CrAmSl(IX),A
__CH_NOAM:	LD L,A
		LD A,B
		AND #15
		ADD A,L
		JP P,__CH_APOS
		XOR A
__CH_APOS:	CP #16
		JR C,__CH_VOL
		LD A,#15
__CH_VOL:	OR +CHNPRM_Volume(IX)
		LD L,A
		LD H,#0
		LD DE,#VT_
		ADD HL,DE
		LD A,(HL)
__CH_ENV:	BIT 0,C
		JR NZ,__CH_NOEN
		OR +CHNPRM_Env_En(IX)
__CH_NOEN:	LD (AYREGS+AR_AmplC),A
		BIT 7,B
		LD A,C
		JR Z,__NO_ENSL
		RLA
		RLA
		SRA A
		SRA A
		SRA A
		ADD A,+CHNPRM_CrEnSl(IX) ;SEE COMMENT BELOW
		BIT 5,B
		JR Z,__NO_ENAC
		LD +CHNPRM_CrEnSl(IX),A
__NO_ENAC:	LD HL,#PT3_AddToEn
		ADD A,(HL) ;BUG IN PT3 - NEED WORD HERE.
			   ;FIX IT IN NEXT VERSION?
		LD (HL),A
		JR __CH_MIX
__NO_ENSL:	RRA
		ADD A,+CHNPRM_CrNsSl(IX)
		LD (AddToNs),A
		BIT 5,B
		JR Z,__CH_MIX
		LD +CHNPRM_CrNsSl(IX),A
__CH_MIX:	LD A,B
		RRA
		AND #0x48
__CH_EXIT:	LD HL,#(AYREGS+AR_Mixer)
		OR (HL)
		RRCA
		LD (HL),A
		POP HL
		XOR A
		OR +CHNPRM_COnOff(IX)
		RET Z
		DEC +CHNPRM_COnOff(IX)
		RET NZ
		XOR +CHNPRM_Flags(IX)
		LD +CHNPRM_Flags(IX),A
		RRA
		LD A,+CHNPRM_OnOffD(IX)
		JR C,__CH_ONDL
		LD A,+CHNPRM_OffOnD(IX)
__CH_ONDL:	LD +CHNPRM_COnOff(IX),A
		RET

PT3_PLAY:	XOR A
		LD (PT3_AddToEn),A
		LD (AYREGS+AR_Mixer),A
		DEC A
		LD (AYREGS+AR_EnvTp),A
		LD HL,#DelyCnt
		DEC (HL)
		JP NZ,__PL2
		LD HL,#(ChanA+CHNPRM_NtSkCn)
		DEC (HL)
		JR NZ,__PL1B
		LD BC,(PT3_AdInPtA)
		LD A,(BC)
		AND A
		JR NZ,__PL1A
		LD D,A
		LD (Ns_Base),A
		LD HL,(PT3_CrPsPtr)
		INC HL
		LD A,(HL)
		INC A
		JR NZ,__PLNLP
		CALL CHECKLP
		LD HL,(PT3_LPosPtr)
		LD A,(HL)
		INC A
__PLNLP:	LD (PT3_CrPsPtr),HL
		DEC A
		ADD A,A
		LD E,A
		RL D
		LD HL,(PT3_PatsPtr)
		ADD HL,DE
		LD DE,(PT3_MODADDR)
		LD (PT3_PSP),SP
		LD SP,HL
		POP HL
		ADD HL,DE
		LD B,H
		LD C,L
		POP HL
		ADD HL,DE
		LD (PT3_AdInPtB),HL
		POP HL
		ADD HL,DE
		LD (PT3_AdInPtC),HL
		LD SP,(PT3_PSP)

__PL1A:		LD IX,#(ChanA+12)
		CALL PTDECOD
		LD (PT3_AdInPtA),BC

__PL1B:		LD HL,#(ChanB+CHNPRM_NtSkCn)
		DEC (HL)
		JR NZ,__PL1C
		LD IX,#(ChanB+12)
		LD BC,(PT3_AdInPtB)
		CALL PTDECOD
		LD (PT3_AdInPtB),BC

__PL1C:		LD HL,#(ChanC+CHNPRM_NtSkCn)
		DEC (HL)
		JR NZ,__PL1D
		LD IX,#(ChanC+12)
		LD BC,(PT3_AdInPtC)
		CALL PTDECOD
		LD (PT3_AdInPtC),BC

__PL1D:		LD A,(PT3_Delay)
		LD (DelyCnt),A

__PL2:		LD IX,#ChanA
		LD HL,(AYREGS+AR_TonA)
		CALL CHREGS
		LD (AYREGS+AR_TonA),HL
		LD A,(AYREGS+AR_AmplC)
		LD (AYREGS+AR_AmplA),A
		LD IX,#ChanB
		LD HL,(AYREGS+AR_TonB)
		CALL CHREGS
		LD (AYREGS+AR_TonB),HL
		LD A,(AYREGS+AR_AmplC)
		LD (AYREGS+AR_AmplB),A
		LD IX,#ChanC
		LD HL,(AYREGS+AR_TonC)
		CALL CHREGS
		LD (AYREGS+AR_TonC),HL

		LD HL,(Ns_Base_AddToNs)
		LD A,H
		ADD A,L
		LD (AYREGS+AR_Noise),A

		LD A,(PT3_AddToEn)
		LD E,A
		ADD A,A
		SBC A,A
		LD D,A
		LD HL,(EnvBase)
		ADD HL,DE
		LD DE,(CurESld)
		ADD HL,DE
		LD (AYREGS+AR_Env),HL

		XOR A
		LD HL,#CurEDel
		OR (HL)
		RET Z
		DEC (HL)
		RET NZ
		LD A,(PT3_Env_Del)
		LD (HL),A
		LD HL,(PT3_ESldAdd)
		ADD HL,DE
		LD (CurESld),HL
		RET

PT3_ROUT:	XOR A
	
ROUT_A0:	; --- FIXES BITS 6 AND 7 OF MIXER ---
		LD	HL,#(AYREGS+AR_Mixer)
		set	7,(hl)
		res	6,(hl)

		LD C,#0xA0
		LD HL,#AYREGS
__LOUT:		OUT (C),A
		INC C
		OUTI 
		DEC C
		INC A
		CP #13
		JR NZ,__LOUT
		OUT (C),A
		LD A,(HL)
		AND A
		RET M
		INC C
		OUT (C),A
		RET

EMPTYSAMORN: 	
	.db 0,1,0,0x90 ;delete 0x90 if you don't need default sample

NT_:	;Note table 2 [if you use another in Vortex Tracker II copy it and paste
	;it from TABLES.TXT]

	.dw 0x0D10,0x0C55,0x0BA4,0x0AFC,0x0A5F,0x09CA,0x093D,0x08B8,0x083B,0x07C5,0x0755,0x06EC
	.dw 0x0688,0x062A,0x05D2,0x057E,0x052F,0x04E5,0x049E,0x045C,0x041D,0x03E2,0x03AB,0x0376
	.dw 0x0344,0x0315,0x02E9,0x02BF,0x0298,0x0272,0x024F,0x022E,0x020F,0x01F1,0x01D5,0x01BB
	.dw 0x01A2,0x018B,0x0174,0x0160,0x014C,0x0139,0x0128,0x0117,0x0107,0x00F9,0x00EB,0x00DD
	.dw 0x00D1,0x00C5,0x00BA,0x00B0,0x00A6,0x009D,0x0094,0x008C,0x0084,0x007C,0x0075,0x006F
	.dw 0x0069,0x0063,0x005D,0x0058,0x0053,0x004E,0x004A,0x0046,0x0042,0x003E,0x003B,0x0037
	.dw 0x0034,0x0031,0x002F,0x002C,0x0029,0x0027,0x0025,0x0023,0x0021,0x001F,0x001D,0x001C
	.dw 0x001A,0x0019,0x0017,0x0016,0x0015,0x0014,0x0012,0x0011,0x0010,0x000F,0x000E,0x000D
