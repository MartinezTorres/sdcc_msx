; C-BASIC(minibas) gen_code.asm (a table for parsing)
;
; Copyright (c) 2005 BouKiCHi.  All rights reserved.
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

table_code:
        dw $0000      ; $0
        dw $0000      ; $1
        dw $0000      ; $2
        dw $0000      ; $3
        dw $0000      ; $4
        dw $0000      ; $5
        dw $0000      ; $6
        dw $0000      ; $7
        dw $0000      ; $8
        dw $0000      ; $9
        dw $0000      ; $A
        dw $0000      ; $B
        dw $0000      ; $C
        dw $0000      ; $D
        dw $0000      ; $E
        dw $0000      ; $F
        dw $0000      ; $10
        dw $0000      ; $11
        dw $0000      ; $12
        dw $0000      ; $13
        dw $0000      ; $14
        dw $0000      ; $15
        dw $0000      ; $16
        dw $0000      ; $17
        dw $0000      ; $18
        dw $0000      ; $19
        dw $0000      ; $1A
        dw $0000      ; $1B
        dw $0000      ; $1C
        dw $0000      ; $1D
        dw $0000      ; $1E
        dw $0000      ; $1F
        dw $0000      ; $20
        dw $0000      ; $21
        dw $0000      ; $22
        dw $0000      ; $23
        dw $0000      ; $24
        dw $0000      ; $25
        dw $0000      ; $26
        dw $0000      ; $27
        dw $0000      ; $28
        dw $0000      ; $29
        dw $0000      ; $2A
        dw $0000      ; $2B
        dw $0000      ; $2C
        dw $0000      ; $2D
        dw $0000      ; $2E
        dw $0000      ; $2F
        dw $0000      ; $30
        dw $0000      ; $31
        dw $0000      ; $32
        dw $0000      ; $33
        dw $0000      ; $34
        dw $0000      ; $35
        dw $0000      ; $36
        dw $0000      ; $37
        dw $0000      ; $38
        dw $0000      ; $39
        dw $0000      ; $3A
        dw $0000      ; $3B
        dw $0000      ; $3C
        dw $0000      ; $3D
        dw $0000      ; $3E
        dw $0000      ; $3F
        dw $0000      ; $40
        dw $0000      ; $41
        dw $0000      ; $42
        dw $0000      ; $43
        dw $0000      ; $44
        dw $0000      ; $45
        dw $0000      ; $46
        dw $0000      ; $47
        dw $0000      ; $48
        dw $0000      ; $49
        dw $0000      ; $4A
        dw $0000      ; $4B
        dw $0000      ; $4C
        dw $0000      ; $4D
        dw $0000      ; $4E
        dw $0000      ; $4F
        dw $0000      ; $50
        dw $0000      ; $51
        dw $0000      ; $52
        dw $0000      ; $53
        dw $0000      ; $54
        dw $0000      ; $55
        dw $0000      ; $56
        dw $0000      ; $57
        dw $0000      ; $58
        dw $0000      ; $59
        dw $0000      ; $5A
        dw $0000      ; $5B
        dw $0000      ; $5C
        dw $0000      ; $5D
        dw $0000      ; $5E
        dw $0000      ; $5F
        dw $0000      ; $60
        dw $0000      ; $61
        dw $0000      ; $62
        dw $0000      ; $63
        dw $0000      ; $64
        dw $0000      ; $65
        dw $0000      ; $66
        dw $0000      ; $67
        dw $0000      ; $68
        dw $0000      ; $69
        dw $0000      ; $6A
        dw $0000      ; $6B
        dw $0000      ; $6C
        dw $0000      ; $6D
        dw $0000      ; $6E
        dw $0000      ; $6F
        dw $0000      ; $70
        dw $0000      ; $71
        dw $0000      ; $72
        dw $0000      ; $73
        dw $0000      ; $74
        dw $0000      ; $75
        dw $0000      ; $76
        dw $0000      ; $77
        dw $0000      ; $78
        dw $0000      ; $79
        dw $0000      ; $7A
        dw $0000      ; $7B
        dw $0000      ; $7C
        dw $0000      ; $7D
        dw $0000      ; $7E
        dw $0000      ; $7F
        dw $0000      ; $80
        dw code_str129 ; $81
        dw code_str130 ; $82
        dw code_str131 ; $83
        dw code_str132 ; $84
        dw code_str133 ; $85
        dw code_str134 ; $86
        dw code_str135 ; $87
        dw code_str136 ; $88
        dw code_str137 ; $89
        dw code_str138 ; $8A
        dw code_str139 ; $8B
        dw code_str140 ; $8C
        dw code_str141 ; $8D
        dw code_str142 ; $8E
        dw $0000      ; $8F
        dw code_str144 ; $90
        dw code_str145 ; $91
        dw code_str146 ; $92
        dw code_str147 ; $93
        dw code_str148 ; $94
        dw code_str149 ; $95
        dw code_str150 ; $96
        dw code_str151 ; $97
        dw code_str152 ; $98
        dw code_str153 ; $99
        dw code_str154 ; $9A
        dw code_str155 ; $9B
        dw code_str156 ; $9C
        dw code_str157 ; $9D
        dw code_str158 ; $9E
        dw code_str159 ; $9F
        dw code_str160 ; $A0
        dw $0000      ; $A1
        dw code_str162 ; $A2
        dw code_str163 ; $A3
        dw code_str164 ; $A4
        dw code_str165 ; $A5
        dw code_str166 ; $A6
        dw code_str167 ; $A7
        dw code_str168 ; $A8
        dw code_str169 ; $A9
        dw code_str170 ; $AA
        dw code_str171 ; $AB
        dw code_str172 ; $AC
        dw code_str173 ; $AD
        dw code_str174 ; $AE
        dw code_str175 ; $AF
        dw code_str176 ; $B0
        dw code_str177 ; $B1
        dw code_str178 ; $B2
        dw code_str179 ; $B3
        dw code_str180 ; $B4
        dw code_str181 ; $B5
        dw code_str182 ; $B6
        dw code_str183 ; $B7
        dw code_str184 ; $B8
        dw code_str185 ; $B9
        dw code_str186 ; $BA
        dw code_str187 ; $BB
        dw code_str188 ; $BC
        dw code_str189 ; $BD
        dw code_str190 ; $BE
        dw code_str191 ; $BF
        dw code_str192 ; $C0
        dw code_str193 ; $C1
        dw code_str194 ; $C2
        dw code_str195 ; $C3
        dw code_str196 ; $C4
        dw code_str197 ; $C5
        dw code_str198 ; $C6
        dw code_str199 ; $C7
        dw code_str200 ; $C8
        dw code_str201 ; $C9
        dw code_str202 ; $CA
        dw code_str203 ; $CB
        dw code_str204 ; $CC
        dw code_str205 ; $CD
        dw code_str206 ; $CE
        dw code_str207 ; $CF
        dw code_str208 ; $D0
        dw code_str209 ; $D1
        dw code_str210 ; $D2
        dw code_str211 ; $D3
        dw code_str212 ; $D4
        dw code_str213 ; $D5
        dw code_str214 ; $D6
        dw code_str215 ; $D7
        dw code_str216 ; $D8
        dw code_str217 ; $D9
        dw code_str218 ; $DA
        dw code_str219 ; $DB
        dw code_str220 ; $DC
        dw code_str221 ; $DD
        dw code_str222 ; $DE
        dw code_str223 ; $DF
        dw code_str224 ; $E0
        dw code_str225 ; $E1
        dw code_str226 ; $E2
        dw code_str227 ; $E3
        dw code_str228 ; $E4
        dw code_str229 ; $E5
        dw $0000      ; $E6
        dw code_str231 ; $E7
        dw code_str232 ; $E8
        dw code_str233 ; $E9
        dw code_str234 ; $EA
        dw code_str235 ; $EB
        dw code_str236 ; $EC
        dw code_str237 ; $ED
        dw code_str238 ; $EE
        dw code_str239 ; $EF
        dw code_str240 ; $F0
        dw code_str241 ; $F1
        dw code_str242 ; $F2
        dw code_str243 ; $F3
        dw code_str244 ; $F4
        dw code_str245 ; $F5
        dw code_str246 ; $F6
        dw code_str247 ; $F7
        dw code_str248 ; $F8
        dw code_str249 ; $F9
        dw code_str250 ; $FA
        dw code_str251 ; $FB
        dw code_str252 ; $FC
        dw $0000      ; $FD
        dw $0000      ; $FE
        dw $0000      ; $FF


