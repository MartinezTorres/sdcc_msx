%include "z80r800.inc"
%include "z80().inc"
%include "tniasm.inc"
%macro if\%if\%endmacro
%macro else\%else\ %endmacro
%macro endif\%endif\ %endmacro
%if #0 = 3
%symfile #3
%outfile #2
%include #1
%endif
