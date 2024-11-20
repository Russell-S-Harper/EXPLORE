;
; File generated by cc65 v 2.19 - Git 39619b629
;
	.fopt		compiler,"cc65 v 2.19 - Git 39619b629"
	.setcpu		"65C02"
	.smart		on
	.autoimport	on
	.case		on
	.debuginfo	off
	.importzp	sp, sreg, regsave, regbank
	.importzp	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4
	.macpack	longbranch
	.import		_stdout
	.import		_fclose
	.import		_fopen
	.import		_fputc
	.import		_fputs
	.import		_fread
	.import		_malloc
	.import		_calloc
	.import		_exit
	.export		_OutputAsNumber
	.export		_ExitProgram
	.import		_InitXM
	.import		_AllocXM
	.import		_GetXMAddress
	.import		_GetXMDirectSigned
	.export		_InitProgram
	.import		_InitSpecific
	.import		_MultiplyThenDivide
	.export		_AdvancePlayer
	.export		_Sin
	.export		_Cos
	.export		_g_string_data
	.export		_g_trig_data
	.export		_g_arena_data
	.export		_g_exploding_prm
	.export		_g_exploding_aux
	.export		_g_vehicle_data
	.export		_g_vehicles
	.export		_g_levels
	.export		_g_exit_program
	.export		_g_display_width
	.export		_g_display_height
	.export		_g_frame_counter
	.export		_g_squares
	.export		_g_arena_index
	.export		_g_max_arena_segments
	.export		_g_max_arena_vertices
	.export		_g_vehicle_index
	.export		_g_max_vehicle_segments
	.export		_g_max_vehicle_vertices

.segment	"RODATA"

S0003:
	.byte	$0D,$D2,$55,$4E,$20,$C4,$C1,$D4,$C1,$2E,$D0,$D2,$C7,$21,$0D,$00
S0001:
	.byte	$45,$58,$50,$4C,$4F,$52,$45,$2E,$44,$41,$54,$00
S0002:
	.byte	$52,$42,$00

.segment	"BSS"

_g_string_data:
	.res	2,$00
_g_trig_data:
	.res	2,$00
_g_arena_data:
	.res	2,$00
_g_exploding_prm:
	.res	2,$00
_g_exploding_aux:
	.res	2,$00
_g_vehicle_data:
	.res	2,$00
_g_vehicles:
	.res	2,$00
_g_levels:
	.res	2,$00
_g_exit_program:
	.res	1,$00
_g_display_width:
	.res	2,$00
_g_display_height:
	.res	2,$00
_g_frame_counter:
	.res	2,$00
_g_squares:
	.res	2,$00
_g_arena_index:
	.res	2,$00
_g_max_arena_segments:
	.res	2,$00
_g_max_arena_vertices:
	.res	2,$00
_g_vehicle_index:
	.res	2,$00
_g_max_vehicle_segments:
	.res	2,$00
_g_max_vehicle_vertices:
	.res	2,$00

; ---------------------------------------------------------------
; void __near__ OutputAsNumber (char prefix, int value)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_OutputAsNumber: near

.segment	"CODE"

	jsr     pushax
	ldy     #$02
	lda     (sp),y
	jsr     pusha0
	lda     _stdout
	ldx     _stdout+1
	jsr     _fputc
	ldy     #$01
	lda     (sp),y
	tax
	cpx     #$80
	bcc     L0002
	ldy     #$01
	lda     (sp),y
	tax
	lda     (sp)
	jsr     negax
	jsr     stax0sp
	lda     #$2D
	jsr     pusha0
	lda     _stdout
	ldx     _stdout+1
	jsr     _fputc
L0002:	ldx     #$27
	lda     #$10
L000F:	sta     M0002
	stx     M0002+1
	lda     M0002
	cmp     #$02
	lda     M0002+1
	sbc     #$00
	bvs     L0007
	eor     #$80
L0007:	bpl     L0004
	lda     #$30
	sta     M0001
	bra     L000A
L000E:	inc     M0001
	lda     M0002
	ldx     M0002+1
	jsr     subeq0sp