table_extcode:
        dw $0000     ; $0
        dw $0000     ; $1
        dw $0000     ; $2
        dw $0000     ; $3
        dw $0000     ; $4
        dw $0000     ; $5
        dw $0000     ; $6
        dw $0000     ; $7
        dw $0000     ; $8
        dw $0000     ; $9
        dw $0000     ; $A
        dw $0000     ; $B
        dw $0000     ; $C
        dw $0000     ; $D
        dw $0000     ; $E
        dw $0000     ; $F
        dw $0000     ; $10
        dw $0000     ; $11
        dw $0000     ; $12
        dw $0000     ; $13
        dw $0000     ; $14
        dw $0000     ; $15
        dw $0000     ; $16
        dw $0000     ; $17
        dw $0000     ; $18
        dw $0000     ; $19
        dw $0000     ; $1A
        dw $0000     ; $1B
        dw $0000     ; $1C
        dw $0000     ; $1D
        dw $0000     ; $1E
        dw $0000     ; $1F
        dw $0000     ; $20
        dw $0000     ; $21
        dw $0000     ; $22
        dw $0000     ; $23
        dw $0000     ; $24
        dw $0000     ; $25
        dw $0000     ; $26
        dw $0000     ; $27
        dw $0000     ; $28
        dw $0000     ; $29
        dw $0000     ; $2A
        dw $0000     ; $2B
        dw $0000     ; $2C
        dw $0000     ; $2D
        dw $0000     ; $2E
        dw $0000     ; $2F
        dw $0000     ; $30
        dw $0000     ; $31
        dw $0000     ; $32
        dw $0000     ; $33
        dw $0000     ; $34
        dw $0000     ; $35
        dw $0000     ; $36
        dw $0000     ; $37
        dw $0000     ; $38
        dw $0000     ; $39
        dw $0000     ; $3A
        dw $0000     ; $3B
        dw $0000     ; $3C
        dw $0000     ; $3D
        dw $0000     ; $3E
        dw $0000     ; $3F
        dw $0000     ; $40
        dw $0000     ; $41
        dw $0000     ; $42
        dw $0000     ; $43
        dw $0000     ; $44
        dw $0000     ; $45
        dw $0000     ; $46
        dw $0000     ; $47
        dw $0000     ; $48
        dw $0000     ; $49
        dw $0000     ; $4A
        dw $0000     ; $4B
        dw $0000     ; $4C
        dw $0000     ; $4D
        dw $0000     ; $4E
        dw $0000     ; $4F
        dw $0000     ; $50
        dw $0000     ; $51
        dw $0000     ; $52
        dw $0000     ; $53
        dw $0000     ; $54
        dw $0000     ; $55
        dw $0000     ; $56
        dw $0000     ; $57
        dw $0000     ; $58
        dw $0000     ; $59
        dw $0000     ; $5A
        dw $0000     ; $5B
        dw $0000     ; $5C
        dw $0000     ; $5D
        dw $0000     ; $5E
        dw $0000     ; $5F
        dw $0000     ; $60
        dw $0000     ; $61
        dw $0000     ; $62
        dw $0000     ; $63
        dw $0000     ; $64
        dw $0000     ; $65
        dw $0000     ; $66
        dw $0000     ; $67
        dw $0000     ; $68
        dw $0000     ; $69
        dw $0000     ; $6A
        dw $0000     ; $6B
        dw $0000     ; $6C
        dw $0000     ; $6D
        dw $0000     ; $6E
        dw $0000     ; $6F
        dw $0000     ; $70
        dw $0000     ; $71
        dw $0000     ; $72
        dw $0000     ; $73
        dw $0000     ; $74
        dw $0000     ; $75
        dw $0000     ; $76
        dw $0000     ; $77
        dw $0000     ; $78
        dw $0000     ; $79
        dw $0000     ; $7A
        dw $0000     ; $7B
        dw $0000     ; $7C
        dw $0000     ; $7D
        dw $0000     ; $7E
        dw $0000     ; $7F
        dw $0000     ; $80
        dw code_str_ex129 ; $81
        dw code_str_ex130 ; $82
        dw code_str_ex131 ; $83
        dw code_str_ex132 ; $84
        dw code_str_ex133 ; $85
        dw code_str_ex134 ; $86
        dw code_str_ex135 ; $87
        dw code_str_ex136 ; $88
        dw code_str_ex137 ; $89
        dw code_str_ex138 ; $8A
        dw code_str_ex139 ; $8B
        dw code_str_ex140 ; $8C
        dw code_str_ex141 ; $8D
        dw code_str_ex142 ; $8E
        dw code_str_ex143 ; $8F
        dw code_str_ex144 ; $90
        dw code_str_ex145 ; $91
        dw code_str_ex146 ; $92
        dw code_str_ex147 ; $93
        dw code_str_ex148 ; $94
        dw code_str_ex149 ; $95
        dw code_str_ex150 ; $96
        dw code_str_ex151 ; $97
        dw code_str_ex152 ; $98
        dw code_str_ex153 ; $99
        dw code_str_ex154 ; $9A
        dw code_str_ex155 ; $9B
        dw code_str_ex156 ; $9C
        dw code_str_ex157 ; $9D
        dw code_str_ex158 ; $9E
        dw code_str_ex159 ; $9F
        dw code_str_ex160 ; $A0
        dw code_str_ex161 ; $A1
        dw code_str_ex162 ; $A2
        dw code_str_ex163 ; $A3
        dw code_str_ex164 ; $A4
        dw code_str_ex165 ; $A5
        dw code_str_ex166 ; $A6
        dw code_str_ex167 ; $A7
        dw code_str_ex168 ; $A8
        dw code_str_ex169 ; $A9
        dw code_str_ex170 ; $AA
        dw code_str_ex171 ; $AB
        dw code_str_ex172 ; $AC
        dw code_str_ex173 ; $AD
        dw code_str_ex174 ; $AE
        dw code_str_ex175 ; $AF
        dw code_str_ex176 ; $B0
        dw $0000     ; $B1
        dw $0000     ; $B2
        dw $0000     ; $B3
        dw $0000     ; $B4
        dw $0000     ; $B5
        dw $0000     ; $B6
        dw $0000     ; $B7
        dw $0000     ; $B8
        dw $0000     ; $B9
        dw $0000     ; $BA
        dw $0000     ; $BB
        dw $0000     ; $BC
        dw $0000     ; $BD
        dw $0000     ; $BE
        dw $0000     ; $BF
        dw $0000     ; $C0
        dw $0000     ; $C1
        dw $0000     ; $C2
        dw $0000     ; $C3
        dw $0000     ; $C4
        dw $0000     ; $C5
        dw $0000     ; $C6
        dw $0000     ; $C7
        dw $0000     ; $C8
        dw $0000     ; $C9
        dw $0000     ; $CA
        dw $0000     ; $CB
        dw $0000     ; $CC
        dw $0000     ; $CD
        dw $0000     ; $CE
        dw $0000     ; $CF
        dw $0000     ; $D0
        dw $0000     ; $D1
        dw $0000     ; $D2
        dw $0000     ; $D3
        dw $0000     ; $D4
        dw $0000     ; $D5
        dw $0000     ; $D6
        dw $0000     ; $D7
        dw $0000     ; $D8
        dw $0000     ; $D9
        dw $0000     ; $DA
        dw $0000     ; $DB
        dw $0000     ; $DC
        dw $0000     ; $DD
        dw $0000     ; $DE
        dw $0000     ; $DF
        dw $0000     ; $E0
        dw $0000     ; $E1
        dw $0000     ; $E2
        dw $0000     ; $E3
        dw $0000     ; $E4
        dw $0000     ; $E5
        dw $0000     ; $E6
        dw $0000     ; $E7
        dw $0000     ; $E8
        dw $0000     ; $E9
        dw $0000     ; $EA
        dw $0000     ; $EB
        dw $0000     ; $EC
        dw $0000     ; $ED
        dw $0000     ; $EE
        dw $0000     ; $EF
        dw $0000     ; $F0
        dw $0000     ; $F1
        dw $0000     ; $F2
        dw $0000     ; $F3
        dw $0000     ; $F4
        dw $0000     ; $F5
        dw $0000     ; $F6
        dw $0000     ; $F7
        dw $0000     ; $F8
        dw $0000     ; $F9
        dw $0000     ; $FA
        dw $0000     ; $FB
        dw $0000     ; $FC
        dw $0000     ; $FD
        dw $0000     ; $FE
        dw $0000     ; $FF


