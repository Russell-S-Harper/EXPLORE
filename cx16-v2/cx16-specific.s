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
	.import		_clock
	.import		_vpeek
	.import		_vpoke
	.import		_kbhit
	.import		_cgetc
	.import		_tgi_static_stddrv
	.import		_tgi_install
	.import		_tgi_init
	.import		_tgi_done
	.import		_tgi_geterror
	.import		_tgi_clear
	.import		_joy_static_stddrv
	.import		_joy_install
	.import		_joy_read
	.import		_abs
	.import		_ExitProgram
	.export		_UpdateDisplay
	.export		_InitSpecific
	.export		_MultiplyThenDivide
	.export		_DrawLineFromTo16
	.export		_DrawLineJustTo16
	.export		_PlotPoint16
	.export		_ErasePoint16
	.export		_GetPlayerInput
	.export		_AddSound
	.export		_Minimum
	.export		_Maximum
	.import		_g_vehicles
	.import		_g_exit_program
	.import		_g_display_width
	.import		_g_display_height
	.import		_g_frame_counter
	.import		_g_vehicle_index

.segment	"BSS"

_f_current_color:
	.res	1,$00
_f_x_from_point:
	.res	2,$00
_f_y_from_point:
	.res	2,$00
_f_x_to_point:
	.res	2,$00
_f_y_to_point:
	.res	2,$00