L000A:	ldy     #$01
	lda     (sp),y
	tax
	lda     (sp)
	cmp     M0002
	txa
	sbc     M0002+1
	bvs     L000C
	eor     #$80
L000C:	bmi     L000E
	lda     M0001
	jsr     pusha0
	lda     _stdout
	ldx     _stdout+1
	jsr     _fputc
	lda     M0002
	ldx     M0002+1
	jsr     pushax
	lda     #$0A
	jsr     tosdiva0
	bra     L000F
L0004:	ldy     #$01
	lda     (sp),y
	tax
	lda     (sp)
	clc
	adc     #$30
	bcc     L000B
	inx
L000B:	jsr     pushax
	lda     _stdout
	ldx     _stdout+1
	jsr     _fputc
	lda     #$0D
	jsr     pusha0
	lda     _stdout
	ldx     _stdout+1
	jsr     _fputc
	jmp     incsp3

.segment	"BSS"

M0001:
	.res	1,$00
M0002:
	.res	2,$00

.endproc

; ---------------------------------------------------------------
; void __near__ ExitProgram (int stat)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_ExitProgram: near

.segment	"CODE"

	jsr     pushax
	ldy     #$01
	lda     (sp),y
	bne     L0007
	lda     (sp)
	cmp     #$06
	beq     L0002
L0007:	lda     _g_string_data
	ora     _g_string_data+1
	beq     L0004
	lda     _g_string_data
	ldx     _g_string_data+1
	jsr     pushax
	ldy     #$03
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	jsr     _GetXMAddress
	bra     L000A
L0004:	lda     #<(S0003)
	ldx     #>(S0003)
L000A:	jsr     pushax
	lda     _stdout
	ldx     _stdout+1
	jsr     _fputs
	ldy     #$01
	lda     (sp),y
	tax
	lda     (sp)
	jsr     _exit
L0002:	jmp     incsp2

.endproc