;------------------------
code_str129: ; $81
        db "END",$00

code_str130: ; $82
        db "FOR",$00

code_str131: ; $83
        db "NEXT",$00

code_str132: ; $84
        db "DATA",$00

code_str133: ; $85
        db "INPUT",$00

code_str134: ; $86
        db "DIM",$00

code_str135: ; $87
        db "READ",$00

code_str136: ; $88
        db "LET",$00

code_str137: ; $89
        db "GOTO",$00

code_str138: ; $8A
        db "RUN",$00

code_str139: ; $8B
        db "IF",$00

code_str140: ; $8C
        db "RESTORE",$00

code_str141: ; $8D
        db "GOSUB",$00

code_str142: ; $8E
        db "RETURN",$00

code_str144: ; $90
        db "STOP",$00

code_str145: ; $91
        db "PRINT",$00

code_str146: ; $92
        db "CLEAR",$00

code_str147: ; $93
        db "LIST",$00

code_str148: ; $94
        db "NEW",$00

code_str149: ; $95
        db "ON",$00

code_str150: ; $96
        db "WAIT",$00

code_str151: ; $97
        db "DEF",$00

code_str152: ; $98
        db "POKE",$00

code_str153: ; $99
        db "CONT",$00

code_str154: ; $9A
        db "CSAVE",$00