; ---------------------------------------------------------------
; void __near__ UpdateDisplay (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_UpdateDisplay: near

.segment	"CODE"

	lda     regbank+4
	ldx     regbank+5
	jsr     pushax
	lda     M0001+3
	sta     sreg+1
	lda     M0001+2
	sta     sreg
	ldx     M0001+1
	lda     M0001
	jsr     bnegeax
	beq     L0002
	jsr     _clock
	jmp     L002B
L0002:	lda     #$00
	jsr     _DefaultCallback
	jsr     _clock
	sta     M0006
	stx     M0006+1
	ldy     sreg
	sty     M0006+2
	ldy     sreg+1
	sty     M0006+3
	lda     M0006+3
	sta     sreg+1
	lda     M0006+2
	sta     sreg
	ldx     M0006+1
	lda     M0006
	jsr     pusheax
	lda     M0001+3
	sta     sreg+1
	lda     M0001+2
	sta     sreg
	ldx     M0001+1
	lda     M0001
	jsr     tosulteax
	bne     L0005
	lda     M0006+3
	sta     sreg+1
	lda     M0006+2
	sta     sreg
	ldx     M0006+1
	lda     M0006
	jsr     pusheax
	lda     M0001+3
	sta     sreg+1
	lda     M0001+2
	sta     sreg
	ldx     M0001+1
	lda     M0001
	jsr     tossubeax
	cmp     #$0A
	txa
	sbc     #$00
	lda     sreg
	sbc     #$00
	lda     sreg+1
	sbc     #$00
	bcc     L0002
L0005:	lda     M0006+3
	sta     sreg+1
	lda     M0006+2
	sta     sreg
	ldx     M0006+1
	lda     M0006
L002B:	sta     M0001
	stx     M0001+1
	ldy     sreg
	sty     M0001+2
	ldy     sreg+1
	sty     M0001+3
L001E:	lda     #$01
	jsr     _DefaultCallback
	lda     $9F26
	ldx     #$00
	and     #$40
	beq     L001F
	inx
L001F:	clc
	lda     $9F28
	bcc     L001B
	inx
L001B:	cmp     #$DF
	txa
	sbc     #$01
	bvc     L000D
	eor     #$80
L000D:	bmi     L001E
	ldx     #$00
	lda     $9F2F
	beq     L0020
	cmp     #$4C
	beq     L0021
	bra     L0022
L0020:	lda     #$4C
	sta     M0002
	stz     M0003
	bra     L0023
L0021:	stz     M0002
	lda     #$98
	sta     M0003
	bra     L0023
L0022:	lda     #$05
	jsr     _ExitProgram
L0023:	lda     M0002
	sta     $9F2F
	stz     $9F20
	lda     M0003
	sta     $9F21
	lda     #$10
	sta     $9F22
	ldx     #$9F
	lda     #$23
	sta     regbank+4
	stx     regbank+4+1
	stz     M0004
L0024:	lda     M0004
	cmp     #$F0
	bcs     L0014
	stz     M0005
L0025:	lda     M0005
	cmp     #$14
	bcs     L0026
	lda     #$00
	sta     (regbank+4)
	sta     (regbank+4)
	sta     (regbank+4)
	sta     (regbank+4)
	sta     (regbank+4)
	sta     (regbank+4)
	sta     (regbank+4)
	sta     (regbank+4)
	inc     M0005
	bra     L0025
L0026:	inc     M0004
	bra     L0024
L0014:	lda     (sp)
	sta     regbank+4
	ldy     #$01
	lda     (sp),y
	sta     regbank+5
	jmp     incsp2

.segment	"BSS"

M0001:
	.res	4,$00
M0002:
	.res	1,$00
M0003:
	.res	1,$00
M0004:
	.res	1,$00
M0005:
	.res	1,$00
M0006:
	.res	4,$00

.endproc

; ---------------------------------------------------------------
; void __near__ InitSpecific (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_InitSpecific: near

.segment	"CODE"

	lda     #<(_tgi_static_stddrv)
	ldx     #>(_tgi_static_stddrv)
	jsr     _tgi_install
	jsr     _tgi_geterror
	cmp     #$00
	beq     L0002
	ldx     #$00
	lda     #$03
	jsr     _ExitProgram
L0002:	jsr     _tgi_init
	jsr     _tgi_clear
	ldx     #$01
	lda     #$40
	sta     _g_display_width
	stx     _g_display_width+1
	lda     #$F0
	sta     _g_display_height
	stz     _g_display_height+1
	lda     #$06
	sta     $9F2D
	lda     #$4C
	sta     $9F2F
	jsr     _UpdateDisplay
	lda     #<(_joy_static_stddrv)
	ldx     #>(_joy_static_stddrv)
	jmp     _joy_install

.endproc

; ---------------------------------------------------------------
; int __near__ MultiplyThenDivide (int num1, int num2, int denom)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_MultiplyThenDivide: near

.segment	"CODE"

	jsr     pushax
	lda     sp
	ldx     sp+1
	clc
	adc     #$04
	bcc     L0002
	inx
L0002:	sta     M0001
	stx     M0001+1
	lda     sp
	ldx     sp+1
	clc
	adc     #$02
	bcc     L0003
	inx
L0003:	sta     M0002
	stx     M0002+1
	lda     #>(M0004)
	sta     M0003+1
	lda     #<(M0004)
	sta     M0003
	lda     #$04
	sta     $9F25
	lda     #$10
	sta     $9F2C
	lda     #$0C
	sta     $9F25
	lda     M0001+1
	sta     ptr1+1
	lda     M0001
	sta     ptr1
	lda     (ptr1)
	sta     $9F29
	lda     M0001
	ldx     M0001+1
	ina
	bne     L0004
	inx
L0004:	sta     ptr1
	stx     ptr1+1
	lda     (ptr1)
	sta     $9F2A
	lda     M0002+1
	sta     ptr1+1
	lda     M0002
	sta     ptr1
	lda     (ptr1)
	sta     $9F2B
	lda     M0002
	ldx     M0002+1
	ina
	bne     L0005
	inx
L0005:	sta     ptr1
	stx     ptr1+1
	lda     (ptr1)
	sta     $9F2C
	lda     #$04
	sta     $9F25
	lda     #$40
	sta     $9F29
	stz     $9F20
	lda     #$96
	sta     $9F21
	stz     $9F22
	stz     $9F23
	lda     #$10
	sta     $9F22
	lda     M0003+1
	sta     ptr1+1
	lda     M0003
	sta     ptr1
	lda     $9F23
	sta     (ptr1)
	lda     M0003
	ldx     M0003+1
	ina
	bne     L0006
	inx
L0006:	sta     ptr1
	stx     ptr1+1
	lda     $9F23
	sta     (ptr1)
	lda     M0003+1
	sta     ptr1+1
	lda     M0003
	sta     ptr1
	ldy     #$02
	lda     $9F23
	sta     (ptr1),y
	lda     M0003+1
	sta     ptr1+1
	lda     M0003
	sta     ptr1
	iny
	lda     $9F23
	sta     (ptr1),y
	stz     $9F29
	lda     M0004+3
	sta     sreg+1
	lda     M0004+2
	sta     sreg
	ldx     M0004+1
	lda     M0004
	jsr     pusheax
	ldy     #$05
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	jsr     axlong
	jsr     tosdiveax
	jmp     incsp6

.segment	"BSS"

M0001:
	.res	2,$00
M0002:
	.res	2,$00
M0003:
	.res	2,$00
M0004:
	.res	4,$00

.endproc

; ---------------------------------------------------------------
; void __near__ DrawLineFromTo16 (int x1, int y1, int x2, int y2, unsigned char color)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_DrawLineFromTo16: near

.segment	"CODE"

	jsr     pusha
	ldy     #$08
	lda     (sp),y
	sta     _f_x_from_point+1
	dey
	lda     (sp),y
	sta     _f_x_from_point
	dey
	lda     (sp),y
	sta     _f_y_from_point+1
	dey
	lda     (sp),y
	sta     _f_y_from_point
	dey
	lda     (sp),y
	sta     _f_x_to_point+1
	dey
	lda     (sp),y
	sta     _f_x_to_point
	dey
	lda     (sp),y
	sta     _f_y_to_point+1
	dey
	lda     (sp),y
	sta     _f_y_to_point
	lda     (sp)
	sta     _f_current_color
	jsr     _DrawLine16
	ldy     #$09
	jmp     addysp

.endproc

; ---------------------------------------------------------------
; void __near__ DrawLineJustTo16 (int x, int y, unsigned char color)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_DrawLineJustTo16: near

.segment	"CODE"

	jsr     pusha
	lda     _f_x_to_point+1
	sta     _f_x_from_point+1
	lda     _f_x_to_point
	sta     _f_x_from_point
	lda     _f_y_to_point+1
	sta     _f_y_from_point+1
	lda     _f_y_to_point
	sta     _f_y_from_point
	ldy     #$04
	lda     (sp),y
	sta     _f_x_to_point+1
	dey
	lda     (sp),y
	sta     _f_x_to_point
	dey
	lda     (sp),y
	sta     _f_y_to_point+1
	dey
	lda     (sp),y
	sta     _f_y_to_point
	lda     (sp)
	sta     _f_current_color
	jsr     _DrawLine16
	jmp     incsp5

.endproc

; ---------------------------------------------------------------
; void __near__ PlotPoint16 (int x, int y, unsigned char color)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_PlotPoint16: near

.segment	"CODE"

	jsr     pusha
	ldy     #$02
	lda     (sp),y
	tax
	dey
	txa
	jmi     L000B
	iny
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	cmp     #$F0
	txa
	sbc     #$00
	bvc     L0004
	eor     #$80
L0004:	jpl     L000B
	ldy     #$04
	lda     (sp),y
	tax
	dey
	txa
	jmi     L000B
	iny
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	cmp     #$40
	txa
	sbc     #$01
	bvc     L0005
	eor     #$80
L0005:	jpl     L000B
	dey
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	jsr     axlong
	jsr     shleax4
	jsr     shleax3
	jsr     pusheax
	ldy     #$06
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	jsr     aslax4
	stx     tmp1
	asl     a
	rol     tmp1
	ldx     tmp1
	jsr     axlong
	jsr     tosaddeax
	jsr     pusheax
	ldy     #$08
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	jsr     asrax1
	jsr     axlong
	jsr     tosaddeax
	jsr     pusheax
	lda     $9F2F
	beq     L0008
	ldx     #$00
	txa
	bra     L0009
L0008:	tax
	lda     #$98
L0009:	jsr     axlong
	ldy     sreg
	sty     sreg+1
	stz     sreg
	tax
	lda     #$00
	jsr     tosaddeax
	sta     M0001
	stx     M0001+1
	ldy     sreg
	sty     M0001+2
	ldy     sreg+1
	sty     M0001+3
	lda     M0001
	sta     $9F20
	lda     M0001+3
	sta     sreg+1
	lda     M0001+2
	sta     sreg
	ldx     M0001+1
	stx     M0001
	ldx     sreg
	ldy     sreg+1
	sty     sreg
	stz     sreg+1
	stx     M0001+1
	ldy     sreg
	sty     M0001+2
	ldy     sreg+1
	sty     M0001+3
	lda     M0001
	sta     $9F21
	lda     M0001+3
	sta     sreg+1
	lda     M0001+2
	sta     sreg
	ldx     M0001+1
	stx     M0001
	ldx     sreg
	ldy     sreg+1
	sty     sreg
	stz     sreg+1
	stx     M0001+1
	ldy     sreg
	sty     M0001+2
	ldy     sreg+1
	sty     M0001+3
	lda     M0001
	sta     $9F22
	ldy     #$03
	lda     (sp),y
	and     #$01
	beq     L0010
	lda     #$0F
	trb     $9F23
	lda     (sp)
	and     #$0F
	bra     L0013
L0010:	lda     #$F0
	trb     $9F23
	lda     (sp)
	asl     a
	asl     a
	asl     a
	asl     a
L0013:	ora     $9F23
	sta     $9F23
L000B:	ldy     #$04
	lda     (sp),y
	sta     _f_x_to_point+1
	dey
	lda     (sp),y
	sta     _f_x_to_point
	dey
	lda     (sp),y
	sta     _f_y_to_point+1
	dey
	lda     (sp),y
	sta     _f_y_to_point
	jmp     incsp5

.segment	"BSS"

M0001:
	.res	4,$00

.endproc

; ---------------------------------------------------------------
; void __near__ ErasePoint16 (int x, int y)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_ErasePoint16: near

.segment	"CODE"

	jsr     pushax
	ldy     #$01
	lda     (sp),y
	jmi     L000B
	ldy     #$01
	lda     (sp),y
	tax
	lda     (sp)
	cmp     #$F0
	txa
	sbc     #$00
	bvc     L0004
	eor     #$80
L0004:	jpl     L000B
	ldy     #$03
	lda     (sp),y
	tax
	dey
	txa
	jmi     L000B
	iny
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	cmp     #$40
	txa
	sbc     #$01
	bvc     L0005
	eor     #$80
L0005:	jpl     L000B
	dey
	lda     (sp),y
	tax
	lda     (sp)
	jsr     axlong
	jsr     shleax4
	jsr     shleax3
	jsr     pusheax
	ldy     #$05
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	jsr     aslax4
	stx     tmp1
	asl     a
	rol     tmp1
	ldx     tmp1
	jsr     axlong
	jsr     tosaddeax
	jsr     pusheax
	ldy     #$07
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	jsr     asrax1
	jsr     axlong
	jsr     tosaddeax
	jsr     pusheax
	lda     $9F2F
	beq     L0008
	ldx     #$00
	txa
	bra     L0009
L0008:	tax
	lda     #$98
L0009:	jsr     axlong
	ldy     sreg
	sty     sreg+1
	stz     sreg
	tax
	lda     #$00
	jsr     tosaddeax
	sta     M0001
	stx     M0001+1
	ldy     sreg
	sty     M0001+2
	ldy     sreg+1
	sty     M0001+3
	lda     M0001
	sta     $9F20
	lda     M0001+3
	sta     sreg+1
	lda     M0001+2
	sta     sreg
	ldx     M0001+1
	stx     M0001
	ldx     sreg
	ldy     sreg+1
	sty     sreg
	stz     sreg+1
	stx     M0001+1
	ldy     sreg
	sty     M0001+2
	ldy     sreg+1
	sty     M0001+3
	lda     M0001
	sta     $9F21
	lda     M0001+3
	sta     sreg+1
	lda     M0001+2
	sta     sreg
	ldx     M0001+1
	stx     M0001
	ldx     sreg
	ldy     sreg+1
	sty     sreg
	stz     sreg+1
	stx     M0001+1
	ldy     sreg
	sty     M0001+2
	ldy     sreg+1
	sty     M0001+3
	lda     M0001
	sta     $9F22
	ldy     #$02
	lda     (sp),y
	and     #$01
	beq     L0010
	lda     $9F23
	and     #$F0
	bra     L000C
L0010:	lda     $9F23
	and     #$0F
L000C:	sta     $9F23
L000B:	jmp     incsp4

.segment	"BSS"

M0001:
	.res	4,$00

.endproc

; ---------------------------------------------------------------
; void __near__ GetPlayerInput (struct $anon-struct-0021 *vehicle)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_GetPlayerInput: near

.segment	"CODE"

	jsr     pushax
	jmp     L0006
L0002:	jsr     _cgetc
	cmp     #$11
	beq     L000E
	cmp     #$1B
	beq     L0008
	cmp     #$1D
	jeq     L0011
	cmp     #$20
	jeq     L0013
	cmp     #$31
	jeq     L0015
	cmp     #$51
	jeq     L001D
	cmp     #$91
	beq     L000B
	cmp     #$9D
	jeq     L0012
	jmp     L0006
L0008:	jsr     _cgetc
	cmp     #$1B
	bne     L0008
	jmp     L0006
L000B:	ldy     #$01
	lda     (sp),y
	sta     ptr1+1
	lda     (sp)
	sta     ptr1
	ldy     #$01
	lda     (ptr1),y
	beq     L002D
	lda     (sp),y
	tax
	lda     (sp)
	sta     sreg
	stx     sreg+1
	ldy     #$04
	jsr     ldaidx
	ina
	sta     (sreg),y
	jmp     L0006
L002D:	lda     (sp),y
	tax
	lda     (sp)
	sta     sreg
	stx     sreg+1
	ldy     #$07
	jsr     ldaidx
	ina
	sta     (sreg),y
	jmp     L0006
L000E:	ldy     #$01
	lda     (sp),y
	sta     ptr1+1
	lda     (sp)
	sta     ptr1
	ldy     #$01
	lda     (ptr1),y
	beq     L002E
	lda     (sp),y
	tax
	lda     (sp)
	sta     sreg
	stx     sreg+1
	ldy     #$04
	jsr     ldaidx
	dea
	sta     (sreg),y
	jmp     L0006
L002E:	lda     (sp),y
	tax
	lda     (sp)
	sta     sreg
	stx     sreg+1
	ldy     #$07
	jsr     ldaidx
	dea
	sta     (sreg),y
	jmp     L0006
L0011:	ldy     #$01
	lda     (sp),y
	tax
	lda     (sp)
	sta     sreg
	stx     sreg+1
	ldy     #$05
	jsr     ldaidx
	ina
	sta     (sreg),y
	jmp     L0006
L0012:	ldy     #$01
	lda     (sp),y
	tax
	lda     (sp)
	sta     sreg
	stx     sreg+1
	ldy     #$05
	jsr     ldaidx
	dea
	sta     (sreg),y
	jmp     L0006
L0013:	ldy     #$01
	lda     (sp),y
	tax
	lda     (sp)
	ldy     #$0B
	jsr     ldaidx
	tax
	jne     L0006
	ldy     #$01
	lda     (sp),y
	sta     ptr1+1
	lda     (sp)
	sta     ptr1
	tya
	ldy     #$03
	sta     (ptr1),y
	bra     L0006
L0015:	lda     #$01
	sta     M0002
	stz     M0002+1
L0016:	lda     M0002
	cmp     #$04
	lda     M0002+1
	sbc     #$00
	bvc     L001A
	eor     #$80
L001A:	bpl     L0006
	lda     _g_vehicle_index
	clc
	adc     M0002
	pha
	lda     _g_vehicle_index+1
	adc     M0002+1
	tax
	pla
	jsr     pushax
	ldx     #$00
	lda     #$04
	jsr     tosmoda0
	sta     M0003
	stx     M0003+1
	lda     _g_vehicles
	ldx     _g_vehicles+1
	jsr     pushax
	lda     M0003
	ldx     M0003+1
	jsr     pushax
	lda     #$23
	jsr     tosmula0
	jsr     tosaddax
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1)
	beq     L0018
	lda     M0003+1
	sta     _g_vehicle_index+1
	lda     M0003
	sta     _g_vehicle_index
	bra     L0006
L0018:	inc     M0002
	bne     L0016
	inc     M0002+1
	bra     L0016
L001D:	jsr     _StopSounds
	jsr     _tgi_done
	lda     #$01
	sta     _g_exit_program
L0006:	jsr     _kbhit
	tax
	jne     L0002
	jsr     _joy_read
	sta     M0001
	tax
	jeq     L002A
	lda     M0001
	and     #$08
	beq     L0030
	ldy     #$01
	lda     (sp),y
	sta     ptr1+1
	lda     (sp)
	sta     ptr1
	ldy     #$01
	lda     (ptr1),y
	beq     L002F
	lda     (sp),y
	tax
	lda     (sp)
	sta     sreg
	stx     sreg+1
	ldy     #$04
	bra     L0036
L002F:	lda     (sp),y
	tax
	lda     (sp)
	sta     sreg
	stx     sreg+1
	ldy     #$07
	jsr     ldaidx
	ina
	bra     L002B
L0030:	lda     M0001
	and     #$04
	beq     L0032
	ldy     #$01
	lda     (sp),y
	sta     ptr1+1
	lda     (sp)
	sta     ptr1
	ldy     #$01
	lda     (ptr1),y
	beq     L0031
	lda     (sp),y
	tax
	lda     (sp)
	sta     sreg
	stx     sreg+1
	ldy     #$04
	jsr     ldaidx
	ina
	bra     L002B
L0031:	lda     (sp),y
	tax
	lda     (sp)
	sta     sreg
	stx     sreg+1
	ldy     #$07
L0036:	jsr     ldaidx
	dea
L002B:	sta     (sreg),y
L0032:	lda     M0001
	and     #$01
	beq     L0033
	ldy     #$01
	lda     (sp),y
	tax
	lda     (sp)
	sta     sreg
	stx     sreg+1
	ldy     #$05
	jsr     ldaidx
	ina
	bra     L002C
L0033:	lda     M0001
	and     #$02
	beq     L0034
	ldy     #$01
	lda     (sp),y
	tax
	lda     (sp)
	sta     sreg
	stx     sreg+1
	ldy     #$05
	jsr     ldaidx
	dea
L002C:	sta     (sreg),y
L0034:	lda     M0001
	and     #$80
	beq     L002A
	ldy     #$01
	lda     (sp),y
	tax
	lda     (sp)
	ldy     #$0B
	jsr     ldaidx
	tax
	bne     L002A
	ldy     #$01
	lda     (sp),y
	sta     ptr1+1
	lda     (sp)
	sta     ptr1
	tya
	ldy     #$03
	sta     (ptr1),y
L002A:	jmp     incsp2

.segment	"BSS"

M0001:
	.res	1,$00
M0002:
	.res	2,$00
M0003:
	.res	2,$00

.endproc

; ---------------------------------------------------------------
; void __near__ AddSound (signed char type)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_AddSound: near

.segment	"CODE"

	jsr     pusha
	ldx     #$00
	lda     M0001
	jsr     aslax2
	jsr     axlong
	jsr     pusheax
	ldx     #$F9
	lda     #$01
	sta     sreg
	stz     sreg+1
	lda     #$C0
	jsr     tosaddeax
	sta     M0002
	stx     M0002+1
	ldy     sreg
	sty     M0002+2
	ldy     sreg+1
	sty     M0002+3
	inc     M0001
	lda     M0001
	cmp     #$10
	bcc     L0002
	stz     M0001
L0002:	lda     (sp)
	beq     L0006
	cmp     #$01
	beq     L0007
	jmp     incsp1
L0006:	lda     #$6B
	jsr     pusha
	lda     M0002+3
	sta     sreg+1
	lda     M0002+2
	sta     sreg
	ldx     M0002+1
	lda     M0002
	jsr     _vpoke
	lda     #$11
	bra     L001B
L0007:	lda     #$BA
	jsr     pusha
	lda     M0002+3
	sta     sreg+1
	lda     M0002+2
	sta     sreg
	ldx     M0002+1
	lda     M0002
	jsr     _vpoke
	lda     #$01
L001B:	jsr     pusha
	ldy     #<(M0002)
	sty     ptr1
	ldy     #>(M0002)
	jsr     laddeq1
	jsr     _vpoke
	lda     #$FF
	jsr     pusha
	ldy     #<(M0002)
	sty     ptr1
	ldy     #>(M0002)
	jsr     laddeq1
	jsr     _vpoke
	lda     #$C0
	jsr     pusha
	ldy     #<(M0002)
	sty     ptr1
	ldy     #>(M0002)
	jsr     laddeq1
	jsr     _vpoke
	jmp     incsp1

.segment	"BSS"

M0001:
	.res	1,$00
M0002:
	.res	4,$00

.endproc

; ---------------------------------------------------------------
; int __near__ Minimum (int a, int b)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_Minimum: near

.segment	"CODE"

	jsr     pushax
	ldy     #$03
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	cmp     (sp)
	txa
	dey
	sbc     (sp),y
	bvc     L0004
	eor     #$80
L0004:	bpl     L0005
	ldy     #$03
L0005:	lda     (sp),y
	tax
	dey
	lda     (sp),y
	jmp     incsp4

.endproc

; ---------------------------------------------------------------
; int __near__ Maximum (int a, int b)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_Maximum: near

.segment	"CODE"

	jsr     pushax
	ldy     #$05
	jsr     pushwysp
	ldy     #$03
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	jsr     tosicmp
	bmi     L0002
	beq     L0002
	ldy     #$03
	bra     L0004
L0002:	ldy     #$01
L0004:	lda     (sp),y
	tax
	dey
	lda     (sp),y
	jmp     incsp4

.endproc

; ---------------------------------------------------------------
; void __near__ DrawLine16 (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_DrawLine16: near

.segment	"CODE"

	lda     regbank+4
	ldx     regbank+5
	jsr     pushax
	lda     regbank+3
	jsr     pusha
	lda     regbank+1
	ldx     regbank+2
	jsr     pushax
	lda     _f_x_from_point+1
	sta     M0003+1
	lda     _f_x_from_point
	sta     M0003
	lda     _f_y_from_point+1
	sta     M0004+1
	lda     _f_y_from_point
	sta     M0004
	lda     _f_x_to_point+1
	sta     M0005+1
	lda     _f_x_to_point
	sta     M0005
	lda     _f_y_to_point+1
	sta     M0006+1
	lda     _f_y_to_point
	sta     M0006
	lda     M0004
	ldx     M0004+1
	jsr     pushax
	lda     M0006
	ldx     M0006+1
	jsr     _Maximum
	sta     M000A
	stx     M000A+1
	lda     M0004
	ldx     M0004+1
	jsr     pushax
	lda     M0006
	ldx     M0006+1
	jsr     _Minimum
	sta     M0009
	stx     M0009+1
	lda     M0003
	ldx     M0003+1
	jsr     pushax
	lda     M0005
	ldx     M0005+1
	jsr     _Maximum
	sta     M0008
	stx     M0008+1
	lda     M0003
	ldx     M0003+1
	jsr     pushax
	lda     M0005
	ldx     M0005+1
	jsr     _Minimum
	sta     M0007
	stx     M0007+1
	lda     M000A
	cmp     #$F0
	lda     M000A+1
	sbc     #$00
	bvs     L0003
	eor     #$80
L0003:	bmi     L0051
	ldx     M0009+1
	cpx     #$80
	bcs     L0051
	lda     M0008
	cmp     #$40
	lda     M0008+1
	sbc     #$01
	bvs     L0005
	eor     #$80
L0005:	bmi     L0051
	ldx     M0007+1
	cpx     #$80
	jcc     L0026
L0051:	ldx     M000A+1
	cpx     #$80
	jcs     L0001
	lda     M0009
	cmp     #$F0
	lda     M0009+1
	sbc     #$00
	bvs     L0009
	eor     #$80
L0009:	jmi     L0001
	ldx     M0008+1
	cpx     #$80
	jcs     L0001
	lda     M0007
	cmp     #$40
	lda     M0007+1
	sbc     #$01
	bvs     L000A
	eor     #$80
L000A:	jmi     L0001
	lda     M0004
	ldx     M0004+1
	jsr     pushax
	lda     M0006
	ldx     M0006+1
	jsr     _Maximum
	cmp     #$F0
	txa
	sbc     #$00
	bvs     L000D
	eor     #$80
L000D:	jpl     L0012
	lda     M0004
	ldx     M0004+1
	cpx     M0006+1
	bne     L000F
	cmp     M0006
	jeq     L0012
L000F:	jsr     decsp4
	lda     #$EF
	sec
	sbc     M0004
	pha
	lda     #$00
	sbc     M0004+1
	tax
	pla
	ldy     #$02
	sta     (sp),y
	iny
	txa
	sta     (sp),y
	lda     M0005
	sec
	sbc     M0003
	pha
	lda     M0005+1
	sbc     M0003+1
	tax
	pla
	sta     (sp)
	ldy     #$01
	txa
	sta     (sp),y
	lda     M0006
	sec
	sbc     M0004
	pha
	lda     M0006+1
	sbc     M0004+1
	tax
	pla
	jsr     _MultiplyThenDivide
	clc
	adc     M0003
	sta     M000B
	txa
	adc     M0003+1
	sta     M000B+1
	lda     M0006
	ldx     M0006+1
	jsr     pushax
	lda     M0004
	ldx     M0004+1
	jsr     tosicmp
	bmi     L0011
	beq     L0011
	lda     #$EF
	sta     M0006
	stz     M0006+1
	lda     M000B+1
	sta     M0005+1
	lda     M000B
	sta     M0005
	bra     L0012
L0011:	lda     #$EF
	sta     M0004
	stz     M0004+1
	lda     M000B+1
	sta     M0003+1
	lda     M000B
	sta     M0003
L0012:	lda     M0004
	ldx     M0004+1
	jsr     pushax
	lda     M0006
	ldx     M0006+1
	jsr     _Minimum
	cpx     #$80
	jcc     L0018
	lda     M0004
	ldx     M0004+1
	cpx     M0006+1
	bne     L0015
	cmp     M0006
	jeq     L0018
L0015:	jsr     decsp4
	lda     M0004
	ldx     M0004+1
	jsr     negax
	ldy     #$02
	sta     (sp),y
	iny
	txa
	sta     (sp),y
	lda     M0005
	sec
	sbc     M0003
	pha
	lda     M0005+1
	sbc     M0003+1
	tax
	pla
	sta     (sp)
	ldy     #$01
	txa
	sta     (sp),y
	lda     M0006
	sec
	sbc     M0004
	pha
	lda     M0006+1
	sbc     M0004+1
	tax
	pla
	jsr     _MultiplyThenDivide
	clc
	adc     M0003
	sta     M000B
	txa
	adc     M0003+1
	sta     M000B+1
	lda     M0006
	cmp     M0004
	lda     M0006+1
	sbc     M0004+1
	bvc     L004B
	eor     #$80
L004B:	asl     a
	bcc     L0058
	stz     M0006
	stz     M0006+1
	lda     M000B+1
	sta     M0005+1
	lda     M000B
	sta     M0005
	bra     L0018
L0058:	stz     M0004
	stz     M0004+1
	lda     M000B+1
	sta     M0003+1
	lda     M000B
	sta     M0003
L0018:	lda     M0003
	ldx     M0003+1
	jsr     pushax
	lda     M0005
	ldx     M0005+1
	jsr     _Maximum
	cmp     #$40
	txa
	sbc     #$01
	bvs     L001A
	eor     #$80
L001A:	jpl     L001F
	lda     M0003
	ldx     M0003+1
	cpx     M0005+1
	bne     L001C
	cmp     M0005
	jeq     L001F
L001C:	jsr     decsp4
	lda     #$3F
	sec
	sbc     M0003
	pha
	lda     #$01
	sbc     M0003+1
	tax
	pla
	ldy     #$02
	sta     (sp),y
	iny
	txa
	sta     (sp),y
	lda     M0006
	sec
	sbc     M0004
	pha
	lda     M0006+1
	sbc     M0004+1
	tax
	pla
	sta     (sp)
	ldy     #$01
	txa
	sta     (sp),y
	lda     M0005
	sec
	sbc     M0003
	pha
	lda     M0005+1
	sbc     M0003+1
	tax
	pla
	jsr     _MultiplyThenDivide
	clc
	adc     M0004
	sta     M000C
	txa
	adc     M0004+1
	sta     M000C+1
	lda     M0005
	ldx     M0005+1
	jsr     pushax
	lda     M0003
	ldx     M0003+1
	jsr     tosicmp
	bmi     L001E
	beq     L001E
	ldx     #$01
	lda     #$3F
	sta     M0005
	stx     M0005+1
	lda     M000C+1
	sta     M0006+1
	lda     M000C
	sta     M0006
	bra     L001F
L001E:	ldx     #$01
	lda     #$3F
	sta     M0003
	stx     M0003+1
	lda     M000C+1
	sta     M0004+1
	lda     M000C
	sta     M0004
L001F:	lda     M0003
	ldx     M0003+1
	jsr     pushax
	lda     M0005
	ldx     M0005+1
	jsr     _Minimum
	cpx     #$80
	jcc     L0025
	lda     M0003
	ldx     M0003+1
	cpx     M0005+1
	bne     L0022
	cmp     M0005
	jeq     L0025
L0022:	jsr     decsp4
	lda     M0003
	ldx     M0003+1
	jsr     negax
	ldy     #$02
	sta     (sp),y
	iny
	txa
	sta     (sp),y
	lda     M0006
	sec
	sbc     M0004
	pha
	lda     M0006+1
	sbc     M0004+1
	tax
	pla
	sta     (sp)
	ldy     #$01
	txa
	sta     (sp),y
	lda     M0005
	sec
	sbc     M0003
	pha
	lda     M0005+1
	sbc     M0003+1
	tax
	pla
	jsr     _MultiplyThenDivide
	clc
	adc     M0004
	sta     M000C
	txa
	adc     M0004+1
	sta     M000C+1
	lda     M0005
	cmp     M0003
	lda     M0005+1
	sbc     M0003+1
	bvc     L004E
	eor     #$80
L004E:	asl     a
	bcc     L005E
	stz     M0005
	stz     M0005+1
	lda     M000C+1
	sta     M0006+1
	lda     M000C
	sta     M0006
	bra     L0025
L005E:	stz     M0003
	stz     M0003+1
	lda     M000C+1
	sta     M0004+1
	lda     M000C
	sta     M0004
L0025:	ldx     M0004+1
	cpx     #$80
	jcs     L0001
	ldx     M0006+1
	cpx     #$80
	jcs     L0001
	lda     M0004
	cmp     #$F0
	lda     M0004+1
	sbc     #$00
	bvs     L0028
	eor     #$80
L0028:	jmi     L0001
	lda     M0006
	cmp     #$F0
	lda     M0006+1
	sbc     #$00
	bvs     L0029
	eor     #$80
L0029:	jmi     L0001
	ldx     M0003+1
	cpx     #$80
	jcs     L0001
	ldx     M0005+1
	cpx     #$80
	jcs     L0001
	lda     M0003
	cmp     #$40
	lda     M0003+1
	sbc     #$01
	bvs     L002A
	eor     #$80
L002A:	jmi     L0001
	lda     M0005
	cmp     #$40
	lda     M0005+1
	sbc     #$01
	bvs     L002B
	eor     #$80
L002B:	jmi     L0001
L0026:	lda     M0003
	cmp     M0005
	lda     M0003+1
	sbc     M0005+1
	bvc     L004F
	eor     #$80
L004F:	bpl     L002D
	lda     M0003+1
	sta     M000B+1
	lda     M0003
	sta     M000B
	lda     M0005+1
	sta     M0003+1
	lda     M0005
	sta     M0003
	lda     M000B+1
	sta     M0005+1
	lda     M000B
	sta     M0005
	lda     M0004+1
	sta     M000C+1
	lda     M0004
	sta     M000C
	lda     M0006+1
	sta     M0004+1
	lda     M0006
	sta     M0004
	lda     M000C+1
	sta     M0006+1
	lda     M000C
	sta     M0006
L002D:	lda     M0005
	sec
	sbc     M0003
	sta     M000D
	lda     M0005+1
	sbc     M0003+1
	sta     M000D+1
	lda     M0006
	sec
	sbc     M0004
	pha
	lda     M0006+1
	sbc     M0004+1
	tax
	pla
	sta     M000E
	stx     M000E+1
	jsr     _abs
	jsr     pushax
	lda     M000D
	ldx     M000D+1
	jsr     _abs
	jsr     tosicmp
	bmi     L002E
	beq     L002E
	jsr     decsp4
	lda     M000D
	ldy     #$02
	sta     (sp),y
	iny
	lda     M000D+1
	sta     (sp),y
	lda     #$00
	sta     (sp)
	ldy     #$01
	lda     #$10
	sta     (sp),y
	lda     M000E
	ldx     M000E+1
	jsr     _MultiplyThenDivide
	jsr     _abs
	jsr     asrax3
	sta     M000F
	stx     M000F+1
	ldx     M000D+1
	cpx     #$80
	lda     #$00
	bcc     L0030
	lda     #$08
L0030:	ora     #$04
	sta     M0001
	ldx     M000E+1
	cpx     #$80
	lda     #$00
	bcc     L0032
	lda     #$08
L0032:	ora     #$D0
	sta     M0002
	lda     M000E
	ldx     M000E+1
	bra     L0068
L002E:	jsr     decsp4
	lda     M000E
	ldy     #$02
	sta     (sp),y
	iny
	lda     M000E+1
	sta     (sp),y
	lda     #$00
	sta     (sp)
	ldy     #$01
	lda     #$10
	sta     (sp),y
	lda     M000D
	ldx     M000D+1
	jsr     _MultiplyThenDivide
	jsr     _abs
	jsr     asrax3
	sta     M000F
	stx     M000F+1
	ldx     M000E+1
	cpx     #$80
	lda     #$00
	bcc     L0036
	lda     #$08
L0036:	ora     #$D0
	sta     M0001
	ldx     M000D+1
	cpx     #$80
	lda     #$00
	bcc     L0038
	lda     #$08
L0038:	ora     #$04
	sta     M0002
	lda     M000D
	ldx     M000D+1
L0068:	jsr     _abs
	ina
	bne     L0039
	inx
L0039:	sta     regbank+1
	stx     regbank+1+1
	ldx     M0004+1
	jsr     axlong
	ldy     sreg
	sty     sreg+1
	stx     sreg
	ldx     M0004
	lda     #$00
	jsr     pusheax
	lda     M0004
	ldx     M0004+1
	jsr     aslax4
	jsr     aslax2
	jsr     axlong
	jsr     tosaddeax
	jsr     pusheax
	lda     M0003
	ldx     M0003+1
	jsr     axlong
	jsr     tosaddeax
	jsr     pusheax
	lda     $9F2F
	beq     L003A
	ldx     #$00
	txa
	bra     L003B
L003A:	tax
	lda     #$98
L003B:	jsr     axlong
	ldy     sreg
	sty     sreg+1
	stz     sreg
	tax
	lda     #$00
	jsr     shleax1
	jsr     tosaddeax
	sta     M0010
	stx     M0010+1
	ldy     sreg
	sty     M0010+2
	ldy     sreg+1
	sty     M0010+3
	lda     #$04
	sta     $9F25
	ina
	sta     $9F29
	sta     $9F25
	lda     M0002
	sta     $9F22
	lda     $9F22
	jsr     pusha0
	lda     M0010
	stz     sreg+1
	stz     sreg
	and     #$01
	jsr     shleax1
	jsr     tosulong
	jsr     tosoreax
	sta     $9F22
	lda     M0010+3
	sta     sreg+1
	lda     M0010+2
	sta     sreg
	ldx     M0010+1
	lda     M0010
	jsr     shreax1
	sta     M0010
	stx     M0010+1
	ldy     sreg
	sty     M0010+2
	ldy     sreg+1
	sty     M0010+3
	lda     M0010
	sta     $9F20
	lda     M0010+3
	sta     sreg+1
	lda     M0010+2
	sta     sreg
	ldx     M0010+1
	stx     M0010
	ldx     sreg
	ldy     sreg+1
	sty     sreg
	stz     sreg+1
	stx     M0010+1
	ldy     sreg
	sty     M0010+2
	ldy     sreg+1
	sty     M0010+3
	lda     M0010
	sta     $9F21
	lda     M0010+3
	sta     sreg+1
	lda     M0010+2
	sta     sreg
	ldx     M0010+1
	stx     M0010
	ldx     sreg
	ldy     sreg+1
	sty     sreg
	stz     sreg+1
	stx     M0010+1
	ldy     sreg
	sty     M0010+2
	ldy     sreg+1
	sty     M0010+3
	lda     $9F22
	jsr     pusha0
	lda     M0010
	and     #$01
	jsr     tosulong
	jsr     tosor0ax
	sta     $9F22
	lda     #$04
	sta     $9F25
	lda     M0001
	sta     $9F22
	lda     #$06
	sta     $9F25
	lda     M000F
	sta     $9F29
	ldx     M000F+1
	txa
	cpx     #$80
	ldx     #$00
	bcc     L003C
	dex
L003C:	sta     M000F
	stx     M000F+1
	and     #$03
	sta     $9F2A
	ldx     #$9F
	lda     #$24
	sta     regbank+4
	stx     regbank+4+1
	lda     _f_current_color
	asl     a
	asl     a
	asl     a
	asl     a
	sta     ptr1
	lda     _f_current_color
	ora     ptr1
	sta     regbank+3
	bra     L003F
L003D:	lda     regbank+3
	sta     (regbank+4)
	lda     regbank+3
	sta     (regbank+4)
	lda     regbank+3
	sta     (regbank+4)
	lda     regbank+3
	sta     (regbank+4)
	lda     regbank+3
	sta     (regbank+4)
	lda     regbank+3
	sta     (regbank+4)
	lda     regbank+3
	sta     (regbank+4)
	lda     regbank+3
	sta     (regbank+4)
	lda     regbank+1
	sec
	sbc     #$08
	sta     regbank+1
	bcs     L003F
	dec     regbank+1+1
L003F:	lda     regbank+1
	cmp     #$08
	lda     regbank+1+1
	sbc     #$00
	bvs     L0040
	eor     #$80
L0040:	bmi     L003D
L003E:	lda     regbank+1
	cmp     #$01
	lda     regbank+1+1
	sbc     #$00
	bvs     L0046
	eor     #$80
L0046:	bpl     L0064
	lda     regbank+3
	sta     (regbank+4)
	lda     regbank+1
	sec
	sbc     #$01
	sta     regbank+1
	bcs     L003E
	dec     regbank+1+1
	bra     L003E
L0064:	lda     #$04
	sta     $9F25
	stz     $9F29
L0001:	ldy     #$00
L0048:	lda     (sp),y
	sta     regbank+1,y
	iny
	cpy     #$05
	bne     L0048
	jmp     incsp5

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
M0007:
	.res	2,$00
M0008:
	.res	2,$00
M0009:
	.res	2,$00
M000A:
	.res	2,$00
M000B:
	.res	2,$00
M000C:
	.res	2,$00
M000D:
	.res	2,$00
M000E:
	.res	2,$00
M000F:
	.res	2,$00
M0010:
	.res	4,$00

.endproc

; ---------------------------------------------------------------
; void __near__ DefaultCallback (signed char waiting)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_DefaultCallback: near

.segment	"CODE"

	jsr     pusha
	lda     (sp)
	beq     L0005
	cmp     #$01
	jeq     L0004
	jmp     incsp1
L0005:	lda     _g_frame_counter
	ldx     _g_frame_counter+1
	cpx     M0001+1
	bne     L0013
	cmp     M0001
	jeq     L0004
L0013:	lda     _g_frame_counter+1
	sta     M0001+1
	lda     _g_frame_counter
	sta     M0001
	stz     M0004
	stz     M0004+1
	ldx     #$F9
	lda     #$01
	sta     sreg
	stz     sreg+1
	lda     #$C2
	sta     M0005
	stx     M0005+1
	ldy     sreg
	sty     M0005+2
	ldy     sreg+1
	sty     M0005+3
L0007:	lda     M0004
	cmp     #$10
	lda     M0004+1
	sbc     #$00
	bvc     L000B
	eor     #$80
L000B:	bpl     L0004
	lda     M0005+3
	sta     sreg+1
	lda     M0005+2
	sta     sreg
	ldx     M0005+1
	lda     M0005
	jsr     _vpeek
	sta     M0002
	and     #$3F
	sta     M0003
	lda     M0003
	beq     L0009
	ldx     #$00
	lda     M0003
	clc
	adc     M0003
	bcc     L0012
	inx
	clc
L0012:	adc     M0003
	bcc     L0010
	inx
L0010:	jsr     asrax2
	sta     M0003
	lda     M0002
	and     #$C0
	sta     ptr1
	lda     M0003
	ora     ptr1
	sta     M0002
	jsr     pusha
	lda     M0005+3
	sta     sreg+1
	lda     M0005+2
	sta     sreg
	ldx     M0005+1
	lda     M0005
	jsr     _vpoke
L0009:	inc     M0004
	bne     L000C
	inc     M0004+1
L000C:	ldy     #<(M0005)
	sty     ptr1
	ldy     #>(M0005)
	lda     #$04
	jsr     laddeqa
	jmp     L0007
L0004:	jmp     incsp1

.segment	"BSS"

M0001:
	.res	2,$00
M0002:
	.res	1,$00
M0003:
	.res	1,$00
M0004:
	.res	2,$00
M0005:
	.res	4,$00

.endproc

; ---------------------------------------------------------------
; void __near__ StopSounds (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_StopSounds: near

.segment	"CODE"

	stz     M0001
	stz     M0001+1
	ldx     #$F9
	lda     #$01
	sta     sreg
	stz     sreg+1
	lda     #$C2
	sta     M0002
	stx     M0002+1
	ldy     sreg
	sty     M0002+2
	ldy     sreg+1
	sty     M0002+3
L0002:	lda     M0001
	cmp     #$10
	lda     M0001+1
	sbc     #$00
	bvc     L0006
	eor     #$80
L0006:	bpl     L0003
	lda     #$C0
	jsr     pusha
	lda     M0002+3
	sta     sreg+1
	lda     M0002+2
	sta     sreg
	ldx     M0002+1
	lda     M0002
	jsr     _vpoke
	inc     M0001
	bne     L0007
	inc     M0001+1
L0007:	ldy     #<(M0002)
	sty     ptr1
	ldy     #>(M0002)
	lda     #$04
	jsr     laddeqa
	bra     L0002
L0003:	rts

.segment	"BSS"

M0001:
	.res	2,$00
M0002:
	.res	4,$00

.endproc