; ---------------------------------------------------------------
; void __near__ InitProgram (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_InitProgram: near

.segment	"CODE"

	jsr     _InitXM
	lda     #<(S0001)
	ldx     #>(S0001)
	jsr     _InitData
	jsr     _InitSpecific
	jsr     _InitSquares
	jmp     _InitVehicles

.endproc

; ---------------------------------------------------------------
; unsigned char __near__ AdvancePlayer (struct $anon-struct-0021 *player)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_AdvancePlayer: near

.segment	"CODE"

	jsr     pushax
	ldy     #$01
	lda     (sp),y
	tax
	lda     (sp)
	ldy     #$06
	jsr     ldaidx
	tax
	bmi     L0002
	ldy     #$01
	lda     (sp),y
	tax
	lda     (sp)
	ldy     #$06
	jsr     ldaidx
	jsr     mulax10
	clc
	adc     _g_levels
	sta     ptr1
	txa
	adc     _g_levels+1
	sta     ptr1+1
	lda     (ptr1)
	beq     L0002
	ldx     #$00
	txa
	jmp     incsp2
L0002:	ldy     #$01
	lda     (sp),y
	sta     ptr1+1
	lda     (sp)
	sta     ptr1
	ldy     #$06
	lda     #$01
	clc
	adc     (ptr1),y
	sta     (ptr1),y
	ldy     #$01
	lda     (sp),y
	tax
	lda     (sp)
	ldy     #$06
	jsr     ldaidx
	jsr     mulax10
	clc
	adc     _g_levels
	sta     M0001
	txa
	adc     _g_levels+1
	sta     M0001+1
	lda     _g_arena_index
	ldx     _g_arena_index+1
	jsr     pushax
	lda     M0001
	ldx     M0001+1
	ldy     #$02
	jsr     ldaidx
	jsr     tosicmp
	bpl     L0006
	lda     M0001
	ldx     M0001+1
	ldy     #$02
	jsr     ldaidx
	sta     _g_arena_index
	stx     _g_arena_index+1
L0006:	ldy     #$01
	lda     (sp),y
	sta     ptr1+1
	lda     (sp)
	sta     ptr1
	lda     #$3C
	ldy     #$0F
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
	ldy     #$01
	lda     (sp),y
	tax
	lda     (sp)
	clc
	adc     #$1D
	bcc     L0007
	inx
L0007:	sta     sreg
	stx     sreg+1
	lda     M0001+1
	sta     ptr1+1
	lda     M0001
	sta     ptr1
	ldy     #$07
	lda     (ptr1),y
	tax
	dey
	lda     (ptr1),y
	sta     (sreg)
	ldy     #$01
	txa
	sta     (sreg),y
	lda     (sp),y
	tax
	lda     (sp)
	clc
	adc     #$1D
	bcc     L000A
	inx
	clc
L000A:	adc     #$04
	bcc     L0009
	inx
L0009:	sta     sreg
	stx     sreg+1
	lda     M0001+1
	sta     ptr1+1
	lda     M0001
	sta     ptr1
	ldy     #$09
	lda     (ptr1),y
	tax
	dey
	lda     (ptr1),y
	sta     (sreg)
	ldy     #$01
	txa
	sta     (sreg),y
	lda     (sp),y
	sta     sreg+1
	lda     (sp)
	sta     sreg
	lda     M0001+1
	sta     ptr1+1
	lda     M0001
	sta     ptr1
	ldy     #$01
	lda     (ptr1),y
	sta     (sreg),y
	lda     (sp),y
	sta     sreg+1
	lda     (sp)
	sta     sreg
	lda     M0001
	ldx     M0001+1
	ldy     #$03
	jsr     ldaidx
	ldy     #$07
	sta     (sreg),y
	ldy     #$01
	lda     (sp),y
	sta     sreg+1
	lda     (sp)
	sta     sreg
	lda     M0001
	ldx     M0001+1
	ldy     #$04
	jsr     ldaidx
	ldy     #$09
	sta     (sreg),y
	ldy     #$01
	lda     (sp),y
	sta     sreg+1
	lda     (sp)
	sta     sreg
	lda     M0001
	ldx     M0001+1
	ldy     #$05
	jsr     ldaidx
	ldy     #$0A
	sta     (sreg),y
	ldx     #$00
	lda     #$01
	jmp     incsp2

.segment	"BSS"

M0001:
	.res	2,$00

.endproc

; ---------------------------------------------------------------
; int __near__ Sin (int angle)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_Sin: near

.segment	"CODE"

	jsr     pushax
	lda     _g_trig_data
	ldx     _g_trig_data+1
	jsr     pushax
	ldy     #$03
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	tay
	txa
	and     #$01
	tax
	tya
	jsr     _GetXMDirectSigned
	jmp     incsp2

.endproc

; ---------------------------------------------------------------
; int __near__ Cos (int angle)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_Cos: near

.segment	"CODE"

	jsr     pushax
	lda     _g_trig_data
	ldx     _g_trig_data+1
	jsr     pushax
	ldy     #$03
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	clc
	adc     #$80
	bcc     L0002
	inx
L0002:	tay
	txa
	and     #$01
	tax
	tya
	jsr     _GetXMDirectSigned
	jmp     incsp2

.endproc

; ---------------------------------------------------------------
; void __near__ InitData (char *file)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_InitData: near

.segment	"CODE"

	jsr     pushax
	stz     M0001
	jsr     pushw0sp
	lda     #<(S0002)
	ldx     #>(S0002)
	jsr     _fopen
	sta     M0006
	stx     M0006+1
	jsr     bnegax
	jeq     L003A
	txa
	jmp     L0038
L0003:	jsr     decsp4
	lda     #<(M0001)
	ldy     #$02
	sta     (sp),y
	iny
	lda     #>(M0001)
	sta     (sp),y
	lda     #$01
	sta     (sp)
	tay
	dea
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	ldx     #$00
	lda     M0001
	cmp     #$01
	beq     L0008
	cmp     #$02
	jeq     L000F
	cmp     #$03
	jeq     L0010
	cmp     #$04
	jeq     L0016
	cmp     #$05
	jeq     L0023
	cmp     #$06
	jeq     L0031
	jmp     L0039
L0008:	jsr     decsp4
	lda     #<(M0005)
	ldy     #$02
	sta     (sp),y
	iny
	lda     #>(M0005)
	sta     (sp),y
	lda     #$02
	sta     (sp)
	ldy     #$01
	txa
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	lda     #$06
	jsr     pusha0
	lda     M0005
	ldx     M0005+1
	stx     tmp1
	asl     a
	rol     tmp1
	ldx     tmp1
	jsr     _AllocXM
	sta     _g_string_data
	stx     _g_string_data+1
	stz     M0003
	stz     M0003+1
L0009:	lda     M0003
	cmp     #$06
	lda     M0003+1
	sbc     #$00
	bvc     L000D
	eor     #$80
L000D:	jpl     L003A
	jsr     decsp4
	lda     _g_string_data
	ldx     _g_string_data+1
	jsr     pushax
	lda     M0003
	ldx     M0003+1
	jsr     _GetXMAddress
	ldy     #$02
	sta     (sp),y
	iny
	txa
	sta     (sp),y
	lda     M0005
	sta     (sp)
	ldy     #$01
	lda     M0005+1
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	inc     M0003
	bne     L0009
	inc     M0003+1
	bra     L0009
L000F:	tax
	lda     #$00
	jsr     pushax
	tax
	lda     #$02
	jsr     _AllocXM
	sta     _g_trig_data
	stx     _g_trig_data+1
	jsr     decsp4
	lda     _g_trig_data
	ldx     _g_trig_data+1
	jsr     pushax
	ldx     #$00
	txa
	jsr     _GetXMAddress
	ldy     #$02
	sta     (sp),y
	iny
	txa
	sta     (sp),y
	lda     #$00
	sta     (sp)
	ldy     #$01
	lda     #$04
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	jmp     L003A
L0010:	jsr     decsp4
	lda     #<(M0004)
	ldy     #$02
	sta     (sp),y
	iny
	lda     #>(M0004)
	sta     (sp),y
	lda     #$02
	sta     (sp)
	ldy     #$01
	txa
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	jsr     decsp4
	lda     #<(M0005)
	ldy     #$02
	sta     (sp),y
	iny
	lda     #>(M0005)
	sta     (sp),y
	lda     #$02
	sta     (sp)
	ldy     #$01
	lda     #$00
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	lda     M0004
	ldx     M0004+1
	jsr     pushax
	lda     M0005
	ldx     M0005+1
	jsr     _AllocXM
	sta     _g_arena_data
	stx     _g_arena_data+1
	stz     _g_arena_index
	stz     _g_arena_index+1
	jsr     decsp4
	lda     #<(_g_max_arena_vertices)
	ldy     #$02
	sta     (sp),y
	iny
	lda     #>(_g_max_arena_vertices)
	sta     (sp),y
	lda     #$02
	sta     (sp)
	ldy     #$01
	lda     #$00
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	jsr     decsp4
	lda     #<(_g_max_arena_segments)
	ldy     #$02
	sta     (sp),y
	iny
	lda     #>(_g_max_arena_segments)
	sta     (sp),y
	lda     #$02
	sta     (sp)
	ldy     #$01
	lda     #$00
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	stz     M0003
	stz     M0003+1
L0011:	lda     M0003
	cmp     M0004
	lda     M0003+1
	sbc     M0004+1
	bvc     L0033
	eor     #$80
L0033:	jpl     L003A
	jsr     decsp4
	lda     _g_arena_data
	ldx     _g_arena_data+1
	jsr     pushax
	lda     M0003
	ldx     M0003+1
	jsr     _GetXMAddress
	ldy     #$02
	sta     (sp),y
	iny
	txa
	sta     (sp),y
	lda     M0005
	sta     (sp)
	ldy     #$01
	lda     M0005+1
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	inc     M0003
	bne     L0011
	inc     M0003+1
	bra     L0011
L0016:	jsr     decsp4
	lda     #<(M0004)
	ldy     #$02
	sta     (sp),y
	iny
	lda     #>(M0004)
	sta     (sp),y
	lda     #$02
	sta     (sp)
	ldy     #$01
	txa
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	jsr     decsp4
	lda     #<(M0005)
	ldy     #$02
	sta     (sp),y
	iny
	lda     #>(M0005)
	sta     (sp),y
	lda     #$02
	sta     (sp)
	ldy     #$01
	lda     #$00
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	lda     M0004
	ldx     M0004+1
	stx     tmp1
	asl     a
	rol     tmp1
	ldx     tmp1
	jsr     _malloc
	sta     _g_vehicle_data
	stx     _g_vehicle_data+1
	stz     M0003
	stz     M0003+1
L0017:	lda     M0003
	cmp     M0004
	lda     M0003+1
	sbc     M0004+1
	bvc     L0034
	eor     #$80
L0034:	bpl     L0018
	lda     M0003
	ldx     M0003+1
	stx     tmp1
	asl     a
	rol     tmp1
	clc
	adc     _g_vehicle_data
	pha
	lda     tmp1
	adc     _g_vehicle_data+1
	tax
	pla
	jsr     pushax
	lda     #$01
	jsr     pusha0
	lda     M0005
	ldx     M0005+1
	jsr     _AllocXM
	ldy     #$00
	jsr     staxspidx
	inc     M0003
	bne     L0017
	inc     M0003+1
	bra     L0017
L0018:	lda     M0004
	ldx     M0004+1
	sec
	sbc     #$02
	bcs     L001C
	dex
L001C:	stx     tmp1
	asl     a
	rol     tmp1
	clc
	adc     _g_vehicle_data
	sta     ptr1
	lda     tmp1
	adc     _g_vehicle_data+1
	sta     ptr1+1
	ldy     #$01
	lda     (ptr1),y
	sta     _g_exploding_prm+1
	lda     (ptr1)
	sta     _g_exploding_prm
	lda     M0004
	ldx     M0004+1
	sec
	sbc     #$01
	bcs     L001D
	dex
L001D:	stx     tmp1
	asl     a
	rol     tmp1
	clc
	adc     _g_vehicle_data
	sta     ptr1
	lda     tmp1
	adc     _g_vehicle_data+1
	sta     ptr1+1
	ldy     #$01
	lda     (ptr1),y
	sta     _g_exploding_aux+1
	lda     (ptr1)
	sta     _g_exploding_aux
	jsr     decsp4
	lda     #<(_g_max_vehicle_vertices)
	iny
	sta     (sp),y
	iny
	lda     #>(_g_max_vehicle_vertices)
	sta     (sp),y
	lda     #$02
	sta     (sp)
	ldy     #$01
	lda     #$00
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	jsr     decsp4
	lda     #<(_g_max_vehicle_segments)
	ldy     #$02
	sta     (sp),y
	iny
	lda     #>(_g_max_vehicle_segments)
	sta     (sp),y
	lda     #$02
	sta     (sp)
	ldy     #$01
	lda     #$00
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	stz     M0003
	stz     M0003+1
L001E:	lda     M0003
	cmp     M0004
	lda     M0003+1
	sbc     M0004+1
	bvc     L0035
	eor     #$80
L0035:	jpl     L003A
	jsr     decsp4
	lda     M0003
	ldx     M0003+1
	stx     tmp1
	asl     a
	rol     tmp1
	clc
	adc     _g_vehicle_data
	sta     ptr1
	lda     tmp1
	adc     _g_vehicle_data+1
	sta     ptr1+1
	ldy     #$01
	lda     (ptr1),y
	tax
	lda     (ptr1)
	jsr     pushax
	ldx     #$00
	txa
	jsr     _GetXMAddress
	ldy     #$02
	sta     (sp),y
	iny
	txa
	sta     (sp),y
	lda     M0005
	sta     (sp)
	ldy     #$01
	lda     M0005+1
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	inc     M0003
	bne     L001E
	inc     M0003+1
	bra     L001E
L0023:	jsr     decsp4
	lda     #<(M0004)
	ldy     #$02
	sta     (sp),y
	iny
	lda     #>(M0004)
	sta     (sp),y
	lda     #$02
	sta     (sp)
	ldy     #$01
	txa
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	lda     M0004
	ldx     M0004+1
	jsr     mulax10
	jsr     _malloc
	sta     _g_levels
	stx     _g_levels+1
	stz     M0003
	stz     M0003+1
L0024:	lda     M0003
	cmp     M0004
	lda     M0003+1
	sbc     M0004+1
	bvc     L0036
	eor     #$80
L0036:	jpl     L003A
	jsr     decsp4
	lda     #<(M0002)
	ldy     #$02
	sta     (sp),y
	iny
	lda     #>(M0002)
	sta     (sp),y
	lda     #$01
	sta     (sp)
	tay
	dea
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	lda     M0003
	ldx     M0003+1
	jsr     mulax10
	clc
	adc     _g_levels
	sta     ptr1
	txa
	adc     _g_levels+1
	sta     ptr1+1
	lda     M0002
	ldy     #$02
	sta     (ptr1),y
	jsr     decsp4
	lda     #<(M0002)
	sta     (sp),y
	iny
	lda     #>(M0002)
	sta     (sp),y
	lda     #$01
	sta     (sp)
	tay
	dea
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	lda     M0003
	ldx     M0003+1
	jsr     mulax10
	clc
	adc     _g_levels
	sta     sreg
	txa
	adc     _g_levels+1
	sta     sreg+1
	ldx     #$00
	lda     M0002
	bpl     L002A
	dex
L002A:	stx     tmp1
	asl     a
	rol     tmp1
	clc
	adc     _g_vehicle_data
	sta     ptr1
	lda     tmp1
	adc     _g_vehicle_data+1
	sta     ptr1+1
	ldy     #$01
	lda     (ptr1),y
	tax
	lda     (ptr1)
	ldy     #$06
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
	jsr     decsp4
	lda     #<(M0002)
	ldy     #$02
	sta     (sp),y
	iny
	lda     #>(M0002)
	sta     (sp),y
	lda     #$01
	sta     (sp)
	tay
	dea
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	lda     M0003
	ldx     M0003+1
	jsr     mulax10
	clc
	adc     _g_levels
	sta     ptr1
	txa
	adc     _g_levels+1
	sta     ptr1+1
	lda     M0002
	jsr     bnega
	jsr     bnegax
	ldy     #$01
	sta     (ptr1),y
	jsr     decsp4
	lda     #<(M0002)
	iny
	sta     (sp),y
	iny
	lda     #>(M0002)
	sta     (sp),y
	lda     #$01
	sta     (sp)
	tay
	txa
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	lda     M0003
	ldx     M0003+1
	jsr     mulax10
	clc
	adc     _g_levels
	sta     ptr1
	txa
	adc     _g_levels+1
	sta     ptr1+1
	lda     M0002
	ldy     #$03
	sta     (ptr1),y
	jsr     decsp4
	lda     #<(M0002)
	dey
	sta     (sp),y
	iny
	lda     #>(M0002)
	sta     (sp),y
	lda     #$01
	sta     (sp)
	tay
	dea
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	lda     M0003
	ldx     M0003+1
	jsr     mulax10
	clc
	adc     _g_levels
	sta     sreg
	txa
	adc     _g_levels+1
	sta     sreg+1
	ldx     #$00
	lda     M0002
	bpl     L002D
	dex
L002D:	stx     tmp1
	asl     a
	rol     tmp1
	clc
	adc     _g_vehicle_data
	sta     ptr1
	lda     tmp1
	adc     _g_vehicle_data+1
	sta     ptr1+1
	ldy     #$01
	lda     (ptr1),y
	tax
	lda     (ptr1)
	ldy     #$08
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
	jsr     decsp4
	lda     #<(M0002)
	ldy     #$02
	sta     (sp),y
	iny
	lda     #>(M0002)
	sta     (sp),y
	lda     #$01
	sta     (sp)
	tay
	dea
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	lda     M0003
	ldx     M0003+1
	jsr     mulax10
	clc
	adc     _g_levels
	sta     ptr1
	txa
	adc     _g_levels+1
	sta     ptr1+1
	lda     M0002
	ldy     #$04
	sta     (ptr1),y
	jsr     decsp4
	lda     #<(M0002)
	ldy     #$02
	sta     (sp),y
	iny
	lda     #>(M0002)
	sta     (sp),y
	lda     #$01
	sta     (sp)
	tay
	dea
	sta     (sp),y
	lda     M0006
	ldx     M0006+1
	jsr     _GetData
	lda     M0003
	ldx     M0003+1
	jsr     mulax10
	clc
	adc     _g_levels
	sta     ptr1
	txa
	adc     _g_levels+1
	sta     ptr1+1
	lda     M0002
	ldy     #$05
	sta     (ptr1),y
	lda     M0003
	ldx     M0003+1
	jsr     mulax10
	clc
	adc     _g_levels
	sta     ptr1
	txa
	adc     _g_levels+1
	sta     ptr1+1
	lda     M0003
	ldx     M0003+1
	ina
	bne     L0030
	inx
L0030:	cpx     M0004+1
	bne     L0037
	cmp     M0004
L0037:	jsr     booleq
	sta     (ptr1)
	inc     M0003
	jne     L0024
	inc     M0003+1
	jmp     L0024
L0031:	lda     M0006
	ldx     M0006+1
	jsr     _fclose
	bra     L003A
L0039:	lda     #$01
L0038:	jsr     _ExitProgram
L003A:	lda     M0001
	cmp     #$06
	jne     L0003
	jmp     incsp2

.segment	"BSS"

M0001:
	.res	1,$00
M0002:
	.res	1,$00
M0003:
	.res	2,$00
M0004:
	.res	2,$00
M0005:
	.res	2,$00
M0006:
	.res	2,$00

.endproc

; ---------------------------------------------------------------
; void __near__ InitSquares (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_InitSquares: near

.segment	"CODE"

	lda     #$73
	jsr     pusha0
	lda     #$02
	jsr     _calloc
	sta     _g_squares
	stx     _g_squares+1
	stz     M0002
	stz     M0002+1
	stz     M0001
	stz     M0001+1
	lda     #$01
	sta     M0003
	stz     M0003+1
L0002:	lda     M0002+1
	cmp     #$00
	bne     L0006
	lda     M0002
	cmp     #$73
L0006:	bcs     L0003
	lda     M0002
	ldx     M0002+1
	stx     tmp1
	asl     a
	rol     tmp1
	clc
	adc     _g_squares
	sta     ptr1
	lda     tmp1
	adc     _g_squares+1
	sta     ptr1+1
	lda     M0001
	sta     (ptr1)
	ldy     #$01
	lda     M0001+1
	sta     (ptr1),y
	lda     M0003
	clc
	adc     M0001
	sta     M0001
	lda     M0003+1
	adc     M0001+1
	sta     M0001+1
	lda     #$02
	clc
	adc     M0003
	sta     M0003
	bcc     L0008
	inc     M0003+1
L0008:	inc     M0002
	bne     L0002
	inc     M0002+1
	bra     L0002
L0003:	rts

.segment	"BSS"

M0001:
	.res	2,$00
M0002:
	.res	2,$00
M0003:
	.res	2,$00

.endproc

; ---------------------------------------------------------------
; void __near__ InitVehicles (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_InitVehicles: near

.segment	"CODE"

	lda     #$18
	jsr     pusha0
	lda     #$23
	jsr     _calloc
	sta     _g_vehicles
	stx     _g_vehicles+1
	stz     M0001
	stz     M0001+1
	lda     _g_vehicles+1
	sta     M0002+1
	lda     _g_vehicles
	sta     M0002
L0002:	lda     M0001
	cmp     #$04
	lda     M0001+1
	sbc     #$00
	bvc     L0006
	eor     #$80
L0006:	bmi     L000A
	rts
L000A:	lda     M0002+1
	sta     ptr1+1
	lda     M0002
	sta     ptr1
	lda     M0001
	ldy     #$0D
	sta     (ptr1),y
	iny
	lda     M0001+1
	sta     (ptr1),y
	lda     M0002+1
	sta     ptr1+1
	lda     M0002
	sta     ptr1
	lda     #$01
	sta     (ptr1)
	lda     M0002+1
	sta     ptr1+1
	lda     M0002
	sta     ptr1
	lda     M0001
	ldx     M0001+1
	jsr     aslax4
	jsr     aslax3
	clc
	adc     #$40
	bcc     L0009
	inx
L0009:	ldy     #$17
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
	lda     M0002
	ldx     M0002+1
	jsr     pushax
	lda     M0002+1
	sta     ptr1+1
	lda     M0002
	sta     ptr1
	ldy     #$18
	lda     (ptr1),y
	tax
	dey
	lda     (ptr1),y
	jsr     _Sin
	ldy     #$19
	jsr     staxspidx
	lda     M0002
	ldx     M0002+1
	jsr     pushax
	lda     M0002+1
	sta     ptr1+1
	lda     M0002
	sta     ptr1
	ldy     #$18
	lda     (ptr1),y
	tax
	dey
	lda     (ptr1),y
	jsr     _Cos
	ldy     #$1B
	jsr     staxspidx
	lda     M0002
	ldx     M0002+1
	jsr     pushax
	ldx     #$1F
	lda     #$FF
	jsr     pushax
	jsr     decsp4
	lda     #$FF
	ldy     #$02
	sta     (sp),y
	iny
	lda     #$0F
	sta     (sp),y
	lda     M0002+1
	sta     ptr1+1
	lda     M0002
	sta     ptr1
	ldy     #$1A
	lda     (ptr1),y
	tax
	dey
	lda     (ptr1),y
	sta     (sp)
	ldy     #$01
	txa
	sta     (sp),y
	ldx     #$10
	lda     #$00
	jsr     _MultiplyThenDivide
	jsr     tossubax
	ldy     #$11
	jsr     staxspidx
	lda     M0002
	ldx     M0002+1
	jsr     pushax
	ldx     #$1F
	lda     #$FF
	jsr     pushax
	jsr     decsp4
	lda     #$FF
	ldy     #$02
	sta     (sp),y
	iny
	lda     #$0F
	sta     (sp),y
	lda     M0002+1
	sta     ptr1+1
	lda     M0002
	sta     ptr1
	ldy     #$1C
	lda     (ptr1),y
	tax
	dey
	lda     (ptr1),y
	sta     (sp)
	ldy     #$01
	txa
	sta     (sp),y
	ldx     #$10
	lda     #$00
	jsr     _MultiplyThenDivide
	jsr     tossubax
	ldy     #$13
	jsr     staxspidx
	lda     M0002+1
	sta     ptr1+1
	lda     M0002
	sta     ptr1
	lda     #$00
	ldy     #$15
	sta     (ptr1),y
	iny
	sta     (ptr1),y
	lda     M0002+1
	sta     ptr1+1
	lda     M0002
	sta     ptr1
	lda     #$1E
	ldy     #$0C
	sta     (ptr1),y
	lda     M0002+1
	sta     ptr1+1
	lda     M0002
	sta     ptr1
	lda     #$FF
	ldy     #$06
	sta     (ptr1),y
	lda     M0002
	ldx     M0002+1
	jsr     _AdvancePlayer
	inc     M0001
	bne     L0007
	inc     M0001+1
L0007:	lda     #$23
	clc
	adc     M0002
	sta     M0002
	jcc     L0002
	inc     M0002+1
	jmp     L0002

.segment	"BSS"

M0001:
	.res	2,$00
M0002:
	.res	2,$00

.endproc

; ---------------------------------------------------------------
; void __near__ GetData (void *buffer, unsigned int size, struct _FILE *ifile)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_GetData: near

.segment	"CODE"

	jsr     pushax
	jsr     decsp6
	ldy     #$0B
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	ldy     #$04
	sta     (sp),y
	iny
	txa
	sta     (sp),y
	ldy     #$09
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	ldy     #$02
	sta     (sp),y
	iny
	txa
	sta     (sp),y
	lda     #$01
	sta     (sp)
	tay
	dea
	sta     (sp),y
	ldy     #$07
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	jsr     _fread
	stx     tmp1
	ora     tmp1
	bne     L0002
	tax
	ina
	jsr     _ExitProgram
L0002:	jmp     incsp6

.endproc