code_str155: ; $9B
        db "CLOAD",$00

code_str156: ; $9C
        db "OUT",$00

code_str157: ; $9D
        db "LPRINT",$00

code_str158: ; $9E
        db "LLIST",$00

code_str159: ; $9F
        db "CLS",$00

code_str160: ; $A0
        db "WIDTH",$00

code_str162: ; $A2
        db "TRON",$00

code_str163: ; $A3
        db "TROFF",$00

code_str164: ; $A4
        db "SWAP",$00

code_str165: ; $A5
        db "ERASE",$00

code_str166: ; $A6
        db "ERROR",$00

code_str167: ; $A7
        db "RESUME",$00

code_str168: ; $A8
        db "DELETE",$00

code_str169: ; $A9
        db "AUTO",$00

code_str170: ; $AA
        db "RENUM",$00

code_str171: ; $AB
        db "DEFSTR",$00

code_str172: ; $AC
        db "DEFINT",$00

code_str173: ; $AD
        db "DEFSNG",$00

code_str174: ; $AE
        db "DEFDBL",$00

code_str175: ; $AF
        db "LINE",$00

code_str176: ; $B0
        db "OPEN",$00

code_str177: ; $B1
        db "FIELD",$00

code_str178: ; $B2
        db "GET",$00

code_str179: ; $B3
        db "PUT",$00

code_str180: ; $B4
        db "CLOSE",$00

code_str181: ; $B5
        db "LOAD",$00

code_str182: ; $B6
        db "MERGE",$00

code_str183: ; $B7
        db "FILES",$00

code_str184: ; $B8
        db "LSET",$00

code_str185: ; $B9
        db "RSET",$00

code_str186: ; $BA
        db "SAVE",$00

code_str187: ; $BB
        db "LFILES",$00

code_str188: ; $BC
        db "CIRCLE",$00

code_str189: ; $BD
        db "COLOR",$00

code_str190: ; $BE
        db "DRAW",$00

code_str191: ; $BF
        db "PAINT",$00

code_str192: ; $C0
        db "BEEP",$00

code_str193: ; $C1
        db "PLAY",$00

code_str194: ; $C2
        db "PSET",$00

code_str195: ; $C3
        db "PRESET",$00

code_str196: ; $C4
        db "SOUND",$00

code_str197: ; $C5
        db "SCREEN",$00

code_str198: ; $C6
        db "VPOKE",$00

code_str199: ; $C7
        db "SPRITE",$00

code_str200: ; $C8
        db "VDP",$00

code_str201: ; $C9
        db "BASE",$00

code_str202: ; $CA
        db "CALL",$00

code_str203: ; $CB
        db "TIME",$00

code_str204: ; $CC
        db "KEY",$00

code_str205: ; $CD
        db "MAX",$00

code_str206: ; $CE
        db "MOTOR",$00

code_str207: ; $CF
        db "BLOAD",$00

code_str208: ; $D0
        db "BSAVE",$00

code_str209: ; $D1
        db "DSKO$",$00

code_str210: ; $D2
        db "SET",$00

code_str211: ; $D3
        db "NAME",$00

code_str212: ; $D4
        db "KILL",$00

code_str213: ; $D5
        db "IPL",$00

code_str214: ; $D6
        db "COPY",$00

code_str215: ; $D7
        db "CMD",$00

code_str216: ; $D8
        db "LOCATE",$00

code_str217: ; $D9
        db "TO",$00

code_str218: ; $DA
        db "THEN",$00

code_str219: ; $DB
        db "TAB(",$00

code_str220: ; $DC
        db "STEP",$00

code_str221: ; $DD
        db "USR",$00

code_str222: ; $DE
        db "FN",$00

code_str223: ; $DF
        db "SPC(",$00

code_str224: ; $E0
        db "NOT",$00

code_str225: ; $E1
        db "ERL",$00

code_str226: ; $E2
        db "ERR",$00

code_str227: ; $E3
        db "STRING$",$00

code_str228: ; $E4
        db "USING",$00

code_str229: ; $E5
        db "INSTR",$00

code_str231: ; $E7
        db "VARPTR",$00

code_str232: ; $E8
        db "CSRLIN",$00

code_str233: ; $E9
        db "ATTR$",$00

code_str234: ; $EA
        db "DSKI$",$00

code_str235: ; $EB
        db "OFF",$00

code_str236: ; $EC
        db "INKEY$",$00

code_str237: ; $ED
        db "POINT",$00

code_str238: ; $EE
        db ">",$00

code_str239: ; $EF
        db "=",$00

code_str240: ; $F0
        db "<",$00

code_str241: ; $F1
        db "+",$00

code_str242: ; $F2
        db "-",$00

code_str243: ; $F3
        db "*",$00

code_str244: ; $F4
        db "/",$00

code_str245: ; $F5
        db "^",$00

code_str246: ; $F6
        db "AND",$00

code_str247: ; $F7
        db "OR",$00

code_str248: ; $F8
        db "XOR",$00

code_str249: ; $F9
        db "EQV",$00

code_str250: ; $FA
        db "IMP",$00

code_str251: ; $FB
        db "MOD",$00

code_str252: ; $FC
        db "\\",$00



;------------------------
code_str_ex129: ; $81
        db "LEFT$",$00

code_str_ex130: ; $82
        db "RIGHT$",$00

code_str_ex131: ; $83
        db "MID$",$00

code_str_ex132: ; $84
        db "SGN",$00

code_str_ex133: ; $85
        db "INT",$00

code_str_ex134: ; $86
        db "ABS",$00

code_str_ex135: ; $87
        db "SQR",$00

code_str_ex136: ; $88
        db "RND",$00

code_str_ex137: ; $89
        db "SIN",$00

code_str_ex138: ; $8A
        db "LOG",$00

code_str_ex139: ; $8B
        db "EXP",$00

code_str_ex140: ; $8C
        db "COS",$00

code_str_ex141: ; $8D
        db "TAN",$00

code_str_ex142: ; $8E
        db "ATN",$00

code_str_ex143: ; $8F
        db "REM",$00

code_str_ex144: ; $90
        db "INP",$00

code_str_ex145: ; $91
        db "POS",$00

code_str_ex146: ; $92
        db "LEN",$00

code_str_ex147: ; $93
        db "STR$",$00

code_str_ex148: ; $94
        db "VAL",$00

code_str_ex149: ; $95
        db "ASC",$00

code_str_ex150: ; $96
        db "CHR$",$00

code_str_ex151: ; $97
        db "PEEK",$00

code_str_ex152: ; $98
        db "VPEEK",$00

code_str_ex153: ; $99
        db "SPACE$",$00

code_str_ex154: ; $9A
        db "OCT$",$00

code_str_ex155: ; $9B
        db "HEX$",$00

code_str_ex156: ; $9C
        db "LPOS",$00

code_str_ex157: ; $9D
        db "BIN$",$00

code_str_ex158: ; $9E
        db "CINT",$00

code_str_ex159: ; $9F
        db "CSNG",$00

code_str_ex160: ; $A0
        db "CDBL",$00

code_str_ex161: ; $A1
        db "FIX",$00

code_str_ex162: ; $A2
        db "STICK",$00

code_str_ex163: ; $A3
        db "STRIG",$00

code_str_ex164: ; $A4
        db "PDL",$00

code_str_ex165: ; $A5
        db "PAD",$00

code_str_ex166: ; $A6
        db "DSKF",$00

code_str_ex167: ; $A7
        db "FPOS",$00

code_str_ex168: ; $A8
        db "CVI",$00

code_str_ex169: ; $A9
        db "CVS",$00

code_str_ex170: ; $AA
        db "CVD",$00

code_str_ex171: ; $AB
        db "EOF",$00

code_str_ex172: ; $AC
        db "LOC",$00

code_str_ex173: ; $AD
        db "LOF",$00

code_str_ex174: ; $AE
        db "MKI$",$00

code_str_ex175: ; $AF
        db "MKS$",$00

code_str_ex176: ; $B0
        db "MKD$",$00

