; NOTE: This software is under the same copyright as FASM with only the
; copyright holder name changed.

; ARMv7 assembler core module v1.31 for flat assembler,
; Copyright (c) 2005-2014, revolution.
; All rights reserved.

; This program is free for commercial and non-commercial use as long as
; the following conditions are adhered to.

; Copyright remains revolution, and as such any Copyright notices
; in the code are not to be removed.

; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are
; met:

; 1. Redistributions of source code must retain the above copyright notice,
;    this list of conditions and the following disclaimer.
; 2. Redistributions in binary form must reproduce the above copyright
;    notice, this list of conditions and the following disclaimer in the
;    documentation and/or other materials provided with the distribution.

; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
; "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
; TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
; PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR
; CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
; EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
; PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
; LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
; NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

; The licence and distribution terms for any publically available
; version or derivative of this code cannot be changed. i.e. this code
; cannot simply be copied and put under another distribution licence
; (including the GNU Public Licence).

; For best viewing pleasure use a tab stop of 8 characters and fixed
; spacing font

; Not everything matches the ARM ADS assembly style, where possible the
; original style is used but there are some differences
; 1) label names cannot begin with a digit
; 2) CPSIE and CPSID formats are changed, use "iflags_aif" form instead of
;    "aif" (eg. "CPSIE iflags_i" instead of "CPSID i")
; 3) SRS with writeback must have a separating space after the mode number
;    and before "!" (eg. "SRSDB 16 !" instead of "SRSDB 16!")
; 4) macro, rept, irp, format, if, virtual etc. are all significant changes
;    from the ARM ADS, so you will need to re-write those sections of
;    existing code





;outline of [code_type] bits

CPU_ACTIVITY_THUMB_NON_UAL	= 1 shl 1	;1 for THUMB_NON_UAL mode, 0 for ARM or THUMB_UAL mode
CPU_ACTIVITY_THUMB_UAL		= 1 shl 2	;1 for THUMB_UAL mode, 0 for ARM or THUMB_NON_UAL mode
CPU_ACTIVITY_ARM		= 1 shl 3	;1 for ARM mode, 0 for THUMB_NON_UAL or THUMB_UAL mode
CPU_ACTIVITY_UNKNOWN		= 1 shl 4	;this the default at startup
CPU_ACTIVITY_THUMBEE		= 1 shl 5	;1 for THUMBEE mode when CPU_ACTIVITY_THUMB_UAL is active

CPU_CAPABILITY_26BIT		= 1 shl 00
CPU_CAPABILITY_V1		= 1 shl 01
CPU_CAPABILITY_V2		= 1 shl 02
CPU_CAPABILITY_A		= 1 shl 03
CPU_CAPABILITY_V3		= 1 shl 04
CPU_CAPABILITY_M		= 1 shl 05
CPU_CAPABILITY_V4		= 1 shl 06
CPU_CAPABILITY_V4T		= 1 shl 07
CPU_CAPABILITY_V5		= 1 shl 08
CPU_CAPABILITY_V5T		= 1 shl 09
CPU_CAPABILITY_E		= 1 shl 10
CPU_CAPABILITY_P		= 1 shl 11
CPU_CAPABILITY_J		= 1 shl 12
CPU_CAPABILITY_X		= 1 shl 13
CPU_CAPABILITY_V6		= 1 shl 14
CPU_CAPABILITY_V6T		= 1 shl 15
CPU_CAPABILITY_ALIGN		= 1 shl 16
CPU_CAPABILITY_K		= 1 shl 17
CPU_CAPABILITY_Z		= 1 shl 18
CPU_CAPABILITY_6M		= 1 shl 19
CPU_CAPABILITY_7M		= 1 shl 20
CPU_CAPABILITY_T2		= 1 shl 21
CPU_CAPABILITY_V7		= 1 shl 22
CPU_CAPABILITY_SYNC		= 1 shl 23
CPU_CAPABILITY_DIV		= 1 shl 24
CPU_CAPABILITY_T2EE		= 1 shl 25
CPU_CAPABILITY_MP		= 1 shl 26
CPU_CAPABILITY_VE		= 1 shl 27

COPRO_CAPABILITY_FPA_V1		= 1 shl 00
COPRO_CAPABILITY_FPA_V2		= 1 shl 01
COPRO_CAPABILITY_MAVERICK	= 1 shl 02
COPRO_CAPABILITY_VFP_V1xD	= 1 shl 03
COPRO_CAPABILITY_VFP_V1		= 1 shl 04
COPRO_CAPABILITY_VFP_V2		= 1 shl 05
COPRO_CAPABILITY_VFP_V3		= 1 shl 06
COPRO_CAPABILITY_VFP_D32	= 1 shl 07
COPRO_CAPABILITY_VFP_HP		= 1 shl 08
COPRO_CAPABILITY_XSCALE		= 1 shl 09
COPRO_CAPABILITY_IWMMXT_V1	= 1 shl 10
COPRO_CAPABILITY_IWMMXT_V2	= 1 shl 11
COPRO_CAPABILITY_SIMD_INT	= 1 shl 12
COPRO_CAPABILITY_SIMD_FLOAT	= 1 shl 13
COPRO_CAPABILITY_SIMD_HP	= 1 shl 14
COPRO_CAPABILITY_VFP_V4		= 1 shl 15
COPRO_CAPABILITY_SIMD_V2	= 1 shl 16

CPU_CAPABILITY_DEFAULT		=\
				1*CPU_CAPABILITY_26BIT+\
				1*CPU_CAPABILITY_V1+\
				1*CPU_CAPABILITY_V2+\
				1*CPU_CAPABILITY_A+\
				1*CPU_CAPABILITY_V3+\
				1*CPU_CAPABILITY_M+\
				1*CPU_CAPABILITY_V4+\
				1*CPU_CAPABILITY_V4T+\
				1*CPU_CAPABILITY_V5+\
				1*CPU_CAPABILITY_V5T+\
				1*CPU_CAPABILITY_E+\
				1*CPU_CAPABILITY_P+\
				1*CPU_CAPABILITY_J+\
				1*CPU_CAPABILITY_X+\
				1*CPU_CAPABILITY_V6+\
				1*CPU_CAPABILITY_V6T+\
				1*CPU_CAPABILITY_ALIGN+\
				1*CPU_CAPABILITY_K+\
				1*CPU_CAPABILITY_Z+\
				1*CPU_CAPABILITY_6M+\
				1*CPU_CAPABILITY_7M+\
				1*CPU_CAPABILITY_T2+\
				1*CPU_CAPABILITY_V7+\
				1*CPU_CAPABILITY_SYNC+\
				1*CPU_CAPABILITY_DIV+\
				1*CPU_CAPABILITY_T2EE+\
				1*CPU_CAPABILITY_MP+\
				1*CPU_CAPABILITY_VE

COPRO_CAPABILITY_DEFAULT	=\
				1*COPRO_CAPABILITY_FPA_V1+\
				1*COPRO_CAPABILITY_FPA_V2+\
				1*COPRO_CAPABILITY_MAVERICK+\
				1*COPRO_CAPABILITY_VFP_V1xD+\
				1*COPRO_CAPABILITY_VFP_V1+\
				1*COPRO_CAPABILITY_VFP_V2+\
				1*COPRO_CAPABILITY_VFP_V3+\
				1*COPRO_CAPABILITY_VFP_D32+\
				1*COPRO_CAPABILITY_VFP_HP+\
				1*COPRO_CAPABILITY_XSCALE+\
				1*COPRO_CAPABILITY_IWMMXT_V1+\
				1*COPRO_CAPABILITY_IWMMXT_V2+\
				1*COPRO_CAPABILITY_SIMD_INT+\
				1*COPRO_CAPABILITY_SIMD_FLOAT+\
				1*COPRO_CAPABILITY_SIMD_HP+\
				1*COPRO_CAPABILITY_VFP_V4+\
				1*COPRO_CAPABILITY_SIMD_V2

ARM_code			= CPU_ACTIVITY_ARM

FLAG_BIT_CONDITION_SET		= 0
FLAG_BIT_FORCE_WIDE		= 1
FLAG_BIT_FORCE_NARROW		= 2
FLAG_BIT_FORCE_AUTO_WIDE	= 3

FLAG_CONDITION_SET		= 1 shl FLAG_BIT_CONDITION_SET
FLAG_FORCE_WIDE			= 1 shl FLAG_BIT_FORCE_WIDE
FLAG_FORCE_NARROW		= 1 shl FLAG_BIT_FORCE_NARROW
FLAG_FORCE_AUTO_WIDE		= 1 shl FLAG_BIT_FORCE_AUTO_WIDE

CONDITION_SEARCH_CHARACTER	= '~'

;undefined instruction space, we use this for triggering unencodable instructions
;xxxx_0111_1111_xxxx_xxxx_xxxx_1111_xxxx  x7fxxxfx arm
;			1101_1110_xxxx_xxxx  dexx	   thumb1
;1111_0111_1111_xxxx_1010_xxxx_xxxx_xxxx  f7fxaxxx thumb2 (hword endian is reversed)

UNENCODEABLE_INSTRUCTION_ARM	= 0xf7ffffff
UNENCODEABLE_INSTRUCTION_THUMB16= 0xdeff
UNENCODEABLE_INSTRUCTION_THUMB32= 0xf7ffafff

THUMB_FLAG_NOT_INSIDE_IT	= 1 shl 0	;not allowable inside an IT block
THUMB_FLAG_ONLY_INSIDE_IT	= 1 shl 1	;only allowable inside an IT block
THUMB_FLAG_ONLY_LAST_IT		= 1 shl 2	;only allowable as last instruction of an IT block
THUMB_FLAG_IS_BCC		= 1 shl 3	;special Bcc opcodes are conditional but exist outside an IT block

ARM_INSTRUCTION_OPCODE_AND	= 0000b
ARM_INSTRUCTION_OPCODE_EOR	= 0001b
ARM_INSTRUCTION_OPCODE_SUB	= 0010b
ARM_INSTRUCTION_OPCODE_RSB	= 0011b
ARM_INSTRUCTION_OPCODE_ADD	= 0100b
ARM_INSTRUCTION_OPCODE_ADC	= 0101b
ARM_INSTRUCTION_OPCODE_SBC	= 0110b
ARM_INSTRUCTION_OPCODE_RSC	= 0111b
ARM_INSTRUCTION_OPCODE_TST	= 1000b
ARM_INSTRUCTION_OPCODE_TEQ	= 1001b
ARM_INSTRUCTION_OPCODE_CMP	= 1010b
ARM_INSTRUCTION_OPCODE_CMN	= 1011b
ARM_INSTRUCTION_OPCODE_ORR	= 1100b
ARM_INSTRUCTION_OPCODE_MOV	= 1101b
ARM_INSTRUCTION_OPCODE_BIC	= 1110b
ARM_INSTRUCTION_OPCODE_MVN	= 1111b

ARM_SHIFT_OPCODE_LSL	= 0
ARM_SHIFT_OPCODE_LSR	= 1
ARM_SHIFT_OPCODE_ASR	= 2
ARM_SHIFT_OPCODE_ROR	= 3

THUMB_INSTRUCTION_OPCODE_AND	= 0000b
THUMB_INSTRUCTION_OPCODE_EOR	= 0001b
THUMB_INSTRUCTION_OPCODE_LSL	= 0010b
THUMB_INSTRUCTION_OPCODE_LSR	= 0011b
THUMB_INSTRUCTION_OPCODE_ASR	= 0100b
THUMB_INSTRUCTION_OPCODE_ADC	= 0101b
THUMB_INSTRUCTION_OPCODE_SBC	= 0110b
THUMB_INSTRUCTION_OPCODE_ROR	= 0111b
THUMB_INSTRUCTION_OPCODE_TST	= 1000b
THUMB_INSTRUCTION_OPCODE_NEG	= 1001b
THUMB_INSTRUCTION_OPCODE_CMP	= 1010b
THUMB_INSTRUCTION_OPCODE_CMN	= 1011b
THUMB_INSTRUCTION_OPCODE_ORR	= 1100b
THUMB_INSTRUCTION_OPCODE_MUL	= 1101b
THUMB_INSTRUCTION_OPCODE_BIC	= 1110b
THUMB_INSTRUCTION_OPCODE_MVN	= 1111b

SIMD_REG_LIST_TYPE_REGISTER	= 0			;dy
SIMD_REG_LIST_TYPE_ELEMENT	= 1			;dy[x]
SIMD_REG_LIST_TYPE_VECTOR	= 2			;dy[]

;conditions for choosing 16 bit thumb when two options are available:
;	OPs	reg,...		- always encoded as short unless forced by a condition or explicit IT block
;	OP	reg,...		- default encoded as long. If the preceeding instruction was the same type,
;				  then if the preceeding instruction was encoded as long we recode it as
;				  short+short (IT + movs reg1,reg2 i.e. to begin the IT block), or if there
;				  are still remaining slots available in a previous IT block, then we encode
;				  this instruction as short (movs reg1,reg2) and extend the previous IT block.

	;renamed variable in X86_64
label force_wide_flag		dword at address_high
label IT_anchor_distance	 byte at segment_register
	;renamed variables in preprocessor for our use
label potential_IT_anchor	dword at macro_line
label explicit_IT_state		dword at macro_block
label current_IT_block		dword at macro_block_line
label thumb32_error		dword at macro_block_line_number
label thumb16_error		dword at macro_symbols
label thumb32_instruction	dword at struc_name
label thumb16_instruction	 word at struc_label
label anchor_instruction	 word at struc_label+2
label arm_instruction		dword at instant_macro_start
label reg_list_bitmap		dword at parameters_end
  label simd_reg_list_first	 byte at parameters_end+0
  label simd_reg_list_count	 byte at parameters_end+1
  label simd_reg_list_x		 byte at parameters_end+2
  label simd_reg_list_type	 byte at parameters_end+3
label operand_registers		dword at locals_counter
  label operand_register0	 byte at locals_counter+0
  label operand_register1	 byte at locals_counter+1
  label operand_register2	 byte at locals_counter+2
  label operand_register3	 byte at locals_counter+3
  label instruction_condition	 byte at locals_counter+4
  label instruction_shift_op	 byte at locals_counter+5
  label	copro_opcode1		 byte at locals_counter+6
  label	copro_opcode2		 byte at locals_counter+7
	;renamed variables in parser for our use
label immediate_value		dword at current_locals_prefix
label immediate_value2		dword at anonymous_reverse
  label current_reg_number	 byte at anonymous_forward+0
  label current_parameter	 byte at anonymous_forward+1
  label	thumb_flags_16		 byte at anonymous_forward+2
  label	thumb_flags_32		 byte at anonymous_forward+3
label cpu_capability_flags	dword at label_hash
label copro_capability_flags	dword at label_leaf
label ARM_error_line		dword at parsed_lines

;remove this macro if you want to drop support for processors that don't have CMOVcc
irp cc,a,ae,b,be,c,e,g,ge,l,le,na,nae,nb,nbe,nc,ne,ng,nge,nl,nle,no,np,ns,nz,o,p,pe,po,s,z {
	local i
	macro cmov#cc [args] \{
	    \common
	    \local .x
		j#cc	.x
		load	i from $-2
		store	(i xor 1) at $-2
		mov	args
	    .x:
	\}
}

macro patch labl,instr_search,instr_replace,offset {
	local code1,mask1,search1,replace1,code2,mask2,search2,replace2,length
	virtual at labl+offset
		irp i,instr_search\{i\}
		length=($-$$)
		dq 0,0
		load search1 qword from $$
		load search2 qword from $$+8
		if length < 8
			mask1=not((-1) shl (length*8))
			mask2=0
		else if length = 8
			mask1=-1
			mask2=0
		else ;length > 8
			mask1=-1
			mask2=not((-1) shl ((length-8)*8))
		end if
	end virtual
	virtual at labl+offset
		irp i,instr_replace\{i\}
		if ($-$$)<>length
			display 'Length original: ',length+'0',', Length replacement: ',($-$$)+'0',13,10
			rb -1 ; "Oops, patch instructions are different lengths"
		end if
		dq 0,0
		load replace1 qword from $$
		load replace2 qword from $$+8
	end virtual
	load code1 qword from labl+offset
	load code2 qword from labl+offset+8
	if ~(((code1 and mask1)=search1) & ((code2 and mask2)=search2))
		rb -1 ;"Oops, cannot do instruction patch"
	end if
	store qword ((code1 and not mask1)+replace1) at labl+offset
	store qword ((code2 and not mask2)+replace2) at labl+offset+8
}

macro override labl, instr {
	local code,mask,replace,length
	virtual at labl
		instr
		length=($-$$)
		dq 0
		load replace qword from $$
		mask=not((-1) shl (length*8))
	end virtual
	load code qword from labl
	store qword ((code and not mask)+replace) at labl
}

;All patches are done here

;patch to add FIT operator

	patch calculation_loop, <je calculate_not>, <call ARM_fit_operator,nop>, 107

;patches to save IT data inside virtual blocks

	patch set_virtual, <jmp instruction_assembled>, <jmp ARM_set_virtual>,  126
	patch end_virtual, <call find_structure_data>,  <call ARM_end_virtual>, 0

;patch to inhibit IT block generation across labels

	patch assemble_line, <jb define_label>, <jb ARM_define_label>, 35

;one patch to support single character symbols '!' and '^'

	patch get_symbol, <<sub cl,2>>, <<sub cl,1>>, 15

;two patches to catch the format arguments

	patch parse_instruction_arguments,<<cmp bx,section_directive-instruction_handler>>,<<cmp bx,ARM_section_directive-instruction_handler>>,88
	patch parse_instruction_arguments,<<cmp bx,format_directive-instruction_handler>>,<<cmp bx,ARM_format_directive-instruction_handler>>,95

;three patches for ELF format

	patch format_elf,	<<mov byte[edx+12h],3>>,	  <<mov byte[edx+12h],40>>,	62	;ARM
	patch format_elf,	<<mov [code_type],32>>,		  <<mov [code_type],ARM_code>>, 74	;CODE32
	patch elf_exe_brand_ok, <<mov [image_base],8048000h>>, <<mov [image_base],8000h>>,   0

;seven patches for PE format

	patch format_pe,	<<mov [machine],14Ch>>,		 	 <<mov [machine],1C0h>>,	  2   ;ARM
	patch format_pe,	<<mov [subsystem],3>>,			 <<mov [subsystem],9>>,		  11  ;WinCE
	patch format_pe,	<<mov [subsystem_version],3+10 shl 16>>, <<mov [subsystem_version],3>>,	  20  ;3.0
	patch format_pe,	<<mov [image_base],400000h>>,		 <<mov [image_base],10000h>>,	  30
	patch pe_alignment_ok,  <<mov dword[edx+16h],10B010Fh>>,	 <<mov dword[edx+16h],10B010Fh>>, 22
	patch pe_org_ok,	<<mov [code_type],32>>,		 	 <<mov [code_type],ARM_code>>,	  14  ;CODE32
	patch new_section,	<<mov [code_type],32>>,			 <<mov [code_type],ARM_code>>,	  83  ;CODE32

;two patches to allow {} around register lists and custom address formats

	patch parse_argument, <<cmp al,'['>,je address_argument>,    <<cmp al,'['>,je ARM_square_parser>, 99
	patch parse_argument, <<cmp al,'{'>,je unallowed_character>, <<cmp al,'{'>,je ARM_curly_parser>,  115
	patch parse_argument, <<cmp al,'#'>,je unallowed_character>, <<cmp al,'#'>,je separator>,	  131

;a patch for setting the alignment bytes to 0xff

	patch nops, <<mov eax,90909090h>>, <<mov eax,-1>>, 0

;patch to give us control when the code stream has finished being assembled

	patch finish_elf_segment, <<mov ebx,[number_of_sections]>,dec ebx>, <nop,push ARM_finish_elf_segment,ret>, 0

	finish_elf_exe_patch_length	= 7
	macro finish_elf_exe_patch_code {
		mov ebx,[number_of_sections]
		dec ebx
	}

;four patches to replace ' with _ as the numeric spacing character

	patch get_dec_digit, <<cmp byte[esi],"'">>, <<cmp byte[esi],'_'>>, 8
	patch get_bin_digit, <<cmp al,"'">>,	    <<cmp al,'_'>>,	   15
	patch get_hex_digit, <<cmp al,"'">>,	    <<cmp al,'_'>>,	   15
	patch get_oct_digit, <<cmp al,"'">>,	    <<cmp al,'_'>>,	   11

;patch the reset state at beginning of assembly passes

	patch pass_loop, <call assemble_line>, <call ARM_assemble_line>, 0

;patch to allow alternative half-precision number range

	patch fp_word_ok, <jge value_out_of_range>, <jg value_out_of_range>, 8

;patch to use a custom instruction searcher to convert '~~' to conditionals

	override get_instruction, jmp ARM_get_instruction

;patch to allow skipping of commas inside address expressions

	override skip_address, jmp skip_done

;patch to allow %p and %c predefined variables

	override get_predefined_id, jmp ARM_get_predefined_id	;in PARSER.INC
	override predefined_label, jmp ARM_predefined_label	;in EXPRESSI.INC

;patch to use a custom instruction handler to store address of line start

	override instruction_handler, jmp ARM_instruction_handler

;patch to disable use of 0x13 as code type setting (this is now a directive)

	patch assemble_line, <je code_type_setting>, <nop,nop>, 61

;patches to recognise base address registers in virtual

	patch symbol_value, <<cmp ah,4>>  , <<cmp ah,0xf>>, 40
	patch symbol_value, <<cmp ah,8>>  , <jmp no_address_register,nop>, 45
	patch check_ip_relative_address, <<cmp bh,0F8h>,je check_rip_relative_address,<cmp bh,0F4h>,jne invalid_address>, <<cmp bh,0F0h>,jae check_dword_value,times 5 nop>, 8

purge patch,override

;That was the last of the patches

;ARM, THUMB and THUMB_T2 encodings are sufficiently different that we use separate decoding code for each.
;The basic premise is that we decode all three together as much a possible using the templates.
;We decode the parameters and then split into each separate encoder once basic syntax checks are passed.
;Once split off, specific invalid encodings are checked by each encoder.
;
;In T2 mode, the IT instruction is checked by a common handler function and it is possible that previous
;instructions will be recoded if IT prediction was sub-optimal or invalid.

;decoder template definitions:

align 4

template_decode_table:
	dd	decode_template.TMPL_modifier_exclaim
	dd	decode_template.TMPL_modifier_translate
	dd	decode_template.TMPL_copro_opcode1
	dd	decode_template.TMPL_copro_opcode2
	dd	decode_template.TMPL_EOL
	dd	decode_template.TMPL_base_reg
	dd	decode_template.TMPL_base_reg!
	dd	decode_template.TMPL_cpro_sel
	dd	decode_template.TMPL_cpro_reg
	dd	decode_template.TMPL_endian
	dd	decode_template.TMPL_shift_op
	dd	decode_template.TMPL_vfps_reg
	dd	decode_template.TMPL_vfpd_reg
	dd	decode_template.TMPL_acc_40bt
	dd	decode_template.TMPL_vfp_syst
	dd	decode_template.TMPL_iflags
	dd	decode_template.TMPL_psr
	dd	decode_template.TMPL_imm
	dd	decode_template.TMPL_imm2
	dd	decode_template.TMPL_bracket_left
	dd	decode_template.TMPL_bracket_right
	dd	decode_template.TMPL_reg_list
	dd	decode_template.TMPL_vfp_reg_list_s
	dd	decode_template.TMPL_vfp_reg_list_d
	dd	decode_template.TMPL_rrx_op
	dd	decode_template.TMPL_add_sub_reg
	dd	decode_template.TMPL_address_reg
	dd	decode_template.TMPL_expression
	dd	decode_template.TMPL_address
	dd	decode_template.TMPL_option
	dd	decode_template.TMPL_comma
	dd	decode_template.TMPL_iwmmx_wreg
	dd	decode_template.TMPL_iwmmx_creg
	dd	decode_template.TMPL_mvrk_areg
	dd	decode_template.TMPL_mvrk_psc
	dd	decode_template.TMPL_fpa_freg
	dd	decode_template.TMPL_imm_float
	dd	decode_template.TMPL_barrier
	dd	decode_template.TMPL_condition
	dd	decode_template.TMPL_simd_qreg
	dd	decode_template.TMPL_vfpdx_reg
	dd	decode_template.TMPL_simd_reg_list
	dd	decode_template.TMPL_address_reg@
	dd	decode_template.TMPL_sysm_reg
	dd	decode_template.TMPL_banked_reg

TMPL_modifier_exclaim	= 0	;'!'
TMPL_modifier_translate	= 1	;'^'
TMPL_copro_opcode1	= 2	;imm
TMPL_copro_opcode2	= 3	;imm
TMPL_EOL		= 4	;0xf
TMPL_base_reg		= 5	;r0-r15
TMPL_base_reg!		= 6	;r0!-r15!
TMPL_cpro_sel		= 7	;p0-p15
TMPL_cpro_reg		= 8	;c0-c15
TMPL_endian		= 9	;le,be
TMPL_shift_op		= 10	;lsl,lsr,asr,ror
TMPL_vfps_reg		= 11	;s0-s31
TMPL_vfpd_reg		= 12	;d0-d31
TMPL_acc_40bt		= 13	;acc0-acc7
TMPL_vfp_syst		= 14	;vfp_syst
TMPL_iflags		= 15	;iflags_*
TMPL_psr		= 16	;cpsr_*,spsr_*
TMPL_imm		= 17	;'('
TMPL_imm2		= 18	;'('
TMPL_bracket_left	= 19	;'['
TMPL_bracket_right	= 20	;']'
TMPL_reg_list		= 21	;'{..}'
TMPL_vfp_reg_list_s	= 22	;'{..}'
TMPL_vfp_reg_list_d	= 23	;'{..}'
TMPL_rrx_op		= 24	;rrx
TMPL_add_sub_reg	= 25	;-rm, +rm
TMPL_address_reg	= 26	;base_reg without comma
TMPL_expression		= 27	;implicit reg + offset
TMPL_address		= 28	;code address immediate
TMPL_option		= 29	;{imm}
TMPL_comma		= 30	;,
TMPL_iwmmx_wreg		= 31	;wr0-wr15
TMPL_iwmmx_creg		= 32	;iwmmx_creg
TMPL_mvrk_areg		= 33	;a0-a3
TMPL_mvrk_psc		= 34	;dspsc
TMPL_fpa_freg		= 35	;f0-f7
TMPL_imm_float		= 36	;float
TMPL_barrier		= 37	;barrier
TMPL_condition		= 38	;conditions
TMPL_simd_qreg		= 39	;q0-q15
TMPL_vfpdx_reg		= 40	;d0[x]-d31[x] x=0-7
TMPL_simd_reg_list	= 41	;{d0,d1,...}, {d0,d2,...}, {d0[x],d1[x],...}, {d0[x],d2[x],...}, {d0[],d1[],...}, {d0[],d2[],...}
TMPL_address_reg@	= 42	;r0@x-r15@x
TMPL_sysm_reg		= 43	;apsr, iapsr, eapsr, xpsr, ipsr, epsr, iepsr, msp, psp, primask, basepri, basepri_max, faultmask, control
TMPL_banked_reg		= 44	;banked registers for MSR/MRS access

TEMPLATE_maximum_operand= 10
TEMPLATE_length		= TEMPLATE_maximum_operand

macro TEMPLATE [arg] {
    common
	local .x
	db	0	;count of templates stored
	 .x:
    forward
	irp val,arg \{
		db val
	\}
		db TMPL_EOL
	while ($-.x) mod TEMPLATE_length <> 0
		db -1
	end while
    common
	store byte (($-.x)/TEMPLATE_length)-1 at .x-1
}

decode_template:
	mov	[instruction_condition],al
	mov	[arm_instruction],edx
	mov	[thumb32_instruction],ecx
	call	ARM_generic_mode_checks
	xor	eax,eax
	mov	[instruction_shift_op],al
	mov	[current_reg_number],al
	mov	[operand_registers],eax
	mov	[current_parameter],al
	mov	[immediate_value2],eax
	mov	[immediate_value],eax
	mov	[thumb_flags_16],al
	mov	[thumb_flags_32],al
	mov	[copro_opcode1],al
	mov	[copro_opcode2],al
	mov	eax,[esp]				;get template definitions
	mov	ebx,0					;ebx=current template definition
	mov	ah,[eax]				;ah=highest constraint
	mov	al,0					;al=lowest constriant
    .refine_constraint:
    .refine_lower_constraint:
	movzx	ecx,al
	imul	ecx,ecx,TEMPLATE_length
	add	ecx,[esp]
	lea	ecx,[ecx+ebx+1]
	movzx	ecx,byte[ecx]				;ecx=template value
	push	ebx ecx eax esi
	call	dword[template_decode_table+ecx*4]
	pop	edx eax ecx ebx
	jnc	.lower_constraint_found
	mov	esi,edx
    .try_next_constraint:
	inc	al
	cmp	al,ah
	ja	.failed
	movzx	edx,al
	imul	edx,edx,TEMPLATE_length
	add	edx,[esp]
	lea	edx,[edx+ebx+1]
	cmp	cl,[edx]
	jz	.try_next_constraint
	jmp	.refine_lower_constraint
    .lower_constraint_found:
    .refine_higher_constraint:
	movzx	edx,ah
	imul	edx,edx,TEMPLATE_length
	add	edx,[esp]
	lea	edx,[edx+ebx+1]
	cmp	cl,[edx]
	jz	.higher_constraint_found
	dec	ah
	cmp	ah,al
	jb	.failed
	jmp	.refine_higher_constraint
    .higher_constraint_found:
    .next_parameter:
	cmp	cl,TMPL_EOL
	jz	.end_of_line
	inc	bl
	cmp	bl,TEMPLATE_maximum_operand
	jb	.refine_constraint
	cmp	byte[esi],0xf				;end of line?
	jne	.extra_characters_on_line
    .end_of_line:
	cmp	ah,al
	jnz	.amibguous_definition
    .finish:
    ;return al=matched template ordinal
	mov	ecx,[esp]				;get template definitions
	movzx	ebx,byte[ecx]
	imul	ebx,ebx,TEMPLATE_length
	lea	ebx,[ebx+ecx+1+TEMPLATE_length]
	mov	[esp],ebx
    ;create a default instruction for ARM
	mov	ebp,[arm_instruction]
	mov	cl,[instruction_condition]
	and	cl,0xf0
	shl	ecx,24
	or	ebp,ecx
	mov	ecx,ebp
	or	ecx,1 shl 20				;set the S bit
	test	[instruction_condition],FLAG_CONDITION_SET
	cmovnz	ebp,ecx
	mov	[arm_instruction],ebp
	ret
    .unexpected_end_of_line:
	jmp	ERROR_unexpected_end_of_line
    .extra_characters_on_line:
	jmp	extra_characters_on_line
    .amibguous_definition:
    .failed:
	cmp	[current_parameter],0
	jz	ERROR_parameter_n_not_valid.first
	cmp	[current_parameter],1
	jz	ERROR_parameter_n_not_valid.second
	cmp	[current_parameter],2
	jz	ERROR_parameter_n_not_valid.third
	cmp	[current_parameter],3
	jz	ERROR_parameter_n_not_valid.fourth
	cmp	[current_parameter],4
	jz	ERROR_parameter_n_not_valid.fifth
	cmp	[current_parameter],5
	jz	ERROR_parameter_n_not_valid.sixth
	jmp	ERROR_parameter_n_not_valid.all
    .skip_comma:
	mov	al,[esi]
	cmp	al,0xf					;end of line?
	jz	.TMPL_okay
	cmp	al,0x0					;end of source?
	jz	.TMPL_okay
	inc	esi
	cmp	al,','
	jnz	.expecting_comma
	cmp	byte[esi],0xf				;end of line?
	jz	.unexpected_end_of_line
	cmp	byte[esi],0x0				;end of source?
	jz	.unexpected_end_of_line
    .TMPL_okay:
	clc
	retn
    .expecting_comma:
    .TMPL_not_matched:
	stc
	retn
    .TMPL_modifier_exclaim:
	cmp	word[esi],modifier + modifier_low shl 8
	jnz	.TMPL_not_matched
	add	esi,2
	jmp	.TMPL_okay
    .TMPL_modifier_translate:
	cmp	word[esi],modifier + modifier_high shl 8
	jnz	.TMPL_not_matched
	add	esi,2
	jmp	.TMPL_okay
    .TMPL_copro_opcode1:
	cmp	byte[esi],'#'
	setz	dl
	movzx	edx,dl
	add	edx,esi
	cmp	byte[edx],'('
	jnz	.TMPL_not_matched
	push	esi
	lea	esi,[edx+1]
	call	ARM_calculate_expression
	mov	edx,esi
	pop	esi
	cmp	word[edi+8],0				;reg2, reg1
	jnz	.TMPL_not_matched
	mov	esi,edx
	mov	eax,[edi+16]
	mov	[symbol_identifier],eax
	mov	al,[edi+12]
	mov	[value_type],al
	mov	eax,[edi]
	mov	edx,[edi+4]
	mov	[copro_opcode1],al
	inc	[current_parameter]
	cmp	eax,0xf
	ja	.TMPL_out_of_range
	test	edx,edx
	jnz	.TMPL_out_of_range
	test	byte[edi+13],1				;check sign
	jz	.skip_comma
    .TMPL_out_of_range:
	mov	ecx,ERROR_value_out_of_range
	call	ARM_defer_error
	jmp	.skip_comma
    .TMPL_copro_opcode2:
	cmp	byte[esi],'#'
	setz	dl
	movzx	edx,dl
	add	edx,esi
	cmp	byte[edx],'('
	jnz	.TMPL_not_matched
	push	esi
	lea	esi,[edx+1]
	call	ARM_calculate_expression
	mov	edx,esi
	pop	esi
	cmp	word[edi+8],0				;reg2, reg1
	jnz	.TMPL_not_matched
	mov	esi,edx
	mov	eax,[edi+16]
	mov	[symbol_identifier],eax
	mov	al,[edi+12]
	mov	[value_type],al
	mov	eax,[edi]
	mov	edx,[edi+4]
	mov	[copro_opcode2],al
	inc	[current_parameter]
	cmp	eax,0x7
	ja	.TMPL_out_of_range
	test	edx,edx
	jnz	.TMPL_out_of_range
	test	byte[edi+13],1				;check sign
	jz	.skip_comma
	jmp	.TMPL_out_of_range
    .TMPL_EOL:
	cmp	byte[esi],0xf				;end of line?
	jz	.TMPL_okay
	cmp	byte[esi],0x0				;end of source?
	jz	.TMPL_okay
	jmp	.TMPL_not_matched
    .TMPL_base_reg:
	cmp	byte[esi],base_reg
	jnz	.TMPL_not_matched
	cmp	byte[esi+1],0xf0
	jb	.TMPL_not_matched
    .TMPL_base_reg.do:
	lodsw						;al=type, ah=reg
	movzx	ecx,[current_reg_number]
	and	ah,0x0f
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	inc	[current_parameter]
	jmp	.skip_comma
    .TMPL_base_reg!:
	cmp	byte[esi],base_reg!
	jnz	.TMPL_not_matched
	jmp	.TMPL_base_reg.do
    .TMPL_cpro_sel:
	cmp	byte[esi],cpro_sel
	jnz	.TMPL_not_matched
	cmp	byte[esi+1],cpro_sel_low
	jb	.TMPL_not_matched
	cmp	byte[esi+1],cpro_sel_high
	ja	.TMPL_not_matched
	lodsw						;al=type, ah=copro
	sub	ah,cpro_sel_low
	movzx	ecx,[current_reg_number]
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	inc	[current_parameter]
	jmp	.skip_comma
    .TMPL_cpro_reg:
	cmp	byte[esi],cpro_reg
	jnz	.TMPL_not_matched
	cmp	byte[esi+1],cpro_reg_low
	jb	.TMPL_not_matched
	cmp	byte[esi+1],cpro_reg_high
	ja	.TMPL_not_matched
	lodsw						;al=type, ah=copro
	sub	ah,cpro_reg_low
	movzx	ecx,[current_reg_number]
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	inc	[current_parameter]
	jmp	.skip_comma
    .TMPL_fpa_freg:
	cmp	byte[esi],fpa_freg
	jnz	.TMPL_not_matched
	cmp	byte[esi+1],fpa_freg_low
	jb	.TMPL_not_matched
	cmp	byte[esi+1],fpa_freg_high
	ja	.TMPL_not_matched
	lodsw						;al=type, ah=freg
	sub	ah,fpa_freg_low
	movzx	ecx,[current_reg_number]
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	inc	[current_parameter]
	jmp	.skip_comma
    .TMPL_barrier:
	cmp	byte[esi],barrier
	jnz	.TMPL_not_matched
	cmp	byte[esi+1],barrier_low
	jb	.TMPL_not_matched
	cmp	byte[esi+1],barrier_high
	ja	.TMPL_not_matched
	lodsw						;al=type, ah=barrier
	sub	ah,barrier_low
	movzx	ecx,[current_reg_number]
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	inc	[current_parameter]
	jmp	.skip_comma
    .TMPL_condition:
	cmp	word[esi],endian+endian_low*0x100	;special value for overlap with LE
	mov	ah,0xd
	jz	.TMPL_condition.found2
	cmp	byte[esi],0xf0				;special value for overlap with EQ
	mov	ah,0x0
	jz	.TMPL_condition.found1
	mov	ax,[esi]
	cmp	al,condition
	jnz	.TMPL_not_matched
	cmp	ah,condition_low
	jb	.TMPL_not_matched
	cmp	ah,condition_high
	ja	.TMPL_not_matched
	sub	ah,condition_low
    .TMPL_condition.found2:
	inc	esi
    .TMPL_condition.found1:
	inc	esi					;al=type, ah=condition
	movzx	ecx,[current_reg_number]
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	inc	[current_parameter]
	jmp	.skip_comma
    .TMPL_shift_op:
	cmp	byte[esi],shift_op
	jnz	.TMPL_not_matched
	cmp	byte[esi+1],shift_op_low
	jb	.TMPL_not_matched
	cmp	byte[esi+1],shift_op_high
	ja	.TMPL_not_matched
	lodsw						;al=type, ah=shift
	sub	ah,shift_op_low
	mov	[instruction_shift_op],ah
	jmp	.TMPL_okay
    .TMPL_vfpdx_reg:
	cmp	byte[esi],vfpd_reg
	jnz	.TMPL_not_matched
	cmp	word[esi+2],'[('
	jnz	.TMPL_not_matched
	push	esi
	add	esi,4
	call	ARM_calculate_expression
	mov	edx,esi
	pop	esi
	cmp	byte[edx],']'
	jnz	.TMPL_not_matched
	cmp	word[edi+8],0				;reg2, reg1
	jnz	.TMPL_not_matched
	mov	ah,[esi+1]				;ah=vfp dreg
    ;double and shift high bit to LSb
	add	ah,ah
	mov	al,ah
	shr	al,5
	and	ax,0x1e01
	or	ah,al
	movzx	ecx,[current_reg_number]
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	lea	esi,[edx+1]
	mov	eax,[edi+16]
	mov	[symbol_identifier],eax
	mov	al,[edi+12]
	mov	[value_type],al
	mov	eax,[edi]
	mov	edx,[edi+4]
	mov	[immediate_value],eax
	inc	[current_parameter]
	cmp	eax,0x7
	ja	.TMPL_out_of_range
	test	edx,edx
	jnz	.TMPL_out_of_range
	test	byte[edi+13],1				;check sign
	jz	.skip_comma
	jmp	.TMPL_out_of_range
    .TMPL_vfps_reg:
	movzx	eax,word[esi]
	cmp	al,vfps_reg
	jnz	.TMPL_not_matched
	cmp	ah,vfps_reg_high
	ja	.TMPL_not_matched
	sub	ah,vfps_reg_low
	jc	.TMPL_not_matched
	add	esi,2
	movzx	ecx,[current_reg_number]
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	inc	[current_parameter]
	jmp	.skip_comma
    .TMPL_vfpd_reg:
	movzx	eax,word[esi]
	cmp	al,vfpd_reg
	jnz	.TMPL_not_matched
	cmp	ah,vfpd_reg_high
	ja	.TMPL_not_matched
	sub	ah,vfpd_reg_low
	jc	.TMPL_not_matched
	add	esi,2
    .TMPL_vfpd_reg.do:
    ;double and shift high bit to LSb
	add	ah,ah
	mov	al,ah
	shr	al,5
	and	ax,0x1e01
	or	ah,al
	movzx	ecx,[current_reg_number]
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	inc	[current_parameter]
	jmp	.skip_comma
    .TMPL_simd_qreg:
	cmp	byte[esi],simd_qreg
	jnz	.TMPL_not_matched
	cmp	byte[esi+1],simd_qreg_low
	jb	.TMPL_not_matched
	cmp	byte[esi+1],simd_qreg_high
	ja	.TMPL_not_matched
	lodsw						;al=type, ah=qreg
	sub	ah,simd_qreg_low
	add	ah,ah
	jmp	.TMPL_vfpd_reg.do
    .TMPL_vfp_syst:
	cmp	byte[esi],vfp_syst
	jnz	.TMPL_not_matched
	cmp	byte[esi+1],vfp_syst_low
	jb	.TMPL_not_matched
	cmp	byte[esi+1],vfp_syst_high
	ja	.TMPL_not_matched
	lodsw						;al=type, ah=vfp system reg
	sub	ah,vfp_syst_low
	movzx	ecx,[current_reg_number]
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	inc	[current_parameter]
	jmp	.skip_comma
    .TMPL_acc_40bt:
	cmp	byte[esi],acc_40bt
	jnz	.TMPL_not_matched
	cmp	byte[esi+1],acc_40bt_low
	jb	.TMPL_not_matched
	cmp	byte[esi+1],acc_40bt_high
	ja	.TMPL_not_matched
	lodsw						;al=type, ah=accumulator reg
	sub	ah,acc_40bt_low
	movzx	ecx,[current_reg_number]
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	inc	[current_parameter]
	jmp	.skip_comma
    .TMPL_endian:
	cmp	byte[esi],endian
	jnz	.TMPL_not_matched
	cmp	byte[esi+1],endian_low
	jb	.TMPL_not_matched
	cmp	byte[esi+1],endian_high
	ja	.TMPL_not_matched
	lodsw						;al=type, ah=endian
	sub	ah,endian_low
	movzx	ecx,[current_reg_number]
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	inc	[current_parameter]
	jmp	.TMPL_okay
    .TMPL_iflags:
	cmp	byte[esi],iflags
	jnz	.TMPL_not_matched
	cmp	byte[esi+1],iflags_low
	jb	.TMPL_not_matched
	cmp	byte[esi+1],iflags_high
	ja	.TMPL_not_matched
	lodsw						;al=type, ah=psr
	sub	ah,iflags_low
	movzx	ecx,[current_reg_number]
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	inc	[current_parameter]
	jmp	.skip_comma
    .TMPL_psr:
	cmp	byte[esi],psr_reg
	jnz	.TMPL_not_matched
	cmp	byte[esi+1],psr_reg_low
	jb	.TMPL_not_matched
	cmp	byte[esi+1],psr_reg_high
	ja	.TMPL_not_matched
	lodsw						;al=type, ah=psr
	sub	ah,psr_reg_low
	movzx	ecx,[current_reg_number]
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	inc	[current_parameter]
	jmp	.skip_comma
    .TMPL_iwmmx_wreg:
	cmp	byte[esi],iwmmx_wreg
	jnz	.TMPL_not_matched
	cmp	byte[esi+1],iwmmx_wreg_low
	jb	.TMPL_not_matched
	cmp	byte[esi+1],iwmmx_wreg_high
	ja	.TMPL_not_matched
	lodsw						;al=type, ah=wreg
	sub	ah,iwmmx_wreg_low
	movzx	ecx,[current_reg_number]
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	inc	[current_parameter]
	jmp	.skip_comma
    .TMPL_iwmmx_creg:
	cmp	byte[esi],iwmmx_creg
	jnz	.TMPL_not_matched
	cmp	byte[esi+1],iwmmx_creg_low
	jb	.TMPL_not_matched
	cmp	byte[esi+1],iwmmx_creg_high
	ja	.TMPL_not_matched
	lodsw						;al=type, ah=creg
	sub	ah,iwmmx_creg_low
	movzx	ecx,[current_reg_number]
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	inc	[current_parameter]
	jmp	.skip_comma
    .TMPL_address:
	cmp	byte[esi],'('
	jnz	.TMPL_not_matched
	cmp	byte[esi+1],'.'				;float?
	jz	.TMPL_not_matched
	cmp	byte[esi],'#'
	setz	dl
	movzx	edx,dl
	add	edx,esi
	cmp	byte[edx],'('
	jnz	.TMPL_not_matched
	push	esi
	lea	esi,[edx+1]
	call	ARM_calculate_expression
	mov	edx,esi
	pop	esi
	pushfd
	pop	ecx
	cmp	word[edi+8],0				;reg2, reg1
	jnz	.TMPL_not_matched
	mov	esi,edx
	mov	eax,[edi+16]
	mov	[symbol_identifier],eax
	mov	al,[edi+12]
	mov	[value_type],al
	mov	eax,[edi]
	mov	edx,[edi+4]
	push	ecx
	popfd
	jnc	.TMPL_address.defined
	mov	eax,[addressing_space]
	mov	eax,[eax+0]				;org origin
	sub	eax,edi
	neg	eax
	and	eax,not 3
	add	eax,8
	mov	[immediate_value],eax
	jmp	.TMPL_okay
    .TMPL_address.defined:
	mov	[immediate_value],eax
	inc	[current_parameter]
	movzx	ecx,byte[edi+13]			;sign bit
	and	ecx,1
	or	ecx,edx
	jz	.TMPL_okay
	cdq
	cmp	edx,[edi+4]
	jnz	.TMPL_address.out_of_range
	xor	dl,[edi+13]				;check sign
	test	dl,1
	jz	.TMPL_okay
    .TMPL_address.out_of_range:
	mov	ecx,ERROR_value_out_of_range
	call	ARM_defer_error
	jmp	.TMPL_okay
    .TMPL_imm_float:
	mov	eax,[esi]
	cmp	ax,'(.'
	jz	.TMPL_imm
	and	eax,0x00ffffff
	cmp	eax,'#(.'
	jz	.TMPL_imm
	jmp	.TMPL_not_matched
    .TMPL_imm2:
	mov	ecx,immediate_value2
	jmp	.TMPL_imm.do
    .TMPL_imm:
	mov	ecx,immediate_value
    .TMPL_imm.do:
	cmp	byte[esi],'#'
	setz	dl
	movzx	edx,dl
	add	edx,esi
	cmp	byte[edx],'('
	jnz	.TMPL_not_matched
	push	esi ecx
	lea	esi,[edx+1]
	call	ARM_calculate_expression
	mov	edx,esi
	pop	ecx esi
	cmp	word[edi+8],0				;reg2, reg1
	jnz	.TMPL_not_matched
	mov	esi,edx
	mov	eax,[edi+16]
	mov	[symbol_identifier],eax
	mov	al,[edi+12]
	mov	[value_type],al
	mov	eax,[edi]
	mov	edx,[edi+4]
	mov	[ecx],eax
	inc	[current_parameter]
	movzx	ecx,byte[edi+13]			;sign bit
	and	ecx,1
	or	ecx,edx
	jz	.TMPL_okay
	cdq
	cmp	edx,[edi+4]
	jnz	.TMPL_imm.out_of_range
	xor	dl,[edi+13]				;check sign
	test	dl,1
	jz	.TMPL_okay
    .TMPL_imm.out_of_range:
	mov	ecx,ERROR_value_out_of_range
	call	ARM_defer_error
	jmp	.TMPL_okay
    .TMPL_bracket_left:
	mov	[size_override],0
	mov	edx,esi
	cmp	byte[edx],size_opr
	jnz	.no_size_operator
	mov	ax,[edx]
	cmp	ah,[operand_size]
	jnz	ERROR_operand_sizes_do_not_match
	add	edx,2
	mov	[size_override],1
    .no_size_operator:
	cmp	byte[edx],'['
	jnz	.TMPL_not_matched
	inc	edx
	mov	esi,edx
	jmp	.TMPL_okay
    .TMPL_bracket_right:
	cmp	byte[esi],']'
	jnz	.TMPL_not_matched
	inc	esi
	cmp	word[esi],modifier + modifier_low shl 8
	jnz	.skip_comma
	jmp	.TMPL_okay
    .TMPL_simd_reg_list:
	cmp	byte[esi],'{'
	jnz	.TMPL_not_matched
	inc	esi
	lodsw
	cmp	al,vfpd_reg
	jnz	.failed
	mov	[simd_reg_list_first],ah
	mov	[simd_reg_list_count],1
	mov	[simd_reg_list_x],0
	mov	[simd_reg_list_type],SIMD_REG_LIST_TYPE_REGISTER
	cmp	byte[esi],'}'
	jz	.TMPL_simd_reg_list.done
	cmp	word[esi],'[]'
	jz	.TMPL_simd_reg_list.vector
	cmp	word[esi],'[('
	jz	.TMPL_simd_reg_list.element
    .TMPL_simd_reg_list.check_next:
	cmp	byte[esi],'}'
	jz	.TMPL_simd_reg_list.done
	cmp	byte[esi],','
	jnz	.failed
	inc	esi
	lodsw
	cmp	al,vfpd_reg
	jnz	.failed
	inc	[simd_reg_list_count]
	cmp	[simd_reg_list_count],2			;second reg sets the separation
	jnz	.TMPL_simd_reg_list.check_separation
	sub	ah,[simd_reg_list_first]
	dec	ah
	test	ah,not 1
	jnz	.failed
	and	ah,1
	ror	ah,1
	or	[simd_reg_list_count],ah
	jmp	.TMPL_simd_reg_list.separation_okay
    .TMPL_simd_reg_list.check_separation:
	mov	al,[simd_reg_list_first]
	mov	cl,[simd_reg_list_count]
	mov	ch,cl
	and	ch,0x7f
	sub	ah,al
	rol	cl,1
	and	cl,1
	shr	ah,cl
	jc	.failed
	inc	ah
	cmp	ah,ch
	jnz	.failed
    .TMPL_simd_reg_list.separation_okay:
	cmp	[simd_reg_list_type],SIMD_REG_LIST_TYPE_VECTOR
	jz	.TMPL_simd_reg_list.check_vector
	cmp	[simd_reg_list_type],SIMD_REG_LIST_TYPE_ELEMENT
	jz	.TMPL_simd_reg_list.check_element
	jmp	.TMPL_simd_reg_list.check_next
    .TMPL_simd_reg_list.check_vector:
	cmp	word[esi],'[]'
	jnz	.failed
	add	esi,2
	jmp	.TMPL_simd_reg_list.check_next
    .TMPL_simd_reg_list.check_element:
	cmp	word[esi],'[('
	jnz	.failed
	add	esi,2
	call	ARM_calculate_expression
	cmp	word[edi+8],0				;reg2, reg1
	jnz	.failed
	mov	eax,[edi]
	mov	edx,[edi+4]
	test	edx,edx
	jnz	.failed
	test	byte[edi+13],1				;check sign
	jnz	.failed
	cmp	eax,7
	ja	.failed
	cmp	[simd_reg_list_x],al
	jnz	.failed
	lodsb
	cmp	al,']'
	jnz	.failed
	jmp	.TMPL_simd_reg_list.check_next
    .TMPL_simd_reg_list.vector:
	mov	[simd_reg_list_type],SIMD_REG_LIST_TYPE_VECTOR
	add	esi,2
	jmp	.TMPL_simd_reg_list.check_next
    .TMPL_simd_reg_list.element:
	mov	[simd_reg_list_type],SIMD_REG_LIST_TYPE_ELEMENT
	add	esi,2
	call	ARM_calculate_expression
	cmp	word[edi+8],0				;reg2, reg1
	jnz	.failed
	mov	eax,[edi]
	mov	edx,[edi+4]
	test	edx,edx
	jnz	.failed
	test	byte[edi+13],1				;check sign
	jnz	.failed
	cmp	eax,7
	ja	.failed
	mov	[simd_reg_list_x],al
	lodsb
	cmp	al,']'
	jnz	.failed
	jmp	.TMPL_simd_reg_list.check_next
    .TMPL_simd_reg_list.done:
	inc	esi
	inc	[current_parameter]
	jmp	.skip_comma
    .TMPL_vfp_reg_list_s:
	mov	dl,vfps_reg
	jmp	.TMPL_reg_list.decode
    .TMPL_vfp_reg_list_d:
	mov	dl,vfpd_reg
	jmp	.TMPL_reg_list.decode
    .TMPL_reg_list:
	mov	dl,base_reg
    .TMPL_reg_list.decode:
	cmp	byte[esi],'{'
	jnz	.TMPL_not_matched
	mov	[reg_list_bitmap],0
	inc	esi
    .TMPL_reg_list.next:
	lodsb
	cmp	al,'}'
	jz	.TMPL_reg_list.done
	cmp	al,dl
	jnz	.TMPL_not_matched
	lodsb
	cmp	dl,base_reg
	setz	dh
	neg	dh
	and	dh,0xf0
	cmp	al,dh
	jb	.failed
	not	dh
	and	al,dh
	movzx	ecx,al
	bts	[reg_list_bitmap],ecx
	jc	ERROR_repeated_register_in_list
	lodsb
	cmp	al,'}'
	jz	.TMPL_reg_list.done
	cmp	al,','
	jz	.TMPL_reg_list.next
	cmp	al,'-'
	jnz	.failed
    .TMPL_reg_list.range:
	lodsw
	cmp	al,dl
	jnz	.failed
	not	dh
	cmp	ah,dh
	jb	.failed
	not	dh
	and	ah,dh
	movzx	ebx,ah
	btr	[reg_list_bitmap],ecx
	cmp	ecx,ebx
	je	ERROR_repeated_register_in_list
	jg	.TMPL_reg_list.range_next
	xchg	ecx,ebx
    .TMPL_reg_list.range_next:
	bts	[reg_list_bitmap],ecx
	jc	ERROR_repeated_register_in_list
	dec	ecx
	cmp	ecx,ebx
	jge	.TMPL_reg_list.range_next
	lodsb
	cmp	al,','
	jz	.TMPL_reg_list.next
	cmp	al,'}'
	jnz	.failed
    .TMPL_reg_list.done:
	cmp	[reg_list_bitmap],0
	jz	ERROR_empty_set
	inc	[current_parameter]
	jmp	.TMPL_okay
    .TMPL_rrx_op:
	cmp	word[esi],rrx_op + rrx_op_offset shl 8
	jnz	.TMPL_not_matched
	add	esi,2
	inc	[current_parameter]
	jmp	.TMPL_okay
    .TMPL_sysm_reg:
	movzx	eax,word[esi]
	cmp	ax,(psr_reg_low+34)*0x100+psr_reg
	jz	.TMPL_sysm_reg.apsr
	cmp	al,sysm_reg
	jnz	.TMPL_not_matched
	cmp	ah,sysm_reg_high
	ja	.TMPL_not_matched
	sub	ah,sysm_reg_low
	jc	.TMPL_not_matched
	jmp	.TMPL_sysm_reg.store
    .TMPL_sysm_reg.apsr:
	mov	ah,0
    .TMPL_sysm_reg.store:
	add	esi,2
	movzx	ecx,[current_reg_number]
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	inc	[current_parameter]
	jmp	.skip_comma
    .TMPL_banked_reg:
	movzx	eax,word[esi]
	cmp	al,banked_reg
	jnz	.TMPL_not_matched
	cmp	ah,banked_reg_high
	ja	.TMPL_not_matched
	sub	ah,banked_reg_low
	jc	.TMPL_not_matched
	add	esi,2
	movzx	ecx,[current_reg_number]
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	inc	[current_parameter]
	jmp	.skip_comma
    .TMPL_mvrk_areg:
	movzx	eax,word[esi]
	cmp	al,mvrk_areg
	jnz	.TMPL_mvrk_areg_a1_a3
	sub	ah,mvrk_areg_offset
	jz	.TMPL_mvrk_areg_a0
    .TMPL_mvrk_areg_a1_a3:
	cmp	al,base_reg
	jnz	.TMPL_not_matched
	cmp	ah,0xf2
	ja	.TMPL_not_matched
	inc	ah
	and	ah,0x0f
    .TMPL_mvrk_areg_a0:
	add	esi,2
	movzx	ecx,[current_reg_number]
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	inc	[current_parameter]
	jmp	.skip_comma
    .TMPL_mvrk_psc:
	cmp	word[esi],mvrk_psc + mvrk_psc_offset shl 8
	jnz	.TMPL_not_matched
	add	esi,2
	inc	[current_parameter]
	jmp	.skip_comma
    .TMPL_add_sub_reg:
	mov	cl,0
	mov	edx,esi
	cmp	byte[esi],'-'
	jnz	.TMPL_add_reg
	mov	cl,0x80
	inc	edx
    .TMPL_add_reg:
	cmp	byte[edx],base_reg
	jnz	.TMPL_reg_in_expression
	cmp	byte[edx+1],0xf0
	jb	.TMPL_not_matched
	mov	esi,edx
	lodsw						;al=type, ah=reg
	and	ah,0x0f
	or	ah,cl
	movzx	ecx,[current_reg_number]
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	inc	[current_parameter]
	cmp	byte[esi],']'
	jnz	.skip_comma
	jmp	.TMPL_okay
    .TMPL_reg_in_expression:
	cmp	byte[esi],'('
	jnz	.TMPL_not_matched
	mov	ax,[esi+1]
	cmp	al,base_reg
	jnz	.TMPL_not_matched
	cmp	ah,0xf0
	jb	.TMPL_not_matched
	and	ah,0x8f
	cmp	byte[esi+3],')'
	jz	.TMPL_reg_in_expression.add_reg
	cmp	word[esi+3],')' shl 8 + 0x83		;unary minus?
	jnz	.TMPL_not_matched
	or	ah,0x80
	inc	esi
    .TMPL_reg_in_expression.add_reg:
	add	esi,4
	movzx	ecx,[current_reg_number]
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	inc	[current_parameter]
	cmp	byte[esi],','
	jz	.skip_comma
	jmp	.TMPL_okay
    .TMPL_address_reg:
	cmp	byte[esi+1],0xf0
	jb	.TMPL_not_matched
    .TMPL_address_reg@:
	cmp	byte[esi],base_reg
	jnz	.TMPL_not_matched
	lodsw						;al=type, ah=reg
	cmp	ah,0xf0
	jb	.TMPL_address_reg@.aligned
	and	ah,0x0f
    .TMPL_address_reg@.aligned:
	movzx	ecx,[current_reg_number]
	mov	[operand_register0+ecx],ah
	inc	[current_reg_number]
	inc	[current_parameter]
	cmp	byte[esi],']'
	jnz	.skip_comma
	jmp	.TMPL_okay
    .TMPL_expression:
	cmp	byte[esi],'('
	jnz	.TMPL_not_matched
	push	esi
	inc	esi
	call	ARM_calculate_expression
	pop	edx
	xchg	esi,edx
	cmp	word[edi+8],0				;reg2, reg1
	jz	.TMPL_not_matched
	xchg	esi,edx
	mov	eax,[edi+16]
	mov	[symbol_identifier],eax
	mov	al,[edi+12]
	mov	[value_type],al
	mov	eax,[edi]
	mov	edx,[edi+4]
	mov	[immediate_value],eax
	mov	ecx,[edi+8]				;mult2, mult1, reg2, reg1
	and	ecx,0xffff0f0f
	xor	ecx,0x00010000
	cmp	ecx,0x10
	jae	.TMPL_invalid_expression
	movzx	eax,[current_reg_number]
	mov	[operand_register0+eax],cl
	inc	[current_reg_number]
	inc	[current_parameter]
	movzx	ecx,byte[edi+13]			;sign bit
	and	ecx,1
	or	ecx,edx
	jz	.TMPL_okay
	cdq
	cmp	edx,[edi+4]
	jnz	.TMPL_expression.out_of_range
	xor	dl,[edi+13]				;check sign
	test	dl,1
	jz	.TMPL_okay
    .TMPL_expression.out_of_range:
	mov	ecx,ERROR_value_out_of_range
	call	ARM_defer_error
	jmp	.TMPL_okay
    .TMPL_invalid_expression:
	mov	ecx,invalid_expression
	call	ARM_defer_error
	jmp	.TMPL_okay
    .TMPL_option:
	cmp	byte[esi],'{'
	jnz	.TMPL_not_matched
	cmp	byte[esi+1],'#'
	setz	dl
	movzx	edx,dl
	lea	edx,[esi+edx+1]
	cmp	byte[edx],'('
	jnz	.TMPL_not_matched
	push	esi
	lea	esi,[edx+1]
	call	ARM_calculate_expression
	mov	edx,esi
	pop	esi
	cmp	word[edi+8],0				;reg2, reg1
	jnz	.TMPL_not_matched
	inc	edx
	cmp	byte[edx-1],'}'
	jnz	.TMPL_not_matched
	mov	esi,edx
	mov	eax,[edi+16]
	mov	[symbol_identifier],eax
	mov	al,[edi+12]
	mov	[value_type],al
	mov	eax,[edi]
	mov	edx,[edi+4]
	mov	[immediate_value],eax
	inc	[current_parameter]
	test	edx,edx
	jnz	.TMPL_option.out_of_range
	test	byte[edi+13],1				;check sign
	jz	.TMPL_okay
    .TMPL_option.out_of_range:
	mov	ecx,ERROR_value_out_of_range
	call	ARM_defer_error
	jmp	.TMPL_okay
    .TMPL_comma:
	cmp	byte[esi],','
	jnz	.TMPL_not_matched
	inc	esi
	jmp	.TMPL_okay

ARM_code16_directive:
	call	ARM_generic_mode_checks
	cmp	[explicit_IT_state],0
	jnz	ERROR_mode_change_inside_IT_block
	mov	[current_IT_block],0
	mov	[potential_IT_anchor],0
	mov	[code_type],CPU_ACTIVITY_THUMB_NON_UAL
	jmp	instruction_assembled

ARM_thumb_directive:
	call	ARM_generic_mode_checks
	cmp	[explicit_IT_state],0
	jnz	ERROR_mode_change_inside_IT_block
	mov	[current_IT_block],0
	mov	[potential_IT_anchor],0
	mov	[code_type],CPU_ACTIVITY_THUMB_UAL
	jmp	instruction_assembled

ARM_thumbee_directive:
	call	ARM_generic_mode_checks
	cmp	[explicit_IT_state],0
	jnz	ERROR_mode_change_inside_IT_block
	mov	[current_IT_block],0
	mov	[potential_IT_anchor],0
	mov	[code_type],CPU_ACTIVITY_THUMB_UAL + CPU_ACTIVITY_THUMBEE
	jmp	instruction_assembled

ARM_code32_directive:
	call	ARM_generic_mode_checks
	cmp	[explicit_IT_state],0
	jnz	ERROR_mode_change_inside_IT_block
	mov	[current_IT_block],0
	mov	[potential_IT_anchor],0
	mov	[code_type],CPU_ACTIVITY_ARM
	jmp	instruction_assembled

ARM_coprocessor_directive:
	mov	edx,COPRO_CAPABILITY_DEFAULT
	mov	ecx,copro_capability_flags
	jmp	ARM_processor_directive.do

ARM_processor_directive:
	mov	edx,CPU_CAPABILITY_DEFAULT
	mov	ecx,cpu_capability_flags
    .do:
	call	decode_template
    TEMPLATE \
	<TMPL_imm>					;0=imm
	mov	ecx,[thumb32_instruction]		;retrieve address
	mov	eax,[immediate_value]
	test	eax,eax
	setnz	dl
	movzx	edx,dl
	dec	edx
	and	edx,ebp					;mask with the default for a value of zero
	or	eax,edx
	mov	[ecx],eax
	jmp	instruction_assembled

ARM_format_directive:
	cmp	dword[esi],05018h + 0c019h shl 16	;"elf dwarf"
	jnz	format_directive
	cmp	word[esi+4],01d19h			;"executable"
	jnz	format_directive
	cmp	edi,[code_start]
	jne	unexpected_instruction
	mov	edx,[addressing_space]
	test	byte [edx+0Ah],1
	jne	unexpected_instruction
	cmp	[output_format],0
	jne	unexpected_instruction
	mov	[format_flags],5	;set to DWARF(4) + executable(1)
	mov	[output_format],5	;set to ELF
	mov	edx,edi
	mov	ecx,34h shr 2
	lea	eax,[edi+ecx*4]
	cmp	eax,[tagged_blocks]
	jae	out_of_memory
	xor	eax,eax
	rep	stosd
	mov	dword[edx],7fh + 'ELF' shl 8
	mov	al,1
	mov	[edx+4],al
	mov	[edx+5],al
	mov	[edx+6],al
	mov	byte[edx+10h],2		;e_type
	mov	byte[edx+12h],40	;machine type ARM
	mov	[edx+14h],al		;e_version
	mov	dword[edx+024h],02000016h ;e_flags
	mov	byte[edx+28h],34h	;e_ehsize
	mov	byte[edx+2ah],20h	;e_phentsize
	mov	byte[edx+2eh],28h	;e_shentsize
	mov	[code_type],ARM_code
	add	esi,6
	mov	[image_base],0
	cmp	byte[esi],80h
	jne	.base_ok
	lodsw
	cmp	ah,'('
	jne	invalid_argument
	cmp	byte[esi],'.'
	je	invalid_value
	push	edx
	call	get_dword_value
	cmp	[value_type],0
	jne	invalid_use_of_symbol
	mov	[image_base],eax
	pop	edx
    .base_ok:
	mov	ebx,edi
	mov	ecx,20h shr 2
	cmp	[current_pass],0
	je	.init_sections
	imul	ecx,[number_of_sections]
    .init_sections:
	xor	eax,eax
	rep	stosd
	mov	[number_of_sections],0
	mov	ecx,edi
	sub	ecx,[code_start]
	mov	eax,[image_base]
	mov	byte[ebx],1		;p_type
	mov	[ebx+4],ecx		;file offset
	mov	[ebx+8],eax		;p_vaddr
	mov	[ebx+0ch],eax		;p_paddr
	mov	byte[ebx+18h],7		;p_flags
	mov	word[ebx+1ch],20h	;p_align
	mov	[edx+18h],eax		;e_entry
	or	edx,-1
	mov	cl,-1
	not	eax
	add	eax,1
	adc	edx,0
	adc	cl,0
	add	eax,edi
	adc	edx,0
	mov	ebx,[addressing_space]
	movzx	ecx,cl
	mov	[ebx+0x00],eax
	mov	[ebx+0x04],edx
	mov	[ebx+0x08],ecx
	mov	dword[ebx+0x10],0
	mov	dword[ebx+0x14],0
	mov	[ebx+0x18],edi
	mov	dword[ebx+0x1c],0
	mov	[symbols_stream],edi
	jmp	instruction_assembled

ARM_section_directive:
	cmp	[output_format],5
	jnz	section_directive
	test	[format_flags],4
	jz	section_directive
	mov	eax,[addressing_space]
	test	byte[eax+0x0a],1
	jne	illegal_instruction
    ;first we close the current section
	cmp	[number_of_sections],0
	jz	.first_section
	call	ARM_finish_elf_segment
	jmp	.next_section
    .first_section:
	cmp	edi,[symbols_stream]	;has some code already been emitted?
	jnz	ERROR_code_is_not_in_a_section
	mov	eax,[image_base]
    .next_section:
	push	eax
	mov	ebx,[number_of_sections]
	shl	ebx,5
	add	ebx,[code_start]
	add	ebx,34h
	cmp	ebx,[symbols_stream]
	jb	.new_section_okay
	mov	ebx,[symbols_stream]
	sub	ebx,20h
	mov	[next_pass_needed],-1
    .new_section_okay:
	push	edi
	mov	edi,ebx
	mov	ecx,20h shr 2
	xor	eax,eax
	rep	stosd
	pop	edi
	pop	ecx
	mov	eax,edi
	sub	eax,[code_start]
	mov	[ebx+04h],eax		;p_offset
	mov	[ebx+08h],ecx		;p_vaddr
	mov	dword[ebx+0ch],0	;p_paddr
	lodsw
	cmp	ax,'('
	jne	ERROR_expecting_section_name
	mov	[ebx],esi		;we use p_type to temporarily store the string offset
	mov	ecx,[esi]
	lea	esi,[esi+4+ecx+1]
    .next_attribute:
	cmp	byte[esi],8Ch
	jz	.section_alignment
	cmp	byte[esi],80h
	jz	.section_at
	cmp	byte[esi],19h
	jnz	.section_attributes_done
	lodsw
	sub	ah,28
	jbe	invalid_argument
	cmp	ah,1
	je	.set_flag
	cmp	ah,3
	ja	invalid_argument
	xor	ah,1
	cmp	ah,2
	je	.set_flag
	inc	ah
    .set_flag:				;at least one flag required
	test	[ebx+18h],ah
	jnz	ERROR_duplicate_flag_setting
	or	[ebx+18h],ah
	jmp	.next_attribute
    .section_alignment:			;required
	inc	esi
	lodsb
	cmp	al,'('
	jne	invalid_argument
	cmp	byte[esi],'.'
	je	invalid_value
	push	ebx
	call	get_dword_value
	pop	ebx
	cmp	[value_type],0
	jne	ERROR_invalid_use_of_symbol_in_align
	mov	edx,eax
	dec	edx
	test	eax,edx
	jnz	invalid_value
	test	eax,eax
	jz	invalid_value
	xchg	[ebx+1ch],eax		;p_align
	test	eax,eax
	jnz	ERROR_duplicate_align_setting
	jmp	.next_attribute
    .section_at:			;optional
	lodsw
	cmp	ah,'('
	jne	invalid_argument
	cmp	byte[esi],'.'
	je	invalid_value
	push	ebx
	call	get_dword_value
	pop	ebx
	cmp	[value_type],0
	jne	ERROR_invalid_use_of_symbol_in_at
	mov	[ebx+0ch],eax		;p_paddr
	jmp	.next_attribute
    .section_attributes_done:
	test	dword[ebx+018h],-1	;p_flags
	jz	ERROR_section_flags_zero
	mov	ecx,[ebx+01ch]		;p_align
	sub	ecx,1
	js	ERROR_section_align_zero
	mov	eax,[ebx+0ch]		;p_paddr
	test	eax,eax
	jz	.no_at_specified
	and	eax,ecx
	jnz	ERROR_section_at_not_aligned
	mov	eax,[ebx+0ch]		;p_paddr
	jmp	.finish
    .no_at_specified:
	mov	eax,[ebx+08h]		;p_vaddr
	add	eax,ecx
	not	ecx
	and	eax,ecx
	mov	[ebx+0ch],eax		;p_paddr
    .finish:
	mov	[ebx+08h],eax		;p_vaddr
	or	edx,-1
	mov	cl,-1
	neg	eax
	cmc
	adc	edx,0
	adc	cl,0
	add	eax,edi
	adc	edx,0
	adc	cl,0
	push	eax ecx edx
	call	create_addressing_space
	pop	edx ecx eax
	mov	ebx,[addressing_space]
	mov	[ebx+0x00],eax
	mov	[ebx+0x04],edx
	mov	[ebx+0x08],cl
	inc	[number_of_sections]
	jmp	instruction_assembled

ARM_finish_elf_segment:
    ;called by close_elf after the last line is assembled
    ;we must patch it to properly close the last section
	cmp	[output_format],5
	jnz	.not_my_format
	test	[format_flags],4
	jz	.not_my_format
	mov	ebx,[number_of_sections]
	dec	ebx
	shl	ebx,5
	add	ebx,[code_start]
	add	ebx,34h
	cmp	ebx,[symbols_stream]
	jb	.exe_section_ok
	mov	ebx,[symbols_stream]
	sub	ebx,20h
    .exe_section_ok:
	mov	edx,edi			;edx=the original data end
	mov	eax,edi
	mov	ecx,[addressing_space]
	sub	eax,[ecx+0x00]		;length of code in this section
	mov	ecx,[ebx+01ch]		;align
	sub	ecx,1
	and	eax,ecx
	jz	.align_done
	not	eax
	lea	eax,[eax+ecx+2]
	add	eax,edi
	cmp	eax,[tagged_blocks]
	jae	out_of_memory
	xor	ecx,ecx
    .align:
	mov	dword[edi],ecx
	add	edi,4
	cmp	edi,eax
	jb	.align
	mov	edi,eax			;edi=the aligned data end
    .align_done:
	mov	eax,edi
	sub	eax,[code_start]	;offset into file position of emitted code on disk
	sub	eax,[ebx+4]		;length code in this section
	cmp	edx,[undefined_data_end]
	jne	.size_ok
	mov	edi,[undefined_data_start]
	mov	dword[edi],0
	mov	ecx,[addressing_space]
	sub	edi,[ecx+0x00]		;length of initialised code in this section
	add	edi,3
	and	edi,not 3
	add	edi,[ecx+0x00]
    .size_ok:
	mov	[ebx+14h],eax		;p_memsz
	mov	eax,edi
	sub	eax,[code_start]	;offset into file position of emitted code on disk
	sub	eax,[ebx+4]		;length code in this section
	mov	[ebx+10h],eax		;p_filesz
	mov	eax,[ebx+8]		;p_vaddr
	add	eax,[ebx+14h]		;p_memsz
    ;return eax = desired p_vaddr for next section
	ret
    .not_my_format:
	mov	ebx,[code_start]
	mov	dword[ebx+024h],0202h	;e_flags
	finish_elf_exe_patch_code
	jmp	finish_elf_segment+finish_elf_exe_patch_length

ARM_sections_added=7

ARM_close_dwarf:
;this is called after the assembler has finished
	cmp	[output_format],3	;pe
	jz	ARM_close_pe
	cmp	[output_format],5	;elf
	jnz	.ret
	test	[format_flags],4
	jz	.ret
	mov	ebx,[code_start]
	mov	eax,edi
	sub	eax,ebx
	mov	[ebx+020h],eax		;e_shoff
	movzx	eax,word[ebx+02ch]	;e_phmnum
	add	eax,ARM_sections_added	;we add some sections
	mov	[ebx+30h],ax		;e_shnum
	lea	ecx,[eax*8]
	sub	esp,ecx
	mov	ebp,esp
    ;build the .shstrtab table
	mov	edx,edi
	sub	edx,[code_start]
	mov	[ebp+eax*8+4-ARM_sections_added*8],edx
	xor	al,al
	stosb
	xor	ecx,ecx
    .next_code_section:
	cmp	cx,[ebx+02ch]		;e_phmnum
	jae	.code_section_names_done
	mov	edx,ecx
	shl	edx,5
	add	edx,[ebx+01ch]		;e_phoff
	add	edx,[code_start]
	mov	eax,1
	xchg	eax,[edx]		;p_type
	cmp	eax,1
	jnz	.code_section_name_found
	mov	eax,ARM_string_noname_section
    .code_section_name_found:
	mov	edx,[edx+04h]		;p_offset
	mov	[ebp+ecx*8+4],edx
	call	.add_string_to_shstrtab
	add	ecx,1
	jmp	.next_code_section
    .code_section_names_done:
	lea	eax,[ecx+1]
	mov	word[ebx+032h],ax	;e_shstrndx
	mov	eax,ARM_string_shstrtab
	call	.add_string_to_shstrtab
	add	ecx,1
	mov	eax,ARM_string_debug_abbrev
	call	.add_string_to_shstrtab
	add	ecx,1
	mov	eax,ARM_string_debug_info
	call	.add_string_to_shstrtab
	add	ecx,1
	mov	eax,ARM_string_debug_line
	call	.add_string_to_shstrtab
	add	ecx,1
	mov	eax,ARM_string_symtab
	call	.add_string_to_shstrtab
	add	ecx,1
	mov	eax,ARM_string_strtab
	call	.add_string_to_shstrtab
	mov	dword[edi],0
	sub	edi,[code_start]
	add	edi,3
	and	edi,not 3
	add	edi,[code_start]
	sub	ecx,ARM_sections_added-3
    ;build the .debug_abbrev table
	mov	edx,edi
	sub	edx,[code_start]
	mov	[ebp+ecx*8+4],edx
	mov	esi,ARM_data_debug_abbrev
	mov	edx,ecx
	mov	ecx,ARM_data_debug_abbrev_len
	lea	eax,[edi+ecx+010h]
	cmp	eax,[tagged_blocks]
	jae	out_of_memory
	rep	movsb
	mov	dword[edi],0
	sub	edi,[code_start]
	add	edi,3
	and	edi,not 3
	add	edi,[code_start]
	lea	ecx,[edx+1]
    ;build the .debug_info table
	mov	edx,edi
	sub	edx,[code_start]
	mov	[ebp+ecx*8+4],edx
	mov	esi,ARM_data_debug_info
	mov	edx,ecx
	mov	ecx,ARM_data_debug_info_len
	lea	eax,[edi+ecx+010h]
	cmp	eax,[tagged_blocks]
	jae	out_of_memory
	rep	movsb
	call	.get_top_level_file_name
	mov	ecx,esi
	test	byte[esi],-1
	jz	.top_level_file_done
    .top_level_file_loop:
	add	esi,1
	test	byte[esi],-1
	jnz	.top_level_file_loop
    .top_level_file_done:
	sub	esi,ecx
	xchg	esi,ecx
	lea	eax,[edi+ecx+010h]
	cmp	eax,[tagged_blocks]
	jae	out_of_memory
	rep	movsb
	xor	al,al
	stosb
	mov	esi,ARM_data_debug_producer
	mov	ecx,ARM_data_debug_producer_len
	lea	eax,[edi+ecx+020h]
	cmp	eax,[tagged_blocks]
	jae	out_of_memory
	rep	movsb
	mov	eax,[image_base]
	stosd		;low_pc
	mov	eax,-1
	stosd		;high_pc
	xor	eax,eax
	stosd		;stmt_list
	stosb
	mov	dword[edi],0
	sub	edi,[code_start]
	add	edi,3
	and	edi,not 3
	mov	eax,edi
	lea	ecx,[edx+1]
	sub	eax,[ebp+ecx*8+4-8]
	add	edi,[code_start]
	lea	edx,[eax-4]
	neg	eax
	mov	[edi+eax],edx
    ;build the .debug_line table
	mov	edx,edi
	sub	edx,[code_start]
	mov	[ebp+ecx*8+4],edx
	push	ecx
	mov	esi,ARM_data_debug_line_prologue
	mov	ecx,ARM_data_debug_line_prologue_len
	lea	eax,[edi+ecx+010h]
	cmp	eax,[tagged_blocks]
	jae	out_of_memory
	rep	movsb
	push	ebp
	call	ARM_debug_add_file_names
	pop	ebp
	mov	eax,[esp]
	mov	eax,[ebp+eax*8+4]
	add	eax,[code_start]
	mov	ebx,edi
	sub	ebx,eax
	sub	ebx,10
	mov	[eax+6],ebx
	push	ebp
	call	ARM_debug_add_line_info
	pop	ebp
	pop	ecx
	sub	edi,[code_start]
	mov	eax,edi
	add	ecx,1
	sub	eax,[ebp+ecx*8+4-8]
	add	edi,[code_start]
	lea	edx,[eax-4]
	neg	eax
	mov	[edi+eax],edx
	mov	dword[edi],0
	sub	edi,[code_start]
	add	edi,3
	and	edi,not 3
	add	edi,[code_start]
    ;build the .symtab table
	mov	edx,edi
	sub	edx,[code_start]
	mov	[ebp+ecx*8+4],edx
	push	ecx ebp
	lea	eax,[edi+20h]
	cmp	eax,[tagged_blocks]
	jae	out_of_memory
	xor	eax,eax
	stosd
	stosd
	stosd
	stosd
	call	ARM_make_code_identifier_table
	call	ARM_make_symbol_symtab
	pop	ebp ecx
	mov	dword[edi],0
	sub	edi,[code_start]
	add	edi,3
	and	edi,not 3
	add	edi,[code_start]
	add	ecx,1
    ;build the .strtab table
	mov	edx,edi
	sub	edx,[code_start]
	mov	[ebp+ecx*8+4],edx
	push	ecx
	call	ARM_make_symbol_strtab
	pop	ecx
	mov	dword[edi],0
	sub	edi,[code_start]
	add	edi,3
	and	edi,not 3
	add	edi,[code_start]
	add	ecx,1
	mov	ebx,[code_start]
	mov	edx,edi
	sub	edx,ebx
	mov	[ebp+ecx*8+4],edx
	mov	[ebx+020h],edx		;e_shoff
    ;zero the new section headers
	movzx	ecx,word[ebx+30h]	;e_shnum
	imul	ecx,28h shr 2
	lea	edx,[ecx*4-28h]		;skip the null section header
	lea	eax,[edi+ecx+10h]
	cmp	eax,[tagged_blocks]
	jae	out_of_memory
	xor	eax,eax
	rep	stosd
	sub	edi,edx
    ;place_the code section headers
	xor	ecx,ecx
    .next_code_placement:
	cmp	cx,[ebx+02ch]		;e_phmnum
	jae	.code_section_placement_done
	mov	edx,ecx
	shl	edx,5
	add	edx,[ebx+01ch]		;e_phoff
	add	edx,[code_start]
	mov	eax,[ebp+ecx*8]
	mov	[edi+00h],eax		;sh_name
	mov	byte[edi+04h],1		;sh_type
	mov	eax,[edx+018h]		;p_flags
	test	al,2			;test write flag
	setnz	ah
	and	al,1
	shl	al,2
	or	al,ah
	or	al,2
	movzx	eax,al
	mov	[edi+08h],eax		;sh_flags
	mov	eax,[edx+08h]		;p_vaddr
	mov	[edi+0ch],eax		;sh_addr
	mov	eax,[edx+04h]		;p_offset
	mov	[edi+010h],eax		;sh_offset
	mov	eax,[edx+010h]		;p_filesz
	mov	[edi+014h],eax		;sh_size
	mov	eax,[edx+01ch]		;p_align
	mov	[edi+020h],eax		;sh_addralign
	add	edi,028h
	add	ecx,1
	jmp	.next_code_placement
    .code_section_placement_done:
    ;place the .shstrtab section header
	mov	dl,3
	call	.make_section_header
    ;place the .debug_abbrev section header
	mov	dl,1
	call	.make_section_header
    ;place the .debug_info section header
	mov	dl,1
	call	.make_section_header
    ;place the .debug_line section header
	mov	dl,1
	call	.make_section_header
    ;place the .symtab section header
	mov	dl,2
	call	.make_section_header
	mov	[edi-28h+018h],ecx		;sh_link
	mov	byte[edi-28h+01ch],0ch		;sh_info
	mov	byte[edi-28h+020h],04h		;sh_addralign
	mov	byte[edi-28h+024h],10h		;sh_size
    ;place the .strtab section header
	mov	dl,3
	call	.make_section_header
    .done:
	movzx	eax,word[ebx+02ch]	;e_phmnum
	add	eax,ARM_sections_added
	lea	esp,[ebp+eax*8]
    .ret:
	ret
    .add_string_to_shstrtab:
    ;ecx=section number
    ;eax=pointer to string
	push	ecx
	mov	edx,edi
	sub	edx,[code_start]
	sub	edx,[ebx+020h]		;s_shoff
	mov	[ebp+ecx*8],edx		;store the string offset
	mov	ecx,[eax]
	lea	esi,[edi+ecx+010h]
	cmp	esi,[tagged_blocks]
	jae	out_of_memory
	lea	esi,[eax+4]
	rep	movsb
	xor	al,al
	stosb
	pop	ecx
	ret
    .make_section_header:
	mov	eax,[ebp+ecx*8]
	mov	[edi+00h],eax		;sh_name
	mov	[edi+04h],dl		;sh_type
	mov	eax,[ebp+ecx*8+4]
	mov	dword[edi+010h],eax	;sh_offset
	sub	eax,[ebp+ecx*8+4+8]
	neg	eax
	mov	[edi+014h],eax		;sh_size
	add	edi,028h
	add	ecx,1
	ret
    .get_top_level_file_name:
	mov	esi,[source_start]
	mov	esi,[esi+1]
	mov	esi,[esi]
	ret

ARM_close_pe:
	mov	ebx,[code_start]	;ebx points to PE header
  ;calculate size of code
	movzx	eax,word[ebx+0x14]
	lea	edx,[ebx+eax+0x18]	;edx points to section headers
	xor	eax,eax
	xor	ecx,ecx
	cmp	cx,[ebx+6]
	jae	.code_size_known
    .code_size_loop:
	test	dword[edx+0x24],0x20
	jz	.next_code_section
	add	eax,[edx+0x10]
	cmp	dword[ebx+0x18+20],0
	jnz	.next_code_section
	mov	esi,[edx+0xc]		;get virtual address
	mov	[ebx+0x18+20],esi	;update base of code
    .next_code_section:
	add	edx,0x28
	inc	ecx
	cmp	cx,[ebx+6]
	jb	.code_size_loop
    .code_size_known:
	mov	[ebx+0x18+4],eax	;update the size of code
  ;calculate size of initialised data
	movzx	eax,word[ebx+0x14]
	lea	edx,[ebx+eax+0x18]	;edx points to section headers
	xor	eax,eax
	xor	ecx,ecx
	cmp	cx,[ebx+6]
	jae	.initialised_size_known
    .initialised_size_loop:
	test	dword[edx+0x24],0x40
	jz	.next_initialised_section
	add	eax,[edx+0x10]
	cmp	dword[ebx+0x18+24],0
	jnz	.next_initialised_section
	mov	esi,[edx+0xc]		;get virtual address
	mov	[ebx+0x18+24],esi	;update base of data
    .next_initialised_section:
	add	edx,0x28
	inc	ecx
	cmp	cx,[ebx+6]
	jb	.initialised_size_loop
    .initialised_size_known:
	mov	[ebx+0x18+8],eax	;update the size of initialised data
  ;calculate size of uninitialised data
	movzx	eax,word[ebx+0x14]
	lea	edx,[ebx+eax+0x18]	;edx points to section headers
	xor	eax,eax
	xor	ecx,ecx
	cmp	cx,[ebx+6]
	jae	.uninitialised_size_known
    .uninitialised_size_loop:
	test	dword[edx+0x24],0x80
	jz	.next_uninitialised_section
	add	eax,[edx+0x8]
	sub	eax,[edx+0x10]
    .next_uninitialised_section:
	add	edx,0x28
	inc	ecx
	cmp	cx,[ebx+6]
	jb	.uninitialised_size_loop
    .uninitialised_size_known:
	mov	[ebx+0x18+8],eax	;update the size of uninitialised data
  ;update relocs stripped flag
	cmp	dword[ebx+0x78+5*8+4],0
	jz	.checksum
	and	word[ebx+0x16],not 1
    .checksum:
	mov	ecx,0x10
	mov	dword[ebx+0x58],0
	jmp	directory_ok		;recompute checksum and return

ARM_start_line_processing:
	add	edi,0fh
	and	edi,not 0fh
	cmp	edi,[tagged_blocks]
	jae	out_of_memory
	mov	eax,[labels_list]
	mov	[tagged_blocks],eax
	mov	eax,[additional_memory]
	mov	[free_additional_memory],eax
	mov	eax,[additional_memory_end]
	mov	[structures_buffer],eax
	mov	esi,[source_start]
	mov	[code_start],edi
	xor	eax,eax
	mov	dword [adjustment],eax
	mov	dword [adjustment+4],eax
	mov	[addressing_space],eax
	mov	[error_line],eax
	mov	[counter],eax
	mov	[format_flags],eax
	mov	[number_of_relocations],eax
	mov	[undefined_data_end],eax
	mov	[file_extension],eax
	mov	[next_pass_needed],al
	mov	[output_format],al
	mov	[adjustment_sign],al
	mov	[code_type],ARM_code
	call    init_addressing_space
	inc	[current_pass]
	ret

ARM_debug_add_file_names:
    ;file names are compared by string and not address,
    ;this is because it is possible to include a file more than once,
    ;then we would have duplicated entries in the table
	push	[code_start]
	push	edi
	call	ARM_start_line_processing
	mov	ebp,esp
	push	-1	;total files
	push	[input_file]
    .line_loop:
	cmp	byte[esi],0Fh
	jne	.next_line
	mov	eax,[esi+1]
	test	byte[eax+7],80h
	jnz	.next_line
	mov	eax,[eax]
	mov	ebx,[ebp-4]
	test	ebx,ebx
	jz	.add_file
    .file_loop:
	mov	ecx,[ebp-4+ebx*4]
	cmp	eax,ecx
	jz	.next_line
    .char_loop:
	mov	dl,[eax]
	mov	dh,[ecx]
	add	eax,1
	add	ecx,1
	cmp	dh,dl
	jnz	.next_file
	test	dl,dl
	jnz	.char_loop
	jmp	.next_line
    .next_file:
	mov	eax,[esi+1]
	mov	eax,[eax]
	add	ebx,1
	jnz	.file_loop
    .add_file:
	mov	ebx,[ebp-4]
	sub	ebx,1
	mov	[ebp-4],ebx
	mov	eax,[esi+1]
	mov	eax,[eax]
	push	eax
    .next_line:
	push	ebp
	call	assemble_line
	pop	ebp
	jnc	.line_loop
	mov	edi,[ebp]
	xor	edx,edx
	cmp	edx,[ebp-4]
	jle	.done
    .copy_loop:
	mov	esi,[ebp-8+edx*4]
	mov	ecx,[input_file]
	cmp	ecx,esi
	jnz	.not_top_level_source
	test	byte[esi],-1
	jz	.top_level_file_done
    .top_level_file_loop:
	add	esi,1
	test	byte[esi],-1
	jnz	.top_level_file_loop
    .top_level_file_done:
	sub	esi,ecx
	xchg	esi,ecx
	jmp	.copy
    .not_top_level_source:
	movzx	ecx,byte[esi-1]
	sub	ecx,1
	jns	.copy
	mov	ecx,dword[esi-4]
    .copy:
	lea	eax,[edi+ecx+010h]
	cmp	eax,[tagged_blocks]
	jae	out_of_memory
	rep	movsb
	xor	eax,eax
	stosd
	sub	edx,1
	cmp	edx,[ebp-4]
	jg	.copy_loop
    .done:
	xor	al,al
	stosb
	mov	[ebp],edi
	mov	esp,ebp
	pop	edi
	pop	[code_start]
	ret

ARM_debug_add_line_info:
	mov	ecx,[code_start]
	mov	ecx,[ecx+38h]
	push	ecx		;+12 offset to first program byte
	push	[code_start]	;+8 real generated code
	push	edi		;+4 current output position
	add	eax,ARM_data_debug_line_prologue_len
	push	eax		;+0 file name buffer
	call	ARM_start_line_processing
	mov	ebp,esp
	push	-1		;-4 current file
	push	1		;-8 current line
	push	-1		;-12 current address
	push	0		;-16 address of last line processed
	push	[code_start]	;-20 last physical code address
    .line_loop:
	cmp	byte[esi],0Fh
	jne	.skip_line
	mov	eax,[esi+1]
	test	byte[eax+7],80h
	jnz	.skip_line
	mov	ecx,[addressing_space]
	test	byte[ecx+0x0a],1
	jnz	.skip_line
	mov	ecx,[addressing_space]
	mov	ecx,[ecx+0x00]
	sub	ecx,edi
	neg	ecx
	mov	eax,[esi+1]
	mov	[eax+12],ecx
	call	.emit_line_data
	mov	[ebp-16],esi
	mov	[ebp-20],edi
    .skip_line:
	push	ebp
	call	assemble_line
	pop	ebp
	jnc	.line_loop
	call	.emit_line_data
	mov	edx,[ebp-16]
	mov	ecx,[addressing_space]
	mov	ecx,[ecx+0x00]
	sub	ecx,edi
	neg	ecx
	mov	eax,[edx+1]
	mov	[eax+12],ecx
	inc	dword[eax+4]
	inc	edi
	call	.emit_line_data
	dec	edi
	mov	edx,[ebp-16]
	mov	eax,[edx+1]
	dec	dword[eax+4]
    .close:
	xchg	edi,[ebp+4]
	mov	eax,010100h
	mov	[edi],eax
	add	edi,3
	xchg	edi,[ebp+4]
	call	close_pass
	lea	esp,[ebp+4]
	pop	edi
	pop	[code_start]
	pop	eax
	ret
    .emit_leb128:
	bsr	ecx,edx
	jnz	.leb_calcu
	or	ecx,-1
    .leb_calcu:
	add	ecx,1
	jmp	.leb_do
    .emit_signed_leb:
    ;edx is the value to emit
	test	edx,edx
	jns	.leb_pos
	not	edx
	bsr	ecx,edx
	not	edx
	jmp	.leb_bits
    .leb_pos:
	bsr	ecx,edx
    .leb_bits:
	jnz	.leb_calc
	or	ecx,-1
    .leb_calc:
	add	ecx,2
    .leb_do:
	xchg	edi,[ebp+4]
    .next_leb128:
	mov	eax,edx
	and	eax,07fh
	shr	edx,7
	sub	ecx,7
	jle	.last_leb128
	or	al,080h
	stosb
	jmp	.next_leb128
    .last_leb128:
	stosb
	xchg	edi,[ebp+4]
	ret
    .emit_line_data:
	mov	eax,[ebp+4]
	lea	eax,[eax+20h]
	cmp	eax,[tagged_blocks]
	jae	out_of_memory
	cmp	edi,[code_start]
	jz	.ret
    ;any data yet?
	mov	eax,edi
	sub	eax,[code_start]
	cmp	eax,[ebp+12]
	jbe	.ret
	cmp	edi,[ebp-20]	;previous physical address
	jz	.ret
    .check_file:
	mov	edx,10000h
	mov	ecx,[ebp]
    .file_loop:
	mov	eax,[ebp-16]
	mov	eax,[eax+1]
	mov	eax,[eax]
    .char_loop:
	mov	dl,[eax]
	mov	dh,[ecx]
	add	eax,1
	add	ecx,1
	cmp	dh,dl
	jnz	.next_file
	test	dl,dl
	jnz	.char_loop
	jmp	.found_file
    .next_file:
	add	edx,10000h
	test	dh,dh
	jz	.skip_null
    .skip_loop:
	add	ecx,1
	mov	dh,[ecx]
	test	dh,dh
	jnz	.skip_loop
    .skip_null:
	add	ecx,4
	cmp	byte[ecx],0
	jnz	.file_loop
	jmp	ERROR_line_processing_error
    .found_file:
	shr	edx,16
	cmp	edx,[ebp-4]
	jz	.check_address
    ;set a new file
	mov	[ebp-4],edx
	push	edx
	mov	al,4
	xchg	edi,[ebp+4]
	stosb
	xchg	edi,[ebp+4]
	pop	edx
	call	.emit_leb128
    .check_address:
	cmp	dword[ebp-12],-1
	jz	.first_pc
	mov	edx,[ebp-16]
	mov	edx,[edx+1]
	mov	edx,[edx+4]
	sub	edx,[ebp-8]
	jz	.ret
	jmp	.subseqeuent_pc
    .first_pc:
	xchg	edi,[ebp+4]
	mov	eax,020500h
	mov	[edi],eax
	add	edi,3
	mov	eax,[ebp-16]
	mov	eax,[eax+1]
	mov	eax,[eax+12]
	mov	[ebp-12],eax
	stosd
	xchg	edi,[ebp+4]
	jmp	.check_line
    .subseqeuent_pc:
	mov	eax,[ebp-16]
	mov	eax,[eax+1]
	mov	eax,[eax+12]
	sub	eax,[ebp-12]
	cmp	eax,40
	jbe	.check_line
	cmp	eax,80
	ja	.pc_far
	add	dword[ebp-12],40
	push	eax
	mov	al,8
	xchg	edi,[ebp+4]
	stosb
	xchg	edi,[ebp+4]
	pop	eax
	sub	eax,40
	jmp	.check_line
    .pc_far:
	add	[ebp-12],eax
	push	eax
	mov	al,2
	xchg	edi,[ebp+4]
	stosb
	xchg	edi,[ebp+4]
	pop	edx
	call	.emit_leb128
	xor	eax,eax
    .check_line:
	mov	edx,[ebp-16]
	mov	edx,[edx+1]
	mov	edx,[edx+4]
	sub	edx,[ebp-8]
	cmp	edx,6
	jb	.send_special_code
    .send_line_advance:
	push	edx
	mov	al,3
	xchg	edi,[ebp+4]
	stosb
	xchg	edi,[ebp+4]
	pop	edx
	call	.emit_signed_leb
	mov	eax,[ebp-16]
	mov	eax,[eax+1]
	mov	edx,[eax+4]
	mov	[ebp-8],edx
	xor	edx,edx
	mov	eax,[eax+12]
	sub	eax,[ebp-12]
	jnz	.send_special_code
    .copy:
	mov	al,1
	xchg	edi,[ebp+4]
	stosb
	xchg	edi,[ebp+4]
	ret
    .send_special_code:
	add	[ebp-12],eax
	imul	eax,6
	add	eax,edx
	jz	.copy
	add	eax,10
	xchg	edi,[ebp+4]
	stosb
	xchg	edi,[ebp+4]
	mov	edx,[ebp-16]
	mov	edx,[edx+1]
	mov	edx,[edx+4]
	mov	[ebp-8],edx
    .ret:
	ret

ARM_string_arm_code_offset	= 1
ARM_string_thumb_code_offset	= 1+ARM_string_arm_code_offset+ARM_string_arm_code_identifier_len
ARM_string_data_offset		= 1+ARM_string_thumb_code_offset+ARM_string_thumb_code_identifier_len
ARM_string_thumbee_code_offset	= 1+ARM_string_data_offset+ARM_string_data_identifier_len

ARM_make_symbol_strtab:
	mov	ebx,[code_start]
	xor	al,al
	stosb
	mov	esi,ARM_string_arm_code_identifier
	mov	ecx,ARM_string_arm_code_identifier_len
	rep	movsb
	stosb
	mov	esi,ARM_string_thumb_code_identifier
	mov	ecx,ARM_string_thumb_code_identifier_len
	rep	movsb
	stosb
	mov	esi,ARM_string_data_identifier
	mov	ecx,ARM_string_data_identifier_len
	rep	movsb
	stosb
	mov	esi,ARM_string_thumbee_code_identifier
	mov	ecx,ARM_string_thumbee_code_identifier_len
	rep	movsb
	stosb
	mov	esi,[ebx-4]		;esi=source of label data
	mov	ebx,[ebx-8]		;ebx=last address+1
	cmp	esi,ebx
	jae	.ret
    .loop:
	movzx	ecx,byte[esi]
	lea	eax,[esi+1]
	mov	edx,[esi+ecx+1]
	lea	esi,[esi+ecx+5]
	test	dword[edx+8],0100h
	jz	.skip
	xchg	esi,eax
	lea	edx,[edi+ecx+10h]
	cmp	edx,[tagged_blocks]
	jae	out_of_memory
	rep	movsb
	xchg	esi,eax
	xor	al,al
	stosb
    .skip:
	cmp	esi,ebx
	jb	.loop
    .ret:
	ret

ARM_make_symbol_symtab:
	mov	ebx,[code_start]
	mov	esi,[ebx-4]		;esi=source of label data
	mov	ebx,[ebx-8]		;ebx=last address+1
	cmp	esi,ebx
	jae	.ret
	push	ebp
	mov	ebp,1+ARM_string_arm_code_identifier_len+\
		    1+ARM_string_thumb_code_identifier_len+\
		    1+ARM_string_data_identifier_len+\
		    1+ARM_string_thumbee_code_identifier_len+\
		    1
    .loop:
	movzx	ecx,byte[esi]
	mov	edx,[esi+ecx+1]
	lea	esi,[esi+ecx+5]
	test	dword[edx+8],0100h
	jz	.skip
	lea	eax,[edi+20h]
	cmp	eax,[tagged_blocks]
	jae	out_of_memory
	mov	[edi],ebp		;name
	lea	ebp,[ebp+ecx+1]
	mov	eax,[edx]
	mov	[edi+4],eax		;value
	movzx	eax,byte[edx+10]
	mov	[edi+8],eax		;size
	test	eax,eax
	setz	al
	movzx	eax,al
	add	eax,1
	mov	[edi+12],ax		;binding type
	mov	eax,[edx]
    ;find the section associated with this label
	mov	ecx,[code_start]
	movzx	edx,word[ecx+02ch]	;e_phnum
	add	edx,1
    .next_section:
	sub	edx,1
	je	.no_section
	lea	ecx,[edx-1]
	shl	ecx,5
	add	ecx,[code_start]
	cmp	eax,[ecx+034h+8]	;p_vaddr
	jb	.next_section
	sub	eax,[ecx+034h+8]	;p_vaddr
	cmp	eax,[ecx+034h+14h]	;p_memsz
	jb	.section_found
	add	eax,[ecx+034h+8]	;p_vaddr
	jmp	.next_section
    .no_section:
	xor	edx,edx
    .section_found:
	mov	word[edi+14],dx		;section
	add	edi,16
    .skip:
	cmp	esi,ebx
	jb	.loop
	pop	ebp
    .ret:
	ret

ARM_make_code_identifier_table:
	mov	ecx,[code_start]
	mov	ecx,[ecx+38h]
	push	ecx		;+8 offset to first program byte
	push	[code_start]
	push	edi		;+0 current output position
	call	ARM_start_line_processing
	mov	ebp,esp
	push	-1	;-4 last code type
	push	-1	;-8 this code type
	push	-1	;-12 this code address
    .line_loop:
	mov	edx,esi
	cmp	byte[edx],0Fh
	jnz	.check_data
	add	edx,5
    .check_data:
    ;any data yet?
	mov	eax,edi
	sub	eax,[code_start]
	cmp	eax,[ebp+8]
	jb	.skip_line
    .find_line:
	mov	al,[edx]
	cmp	al,1
	jz	.do_line
	cmp	al,2
	jz	.do_value
	cmp	al,3
	jnz	.skip_line
    .do_value:
	add	edx,6
	jmp	.find_line
    .do_line:
	mov	ecx,[addressing_space]
	test	byte[ecx+0x0a],1
	jnz	.skip_line
	movzx	eax,word[edx+1]
	mov	ecx,080h	;data code type
	cmp	eax,ARM_code_generator_start-instruction_handler
	jb	.found_generator
	movzx	ecx,[code_type]
    .found_generator:
	mov	[ebp-8],ecx
	mov	eax,[addressing_space]
	mov	eax,[eax+0x00]
	sub	eax,edi
	neg	eax
	mov	[ebp-12],eax
	push	ebp
	call	assemble_line
	pop	ebp
	lahf
	mov	ecx,[ebp-8]
	cmp	ecx,[ebp-4]
	jz	.next_line
	mov	edx,[addressing_space]
	test	byte[edx+0x0a],1
	jnz	.next_line
	push	eax
	mov	eax,[addressing_space]
	mov	eax,[eax+0x00]
	sub	eax,edi
	neg	eax
	cmp	eax,[ebp-12]
	jz	.non_code_line
	xchg	edi,[ebp]
	mov	[ebp-4],ecx
	lea	edx,[edi+20h]
	cmp	edx,[tagged_blocks]
	jae	out_of_memory
	mov	edx,ARM_string_data_offset
	test	cl,cl
	js	.found_generator2
	mov	edx,ARM_string_arm_code_offset
	test	ecx,CPU_ACTIVITY_ARM
	jnz	.found_generator2
	mov	edx,ARM_string_thumb_code_offset
	test	ecx,CPU_ACTIVITY_THUMBEE
	jz	.found_generator2
	mov	edx,ARM_string_thumbee_code_offset
    .found_generator2:
	mov	eax,[ebp-12]
	mov	[edi],edx		;name
	mov	[edi+4],eax		;address
	mov	dword[edi+8],0		;length
	mov	ecx,2
	cmp	edx,ARM_string_data_offset
	jnz	@f
	mov	ecx,1
    @@:	mov	[edi+12],cx		;binding type
    ;find the section associated with this address
	mov	ecx,[ebp+4]
	movzx	edx,word[ecx+02ch]	;e_phnum
	add	edx,1
    .next_section:
	sub	edx,1
	je	.no_section
	lea	ecx,[edx-1]
	shl	ecx,5
	add	ecx,[ebp+4]
	cmp	eax,[ecx+034h+8]	;p_vaddr
	jb	.next_section
	sub	eax,[ecx+034h+8]	;p_vaddr
	cmp	eax,[ecx+034h+14h]	;p_memsz
	jb	.section_found
	add	eax,[ecx+034h+8]	;p_vaddr
	jmp	.next_section
    .no_section:
	xor	edx,edx
    .section_found:
	mov	word[edi+14],dx		;section
	add	edi,16
	xchg	edi,[ebp]
    .non_code_line:
	pop	eax
    .next_line:
	sahf
	jmp	.finish_line
    .skip_line:
	push	ebp
	call	assemble_line
	pop	ebp
    .finish_line:
	jnc	.line_loop
	mov	ecx,[ebp-8]
	test	cl,cl
	js	.done
	mov	eax,[addressing_space]
	mov	eax,[eax+0x00]
	sub	eax,edi
	neg	eax
	xchg	edi,[ebp]
	mov	edx,ARM_string_data_offset
	mov	[edi],edx		;name
	mov	[edi+4],eax		;address
	mov	dword[edi+8],0		;length
	mov	word[edi+12],1		;binding type
	mov	word[edi+14],0		;section
	add	edi,16
	xchg	edi,[ebp]
    .done:
	mov	esp,ebp
	call	close_pass
	pop	edi
	pop	[code_start]
	pop	eax
	ret

ARM_label_walker:
	mov	edi,[code_start]
	add	edi,4
	push	edi
	call	.walk
	pop	esi
	mov	ebx,edi
	add	edi,0fh+8
	and	edi,not 0fh
	mov	[code_start],edi
	mov	[edi-4],esi
	mov	[edi-8],ebx
	mov	[esi-4],eax
	ret
    .walk:
	mov	edx,32+1
	mov	ebx,hash_tree
	xor	eax,eax
	test	dword[ebx],-1
	jz	.done
    .recur:
	mov	ebx,[ebx]
	sub	edx,1
	jz	.store
	test	dword[ebx],-1
	jz	.right
	push	ebx
	call	.recur
	pop	ebx
    .right:
	add	ebx,4
	test	dword[ebx],-1
	jz	.done
	call	.recur
    .done:
	add	edx,1
	ret
    .store:
	add	eax,1		;bump the label count
	mov	esi,[ebx+4]
	mov	esi,[esi+18h]	;string text name
	movzx	ecx,byte[esi-1]	;name length
	mov	[edi],cl
	add	edi,1
	lea	esi,[edi+ecx+20h]
	cmp	esi,[labels_list]
	jae	out_of_memory
	mov	esi,[ebx+4]
	mov	esi,[esi+18h]	;string text name
	rep	movsb
	lea	esi,[ebx+4]	;offset to 24 byte data field
	movsd
	mov	ebx,[ebx]	;next link?
	test	ebx,ebx
	jnz	.store
	jmp	.done

ARM_org_directive:
	cmp	[output_format],5
	jnz	org_directive
	test	[format_flags],4
	jz	org_directive
	jmp	ERROR_org_not_allowed

ARM_data_debug_abbrev:
    ;this is the debug_abbrev table, it is the minimum requirement to make a complete table for AXD
	db	1	;abbrev code
	db	11h	;tag_compile_unit
	db	1	;has children
	db	3,8	;name,string
	db	25h,8	;producer,string
	db	11h,1	;low_pc,address
	db	12h,1	;high_pc,address
	db	10h,6	;stmt_list,data4
	db	0	;terminate child
	db	0	;no siblings
ARM_data_debug_abbrev_len=$-ARM_data_debug_abbrev

ARM_data_debug_producer:
	db	'ARMv7 assembler core v1.31 for flat assembler v',VERSION_STRING,' by revolution',0
ARM_data_debug_producer_len=$-ARM_data_debug_producer

ARM_data_debug_info:
	dd	0	;length
	dw	2	;version
	dd	0	;offset to abbrev table
	db	4	;address size
	db	1	;abbrev code
ARM_data_debug_info_len=$-ARM_data_debug_info

ARM_data_debug_line_prologue:
    ;this defines the layout of the line numbers table
	dd	0	;total length
	dw	2	;version
	dd	0	;prologue length
	db	1	;minimum instruction length
	db	1	;default is statment
	db	0	;line base
	db	6	;line range
	db	10	;opcode base
	db	0,1,1,1,1,0,0,0,0	;standard opcode lengths
	db	0	;no include directories
ARM_data_debug_line_prologue_len=$-ARM_data_debug_line_prologue

ARM_string_arm_code_identifier:
	db	'$a'
	ARM_string_arm_code_identifier_len=$-ARM_string_arm_code_identifier
ARM_string_thumb_code_identifier:
	db	'$t'
	ARM_string_thumb_code_identifier_len=$-ARM_string_thumb_code_identifier
ARM_string_data_identifier:
	db	'$d'
	ARM_string_data_identifier_len=$-ARM_string_data_identifier
ARM_string_thumbee_code_identifier:
	db	'$t.x'
	ARM_string_thumbee_code_identifier_len=$-ARM_string_thumbee_code_identifier
ARM_string_shstrtab:
	dd	ARM_string_shstrtab_len
	db	'.shstrtab'
	ARM_string_shstrtab_len=$-ARM_string_shstrtab-4
ARM_string_debug_abbrev:
	dd	ARM_string_debug_abbrev_len
	db	'.debug_abbrev'
	ARM_string_debug_abbrev_len=$-ARM_string_debug_abbrev-4
ARM_string_debug_info:
	dd	ARM_string_debug_info_len
	db	'.debug_info'
	ARM_string_debug_info_len=$-ARM_string_debug_info-4
ARM_string_debug_line:
	dd	ARM_string_debug_line_len
	db	'.debug_line'
	ARM_string_debug_line_len=$-ARM_string_debug_line-4
ARM_string_symtab:
	dd	ARM_string_symtab_len
	db	'.symtab'
	ARM_string_symtab_len=$-ARM_string_symtab-4
ARM_string_strtab:
	dd	ARM_string_strtab_len
	db	'.strtab'
	ARM_string_strtab_len=$-ARM_string_strtab-4
ARM_string_noname_section:
	dd	ARM_string_noname_section_len
	db	'.flat'
	ARM_string_noname_section_len=$-ARM_string_noname_section-4

ARM_code_generator_start:

;ARM/THUMB jump table

ARM_instruction_adc:
	mov	edx,ARM_INSTRUCTION_OPCODE_ADC shl 21
	mov	ecx,0xe shl 28 + 0xa shl 21
	jmp	ARM_rd_rn_shifter
ARM_instruction_add:
	mov	edx,ARM_INSTRUCTION_OPCODE_ADD shl 21
	mov	ecx,0xe shl 28 + 0x8 shl 21
	jmp	ARM_rd_rn_shifter
ARM_instruction_addw:
	mov	edx,ARM_INSTRUCTION_OPCODE_ADD shl 21
	mov	ecx,0xf2000000
	jmp	THUMB_rd_rn_imm12
ARM_instruction_adr:
	mov	edx,ARM_INSTRUCTION_OPCODE_ADD shl 21
	jmp	ARM_rd_target
ARM_instruction_and:
	mov	edx,ARM_INSTRUCTION_OPCODE_AND shl 21
	mov	ecx,0xe shl 28 + 0x0 shl 21
	jmp	ARM_rd_rn_shifter
ARM_instruction_asr:
	mov	edx,ARM_INSTRUCTION_OPCODE_MOV shl 21 + ARM_SHIFT_OPCODE_ASR shl 5
	jmp	ARM_rd_rn_shift
ARM_instruction_b:
	mov	edx,0x0a000000
	jmp	ARM_target
ARM_instruction_bfc:
	mov	edx,0x07c0001f
	mov	ecx,0xf36f0000
	jmp	ARM_rd_imm_imm
ARM_instruction_bfi:
	mov	edx,0x07c00010
	mov	ecx,0xf3600000
	jmp	ARM_rd_rn_imm_imm
ARM_instruction_bic:
	mov	edx,ARM_INSTRUCTION_OPCODE_BIC shl 21
	mov	ecx,0xe shl 28 + 0x1 shl 21
	jmp	ARM_rd_rn_shifter
ARM_instruction_bkpt:
	mov	edx,0x01200070
	jmp	ARM_immediate16
ARM_instruction_bl:
	mov	edx,0x0b000000
	jmp	ARM_target
ARM_instruction_blx:
	mov	edx,0					;instruction varies depending upon parameters
	jmp	ARM_Xtarget
ARM_instruction_bx:
	mov	edx,0x012fff10
	jmp	ARM_rm
ARM_instruction_bxj:
	mov	edx,0x012fff20
	mov	ecx,0xf3c08f00
	jmp	ARM_rm_J
ARM_instruction_cbnz:
	mov	ecx,0xb900
	jmp	THUMB_reg_address
ARM_instruction_cbz:
	mov	ecx,0xb100
	jmp	THUMB_reg_address
ARM_instruction_cdp:
	mov	edx,0x0e000000
	jmp	ARM_copro_op1_crd_crn_crm_op2
ARM_instruction_cdp2:
	mov	edx,0xfe000000
	jmp	ARM_copro_op1_crd_crn_crm_op2
ARM_instruction_chka:
	jmp	THUMBEE_rn_rm
ARM_instruction_clrex:
	mov	edx,0xf57ff01f
	mov	ecx,0xf3bf8f2f
	jmp	ARM_clrex
ARM_instruction_clz:
	mov	edx,0x016f0f10
	mov	ecx,0xfab0f080
	jmp	ARM_rd_rm_CLZ
ARM_instruction_cmn:
	mov	edx,ARM_INSTRUCTION_OPCODE_CMN shl 21 + 1 shl 20
	mov	ecx,0xe shl 28 + 0x8 shl 21 + 1 shl 20 + 0xf shl 8
	jmp	ARM_rn_shifter
ARM_instruction_cmnp:
	mov	edx,ARM_INSTRUCTION_OPCODE_CMN shl 21 + 1 shl 20 + 0xf shl 12
	jmp	ARM_rn_shifter_26bit
ARM_instruction_cmp:
	mov	edx,ARM_INSTRUCTION_OPCODE_CMP shl 21 + 1 shl 20
	mov	ecx,0xe shl 28 + 0xd shl 21 + 1 shl 20 + 0xf shl 8
	jmp	ARM_rn_shifter
ARM_instruction_cmpp:
	mov	edx,ARM_INSTRUCTION_OPCODE_CMP shl 21 + 1 shl 20 + 0xf shl 12
	jmp	ARM_rn_shifter_26bit
ARM_instruction_cps:
	mov	edx,0xf1020000
	mov	ecx,0xf3af8100
	jmp	ARM_mode
ARM_instruction_cpsid:
	mov	edx,0xf10c0000
	mov	ecx,0xf3af8600
	jmp	ARM_iflags_mode
ARM_instruction_cpsie:
	mov	edx,0xf1080000
	mov	ecx,0xf3af8400
	jmp	ARM_iflags_mode
ARM_instruction_cpy:
	mov	edx,ARM_INSTRUCTION_OPCODE_MOV shl 21
	mov	ecx,0xe shl 28 + 0x2 shl 21 + 0xf shl 16
	jmp	ARM_rd_rm_CPY
ARM_instruction_dbg:
	mov	edx,0x0320f0f0
	mov	ecx,0xf3af80f0
	jmp	ARM_imm4
ARM_instruction_dmb:
	mov	edx,0xf57ff050
	mov	ecx,0xf3bf8f50
	jmp	ARM_barrier
ARM_instruction_dsb:
	mov	edx,0xf57ff040
	mov	ecx,0xf3bf8f40
	jmp	ARM_barrier
ARM_instruction_enterx:
	mov	ecx,0xf3bf8f1f
	jmp	THUMBEE_enterx
ARM_instruction_eor:
	mov	edx,ARM_INSTRUCTION_OPCODE_EOR shl 21
	mov	ecx,0xe shl 28 + 0x4 shl 21
	jmp	ARM_rd_rn_shifter
ARM_instruction_hb:
	mov	edx,0x0000c200
	jmp	THUMBEE_imm
ARM_instruction_hbl:
	mov	edx,0x0000c300
	jmp	THUMBEE_imm
ARM_instruction_hblp:
	mov	edx,0x0000c400
	jmp	THUMBEE_imm5_imm5
ARM_instruction_hbp:
	mov	edx,0x0000c000
	jmp	THUMBEE_imm3_imm5
ARM_instruction_isb:
	mov	edx,0xf57ff060
	mov	ecx,0xf3bf8f60
	jmp	ARM_barrier
ARM_instruction_it:
	mov	ecx,0xbf08
	jmp	THUMB_condition
ARM_instruction_ite:
	mov	ecx,0xbf0c
	jmp	THUMB_condition
ARM_instruction_itee:
	mov	ecx,0xbf0e
	jmp	THUMB_condition
ARM_instruction_iteee:
	mov	ecx,0xbf0f
	jmp	THUMB_condition
ARM_instruction_iteet:
	mov	ecx,0xbf0d
	jmp	THUMB_condition
ARM_instruction_itet:
	mov	ecx,0xbf0a
	jmp	THUMB_condition
ARM_instruction_itete:
	mov	ecx,0xbf0b
	jmp	THUMB_condition
ARM_instruction_itett:
	mov	ecx,0xbf09
	jmp	THUMB_condition
ARM_instruction_itt:
	mov	ecx,0xbf04
	jmp	THUMB_condition
ARM_instruction_itte:
	mov	ecx,0xbf06
	jmp	THUMB_condition
ARM_instruction_ittee:
	mov	ecx,0xbf07
	jmp	THUMB_condition
ARM_instruction_ittet:
	mov	ecx,0xbf05
	jmp	THUMB_condition
ARM_instruction_ittt:
	mov	ecx,0xbf02
	jmp	THUMB_condition
ARM_instruction_ittte:
	mov	ecx,0xbf03
	jmp	THUMB_condition
ARM_instruction_itttt:
	mov	ecx,0xbf01
	jmp	THUMB_condition
ARM_instruction_ldc:
	mov	edx,0x0c100000
	jmp	ARM_copro_crd_address5
ARM_instruction_ldc2:
	mov	edx,0xfc100000
	jmp	ARM_copro_crd_address5
ARM_instruction_ldc2l:
	mov	edx,0xfc500000
	jmp	ARM_copro_crd_address5
ARM_instruction_ldcl:
	mov	edx,0x0c500000
	jmp	ARM_copro_crd_address5
ARM_instruction_ldm:
ARM_instruction_ldmia:
ARM_instruction_ldmfd:
	mov	edx,0x08900000
	mov	ecx,0xe8900000
	jmp	ARM_rn_address4
ARM_instruction_ldmda:
ARM_instruction_ldmfa:
	mov	edx,0x08100000
	jmp	ARM_rn_address4
ARM_instruction_ldmdb:
ARM_instruction_ldmea:
	mov	edx,0x09100000
	mov	ecx,0xe9100000
	jmp	ARM_rn_address4
ARM_instruction_ldmed:
ARM_instruction_ldmib:
	mov	edx,0x09900000
	jmp	ARM_rn_address4
ARM_instruction_ldr:
	mov	[operand_size],4
	mov	edx,0x04100000
	mov	ecx,0xf8500000
	jmp	ARM_rd_address2
ARM_instruction_ldrb:
	mov	[operand_size],1
	mov	edx,0x04500000
	mov	ecx,0xf8100000
	jmp	ARM_rd_address2
ARM_instruction_ldrbt:
	mov	[operand_size],1
	mov	edx,0x04700000
	mov	ecx,0xf8100e00
	jmp	ARM_rd_address2_post
ARM_instruction_ldrd:
	mov	[operand_size],8
	mov	edx,0x000000d0
	mov	ecx,0xe8500000
	jmp	ARM_rd_address3D
ARM_instruction_ldrex:
	mov	[operand_size],4
	mov	edx,0x01900f9f
	mov	ecx,0xe8500f00
	jmp	ARM_rd_q_rn_p
ARM_instruction_ldrexb:
	mov	[operand_size],1
	mov	edx,0x01d00f9f
	mov	ecx,0xe8d00f4f
	jmp	ARM_rd_q_rn_p
ARM_instruction_ldrexd:
	mov	[operand_size],8
	mov	edx,0x01b00f9f
	mov	ecx,0xe8d0007f
	jmp	ARM_rt_rt2_q_rn_p
ARM_instruction_ldrexh:
	mov	[operand_size],2
	mov	edx,0x01f00f9f
	mov	ecx,0xe8d00f5f
	jmp	ARM_rd_q_rn_p
ARM_instruction_ldrh:
	mov	[operand_size],2
	mov	edx,0x001000b0
	mov	ecx,0xf8300000
	jmp	ARM_rd_address3
ARM_instruction_ldrht:
	mov	[operand_size],2
	mov	edx,0x003000b0
	mov	ecx,0xf8300e00
	jmp	ARM_rd_address3T
ARM_instruction_ldrsb:
	mov	[operand_size],1
	mov	edx,0x001000d0
	mov	ecx,0xf9100000
	jmp	ARM_rd_address3
ARM_instruction_ldrsbt:
	mov	[operand_size],1
	mov	edx,0x003000d0
	mov	ecx,0xf9100e00
	jmp	ARM_rd_address3T
ARM_instruction_ldrsh:
	mov	[operand_size],2
	mov	edx,0x001000f0
	mov	ecx,0xf9300000
	jmp	ARM_rd_address3
ARM_instruction_ldrsht:
	mov	[operand_size],2
	mov	edx,0x003000f0
	mov	ecx,0xf9300e00
	jmp	ARM_rd_address3T
ARM_instruction_ldrt:
	mov	[operand_size],4
	mov	edx,0x04300000
	mov	ecx,0xf8500e00
	jmp	ARM_rd_address2_post
ARM_instruction_leavex:
	mov	ecx,0xf3bf8f0f
	jmp	THUMBEE_enterx
ARM_instruction_lsl:
	mov	edx,ARM_INSTRUCTION_OPCODE_MOV shl 21 + ARM_SHIFT_OPCODE_LSL shl 5
	jmp	ARM_rd_rn_shift
ARM_instruction_lsr:
	mov	edx,ARM_INSTRUCTION_OPCODE_MOV shl 21 + ARM_SHIFT_OPCODE_LSR shl 5
	jmp	ARM_rd_rn_shift
ARM_instruction_mcr:
	mov	edx,0x0e000010
	jmp	ARM_copro_op1_rd_crn_crm_op2
ARM_instruction_mcr2:
	mov	edx,0xfe000010
	jmp	ARM_copro_op1_rd_crn_crm_op2
ARM_instruction_mcrr:
	mov	edx,0x0c400000
	jmp	ARM_copro_op1_rd_rn_crm
ARM_instruction_mcrr2:
	mov	edx,0xfc400000
	jmp	ARM_copro_op1_rd_rn_crm
ARM_instruction_mla:
	mov	edx,0x00200090
	mov	ecx,0xfb000000
	jmp	ARM_rd_rm_rs_rn
ARM_instruction_mls:
	mov	edx,0x00600090
	mov	ecx,0xfb000010
	jmp	ARM_rd_rm_rs_rn
ARM_instruction_mov:
	mov	edx,ARM_INSTRUCTION_OPCODE_MOV shl 21
	mov	ecx,0xe shl 28 + 0x2 shl 21 + 0xf shl 16
	jmp	ARM_rd_shifter_exp
ARM_instruction_movt:
	mov	edx,0x03400000
	mov	ecx,0xf2c00000
	jmp	ARM_rd_imm16
ARM_instruction_movw:
	mov	edx,0x03000000
	mov	ecx,0xf2400000
	jmp	ARM_rd_imm16
ARM_instruction_mrc:
	mov	edx,0x0e100010
	jmp	ARM_copro_op1_rd_crn_crm_op2
ARM_instruction_mrc2:
	mov	edx,0xfe100010
	jmp	ARM_copro_op1_rd_crn_crm_op2
ARM_instruction_mrrc:
	mov	edx,0x0c500000
	jmp	ARM_copro_op1_rd_rn_crm
ARM_instruction_mrrc2:
	mov	edx,0xfc500000
	jmp	ARM_copro_op1_rd_rn_crm
ARM_instruction_mrs:
	mov	edx,0x010f0000
	mov	ecx,0xf3ef8000
	jmp	ARM_rd_psr
ARM_instruction_msr:
	mov	edx,0x0120f000
	mov	ecx,0xf3808000
	jmp	ARM_psr_value
ARM_instruction_mul:
	mov	edx,0x00000090
	mov	ecx,0xfb00f000
	jmp	ARM_rd_rm_rs
ARM_instruction_mvn:
	mov	edx,ARM_INSTRUCTION_OPCODE_MVN shl 21
	mov	ecx,0xe shl 28 + 0x3 shl 21 + 0xf shl 16
	jmp	ARM_rd_shifter
ARM_instruction_neg:
	mov	edx,ARM_INSTRUCTION_OPCODE_RSB shl 21
	mov	ecx,0xe shl 28 + 0xe shl 21
	jmp	ARM_rd_rm
ARM_instruction_nop:
	mov	edx,0x0320f000
	jmp	ARM_nop
ARM_instruction_orn:
	mov	ecx,0xe shl 28 + 0x3 shl 21
	jmp	THUMB_rd_rn_shifter_ORN
ARM_instruction_orr:
	mov	edx,ARM_INSTRUCTION_OPCODE_ORR shl 21
	mov	ecx,0xe shl 28 + 0x2 shl 21
	jmp	ARM_rd_rn_shifter
ARM_instruction_pkhbt:
	mov	edx,0x06800010
	mov	ecx,0xeac00000
	jmp	ARM_rd_rn_rm_shift_imm
ARM_instruction_pkhtb:
	mov	edx,0x06800050
	mov	ecx,0xeac00020
	jmp	ARM_rd_rn_rm_shift_imm
ARM_instruction_pld:
	mov	[operand_size],1
	mov	edx,0x04500000
	mov	ecx,0xf810f000
	jmp	ARM_address2
ARM_instruction_pldw:
	mov	[operand_size],1
	mov	edx,0x04100000
	mov	ecx,0xf830f000
	jmp	ARM_address2
ARM_instruction_pli:
	mov	[operand_size],1
	mov	edx,0x05500000
	mov	ecx,0xf910f000
	jmp	ARM_address2
ARM_instruction_pop:
	mov	edx,0x08900000
	mov	ecx,0xe8900000
	jmp	ARM_address4
ARM_instruction_push:
	mov	edx,0x09000000
	mov	ecx,0xe9000000
	jmp	ARM_address4
ARM_instruction_qadd:
	mov	edx,0x01000050
	mov	ecx,0xfa80f080
	jmp	ARM_rd_rm_rn
ARM_instruction_qadd16:
	mov	edx,0x06200f10
	mov	ecx,0xfa90f010
	jmp	ARM_rd_rn_rm
ARM_instruction_qadd8:
	mov	edx,0x06200f90
	mov	ecx,0xfa80f010
	jmp	ARM_rd_rn_rm
ARM_instruction_qaddsubx:
ARM_instruction_qasx:
	mov	edx,0x06200f30
	mov	ecx,0xfaa0f010
	jmp	ARM_rd_rn_rm
ARM_instruction_qdadd:
	mov	edx,0x01400050
	mov	ecx,0xfa80f090
	jmp	ARM_rd_rm_rn
ARM_instruction_qdsub:
	mov	edx,0x01600050
	mov	ecx,0xfa80f0b0
	jmp	ARM_rd_rm_rn
ARM_instruction_qsub:
	mov	edx,0x01200050
	mov	ecx,0xfa80f0a0
	jmp	ARM_rd_rm_rn
ARM_instruction_qsub16:
	mov	edx,0x06200f70
	mov	ecx,0xfad0f010
	jmp	ARM_rd_rn_rm
ARM_instruction_qsub8:
	mov	edx,0x06200ff0
	mov	ecx,0xfac0f010
	jmp	ARM_rd_rn_rm
ARM_instruction_qsubaddx:
ARM_instruction_qsax:
	mov	edx,0x06200f50
	mov	ecx,0xfae0f010
	jmp	ARM_rd_rn_rm
ARM_instruction_rbit:
	mov	edx,0x06ff0f30
	mov	ecx,0xfa90f0a0
	jmp	ARM_rd_rm_REV
ARM_instruction_rev:
	mov	edx,0x06bf0f30
	mov	ecx,0xfa90f080
	jmp	ARM_rd_rm_REV
ARM_instruction_rev16:
	mov	edx,0x06bf0fb0
	mov	ecx,0xfa90f090
	jmp	ARM_rd_rm_REV
ARM_instruction_revsh:
	mov	edx,0x06ff0fb0
	mov	ecx,0xfa90f0b0
	jmp	ARM_rd_rm_REV
ARM_instruction_rfe:
ARM_instruction_rfeia:
ARM_instruction_rfefd:
	mov	edx,0xf8900a00
	mov	ecx,0xe990c000
	jmp	ARM_rn
ARM_instruction_rfeda:
ARM_instruction_rfefa:
	mov	edx,0xf8100a00
	jmp	ARM_rn
ARM_instruction_rfedb:
ARM_instruction_rfeea:
	mov	edx,0xf9100a00
	mov	ecx,0xe810c000
	jmp	ARM_rn
ARM_instruction_rfeib:
ARM_instruction_rfeed:
	mov	edx,0xf9900a00
	jmp	ARM_rn
ARM_instruction_ror:
	mov	edx,ARM_INSTRUCTION_OPCODE_MOV shl 21 + ARM_SHIFT_OPCODE_ROR shl 5
	jmp	ARM_rd_rn_shift
ARM_instruction_rrx:
	mov	edx,ARM_INSTRUCTION_OPCODE_MOV shl 21 + ARM_SHIFT_OPCODE_ROR shl 5
	mov	ecx,0xea4f0000
	jmp	ARM_rd_shift
ARM_instruction_rsb:
	mov	edx,ARM_INSTRUCTION_OPCODE_RSB shl 21
	mov	ecx,0xe shl 28 + 0xe shl 21
	jmp	ARM_rd_rn_shifter
ARM_instruction_rsc:
	mov	edx,ARM_INSTRUCTION_OPCODE_RSC shl 21
	jmp	ARM_rd_rn_shifter
ARM_instruction_sadd16:
	mov	edx,0x06100f10
	mov	ecx,0xfa90f000
	jmp	ARM_rd_rn_rm
ARM_instruction_sadd8:
	mov	edx,0x06100f90
	mov	ecx,0xfa80f000
	jmp	ARM_rd_rn_rm
ARM_instruction_saddsubx:
ARM_instruction_sasx:
	mov	edx,0x06100f30
	mov	ecx,0xfaa0f000
	jmp	ARM_rd_rn_rm
ARM_instruction_sbc:
	mov	edx,ARM_INSTRUCTION_OPCODE_SBC shl 21
	mov	ecx,0xe shl 28 + 0xb shl 21
	jmp	ARM_rd_rn_shifter
ARM_instruction_sbfx:
	mov	edx,0x07a00050
	mov	ecx,0xf3400000
	jmp	ARM_rd_rn_imm_imm_X
ARM_instruction_sdiv:
	mov	edx,0x0710f010
	mov	ecx,0xfb90f0f0
	jmp	ARM_rd_rn_rm_DIV
ARM_instruction_sel:
	mov	edx,0x06800fb0
	mov	ecx,0xfaa0f080
	jmp	ARM_rd_rn_rm
ARM_instruction_setend:
	mov	edx,0xf1010000
	jmp	ARM_endian
ARM_instruction_sev:
	mov	edx,0x0320f004
	mov	ecx,0xf3af8004
	jmp	ARM_nops
ARM_instruction_shadd16:
	mov	edx,0x06300f10
	mov	ecx,0xfa90f020
	jmp	ARM_rd_rn_rm
ARM_instruction_shadd8:
	mov	edx,0x06300f90
	mov	ecx,0xfa80f020
	jmp	ARM_rd_rn_rm
ARM_instruction_shaddsubx:
ARM_instruction_shasx:
	mov	edx,0x06300f30
	mov	ecx,0xfaa0f020
	jmp	ARM_rd_rn_rm
ARM_instruction_shsub16:
	mov	edx,0x06300f70
	mov	ecx,0xfad0f020
	jmp	ARM_rd_rn_rm
ARM_instruction_shsub8:
	mov	edx,0x06300ff0
	mov	ecx,0xfac0f020
	jmp	ARM_rd_rn_rm
ARM_instruction_shsubaddx:
ARM_instruction_shsax:
	mov	edx,0x06300f50
	mov	ecx,0xfae0f020
	jmp	ARM_rd_rn_rm
ARM_instruction_smc:
ARM_instruction_smi:
	mov	edx,0x01600070
	mov	ecx,0xf7f08000
	jmp	ARM_immediate4
ARM_instruction_smlabb:
	mov	edx,0x01000080
	mov	ecx,0xfb100000
	jmp	ARM_rd_rm_rs_rn_E
ARM_instruction_smlabt:
	mov	edx,0x010000c0
	mov	ecx,0xfb100010
	jmp	ARM_rd_rm_rs_rn_E
ARM_instruction_smlad:
	mov	edx,0x07000010
	mov	ecx,0xfb200000
	jmp	ARM_rd_rm_rs_rn_D
ARM_instruction_smladx:
	mov	edx,0x07000030
	mov	ecx,0xfb200010
	jmp	ARM_rd_rm_rs_rn_D
ARM_instruction_smlal:
	mov	edx,0x00e00090
	mov	ecx,0xfbc00000
	jmp	ARM_rdlo_rdhi_rm_rs
ARM_instruction_smlalbb:
	mov	edx,0x01400080
	mov	ecx,0xfbc00080
	jmp	ARM_rdlo_rdhi_rm_rs_E
ARM_instruction_smlalbt:
	mov	edx,0x014000c0
	mov	ecx,0xfbc00090
	jmp	ARM_rdlo_rdhi_rm_rs_E
ARM_instruction_smlald:
	mov	edx,0x07400010
	mov	ecx,0xfbc000c0
	jmp	ARM_rdlo_rdhi_rm_rs_D
ARM_instruction_smlaldx:
	mov	edx,0x07400030
	mov	ecx,0xfbc000d0
	jmp	ARM_rdlo_rdhi_rm_rs_D
ARM_instruction_smlaltb:
	mov	edx,0x014000a0
	mov	ecx,0xfbc000a0
	jmp	ARM_rdlo_rdhi_rm_rs_E
ARM_instruction_smlaltt:
	mov	edx,0x014000e0
	mov	ecx,0xfbc000b0
	jmp	ARM_rdlo_rdhi_rm_rs_E
ARM_instruction_smlatb:
	mov	edx,0x010000a0
	mov	ecx,0xfb100020
	jmp	ARM_rd_rm_rs_rn_E
ARM_instruction_smlatt:
	mov	edx,0x010000e0
	mov	ecx,0xfb100030
	jmp	ARM_rd_rm_rs_rn_E
ARM_instruction_smlawb:
	mov	edx,0x01200080
	mov	ecx,0xfb300000
	jmp	ARM_rd_rm_rs_rn_E
ARM_instruction_smlawt:
	mov	edx,0x012000c0
	mov	ecx,0xfb300010
	jmp	ARM_rd_rm_rs_rn_E
ARM_instruction_smlsd:
	mov	edx,0x07000050
	mov	ecx,0xfb400000
	jmp	ARM_rd_rm_rs_rn_D
ARM_instruction_smlsdx:
	mov	edx,0x07000070
	mov	ecx,0xfb400010
	jmp	ARM_rd_rm_rs_rn_D
ARM_instruction_smlsld:
	mov	edx,0x07400050
	mov	ecx,0xfbd000c0
	jmp	ARM_rdlo_rdhi_rm_rs_D
ARM_instruction_smlsldx:
	mov	edx,0x07400070
	mov	ecx,0xfbd000d0
	jmp	ARM_rdlo_rdhi_rm_rs_D
ARM_instruction_smmla:
	mov	edx,0x07500010
	mov	ecx,0xfb500000
	jmp	ARM_rd_rm_rs_rn_D
ARM_instruction_smmlar:
	mov	edx,0x07500030
	mov	ecx,0xfb500010
	jmp	ARM_rd_rm_rs_rn_D
ARM_instruction_smmls:
	mov	edx,0x075000d0
	mov	ecx,0xfb600000
	jmp	ARM_rd_rm_rs_rn_D
ARM_instruction_smmlsr:
	mov	edx,0x075000f0
	mov	ecx,0xfb600010
	jmp	ARM_rd_rm_rs_rn_D
ARM_instruction_smmul:
	mov	edx,0x0750f010
	mov	ecx,0xfb50f000
	jmp	ARM_rd_rm_rs_M
ARM_instruction_smmulr:
	mov	edx,0x0750f030
	mov	ecx,0xfb50f010
	jmp	ARM_rd_rm_rs_M
ARM_instruction_smuad:
	mov	edx,0x0700f010
	mov	ecx,0xfb20f000
	jmp	ARM_rd_rm_rs_M
ARM_instruction_smuadx:
	mov	edx,0x0700f030
	mov	ecx,0xfb20f010
	jmp	ARM_rd_rm_rs_M
ARM_instruction_smulbb:
	mov	edx,0x01600080
	mov	ecx,0xfb10f000
	jmp	ARM_rd_rm_rs_E
ARM_instruction_smulbt:
	mov	edx,0x016000c0
	mov	ecx,0xfb10f010
	jmp	ARM_rd_rm_rs_E
ARM_instruction_smull:
	mov	edx,0x00c00090
	mov	ecx,0xfb800000
	jmp	ARM_rdlo_rdhi_rm_rs
ARM_instruction_smultb:
	mov	edx,0x016000a0
	mov	ecx,0xfb10f020
	jmp	ARM_rd_rm_rs_E
ARM_instruction_smultt:
	mov	edx,0x016000e0
	mov	ecx,0xfb10f030
	jmp	ARM_rd_rm_rs_E
ARM_instruction_smulwb:
	mov	edx,0x012000a0
	mov	ecx,0xfb30f000
	jmp	ARM_rd_rm_rs_E
ARM_instruction_smulwt:
	mov	edx,0x012000e0
	mov	ecx,0xfb30f010
	jmp	ARM_rd_rm_rs_E
ARM_instruction_smusd:
	mov	edx,0x0700f050
	mov	ecx,0xfb40f000
	jmp	ARM_rd_rm_rs_M
ARM_instruction_smusdx:
	mov	edx,0x0700f070
	mov	ecx,0xfb40f010
	jmp	ARM_rd_rm_rs_M
ARM_instruction_srs:
ARM_instruction_srsia:
ARM_instruction_srsea:
	mov	edx,0xf8cd0500
	mov	ecx,0xe98dc000
	jmp	ARM_reg_mode
ARM_instruction_srsda:
ARM_instruction_srsfa:
	mov	edx,0xf84d0500
	jmp	ARM_reg_mode
ARM_instruction_srsdb:
ARM_instruction_srsfd:
	mov	edx,0xf94d0500
	mov	ecx,0xe80dc000
	jmp	ARM_reg_mode
ARM_instruction_srsib:
ARM_instruction_srsed:
	mov	edx,0xf9cd0500
	jmp	ARM_reg_mode
ARM_instruction_ssat:
	mov	edx,0x06a00010
	mov	ecx,0xf3000000
	jmp	ARM_rd_imm_rm_shift
ARM_instruction_ssat16:
	mov	edx,0x06a00f30
	mov	ecx,0xf3200000
	jmp	ARM_rd_imm_rm
ARM_instruction_ssub16:
	mov	edx,0x06100f70
	mov	ecx,0xfad0f000
	jmp	ARM_rd_rn_rm
ARM_instruction_ssub8:
	mov	edx,0x06100ff0
	mov	ecx,0xfac0f000
	jmp	ARM_rd_rn_rm
ARM_instruction_ssubaddx:
ARM_instruction_ssax:
	mov	edx,0x06100f50
	mov	ecx,0xfae0f000
	jmp	ARM_rd_rn_rm
ARM_instruction_stc:
	mov	edx,0x0c000000
	jmp	ARM_copro_crd_address5
ARM_instruction_stc2:
	mov	edx,0xfc000000
	jmp	ARM_copro_crd_address5
ARM_instruction_stc2l:
	mov	edx,0xfc400000
	jmp	ARM_copro_crd_address5
ARM_instruction_stcl:
	mov	edx,0x0c400000
	jmp	ARM_copro_crd_address5
ARM_instruction_stm:
ARM_instruction_stmia:
ARM_instruction_stmea:
	mov	edx,0x08800000
	mov	ecx,0xe8800000
	jmp	ARM_rn_address4
ARM_instruction_stmda:
ARM_instruction_stmed:
	mov	edx,0x08000000
	jmp	ARM_rn_address4
ARM_instruction_stmdb:
ARM_instruction_stmfd:
	mov	edx,0x09000000
	mov	ecx,0xe9000000
	jmp	ARM_rn_address4
ARM_instruction_stmib:
ARM_instruction_stmfa:
	mov	edx,0x09800000
	jmp	ARM_rn_address4
ARM_instruction_str:
	mov	[operand_size],4
	mov	edx,0x04000000
	mov	ecx,0xf8400000
	jmp	ARM_rd_address2
ARM_instruction_strb:
	mov	[operand_size],1
	mov	edx,0x04400000
	mov	ecx,0xf8000000
	jmp	ARM_rd_address2
ARM_instruction_strbt:
	mov	[operand_size],1
	mov	edx,0x04600000
	mov	ecx,0xf8000e00
	jmp	ARM_rd_address2_post
ARM_instruction_strd:
	mov	[operand_size],8
	mov	edx,0x000000f0
	mov	ecx,0xe8400000
	jmp	ARM_rd_address3D
ARM_instruction_strex:
	mov	[operand_size],4
	mov	edx,0x01800f90
	mov	ecx,0xe8400000
	jmp	ARM_rd_rm_q_rn_p_STREX
ARM_instruction_strexb:
	mov	[operand_size],1
	mov	edx,0x01c00f90
	mov	ecx,0xe8c00f40
	jmp	ARM_rd_rm_q_rn_p_STREX
ARM_instruction_strexd:
	mov	[operand_size],8
	mov	edx,0x01a00f90
	mov	ecx,0xe8c00070
	jmp	ARM_rd_rt_rt2_q_rn_p
ARM_instruction_strexh:
	mov	[operand_size],2
	mov	edx,0x01e00f90
	mov	ecx,0xe8c00f50
	jmp	ARM_rd_rm_q_rn_p_STREX
ARM_instruction_strh:
	mov	[operand_size],2
	mov	edx,0x000000b0
	mov	ecx,0xf8200000
	jmp	ARM_rd_address3
ARM_instruction_strht:
	mov	[operand_size],2
	mov	edx,0x002000b0
	mov	ecx,0xf8200e00
	jmp	ARM_rd_address3T
ARM_instruction_strt:
	mov	[operand_size],4
	mov	edx,0x04200000
	mov	ecx,0xf8400e00
	jmp	ARM_rd_address2_post
ARM_instruction_sub:
	mov	edx,ARM_INSTRUCTION_OPCODE_SUB shl 21
	mov	ecx,0xe shl 28 + 0xd shl 21
	jmp	ARM_rd_rn_shifter
ARM_instruction_subw:
	mov	edx,ARM_INSTRUCTION_OPCODE_SUB shl 21
	mov	ecx,0xf2000000 + 5 shl 21
	jmp	THUMB_rd_rn_imm12
ARM_instruction_svc:
ARM_instruction_swi:
	mov	edx,0x0f000000
	jmp	ARM_immediate24
ARM_instruction_swp:
	mov	edx,0x01000090
	jmp	ARM_rd_rm_q_rn_p
ARM_instruction_swpb:
	mov	edx,0x01400090
	jmp	ARM_rd_rm_q_rn_p
ARM_instruction_sxtab:
	mov	edx,0x06a00070
	mov	ecx,0xfa40f080
	jmp	ARM_rd_rn_rm_rotation
ARM_instruction_sxtab16:
	mov	edx,0x06800070
	mov	ecx,0xfa20f080
	jmp	ARM_rd_rn_rm_rotation
ARM_instruction_sxtah:
	mov	edx,0x06b00070
	mov	ecx,0xfa00f080
	jmp	ARM_rd_rn_rm_rotation
ARM_instruction_sxtb:
	mov	edx,0x06af0070
	mov	ecx,0xfa4ff080
	jmp	ARM_rd_rm_rotation
ARM_instruction_sxtb16:
	mov	edx,0x068f0070
	mov	ecx,0xfa2ff080
	jmp	ARM_rd_rm_rotation
ARM_instruction_sxth:
	mov	edx,0x06bf0070
	mov	ecx,0xfa0ff080
	jmp	ARM_rd_rm_rotation
ARM_instruction_tbb:
	mov	ecx,0xe8d0f000
	jmp	THUMB_q_rn_rm_p
ARM_instruction_tbh:
	mov	ecx,0xe8d0f010
	jmp	THUMB_q_rn_rm_lsl_1_p
ARM_instruction_teq:
	mov	edx,ARM_INSTRUCTION_OPCODE_TEQ shl 21 + 1 shl 20
	mov	ecx,0xe shl 28 + 0x4 shl 21 + 1 shl 20 + 0xf shl 8
	jmp	ARM_rn_shifter
ARM_instruction_teqp:
	mov	edx,ARM_INSTRUCTION_OPCODE_TEQ shl 21 + 1 shl 20 + 0xf shl 12
	jmp	ARM_rn_shifter_26bit
ARM_instruction_tst:
	mov	edx,ARM_INSTRUCTION_OPCODE_TST shl 21 + 1 shl 20
	mov	ecx,0xe shl 28 + 0x0 shl 21 + 1 shl 20 + 0xf shl 8
	jmp	ARM_rn_shifter
ARM_instruction_tstp:
	mov	edx,ARM_INSTRUCTION_OPCODE_TST shl 21 + 1 shl 20 + 0xf shl 12
	jmp	ARM_rn_shifter_26bit
ARM_instruction_uadd16:
	mov	edx,0x06500f10
	mov	ecx,0xfa90f040
	jmp	ARM_rd_rn_rm
ARM_instruction_uadd8:
	mov	edx,0x06500f90
	mov	ecx,0xfa80f040
	jmp	ARM_rd_rn_rm
ARM_instruction_uaddsubx:
ARM_instruction_uasx:
	mov	edx,0x06500f30
	mov	ecx,0xfaa0f040
	jmp	ARM_rd_rn_rm
ARM_instruction_ubfx:
	mov	edx,0x07e00050
	mov	ecx,0xf3c00000
	jmp	ARM_rd_rn_imm_imm_X
ARM_instruction_udiv:
	mov	edx,0x0730f010
	mov	ecx,0xfbb0f0f0
	jmp	ARM_rd_rn_rm_DIV
ARM_instruction_uhadd16:
	mov	edx,0x06700f10
	mov	ecx,0xfa90f060
	jmp	ARM_rd_rn_rm
ARM_instruction_uhadd8:
	mov	edx,0x06700f90
	mov	ecx,0xfa80f060
	jmp	ARM_rd_rn_rm
ARM_instruction_uhaddsubx:
ARM_instruction_uhasx:
	mov	edx,0x06700f30
	mov	ecx,0xfaa0f060
	jmp	ARM_rd_rn_rm
ARM_instruction_uhsub16:
	mov	edx,0x06700f70
	mov	ecx,0xfad0f060
	jmp	ARM_rd_rn_rm
ARM_instruction_uhsub8:
	mov	edx,0x06700ff0
	mov	ecx,0xfac0f060
	jmp	ARM_rd_rn_rm
ARM_instruction_uhsubaddx:
ARM_instruction_uhsax:
	mov	edx,0x06700f50
	mov	ecx,0xfae0f060
	jmp	ARM_rd_rn_rm
ARM_instruction_umaal:
	mov	edx,0x00400090
	mov	ecx,0xfbe00060
	jmp	ARM_rdlo_rdhi_rm_rs
ARM_instruction_umlal:
	mov	edx,0x00a00090
	mov	ecx,0xfbe00000
	jmp	ARM_rdlo_rdhi_rm_rs
ARM_instruction_umull:
	mov	edx,0x00800090
	mov	ecx,0xfba00000
	jmp	ARM_rdlo_rdhi_rm_rs
ARM_instruction_und:
	mov	edx,0x07f000f0
	mov	ecx,0xf7f0a0f0
	jmp	ARM_und
ARM_instruction_uqadd16:
	mov	edx,0x06600f10
	mov	ecx,0xfa90f050
	jmp	ARM_rd_rn_rm
ARM_instruction_uqadd8:
	mov	edx,0x06600f90
	mov	ecx,0xfa80f050
	jmp	ARM_rd_rn_rm
ARM_instruction_uqaddsubx:
ARM_instruction_uqasx:
	mov	edx,0x06600f30
	mov	ecx,0xfaa0f050
	jmp	ARM_rd_rn_rm
ARM_instruction_uqsub16:
	mov	edx,0x06600f70
	mov	ecx,0xfad0f050
	jmp	ARM_rd_rn_rm
ARM_instruction_uqsub8:
	mov	edx,0x06600ff0
	mov	ecx,0xfac0f050
	jmp	ARM_rd_rn_rm
ARM_instruction_uqsubaddx:
ARM_instruction_uqsax:
	mov	edx,0x06600f50
	mov	ecx,0xfae0f050
	jmp	ARM_rd_rn_rm
ARM_instruction_usad8:
	mov	edx,0x0780f010
	mov	ecx,0xfb70f000
	jmp	ARM_rd_rm_rs_SAD
ARM_instruction_usada8:
	mov	edx,0x07800010
	mov	ecx,0xfb700000
	jmp	ARM_rd_rm_rs_rn_D
ARM_instruction_usat:
	mov	edx,0x06e00010
	mov	ecx,0xf3800000
	jmp	ARM_rd_imm_rm_shift
ARM_instruction_usat16:
	mov	edx,0x06e00f30
	mov	ecx,0xf3a00000
	jmp	ARM_rd_imm_rm
ARM_instruction_usub16:
	mov	edx,0x06500f70
	mov	ecx,0xfad0f040
	jmp	ARM_rd_rn_rm
ARM_instruction_usub8:
	mov	edx,0x06500ff0
	mov	ecx,0xfac0f040
	jmp	ARM_rd_rn_rm
ARM_instruction_usubaddx:
ARM_instruction_usax:
	mov	edx,0x06500f50
	mov	ecx,0xfae0f040
	jmp	ARM_rd_rn_rm
ARM_instruction_uxtab:
	mov	edx,0x06e00070
	mov	ecx,0xfa50f080
	jmp	ARM_rd_rn_rm_rotation
ARM_instruction_uxtab16:
	mov	edx,0x06c00070
	mov	ecx,0xfa30f080
	jmp	ARM_rd_rn_rm_rotation
ARM_instruction_uxtah:
	mov	edx,0x06f00070
	mov	ecx,0xfa10f080
	jmp	ARM_rd_rn_rm_rotation
ARM_instruction_uxtb:
	mov	edx,0x06ef0070
	mov	ecx,0xfa5ff080
	jmp	ARM_rd_rm_rotation
ARM_instruction_uxtb16:
	mov	edx,0x06cf0070
	mov	ecx,0xfa3ff080
	jmp	ARM_rd_rm_rotation
ARM_instruction_uxth:
	mov	edx,0x06ff0070
	mov	ecx,0xfa1ff080
	jmp	ARM_rd_rm_rotation
ARM_instruction_wfe:
	mov	edx,0x0320f002
	mov	ecx,0xf3af8002
	jmp	ARM_nops
ARM_instruction_wfi:
	mov	edx,0x0320f003
	mov	ecx,0xf3af8003
	jmp	ARM_nops
ARM_instruction_yield:
	mov	edx,0x0320f001
	mov	ecx,0xf3af8001
	jmp	ARM_nops

;FPA jump table

ARM_instruction_absd:
	mov	edx,0x0e208180
	jmp	FPA_fd_fm
ARM_instruction_absdm:
	mov	edx,0x0e2081c0
	jmp	FPA_fd_fm
ARM_instruction_absdp:
	mov	edx,0x0e2081a0
	jmp	FPA_fd_fm
ARM_instruction_absdz:
	mov	edx,0x0e2081e0
	jmp	FPA_fd_fm
ARM_instruction_abse:
	mov	edx,0x0e288100
	jmp	FPA_fd_fm
ARM_instruction_absem:
	mov	edx,0x0e288140
	jmp	FPA_fd_fm
ARM_instruction_absep:
	mov	edx,0x0e288120
	jmp	FPA_fd_fm
ARM_instruction_absez:
	mov	edx,0x0e288160
	jmp	FPA_fd_fm
ARM_instruction_abss:
	mov	edx,0x0e208100
	jmp	FPA_fd_fm
ARM_instruction_abssm:
	mov	edx,0x0e208140
	jmp	FPA_fd_fm
ARM_instruction_abssp:
	mov	edx,0x0e208120
	jmp	FPA_fd_fm
ARM_instruction_abssz:
	mov	edx,0x0e208160
	jmp	FPA_fd_fm
ARM_instruction_acsd:
	mov	edx,0x0ec08180
	jmp	FPA_fd_fm
ARM_instruction_acsdm:
	mov	edx,0x0ec081c0
	jmp	FPA_fd_fm
ARM_instruction_acsdp:
	mov	edx,0x0ec081a0
	jmp	FPA_fd_fm
ARM_instruction_acsdz:
	mov	edx,0x0ec081e0
	jmp	FPA_fd_fm
ARM_instruction_acse:
	mov	edx,0x0ec88100
	jmp	FPA_fd_fm
ARM_instruction_acsem:
	mov	edx,0x0ec88140
	jmp	FPA_fd_fm
ARM_instruction_acsep:
	mov	edx,0x0ec88120
	jmp	FPA_fd_fm
ARM_instruction_acsez:
	mov	edx,0x0ec88160
	jmp	FPA_fd_fm
ARM_instruction_acss:
	mov	edx,0x0ec08100
	jmp	FPA_fd_fm
ARM_instruction_acssm:
	mov	edx,0x0ec08140
	jmp	FPA_fd_fm
ARM_instruction_acssp:
	mov	edx,0x0ec08120
	jmp	FPA_fd_fm
ARM_instruction_acssz:
	mov	edx,0x0ec08160
	jmp	FPA_fd_fm
ARM_instruction_adfd:
	mov	edx,0x0e000180
	jmp	FPA_fd_fn_fm
ARM_instruction_adfdm:
	mov	edx,0x0e0001c0
	jmp	FPA_fd_fn_fm
ARM_instruction_adfdp:
	mov	edx,0x0e0001a0
	jmp	FPA_fd_fn_fm
ARM_instruction_adfdz:
	mov	edx,0x0e0001e0
	jmp	FPA_fd_fn_fm
ARM_instruction_adfe:
	mov	edx,0x0e080100
	jmp	FPA_fd_fn_fm
ARM_instruction_adfem:
	mov	edx,0x0e080140
	jmp	FPA_fd_fn_fm
ARM_instruction_adfep:
	mov	edx,0x0e080120
	jmp	FPA_fd_fn_fm
ARM_instruction_adfez:
	mov	edx,0x0e080160
	jmp	FPA_fd_fn_fm
ARM_instruction_adfs:
	mov	edx,0x0e000100
	jmp	FPA_fd_fn_fm
ARM_instruction_adfsm:
	mov	edx,0x0e000140
	jmp	FPA_fd_fn_fm
ARM_instruction_adfsp:
	mov	edx,0x0e000120
	jmp	FPA_fd_fn_fm
ARM_instruction_adfsz:
	mov	edx,0x0e000160
	jmp	FPA_fd_fn_fm
ARM_instruction_asnd:
	mov	edx,0x0eb08180
	jmp	FPA_fd_fm
ARM_instruction_asndm:
	mov	edx,0x0eb081c0
	jmp	FPA_fd_fm
ARM_instruction_asndp:
	mov	edx,0x0eb081a0
	jmp	FPA_fd_fm
ARM_instruction_asndz:
	mov	edx,0x0eb081e0
	jmp	FPA_fd_fm
ARM_instruction_asne:
	mov	edx,0x0eb88100
	jmp	FPA_fd_fm
ARM_instruction_asnem:
	mov	edx,0x0eb88140
	jmp	FPA_fd_fm
ARM_instruction_asnep:
	mov	edx,0x0eb88120
	jmp	FPA_fd_fm
ARM_instruction_asnez:
	mov	edx,0x0eb88160
	jmp	FPA_fd_fm
ARM_instruction_asns:
	mov	edx,0x0eb08100
	jmp	FPA_fd_fm
ARM_instruction_asnsm:
	mov	edx,0x0eb08140
	jmp	FPA_fd_fm
ARM_instruction_asnsp:
	mov	edx,0x0eb08120
	jmp	FPA_fd_fm
ARM_instruction_asnsz:
	mov	edx,0x0eb08160
	jmp	FPA_fd_fm
ARM_instruction_atnd:
	mov	edx,0x0ed08180
	jmp	FPA_fd_fm
ARM_instruction_atndm:
	mov	edx,0x0ed081c0
	jmp	FPA_fd_fm
ARM_instruction_atndp:
	mov	edx,0x0ed081a0
	jmp	FPA_fd_fm
ARM_instruction_atndz:
	mov	edx,0x0ed081e0
	jmp	FPA_fd_fm
ARM_instruction_atne:
	mov	edx,0x0ed88100
	jmp	FPA_fd_fm
ARM_instruction_atnem:
	mov	edx,0x0ed88140
	jmp	FPA_fd_fm
ARM_instruction_atnep:
	mov	edx,0x0ed88120
	jmp	FPA_fd_fm
ARM_instruction_atnez:
	mov	edx,0x0ed88160
	jmp	FPA_fd_fm
ARM_instruction_atns:
	mov	edx,0x0ed08100
	jmp	FPA_fd_fm
ARM_instruction_atnsm:
	mov	edx,0x0ed08140
	jmp	FPA_fd_fm
ARM_instruction_atnsp:
	mov	edx,0x0ed08120
	jmp	FPA_fd_fm
ARM_instruction_atnsz:
	mov	edx,0x0ed08160
	jmp	FPA_fd_fm
ARM_instruction_cmf:
	mov	edx,0x0e90f110
	jmp	FPA_fn_fm
ARM_instruction_cmfe:
	mov	edx,0x0ed0f110
	jmp	FPA_fn_fm
ARM_instruction_cnf:
	mov	edx,0x0eb0f110
	jmp	FPA_fn_fm
ARM_instruction_cnfe:
	mov	edx,0x0ef0f110
	jmp	FPA_fn_fm
ARM_instruction_cosd:
	mov	edx,0x0e908180
	jmp	FPA_fd_fm
ARM_instruction_cosdm:
	mov	edx,0x0e9081c0
	jmp	FPA_fd_fm
ARM_instruction_cosdp:
	mov	edx,0x0e9081a0
	jmp	FPA_fd_fm
ARM_instruction_cosdz:
	mov	edx,0x0e9081e0
	jmp	FPA_fd_fm
ARM_instruction_cose:
	mov	edx,0x0e988100
	jmp	FPA_fd_fm
ARM_instruction_cosem:
	mov	edx,0x0e988140
	jmp	FPA_fd_fm
ARM_instruction_cosep:
	mov	edx,0x0e988120
	jmp	FPA_fd_fm
ARM_instruction_cosez:
	mov	edx,0x0e988160
	jmp	FPA_fd_fm
ARM_instruction_coss:
	mov	edx,0x0e908100
	jmp	FPA_fd_fm
ARM_instruction_cossm:
	mov	edx,0x0e908140
	jmp	FPA_fd_fm
ARM_instruction_cossp:
	mov	edx,0x0e908120
	jmp	FPA_fd_fm
ARM_instruction_cossz:
	mov	edx,0x0e908160
	jmp	FPA_fd_fm
ARM_instruction_dvfd:
	mov	edx,0x0e400180
	jmp	FPA_fd_fn_fm
ARM_instruction_dvfdm:
	mov	edx,0x0e4001c0
	jmp	FPA_fd_fn_fm
ARM_instruction_dvfdp:
	mov	edx,0x0e4001a0
	jmp	FPA_fd_fn_fm
ARM_instruction_dvfdz:
	mov	edx,0x0e4001e0
	jmp	FPA_fd_fn_fm
ARM_instruction_dvfe:
	mov	edx,0x0e480100
	jmp	FPA_fd_fn_fm
ARM_instruction_dvfem:
	mov	edx,0x0e480140
	jmp	FPA_fd_fn_fm
ARM_instruction_dvfep:
	mov	edx,0x0e480120
	jmp	FPA_fd_fn_fm
ARM_instruction_dvfez:
	mov	edx,0x0e480160
	jmp	FPA_fd_fn_fm
ARM_instruction_dvfs:
	mov	edx,0x0e400100
	jmp	FPA_fd_fn_fm
ARM_instruction_dvfsm:
	mov	edx,0x0e400140
	jmp	FPA_fd_fn_fm
ARM_instruction_dvfsp:
	mov	edx,0x0e400120
	jmp	FPA_fd_fn_fm
ARM_instruction_dvfsz:
	mov	edx,0x0e400160
	jmp	FPA_fd_fn_fm
ARM_instruction_expd:
	mov	edx,0x0e708180
	jmp	FPA_fd_fm
ARM_instruction_expdm:
	mov	edx,0x0e7081c0
	jmp	FPA_fd_fm
ARM_instruction_expdp:
	mov	edx,0x0e7081a0
	jmp	FPA_fd_fm
ARM_instruction_expdz:
	mov	edx,0x0e7081e0
	jmp	FPA_fd_fm
ARM_instruction_expe:
	mov	edx,0x0e788100
	jmp	FPA_fd_fm
ARM_instruction_expem:
	mov	edx,0x0e788140
	jmp	FPA_fd_fm
ARM_instruction_expep:
	mov	edx,0x0e788120
	jmp	FPA_fd_fm
ARM_instruction_expez:
	mov	edx,0x0e788160
	jmp	FPA_fd_fm
ARM_instruction_exps:
	mov	edx,0x0e708100
	jmp	FPA_fd_fm
ARM_instruction_expsm:
	mov	edx,0x0e708140
	jmp	FPA_fd_fm
ARM_instruction_expsp:
	mov	edx,0x0e708120
	jmp	FPA_fd_fm
ARM_instruction_expsz:
	mov	edx,0x0e708160
	jmp	FPA_fd_fm
ARM_instruction_fdvd:
	mov	edx,0x0ea00180
	jmp	FPA_fd_fn_fm
ARM_instruction_fdvdm:
	mov	edx,0x0ea001c0
	jmp	FPA_fd_fn_fm
ARM_instruction_fdvdp:
	mov	edx,0x0ea001a0
	jmp	FPA_fd_fn_fm
ARM_instruction_fdvdz:
	mov	edx,0x0ea001e0
	jmp	FPA_fd_fn_fm
ARM_instruction_fdve:
	mov	edx,0x0ea80100
	jmp	FPA_fd_fn_fm
ARM_instruction_fdvem:
	mov	edx,0x0ea80140
	jmp	FPA_fd_fn_fm
ARM_instruction_fdvep:
	mov	edx,0x0ea80120
	jmp	FPA_fd_fn_fm
ARM_instruction_fdvez:
	mov	edx,0x0ea80160
	jmp	FPA_fd_fn_fm
ARM_instruction_fdvs:
	mov	edx,0x0ea00100
	jmp	FPA_fd_fn_fm
ARM_instruction_fdvsm:
	mov	edx,0x0ea00140
	jmp	FPA_fd_fn_fm
ARM_instruction_fdvsp:
	mov	edx,0x0ea00120
	jmp	FPA_fd_fn_fm
ARM_instruction_fdvsz:
	mov	edx,0x0ea00160
	jmp	FPA_fd_fn_fm
ARM_instruction_fix:
	mov	edx,0x0e100110
	jmp	FPA_rd_fm
ARM_instruction_fixm:
	mov	edx,0x0e100150
	jmp	FPA_rd_fm
ARM_instruction_fixp:
	mov	edx,0x0e100130
	jmp	FPA_rd_fm
ARM_instruction_fixz:
	mov	edx,0x0e100170
	jmp	FPA_rd_fm
ARM_instruction_fltd:
	mov	edx,0x0e000190
	jmp	FPA_fn_rd
ARM_instruction_fltdm:
	mov	edx,0x0e0001d0
	jmp	FPA_fn_rd
ARM_instruction_fltdp:
	mov	edx,0x0e0001b0
	jmp	FPA_fn_rd
ARM_instruction_fltdz:
	mov	edx,0x0e0001f0
	jmp	FPA_fn_rd
ARM_instruction_flte:
	mov	edx,0x0e080110
	jmp	FPA_fn_rd
ARM_instruction_fltem:
	mov	edx,0x0e080150
	jmp	FPA_fn_rd
ARM_instruction_fltep:
	mov	edx,0x0e080130
	jmp	FPA_fn_rd
ARM_instruction_fltez:
	mov	edx,0x0e080170
	jmp	FPA_fn_rd
ARM_instruction_flts:
	mov	edx,0x0e000110
	jmp	FPA_fn_rd
ARM_instruction_fltsm:
	mov	edx,0x0e000150
	jmp	FPA_fn_rd
ARM_instruction_fltsp:
	mov	edx,0x0e000130
	jmp	FPA_fn_rd
ARM_instruction_fltsz:
	mov	edx,0x0e000170
	jmp	FPA_fn_rd
ARM_instruction_fmld:
	mov	edx,0x0e900180
	jmp	FPA_fd_fn_fm
ARM_instruction_fmldm:
	mov	edx,0x0e9001c0
	jmp	FPA_fd_fn_fm
ARM_instruction_fmldp:
	mov	edx,0x0e9001a0
	jmp	FPA_fd_fn_fm
ARM_instruction_fmldz:
	mov	edx,0x0e9001e0
	jmp	FPA_fd_fn_fm
ARM_instruction_fmle:
	mov	edx,0x0e980100
	jmp	FPA_fd_fn_fm
ARM_instruction_fmlem:
	mov	edx,0x0e980140
	jmp	FPA_fd_fn_fm
ARM_instruction_fmlep:
	mov	edx,0x0e980120
	jmp	FPA_fd_fn_fm
ARM_instruction_fmlez:
	mov	edx,0x0e980160
	jmp	FPA_fd_fn_fm
ARM_instruction_fmls:
	mov	edx,0x0e900100
	jmp	FPA_fd_fn_fm
ARM_instruction_fmlsm:
	mov	edx,0x0e900140
	jmp	FPA_fd_fn_fm
ARM_instruction_fmlsp:
	mov	edx,0x0e900120
	jmp	FPA_fd_fn_fm
ARM_instruction_fmlsz:
	mov	edx,0x0e900160
	jmp	FPA_fd_fn_fm
ARM_instruction_frdd:
	mov	edx,0x0eb00180
	jmp	FPA_fd_fn_fm
ARM_instruction_frddm:
	mov	edx,0x0eb001c0
	jmp	FPA_fd_fn_fm
ARM_instruction_frddp:
	mov	edx,0x0eb001a0
	jmp	FPA_fd_fn_fm
ARM_instruction_frddz:
	mov	edx,0x0eb001e0
	jmp	FPA_fd_fn_fm
ARM_instruction_frde:
	mov	edx,0x0eb80100
	jmp	FPA_fd_fn_fm
ARM_instruction_frdem:
	mov	edx,0x0eb80140
	jmp	FPA_fd_fn_fm
ARM_instruction_frdep:
	mov	edx,0x0eb80120
	jmp	FPA_fd_fn_fm
ARM_instruction_frdez:
	mov	edx,0x0eb80160
	jmp	FPA_fd_fn_fm
ARM_instruction_frds:
	mov	edx,0x0eb00100
	jmp	FPA_fd_fn_fm
ARM_instruction_frdsm:
	mov	edx,0x0eb00140
	jmp	FPA_fd_fn_fm
ARM_instruction_frdsp:
	mov	edx,0x0eb00120
	jmp	FPA_fd_fn_fm
ARM_instruction_frdsz:
	mov	edx,0x0eb00160
	jmp	FPA_fd_fn_fm
ARM_instruction_ldfd:
	mov	edx,0x0c108100
	jmp	FPA_fd_address5
ARM_instruction_ldfe:
	mov	edx,0x0c500100
	jmp	FPA_fd_address5
ARM_instruction_ldfp:
	mov	edx,0x0c508100
	jmp	FPA_fd_address5
ARM_instruction_ldfs:
	mov	edx,0x0c100100
	jmp	FPA_fd_address5
ARM_instruction_lfm:
	mov	edx,0x0c100200
	jmp	FPA_fd_imm_address5
ARM_instruction_lfmea:
	mov	edx,0x0d100200
	jmp	FPA_fd_imm_rn
ARM_instruction_lfmfd:
	mov	edx,0x0c900200
	jmp	FPA_fd_imm_rn
ARM_instruction_lgnd:
	mov	edx,0x0e608180
	jmp	FPA_fd_fm
ARM_instruction_lgndm:
	mov	edx,0x0e6081c0
	jmp	FPA_fd_fm
ARM_instruction_lgndp:
	mov	edx,0x0e6081a0
	jmp	FPA_fd_fm
ARM_instruction_lgndz:
	mov	edx,0x0e6081e0
	jmp	FPA_fd_fm
ARM_instruction_lgne:
	mov	edx,0x0e688100
	jmp	FPA_fd_fm
ARM_instruction_lgnem:
	mov	edx,0x0e688140
	jmp	FPA_fd_fm
ARM_instruction_lgnep:
	mov	edx,0x0e688120
	jmp	FPA_fd_fm
ARM_instruction_lgnez:
	mov	edx,0x0e688160
	jmp	FPA_fd_fm
ARM_instruction_lgns:
	mov	edx,0x0e608100
	jmp	FPA_fd_fm
ARM_instruction_lgnsm:
	mov	edx,0x0e608140
	jmp	FPA_fd_fm
ARM_instruction_lgnsp:
	mov	edx,0x0e608120
	jmp	FPA_fd_fm
ARM_instruction_lgnsz:
	mov	edx,0x0e608160
	jmp	FPA_fd_fm
ARM_instruction_logd:
	mov	edx,0x0e508180
	jmp	FPA_fd_fm
ARM_instruction_logdm:
	mov	edx,0x0e5081c0
	jmp	FPA_fd_fm
ARM_instruction_logdp:
	mov	edx,0x0e5081a0
	jmp	FPA_fd_fm
ARM_instruction_logdz:
	mov	edx,0x0e5081e0
	jmp	FPA_fd_fm
ARM_instruction_loge:
	mov	edx,0x0e588100
	jmp	FPA_fd_fm
ARM_instruction_logem:
	mov	edx,0x0e588140
	jmp	FPA_fd_fm
ARM_instruction_logep:
	mov	edx,0x0e588120
	jmp	FPA_fd_fm
ARM_instruction_logez:
	mov	edx,0x0e588160
	jmp	FPA_fd_fm
ARM_instruction_logs:
	mov	edx,0x0e508100
	jmp	FPA_fd_fm
ARM_instruction_logsm:
	mov	edx,0x0e508140
	jmp	FPA_fd_fm
ARM_instruction_logsp:
	mov	edx,0x0e508120
	jmp	FPA_fd_fm
ARM_instruction_logsz:
	mov	edx,0x0e508160
	jmp	FPA_fd_fm
ARM_instruction_mnfd:
	mov	edx,0x0e108180
	jmp	FPA_fd_fm
ARM_instruction_mnfdm:
	mov	edx,0x0e1081c0
	jmp	FPA_fd_fm
ARM_instruction_mnfdp:
	mov	edx,0x0e1081a0
	jmp	FPA_fd_fm
ARM_instruction_mnfdz:
	mov	edx,0x0e1081e0
	jmp	FPA_fd_fm
ARM_instruction_mnfe:
	mov	edx,0x0e188100
	jmp	FPA_fd_fm
ARM_instruction_mnfem:
	mov	edx,0x0e188140
	jmp	FPA_fd_fm
ARM_instruction_mnfep:
	mov	edx,0x0e188120
	jmp	FPA_fd_fm
ARM_instruction_mnfez:
	mov	edx,0x0e188160
	jmp	FPA_fd_fm
ARM_instruction_mnfs:
	mov	edx,0x0e108100
	jmp	FPA_fd_fm
ARM_instruction_mnfsm:
	mov	edx,0x0e108140
	jmp	FPA_fd_fm
ARM_instruction_mnfsp:
	mov	edx,0x0e108120
	jmp	FPA_fd_fm
ARM_instruction_mnfsz:
	mov	edx,0x0e108160
	jmp	FPA_fd_fm
ARM_instruction_mufd:
	mov	edx,0x0e100180
	jmp	FPA_fd_fn_fm
ARM_instruction_mufdm:
	mov	edx,0x0e1001c0
	jmp	FPA_fd_fn_fm
ARM_instruction_mufdp:
	mov	edx,0x0e1001a0
	jmp	FPA_fd_fn_fm
ARM_instruction_mufdz:
	mov	edx,0x0e1001e0
	jmp	FPA_fd_fn_fm
ARM_instruction_mufe:
	mov	edx,0x0e180100
	jmp	FPA_fd_fn_fm
ARM_instruction_mufem:
	mov	edx,0x0e180140
	jmp	FPA_fd_fn_fm
ARM_instruction_mufep:
	mov	edx,0x0e180120
	jmp	FPA_fd_fn_fm
ARM_instruction_mufez:
	mov	edx,0x0e180160
	jmp	FPA_fd_fn_fm
ARM_instruction_mufs:
	mov	edx,0x0e100100
	jmp	FPA_fd_fn_fm
ARM_instruction_mufsm:
	mov	edx,0x0e100140
	jmp	FPA_fd_fn_fm
ARM_instruction_mufsp:
	mov	edx,0x0e100120
	jmp	FPA_fd_fn_fm
ARM_instruction_mufsz:
	mov	edx,0x0e100160
	jmp	FPA_fd_fn_fm
ARM_instruction_mvfd:
	mov	edx,0x0e008180
	jmp	FPA_fd_fm
ARM_instruction_mvfdm:
	mov	edx,0x0e0081c0
	jmp	FPA_fd_fm
ARM_instruction_mvfdp:
	mov	edx,0x0e0081a0
	jmp	FPA_fd_fm
ARM_instruction_mvfdz:
	mov	edx,0x0e0081e0
	jmp	FPA_fd_fm
ARM_instruction_mvfe:
	mov	edx,0x0e088100
	jmp	FPA_fd_fm
ARM_instruction_mvfem:
	mov	edx,0x0e088140
	jmp	FPA_fd_fm
ARM_instruction_mvfep:
	mov	edx,0x0e088120
	jmp	FPA_fd_fm
ARM_instruction_mvfez:
	mov	edx,0x0e088160
	jmp	FPA_fd_fm
ARM_instruction_mvfs:
	mov	edx,0x0e008100
	jmp	FPA_fd_fm
ARM_instruction_mvfsm:
	mov	edx,0x0e008140
	jmp	FPA_fd_fm
ARM_instruction_mvfsp:
	mov	edx,0x0e008120
	jmp	FPA_fd_fm
ARM_instruction_mvfsz:
	mov	edx,0x0e008160
	jmp	FPA_fd_fm
ARM_instruction_nrmd:
	mov	edx,0x0ef08180
	jmp	FPA_fd_fm
ARM_instruction_nrmdm:
	mov	edx,0x0ef081c0
	jmp	FPA_fd_fm
ARM_instruction_nrmdp:
	mov	edx,0x0ef081a0
	jmp	FPA_fd_fm
ARM_instruction_nrmdz:
	mov	edx,0x0ef081e0
	jmp	FPA_fd_fm
ARM_instruction_nrme:
	mov	edx,0x0ef88100
	jmp	FPA_fd_fm
ARM_instruction_nrmem:
	mov	edx,0x0ef88140
	jmp	FPA_fd_fm
ARM_instruction_nrmep:
	mov	edx,0x0ef88120
	jmp	FPA_fd_fm
ARM_instruction_nrmez:
	mov	edx,0x0ef88160
	jmp	FPA_fd_fm
ARM_instruction_nrms:
	mov	edx,0x0ef08100
	jmp	FPA_fd_fm
ARM_instruction_nrmsm:
	mov	edx,0x0ef08140
	jmp	FPA_fd_fm
ARM_instruction_nrmsp:
	mov	edx,0x0ef08120
	jmp	FPA_fd_fm
ARM_instruction_nrmsz:
	mov	edx,0x0ef08160
	jmp	FPA_fd_fm
ARM_instruction_pold:
	mov	edx,0x0ec00180
	jmp	FPA_fd_fn_fm
ARM_instruction_poldm:
	mov	edx,0x0ec001c0
	jmp	FPA_fd_fn_fm
ARM_instruction_poldp:
	mov	edx,0x0ec001a0
	jmp	FPA_fd_fn_fm
ARM_instruction_poldz:
	mov	edx,0x0ec001e0
	jmp	FPA_fd_fn_fm
ARM_instruction_pole:
	mov	edx,0x0ec80100
	jmp	FPA_fd_fn_fm
ARM_instruction_polem:
	mov	edx,0x0ec80140
	jmp	FPA_fd_fn_fm
ARM_instruction_polep:
	mov	edx,0x0ec80120
	jmp	FPA_fd_fn_fm
ARM_instruction_polez:
	mov	edx,0x0ec80160
	jmp	FPA_fd_fn_fm
ARM_instruction_pols:
	mov	edx,0x0ec00100
	jmp	FPA_fd_fn_fm
ARM_instruction_polsm:
	mov	edx,0x0ec00140
	jmp	FPA_fd_fn_fm
ARM_instruction_polsp:
	mov	edx,0x0ec00120
	jmp	FPA_fd_fn_fm
ARM_instruction_polsz:
	mov	edx,0x0ec00160
	jmp	FPA_fd_fn_fm
ARM_instruction_powd:
	mov	edx,0x0e600180
	jmp	FPA_fd_fn_fm
ARM_instruction_powdm:
	mov	edx,0x0e6001c0
	jmp	FPA_fd_fn_fm
ARM_instruction_powdp:
	mov	edx,0x0e6001a0
	jmp	FPA_fd_fn_fm
ARM_instruction_powdz:
	mov	edx,0x0e6001e0
	jmp	FPA_fd_fn_fm
ARM_instruction_powe:
	mov	edx,0x0e680100
	jmp	FPA_fd_fn_fm
ARM_instruction_powem:
	mov	edx,0x0e680140
	jmp	FPA_fd_fn_fm
ARM_instruction_powep:
	mov	edx,0x0e680120
	jmp	FPA_fd_fn_fm
ARM_instruction_powez:
	mov	edx,0x0e680160
	jmp	FPA_fd_fn_fm
ARM_instruction_pows:
	mov	edx,0x0e600100
	jmp	FPA_fd_fn_fm
ARM_instruction_powsm:
	mov	edx,0x0e600140
	jmp	FPA_fd_fn_fm
ARM_instruction_powsp:
	mov	edx,0x0e600120
	jmp	FPA_fd_fn_fm
ARM_instruction_powsz:
	mov	edx,0x0e600160
	jmp	FPA_fd_fn_fm
ARM_instruction_rdfd:
	mov	edx,0x0e500180
	jmp	FPA_fd_fn_fm
ARM_instruction_rdfdm:
	mov	edx,0x0e5001c0
	jmp	FPA_fd_fn_fm
ARM_instruction_rdfdp:
	mov	edx,0x0e5001a0
	jmp	FPA_fd_fn_fm
ARM_instruction_rdfdz:
	mov	edx,0x0e5001e0
	jmp	FPA_fd_fn_fm
ARM_instruction_rdfe:
	mov	edx,0x0e580100
	jmp	FPA_fd_fn_fm
ARM_instruction_rdfem:
	mov	edx,0x0e580140
	jmp	FPA_fd_fn_fm
ARM_instruction_rdfep:
	mov	edx,0x0e580120
	jmp	FPA_fd_fn_fm
ARM_instruction_rdfez:
	mov	edx,0x0e580160
	jmp	FPA_fd_fn_fm
ARM_instruction_rdfs:
	mov	edx,0x0e500100
	jmp	FPA_fd_fn_fm
ARM_instruction_rdfsm:
	mov	edx,0x0e500140
	jmp	FPA_fd_fn_fm
ARM_instruction_rdfsp:
	mov	edx,0x0e500120
	jmp	FPA_fd_fn_fm
ARM_instruction_rdfsz:
	mov	edx,0x0e500160
	jmp	FPA_fd_fn_fm
ARM_instruction_rfc:
	mov	edx,0x0e500110
	jmp	FPA_rd
ARM_instruction_rfs:
	mov	edx,0x0e300110
	jmp	FPA_rd
ARM_instruction_rmfd:
	mov	edx,0x0e800180
	jmp	FPA_fd_fn_fm
ARM_instruction_rmfdm:
	mov	edx,0x0e8001c0
	jmp	FPA_fd_fn_fm
ARM_instruction_rmfdp:
	mov	edx,0x0e8001a0
	jmp	FPA_fd_fn_fm
ARM_instruction_rmfdz:
	mov	edx,0x0e8001e0
	jmp	FPA_fd_fn_fm
ARM_instruction_rmfe:
	mov	edx,0x0e880100
	jmp	FPA_fd_fn_fm
ARM_instruction_rmfem:
	mov	edx,0x0e880140
	jmp	FPA_fd_fn_fm
ARM_instruction_rmfep:
	mov	edx,0x0e880120
	jmp	FPA_fd_fn_fm
ARM_instruction_rmfez:
	mov	edx,0x0e880160
	jmp	FPA_fd_fn_fm
ARM_instruction_rmfs:
	mov	edx,0x0e800100
	jmp	FPA_fd_fn_fm
ARM_instruction_rmfsm:
	mov	edx,0x0e800140
	jmp	FPA_fd_fn_fm
ARM_instruction_rmfsp:
	mov	edx,0x0e800120
	jmp	FPA_fd_fn_fm
ARM_instruction_rmfsz:
	mov	edx,0x0e800160
	jmp	FPA_fd_fn_fm
ARM_instruction_rndd:
	mov	edx,0x0e308180
	jmp	FPA_fd_fm
ARM_instruction_rnddm:
	mov	edx,0x0e3081c0
	jmp	FPA_fd_fm
ARM_instruction_rnddp:
	mov	edx,0x0e3081a0
	jmp	FPA_fd_fm
ARM_instruction_rnddz:
	mov	edx,0x0e3081e0
	jmp	FPA_fd_fm
ARM_instruction_rnde:
	mov	edx,0x0e388100
	jmp	FPA_fd_fm
ARM_instruction_rndem:
	mov	edx,0x0e388140
	jmp	FPA_fd_fm
ARM_instruction_rndep:
	mov	edx,0x0e388120
	jmp	FPA_fd_fm
ARM_instruction_rndez:
	mov	edx,0x0e388160
	jmp	FPA_fd_fm
ARM_instruction_rnds:
	mov	edx,0x0e308100
	jmp	FPA_fd_fm
ARM_instruction_rndsm:
	mov	edx,0x0e308140
	jmp	FPA_fd_fm
ARM_instruction_rndsp:
	mov	edx,0x0e308120
	jmp	FPA_fd_fm
ARM_instruction_rndsz:
	mov	edx,0x0e308160
	jmp	FPA_fd_fm
ARM_instruction_rpwd:
	mov	edx,0x0e700180
	jmp	FPA_fd_fn_fm
ARM_instruction_rpwdm:
	mov	edx,0x0e7001c0
	jmp	FPA_fd_fn_fm
ARM_instruction_rpwdp:
	mov	edx,0x0e7001a0
	jmp	FPA_fd_fn_fm
ARM_instruction_rpwdz:
	mov	edx,0x0e7001e0
	jmp	FPA_fd_fn_fm
ARM_instruction_rpwe:
	mov	edx,0x0e780100
	jmp	FPA_fd_fn_fm
ARM_instruction_rpwem:
	mov	edx,0x0e780140
	jmp	FPA_fd_fn_fm
ARM_instruction_rpwep:
	mov	edx,0x0e780120
	jmp	FPA_fd_fn_fm
ARM_instruction_rpwez:
	mov	edx,0x0e780160
	jmp	FPA_fd_fn_fm
ARM_instruction_rpws:
	mov	edx,0x0e700100
	jmp	FPA_fd_fn_fm
ARM_instruction_rpwsm:
	mov	edx,0x0e700140
	jmp	FPA_fd_fn_fm
ARM_instruction_rpwsp:
	mov	edx,0x0e700120
	jmp	FPA_fd_fn_fm
ARM_instruction_rpwsz:
	mov	edx,0x0e700160
	jmp	FPA_fd_fn_fm
ARM_instruction_rsfd:
	mov	edx,0x0e300180
	jmp	FPA_fd_fn_fm
ARM_instruction_rsfdm:
	mov	edx,0x0e3001c0
	jmp	FPA_fd_fn_fm
ARM_instruction_rsfdp:
	mov	edx,0x0e3001a0
	jmp	FPA_fd_fn_fm
ARM_instruction_rsfdz:
	mov	edx,0x0e3001e0
	jmp	FPA_fd_fn_fm
ARM_instruction_rsfe:
	mov	edx,0x0e380100
	jmp	FPA_fd_fn_fm
ARM_instruction_rsfem:
	mov	edx,0x0e380140
	jmp	FPA_fd_fn_fm
ARM_instruction_rsfep:
	mov	edx,0x0e380120
	jmp	FPA_fd_fn_fm
ARM_instruction_rsfez:
	mov	edx,0x0e380160
	jmp	FPA_fd_fn_fm
ARM_instruction_rsfs:
	mov	edx,0x0e300100
	jmp	FPA_fd_fn_fm
ARM_instruction_rsfsm:
	mov	edx,0x0e300140
	jmp	FPA_fd_fn_fm
ARM_instruction_rsfsp:
	mov	edx,0x0e300120
	jmp	FPA_fd_fn_fm
ARM_instruction_rsfsz:
	mov	edx,0x0e300160
	jmp	FPA_fd_fn_fm
ARM_instruction_sfm:
	mov	edx,0x0c000200
	jmp	FPA_fd_imm_address5
ARM_instruction_sfmea:
	mov	edx,0x0c800200
	jmp	FPA_fd_imm_rn
ARM_instruction_sfmfd:
	mov	edx,0x0d000200
	jmp	FPA_fd_imm_rn
ARM_instruction_sind:
	mov	edx,0x0e808180
	jmp	FPA_fd_fm
ARM_instruction_sindm:
	mov	edx,0x0e8081c0
	jmp	FPA_fd_fm
ARM_instruction_sindp:
	mov	edx,0x0e8081a0
	jmp	FPA_fd_fm
ARM_instruction_sindz:
	mov	edx,0x0e8081e0
	jmp	FPA_fd_fm
ARM_instruction_sine:
	mov	edx,0x0e888100
	jmp	FPA_fd_fm
ARM_instruction_sinem:
	mov	edx,0x0e888140
	jmp	FPA_fd_fm
ARM_instruction_sinep:
	mov	edx,0x0e888120
	jmp	FPA_fd_fm
ARM_instruction_sinez:
	mov	edx,0x0e888160
	jmp	FPA_fd_fm
ARM_instruction_sins:
	mov	edx,0x0e808100
	jmp	FPA_fd_fm
ARM_instruction_sinsm:
	mov	edx,0x0e808140
	jmp	FPA_fd_fm
ARM_instruction_sinsp:
	mov	edx,0x0e808120
	jmp	FPA_fd_fm
ARM_instruction_sinsz:
	mov	edx,0x0e808160
	jmp	FPA_fd_fm
ARM_instruction_sqtd:
	mov	edx,0x0e408180
	jmp	FPA_fd_fm
ARM_instruction_sqtdm:
	mov	edx,0x0e4081c0
	jmp	FPA_fd_fm
ARM_instruction_sqtdp:
	mov	edx,0x0e4081a0
	jmp	FPA_fd_fm
ARM_instruction_sqtdz:
	mov	edx,0x0e4081e0
	jmp	FPA_fd_fm
ARM_instruction_sqte:
	mov	edx,0x0e488100
	jmp	FPA_fd_fm
ARM_instruction_sqtem:
	mov	edx,0x0e488140
	jmp	FPA_fd_fm
ARM_instruction_sqtep:
	mov	edx,0x0e488120
	jmp	FPA_fd_fm
ARM_instruction_sqtez:
	mov	edx,0x0e488160
	jmp	FPA_fd_fm
ARM_instruction_sqts:
	mov	edx,0x0e408100
	jmp	FPA_fd_fm
ARM_instruction_sqtsm:
	mov	edx,0x0e408140
	jmp	FPA_fd_fm
ARM_instruction_sqtsp:
	mov	edx,0x0e408120
	jmp	FPA_fd_fm
ARM_instruction_sqtsz:
	mov	edx,0x0e408160
	jmp	FPA_fd_fm
ARM_instruction_stfd:
	mov	edx,0x0c008100
	jmp	FPA_fd_address5
ARM_instruction_stfe:
	mov	edx,0x0c400100
	jmp	FPA_fd_address5
ARM_instruction_stfp:
	mov	edx,0x0c408100
	jmp	FPA_fd_address5
ARM_instruction_stfs:
	mov	edx,0x0c000100
	jmp	FPA_fd_address5
ARM_instruction_sufd:
	mov	edx,0x0e200180
	jmp	FPA_fd_fn_fm
ARM_instruction_sufdm:
	mov	edx,0x0e2001c0
	jmp	FPA_fd_fn_fm
ARM_instruction_sufdp:
	mov	edx,0x0e2001a0
	jmp	FPA_fd_fn_fm
ARM_instruction_sufdz:
	mov	edx,0x0e2001e0
	jmp	FPA_fd_fn_fm
ARM_instruction_sufe:
	mov	edx,0x0e280100
	jmp	FPA_fd_fn_fm
ARM_instruction_sufem:
	mov	edx,0x0e280140
	jmp	FPA_fd_fn_fm
ARM_instruction_sufep:
	mov	edx,0x0e280120
	jmp	FPA_fd_fn_fm
ARM_instruction_sufez:
	mov	edx,0x0e280160
	jmp	FPA_fd_fn_fm
ARM_instruction_sufs:
	mov	edx,0x0e200100
	jmp	FPA_fd_fn_fm
ARM_instruction_sufsm:
	mov	edx,0x0e200140
	jmp	FPA_fd_fn_fm
ARM_instruction_sufsp:
	mov	edx,0x0e200120
	jmp	FPA_fd_fn_fm
ARM_instruction_sufsz:
	mov	edx,0x0e200160
	jmp	FPA_fd_fn_fm
ARM_instruction_tand:
	mov	edx,0x0ea08180
	jmp	FPA_fd_fm
ARM_instruction_tandm:
	mov	edx,0x0ea081c0
	jmp	FPA_fd_fm
ARM_instruction_tandp:
	mov	edx,0x0ea081a0
	jmp	FPA_fd_fm
ARM_instruction_tandz:
	mov	edx,0x0ea081e0
	jmp	FPA_fd_fm
ARM_instruction_tane:
	mov	edx,0x0ea88100
	jmp	FPA_fd_fm
ARM_instruction_tanem:
	mov	edx,0x0ea88140
	jmp	FPA_fd_fm
ARM_instruction_tanep:
	mov	edx,0x0ea88120
	jmp	FPA_fd_fm
ARM_instruction_tanez:
	mov	edx,0x0ea88160
	jmp	FPA_fd_fm
ARM_instruction_tans:
	mov	edx,0x0ea08100
	jmp	FPA_fd_fm
ARM_instruction_tansm:
	mov	edx,0x0ea08140
	jmp	FPA_fd_fm
ARM_instruction_tansp:
	mov	edx,0x0ea08120
	jmp	FPA_fd_fm
ARM_instruction_tansz:
	mov	edx,0x0ea08160
	jmp	FPA_fd_fm
ARM_instruction_urdd:
	mov	edx,0x0ee08180
	jmp	FPA_fd_fm
ARM_instruction_urddm:
	mov	edx,0x0ee081c0
	jmp	FPA_fd_fm
ARM_instruction_urddp:
	mov	edx,0x0ee081a0
	jmp	FPA_fd_fm
ARM_instruction_urddz:
	mov	edx,0x0ee081e0
	jmp	FPA_fd_fm
ARM_instruction_urde:
	mov	edx,0x0ee88100
	jmp	FPA_fd_fm
ARM_instruction_urdem:
	mov	edx,0x0ee88140
	jmp	FPA_fd_fm
ARM_instruction_urdep:
	mov	edx,0x0ee88120
	jmp	FPA_fd_fm
ARM_instruction_urdez:
	mov	edx,0x0ee88160
	jmp	FPA_fd_fm
ARM_instruction_urds:
	mov	edx,0x0ee08100
	jmp	FPA_fd_fm
ARM_instruction_urdsm:
	mov	edx,0x0ee08140
	jmp	FPA_fd_fm
ARM_instruction_urdsp:
	mov	edx,0x0ee08120
	jmp	FPA_fd_fm
ARM_instruction_urdsz:
	mov	edx,0x0ee08160
	jmp	FPA_fd_fm
ARM_instruction_wfc:
	mov	edx,0x0e400110
	jmp	FPA_rd
ARM_instruction_wfs:
	mov	edx,0x0e200110
	jmp	FPA_rd

;Maverick jump table

ARM_instruction_cfabs32:
	mov	edx,0x0e300500
	jmp	MAVERICK_crd_crn
ARM_instruction_cfabs64:
	mov	edx,0x0e300520
	jmp	MAVERICK_crd_crn
ARM_instruction_cfabsd:
	mov	edx,0x0e300420
	jmp	MAVERICK_crd_crn
ARM_instruction_cfabss:
	mov	edx,0x0e300400
	jmp	MAVERICK_crd_crn
ARM_instruction_cfadd32:
	mov	edx,0x0e300580
	jmp	MAVERICK_crd_crn_crm
ARM_instruction_cfadd64:
	mov	edx,0x0e3005a0
	jmp	MAVERICK_crd_crn_crm
ARM_instruction_cfaddd:
	mov	edx,0x0e3004a0
	jmp	MAVERICK_crd_crn_crm
ARM_instruction_cfadds:
	mov	edx,0x0e300480
	jmp	MAVERICK_crd_crn_crm
ARM_instruction_cfcmp32:
	mov	edx,0x0e100590
	jmp	MAVERICK_rd_crn_crm
ARM_instruction_cfcmp64:
	mov	edx,0x0e1005b0
	jmp	MAVERICK_rd_crn_crm
ARM_instruction_cfcmpd:
	mov	edx,0x0e1004b0
	jmp	MAVERICK_rd_crn_crm
ARM_instruction_cfcmps:
	mov	edx,0x0e100490
	jmp	MAVERICK_rd_crn_crm
ARM_instruction_cfcpyd:
	mov	edx,0x0e000420
	jmp	MAVERICK_crd_crn
ARM_instruction_cfcpys:
	mov	edx,0x0e000400
	jmp	MAVERICK_crd_crn
ARM_instruction_cfcvt32d:
	mov	edx,0x0e0004a0
	jmp	MAVERICK_crd_crn
ARM_instruction_cfcvt32s:
	mov	edx,0x0e000480
	jmp	MAVERICK_crd_crn
ARM_instruction_cfcvt64d:
	mov	edx,0x0e0004e0
	jmp	MAVERICK_crd_crn
ARM_instruction_cfcvt64s:
	mov	edx,0x0e0004c0
	jmp	MAVERICK_crd_crn
ARM_instruction_cfcvtd32:
	mov	edx,0x0e1005a0
	jmp	MAVERICK_crd_crn
ARM_instruction_cfcvtds:
	mov	edx,0x0e000440
	jmp	MAVERICK_crd_crn
ARM_instruction_cfcvts32:
	mov	edx,0x0e100580
	jmp	MAVERICK_crd_crn
ARM_instruction_cfcvtsd:
	mov	edx,0x0e000460
	jmp	MAVERICK_crd_crn
ARM_instruction_cfldr32:
	mov	edx,0x0c100500
	jmp	MAVERICK_crd_address5
ARM_instruction_cfldr64:
	mov	edx,0x0c500500
	jmp	MAVERICK_crd_address5
ARM_instruction_cfldrd:
	mov	edx,0x0c500400
	jmp	MAVERICK_crd_address5
ARM_instruction_cfldrs:
	mov	edx,0x0c100400
	jmp	MAVERICK_crd_address5
ARM_instruction_cfmac32:
	mov	edx,0x0e100540
	jmp	MAVERICK_crd_crn_crm
ARM_instruction_cfmadd32:
	mov	edx,0x0e000600
	jmp	MAVERICK_aa_crd_crn_crm
ARM_instruction_cfmadda32:
	mov	edx,0x0e200600
	jmp	MAVERICK_aa_ad_crn_crm
ARM_instruction_cfmsc32:
	mov	edx,0x0e100560
	jmp	MAVERICK_crd_crn_crm
ARM_instruction_cfmsub32:
	mov	edx,0x0e100600
	jmp	MAVERICK_aa_crd_crn_crm
ARM_instruction_cfmsuba32:
	mov	edx,0x0e300600
	jmp	MAVERICK_aa_ad_crn_crm
ARM_instruction_cfmul32:
	mov	edx,0x0e100500
	jmp	MAVERICK_crd_crn_crm
ARM_instruction_cfmul64:
	mov	edx,0x0e100520
	jmp	MAVERICK_crd_crn_crm
ARM_instruction_cfmuld:
	mov	edx,0x0e100420
	jmp	MAVERICK_crd_crn_crm
ARM_instruction_cfmuls:
	mov	edx,0x0e100400
	jmp	MAVERICK_crd_crn_crm
ARM_instruction_cfmv32a:
	mov	edx,0x0e1004a0
	jmp	MAVERICK_crd_an
ARM_instruction_cfmv32ah:
	mov	edx,0x0e100480
	jmp	MAVERICK_crd_an
ARM_instruction_cfmv32al:
	mov	edx,0x0e100440
	jmp	MAVERICK_crd_an
ARM_instruction_cfmv32am:
	mov	edx,0x0e100460
	jmp	MAVERICK_crd_an
ARM_instruction_cfmv32sc:
	mov	edx,0x0e1004e0
	jmp	MAVERICK_crd_psc
ARM_instruction_cfmv64a:
	mov	edx,0x0e1004c0
	jmp	MAVERICK_crd_an
ARM_instruction_cfmv64hr:
	mov	edx,0x0e000530
	jmp	MAVERICK_crn_rd
ARM_instruction_cfmv64lr:
	mov	edx,0x0e000510
	jmp	MAVERICK_crn_rd
ARM_instruction_cfmva32:
	mov	edx,0x0e2004a0
	jmp	MAVERICK_ad_crn
ARM_instruction_cfmva64:
	mov	edx,0x0e2004c0
	jmp	MAVERICK_ad_crn
ARM_instruction_cfmvah32:
	mov	edx,0x0e200480
	jmp	MAVERICK_ad_crn
ARM_instruction_cfmval32:
	mov	edx,0x0e200440
	jmp	MAVERICK_ad_crn
ARM_instruction_cfmvam32:
	mov	edx,0x0e200460
	jmp	MAVERICK_ad_crn
ARM_instruction_cfmvdhr:
	mov	edx,0x0e000430
	jmp	MAVERICK_crn_rd
ARM_instruction_cfmvdlr:
	mov	edx,0x0e000410
	jmp	MAVERICK_crn_rd
ARM_instruction_cfmvr64h:
	mov	edx,0x0e100530
	jmp	MAVERICK_rd_crn
ARM_instruction_cfmvr64l:
	mov	edx,0x0e100510
	jmp	MAVERICK_rd_crn
ARM_instruction_cfmvrdh:
	mov	edx,0x0e100430
	jmp	MAVERICK_rd_crn
ARM_instruction_cfmvrdl:
	mov	edx,0x0e100410
	jmp	MAVERICK_rd_crn
ARM_instruction_cfmvrs:
	mov	edx,0x0e100450
	jmp	MAVERICK_rd_crn
ARM_instruction_cfmvsc32:
	mov	edx,0x0e2004e0
	jmp	MAVERICK_psc_crd
ARM_instruction_cfmvsr:
	mov	edx,0x0e000450
	jmp	MAVERICK_crn_rd
ARM_instruction_cfneg32:
	mov	edx,0x0e300540
	jmp	MAVERICK_crd_crn
ARM_instruction_cfneg64:
	mov	edx,0x0e300560
	jmp	MAVERICK_crd_crn
ARM_instruction_cfnegd:
	mov	edx,0x0e300460
	jmp	MAVERICK_crd_crn
ARM_instruction_cfnegs:
	mov	edx,0x0e300440
	jmp	MAVERICK_crd_crn
ARM_instruction_cfrshl32:
	mov	edx,0x0e000550
	jmp	MAVERICK_crm_crn_rd
ARM_instruction_cfrshl64:
	mov	edx,0x0e000570
	jmp	MAVERICK_crm_crn_rd
ARM_instruction_cfsh32:
	mov	edx,0x0e000500
	jmp	MAVERICK_crd_crn_imm
ARM_instruction_cfsh64:
	mov	edx,0x0e200500
	jmp	MAVERICK_crd_crn_imm
ARM_instruction_cfstr32:
	mov	edx,0x0c000500
	jmp	MAVERICK_crd_address5
ARM_instruction_cfstr64:
	mov	edx,0x0c400500
	jmp	MAVERICK_crd_address5
ARM_instruction_cfstrd:
	mov	edx,0x0c400400
	jmp	MAVERICK_crd_address5
ARM_instruction_cfstrs:
	mov	edx,0x0c000400
	jmp	MAVERICK_crd_address5
ARM_instruction_cfsub32:
	mov	edx,0x0e3005c0
	jmp	MAVERICK_crd_crn_crm
ARM_instruction_cfsub64:
	mov	edx,0x0e3005e0
	jmp	MAVERICK_crd_crn_crm
ARM_instruction_cfsubd:
	mov	edx,0x0e3004e0
	jmp	MAVERICK_crd_crn_crm
ARM_instruction_cfsubs:
	mov	edx,0x0e3004c0
	jmp	MAVERICK_crd_crn_crm
ARM_instruction_cftruncd32:
	mov	edx,0x0e1005e0
	jmp	MAVERICK_crd_crn
ARM_instruction_cftruncs32:
	mov	edx,0x0e1005c0
	jmp	MAVERICK_crd_crn

;VFP jump table

ARM_instruction_fabsd:
	mov	edx,0x0eb00bc0
	jmp	VFP_dd_dm
ARM_instruction_fabss:
	mov	edx,0x0eb00ac0
	jmp	VFP_sd_sm
ARM_instruction_faddd:
	mov	edx,0x0e300b00
	jmp	VFP_dd_dn_dm
ARM_instruction_fadds:
	mov	edx,0x0e300a00
	jmp	VFP_sd_sn_sm
ARM_instruction_fcmpd:
	mov	edx,0x0eb40b40
	jmp	VFP_dd_dm_zero
ARM_instruction_fcmped:
	mov	edx,0x0eb40bc0
	jmp	VFP_dd_dm_zero
ARM_instruction_fcmpes:
	mov	edx,0x0eb40ac0
	jmp	VFP_sd_sm_zero
ARM_instruction_fcmpezd:
	mov	edx,0x0eb50bc0
	jmp	VFP_dd
ARM_instruction_fcmpezs:
	mov	edx,0x0eb50ac0
	jmp	VFP_sd
ARM_instruction_fcmps:
	mov	edx,0x0eb40a40
	jmp	VFP_sd_sm_zero
ARM_instruction_fcmpzd:
	mov	edx,0x0eb50b40
	jmp	VFP_dd
ARM_instruction_fcmpzs:
	mov	edx,0x0eb50a40
	jmp	VFP_sd
ARM_instruction_fconstd:
	mov	edx,0x0eb00b00
	jmp	VFP_dm_imm
ARM_instruction_fconsts:
	mov	edx,0x0eb00a00
	jmp	VFP_sm_imm
ARM_instruction_fcpyd:
	mov	edx,0x0eb00b40
	jmp	VFP_dd_dm
ARM_instruction_fcpys:
	mov	edx,0x0eb00a40
	jmp	VFP_sd_sm
ARM_instruction_fcvtds:
	mov	edx,0x0eb70ac0
	jmp	VFP_dd_sm
ARM_instruction_fcvtsd:
	mov	edx,0x0eb70bc0
	jmp	VFP_sd_dm
ARM_instruction_fdivd:
	mov	edx,0x0e800b00
	jmp	VFP_dd_dn_dm
ARM_instruction_fdivs:
	mov	edx,0x0e800a00
	jmp	VFP_sd_sn_sm
ARM_instruction_fldd:
	mov	edx,0x0d100b00
	jmp	VFP_dd_rn_offset
ARM_instruction_fldmdbd:
ARM_instruction_fldmead:
	mov	edx,0xd100b00
	jmp	VFP_rn_list_d
ARM_instruction_fldmdbx:
ARM_instruction_fldmeax:
	mov	edx,0xd100b01
	jmp	VFP_rn_list_d
ARM_instruction_fldmdbs:
ARM_instruction_fldmeas:
	mov	edx,0xd100a00
	jmp	VFP_rn_list_s
ARM_instruction_fldmd:
ARM_instruction_fldmiad:
ARM_instruction_fldmfdd:
	mov	edx,0xc900b00
	jmp	VFP_rn_list_d
ARM_instruction_fldmx:
ARM_instruction_fldmiax:
ARM_instruction_fldmfdx:
	mov	edx,0xc900b01
	jmp	VFP_rn_list_d
ARM_instruction_fldms:
ARM_instruction_fldmias:
ARM_instruction_fldmfds:
	mov	edx,0xc900a00
	jmp	VFP_rn_list_s
ARM_instruction_flds:
	mov	edx,0x0d100a00
	jmp	VFP_sd_rn_offset
ARM_instruction_fmacd:
	mov	edx,0x0e000b00
	jmp	VFP_FLOAT_dd_dn_dm
ARM_instruction_fmacs:
	mov	edx,0x0e000a00
	jmp	VFP_FLOAT_sd_sn_sm
ARM_instruction_fmdhr:
	mov	edx,0xe200b10
	jmp	VFP_dn_rd
ARM_instruction_fmdlr:
	mov	edx,0xe000b10
	jmp	VFP_dn_rd
ARM_instruction_fmdrr:
	mov	edx,0x0c400b10
	jmp	VFP_dm_rd_rn
ARM_instruction_fmrdh:
	mov	edx,0x0e300b10
	jmp	VFP_rd_dn
ARM_instruction_fmrdl:
	mov	edx,0x0e100b10
	jmp	VFP_rd_dn
ARM_instruction_fmrrd:
	mov	edx,0x0c500b10
	jmp	VFP_rd_rn_dm
ARM_instruction_fmrrs:
	mov	edx,0x0c500a10
	jmp	VFP_rd_rn_sm
ARM_instruction_fmrs:
	mov	edx,0x0e100a10
	jmp	VFP_rd_sn
ARM_instruction_fmrx:
	mov	edx,0x0ef00a10
	jmp	VFP_rd_sysreg
ARM_instruction_fmscd:
	mov	edx,0x0e100b00
	jmp	VFP_FLOAT_dd_dn_dm
ARM_instruction_fmscs:
	mov	edx,0x0e100a00
	jmp	VFP_FLOAT_sd_sn_sm
ARM_instruction_fmsr:
	mov	edx,0x0e000a10
	jmp	VFP_sn_rd
ARM_instruction_fmsrr:
	mov	edx,0x0c400a10
	jmp	VFP_sm_rd_rn
ARM_instruction_fmstat:
	mov	edx,0x0ef1fa10
	jmp	VFP_fmstat
ARM_instruction_fmuld:
	mov	edx,0x0e200b00
	jmp	VFP_dd_dn_dm
ARM_instruction_fmuls:
	mov	edx,0x0e200a00
	jmp	VFP_sd_sn_sm
ARM_instruction_fmxr:
	mov	edx,0x0ee00a10
	jmp	VFP_sysreg_rd
ARM_instruction_fnegd:
	mov	edx,0x0eb10b40
	jmp	VFP_dd_dm
ARM_instruction_fnegs:
	mov	edx,0x0eb10a40
	jmp	VFP_sd_sm
ARM_instruction_fnmacd:
	mov	edx,0x0e000b40
	jmp	VFP_FLOAT_dd_dn_dm
ARM_instruction_fnmacs:
	mov	edx,0x0e000a40
	jmp	VFP_FLOAT_sd_sn_sm
ARM_instruction_fnmscd:
	mov	edx,0x0e100b40
	jmp	VFP_FLOAT_dd_dn_dm
ARM_instruction_fnmscs:
	mov	edx,0x0e100a40
	jmp	VFP_FLOAT_sd_sn_sm
ARM_instruction_fnmuld:
	mov	edx,0x0e200b40
	jmp	VFP_dd_dn_dm
ARM_instruction_fnmuls:
	mov	edx,0x0e200a40
	jmp	VFP_sd_sn_sm
ARM_instruction_fshtod:
	mov	edx,0x0eba0b40
	jmp	VFP_dd_dd_imm
ARM_instruction_fshtos:
	mov	edx,0x0eba0a40
	jmp	VFP_sd_sd_imm
ARM_instruction_fsitod:
	mov	edx,0x0eb80bc0
	jmp	VFP_dd_sm
ARM_instruction_fsitos:
	mov	edx,0x0eb80ac0
	jmp	VFP_sd_sm
ARM_instruction_fsltod:
	mov	edx,0x0eba0bc0
	jmp	VFP_dd_dd_imm
ARM_instruction_fsltos:
	mov	edx,0x0eba0ac0
	jmp	VFP_sd_sd_imm
ARM_instruction_fsqrtd:
	mov	edx,0x0eb10bc0
	jmp	VFP_dd_dm
ARM_instruction_fsqrts:
	mov	edx,0x0eb10ac0
	jmp	VFP_sd_sm
ARM_instruction_fstd:
	mov	edx,0x0d000b00
	jmp	VFP_dd_rn_offset
ARM_instruction_fstmdbd:
ARM_instruction_fstmfdd:
	mov	edx,0xd000b00
	jmp	VFP_rn_list_d
ARM_instruction_fstmdbx:
ARM_instruction_fstmfdx:
	mov	edx,0xd000b01
	jmp	VFP_rn_list_d
ARM_instruction_fstmdbs:
ARM_instruction_fstmfds:
	mov	edx,0xd000a00
	jmp	VFP_rn_list_s
ARM_instruction_fstmd:
ARM_instruction_fstmiad:
ARM_instruction_fstmead:
	mov	edx,0xc800b00
	jmp	VFP_rn_list_d
ARM_instruction_fstmx:
ARM_instruction_fstmiax:
ARM_instruction_fstmeax:
	mov	edx,0xc800b01
	jmp	VFP_rn_list_d
ARM_instruction_fstms:
ARM_instruction_fstmias:
ARM_instruction_fstmeas:
	mov	edx,0xc800a00
	jmp	VFP_rn_list_s
ARM_instruction_fsts:
	mov	edx,0x0d000a00
	jmp	VFP_sd_rn_offset
ARM_instruction_fsubd:
	mov	edx,0x0e300b40
	jmp	VFP_dd_dn_dm
ARM_instruction_fsubs:
	mov	edx,0x0e300a40
	jmp	VFP_sd_sn_sm
ARM_instruction_ftoshd:
	mov	edx,0x0ebe0b40
	jmp	VFP_dd_dd_imm
ARM_instruction_ftoshs:
	mov	edx,0x0ebe0a40
	jmp	VFP_sd_sd_imm
ARM_instruction_ftosid:
	mov	edx,0x0ebd0b40
	jmp	VFP_sd_dm
ARM_instruction_ftosis:
	mov	edx,0x0ebd0a40
	jmp	VFP_sd_sm
ARM_instruction_ftosizd:
	mov	edx,0x0ebd0bc0
	jmp	VFP_sd_dm
ARM_instruction_ftosizs:
	mov	edx,0x0ebd0ac0
	jmp	VFP_sd_sm
ARM_instruction_ftosld:
	mov	edx,0x0ebe0bc0
	jmp	VFP_dd_dd_imm
ARM_instruction_ftosls:
	mov	edx,0x0ebe0ac0
	jmp	VFP_sd_sd_imm
ARM_instruction_ftouhd:
	mov	edx,0x0ebf0b40
	jmp	VFP_dd_dd_imm
ARM_instruction_ftouhs:
	mov	edx,0x0ebf0a40
	jmp	VFP_sd_sd_imm
ARM_instruction_ftouid:
	mov	edx,0x0ebc0b40
	jmp	VFP_sd_dm
ARM_instruction_ftouis:
	mov	edx,0x0ebc0a40
	jmp	VFP_sd_sm
ARM_instruction_ftouizd:
	mov	edx,0x0ebc0bc0
	jmp	VFP_sd_dm
ARM_instruction_ftouizs:
	mov	edx,0x0ebc0ac0
	jmp	VFP_sd_sm
ARM_instruction_ftould:
	mov	edx,0x0ebf0bc0
	jmp	VFP_dd_dd_imm
ARM_instruction_ftouls:
	mov	edx,0x0ebf0ac0
	jmp	VFP_sd_sd_imm
ARM_instruction_fuhtod:
	mov	edx,0x0ebb0b40
	jmp	VFP_dd_dd_imm
ARM_instruction_fuhtos:
	mov	edx,0x0ebb0a40
	jmp	VFP_sd_sd_imm
ARM_instruction_fuitod:
	mov	edx,0x0eb80b40
	jmp	VFP_dd_sm
ARM_instruction_fuitos:
	mov	edx,0x0eb80a40
	jmp	VFP_sd_sm
ARM_instruction_fultod:
	mov	edx,0x0ebb0bc0
	jmp	VFP_dd_dd_imm
ARM_instruction_fultos:
	mov	edx,0x0ebb0ac0
	jmp	VFP_sd_sd_imm
ARM_instruction_vcvtb.f16.f32:
	mov	edx,0x0eb30a40
	jmp	VFP_sd_sm_HP
ARM_instruction_vcvtb.f32.f16:
	mov	edx,0x0eb20a40
	jmp	VFP_sd_sm_HP
ARM_instruction_vcvtt.f16.f32:
	mov	edx,0x0eb30ac0
	jmp	VFP_sd_sm_HP
ARM_instruction_vcvtt.f32.f16:
	mov	edx,0x0eb20ac0
	jmp	VFP_sd_sm_HP

;xScale jump table

ARM_instruction_mar:
	mov	edx,0x0c400000
	jmp	XSCALE_acc_rdlo_rdhi
ARM_instruction_mia:
	mov	edx,0x0e200010
	jmp	XSCALE_acc_rm_rs
ARM_instruction_miabb:
	mov	edx,0x0e2c0010
	jmp	XSCALE_acc_rm_rs
ARM_instruction_miabt:
	mov	edx,0x0e2d0010
	jmp	XSCALE_acc_rm_rs
ARM_instruction_miaph:
	mov	edx,0x0e280010
	jmp	XSCALE_acc_rm_rs
ARM_instruction_miatb:
	mov	edx,0x0e2e0010
	jmp	XSCALE_acc_rm_rs
ARM_instruction_miatt:
	mov	edx,0x0e2f0010
	jmp	XSCALE_acc_rm_rs
ARM_instruction_mra:
	mov	edx,0x0c500000
	jmp	XSCALE_rdlo_rdhi_acc

;IWMMXT v1 jump table

ARM_instruction_tandcb:
	mov	edx,0x0e13f130
	jmp	IWMMXT_r15
ARM_instruction_tandch:
	mov	edx,0x0e53f130
	jmp	IWMMXT_r15
ARM_instruction_tandcw:
	mov	edx,0x0e93f130
	jmp	IWMMXT_r15
ARM_instruction_tbcstb:
	mov	edx,0x0e400010
	jmp	IWMMXT_wrd_rn
ARM_instruction_tbcsth:
	mov	edx,0x0e400050
	jmp	IWMMXT_wrd_rn
ARM_instruction_tbcstw:
	mov	edx,0x0e400090
	jmp	IWMMXT_wrd_rn
ARM_instruction_textrcb:
	mov	edx,0x0e13f170
	jmp	IWMMXT_r15_imm
ARM_instruction_textrch:
	mov	edx,0x0e53f170
	jmp	IWMMXT_r15_imm
ARM_instruction_textrcw:
	mov	edx,0x0e93f170
	jmp	IWMMXT_r15_imm
ARM_instruction_textrmsb:
	mov	edx,0x0e100078
	jmp	IWMMXT_rd_wrn_imm
ARM_instruction_textrmsh:
	mov	edx,0x0e500078
	jmp	IWMMXT_rd_wrn_imm
ARM_instruction_textrmsw:
	mov	edx,0x0e900078
	jmp	IWMMXT_rd_wrn_imm
ARM_instruction_textrmub:
	mov	edx,0x0e100070
	jmp	IWMMXT_rd_wrn_imm
ARM_instruction_textrmuh:
	mov	edx,0x0e500070
	jmp	IWMMXT_rd_wrn_imm
ARM_instruction_textrmuw:
	mov	edx,0x0e900070
	jmp	IWMMXT_rd_wrn_imm
ARM_instruction_tinsrb:
	mov	edx,0x0e600010
	jmp	IWMMXT_wrd_rn_imm
ARM_instruction_tinsrh:
	mov	edx,0x0e600050
	jmp	IWMMXT_wrd_rn_imm
ARM_instruction_tinsrw:
	mov	edx,0x0e600090
	jmp	IWMMXT_wrd_rn_imm
ARM_instruction_tmcr:
	mov	edx,0x0e000110
	jmp	IWMMXT_wcd_rn
ARM_instruction_tmcrr:
	mov	edx,0x0c400000
	jmp	IWMMXT_wrd_rdlo_rdhi
ARM_instruction_tmia:
	mov	edx,0x0e200010
	jmp	IWMMXT_wrd_rm_rs
ARM_instruction_tmiabb:
	mov	edx,0x0e2c0010
	jmp	IWMMXT_wrd_rm_rs
ARM_instruction_tmiabt:
	mov	edx,0x0e2d0010
	jmp	IWMMXT_wrd_rm_rs
ARM_instruction_tmiaph:
	mov	edx,0x0e280010
	jmp	IWMMXT_wrd_rm_rs
ARM_instruction_tmiatb:
	mov	edx,0x0e2e0010
	jmp	IWMMXT_wrd_rm_rs
ARM_instruction_tmiatt:
	mov	edx,0x0e2f0010
	jmp	IWMMXT_wrd_rm_rs
ARM_instruction_tmovmskb:
	mov	edx,0x0e100030
	jmp	IWMMXT_rd_wrn
ARM_instruction_tmovmskh:
	mov	edx,0x0e500030
	jmp	IWMMXT_rd_wrn
ARM_instruction_tmovmskw:
	mov	edx,0x0e900030
	jmp	IWMMXT_rd_wrn
ARM_instruction_tmrc:
	mov	edx,0x0e100110
	jmp	IWMMXT_rd_wcn
ARM_instruction_tmrrc:
	mov	edx,0x0c500000
	jmp	IWMMXT_rdlo_rdhi_wrn
ARM_instruction_torcb:
	mov	edx,0x0e13f150
	jmp	IWMMXT_r15
ARM_instruction_torch:
	mov	edx,0x0e53f150
	jmp	IWMMXT_r15
ARM_instruction_torcw:
	mov	edx,0x0e93f150
	jmp	IWMMXT_r15
ARM_instruction_waccb:
	mov	edx,0x0e0001c0
	jmp	IWMMXT_wrd_wrn
ARM_instruction_wacch:
	mov	edx,0x0e4001c0
	jmp	IWMMXT_wrd_wrn
ARM_instruction_waccw:
	mov	edx,0x0e8001c0
	jmp	IWMMXT_wrd_wrn
ARM_instruction_waddb:
	mov	edx,0x0e000180
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_waddbss:
	mov	edx,0x0e300180
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_waddbus:
	mov	edx,0x0e100180
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_waddh:
	mov	edx,0x0e400180
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_waddhss:
	mov	edx,0x0e700180
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_waddhus:
	mov	edx,0x0e500180
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_waddw:
	mov	edx,0x0e800180
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_waddwss:
	mov	edx,0x0eb00180
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_waddwus:
	mov	edx,0x0e900180
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_waligni:
	mov	edx,0x0e000020
	jmp	IWMMXT_wrd_wrn_wrm_imm
ARM_instruction_walignr0:
	mov	edx,0x0e800020
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_walignr1:
	mov	edx,0x0e900020
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_walignr2:
	mov	edx,0x0ea00020
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_walignr3:
	mov	edx,0x0eb00020
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wand:
	mov	edx,0x0e200000
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wandn:
	mov	edx,0x0e300000
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wavg2b:
	mov	edx,0x0e800000
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wavg2br:
	mov	edx,0x0e900000
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wavg2h:
	mov	edx,0x0ec00000
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wavg2hr:
	mov	edx,0x0ed00000
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wcmpeqb:
	mov	edx,0x0e000060
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wcmpeqh:
	mov	edx,0x0e400060
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wcmpeqw:
	mov	edx,0x0e800060
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wcmpgtsb:
	mov	edx,0x0e300060
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wcmpgtsh:
	mov	edx,0x0e700060
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wcmpgtsw:
	mov	edx,0x0eb00060
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wcmpgtub:
	mov	edx,0x0e100060
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wcmpgtuh:
	mov	edx,0x0e500060
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wcmpgtuw:
	mov	edx,0x0e900060
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wldrb:
	mov	edx,0x0c100000
	jmp	IWMMXT_wrd_address5
ARM_instruction_wldrd:
	mov	edx,0x0c500100
	jmp	IWMMXT_wrd_address5_reg_offset
ARM_instruction_wldrh:
	mov	edx,0x0c500000
	jmp	IWMMXT_wrd_address5
ARM_instruction_wldrw:
	mov	edx,0x0c100100
	jmp	IWMMXT_wrd_address5
ARM_instruction_wmacs:
	mov	edx,0x0e600100
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wmacsz:
	mov	edx,0x0e700100
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wmacu:
	mov	edx,0x0e400100
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wmacuz:
	mov	edx,0x0e500100
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wmadds:
	mov	edx,0x0ea00100
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wmaddu:
	mov	edx,0x0e800100
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wmaxsb:
	mov	edx,0x0e200160
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wmaxsh:
	mov	edx,0x0e600160
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wmaxsw:
	mov	edx,0x0ea00160
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wmaxub:
	mov	edx,0x0e000160
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wmaxuh:
	mov	edx,0x0e400160
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wmaxuw:
	mov	edx,0x0e800160
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wminsb:
	mov	edx,0x0e300160
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wminsh:
	mov	edx,0x0e700160
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wminsw:
	mov	edx,0x0eb00160
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wminub:
	mov	edx,0x0e100160
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wminuh:
	mov	edx,0x0e500160
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wminuw:
	mov	edx,0x0e900160
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wmov:
	mov	edx,0x0e000000
	jmp	IWMMXT_wrd_wrn_WMOV
ARM_instruction_wmulsl:
	mov	edx,0x0e200100
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wmulsm:
	mov	edx,0x0e300100
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wmulul:
	mov	edx,0x0e000100
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wmulum:
	mov	edx,0x0e100100
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wor:
	mov	edx,0x0e000000
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wpackdss:
	mov	edx,0x0ef00080
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wpackdus:
	mov	edx,0x0ed00080
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wpackhss:
	mov	edx,0x0e700080
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wpackhus:
	mov	edx,0x0e500080
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wpackwss:
	mov	edx,0x0eb00080
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wpackwus:
	mov	edx,0x0e900080
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wrord:
	mov	edx,0x0ef00040
	jmp	IWMMXT_wrd_wrn_param
ARM_instruction_wrordg:
	mov	edx,0x0ef00140
	jmp	IWMMXT_wrd_wrn_wcm
ARM_instruction_wrorh:
	mov	edx,0x0e700040
	jmp	IWMMXT_wrd_wrn_param
ARM_instruction_wrorhg:
	mov	edx,0x0e700140
	jmp	IWMMXT_wrd_wrn_wcm
ARM_instruction_wrorw:
	mov	edx,0x0eb00040
	jmp	IWMMXT_wrd_wrn_param
ARM_instruction_wrorwg:
	mov	edx,0x0eb00140
	jmp	IWMMXT_wrd_wrn_wcm
ARM_instruction_wsadb:
	mov	edx,0x0e000120
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wsadbz:
	mov	edx,0x0e100120
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wsadh:
	mov	edx,0x0e400120
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wsadhz:
	mov	edx,0x0e500120
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wshufh:
	mov	edx,0x0e0001e0
	jmp	IWMMXT_wrd_wrn_imm
ARM_instruction_wslld:
	mov	edx,0x0ed00040
	jmp	IWMMXT_wrd_wrn_param
ARM_instruction_wslldg:
	mov	edx,0x0ed00140
	jmp	IWMMXT_wrd_wrn_wcm
ARM_instruction_wsllh:
	mov	edx,0x0e500040
	jmp	IWMMXT_wrd_wrn_param
ARM_instruction_wsllhg:
	mov	edx,0x0e500140
	jmp	IWMMXT_wrd_wrn_wcm
ARM_instruction_wsllw:
	mov	edx,0x0e900040
	jmp	IWMMXT_wrd_wrn_param
ARM_instruction_wsllwg:
	mov	edx,0x0e900140
	jmp	IWMMXT_wrd_wrn_wcm
ARM_instruction_wsrad:
	mov	edx,0x0ec00040
	jmp	IWMMXT_wrd_wrn_param
ARM_instruction_wsradg:
	mov	edx,0x0ec00140
	jmp	IWMMXT_wrd_wrn_wcm
ARM_instruction_wsrah:
	mov	edx,0x0e400040
	jmp	IWMMXT_wrd_wrn_param
ARM_instruction_wsrahg:
	mov	edx,0x0e400140
	jmp	IWMMXT_wrd_wrn_wcm
ARM_instruction_wsraw:
	mov	edx,0x0e800040
	jmp	IWMMXT_wrd_wrn_param
ARM_instruction_wsrawg:
	mov	edx,0x0e800140
	jmp	IWMMXT_wrd_wrn_wcm
ARM_instruction_wsrld:
	mov	edx,0x0ee00040
	jmp	IWMMXT_wrd_wrn_param
ARM_instruction_wsrldg:
	mov	edx,0x0ee00140
	jmp	IWMMXT_wrd_wrn_wcm
ARM_instruction_wsrlh:
	mov	edx,0x0e600040
	jmp	IWMMXT_wrd_wrn_param
ARM_instruction_wsrlhg:
	mov	edx,0x0e600140
	jmp	IWMMXT_wrd_wrn_wcm
ARM_instruction_wsrlw:
	mov	edx,0x0ea00040
	jmp	IWMMXT_wrd_wrn_param
ARM_instruction_wsrlwg:
	mov	edx,0x0ea00140
	jmp	IWMMXT_wrd_wrn_wcm
ARM_instruction_wstrb:
	mov	edx,0x0c000000
	jmp	IWMMXT_wrd_address5
ARM_instruction_wstrd:
	mov	edx,0x0c400100
	jmp	IWMMXT_wrd_address5_reg_offset
ARM_instruction_wstrh:
	mov	edx,0x0c400000
	jmp	IWMMXT_wrd_address5
ARM_instruction_wstrw:
	mov	edx,0x0c000100
	jmp	IWMMXT_wrd_address5
ARM_instruction_wsubb:
	mov	edx,0x0e0001a0
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wsubbss:
	mov	edx,0x0e3001a0
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wsubbus:
	mov	edx,0x0e1001a0
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wsubh:
	mov	edx,0x0e4001a0
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wsubhss:
	mov	edx,0x0e7001a0
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wsubhus:
	mov	edx,0x0e5001a0
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wsubw:
	mov	edx,0x0e8001a0
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wsubwss:
	mov	edx,0x0eb001a0
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wsubwus:
	mov	edx,0x0e9001a0
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wunpckehsb:
	mov	edx,0x0e2000c0
	jmp	IWMMXT_wrd_wrn
ARM_instruction_wunpckehsh:
	mov	edx,0x0e6000c0
	jmp	IWMMXT_wrd_wrn
ARM_instruction_wunpckehsw:
	mov	edx,0x0ea000c0
	jmp	IWMMXT_wrd_wrn
ARM_instruction_wunpckehub:
	mov	edx,0x0e0000c0
	jmp	IWMMXT_wrd_wrn
ARM_instruction_wunpckehuh:
	mov	edx,0x0e4000c0
	jmp	IWMMXT_wrd_wrn
ARM_instruction_wunpckehuw:
	mov	edx,0x0e8000c0
	jmp	IWMMXT_wrd_wrn
ARM_instruction_wunpckelsb:
	mov	edx,0x0e2000e0
	jmp	IWMMXT_wrd_wrn
ARM_instruction_wunpckelsh:
	mov	edx,0x0e6000e0
	jmp	IWMMXT_wrd_wrn
ARM_instruction_wunpckelsw:
	mov	edx,0x0ea000e0
	jmp	IWMMXT_wrd_wrn
ARM_instruction_wunpckelub:
	mov	edx,0x0e0000e0
	jmp	IWMMXT_wrd_wrn
ARM_instruction_wunpckeluh:
	mov	edx,0x0e4000e0
	jmp	IWMMXT_wrd_wrn
ARM_instruction_wunpckeluw:
	mov	edx,0x0e8000e0
	jmp	IWMMXT_wrd_wrn
ARM_instruction_wunpckihb:
	mov	edx,0x0e1000c0
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wunpckihh:
	mov	edx,0x0e5000c0
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wunpckihw:
	mov	edx,0x0e9000c0
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wunpckilb:
	mov	edx,0x0e1000e0
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wunpckilh:
	mov	edx,0x0e5000e0
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wunpckilw:
	mov	edx,0x0e9000e0
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wxor:
	mov	edx,0x0e100000
	jmp	IWMMXT_wrd_wrn_wrm
ARM_instruction_wzero:
	mov	edx,0x0e300000
	jmp	IWMMXT_wrd
;IWMMXT V2 jump table
ARM_instruction_torvscb:
	mov	edx,0x0e12f190
	jmp	IWMMXT_r15_v2
ARM_instruction_torvsch:
	mov	edx,0x0e52f190
	jmp	IWMMXT_r15_v2
ARM_instruction_torvscw:
	mov	edx,0x0e92f190
	jmp	IWMMXT_r15_v2
ARM_instruction_wabsb:
	mov	edx,0x0e2001c0
	jmp	IWMMXT_rd_rn_v2
ARM_instruction_wabsh:
	mov	edx,0x0e6001c0
	jmp	IWMMXT_rd_rn_v2
ARM_instruction_wabsw:
	mov	edx,0x0ea001c0
	jmp	IWMMXT_rd_rn_v2
ARM_instruction_wabsdiffb:
	mov	edx,0x0e1001c0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wabsdiffh:
	mov	edx,0x0e5001c0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wabsdiffw:
	mov	edx,0x0e9001c0
	jmp	IWMMXT_rd_rn_rm_v2
;;ARM_instruction_waddbhusl:
;;	mov	edx,0x0e2001a0
;;	jmp	IWMMXT_rd_rn_rm_v2
;;ARM_instruction_waddbhusm:
;;	mov	edx,0x0e6001a0
;;	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_waddhc:
	mov	edx,0x0e600180
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_waddwc:
	mov	edx,0x0ea00180
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_waddsubhx:
	mov	edx,0x0ea001a0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wavg4:
	mov	edx,0x0e400000
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wavg4r:
	mov	edx,0x0e500000
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmaddsn:
	mov	edx,0x0ee00100
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmaddsx:
	mov	edx,0x0eb00100
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmaddun:
	mov	edx,0x0ec00100
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmaddux:
	mov	edx,0x0e900100
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmerge:
	mov	edx,0x0e000080
	jmp	IWMMXT_rd_rn_rm_imm
ARM_instruction_wmiabb:
	mov	edx,0x0e0000a0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmiabt:
	mov	edx,0x0e1000a0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmiatb:
	mov	edx,0x0e2000a0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmiatt:
	mov	edx,0x0e3000a0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmiabbn:
	mov	edx,0x0e4000a0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmiabtn:
	mov	edx,0x0e5000a0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmiatbn:
	mov	edx,0x0e6000a0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmiattn:
	mov	edx,0x0e7000a0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmiawbb:
	mov	edx,0x0e800120
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmiawbt:
	mov	edx,0x0e900120
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmiawtb:
	mov	edx,0x0ea00120
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmiawtt:
	mov	edx,0x0eb00120
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmiawbbn:
	mov	edx,0x0ec00120
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmiawbtn:
	mov	edx,0x0ed00120
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmiawtbn:
	mov	edx,0x0ee00120
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmiawttn:
	mov	edx,0x0ef00120
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmulsmr:
	mov	edx,0x0ef00100
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmulumr:
	mov	edx,0x0ed00100
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmulwumr:
	mov	edx,0x0ec000c0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmulwsmr:
	mov	edx,0x0ee000c0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmulwum:
	mov	edx,0x0ed000c0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmulwsm:
	mov	edx,0x0ef000c0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wmulwl:
	mov	edx,0x0eb000c0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wqmiabb:
	mov	edx,0x0e8000a0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wqmiabt:
	mov	edx,0x0e9000a0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wqmiatb:
	mov	edx,0x0ea000a0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wqmiatt:
	mov	edx,0x0eb000a0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wqmiabbn:
	mov	edx,0x0ec000a0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wqmiabtn:
	mov	edx,0x0ed000a0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wqmiatbn:
	mov	edx,0x0ee000a0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wqmiattn:
	mov	edx,0x0ef000a0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wqmulm:
	mov	edx,0x0e100080
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wqmulmr:
	mov	edx,0x0e300080
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wqmulwm:
	mov	edx,0x0ec000e0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wqmulwmr:
	mov	edx,0x0ee000e0
	jmp	IWMMXT_rd_rn_rm_v2
ARM_instruction_wsubaddhx:
	mov	edx,0x0ed001c0
	jmp	IWMMXT_rd_rn_rm_v2

;SIMD int & float jump table, with some aliases to VFP opcodes

ARM_instruction_vaba.s16:
	mov	edx,0xf2100710
	jmp	SIMD_INT_vd_vn_vm
ARM_instruction_vaba.s32:
	mov	edx,0xf2200710
	jmp	SIMD_INT_vd_vn_vm
ARM_instruction_vaba.s8:
	mov	edx,0xf2000710
	jmp	SIMD_INT_vd_vn_vm
ARM_instruction_vaba.u16:
	mov	edx,0xf3100710
	jmp	SIMD_INT_vd_vn_vm
ARM_instruction_vaba.u32:
	mov	edx,0xf3200710
	jmp	SIMD_INT_vd_vn_vm
ARM_instruction_vaba.u8:
	mov	edx,0xf3000710
	jmp	SIMD_INT_vd_vn_vm
ARM_instruction_vabal.s16:
	mov	edx,0xf2900500
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vabal.s32:
	mov	edx,0xf2a00500
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vabal.s8:
	mov	edx,0xf2800500
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vabal.u16:
	mov	edx,0xf3900500
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vabal.u32:
	mov	edx,0xf3a00500
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vabal.u8:
	mov	edx,0xf3800500
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vabd.f32:
	mov	edx,0xf3200d00
	jmp	SIMD_FLOAT_vd_vn_vm_alt
ARM_instruction_vabd.s16:
	mov	edx,0xf2100700
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vabd.s32:
	mov	edx,0xf2200700
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vabd.s8:
	mov	edx,0xf2000700
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vabd.u16:
	mov	edx,0xf3100700
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vabd.u32:
	mov	edx,0xf3200700
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vabd.u8:
	mov	edx,0xf3000700
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vabdl.s16:
	mov	edx,0xf2900700
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vabdl.s32:
	mov	edx,0xf2a00700
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vabdl.s8:
	mov	edx,0xf2800700
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vabdl.u16:
	mov	edx,0xf3900700
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vabdl.u32:
	mov	edx,0xf3a00700
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vabdl.u8:
	mov	edx,0xf3800700
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vabs.f32:
	mov	edx,0xf3b90700
	mov	ecx,0x0eb00ac0
	jmp	SIMD_FLOAT_vd_vm_f32
ARM_instruction_vabs.f64 = ARM_instruction_fabsd
ARM_instruction_vabs.s16:
	mov	edx,0xf3b50300
	jmp	SIMD_INT_vd_vm
ARM_instruction_vabs.s32:
	mov	edx,0xf3b90300
	jmp	SIMD_INT_vd_vm
ARM_instruction_vabs.s8:
	mov	edx,0xf3b10300
	jmp	SIMD_INT_vd_vm
ARM_instruction_vacge.f32:
	mov	edx,0xf3000e10
	jmp	SIMD_FLOAT_vd_vn_vm_alt
ARM_instruction_vacgt.f32:
	mov	edx,0xf3200e10
	jmp	SIMD_FLOAT_vd_vn_vm_alt
ARM_instruction_vacle.f32:
	mov	edx,0xf3000e10
	jmp	SIMD_FLOAT_vd_vm_vn_alt
ARM_instruction_vaclt.f32:
	mov	edx,0xf3200e10
	jmp	SIMD_FLOAT_vd_vm_vn_alt
ARM_instruction_vadd.f32:
	mov	edx,0xf2000d00
	mov	ecx,0x0e300a00
	jmp	SIMD_FLOAT_vd_vn_vm_f32
ARM_instruction_vadd.f64 = ARM_instruction_faddd
ARM_instruction_vadd.i16:
	mov	edx,0xf2100800
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vadd.i32:
	mov	edx,0xf2200800
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vadd.i64:
	mov	edx,0xf2300800
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vadd.i8:
	mov	edx,0xf2000800
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vaddhn.i16:
	mov	edx,0xf2800400
	jmp	SIMD_INT_vd_vn_vm_narrow
ARM_instruction_vaddhn.i32:
	mov	edx,0xf2900400
	jmp	SIMD_INT_vd_vn_vm_narrow
ARM_instruction_vaddhn.i64:
	mov	edx,0xf2a00400
	jmp	SIMD_INT_vd_vn_vm_narrow
ARM_instruction_vaddl.s16:
	mov	edx,0xf2900000
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vaddl.s32:
	mov	edx,0xf2a00000
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vaddl.s8:
	mov	edx,0xf2800000
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vaddl.u16:
	mov	edx,0xf3900000
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vaddl.u32:
	mov	edx,0xf3a00000
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vaddl.u8:
	mov	edx,0xf3800000
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vaddw.s16:
	mov	edx,0xf2900100
	jmp	SIMD_INT_vd_vn_vm_wide
ARM_instruction_vaddw.s32:
	mov	edx,0xf2a00100
	jmp	SIMD_INT_vd_vn_vm_wide
ARM_instruction_vaddw.s8:
	mov	edx,0xf2800100
	jmp	SIMD_INT_vd_vn_vm_wide
ARM_instruction_vaddw.u16:
	mov	edx,0xf3900100
	jmp	SIMD_INT_vd_vn_vm_wide
ARM_instruction_vaddw.u32:
	mov	edx,0xf3a00100
	jmp	SIMD_INT_vd_vn_vm_wide
ARM_instruction_vaddw.u8:
	mov	edx,0xf3800100
	jmp	SIMD_INT_vd_vn_vm_wide
ARM_instruction_vand.i16:
	mov	edx,0xf2800931
	jmp	SIMD_INT_vd_imm
ARM_instruction_vand.i32:
	mov	edx,0xf2800131
	jmp	SIMD_INT_vd_imm
ARM_instruction_vand:
	mov	edx,0xf2000110
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vbic.i16:
	mov	edx,0xf2800930
	jmp	SIMD_INT_vd_imm
ARM_instruction_vbic.i32:
	mov	edx,0xf2800130
	jmp	SIMD_INT_vd_imm
ARM_instruction_vbic:
	mov	edx,0xf2100110
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vbif:
	mov	edx,0xf3300110
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vbit:
	mov	edx,0xf3200110
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vbsl:
	mov	edx,0xf3100110
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vceq.f32:
	mov	edx,0xf2000e00
	mov	ecx,0xf3b90500
	jmp	SIMD_FLOAT_vd_vn_vm_alt_zero
ARM_instruction_vceq.i16:
	mov	edx,0xf3100810
	mov	ecx,0xf3b50100
	jmp	SIMD_INT_vd_vn_vm_alt_zero
ARM_instruction_vceq.i32:
	mov	edx,0xf3200810
	mov	ecx,0xf3b90100
	jmp	SIMD_INT_vd_vn_vm_alt_zero
ARM_instruction_vceq.i8:
	mov	edx,0xf3000810
	mov	ecx,0xf3b10100
	jmp	SIMD_INT_vd_vn_vm_alt_zero
ARM_instruction_vcge.f32:
	mov	edx,0xf3000e00
	mov	ecx,0xf3b90480
	jmp	SIMD_FLOAT_vd_vn_vm_alt_zero
ARM_instruction_vcge.s16:
	mov	edx,0xf2100310
	mov	ecx,0xf3b50080
	jmp	SIMD_INT_vd_vn_vm_alt_zero
ARM_instruction_vcge.s32:
	mov	edx,0xf2200310
	mov	ecx,0xf3b90080
	jmp	SIMD_INT_vd_vn_vm_alt_zero
ARM_instruction_vcge.s8:
	mov	edx,0xf2000310
	mov	ecx,0xf3b10080
	jmp	SIMD_INT_vd_vn_vm_alt_zero
ARM_instruction_vcge.u16:
	mov	edx,0xf3100310
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vcge.u32:
	mov	edx,0xf3200310
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vcge.u8:
	mov	edx,0xf3000310
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vcgt.f32:
	mov	edx,0xf3200e00
	mov	ecx,0xf3b90400
	jmp	SIMD_FLOAT_vd_vn_vm_alt_zero
ARM_instruction_vcgt.s16:
	mov	edx,0xf2100300
	mov	ecx,0xf3b50000
	jmp	SIMD_INT_vd_vn_vm_alt_zero
ARM_instruction_vcgt.s32:
	mov	edx,0xf2200300
	mov	ecx,0xf3b90000
	jmp	SIMD_INT_vd_vn_vm_alt_zero
ARM_instruction_vcgt.s8:
	mov	edx,0xf2000300
	mov	ecx,0xf3b10000
	jmp	SIMD_INT_vd_vn_vm_alt_zero
ARM_instruction_vcgt.u16:
	mov	edx,0xf3100300
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vcgt.u32:
	mov	edx,0xf3200300
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vcgt.u8:
	mov	edx,0xf3000300
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vcle.f32:
	mov	edx,0xf3000e00
	mov	ecx,0xf3b90580
	jmp	SIMD_FLOAT_vd_vn_vm_alt_zero_swap
ARM_instruction_vcle.s16:
	mov	edx,0xf2100310
	mov	ecx,0xf3b50180
	jmp	SIMD_INT_vd_vn_vm_alt_zero_swap
ARM_instruction_vcle.s32:
	mov	edx,0xf2200310
	mov	ecx,0xf3b90180
	jmp	SIMD_INT_vd_vn_vm_alt_zero_swap
ARM_instruction_vcle.s8:
	mov	edx,0xf2000310
	mov	ecx,0xf3b10180
	jmp	SIMD_INT_vd_vn_vm_alt_zero_swap
ARM_instruction_vcle.u16:
	mov	edx,0xf3100310
	jmp	SIMD_INT_vd_vn_vm_alt_swap
ARM_instruction_vcle.u32:
	mov	edx,0xf3200310
	jmp	SIMD_INT_vd_vn_vm_alt_swap
ARM_instruction_vcle.u8:
	mov	edx,0xf3000310
	jmp	SIMD_INT_vd_vn_vm_alt_swap
ARM_instruction_vcls.s16:
	mov	edx,0xf3b40400
	jmp	SIMD_INT_vd_vm
ARM_instruction_vcls.s32:
	mov	edx,0xf3b80400
	jmp	SIMD_INT_vd_vm
ARM_instruction_vcls.s8:
	mov	edx,0xf3b00400
	jmp	SIMD_INT_vd_vm
ARM_instruction_vclt.f32:
	mov	edx,0xf3200e00
	mov	ecx,0xf3b90600
	jmp	SIMD_FLOAT_vd_vn_vm_alt_zero_swap
ARM_instruction_vclt.s16:
	mov	edx,0xf2100300
	mov	ecx,0xf3b50200
	jmp	SIMD_INT_vd_vn_vm_alt_zero_swap
ARM_instruction_vclt.s32:
	mov	edx,0xf2200300
	mov	ecx,0xf3b90200
	jmp	SIMD_INT_vd_vn_vm_alt_zero_swap
ARM_instruction_vclt.s8:
	mov	edx,0xf2000300
	mov	ecx,0xf3b10200
	jmp	SIMD_INT_vd_vn_vm_alt_zero_swap
ARM_instruction_vclt.u16:
	mov	edx,0xf3100300
	jmp	SIMD_INT_vd_vn_vm_alt_swap
ARM_instruction_vclt.u32:
	mov	edx,0xf3200300
	jmp	SIMD_INT_vd_vn_vm_alt_swap
ARM_instruction_vclt.u8:
	mov	edx,0xf3000300
	jmp	SIMD_INT_vd_vn_vm_alt_swap
ARM_instruction_vclz.i16:
	mov	edx,0xf3b40480
	jmp	SIMD_INT_vd_vm
ARM_instruction_vclz.i32:
	mov	edx,0xf3b80480
	jmp	SIMD_INT_vd_vm
ARM_instruction_vclz.i8:
	mov	edx,0xf3b00480
	jmp	SIMD_INT_vd_vm
ARM_instruction_vcmp.f32 = ARM_instruction_fcmps
ARM_instruction_vcmp.f64 = ARM_instruction_fcmpd
ARM_instruction_vcmpe.f32 = ARM_instruction_fcmpes
ARM_instruction_vcmpe.f64 = ARM_instruction_fcmped
ARM_instruction_vcnt.8:
	mov	edx,0xf3b00500
	jmp	SIMD_INT_vd_vm
ARM_instruction_vcvt.f16.f32:
	mov	edx,0xf3b60600
	jmp	SIMD_FLOAT_dd_qm
ARM_instruction_vcvt.f32.f16:
	mov	edx,0xf3b60700
	jmp	SIMD_FLOAT_qd_dm
ARM_instruction_vcvt.f32.f64 = ARM_instruction_fcvtsd
ARM_instruction_vcvt.f32.s16 = ARM_instruction_fshtos
ARM_instruction_vcvt.f32.s32:
	mov	edx,0xf3bb0600
	mov	ecx,0x0eb80ac0				;FSITOS
	jmp	SIMD_FLOAT_vd_vm_imm
ARM_instruction_vcvt.f32.u16 = ARM_instruction_fuhtos
ARM_instruction_vcvt.f32.u32:
	mov	edx,0xf3bb0680
	mov	ecx,0x0eb80a40				;FUITOS
	jmp	SIMD_FLOAT_vd_vm_imm
ARM_instruction_vcvt.f64.f32 = ARM_instruction_fcvtds
ARM_instruction_vcvt.f64.s16 = ARM_instruction_fshtod
ARM_instruction_vcvt.f64.s32:
	mov	edx,0x0eb80bc0				;FSITOD
	mov	ecx,0x0eba0bc0				;FSLTOD
	jmp	SIMD_FLOAT_dd_sm_CVT
ARM_instruction_vcvt.f64.u16 = ARM_instruction_fuhtod
ARM_instruction_vcvt.f64.u32:
	mov	edx,0x0eb80b40				;FUITOD
	mov	ecx,0x0ebb0bc0				;FULTOD
	jmp	SIMD_FLOAT_dd_sm_CVT
ARM_instruction_vcvt.s16.f32 = ARM_instruction_ftoshs
ARM_instruction_vcvt.s16.f64 = ARM_instruction_ftoshd
ARM_instruction_vcvt.s32.f32:
	mov	edx,0xf3bb0700
	mov	ecx,0x0ebd0ac0				;FTOSIZS
	jmp	SIMD_FLOAT_vd_vm_imm
ARM_instruction_vcvt.s32.f64:
	mov	edx,0x0ebd0bc0				;FTOSIZD
	mov	ecx,0x0ebe0bc0				;FTOSLD
	jmp	SIMD_FLOAT_sd_dm_CVT
ARM_instruction_vcvt.u16.f32 = ARM_instruction_ftouhs
ARM_instruction_vcvt.u16.f64 = ARM_instruction_ftouhd
ARM_instruction_vcvt.u32.f32:
	mov	edx,0xf3bb0780
	mov	ecx,0x0ebc0ac0				;FTOUIZS
	jmp	SIMD_FLOAT_vd_vm_imm
ARM_instruction_vcvt.u32.f64:
	mov	edx,0x0ebc0bc0				;FTOUIZD
	mov	ecx,0x0ebf0bc0				;FTOULD
	jmp	SIMD_FLOAT_sd_dm_CVT
ARM_instruction_vcvtr.s32.f32 = ARM_instruction_ftosis
ARM_instruction_vcvtr.s32.f64 = ARM_instruction_ftosid
ARM_instruction_vcvtr.u32.f32 = ARM_instruction_ftouis
ARM_instruction_vcvtr.u32.f64 = ARM_instruction_ftouid
ARM_instruction_vdiv.f32 = ARM_instruction_fdivs
ARM_instruction_vdiv.f64 = ARM_instruction_fdivd
ARM_instruction_vdup.16:
	mov	edx,0xf3b20c00
	mov	ecx,0x0e800b30
	jmp	SIMD_INT_qd_dmx
ARM_instruction_vdup.32:
	mov	edx,0xf3b30c00
	mov	ecx,0x0e800b10
	jmp	SIMD_INT_qd_dmx
ARM_instruction_vdup.8:
	mov	edx,0xf3b10c00
	mov	ecx,0x0ec00b10
	jmp	SIMD_INT_qd_dmx
ARM_instruction_veor:
	mov	edx,0xf3000110
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vext.16:
	mov	edx,0xf2b00200
	jmp	SIMD_INT_vd_vn_vm_imm
ARM_instruction_vext.32:
	mov	edx,0xf2b00400
	jmp	SIMD_INT_vd_vn_vm_imm
ARM_instruction_vext.64:
	mov	edx,0xf2b00800
	jmp	SIMD_INT_vd_vn_vm_imm
ARM_instruction_vext.8:
	mov	edx,0xf2b00100
	jmp	SIMD_INT_vd_vn_vm_imm
ARM_instruction_vhadd.s16:
	mov	edx,0xf2100000
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vhadd.s32:
	mov	edx,0xf2200000
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vhadd.s8:
	mov	edx,0xf2000000
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vhadd.u16:
	mov	edx,0xf3100000
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vhadd.u32:
	mov	edx,0xf3200000
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vhadd.u8:
	mov	edx,0xf3000000
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vhsub.s16:
	mov	edx,0xf2100200
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vhsub.s32:
	mov	edx,0xf2200200
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vhsub.s8:
	mov	edx,0xf2000200
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vhsub.u16:
	mov	edx,0xf3100200
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vhsub.u32:
	mov	edx,0xf3200200
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vhsub.u8:
	mov	edx,0xf3000200
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vld1.16:
	mov	edx,0xf4200040
	jmp	SIMD_INT_list_rn_rm_1
ARM_instruction_vld1.32:
	mov	edx,0xf4200080
	jmp	SIMD_INT_list_rn_rm_1
ARM_instruction_vld1.64:
	mov	edx,0xf42000c0
	jmp	SIMD_INT_list_rn_rm_1
ARM_instruction_vld1.8:
	mov	edx,0xf4200000
	jmp	SIMD_INT_list_rn_rm_1
ARM_instruction_vld2.16:
	mov	edx,0xf4200040
	jmp	SIMD_INT_list_rn_rm_2
ARM_instruction_vld2.32:
	mov	edx,0xf4200080
	jmp	SIMD_INT_list_rn_rm_2
ARM_instruction_vld2.8:
	mov	edx,0xf4200000
	jmp	SIMD_INT_list_rn_rm_2
ARM_instruction_vld3.16:
	mov	edx,0xf4200040
	jmp	SIMD_INT_list_rn_rm_3
ARM_instruction_vld3.32:
	mov	edx,0xf4200080
	jmp	SIMD_INT_list_rn_rm_3
ARM_instruction_vld3.8:
	mov	edx,0xf4200000
	jmp	SIMD_INT_list_rn_rm_3
ARM_instruction_vld4.16:
	mov	edx,0xf4200040
	jmp	SIMD_INT_list_rn_rm_4
ARM_instruction_vld4.32:
	mov	edx,0xf4200080
	jmp	SIMD_INT_list_rn_rm_4
ARM_instruction_vld4.8:
	mov	edx,0xf4200000
	jmp	SIMD_INT_list_rn_rm_4
ARM_instruction_vldm.32 = ARM_instruction_fldmias
ARM_instruction_vldm.64 = ARM_instruction_fldmiad
ARM_instruction_vldmdb.32 = ARM_instruction_fldmdbs
ARM_instruction_vldmdb.64 = ARM_instruction_fldmdbd
ARM_instruction_vldmfd.32 = ARM_instruction_fldmfds
ARM_instruction_vldmfd.64 = ARM_instruction_fldmfdd
ARM_instruction_vldmea.32 = ARM_instruction_fldmeas
ARM_instruction_vldmea.64 = ARM_instruction_fldmead
ARM_instruction_vldmia.32 = ARM_instruction_fldmias
ARM_instruction_vldmia.64 = ARM_instruction_fldmiad
ARM_instruction_vldmea:
ARM_instruction_vldmdb:
	mov	edx,0xd100a00
	jmp	SIMD_INT_rn_list
ARM_instruction_vldm:
ARM_instruction_vldmfd:
ARM_instruction_vldmia:
	mov	edx,0xc900a00
	jmp	SIMD_INT_rn_list
ARM_instruction_vldr.32 = ARM_instruction_flds
ARM_instruction_vldr.64 = ARM_instruction_fldd
ARM_instruction_vldr:
	mov	edx,0x0d100a00
	jmp	SIMD_INT_vd_rn_offset
ARM_instruction_vmax.f32:
	mov	edx,0xf2000f00
	jmp	SIMD_FLOAT_vd_vn_vm_alt
ARM_instruction_vmax.s16:
	mov	edx,0xf2100600
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vmax.s32:
	mov	edx,0xf2200600
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vmax.s8:
	mov	edx,0xf2000600
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vmax.u16:
	mov	edx,0xf3100600
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vmax.u32:
	mov	edx,0xf3200600
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vmax.u8:
	mov	edx,0xf3000600
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vmin.f32:
	mov	edx,0xf2200f00
	jmp	SIMD_FLOAT_vd_vn_vm_alt
ARM_instruction_vmin.s16:
	mov	edx,0xf2100610
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vmin.s32:
	mov	edx,0xf2200610
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vmin.s8:
	mov	edx,0xf2000610
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vmin.u16:
	mov	edx,0xf3100610
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vmin.u32:
	mov	edx,0xf3200610
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vmin.u8:
	mov	edx,0xf3000610
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vmla.f32:
	mov	edx,0xf2000d10
	mov	ecx,0x0e000a00
	jmp	SIMD_FLOAT_vd_vn_vmx_f32
ARM_instruction_vmla.f64 = ARM_instruction_fmacd
ARM_instruction_vmla.i16:
	mov	edx,0xf2100900
	jmp	SIMD_INT_vd_vn_vmx
ARM_instruction_vmla.i32:
	mov	edx,0xf2200900
	jmp	SIMD_INT_vd_vn_vmx
ARM_instruction_vmla.i8:
	mov	edx,0xf2000900
	jmp	SIMD_INT_vd_vn_vm
ARM_instruction_vmla.s16 = ARM_instruction_vmla.i16
ARM_instruction_vmla.s32 = ARM_instruction_vmla.i32
ARM_instruction_vmla.s8 = ARM_instruction_vmla.i8
ARM_instruction_vmla.u16 = ARM_instruction_vmla.i16
ARM_instruction_vmla.u32 = ARM_instruction_vmla.i32
ARM_instruction_vmla.u8 = ARM_instruction_vmla.i8
ARM_instruction_vmlal.s16:
	mov	edx,0xf2900800
	mov	ecx,0xf2900240
	jmp	SIMD_INT_vd_vn_vmx_long
ARM_instruction_vmlal.s32:
	mov	edx,0xf2a00800
	mov	ecx,0xf2a00240
	jmp	SIMD_INT_vd_vn_vmx_long
ARM_instruction_vmlal.s8:
	mov	edx,0xf2800800
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vmlal.u16:
	mov	edx,0xf3900800
	mov	ecx,0xf3900240
	jmp	SIMD_INT_vd_vn_vmx_long
ARM_instruction_vmlal.u32:
	mov	edx,0xf3a00800
	mov	ecx,0xf3a00240
	jmp	SIMD_INT_vd_vn_vmx_long
ARM_instruction_vmlal.u8:
	mov	edx,0xf3800800
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vmls.f32:
	mov	edx,0xf2200d10
	mov	ecx,0x0e000a40
	jmp	SIMD_FLOAT_vd_vn_vmx_f32
ARM_instruction_vmls.f64 = ARM_instruction_fnmacd
ARM_instruction_vmls.i16:
	mov	edx,0xf3100900
	jmp	SIMD_INT_vd_vn_vmx
ARM_instruction_vmls.i32:
	mov	edx,0xf3200900
	jmp	SIMD_INT_vd_vn_vmx
ARM_instruction_vmls.i8:
	mov	edx,0xf3000900
	jmp	SIMD_INT_vd_vn_vm
ARM_instruction_vmls.s16 = ARM_instruction_vmls.i16
ARM_instruction_vmls.s32 = ARM_instruction_vmls.i32
ARM_instruction_vmls.s8 = ARM_instruction_vmls.i8
ARM_instruction_vmls.u16 = ARM_instruction_vmls.i16
ARM_instruction_vmls.u32 = ARM_instruction_vmls.i32
ARM_instruction_vmls.u8 = ARM_instruction_vmls.i8
ARM_instruction_vmlsl.s16:
	mov	edx,0xf2900a00
	mov	ecx,0xf2900640
	jmp	SIMD_INT_vd_vn_vmx_long
ARM_instruction_vmlsl.s32:
	mov	edx,0xf2a00a00
	mov	ecx,0xf2a00640
	jmp	SIMD_INT_vd_vn_vmx_long
ARM_instruction_vmlsl.s8:
	mov	edx,0xf2800a00
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vmlsl.u16:
	mov	edx,0xf3900a00
	mov	ecx,0xf3900640
	jmp	SIMD_INT_vd_vn_vmx_long
ARM_instruction_vmlsl.u32:
	mov	edx,0xf3a00a00
	mov	ecx,0xf3a00640
	jmp	SIMD_INT_vd_vn_vmx_long
ARM_instruction_vmlsl.u8:
	mov	edx,0xf3800a00
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vmov.16:
	mov	edx,0x0e000b30
	jmp	SIMD_INT_ddx_rn
ARM_instruction_vmov.32:
	mov	edx,0x0e000b10
	jmp	SIMD_INT_rdd_rdn
ARM_instruction_vmov.8:
	mov	edx,0x0e400b10
	jmp	SIMD_INT_ddx_rn
ARM_instruction_vmov.f32:
	mov	edx,0x0eb00a00				;FCONSTS
	mov	ecx,0xf2800f10
	jmp	SIMD_FLOAT_sm_imm
ARM_instruction_vmov.f64:
	mov	edx,0x0eb00b00				;FCONSTD
	jmp	SIMD_FLOAT_dm_imm
ARM_instruction_vmov.i16:
	mov	edx,0xf2800810
	jmp	SIMD_INT_vd_imm_i16
ARM_instruction_vmov.i32:
	mov	edx,0xf2800010
	jmp	SIMD_INT_vd_imm_i32
ARM_instruction_vmov.i64:
	mov	edx,0xf2800e30
	jmp	SIMD_INT_vd_imm_i8
ARM_instruction_vmov.i8:
	mov	edx,0xf2800e10
	jmp	SIMD_INT_vd_imm_i8
ARM_instruction_vmov.s16:
	mov	edx,0x0e100b30
	jmp	SIMD_INT_rd_dnx
ARM_instruction_vmov.s8:
	mov	edx,0x0e500b10
	jmp	SIMD_INT_rd_dnx
ARM_instruction_vmov.u16:
	mov	edx,0x0e900b30
	jmp	SIMD_INT_rd_dnx
ARM_instruction_vmov.u8:
	mov	edx,0x0ed00b10
	jmp	SIMD_INT_rd_dnx
ARM_instruction_vmov:
	jmp	SIMD_INT_MOV
ARM_instruction_vmovl.s16:
	mov	edx,0xf2900a10
	jmp	SIMD_INT_qd_dm
ARM_instruction_vmovl.s32:
	mov	edx,0xf2a00a10
	jmp	SIMD_INT_qd_dm
ARM_instruction_vmovl.s8:
	mov	edx,0xf2880a10
	jmp	SIMD_INT_qd_dm
ARM_instruction_vmovl.u16:
	mov	edx,0xf3900a10
	jmp	SIMD_INT_qd_dm
ARM_instruction_vmovl.u32:
	mov	edx,0xf3a00a10
	jmp	SIMD_INT_qd_dm
ARM_instruction_vmovl.u8:
	mov	edx,0xf3880a10
	jmp	SIMD_INT_qd_dm
ARM_instruction_vmovn.i16:
	mov	edx,0xf3b60200
	jmp	SIMD_INT_dd_qm
ARM_instruction_vmovn.i32:
	mov	edx,0xf3ba0200
	jmp	SIMD_INT_dd_qm
ARM_instruction_vmovn.i8:
	mov	edx,0xf3b20200
	jmp	SIMD_INT_dd_qm
ARM_instruction_vmrs = ARM_instruction_fmrx
ARM_instruction_vmsr = ARM_instruction_fmxr
ARM_instruction_vmul.f32:
	mov	edx,0xf3000d10
	mov	ecx,0x0e200a00
	jmp	SIMD_FLOAT_vd_vn_vmx_alt_f32
ARM_instruction_vmul.f64 = ARM_instruction_fmuld
ARM_instruction_vmul.i16:
	mov	edx,0xf2100910
	mov	ecx,0xf2900840
	jmp	SIMD_INT_vd_vn_vmx_alt
ARM_instruction_vmul.i32:
	mov	edx,0xf2200910
	mov	ecx,0xf2a00840
	jmp	SIMD_INT_vd_vn_vmx_alt
ARM_instruction_vmul.i8:
	mov	edx,0xf2000910
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vmul.p8:
	mov	edx,0xf3000910
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vmul.s16:
	mov	edx,0xf2100910
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vmul.s32:
	mov	edx,0xf2200910
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vmul.s8 = ARM_instruction_vmul.i8
ARM_instruction_vmul.u16 = ARM_instruction_vmul.s16
ARM_instruction_vmul.u32 = ARM_instruction_vmul.s32
ARM_instruction_vmul.u8 = ARM_instruction_vmul.i8
ARM_instruction_vmull.p8:
	mov	edx,0xf3800e00
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vmull.s16:
	mov	edx,0xf2900c00
	mov	ecx,0xf2900a40
	jmp	SIMD_INT_vd_vn_vmx_long
ARM_instruction_vmull.s32:
	mov	edx,0xf2a00c00
	mov	ecx,0xf2a00a40
	jmp	SIMD_INT_vd_vn_vmx_long
ARM_instruction_vmull.s8:
	mov	edx,0xf2800c00
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vmull.u16:
	mov	edx,0xf3900c00
	mov	ecx,0xf3900a40
	jmp	SIMD_INT_vd_vn_vmx_long
ARM_instruction_vmull.u32:
	mov	edx,0xf3a00c00
	mov	ecx,0xf3a00a40
	jmp	SIMD_INT_vd_vn_vmx_long
ARM_instruction_vmull.u8:
	mov	edx,0xf3800c00
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vmvn.i16:
	mov	edx,0xf2800830
	jmp	SIMD_INT_vd_imm_i16
ARM_instruction_vmvn.i32:
	mov	edx,0xf2800030
	jmp	SIMD_INT_vd_imm_i32
ARM_instruction_vmvn:
	mov	edx,0xf3b00580
	jmp	SIMD_INT_vd_vm
ARM_instruction_vneg.f32:
	mov	edx,0xf3b90780
	mov	ecx,0x0eb10a40
	jmp	SIMD_FLOAT_vd_vm_f32
ARM_instruction_vneg.f64 = ARM_instruction_fnegd
ARM_instruction_vneg.s16:
	mov	edx,0xf3b50380
	jmp	SIMD_INT_vd_vm
ARM_instruction_vneg.s32:
	mov	edx,0xf3b90380
	jmp	SIMD_INT_vd_vm
ARM_instruction_vneg.s8:
	mov	edx,0xf3b10380
	jmp	SIMD_INT_vd_vm
ARM_instruction_vnmla.f32 = ARM_instruction_fnmscs
ARM_instruction_vnmla.f64 = ARM_instruction_fnmscd
ARM_instruction_vnmls.f32 = ARM_instruction_fmscs
ARM_instruction_vnmls.f64 = ARM_instruction_fmscd
ARM_instruction_vnmul.f32 = ARM_instruction_fnmuls
ARM_instruction_vnmul.f64 = ARM_instruction_fnmuld
ARM_instruction_vorn.i16:
	mov	edx,0xf2800911
	jmp	SIMD_INT_vd_imm
ARM_instruction_vorn.i32:
	mov	edx,0xf2800111
	jmp	SIMD_INT_vd_imm
ARM_instruction_vorn:
	mov	edx,0xf2300110
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vorr.i16:
	mov	edx,0xf2800910
	jmp	SIMD_INT_vd_imm
ARM_instruction_vorr.i32:
	mov	edx,0xf2800110
	jmp	SIMD_INT_vd_imm
ARM_instruction_vorr:
	mov	edx,0xf2200110
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vpadal.s16:
	mov	edx,0xf3b40600
	jmp	SIMD_INT_vd_vm
ARM_instruction_vpadal.s32:
	mov	edx,0xf3b80600
	jmp	SIMD_INT_vd_vm
ARM_instruction_vpadal.s8:
	mov	edx,0xf3b00600
	jmp	SIMD_INT_vd_vm
ARM_instruction_vpadal.u16:
	mov	edx,0xf3b40680
	jmp	SIMD_INT_vd_vm
ARM_instruction_vpadal.u32:
	mov	edx,0xf3b80680
	jmp	SIMD_INT_vd_vm
ARM_instruction_vpadal.u8:
	mov	edx,0xf3b00680
	jmp	SIMD_INT_vd_vm
ARM_instruction_vpadd.f32:
	mov	edx,0xf3000d00
	jmp	SIMD_FLOAT_dd_dn_dm_alt
ARM_instruction_vpadd.i16:
	mov	edx,0xf2100b10
	jmp	SIMD_INT_dd_dn_dm_alt
ARM_instruction_vpadd.i32:
	mov	edx,0xf2200b10
	jmp	SIMD_INT_dd_dn_dm_alt
ARM_instruction_vpadd.i8:
	mov	edx,0xf2000b10
	jmp	SIMD_INT_dd_dn_dm_alt
ARM_instruction_vpaddl.s16:
	mov	edx,0xf3b40200
	jmp	SIMD_INT_vd_vm
ARM_instruction_vpaddl.s32:
	mov	edx,0xf3b80200
	jmp	SIMD_INT_vd_vm
ARM_instruction_vpaddl.s8:
	mov	edx,0xf3b00200
	jmp	SIMD_INT_vd_vm
ARM_instruction_vpaddl.u16:
	mov	edx,0xf3b40280
	jmp	SIMD_INT_vd_vm
ARM_instruction_vpaddl.u32:
	mov	edx,0xf3b80280
	jmp	SIMD_INT_vd_vm
ARM_instruction_vpaddl.u8:
	mov	edx,0xf3b00280
	jmp	SIMD_INT_vd_vm
ARM_instruction_vpmax.f32:
	mov	edx,0xf3000f00
	jmp	SIMD_FLOAT_dd_dn_dm_alt
ARM_instruction_vpmax.s16:
	mov	edx,0xf2100a00
	jmp	SIMD_INT_dd_dn_dm_alt
ARM_instruction_vpmax.s32:
	mov	edx,0xf2200a00
	jmp	SIMD_INT_dd_dn_dm_alt
ARM_instruction_vpmax.s8:
	mov	edx,0xf2000a00
	jmp	SIMD_INT_dd_dn_dm_alt
ARM_instruction_vpmax.u16:
	mov	edx,0xf3100a00
	jmp	SIMD_INT_dd_dn_dm_alt
ARM_instruction_vpmax.u32:
	mov	edx,0xf3200a00
	jmp	SIMD_INT_dd_dn_dm_alt
ARM_instruction_vpmax.u8:
	mov	edx,0xf3000a00
	jmp	SIMD_INT_dd_dn_dm_alt
ARM_instruction_vpmin.f32:
	mov	edx,0xf3200f00
	jmp	SIMD_FLOAT_dd_dn_dm_alt
ARM_instruction_vpmin.s16:
	mov	edx,0xf2100a10
	jmp	SIMD_INT_dd_dn_dm_alt
ARM_instruction_vpmin.s32:
	mov	edx,0xf2200a10
	jmp	SIMD_INT_dd_dn_dm_alt
ARM_instruction_vpmin.s8:
	mov	edx,0xf2000a10
	jmp	SIMD_INT_dd_dn_dm_alt
ARM_instruction_vpmin.u16:
	mov	edx,0xf3100a10
	jmp	SIMD_INT_dd_dn_dm_alt
ARM_instruction_vpmin.u32:
	mov	edx,0xf3200a10
	jmp	SIMD_INT_dd_dn_dm_alt
ARM_instruction_vpmin.u8:
	mov	edx,0xf3000a10
	jmp	SIMD_INT_dd_dn_dm_alt
ARM_instruction_vpop:
	mov	edx,0x0c900a00
	jmp	SIMD_INT_list
ARM_instruction_vpush:
	mov	edx,0x0d000a00
	jmp	SIMD_INT_list
ARM_instruction_vqabs.s16:
	mov	edx,0xf3b40700
	jmp	SIMD_INT_vd_vm
ARM_instruction_vqabs.s32:
	mov	edx,0xf3b80700
	jmp	SIMD_INT_vd_vm
ARM_instruction_vqabs.s8:
	mov	edx,0xf3b00700
	jmp	SIMD_INT_vd_vm
ARM_instruction_vqadd.s16:
	mov	edx,0xf2100010
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqadd.s32:
	mov	edx,0xf2200010
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqadd.s64:
	mov	edx,0xf2300010
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqadd.s8:
	mov	edx,0xf2000010
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqadd.u16:
	mov	edx,0xf3100010
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqadd.u32:
	mov	edx,0xf3200010
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqadd.u64:
	mov	edx,0xf3300010
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqadd.u8:
	mov	edx,0xf3000010
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqdmlal.s16:
	mov	edx,0xf2900900
	mov	ecx,0xf2900340
	jmp	SIMD_INT_vd_vn_vmx_long
ARM_instruction_vqdmlal.s32:
	mov	edx,0xf2a00900
	mov	ecx,0xf2a00340
	jmp	SIMD_INT_vd_vn_vmx_long
ARM_instruction_vqdmlsl.s16:
	mov	edx,0xf2900b00
	mov	ecx,0xf2900740
	jmp	SIMD_INT_vd_vn_vmx_long
ARM_instruction_vqdmlsl.s32:
	mov	edx,0xf2a00b00
	mov	ecx,0xf2a00740
	jmp	SIMD_INT_vd_vn_vmx_long
ARM_instruction_vqdmulh.s16:
	mov	edx,0xf2100b00
	mov	ecx,0xf2900c40
	jmp	SIMD_INT_vd_vn_vmx_alt
ARM_instruction_vqdmulh.s32:
	mov	edx,0xf2200b00
	mov	ecx,0xf2a00c40
	jmp	SIMD_INT_vd_vn_vmx_alt
ARM_instruction_vqdmull.s16:
	mov	edx,0xf2900d00
	mov	ecx,0xf2900b40
	jmp	SIMD_INT_vd_vn_vmx_long
ARM_instruction_vqdmull.s32:
	mov	edx,0xf2a00d00
	mov	ecx,0xf2a00b40
	jmp	SIMD_INT_vd_vn_vmx_long
ARM_instruction_vqmovn.s16:
	mov	edx,0xf3b20280
	jmp	SIMD_INT_dd_qm
ARM_instruction_vqmovn.s32:
	mov	edx,0xf3b60280
	jmp	SIMD_INT_dd_qm
ARM_instruction_vqmovn.s64:
	mov	edx,0xf3ba0280
	jmp	SIMD_INT_dd_qm
ARM_instruction_vqmovn.u16:
	mov	edx,0xf3b202c0
	jmp	SIMD_INT_dd_qm
ARM_instruction_vqmovn.u32:
	mov	edx,0xf3b602c0
	jmp	SIMD_INT_dd_qm
ARM_instruction_vqmovn.u64:
	mov	edx,0xf3ba02c0
	jmp	SIMD_INT_dd_qm
ARM_instruction_vqmovun.s16:
	mov	edx,0xf3b20240
	jmp	SIMD_INT_dd_qm
ARM_instruction_vqmovun.s32:
	mov	edx,0xf3b60240
	jmp	SIMD_INT_dd_qm
ARM_instruction_vqmovun.s64:
	mov	edx,0xf3ba0240
	jmp	SIMD_INT_dd_qm
ARM_instruction_vqneg.s16:
	mov	edx,0xf3b40780
	jmp	SIMD_INT_vd_vm
ARM_instruction_vqneg.s32:
	mov	edx,0xf3b80780
	jmp	SIMD_INT_vd_vm
ARM_instruction_vqneg.s8:
	mov	edx,0xf3b00780
	jmp	SIMD_INT_vd_vm
ARM_instruction_vqrdmulh.s16:
	mov	edx,0xf3100b00
	mov	ecx,0xf2900d40
	jmp	SIMD_INT_vd_vn_vmx_alt
ARM_instruction_vqrdmulh.s32:
	mov	edx,0xf3200b00
	mov	ecx,0xf2a00d40
	jmp	SIMD_INT_vd_vn_vmx_alt
ARM_instruction_vqrshl.s16:
	mov	edx,0xf2100510
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqrshl.s32:
	mov	edx,0xf2200510
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqrshl.s64:
	mov	edx,0xf2300510
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqrshl.s8:
	mov	edx,0xf2000510
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqrshl.u16:
	mov	edx,0xf3100510
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqrshl.u32:
	mov	edx,0xf3200510
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqrshl.u64:
	mov	edx,0xf3300510
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqrshl.u8:
	mov	edx,0xf3000510
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqrshrn.s16:
	mov	edx,0xf2880950
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vqrshrn.s32:
	mov	edx,0xf2900950
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vqrshrn.s64:
	mov	edx,0xf2a00950
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vqrshrn.u16:
	mov	edx,0xf3880950
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vqrshrn.u32:
	mov	edx,0xf3900950
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vqrshrn.u64:
	mov	edx,0xf3a00950
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vqrshrun.s16:
	mov	edx,0xf3880850
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vqrshrun.s32:
	mov	edx,0xf3900850
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vqrshrun.s64:
	mov	edx,0xf3a00850
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vqshl.s16:
	mov	edx,0xf2100410
	mov	ecx,0xf2900710
	jmp	SIMD_INT_vd_vn_vm_imm_alt
ARM_instruction_vqshl.s32:
	mov	edx,0xf2200410
	mov	ecx,0xf2a00710
	jmp	SIMD_INT_vd_vn_vm_imm_alt
ARM_instruction_vqshl.s64:
	mov	edx,0xf2300410
	mov	ecx,0xf2800790
	jmp	SIMD_INT_vd_vn_vm_imm_alt
ARM_instruction_vqshl.s8:
	mov	edx,0xf2000410
	mov	ecx,0xf2880710
	jmp	SIMD_INT_vd_vn_vm_imm_alt
ARM_instruction_vqshl.u16:
	mov	edx,0xf3100410
	mov	ecx,0xf3900710
	jmp	SIMD_INT_vd_vn_vm_imm_alt
ARM_instruction_vqshl.u32:
	mov	edx,0xf3200410
	mov	ecx,0xf3a00710
	jmp	SIMD_INT_vd_vn_vm_imm_alt
ARM_instruction_vqshl.u64:
	mov	edx,0xf3300410
	mov	ecx,0xf3800790
	jmp	SIMD_INT_vd_vn_vm_imm_alt
ARM_instruction_vqshl.u8:
	mov	edx,0xf3000410
	mov	ecx,0xf3880710
	jmp	SIMD_INT_vd_vn_vm_imm_alt
ARM_instruction_vqshlu.s16:
	mov	edx,0xf3900610
	jmp	SIMD_INT_vd_vm_imm_alt
ARM_instruction_vqshlu.s32:
	mov	edx,0xf3a00610
	jmp	SIMD_INT_vd_vm_imm_alt
ARM_instruction_vqshlu.s64:
	mov	edx,0xf3800690
	jmp	SIMD_INT_vd_vm_imm_alt
ARM_instruction_vqshlu.s8:
	mov	edx,0xf3880610
	jmp	SIMD_INT_vd_vm_imm_alt
ARM_instruction_vqshrn.s16:
	mov	edx,0xf2880910
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vqshrn.s32:
	mov	edx,0xf2900910
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vqshrn.s64:
	mov	edx,0xf2a00910
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vqshrn.u16:
	mov	edx,0xf3880910
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vqshrn.u32:
	mov	edx,0xf3900910
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vqshrn.u64:
	mov	edx,0xf3a00910
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vqshrun.s16:
	mov	edx,0xf3880810
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vqshrun.s32:
	mov	edx,0xf3900810
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vqshrun.s64:
	mov	edx,0xf3a00810
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vqsub.s16:
	mov	edx,0xf2100210
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqsub.s32:
	mov	edx,0xf2200210
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqsub.s64:
	mov	edx,0xf2300210
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqsub.s8:
	mov	edx,0xf2000210
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqsub.u16:
	mov	edx,0xf3100210
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqsub.u32:
	mov	edx,0xf3200210
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqsub.u64:
	mov	edx,0xf3300210
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vqsub.u8:
	mov	edx,0xf3000210
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vraddhn.i16:
	mov	edx,0xf3800400
	jmp	SIMD_INT_vd_vn_vm_narrow
ARM_instruction_vraddhn.i32:
	mov	edx,0xf3900400
	jmp	SIMD_INT_vd_vn_vm_narrow
ARM_instruction_vraddhn.i64:
	mov	edx,0xf3a00400
	jmp	SIMD_INT_vd_vn_vm_narrow
ARM_instruction_vrecpe.f32:
	mov	edx,0xf3bb0500
	jmp	SIMD_FLOAT_vd_vm
ARM_instruction_vrecpe.u32:
	mov	edx,0xf3bb0400
	jmp	SIMD_INT_vd_vm
ARM_instruction_vrecps.f32:
	mov	edx,0xf2000f10
	jmp	SIMD_FLOAT_vd_vn_vm_alt
ARM_instruction_vrev16.8:
	mov	edx,0xf3b00100
	jmp	SIMD_INT_vd_vm
ARM_instruction_vrev32.16:
	mov	edx,0xf3b40080
	jmp	SIMD_INT_vd_vm
ARM_instruction_vrev32.8:
	mov	edx,0xf3b00080
	jmp	SIMD_INT_vd_vm
ARM_instruction_vrev64.16:
	mov	edx,0xf3b40000
	jmp	SIMD_INT_vd_vm
ARM_instruction_vrev64.32:
	mov	edx,0xf3b80000
	jmp	SIMD_INT_vd_vm
ARM_instruction_vrev64.8:
	mov	edx,0xf3b00000
	jmp	SIMD_INT_vd_vm
ARM_instruction_vrhadd.s16:
	mov	edx,0xf2100100
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vrhadd.s32:
	mov	edx,0xf2200100
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vrhadd.s8:
	mov	edx,0xf2000100
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vrhadd.u16:
	mov	edx,0xf3100100
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vrhadd.u32:
	mov	edx,0xf3200100
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vrhadd.u8:
	mov	edx,0xf3000100
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vrshl.s16:
	mov	edx,0xf2100500
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vrshl.s32:
	mov	edx,0xf2200500
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vrshl.s64:
	mov	edx,0xf2300500
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vrshl.s8:
	mov	edx,0xf2000500
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vrshl.u16:
	mov	edx,0xf3100500
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vrshl.u32:
	mov	edx,0xf3200500
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vrshl.u64:
	mov	edx,0xf3300500
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vrshl.u8:
	mov	edx,0xf3000500
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vrshr.s16:
	mov	edx,0xf2900210
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vrshr.s32:
	mov	edx,0xf2a00210
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vrshr.s64:
	mov	edx,0xf2800290
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vrshr.s8:
	mov	edx,0xf2880210
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vrshr.u16:
	mov	edx,0xf3900210
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vrshr.u32:
	mov	edx,0xf3a00210
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vrshr.u64:
	mov	edx,0xf3800290
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vrshr.u8:
	mov	edx,0xf3880210
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vrshrn.i16:
	mov	edx,0xf2880850
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vrshrn.i32:
	mov	edx,0xf2900850
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vrshrn.i64:
	mov	edx,0xf2a00850
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vrsqrte.f32:
	mov	edx,0xf3bb0580
	jmp	SIMD_FLOAT_vd_vm
ARM_instruction_vrsqrte.u32:
	mov	edx,0xf3bb0480
	jmp	SIMD_INT_vd_vm
ARM_instruction_vrsqrts.f32:
	mov	edx,0xf2200f10
	jmp	SIMD_FLOAT_vd_vn_vm_alt
ARM_instruction_vrsra.s16:
	mov	edx,0xf2900310
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vrsra.s32:
	mov	edx,0xf2a00310
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vrsra.s64:
	mov	edx,0xf2800390
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vrsra.s8:
	mov	edx,0xf2880310
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vrsra.u16:
	mov	edx,0xf3900310
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vrsra.u32:
	mov	edx,0xf3a00310
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vrsra.u64:
	mov	edx,0xf3800390
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vrsra.u8:
	mov	edx,0xf3880310
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vrsubhn.i16:
	mov	edx,0xf3800600
	jmp	SIMD_INT_vd_vn_vm_narrow
ARM_instruction_vrsubhn.i32:
	mov	edx,0xf3900600
	jmp	SIMD_INT_vd_vn_vm_narrow
ARM_instruction_vrsubhn.i64:
	mov	edx,0xf3a00600
	jmp	SIMD_INT_vd_vn_vm_narrow
ARM_instruction_vshl.i16:
	mov	edx,0xf2900510
	jmp	SIMD_INT_vd_vm_imm_alt
ARM_instruction_vshl.i32:
	mov	edx,0xf2a00510
	jmp	SIMD_INT_vd_vm_imm_alt
ARM_instruction_vshl.i64:
	mov	edx,0xf2800590
	jmp	SIMD_INT_vd_vm_imm_alt
ARM_instruction_vshl.i8:
	mov	edx,0xf2880510
	jmp	SIMD_INT_vd_vm_imm_alt
ARM_instruction_vshl.s16:
	mov	edx,0xf2100400
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vshl.s32:
	mov	edx,0xf2200400
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vshl.s64:
	mov	edx,0xf2300400
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vshl.s8:
	mov	edx,0xf2000400
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vshl.u16:
	mov	edx,0xf3100400
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vshl.u32:
	mov	edx,0xf3200400
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vshl.u64:
	mov	edx,0xf3300400
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vshl.u8:
	mov	edx,0xf3000400
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vshll.i16:
	mov	edx,0xf3b60300
	jmp	SIMD_INT_qd_dm_imm_I
ARM_instruction_vshll.i32:
	mov	edx,0xf3ba0300
	jmp	SIMD_INT_qd_dm_imm_I
ARM_instruction_vshll.i8:
	mov	edx,0xf3b20300
	jmp	SIMD_INT_qd_dm_imm_I
ARM_instruction_vshll.s16:
	mov	edx,0xf2900a10
	mov	ecx,0xf3b60300
	jmp	SIMD_INT_qd_dm_imm
ARM_instruction_vshll.s32:
	mov	edx,0xf2a00a10
	mov	ecx,0xf3ba0300
	jmp	SIMD_INT_qd_dm_imm
ARM_instruction_vshll.s8:
	mov	edx,0xf2880a10
	mov	ecx,0xf3b20300
	jmp	SIMD_INT_qd_dm_imm
ARM_instruction_vshll.u16:
	mov	edx,0xf3900a10
	mov	ecx,0xf3b60300
	jmp	SIMD_INT_qd_dm_imm
ARM_instruction_vshll.u32:
	mov	edx,0xf3a00a10
	mov	ecx,0xf3ba0300
	jmp	SIMD_INT_qd_dm_imm
ARM_instruction_vshll.u8:
	mov	edx,0xf3880a10
	mov	ecx,0xf3b20300
	jmp	SIMD_INT_qd_dm_imm
ARM_instruction_vshr.s16:
	mov	edx,0xf2900010
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vshr.s32:
	mov	edx,0xf2a00010
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vshr.s64:
	mov	edx,0xf2800090
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vshr.s8:
	mov	edx,0xf2880010
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vshr.u16:
	mov	edx,0xf3900010
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vshr.u32:
	mov	edx,0xf3a00010
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vshr.u64:
	mov	edx,0xf3800090
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vshr.u8:
	mov	edx,0xf3880010
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vshrn.i16:
	mov	edx,0xf2880810
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vshrn.i32:
	mov	edx,0xf2900810
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vshrn.i64:
	mov	edx,0xf2a00810
	jmp	SIMD_INT_dd_qm_imm
ARM_instruction_vsli.16:
	mov	edx,0xf3900510
	jmp	SIMD_INT_vd_vm_imm_alt
ARM_instruction_vsli.32:
	mov	edx,0xf3a00510
	jmp	SIMD_INT_vd_vm_imm_alt
ARM_instruction_vsli.64:
	mov	edx,0xf3800590
	jmp	SIMD_INT_vd_vm_imm_alt
ARM_instruction_vsli.8:
	mov	edx,0xf3880510
	jmp	SIMD_INT_vd_vm_imm_alt
ARM_instruction_vsqrt.f32 = ARM_instruction_fsqrts
ARM_instruction_vsqrt.f64 = ARM_instruction_fsqrtd
ARM_instruction_vsra.s16:
	mov	edx,0xf2900110
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vsra.s32:
	mov	edx,0xf2a00110
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vsra.s64:
	mov	edx,0xf2800190
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vsra.s8:
	mov	edx,0xf2880110
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vsra.u16:
	mov	edx,0xf3900110
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vsra.u32:
	mov	edx,0xf3a00110
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vsra.u64:
	mov	edx,0xf3800190
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vsra.u8:
	mov	edx,0xf3880110
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vsri.16:
	mov	edx,0xf3900410
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vsri.32:
	mov	edx,0xf3a00410
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vsri.64:
	mov	edx,0xf3800490
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vsri.8:
	mov	edx,0xf3880410
	jmp	SIMD_INT_vd_vm_imm_alt_neg
ARM_instruction_vst1.16:
	mov	edx,0xf4000040
	jmp	SIMD_INT_list_rn_rm_1
ARM_instruction_vst1.32:
	mov	edx,0xf4000080
	jmp	SIMD_INT_list_rn_rm_1
ARM_instruction_vst1.64:
	mov	edx,0xf40000c0
	jmp	SIMD_INT_list_rn_rm_1
ARM_instruction_vst1.8:
	mov	edx,0xf4000000
	jmp	SIMD_INT_list_rn_rm_1
ARM_instruction_vst2.16:
	mov	edx,0xf4000040
	jmp	SIMD_INT_list_rn_rm_2
ARM_instruction_vst2.32:
	mov	edx,0xf4000080
	jmp	SIMD_INT_list_rn_rm_2
ARM_instruction_vst2.8:
	mov	edx,0xf4000000
	jmp	SIMD_INT_list_rn_rm_2
ARM_instruction_vst3.16:
	mov	edx,0xf4000040
	jmp	SIMD_INT_list_rn_rm_3
ARM_instruction_vst3.32:
	mov	edx,0xf4000080
	jmp	SIMD_INT_list_rn_rm_3
ARM_instruction_vst3.8:
	mov	edx,0xf4000000
	jmp	SIMD_INT_list_rn_rm_3
ARM_instruction_vst4.16:
	mov	edx,0xf4000040
	jmp	SIMD_INT_list_rn_rm_4
ARM_instruction_vst4.32:
	mov	edx,0xf4000080
	jmp	SIMD_INT_list_rn_rm_4
ARM_instruction_vst4.8:
	mov	edx,0xf4000000
	jmp	SIMD_INT_list_rn_rm_4
ARM_instruction_vstm.32 = ARM_instruction_fstmias
ARM_instruction_vstm.64 = ARM_instruction_fstmiad
ARM_instruction_vstmdb.32 = ARM_instruction_fstmdbs
ARM_instruction_vstmdb.64 = ARM_instruction_fstmdbd
ARM_instruction_vstmfd.32 = ARM_instruction_fstmfds
ARM_instruction_vstmfd.64 = ARM_instruction_fstmfdd
ARM_instruction_vstmea.32 = ARM_instruction_fstmeas
ARM_instruction_vstmea.64 = ARM_instruction_fstmead
ARM_instruction_vstmia.32 = ARM_instruction_fstmias
ARM_instruction_vstmia.64 = ARM_instruction_fstmiad
ARM_instruction_vstmfd:
ARM_instruction_vstmdb:
	mov	edx,0xd000a00
	jmp	SIMD_INT_rn_list
ARM_instruction_vstm:
ARM_instruction_vstmea:
ARM_instruction_vstmia:
	mov	edx,0xc800a00
	jmp	SIMD_INT_rn_list
ARM_instruction_vstr.32 = ARM_instruction_fsts
ARM_instruction_vstr.64 = ARM_instruction_fstd
ARM_instruction_vstr:
	mov	edx,0x0d000a00
	jmp	SIMD_INT_vd_rn_offset
ARM_instruction_vsub.f32:
	mov	edx,0xf2200d00
	mov	ecx,0x0e300a40
	jmp	SIMD_FLOAT_vd_vn_vm_f32
ARM_instruction_vsub.f64 = ARM_instruction_fsubd
ARM_instruction_vsub.i16:
	mov	edx,0xf3100800
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vsub.i32:
	mov	edx,0xf3200800
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vsub.i64:
	mov	edx,0xf3300800
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vsub.i8:
	mov	edx,0xf3000800
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vsubhn.i16:
	mov	edx,0xf2800600
	jmp	SIMD_INT_vd_vn_vm_narrow
ARM_instruction_vsubhn.i32:
	mov	edx,0xf2900600
	jmp	SIMD_INT_vd_vn_vm_narrow
ARM_instruction_vsubhn.i64:
	mov	edx,0xf2a00600
	jmp	SIMD_INT_vd_vn_vm_narrow
ARM_instruction_vsubl.s16:
	mov	edx,0xf2900200
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vsubl.s32:
	mov	edx,0xf2a00200
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vsubl.s8:
	mov	edx,0xf2800200
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vsubl.u16:
	mov	edx,0xf3900200
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vsubl.u32:
	mov	edx,0xf3a00200
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vsubl.u8:
	mov	edx,0xf3800200
	jmp	SIMD_INT_vd_vn_vm_long
ARM_instruction_vsubw.s16:
	mov	edx,0xf2900300
	jmp	SIMD_INT_vd_vn_vm_wide
ARM_instruction_vsubw.s32:
	mov	edx,0xf2a00300
	jmp	SIMD_INT_vd_vn_vm_wide
ARM_instruction_vsubw.s8:
	mov	edx,0xf2800300
	jmp	SIMD_INT_vd_vn_vm_wide
ARM_instruction_vsubw.u16:
	mov	edx,0xf3900300
	jmp	SIMD_INT_vd_vn_vm_wide
ARM_instruction_vsubw.u32:
	mov	edx,0xf3a00300
	jmp	SIMD_INT_vd_vn_vm_wide
ARM_instruction_vsubw.u8:
	mov	edx,0xf3800300
	jmp	SIMD_INT_vd_vn_vm_wide
ARM_instruction_vswp:
	mov	edx,0xf3b20000
	jmp	SIMD_INT_vd_vm
ARM_instruction_vtbl.8:
	mov	edx,0xf3b00800
	jmp	SIMD_dn_list_dm
ARM_instruction_vtbx.8:
	mov	edx,0xf3b00840
	jmp	SIMD_dn_list_dm
ARM_instruction_vtrn.16:
	mov	edx,0xf3b60080
	jmp	SIMD_INT_vd_vm
ARM_instruction_vtrn.32:
	mov	edx,0xf3ba0080
	jmp	SIMD_INT_vd_vm
ARM_instruction_vtrn.8:
	mov	edx,0xf3b20080
	jmp	SIMD_INT_vd_vm
ARM_instruction_vtst.16:
	mov	edx,0xf2100810
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vtst.32:
	mov	edx,0xf2200810
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vtst.8:
	mov	edx,0xf2000810
	jmp	SIMD_INT_vd_vn_vm_alt
ARM_instruction_vuzp.16:
	mov	edx,0xf3b60100
	jmp	SIMD_INT_vd_vm
ARM_instruction_vuzp.32:
	mov	edx,0xf3ba0100
	jmp	SIMD_INT_vd_vm
ARM_instruction_vuzp.8:
	mov	edx,0xf3b20100
	jmp	SIMD_INT_vd_vm
ARM_instruction_vzip.16:
	mov	edx,0xf3b60180
	jmp	SIMD_INT_vd_vm
ARM_instruction_vzip.32:
	mov	edx,0xf3ba0180
	jmp	SIMD_INT_vd_vm
ARM_instruction_vzip.8:
	mov	edx,0xf3b20180
	jmp	SIMD_INT_vd_vm

;VFPv4 and SIMDv2

ARM_instruction_vfma.f32:
	mov	edx,0xf2000c10
	mov	ecx,0x0ea00a00
	jmp	SIMD_FLOAT_vd_vn_vm
ARM_instruction_vfms.f32:
	mov	edx,0xf2200c10
	mov	ecx,0x0ea00a40
	jmp	SIMD_FLOAT_vd_vn_vm
ARM_instruction_vfma.f64:
	mov	ecx,0x0ea00b00
	jmp	SIMD_FLOAT_dd_dn_dm
ARM_instruction_vfms.f64:
	mov	ecx,0x0ea00b40
	jmp	SIMD_FLOAT_dd_dn_dm
ARM_instruction_vfnma.f32:
	mov	ecx,0x0e900a40
	jmp	SIMD_FLOAT_sd_sn_sm
ARM_instruction_vfnms.f32:
	mov	ecx,0x0e900a00
	jmp	SIMD_FLOAT_sd_sn_sm
ARM_instruction_vfnma.f64:
	mov	ecx,0x0e900b40
	jmp	SIMD_FLOAT_dd_dn_dm
ARM_instruction_vfnms.f64:
	mov	ecx,0x0e900b00
	jmp	SIMD_FLOAT_dd_dn_dm

;v7VE

ARM_instruction_eret:
	mov	edx,0x0160006e
	mov	ecx,0xf3de8f00
	jmp	ARM_eret
ARM_instruction_hvc:
	mov	edx,0x01400070
	mov	ecx,0xf7e08000
	jmp	ARM_immediate16_ve

;special

ARM_instruction_crc32:
	jmp	CRC_32

ARM_bracket_parser:
	stosb
    .next:
	lodsb
	test	al,al
	jz	.done
	cmp	al,[esp+4]
	jz	.done
	cmp	al,'#'
	jz	.store
	cmp	al,'['
	jz	.store
	cmp	al,']'
	jz	.close
	cmp	al,01ah
	jnz	.expression
	lodsb
	movzx	ecx,al
	push	edi
	mov	edi,symbols
	call	get_symbol
	pop	edi
	jnc	.symbol
	sub	esi,1
    .expression:
	sub	esi,1
	mov	cx,[edi-2]
	cmp	cx,',-'
	setz	cl
	movzx	ecx,cl
	sub	edi,ecx
	sub	esi,ecx
	mov	al,'('
	stosb
	call	convert_expression
	mov	al,')'
	stosb
	lodsb
	cmp	byte[edi-2],'!'
	jz	.next
	cmp	al,[esp+4]
	jz	.done
	cmp	al,','
	jz	.comma2
	dec	esi
	jmp	.next
    .done:
	ret
    .symbol:
	stosw
	cmp	al,shift_op
	jz	.next
	mov	al,[esi]
	cmp	al,','
	jz	.comma
	cmp	al,'-'
	jnz	.next
	add	esi,1
    .store:
	stosb
	jmp	.next
    .close:
	stosb
	cmp	byte[esi],','
	jnz	.next
	lodsb
	jmp	.store
    .comma:
	add	esi,1
    .comma2:
	stosb
	lodsb
	cmp	al,'-'
	jz	.store
	cmp	al,'+'
	setnz	cl
	movzx	ecx,cl
	sub	esi,ecx
	jmp	.next

ARM_square_parser:
	push	']'
	call	ARM_bracket_parser
	pop	ecx
	cmp	al,']'
	jnz	ERROR_expecting_square_bracket_close
	stosb
	jmp	argument_parsed

ARM_curly_parser:
	push	'}'
	call	ARM_bracket_parser
	pop	ecx
	cmp	al,'}'
	jnz	ERROR_expecting_curly_bracket_close
	stosb
	jmp	argument_parsed

ARM_calculate_expression:
	push	[error] [error_info] [error_line]
	mov	[error_line],0
	mov	[value_size],0
	cmp	byte[esi],'.'					;float?
	jnz	.size_known
	mov	[value_size],4
    .size_known:
	call	calculate_expression
	mov	ecx,[error_line]
	test	ecx,ecx
	jz	.no_error
	mov	[ARM_error_line],ecx
	pop	ecx
	test	ecx,ecx
	jz	.new_error
	mov	[error_line],ecx
	pop	[error_info] [error]
	stc
	ret
    .new_error:
	pop	ecx ecx
	stc
	ret
    .no_error:
	pop	[error_line] [error_info] [error]
	clc
	ret

ARM_defer_error:
	mov	ebp,UNENCODEABLE_INSTRUCTION_ARM
	cmp	[error_line],0
	jne	@f
	mov	[error],ecx
	mov	ecx,[current_line]
	mov	[error_line],ecx
    @@:	mov	ecx,[current_line]
	mov	[ARM_error_line],ecx
	ret

ARM_store_instruction_with_error:
	call	ARM_defer_error
	jmp	ARM_store_instruction.store
ARM_store_instruction:
	cmp	ebp,0xf shl 28
	jb	.unconditional_okay
	mov	ah,[instruction_condition]
	and	ah,0xf0
	cmp	ah,0xe0
	jb	ERROR_instruction_not_conditional
    .unconditional_okay:
	cmp	[explicit_IT_state],0
	jz	.explicit_okay
	mov	cl,byte[explicit_IT_state]		;get currently specified condition
	mov	ch,[instruction_condition]
	and	cx,0xf0f0
	cmp	ch,cl
	mov	ecx,ERROR_condition_does_not_match_IT_specifier
	jnz	ARM_store_instruction_with_error
    .explicit_okay:
	mov	eax,[addressing_space]
	mov	eax,[eax+0x00]
	sub	eax,edi
	neg	eax
	test	eax,3
	mov	ecx,ERROR_instruction_not_aligned
	jnz	ARM_store_instruction_with_error
    .store:
	shr	[explicit_IT_state],8
	cmp	[error_line],0
	jz	.store_instruction
	mov	ecx,[current_line]
	cmp	[ARM_error_line],ecx
	jnz	.store_instruction
	cmp	[current_pass],1
	jbe	.store_instruction
	cmp	[error],ERROR_immediate_cannot_be_encoded
	jz	.soft_error
	cmp	[error],ERROR_immediate_offset_out_of_range
	jz	.soft_error
	cmp	[error],ERROR_value_out_of_range
	jne	.store_instruction
    .soft_error:
	or	ebp,-1	;set to this value so that we can use if/else/end if to detect unencodable instructions
    .store_instruction:
	mov	[edi],ebp
	add	edi,4
	jmp	instruction_assembled

;we must supply this function for expressi.inc to assemble
;called by expressi.inc during store
get_size_operator:
	xor	ah,ah
	cmp	al,11h
	jne	.no_size_operator
	mov	[size_declared],1
	lods	word[esi]
	xchg	al,ah
	mov	[size_override],1
	cmp	ah,[operand_size]
	je	.size_operator_ok
	cmp	[operand_size],0
	jne	operand_sizes_do_not_match
	mov	[operand_size],ah
    .size_operator_ok:
	ret
    .no_size_operator:
	mov	[size_declared],0
	cmp	al,'['
	jne	.size_operator_ok
	mov	[size_override],0
	ret

;we must supply this label for parser.inc to assemble

prefix_instruction:	;not used in ARM

;we must provide this function for assemble.inc

segment_prefix = ERROR_illegal_instruction

;use a custom instruction searcher to convert '~~' to conditionals

conditional_strings:
	db	'eq',0,0x00
	db	'ne',0,0x10
	db	'cs',0,0x20
	db	'cc',0,0x30
	db	'mi',0,0x40
	db	'pl',0,0x50
	db	'vs',0,0x60
	db	'vc',0,0x70
	db	'hi',0,0x80
	db	'ls',0,0x90
	db	'ge',0,0xa0
	db	'lt',0,0xb0
	db	'gt',0,0xc0
	db	'le',0,0xd0
	db	'al',0,0xe0
	db	'hs',0,0x20
	db	'lo',0,0x30
	db	0

ARM_get_instruction:
	;esi=pointer to string
	;ecx=length of string
	push	esi ecx
	call	ARM_find_instruction
	jnc	.found
	lea	edi,[converted+ecx-2]
    .try_condition:
	cmp	edi,converted
	jbe	.not_found
	mov	ax,[edi]
	mov	esi,conditional_strings
    .next_condition:
	cmp	ax,[esi]
	jz	.condition_matched
	add	esi,4
	cmp	byte[esi],0
	jnz	.next_condition
    .next_position:
	dec	edi
	jmp	.try_condition
    .condition_matched:
	push	dword[edi]
	mov	word[edi],CONDITION_SEARCH_CHARACTER*0x101
	movzx	eax,byte[esi+3]
	push	eax edi
	mov	esi,converted
	call	ARM_find_instruction
	pop	edi edx
	pop	dword[edi]
	jc	.next_position
	or	al,dl
    .found:
	pop	ecx esi
	add	esi,ecx
	clc
	ret
    .not_found:
	pop	ecx esi
	stc
	ret

ARM_find_instruction:
	push	esi
	mov	ebp,ecx
	mov	byte[characters+CONDITION_SEARCH_CHARACTER],CONDITION_SEARCH_CHARACTER
	call	lower_case
	mov	byte[characters+CONDITION_SEARCH_CHARACTER],0
	mov	ecx,ebp
	cmp	cl,15
	ja	.no_instruction
	sub	cl,1
	jc	.no_instruction
	mov	ebx,[instructions+ecx*8]
	add	ebx,instructions
	mov	edx,[instructions+ecx*8+4]
    .scan_instructions:
	or	edx,edx
	jz	.no_instruction
	mov	eax,edx
	shr	eax,1
	lea	edi,[ebp+3]
	imul	eax,edi
	lea	edi,[ebx+eax]
	mov	esi,converted
	mov	ecx,ebp
	repe	cmps byte [esi],[edi]
	ja	.instructions_up
	jb	.instructions_down
	pop	esi
	add	esi,ebp
	mov	al,[edi]
	mov	bx,[edi+1]
	clc
	ret
    .no_instruction:
	pop	esi
	mov	ecx,ebp
	stc
	ret
    .instructions_down:
	shr	edx,1
	jmp	.scan_instructions
    .instructions_up:
	lea	ebx,[edi+ecx+3]
	shr	edx,1
	adc	edx,-1
	jmp	.scan_instructions

;custom handler to store address of line start

ARM_instruction_handler:
	movzx	ebx,word [esi]
	mov	al,[esi+2]
	add	esi,3
	mov	[force_wide_flag],esi
	add	[esp],ebx
	ret

ARM_opcode_swap_table:
	db	(0eh xor 00h)				;0 - and --> bic, invert
	db	0					;1 - eor
	db	(04h xor 02h)+0x80			;2 - sub --> add, negate
	db	0					;3 - rsb
	db	(02h xor 04h)+0x80			;4 - add --> sub, negate
	db	(06h xor 05h)+0x80			;5 - adc --> sbc, negate
	db	(05h xor 06h)+0x80			;6 - sbc --> adc, negate
	db	0					;7 - rsc
	db	0					;8 - tst
	db	0					;9 - teq
	db	(0bh xor 0ah)+0x80			;a - cmp --> cmn, negate
	db	(0ah xor 0bh)+0x80			;b - cmn --> cmp, negate
	db	0					;c - orr
	db	(0fh xor 0dh)				;d - mov --> mvn, invert
	db	(00h xor 0eh)				;e - bic --> and, invert
	db	(0dh xor 0fh)				;f - mvn --> mov, invert

ARM_encode_immediate_with_opcode_swap:
	call	ARM_encode_immediate
	jnc	.done
	mov	eax,[arm_instruction]
	shr	eax,21
	and	eax,0xf
	mov	al,[eax+ARM_opcode_swap_table]
	test	al,al
	jz	.fail
	mov	ecx,[immediate_value]
	not	ecx
	test	al,0x80
	jz	.try
	inc	ecx
	and	eax,0xf
    .try:
	mov	[immediate_value],ecx
	shl	eax,21
	xor	[arm_instruction],eax
	xor	ebp,eax
	call	ARM_encode_immediate
	jnc	.done
    .fail:
	stc
    .done:
	ret

ARM_encode_immediate:
	mov	eax,[immediate_value]
	xor	ecx,ecx
    .try:
	cmp	eax,0ffh
	jbe	.immediate_ok
	add	ch,1
	rol	eax,2
	cmp	ch,010h
	jb	.try
	stc
	ret
    .immediate_ok:
	or	eax,ecx
	mov	[immediate_value],eax
	clc
	ret

THUMB_opcode_swap_table:
	db	(01h xor 00h)				;0 - and --> bic, invert
	db	(00h xor 01h)				;1 - bic --> and, invert
	db	(03h xor 02h)				;2 - mov --> mvn, invert, also orr --> orn, invert
	db	(02h xor 03h)				;3 - mvn --> mov, invert, also orn --> orr, invert
	db	0					;4 - eor, teq
	db	0					;5 -
	db	0					;6 -
	db	0					;7 -
	db	(0dh xor 08h)+0x80			;8 - add --> sub, negate, also cmn --> cmp, negate
	db	0					;9 -
	db	(0bh xor 0ah)+0x80			;a - adc --> sbc, negate
	db	(0ah xor 0bh)+0x80			;b - sbc --> adc, negate
	db	0					;c -
	db	(08h xor 0dh)+0x80			;d - sub --> add, negate, also cmp --> cmn, negate
	db	0					;e - rsb
	db	0					;f -

THUMB_encode_immediate_with_opcode_swap:
	call	THUMB_encode_immediate
	jnc	.done
	mov	eax,[thumb32_instruction]
	shr	eax,21
	and	eax,0xf
	mov	al,[eax+THUMB_opcode_swap_table]
	test	al,al
	jz	.fail
	mov	ecx,[immediate_value]
	not	ecx
	test	al,0x80
	jz	.try
	inc	ecx
	and	eax,0xf
    .try:
	mov	[immediate_value],ecx
	shl	eax,21
	xor	[thumb32_instruction],eax
	xor	ebp,eax
	call	THUMB_encode_immediate
	jnc	.done
    .fail:
	stc
    .done:
	ret

THUMB_encode_immediate:
	mov	eax,[immediate_value]
	xor	edx,edx
	bsr	ecx,eax
	jz	.immediate_ok
	sub	ecx,8
	jb	.immediate_ok
	mov	edx,2
	shl	edx,cl
	dec	edx
	test	eax,edx
	jnz	.check_special_combos
	sub	ecx,31
	neg	ecx
	mov	edx,ecx
	shl	edx,7
	rol	eax,cl
	and	eax,0x7f
	jmp	.immediate_ok
    .check_special_combos:
	mov	ecx,eax
	shr	ecx,16
	cmp	ax,cx
	jnz	.fail
	mov	edx,1 shl 8
	cmp	ah,0					;0X0X
	jz	.immediate_ok
	mov	edx,2 shl 8
	xchg	ah,al
	cmp	ah,0					;X0X0
	jz	.immediate_ok
	mov	edx,3 shl 8
	movzx	eax,al
	cmp	ch,cl					;XXXX
	jz	.immediate_ok
    .fail:
	stc
	ret
    .immediate_ok:
	or	eax,edx
	mov	[immediate_value],eax
	clc
	ret

THUMB_check_12bit_immediate:
	mov	eax,[immediate_value]
	mov	ecx,eax
	neg	ecx
	cmovns	eax,ecx
	test	eax,not 0xfff
	jnz	.fail
    ;check for fit into a modified immediate value
	bsr	ecx,eax
	jz	.fail
	bsf	edx,eax
	sub	ecx,edx
	cmp	ecx,8
	jb	.fail
	clc
	ret
    .fail:
	stc
	ret

PREDEFINED_LABEL_PROCESSOR	= 4
PREDEFINED_LABEL_COPROCESSOR	= 5

ARM_predefined_label:
	or	eax,eax
	jz	current_offset_label
	cmp	eax,1
	je	counter_label
	cmp	eax,2
	je	timestamp_label
	cmp	eax,3
	je	org_origin_label
	cmp	eax,PREDEFINED_LABEL_PROCESSOR
	je	.processor_id
	cmp	eax,PREDEFINED_LABEL_COPROCESSOR
	je	.coprocessor_id
	mov	edx,invalid_value
	jmp	error_undefined
    .processor_id:
	mov	eax,[cpu_capability_flags]
	jmp	make_dword_label_value
    .coprocessor_id:
	mov	eax,[copro_capability_flags]
	jmp	make_dword_label_value

ARM_get_predefined_id:
	cmp	ecx,2
	ja	find_label
	inc	esi
	cmp	cl,1
	je	get_counter_id
	lods	byte [esi]
	mov	ebx,characters
	xlat	[ebx]
	cmp	al,'t'
	je	get_timestamp_id
	cmp	al,'p'
	je	.processor_id
	cmp	al,'c'
	je	.coprocessor_id
	sub	esi,2
	jmp	find_label
    .processor_id:
	mov	eax,PREDEFINED_LABEL_PROCESSOR
	ret
    .coprocessor_id:
	mov	eax,PREDEFINED_LABEL_COPROCESSOR
	ret

ARM_assemble_line:
	test	[code_type],CPU_ACTIVITY_UNKNOWN
	jz	assemble_line
	jmp	ARM_generic_mode_checks.default

ARM_generic_mode_checks:
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.other
    .arm:
	test	al,FLAG_FORCE_NARROW
	jnz	ERROR_narrow_instructions_are_not_encodable_in_arm_mode
	ret
    .other:
	test	[code_type],CPU_ACTIVITY_UNKNOWN
	jnz	.default
	mov	[thumb16_error],ERROR_instruction_not_16bit
	mov	[thumb32_error],ERROR_instruction_not_16bit
	ret
    .default:
	mov	[code_type],CPU_ACTIVITY_ARM
	mov	[cpu_capability_flags],CPU_CAPABILITY_DEFAULT
	mov	[copro_capability_flags],COPRO_CAPABILITY_DEFAULT
	mov	[potential_IT_anchor],0
	mov	[explicit_IT_state],0
	mov	[current_IT_block],0
	jmp	.arm

ARM_check_shift_range:
	mov	eax,[immediate_value]
	cmp	eax,0
	jnz	.shift_not_zero
    ;force to LSL 0
	mov	[instruction_shift_op],ARM_SHIFT_OPCODE_LSL
	and	ebp,not (3 shl 5)
    .shift_not_zero:
	movzx	ecx,[instruction_shift_op]
	shl	ecx,5
	or	ebp,ecx
	mov	edx,0x1f00				;lsl is 0 to 31
	cmp	ecx,ARM_SHIFT_OPCODE_LSL shl 5
	je	.check_range
	mov	edx,0x2001				;lsr & asr are 1 to 32
	cmp	ecx,ARM_SHIFT_OPCODE_LSR shl 5
	je	.check_range
	cmp	ecx,ARM_SHIFT_OPCODE_ASR shl 5
	je	.check_range
	mov	edx,0x1f01				;ror is 1 to 31
    .check_range:
	cmp	eax,32
	ja	.shift_out_of_range
	cmp	al,dl
	jb	.shift_out_of_range
	cmp	al,dh
	ja	.shift_out_of_range
	and	eax,0x1f
	shl	eax,7
	or	ebp,eax
	ret
    .shift_out_of_range:
	cmp	edx,0x1f00
	mov	ecx,ERROR_shift_value_out_of_range.0_31
	jz	.defer_error
	cmp	edx,0x2001
	mov	ecx,ERROR_shift_value_out_of_range.1_32
	jz	.defer_error
	cmp	edx,0x1f01
	mov	ecx,ERROR_shift_value_out_of_range.1_31
	jz	.defer_error
	ud2
    .defer_error:
	call	ARM_defer_error
	ret

	align 4
THUMB_UAL_s_table:
	;bit=1 instruction needs 's' modifier outside of IT block
	;bit=0 instruction never uses 's' modifier
	;everything after 0x4400 never uses 's' modifier
	dd	0xffffffff	;0000-07ff lsl r,r,i5
	dd	0xffffffff	;0800-0fff lsr r,r,i5
	dd	0xffffffff	;1000-17ff asr r,r,i5
	dd	0xffffffff	;1800-1fff add r,r,r/sub r,r,r/add r,r,i3/sub r,r,i3
	dd	0xffffffff	;2000-27ff mov r,i8
	dd	0x00000000	;2800-2fff cmp r,i8
	dd	0xffffffff	;3000-37ff add r,i8
	dd	0xffffffff	;3800-3fff sub r,i8
	dw	0xf2ff		;4000-43ff {and/eor/lsl/lsr/asr/adc/sbc/ror/tst/neg/cmp/cmn/orr/mul/bic/mvn} r,r

THUMB_do_non_UAL_S_override:
	;in pre-UAL code a 'OP reg,...' and 'OPs reg,...' are the same and use the 'OPs' form
	;we can fake this by forcing the S version if the encoding is in range
	cmp	[thumb16_error],0
	jnz	.16bit_okay
	movzx	eax,[thumb16_instruction]
	shr	eax,6
	test	[code_type],CPU_ACTIVITY_THUMB_NON_UAL
	jz	.flag_set_override_done
	cmp	eax,0x4400 shr 6
	jae	.flag_set_override_done
	bt	[THUMB_UAL_s_table],eax
	jnc	.flag_set_override_done
	or	[instruction_condition],FLAG_CONDITION_SET
	;every thumb32 equivalent instruction except MUL has the 's' in bit 20
	cmp	eax,0x4340 shr 6			;MUL?
	jnz	.do_thumb32_s_override
	;cannot be encoded in thumb32
	mov	[thumb32_error],ERROR_instruction_not_16bit
	jmp	.flag_set_override_done
    .do_thumb32_s_override:
	or	[thumb32_instruction],1 shl 20
    .flag_set_override_done:
	;now check the S flag
	cmp	eax,0x4400 shr 6
	jae	.flag_must_be_unset
	bt	[THUMB_UAL_s_table],eax
	jnc	.flag_must_be_unset
	test	[instruction_condition],FLAG_CONDITION_SET
	jnz	.only_outside
    ;only inside IT
	or	[thumb_flags_16],THUMB_FLAG_ONLY_INSIDE_IT
	jmp	.16bit_okay
    .only_outside:
	or	[thumb_flags_16],THUMB_FLAG_NOT_INSIDE_IT
	jmp	.16bit_okay
    .flag_must_be_unset:
	test	[instruction_condition],FLAG_CONDITION_SET
	jz	.16bit_okay
    .16bit_unavailable:
	mov	[thumb16_error],ERROR_instruction_not_16bit
    .16bit_okay:
	ret

THUMB_try_place_narrow_with_IT:
	cmp	[thumb16_error],ERROR_instruction_not_16bit
	jz	.unencodable
	call	THUMB_try_place_narrow_without_IT
	jnc	.done
	test	[thumb_flags_16],THUMB_FLAG_NOT_INSIDE_IT
	jnz	.fail
	call	THUMB_start_IT_block
    .done:
	ret
    .unencodable:
	mov	ecx,ERROR_instruction_not_16bit
    .fail:
	stc
	ret

THUMB_try_place_wide_with_IT:
	cmp	[thumb32_error],ERROR_instruction_not_16bit
	jz	.unencodable
	call	THUMB_try_place_wide_without_IT
	jnc	.done
	test	[thumb_flags_32],THUMB_FLAG_NOT_INSIDE_IT
	jnz	.fail
	call	THUMB_start_IT_block
    .done:
	ret
    .unencodable:
	mov	ecx,ERROR_instruction_not_16bit
    .fail:
	stc
	ret

THUMB_try_place_wide_without_IT:
	cmp	[thumb32_error],ERROR_instruction_not_16bit
	jz	.unencodable
	mov	ecx,thumb_flags_32
	jmp	THUMB_try_place_without_IT
    .unencodable:
	mov	ecx,ERROR_instruction_not_16bit
	stc
	ret

THUMB_try_place_narrow_without_IT:
	cmp	[thumb16_error],ERROR_instruction_not_16bit
	jz	.unencodable
	mov	ecx,thumb_flags_16
	jmp	THUMB_try_place_without_IT
    .unencodable:
	mov	ecx,ERROR_instruction_not_16bit
	stc
	ret

THUMB_try_place_without_IT:
	;ecx=flag check address
	mov	al,[instruction_condition]
	and	al,0xf0
	test	byte[ecx],THUMB_FLAG_ONLY_INSIDE_IT
	jnz	.force_inside
	test	byte[ecx],THUMB_FLAG_NOT_INSIDE_IT
	jnz	.force_outside
    ;decide based upon the condition
	cmp	al,0xf0					;unconditional?
	jae	.success
	cmp	al,0xe0
	jb	.force_inside
    ;try inside with AL condition. We try to extend open IT blocks if possible
	test	byte[ecx],THUMB_FLAG_NOT_INSIDE_IT
	jnz	.success				;this can happen when an explicit IT block is used
	call	THUMB_extend_IT_block
	clc						;always give success even if the extension failed
	ret
    .force_inside:
	test	byte[ecx],THUMB_FLAG_NOT_INSIDE_IT
	jnz	.fail					;this can happen when an explicit IT block is used
	cmp	[explicit_IT_state],0
	jnz	.success
	call	THUMB_extend_IT_block
	ret
    .force_outside:
	test	[explicit_IT_state],2
	jnz	.success
	cmp	[explicit_IT_state],0
	jnz	.fail
	cmp	al,0xe0					;AL condition?
	jae	.success
	test	byte[ecx],THUMB_FLAG_IS_BCC
	jz	.fail
    .success:
	mov	[current_IT_block],0			;since we are not extending the IT block we have to kill it
	clc
	ret
    .fail:
	mov	ecx,ERROR_instruction_not_16bit
	stc
	ret

THUMB_query_condition_pc:
	mov	ecx,edi
	test	[code_type],CPU_ACTIVITY_ARM
	jnz	.done
	mov	ah,[instruction_condition]
	and	ah,0xf0
	cmp	ah,0xe0
	jae	.done
	push	eax ecx
	call	THUMB_query_condition_match
	pop	ecx eax
	setc	dl
	movzx	edx,dl
	lea	ecx,[ecx+edx*2]				;adjust for IT block
    .done:
	ret

THUMB_query_condition_match:
	;check to see if the current instruction would match or extend an existing IT block
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	.not_match
	cmp	[explicit_IT_state],0
	jnz	.explicit
	mov	eax,[current_IT_block]
	test	eax,eax
	jz	.not_match
	mov	ch,[instruction_condition]
	cmp	ch,0xf0
	jae	.not_match
	mov	cl,[eax]
	mov	edx,ecx
	and	ecx,0xe0e0
	cmp	ch,cl
	jnz	.not_match
    .okay:
	clc
	ret
    .explicit:
	mov	cl,byte[explicit_IT_state]		;get currently specified condition
	mov	ch,[instruction_condition]
	and	cx,0xf0f0
	cmp	ch,cl
	jz	.okay
    .not_match:
	stc
	ret

BKPT_force_condition_match:
	;used by BKPT
	mov	cl,[instruction_condition]
	mov	eax,[explicit_IT_state]
	test	eax,eax
	jnz	.explicit
	mov	eax,[current_IT_block]
	test	eax,eax
	jz	.done
	mov	al,[eax]
    .explicit:
	and	al,0xf0
	and	cl,0x0f
	or	al,cl
	mov	[instruction_condition],al
    .done:
	ret

THUMB_check_condition:
	mov	al,[instruction_condition]
	and	al,0xf0
	cmp	[explicit_IT_state],0
	jnz	.explicit
    .not_explicit:
	cmp	al,0xe0					;AL condition?
	jz	.okay
	test	[thumb_flags_32],THUMB_FLAG_IS_BCC
	jnz	.okay
	cmp	al,0xf0
	jz	.not_conditional
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	mov	ecx,ERROR_requires_cpu_capability_arm_7m
	jz	.fail
    .inside:
	or	[thumb_flags_16],THUMB_FLAG_ONLY_INSIDE_IT
	or	[thumb_flags_32],THUMB_FLAG_ONLY_INSIDE_IT
    .okay:
	clc
	ret
    .not_conditional:
	or	[thumb_flags_16],THUMB_FLAG_NOT_INSIDE_IT
	or	[thumb_flags_32],THUMB_FLAG_NOT_INSIDE_IT
	jmp	.okay
    .explicit:
	mov	edx,[explicit_IT_state]
	test	edx,2
	jnz	.not_explicit
	cmp	al,0xf0					;AL condition?
	jz	ERROR_instruction_not_conditional
	test	[thumb_flags_16],THUMB_FLAG_ONLY_LAST_IT
	jz	.check_explicit_match
	mov	ecx,ERROR_instruction_must_be_last_in_IT_block
	test	edx,0xffffff00				;must be no more conditions
	jnz	.fail
    .check_explicit_match:
	and	dl,0xf0
	cmp	al,dl
	jz	.inside
	mov	ecx,ERROR_condition_does_not_match_IT_specifier
    .fail:
	stc
	ret

THUMB_start_IT_block:
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	.capability
	mov	al,[instruction_condition]
	and	eax,0xf0
	cmp	al,0xf0
	jae	.cannot_start
	cmp	al,0xe0
	jnz	.make_new_IT_here
	call	THUMB_convert_anchor
	jnc	.okay
    .make_new_IT_here:
	mov	[potential_IT_anchor],0
	or	eax,0xbf08
	mov	[current_IT_block],edi
	mov	[edi],ax
	add	edi,2
    .okay:
	clc
	ret
    .cannot_start:
	mov	ecx,ERROR_instruction_not_conditional
	stc
	ret
    .capability:
	mov	ecx,ERROR_requires_cpu_capability_arm_7m
	stc
	ret

THUMB_convert_anchor:
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	.cannot_convert
	mov	al,[instruction_condition]
	and	eax,0xf0
	cmp	al,0xf0
	jae	.cannot_convert
	cmp	al,0xe0
	jnz	.cannot_convert
	mov	cl,[IT_anchor_distance]
	cmp	cl,3
	ja	.cannot_convert
	mov	eax,[potential_IT_anchor]
	test	eax,eax
	jz	.cannot_convert
	mov	dx,[anchor_instruction]
	shl	edx,16
	or	edx,0xbfe0
	mov	ch,0x10
	shr	ch,cl
	movzx	ecx,ch
	or	edx,ecx
	mov	[current_IT_block],eax
	mov	[eax],edx
	mov	[potential_IT_anchor],0
	clc
	ret
    .cannot_convert:
	stc
	ret

THUMB_extend_IT_block:
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	.cannot_extend
	mov	eax,[current_IT_block]
	test	eax,eax
	jz	.cannot_extend
	cmp	[explicit_IT_state],0
	jnz	.okay
	mov	ch,[instruction_condition]
	cmp	ch,0xf0
	jae	.cannot_extend
	mov	cl,[eax]
	mov	edx,ecx
	and	ecx,0xe0e0
	cmp	ch,cl
	jnz	.cannot_extend
	bsf	ecx,edx
	test	ecx,ecx
	jz	.cannot_extend
	shr	dh,4
	and	dh,0x1
	shl	dh,cl
	btr	edx,ecx
	or	dl,dh
	dec	ecx
	bts	edx,ecx
	mov	[eax],dl
    .okay:
	clc
	ret
    .cannot_extend:
	mov	[current_IT_block],0
	stc
	ret

ARM_post_process_simd_with_error:
	test	[code_type],CPU_ACTIVITY_ARM
	jnz	ARM_store_instruction_with_error
	mov	[thumb32_error],ecx
	jmp	THUMB_post_process

ARM_post_process_simd:
	test	[code_type],CPU_ACTIVITY_ARM
	jnz	ARM_store_instruction
	mov	[thumb32_error],0
	mov	eax,ebp
	rol	eax,8
	mov	ecx,eax
	mov	cl,0xf9
	rol	al,4
	or	al,0xc0
	cmp	al,0xcf
	cmovz	eax,ecx
	ror	eax,8
	mov	[thumb32_instruction],eax
	jmp	THUMB_post_process

ARM_post_process_copro_with_error:
	test	[code_type],CPU_ACTIVITY_ARM
	jnz	ARM_store_instruction_with_error
	jmp	ARM_post_process_copro.thumb

ARM_post_process_copro:
	test	[code_type],CPU_ACTIVITY_ARM
	jnz	ARM_store_instruction
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	ERROR_requires_cpu_capability_arm_7m
	xor	ecx,ecx
    .thumb:
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],ecx
	cmp	ebp,0xf shl 28				;is it a '2' version?
	jae	THUMB_post_process
	and	ebp,not (0xf shl 28)
	or	ebp,0xe shl 28
	mov	[thumb32_instruction],ebp
	jmp	THUMB_post_process

THUMB_post_process:
	cmp	[thumb16_error],0
	jz	.forced_wide_done
	cmp	[thumb32_error],0
	jnz	.forced_wide_done
	mov	eax,[force_wide_flag]
	mov	cl,[eax-1]
	and	cl,FLAG_FORCE_AUTO_WIDE
	shr	cl,FLAG_BIT_FORCE_AUTO_WIDE - FLAG_BIT_FORCE_WIDE
	or	cl,FLAG_FORCE_AUTO_WIDE
	or	[eax-1],cl
    .forced_wide_done:
	call	THUMB_do_non_UAL_S_override
	call	THUMB_check_condition
	jc	.fail
	test	[instruction_condition],FLAG_FORCE_NARROW
	jz	.narrow_not_forced
	call	THUMB_try_place_narrow_with_IT
	jnc	.store_narrow
	jmp	.fail
    .narrow_not_forced:
	test	[instruction_condition],FLAG_FORCE_WIDE
	jz	.wide_not_forced
	call	THUMB_try_place_wide_with_IT
	jnc	.store_wide
	jmp	.fail
    .wide_not_forced:
	cmp	[thumb16_error],0
	setz	al
	cmp	[thumb32_error],0
	setz	ah
	xor	al,ah
	jnz	.no_free_choice
	test	ah,ah
	jnz	.free_choice_without_error_copy
	mov	ecx,[thumb32_error]
	cmp	ecx,ERROR_instruction_not_16bit
	jz	.free_choice_without_error_copy
	mov	[thumb16_error],ecx
    .free_choice_without_error_copy:
	call	THUMB_try_place_narrow_without_IT
	jnc	.store_narrow
	call	THUMB_convert_anchor
	jc	.not_converted
	call	THUMB_try_place_narrow_without_IT
	jnc	.store_narrow
    .not_converted:
	call	THUMB_try_place_wide_without_IT
	jnc	.store_wide
	call	THUMB_try_place_narrow_with_IT
	jnc	.store_narrow
	call	THUMB_try_place_wide_with_IT
	jnc	.store_wide
	cmp	ecx,ERROR_instruction_not_16bit
	jnz	.fail
	mov	eax,[thumb32_error]
	test	eax,eax
	cmovnz	ecx,eax
	jmp	.fail
    .no_free_choice:
	test	ah,ah
	jz	.not_prefer_wide
	call	THUMB_try_place_wide_with_IT
	jnc	.store_wide
	jmp	.fail
    .not_prefer_wide:
	call	THUMB_try_place_narrow_with_IT
	jnc	.store_narrow
	jmp	.fail
    .store_narrow:
	mov	ecx,[thumb16_error]
	movzx	ebp,[thumb16_instruction]
	test	ecx,ecx
	jnz	THUMB_store_instruction_16_with_error
	jmp	THUMB_store_instruction_16
    .store_wide:
	mov	ecx,[thumb32_error]
	mov	ebp,[thumb32_instruction]
	test	ecx,ecx
	jnz	THUMB_store_instruction_32_with_error
	jmp	THUMB_store_instruction_32
    .fail:
	jmp	ecx

THUMB_store_instruction_16_with_error:
	call	ARM_defer_error
	jmp	THUMB_store_instruction_16.store
THUMB_store_instruction_16:
	mov	eax,[addressing_space]
	mov	eax,[eax+0x00]
	sub	eax,edi
	neg	eax
	test	eax,1
	mov	ecx,ERROR_instruction_not_aligned
	jnz	THUMB_store_instruction_16_with_error
    .store:
	mov	[edi],bp
	mov	ebp,2
	jmp	THUMB_finalise_instruction

THUMB_store_instruction_32_with_error:
	call	ARM_defer_error
	jmp	THUMB_store_instruction_32.store
THUMB_store_instruction_32:
	mov	eax,[addressing_space]
	mov	eax,[eax+0x00]
	sub	eax,edi
	neg	eax
	test	eax,1
	mov	ecx,ERROR_instruction_not_aligned
	jnz	THUMB_store_instruction_32_with_error
    .store:
	ror	ebp,16					;swap the hword endian
	mov	[edi],ebp
	mov	ebp,4
	jmp	THUMB_finalise_instruction

THUMB_finalise_instruction:
	mov	al,[IT_anchor_distance]
	add	al,1
	sbb	al,0
	mov	[IT_anchor_distance],al
	add	edi,ebp
	mov	ecx,[explicit_IT_state]
	btr	[explicit_IT_state],1
	jc	.explicit_beginning
	shr	[explicit_IT_state],8
    .explicit_beginning:
	test	ecx,ecx
	jnz	instruction_assembled
	mov	eax,[current_IT_block]
	test	eax,eax
	jz	.check_potential_IT_anchor
	test	[thumb_flags_16],THUMB_FLAG_ONLY_LAST_IT
	jnz	.kill_implicit_IT_block
	test	byte[eax],1				;end of condition?
	jz	instruction_assembled
    .kill_implicit_IT_block:
	mov	[current_IT_block],0
	jmp	instruction_assembled
    .check_potential_IT_anchor:
	cmp	[instruction_condition],0xe0
	jb	.kill_potential_anchor
	cmp	[instruction_condition],0xf0
	jae	.kill_potential_anchor
	cmp	ebp,4
	jnz	.check_potential_anchor_extension
	cmp	[thumb16_error],0
	jnz	.check_potential_anchor_extension
	test	[instruction_condition],FLAG_CONDITION_SET + FLAG_FORCE_WIDE
	jnz	.check_potential_anchor_extension
	lea	eax,[edi-4]				;make a new anchor
	mov	cx,[thumb16_instruction]
	mov	[IT_anchor_distance],1
	mov	[potential_IT_anchor],eax
	mov	[anchor_instruction],cx
	jmp	instruction_assembled
    .check_potential_anchor_extension:
	cmp	[potential_IT_anchor],0
	jz	instruction_assembled
	cmp	ebp,2
	jnz	.check_potential_anchor_extension_32
	test	[instruction_condition],FLAG_CONDITION_SET
	jnz	.kill_potential_anchor
    .check_potential_anchor_extension_32:
	test	[thumb_flags_16],THUMB_FLAG_NOT_INSIDE_IT + THUMB_FLAG_ONLY_LAST_IT + THUMB_FLAG_IS_BCC
	jnz	.kill_potential_anchor
	cmp	[IT_anchor_distance],4
	jbe	instruction_assembled			;anchor extention is okay
    .kill_potential_anchor:
	mov	[potential_IT_anchor],0
	jmp	instruction_assembled

ARM_define_label:
	cmp	[explicit_IT_state],0
	jnz	ERROR_label_inside_IT_block
	mov	[current_IT_block],0
	mov	[potential_IT_anchor],0
	jmp	define_label

ARM_set_virtual:
	call	allocate_structure_data
	mov	al,[IT_anchor_distance]
	mov	ecx,[potential_IT_anchor]
	mov	edx,[explicit_IT_state]
	mov	ebp,[current_IT_block]
	mov	word[ebx],ARM_set_virtual-instruction_handler
	mov	[ebx+2],al
	mov	[ebx+4],ecx
	mov	[ebx+8],edx
	mov	[ebx+12],ebp
	mov	ax,[anchor_instruction]
	mov	[ebx+16],ax
	xor	eax,eax
	mov	[IT_anchor_distance],al
	mov	[potential_IT_anchor],eax
	mov	[explicit_IT_state],eax
	mov	[current_IT_block],eax
	mov	[anchor_instruction],ax
	jmp	instruction_assembled

ARM_end_virtual:
	call	find_structure_data
	jc	.ret
	sub	ebx,0x18
	mov	al,[ebx+2]
	mov	ecx,[ebx+4]
	mov	edx,[ebx+8]
	mov	ebp,[ebx+12]
	mov	[IT_anchor_distance],al
	mov	[potential_IT_anchor],ecx
	mov	[explicit_IT_state],edx
	mov	[current_IT_block],ebp
	mov	ax,[ebx+16]
	mov	[anchor_instruction],ax
	call	remove_structure_data
	add	ebx,0x18
    .ret:
	ret

ARM_fit_operator:
	pop	edx
	je	calculate_not
	cmp	al,0D1h					;FIT?
	je	.calculate_fit
	jmp	edx
    .calculate_fit:
	cmp	word [edi+8],0
	jne	invalid_expression
	cmp	byte [edi+12],0
	je	.fit_ok
	cmp	[error_line],0
	jne	.fit_ok
	mov	eax,[current_line]
	mov	[error_line],eax
	mov	[error],invalid_use_of_symbol
    .fit_ok:
	mov	eax,[edi]
	mov	edx,[edi+4]
	xor	ebx,ebx
	test	edx,edx
	jz	.fit_test
	cdq
	cmp	edx,[edi+4]
	jnz	.fit_done
    .fit_test:
	mov	ebp,ARM_encode_immediate
	mov	edx,THUMB_encode_immediate
	test	[code_type],CPU_ACTIVITY_ARM
	cmovz	ebp,edx
	mov	[immediate_value],eax
	call	ebp
	setnc	bl					;bit-0 = can fit normal
	mov	eax,[edi]
	not	eax
	mov	[immediate_value],eax
	call	ebp
	setnc	cl
	shl	cl,1
	or	bl,cl					;bit-1 = can fit inverted
	mov	eax,[edi]
	neg	eax
	mov	[immediate_value],eax
	call	ebp
	setnc	cl
	shl	cl,2
	or	bl,cl					;bit-2 = can fit negated
    .fit_done:
	bsf	eax,[edi]
	jnz	.fits_32bit
	bsf	eax,[edi+4]
	jnz	.fits_64bit
	mov	eax,32
    .fits_64bit:
	add	eax,32
    .fits_32bit:
	shl	eax,3
	or	ebx,eax					;bits-9:3 = number of trailing zeros
	mov	[edi],ebx				;set low dword
	mov	dword[edi+4],0				;set high dword
	mov	byte[edi+13],0				;set sign
	add	edi,14h
	jmp	calculation_loop

;V1

ARM_rd_rn_shifter:
	;used by ADC, ADD, AND, BIC, EOR, ORR, RSB, RSC, SBC, SUB
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_imm>,\						; 0=reg,imm		alternate for reg,reg,imm
	<TMPL_base_reg,TMPL_imm,TMPL_comma,TMPL_imm2>,\				; 1=reg,byte,rot	alternate for reg,reg,byte,rot
	<TMPL_base_reg,TMPL_base_reg>,\						; 2=reg,reg		alternate for reg,reg,reg
	<TMPL_base_reg,TMPL_base_reg,TMPL_rrx_op>,\				; 3=reg,reg,rrx		alternate for reg,reg,reg,rrx
	<TMPL_base_reg,TMPL_base_reg,TMPL_shift_op,TMPL_imm>,\			; 4=reg,reg,shift imm	alternate for reg,reg,reg,shift imm
	<TMPL_base_reg,TMPL_base_reg,TMPL_shift_op,TMPL_base_reg>,\		; 5=reg,reg,shift reg	alternate for reg,reg,reg,shift reg
	<TMPL_base_reg,TMPL_base_reg,TMPL_imm>,\				; 6=reg,reg,imm
	<TMPL_base_reg,TMPL_base_reg,TMPL_imm,TMPL_comma,TMPL_imm2>,\		; 7=reg,reg,byte,rot
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg>,\				; 8=reg,reg,reg
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg,TMPL_rrx_op>,\		; 9=reg,reg,reg,rrx
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg,TMPL_shift_op,TMPL_imm>,\	;10=reg,reg,reg,shift imm
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg,TMPL_shift_op,TMPL_base_reg>	;11=reg,reg,reg,shift reg
	cmp	al,6
	jae	.encode_instruction
	mov	edx,[operand_registers]
	movzx	ecx,dl
	shl	edx,8
	or	edx,ecx
	mov	[operand_registers],edx
    .encode_shifter:
	;used by ADC, ADD, AND, BIC, EOR, ORR, RSB (NEG), RSC, SBC, SUB
	;used by MOV (LSL, LSR, ASR, ROR, RRX), MVN, CMN, CMP, TEQ, TST
	add	al,6
    .encode_instruction:
	;used by ADR, MOV reg,exp
	cmp	[operand_register0],0xf
	jnz	.pc_check_done
	or	[thumb_flags_16],THUMB_FLAG_ONLY_LAST_IT
	or	[thumb_flags_32],THUMB_FLAG_ONLY_LAST_IT
    .pc_check_done:
	test	[code_type],CPU_ACTIVITY_ARM
	jz	THUMB_rd_rn_shifter
	test	[cpu_capability_flags],CPU_CAPABILITY_V1
	jz	ERROR_requires_cpu_capability_arm_v1
    .encode:
	;used by CMNP, CMPP, TEQP, TSTP
	cmp	al,6
	jz	.reg_reg_imm
	cmp	al,7
	jz	.reg_reg_byte_rot
	cmp	al,8
	jz	.reg_reg_reg
	cmp	al,9
	jz	.reg_reg_reg_rrx
	cmp	al,10
	jz	.reg_reg_reg_shift_imm
	cmp	al,11
	jz	.reg_reg_reg_shift_reg
	ud2
    .reg_reg_byte_rot:
	cmp	[immediate_value],0xff
	ja	.byte_out_of_range
	test	[immediate_value2],not (0xf shl 1)
	jnz	.rotation_out_of_range
	mov	eax,[immediate_value2]
	shl	eax,8-1
	or	[immediate_value],eax
	jmp	.reg_reg_imm_make
    .reg_reg_imm:
	call	ARM_encode_immediate_with_opcode_swap
	jc	.immediate_out_of_range
    .reg_reg_imm_make:
	movzx	eax,[operand_register0]
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]
	shl	eax,16
	or	ebp,eax
	mov	eax,[immediate_value]
	or	ebp,eax
	bts	ebp,25
	jmp	ARM_store_instruction
    .reg_reg_reg_shift_imm:
	call	ARM_check_shift_range
    .reg_reg_reg:
	movzx	eax,[operand_register0]
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]
	or	ebp,eax
	jmp	ARM_store_instruction
    .reg_reg_reg_shift_reg:
	movzx	eax,[operand_register0]
	cmp	al,0xf
	jz	ERROR_r15_not_valid.all
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]
	cmp	al,0xf
	jz	ERROR_r15_not_valid.all
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]
	cmp	al,0xf
	jz	ERROR_r15_not_valid.all
	or	ebp,eax
	movzx	eax,[instruction_shift_op]
	shl	eax,5
	or	ebp,eax
	movzx	eax,[operand_register3]
	cmp	al,0xf
	jz	ERROR_r15_not_valid.all
	shl	eax,8
	or	ebp,eax
	bts	ebp,4
	jmp	ARM_store_instruction
    .reg_reg_reg_rrx:
	movzx	eax,[operand_register0]
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]
	or	ebp,eax
	or	ebp,ARM_SHIFT_OPCODE_ROR shl 5
	jmp	ARM_store_instruction
    .immediate_out_of_range:
	mov	ecx,ERROR_immediate_cannot_be_encoded
	jmp	ARM_store_instruction_with_error
    .byte_out_of_range:
	mov	ecx,ERROR_byte_value_out_of_range
	jmp	ARM_store_instruction_with_error
    .rotation_out_of_range:
	mov	ecx,ERROR_rotation_value_out_of_range
	jmp	ARM_store_instruction_with_error

ARM_rd_shifter:
	;used by MVN
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_imm>,\					;0=reg,imm
	<TMPL_base_reg,TMPL_imm,TMPL_comma,TMPL_imm2>,\			;1=reg,byte,rot
	<TMPL_base_reg,TMPL_base_reg>,\					;2=reg,reg
	<TMPL_base_reg,TMPL_base_reg,TMPL_rrx_op>,\			;3=reg,reg,rrx
	<TMPL_base_reg,TMPL_base_reg,TMPL_shift_op,TMPL_imm>,\		;4=reg,reg,shift imm
	<TMPL_base_reg,TMPL_base_reg,TMPL_shift_op,TMPL_base_reg>	;5=reg,reg,shift reg
    .encode:
	cmp	al,0
	jnz	.do
	mov	ecx,[immediate_value]
	cmp	ecx,0xffff
	ja	.do
	test	ebp,1 shl 22				;1=mvn
	jnz	.do
	test	[instruction_condition],FLAG_CONDITION_SET
	jnz	.do
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB
	push	eax ecx
	call	ARM_encode_immediate
	pop	[immediate_value] eax
	jnc	.do
	and	ebp,0xf shl 28
	or	ebp,0x03000000				;switch to movw
	jmp	ARM_rd_imm16.do				;do movw
    .THUMB:
	push	eax [immediate_value]
	call	THUMB_encode_immediate
	pop	[immediate_value] eax
	jnc	.do
	mov	[thumb32_instruction],0xf2400000	;switch to movw
	jmp	ARM_rd_imm16.do				;do movw
    .do:
	;used by CPY
	mov	edx,[operand_registers]
	movzx	ecx,dl
	shl	edx,8
	mov	dh,0
	or	edx,ecx
	mov	[operand_registers],edx
	jmp	ARM_rd_rn_shifter.encode_shifter

ARM_rd_shifter_exp:
	;used by MOV
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_imm>,\					;0=reg,imm
	<TMPL_base_reg,TMPL_imm,TMPL_comma,TMPL_imm2>,\			;1=reg,byte,rot
	<TMPL_base_reg,TMPL_base_reg>,\					;2=reg,reg
	<TMPL_base_reg,TMPL_base_reg,TMPL_rrx_op>,\			;3=reg,reg,rrx
	<TMPL_base_reg,TMPL_base_reg,TMPL_shift_op,TMPL_imm>,\		;4=reg,reg,shift imm
	<TMPL_base_reg,TMPL_base_reg,TMPL_shift_op,TMPL_base_reg>,\	;5=reg,reg,shift reg
	<TMPL_base_reg,TMPL_expression>					;6=reg,exp	converted to add reg,reg,imm
	cmp	al,6
	jb	ARM_rd_shifter.encode
	xor	ebp,(ARM_INSTRUCTION_OPCODE_MOV xor ARM_INSTRUCTION_OPCODE_ADD) shl 21
	xor	[arm_instruction],(ARM_INSTRUCTION_OPCODE_MOV xor ARM_INSTRUCTION_OPCODE_ADD) shl 21
	xor	[thumb32_instruction],(0x2 shl 21 + 0xf shl 16) xor (0x8 shl 21)
	jmp	ARM_rd_rn_shifter.encode_instruction

ARM_rd_rn_shift:
	;used by LSL, LSR, ASR, ROR
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_imm>,\			;0=reg,imm	alternate for reg,reg,imm
	<TMPL_base_reg,TMPL_base_reg>,\			;1=reg,reg	alternate for reg,reg,reg
	<TMPL_base_reg,TMPL_base_reg,TMPL_imm>,\	;2=reg,reg,imm
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg>	;3=reg,reg,reg
	mov	ecx,ebp
	shr	ecx,5
	and	ecx,3
	mov	[instruction_shift_op],cl
	;make the thumb32 opcode
	test	al,1
	jz	.reg_reg_imm
	;for reg,reg,reg
	shl	ecx,21
	or	ecx,0xfa00f000
	mov	[thumb32_instruction],ecx
	jmp	.thumb32_done
    .reg_reg_imm:
	shl	ecx,4
	or	ecx,0xea4f0000
	mov	[thumb32_instruction],ecx
    .thumb32_done:
	cmp	al,2
	jae	.registers_shifted
	mov	edx,[operand_registers]
	movzx	ecx,dl
	shl	edx,8
	or	edx,ecx
	mov	[operand_registers],edx
	add	al,2
    .registers_shifted:
	mov	edx,[operand_registers]
	movzx	ecx,dl
	shl	edx,8
	mov	dh,0
	or	edx,ecx
	mov	[operand_registers],edx
	add	al,2
	jmp	ARM_rd_rn_shifter.encode_shifter

ARM_rd_shift:
	;used by RRX
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg>,\				;0=reg		alternate for reg,reg
	<TMPL_base_reg,TMPL_base_reg>			;1=reg,reg
	mov	[instruction_shift_op],ARM_SHIFT_OPCODE_ROR
	cmp	al,1
	jae	.registers_shifted
	mov	edx,[operand_registers]
	movzx	ecx,dl
	shl	edx,8
	or	edx,ecx
	mov	[operand_registers],edx
    .registers_shifted:
	mov	edx,[operand_registers]
	movzx	ecx,dl
	shl	edx,8
	mov	dh,0
	or	edx,ecx
	mov	[operand_registers],edx
	mov	al,3
	jmp	ARM_rd_rn_shifter.encode_shifter

ARM_rn_shifter:
	;used by CMN, CMP, TEQ, TST
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_imm>,\					;0=reg,imm
	<TMPL_base_reg,TMPL_imm,TMPL_comma,TMPL_imm2>,\			;1=reg,byte,rot
	<TMPL_base_reg,TMPL_base_reg>,\					;2=reg,reg
	<TMPL_base_reg,TMPL_base_reg,TMPL_rrx_op>,\			;3=reg,reg,rrx
	<TMPL_base_reg,TMPL_base_reg,TMPL_shift_op,TMPL_imm>,\		;4=reg,reg,shift imm
	<TMPL_base_reg,TMPL_base_reg,TMPL_shift_op,TMPL_base_reg>	;5=reg,reg,shift reg
	mov	edx,[operand_registers]
	shl	edx,8
	mov	[operand_registers],edx
	jmp	ARM_rd_rn_shifter.encode_shifter

ARM_rn_shifter_26bit:
	;used by CMNP, CMPP, TEQP, TSTP
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_imm>,\					;0=reg,imm
	<TMPL_base_reg,TMPL_imm,TMPL_comma,TMPL_imm2>,\			;1=reg,byte,rot
	<TMPL_base_reg,TMPL_base_reg>,\					;2=reg,reg
	<TMPL_base_reg,TMPL_base_reg,TMPL_rrx_op>,\			;3=reg,reg,rrx
	<TMPL_base_reg,TMPL_base_reg,TMPL_shift_op,TMPL_imm>,\		;4=reg,reg,shift imm
	<TMPL_base_reg,TMPL_base_reg,TMPL_shift_op,TMPL_base_reg>	;5=reg,reg,shift reg
	test	[code_type],CPU_ACTIVITY_ARM
	jz	ERROR_instruction_not_16bit
	test	[cpu_capability_flags],CPU_CAPABILITY_26BIT
	jz	ERROR_requires_cpu_capability_arm_26bit
	mov	edx,[operand_registers]
	shl	edx,8
	mov	[operand_registers],edx
	add	al,6
	jmp	ARM_rd_rn_shifter.encode

ARM_rd_address2:
	test	[code_type],CPU_ACTIVITY_ARM
	jz	THUMB2_rd_address
	;used by LDR, LDRB, STR, STRB, selectively: PLD, LDRT, LDRBT, STRT, STRBT
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\									; 0=rd,[rn]
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_add_sub_reg>,\						; 1=rd,[rn],+-rm
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_add_sub_reg,TMPL_shift_op,TMPL_imm>,\				; 2=rd,[rn],+-rm,shift imm
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_add_sub_reg,TMPL_rrx_op>,\					; 3=rd,[rn],+-rm,rrx
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_imm>,\							; 4=rd,[rn],imm
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\							; 5=rd,[rn,imm]
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right,TMPL_modifier_exclaim>,\					; 6=rd,[rn,imm]!
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_bracket_right>,\						; 7=rd,[rn,+-rm]
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_bracket_right,TMPL_modifier_exclaim>,\				; 8=rd,[rn,+-rm]!
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_shift_op,TMPL_imm,TMPL_bracket_right>,\				; 9=rd,[rn,+-rm,shift imm]
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_shift_op,TMPL_imm,TMPL_bracket_right,TMPL_modifier_exclaim>,\	;10=rd,[rn,+-rm,shift imm]!
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_rrx_op,TMPL_bracket_right>,\					;11=rd,[rn,+-rm,rrx]
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_rrx_op,TMPL_bracket_right,TMPL_modifier_exclaim>,\		;12=rd,[rn,+-rm,rrx]!
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address,TMPL_bracket_right>,\									;13=rd,[imm]	PC relative
	<TMPL_base_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>,\									;14=rd,[exp]	implicit reg from structure
	<TMPL_base_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right,TMPL_modifier_exclaim>						;15=rd,[exp]!	implicit reg from structure
	test	[cpu_capability_flags],CPU_CAPABILITY_V1
	jz	ERROR_requires_cpu_capability_arm_v1
    ;check that rd!=15 unless LDR, STR
	cmp	[operand_size],4
	jnz	.check_rd
    ;check for LDRT
	mov	edx,[arm_instruction]
	and	edx,0xf shl 21
	cmp	edx,1 shl 21
	jnz	.process
    .check_rd:
	mov	cl,[operand_register0]
	cmp	cl,15
	jz	ERROR_r15_not_valid.first
    .process:
	cmp	al,0
	jz	.rd_q_rn_p
	cmp	al,1
	jz	.rd_q_rn_p_PMrm
	cmp	al,2
	jz	.rd_q_rn_p_PMrm_shift_imm
	cmp	al,3
	jz	.rd_q_rn_p_PMrm_rrx
	cmp	al,4
	jz	.rd_q_rn_p_imm
	cmp	al,5
	jz	.rd_q_rn_imm_p
	cmp	al,6
	jz	.rd_q_rn_imm_p!
	cmp	al,7
	jz	.rd_q_rn_PMrm_p
	cmp	al,8
	jz	.rd_q_rn_PMrm_p!
	cmp	al,9
	jz	.rd_q_rn_PMrm_shift_imm_p
	cmp	al,10
	jz	.rd_q_rn_PMrm_shift_imm_p!
	cmp	al,11
	jz	.rd_q_rn_PMrm_rrx_p
	cmp	al,12
	jz	.rd_q_rn_PMrm_rrx_p!
	cmp	al,13
	jz	.rd_q_imm_p
	cmp	al,14
	jz	.rd_q_exp_p
	cmp	al,15
	jz	.rd_q_exp_p!
	ud2
    .rd_q_rn_p_imm:
	cmp	[immediate_value],0
	jz	.rd_q_rn_p
    .rd_q_rn_p_imm.do:
	cmp	[operand_register1],0xf
	jz	ERROR_r15_not_valid.post
	mov	ax,word[operand_register0]
	cmp	al,ah
	jz	ERROR_base_and_dest_must_differ_with_writeback
	jmp	.rd_rn_imm
    .rd_q_rn_imm_p!:
	cmp	[operand_register1],0xf
	jz	ERROR_r15_not_valid.post
	mov	ax,word[operand_register0]
	cmp	al,ah
	jz	ERROR_base_and_dest_must_differ_with_writeback
	mov	eax,[immediate_value]
	test	eax,eax
	jz	.rd_q_rn_imm_p
	xor	ebp,1 shl 21 + 1 shl 24
	jmp	.rd_rn_imm
    .rd_q_rn_p:
    .rd_q_rn_imm_p:
	xor	ebp,1 shl 24
    .rd_rn_imm:
	or	ebp,1 shl 23
	movzx	eax,[operand_register0]
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]
	shl	eax,16
	or	ebp,eax
	mov	eax,[immediate_value]
	cmp	eax,0xfff
	jg	.immediate_offset_out_of_range
	cmp	eax,-0xfff
	jl	.immediate_offset_out_of_range
	test	eax,eax
	jns	.store_offset
	neg	eax
	btr	ebp,23
    .store_offset:
	or	ebp,eax
	jmp	ARM_store_instruction
    .rd_q_rn_PMrm_p!:
	mov	[instruction_shift_op],0
	mov	ax,word[operand_register0]
	cmp	al,ah
	jz	ERROR_base_and_dest_must_differ_with_writeback
	jmp	.rd_q_rn_PMrm_shift_imm_p!
    .rd_q_rn_p_PMrm:
	mov	[instruction_shift_op],0
	mov	ax,word[operand_register0]
	cmp	al,ah
	jz	ERROR_base_and_dest_must_differ_with_writeback
	jmp	.rd_q_rn_p_PMrm_shift_imm
    .rd_q_rn_PMrm_p:
	mov	[instruction_shift_op],0
	jmp	.rd_q_rn_PMrm_shift_imm_p
    .rd_q_rn_PMrm_shift_imm_p!:
	call	ARM_check_shift_range
	xor	ebp,1 shl 21 + 1 shl 24
	cmp	[operand_register1],0xf
	jz	ERROR_r15_not_valid.post
	mov	ax,word[operand_register0]
	cmp	al,ah
	jz	ERROR_base_and_dest_must_differ_with_writeback
	jmp	.check_rn_rm
    .rd_q_rn_p_PMrm_shift_imm:
	call	ARM_check_shift_range
	cmp	[operand_register1],0xf
	jz	ERROR_r15_not_valid.post
	mov	ax,word[operand_register0]
	cmp	al,ah
	jz	ERROR_base_and_dest_must_differ_with_writeback
    .check_rn_rm:
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jnz	.rd_rn_PMrm_shift_imm
	mov	ax,word[operand_register1]
	and	ax,0x7f7f
	cmp	al,ah
	jz	ERROR_base_and_offset_must_differ_with_writeback
	jmp	.rd_rn_PMrm_shift_imm
    .rd_q_rn_PMrm_shift_imm_p:
	call	ARM_check_shift_range
	xor	ebp,1 shl 24
    .rd_rn_PMrm_shift_imm:
	or	ebp,1 shl 23 + 1 shl 25
	movzx	eax,[operand_register0]
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]
	test	al,al
	jns	.store_reg
	and	eax,0xf
	btr	ebp,23
    .store_reg:
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.third
	or	ebp,eax
	jmp	ARM_store_instruction
    .rd_q_rn_PMrm_rrx_p!:
	xor	ebp,1 shl 21 + 1 shl 24 + 3 shl 5
	jmp	.rd_q_rn_p_PMrm_rrx.do
    .rd_q_rn_p_PMrm_rrx:
	or	ebp,3 shl 5
    .rd_q_rn_p_PMrm_rrx.do:
	cmp	[operand_register1],0xf
	jz	ERROR_r15_not_valid.second
	mov	ax,word[operand_register0]
	cmp	al,ah
	jz	ERROR_base_and_dest_must_differ_with_writeback
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jnz	.rd_rn_PMrm_shift_imm
	mov	ax,word[operand_register1]
	and	ax,0x7f7f
	cmp	al,ah
	jz	ERROR_base_and_offset_must_differ_with_writeback
	jmp	.rd_rn_PMrm_shift_imm
    .rd_q_rn_PMrm_rrx_p:
	xor	ebp,1 shl 24 + 3 shl 5
	jmp	.rd_rn_PMrm_shift_imm
    .rd_q_imm_p:
	mov	[operand_register1],0xf
	mov	eax,[addressing_space]
	mov	eax,[eax+0x00]
	add	eax,[immediate_value]
	sub	eax,edi
	sub	eax,8
	mov	[immediate_value],eax
	jmp	.rd_q_rn_imm_p
    .rd_q_exp_p:
	jmp	.rd_q_rn_imm_p
    .rd_q_exp_p!:
	jmp	.rd_q_rn_imm_p!
    .immediate_offset_out_of_range:
	mov	ecx,ERROR_immediate_offset_out_of_range.0xfff
	jmp	ARM_store_instruction_with_error

ARM_rd_address2_post:
	test	[code_type],CPU_ACTIVITY_ARM
	jz	THUMB2_rd_address
	;used by LDRT, LDRBT, STRT, STRBT
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\						; 0=rd,[rn]
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_add_sub_reg>,\			; 1=rd,[rn],+-rm
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_add_sub_reg,TMPL_shift_op,TMPL_imm>,\	; 2=rd,[rn],+-rm,shift imm
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_add_sub_reg,TMPL_rrx_op>,\		; 3=rd,[rn],+-rm,rrx
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_imm>					; 4=rd,[rn],imm
	test	[cpu_capability_flags],CPU_CAPABILITY_V1
	jz	ERROR_requires_cpu_capability_arm_v1
	cmp	[operand_register0],15
	jz	ERROR_r15_not_valid.first
	cmp	al,0
	jz	ARM_rd_address2.rd_q_rn_p_imm.do
	cmp	al,1
	jz	ARM_rd_address2.rd_q_rn_p_PMrm
	cmp	al,2
	jz	ARM_rd_address2.rd_q_rn_p_PMrm_shift_imm
	cmp	al,3
	jz	ARM_rd_address2.rd_q_rn_p_PMrm_rrx
	cmp	al,4
	jz	ARM_rd_address2.rd_q_rn_p_imm.do
	ud2

ARM_rn_address4:
	;used by LDM, STM
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_reg_list>,\				;0=rn,{..}
	<TMPL_base_reg,TMPL_reg_list,TMPL_modifier_translate>,\	;1=rn,{..}^
	<TMPL_base_reg!,TMPL_reg_list>,\			;2=rn!,{..}
	<TMPL_base_reg!,TMPL_reg_list,TMPL_modifier_translate>	;3=rn!,{..}^
	test	[code_type],CPU_ACTIVITY_ARM
	jz	THUMB_rn_address4
	test	[cpu_capability_flags],CPU_CAPABILITY_V1
	jz	ERROR_requires_cpu_capability_arm_v1
	test	al,1
	jnz	.do_multiple				;cannot change to ldr when using translate
    .do:
	mov	ecx,[reg_list_bitmap]
	dec	ecx
	and	ecx,[reg_list_bitmap]
	jz	.do_single				;single registers use a different encoding
    .do_multiple:
	cmp	al,0
	jz	.rn_list
	cmp	al,1
	jz	.rn_list_t
	cmp	al,2
	jz	.rn!_list
	cmp	al,3
	jz	.rn!_list_t
	ud2
    .rn_list:
	mov	ecx,0
	jmp	.make
    .rn_list_t:
	mov	ecx,1 shl 22
	jmp	.make
    .rn!_list:
	mov	ecx,1 shl 21
	jmp	.make
    .rn!_list_t:
	test	[reg_list_bitmap],1 shl 15		;if PC is not present then we can't use writeback
	jz	ERROR_register_writeback_not_allowed
	mov	ecx,1 shl 22 + 1 shl 21
    .make:
	or	ebp,ecx
	or	ebp,[reg_list_bitmap]
	movzx	ecx,[operand_register0]
	bt	ebp,21
	jnc	.dest_okay
	bt	ebp,20
	jc	.ldm_check
    ;stm check
	bt	ebp,22					;translate?
	jnc	.writeback_okay
	bt	ebp,21					;writeback?
	jc	ERROR_register_writeback_not_allowed
    .writeback_okay:
	bsf	edx,ebp
	cmp	edx,ecx
	jz	.dest_okay
    .ldm_check:
	bt	ebp,ecx
	jc	ERROR_destination_register_not_allowed_in_list
    .dest_okay:
	cmp	ecx,15					;PC?
	jz	ERROR_r15_not_valid.first
	shl	ecx,16
	or	ebp,ecx
	jmp	ARM_store_instruction
    .do_single:
	and	eax,2
	shl	eax,21-1
	or	ebp,eax
	bsf	ecx,[reg_list_bitmap]
	mov	ch,[operand_register0]
	mov	[operand_registers],ecx
	mov	[operand_size],4
	mov	ecx,ebp
	and	ecx,1 shl 24 + 1 shl 21
	;IA,DA		; 0=rd,[rn]
	;IA!,DA!	; 4=rd,[rn],imm
	;IB,DB		; 5=rd,[rn,imm]
	;IB!,DB!	; 6=rd,[rn,imm]!
	mov	eax,0					; 0=rd,[rn]
	mov	edx,4					; 4=rd,[rn],imm
	cmp	ecx,0 shl 24 + 1 shl 21
	cmovz	eax,edx
	mov	edx,5					; 5=rd,[rn,imm]
	cmp	ecx,1 shl 24 + 0 shl 21
	cmovz	eax,edx
	mov	edx,6					; 6=rd,[rn,imm]!
	cmp	ecx,1 shl 24 + 1 shl 21
	cmovz	eax,edx
	mov	ecx,eax
	and	ecx,4
	mov	edx,ecx
	neg	edx
	test	ebp,1 shl 23
	cmovz	ecx,edx
	mov	[immediate_value],ecx
	movzx	ecx,[instruction_condition]
	and	ecx,0xf0
	shl	ecx,24
	and	ebp,0x00100000
	lea	ebp,[ebp+ecx+0x04000000]
	jmp	ARM_rd_address2.process

ARM_address4:
	;used by PUSH, POP
	call	decode_template
    TEMPLATE \
	<TMPL_reg_list>					;0={..}
	mov	[operand_register0],13	;sp
	test	[code_type],CPU_ACTIVITY_ARM
	jz	THUMB_address4
	test	[cpu_capability_flags],CPU_CAPABILITY_V1
	jz	ERROR_requires_cpu_capability_arm_v1
	mov	al,2					;2=rn!,{..}
	jmp	ARM_rn_address4.do

ARM_target:
	;used by B, BL
	call	decode_template
    TEMPLATE \
	<TMPL_address>					;0=imm	PC relative
	test	[code_type],CPU_ACTIVITY_ARM
	jz	THUMB_B_target
	test	[cpu_capability_flags],CPU_CAPABILITY_V1
	jz	ERROR_requires_cpu_capability_arm_v1
	mov	eax,[addressing_space]
	mov	eax,[eax+0x00]
	add	eax,[immediate_value]
	sub	eax,edi
	sub	eax,8
	test	eax,3
	jnz	.not_aligned
	cmp	eax,1 shl 25
	jge	.jump_out_of_range
	cmp	eax,-(1 shl 25)
	jl	.jump_out_of_range
	shr	eax,2
	and	eax,0x00ffffff
	or	ebp,eax
	or	ebp,1 shl 27 + 1 shl 25
	jmp	ARM_store_instruction
    .jump_out_of_range:
	mov	ecx,ERROR_relative_jump_out_of_range
	jmp	ARM_store_instruction_with_error
    .not_aligned:
	mov	ecx,ERROR_relative_jump_not_aligned
	jmp	ARM_store_instruction_with_error

ARM_immediate24:
	;used by SWI
	call	decode_template
    TEMPLATE \
	<TMPL_imm>					;0=imm
	test	[code_type],CPU_ACTIVITY_ARM
	jz	THUMB_immediate8
	test	[cpu_capability_flags],CPU_CAPABILITY_V1
	jz	ERROR_requires_cpu_capability_arm_v1
	mov	eax,[immediate_value]
	cmp	eax,0x00ffffff
	ja	.out_of_range
	or	ebp,eax
	jmp	ARM_store_instruction
    .out_of_range:
	mov	ecx,ERROR_immediate_offset_out_of_range.0_0xffffff
	jmp	ARM_store_instruction_with_error

ARM_rd_target:
	;used by ADR
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_address>			;0=reg,imm
	mov	[operand_register1],0xf			;rn=pc
	mov	ecx,[addressing_space]
	mov	ecx,[ecx+0x00]
	add	ecx,[immediate_value]
	sub	ecx,edi
	sub	ecx,8
	test	[code_type],CPU_ACTIVITY_ARM
	jnz	.offset_done
	call	THUMB_query_condition_pc		;adjust for THUMB
	mov	eax,[addressing_space]
	sub	ecx,[eax+0x00]
	add	ecx,4
	and	ecx,not 3
	neg	ecx
	add	ecx,[immediate_value]
    .offset_done:
	mov	[immediate_value],ecx
	mov	al,6					;5=reg,reg,imm
	jmp	ARM_rd_rn_shifter.encode_instruction

ARM_rd_rm:
	;used by NEG
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg>,\				;0=reg
	<TMPL_base_reg,TMPL_base_reg>			;1=reg,reg
	cmp	al,0
	jnz	.registers_shifted
	mov	edx,[operand_registers]
	movzx	ecx,dl
	shl	edx,8
	or	edx,ecx
	mov	[operand_registers],edx
    .registers_shifted:
	mov	al,0					;reg,reg,imm
	jmp	ARM_rd_rn_shifter.encode_shifter

;V2

ARM_copro_op1_crd_crn_crm_op2:
	;used by CDP, CDP2
	call	decode_template
    TEMPLATE \
	<TMPL_cpro_sel,TMPL_copro_opcode1,TMPL_cpro_reg,TMPL_cpro_reg,TMPL_cpro_reg>,\			;0=copro,op1,crd,crn,crm
	<TMPL_cpro_sel,TMPL_copro_opcode1,TMPL_cpro_reg,TMPL_cpro_reg,TMPL_cpro_reg,TMPL_copro_opcode2>	;1=copro,op1,crd,crn,crm,op2
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.check_v7m
	cmp	ebp,0xf shl 28				;CDP2?
	jae	.check_v5
	jmp	.check_v2
    .check_v7m:
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	ERROR_requires_cpu_capability_arm_7m
	jmp	.version_check_okay
    .check_v2:
	test	[cpu_capability_flags],CPU_CAPABILITY_V2
	jz	ERROR_requires_cpu_capability_arm_v2
	jmp	.version_check_okay
    .check_v5:
	test	[cpu_capability_flags],CPU_CAPABILITY_V5
	jz	ERROR_requires_cpu_capability_arm_v5
    .version_check_okay:
	movzx	eax,[operand_register0]			;cpnum
	shl	eax,8
	or	ebp,eax
	movzx	eax,[copro_opcode1]			;op1
	shl	eax,20
	or	ebp,eax
	movzx	eax,[operand_register1]			;crd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register2]			;crn
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register3]			;crm
	or	ebp,eax
	movzx	eax,[copro_opcode2]			;op2
	shl	eax,5
	or	ebp,eax
	jmp	ARM_post_process_copro

ARM_copro_crd_address5:
	;used by LDC, LDCL, LDC2, LDC2L, STC, STCL, STC2, STC2L
	call	decode_template
    TEMPLATE \
	<TMPL_cpro_sel,TMPL_cpro_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\					;0=copro,crd,[rn]
	<TMPL_cpro_sel,TMPL_cpro_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_imm>,\				;1=copro,crd,[rn],imm
	<TMPL_cpro_sel,TMPL_cpro_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_option>,\			;2=copro,crd,[rn],{imm}
	<TMPL_cpro_sel,TMPL_cpro_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\				;3=copro,crd,[rn,imm]
	<TMPL_cpro_sel,TMPL_cpro_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right,TMPL_modifier_exclaim>,\	;4=copro,crd,[rn,imm]!
	<TMPL_cpro_sel,TMPL_cpro_reg,TMPL_bracket_left,TMPL_address,TMPL_bracket_right>,\					;5=copro,crd,[imm]	PC relative
	<TMPL_cpro_sel,TMPL_cpro_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>,\					;6=copro,crd,[exp]	implicit reg from structure
	<TMPL_cpro_sel,TMPL_cpro_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right,TMPL_modifier_exclaim>		;7=copro,crd,[exp]!	implicit reg from structure
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.check_v7m
	cmp	ebp,0xf shl 28				;LDC2/STC2?
	jae	.check_v5
	jmp	.check_v2
    .check_v7m:
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	ERROR_requires_cpu_capability_arm_7m
	jmp	.version_check_okay
    .check_v2:
	test	[cpu_capability_flags],CPU_CAPABILITY_V2
	jz	ERROR_requires_cpu_capability_arm_v2
	jmp	.version_check_okay
    .check_v5:
	test	[cpu_capability_flags],CPU_CAPABILITY_V5
	jz	ERROR_requires_cpu_capability_arm_v5
    .version_check_okay:
	cmp	al,0
	jz	.copro_crd_q_rn_p
	cmp	al,1
	jz	.copro_crd_q_rn_p_imm
	cmp	al,2
	jz	.copro_crd_q_rn_p_option
	cmp	al,3
	jz	.copro_crd_q_rn_imm_p
	cmp	al,4
	jz	.copro_crd_q_rn_imm_p!
	cmp	al,5
	jz	.copro_crd_q_imm_p
	cmp	al,6
	jz	.copro_crd_q_exp_p
	cmp	al,7
	jz	.copro_crd_q_exp_p!
	ud2
    .copro_crd_q_rn_p_option:
	cmp	[immediate_value],0xff
	ja	.option_out_of_range
	shl	[immediate_value],2
	jmp	.encode
    .copro_crd_q_imm_p:
	mov	[operand_register2],0xf
	call	THUMB_query_condition_pc
	mov	eax,[addressing_space]
	sub	ecx,[eax+0x00]
	and	ecx,not 3				;round down
	neg	ecx
	add	ecx,[immediate_value]
	test	[code_type],CPU_ACTIVITY_ARM
	setnz	al
	movzx	eax,al
	shl	eax,2
	add	eax,4					;offset=4 for THUMB and 8 for ARM
	sub	ecx,eax
	mov	[immediate_value],ecx
    .copro_crd_q_rn_p:
    .copro_crd_q_exp_p:
    .copro_crd_q_rn_imm_p:
	or	ebp,1 shl 24				;P bit
	jmp	.encode
    .copro_crd_q_exp_p!:
    .copro_crd_q_rn_imm_p!:
	cmp	[operand_register2],0xf
	jz	ERROR_r15_not_valid.third
	or	ebp,1 shl 24				;P bit
    .copro_crd_q_rn_p_imm:
	cmp	[immediate_value],0
	jz	.copro_crd_q_rn_p
	or	ebp,1 shl 21				;W bit
	cmp	[operand_register2],0xf
	jz	ERROR_r15_not_valid.third
    .encode:
	movzx	eax,[operand_register0]			;cpnum
	shl	eax,8
	or	ebp,eax
	movzx	eax,[operand_register1]			;crd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register2]			;rn
	shl	eax,16
	or	ebp,eax
	mov	eax,[immediate_value]
	test	eax,3
	jnz	.out_of_range
	sar	eax,2
	mov	ecx,1 shl 23				;U bit
	or	ecx,ebp
	mov	edx,eax
	neg	edx
	test	eax,eax
	cmovns	ebp,ecx
	cmovs	eax,edx
	cmp	eax,0xff
	ja	.out_of_range
	or	ebp,eax
	jmp	ARM_post_process_copro
    .out_of_range:
	mov	ecx,ERROR_immediate_offset_out_of_range.0x3fc
	jmp	ARM_post_process_copro_with_error
    .option_out_of_range:
	mov	ecx,ERROR_option_out_of_range
	jmp	ARM_post_process_copro_with_error

ARM_copro_op1_rd_crn_crm_op2:
	;used by MCR, MCR2, MRC, MRC2
	call	decode_template
    TEMPLATE \
	<TMPL_cpro_sel,TMPL_copro_opcode1,TMPL_base_reg,TMPL_cpro_reg,TMPL_cpro_reg>,\			;0=copro,op1,rd,crn,crm
	<TMPL_cpro_sel,TMPL_copro_opcode1,TMPL_base_reg,TMPL_cpro_reg,TMPL_cpro_reg,TMPL_copro_opcode2>	;1=copro,op1,rd,crn,crm,op2
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.check_v7m
	cmp	ebp,0xf shl 28				;MCR2 or MRC2?
	jae	.check_v5
	jmp	.check_v2
    .check_v7m:
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	ERROR_requires_cpu_capability_arm_7m
	jmp	.version_check_okay
    .check_v2:
	test	[cpu_capability_flags],CPU_CAPABILITY_V2
	jz	ERROR_requires_cpu_capability_arm_v2
	jmp	.version_check_okay
    .check_v5:
	test	[cpu_capability_flags],CPU_CAPABILITY_V5
	jz	ERROR_requires_cpu_capability_arm_v5
    .version_check_okay:
	movzx	eax,[operand_register0]			;cpnum
	shl	eax,8
	or	ebp,eax
	movzx	eax,[copro_opcode1]			;op1
	cmp	eax,7
	ja	.out_of_range
	shl	eax,21
	or	ebp,eax
	movzx	eax,[operand_register1]			;rd
	cmp	eax,15
	jz	ERROR_r15_not_valid.third
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register2]			;crn
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register3]			;crm
	or	ebp,eax
	movzx	eax,[copro_opcode2]			;op2
	shl	eax,5
	or	ebp,eax
	jmp	ARM_post_process_copro
    .out_of_range:
	mov	ecx,ERROR_opcode3_out_of_range
	jmp	ARM_post_process_copro_with_error

ARM_rd_rm_rs_rn:
	;used by MLA, MLS
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg,TMPL_base_reg>	;0=rd,rm,rs,rn
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_rd_rm_rs_rn.7m
	test	ebp,1 shl 22				;MLS?
	jz	.do
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	ERROR_requires_cpu_capability_arm_t2
	jmp	.encode
    .do:
	;used by MUL
	test	[cpu_capability_flags],CPU_CAPABILITY_V2
	jz	ERROR_requires_cpu_capability_arm_v2
    .encode:
	;used by SMLAD, SMLADX, SMLSD, SMLSDX, SMMLA, SMMLAR, SMMLS, SMMLSR, USADA8
	movzx	eax,[operand_register0]			;rd
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.first
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register1]			;rm
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.second
	or	ebp,eax
	movzx	eax,[operand_register2]			;rs
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.third
	shl	eax,8
	or	ebp,eax
	movzx	eax,[operand_register3]			;rn
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.fourth
	shl	eax,12
	or	ebp,eax
    ;version 6 and above relax the rd!=rm restriction
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jnz	ARM_store_instruction
	mov	al,[operand_register0]			;rd
	mov	ah,[operand_register1]			;rm
	cmp	al,ah
	jz	ERROR_source_rm_and_dest_must_differ
	jmp	ARM_store_instruction
    .THUMB_rd_rm_rs_rn.7m:
	test	[instruction_condition],FLAG_CONDITION_SET
	jnz	THUMB_post_process
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_7m
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	THUMB_post_process
	jmp	.THUMB_rd_rm_rs_rn.encode
    .THUMB_rd_rm_rs_rn:
	;used by SMLAD, SMLADX, SMLSD, SMLSDX, SMMLA, SMMLAR, SMMLS, SMMLSR, USADA8
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_t2
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	THUMB_post_process
    .THUMB_rd_rm_rs_rn.encode:
	mov	cl,0xff
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	ebp,[thumb32_instruction]
	movzx	eax,[operand_register0]			;rd
	shl	eax,8
	or	ebp,eax
	movzx	eax,[operand_register1]			;rm
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]			;rs
	or	ebp,eax
	movzx	eax,[operand_register3]			;rn
	shl	eax,12
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_rd_rm_rs:
	;used by MUL
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg>,\			;0=rd,rm
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg>	;1=rd,rm,rs
	mov	[operand_register3],0			;rn=0
	cmp	al,0
	jnz	.do
	mov	al,[operand_register0]
	mov	[operand_register2],al
    .do:
	test	[code_type],CPU_ACTIVITY_ARM
	jz	THUMB_MUL_rd_rm_rs
	jmp	ARM_rd_rm_rs_rn.do

;v2a

ARM_rd_rm_q_rn_p:
	;used by SWP, SWPB
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>	;0=rd,rm,[rn]
	test	[code_type],CPU_ACTIVITY_ARM
	jz	ERROR_instruction_not_16bit
	test	[cpu_capability_flags],CPU_CAPABILITY_A
	jz	ERROR_requires_cpu_capability_arm_v2a
	movzx	eax,[operand_register0]			;rd
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.first
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;rm
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.second
	or	ebp,eax
	movzx	eax,[operand_register2]			;rn
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.third
	shl	eax,16
	or	ebp,eax
	mov	al,[operand_register0]			;rd
	mov	ah,[operand_register1]			;rm
	mov	cl,[operand_register2]			;rn
	cmp	cl,al
	jz	ERROR_memory_address_cannot_be_source_or_dest
	cmp	cl,ah
	jz	ERROR_memory_address_cannot_be_source_or_dest
	jmp	ARM_store_instruction

;v3

ARM_rd_psr:
	;used by MRS
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_sysm_reg>,\			;0=rd,sysm
	<TMPL_base_reg,TMPL_psr>,\			;1=rd,psr
	<TMPL_base_reg,TMPL_banked_reg>			;2=rd,banked_reg
	cmp	al,2
	jz	.banked
	cmp	al,0
	jz	.THUMB_sysm
	movzx	ecx,[operand_register1]			;PSR
	mov	edx,32
	cmp	ecx,15
	cmovz	ecx,edx
	mov	edx,33
	cmp	ecx,31
	cmovz	ecx,edx
	cmp	ecx,32
	jb	ERROR_must_use_full_psr
	and	ecx,1					;ecx = R bit
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_rd_psr
   .encode:
	test	[cpu_capability_flags],CPU_CAPABILITY_V3
	jz	ERROR_requires_cpu_capability_arm_v3
	movzx	eax,[operand_register0]			;rd
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.first
	shl	eax,12
	or	ebp,eax
	shl	ecx,22
	or	ebp,ecx
	jmp	ARM_store_instruction
    .ARM_sysm:
	movzx	ecx,[operand_register1]
	test	ecx,ecx					;APSR?
	jnz	ERROR_instruction_not_32bit
	jmp	.encode
    .THUMB_sysm:
	test	[code_type],CPU_ACTIVITY_ARM
	jnz	.ARM_sysm
	xor	ecx,ecx
	movzx	edx,[operand_register1]
	or	[thumb32_instruction],edx
	cmp	edx,20					;control is 6M
	jz	.THUMB_check_6M
	cmp	edx,17					;17+ are 7M
	jae	.THUMB_check_7M
    .THUMB_check_6M:
	test	[cpu_capability_flags],CPU_CAPABILITY_6M
	jz	ERROR_requires_cpu_capability_arm_6m
	jmp	.THUMB_rd_psr.encode
    .THUMB_check_7M:
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	ERROR_requires_cpu_capability_arm_7m
	jmp	.THUMB_rd_psr.encode
    .THUMB_rd_psr:
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	ERROR_requires_cpu_capability_arm_t2
    .THUMB_rd_psr.encode:
	mov	ebp,[thumb32_instruction]
	mov	[thumb32_error],ERROR_r13_r15_not_valid
	movzx	eax,[operand_register0]			;rd
	cmp	eax,0xf
	jz	THUMB_post_process
	cmp	eax,0xd
	jz	THUMB_post_process
	shl	eax,8
	or	ebp,eax
	shl	ecx,20
	or	ebp,ecx
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process
    .banked:
	test	[cpu_capability_flags],CPU_CAPABILITY_VE
	jz	ERROR_requires_cpu_capability_arm_ve
	movzx	eax,[operand_register0]			;rd
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.second
	movzx	ebp,[operand_register1]
	mov	ecx,ebp
	mov	edx,ebp
	and	ebp,0x0f				;get m1 field
	and	ecx,0x10				;get m field
	and	edx,0x20				;get R field
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_banked
	shl	ebp,16
	shl	ecx,8-4
	shl	edx,22-5
	or	ebp,ecx
	or	ebp,edx
	or	ebp,eax
	or	ebp,0x0100f200
	movzx	eax,[instruction_condition]
	and	eax,0xf0
	shl	eax,28-4
	or	ebp,eax
	jmp	ARM_store_instruction
    .THUMB_banked:
	cmp	eax,0xd
	jz	ERROR_r13_not_valid.second
	shl	ebp,8
	shl	ecx,4-4
	shl	edx,20-5
	shl	eax,16
	or	ebp,ecx
	or	ebp,edx
	or	ebp,eax
	or	ebp,0xf3e08020
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_psr_value:
	;used by MSR
	call	decode_template
    TEMPLATE \
	<TMPL_sysm_reg,TMPL_base_reg>,\			;0=sysm,rn
	<TMPL_psr,TMPL_base_reg>,\			;1=psr,rn
	<TMPL_psr,TMPL_imm>,\				;2=psr,imm
	<TMPL_banked_reg,TMPL_base_reg>			;3=banked_reg,rn
	cmp	al,3
	jz	.banked
	sub	al,1
	jc	.THUMB_sysm
	movzx	edx,[operand_register0]			;psr
	mov	ecx,0x09				;CPSR_fc
	cmp	edx,32					;CPSR?
	cmovz	edx,ecx
	mov	ecx,0x19				;SPSR_fc
	cmp	edx,33					;SPSR?
	cmovz	edx,ecx
	mov	ecx,0x0c				;APSR_nzcvqg
	cmp	edx,34					;APSR?
	cmovz	edx,ecx
	mov	ecx,0x10				;extract R bit
	and	ecx,edx
	and	edx,0xf
	cmp	al,1
	jz	.psr_imm
    .psr_rm:
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_psr_rm
    .encode:
	test	[cpu_capability_flags],CPU_CAPABILITY_V3
	jz	ERROR_requires_cpu_capability_arm_v3
	shl	edx,16
	or	ebp,edx
	shl	ecx,22-4
	or	ebp,ecx
	movzx	eax,[operand_register1]			;rn
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.second
	or	ebp,eax
	jmp	ARM_store_instruction
    .psr_imm:
	test	[code_type],CPU_ACTIVITY_ARM
	jz	ERROR_instruction_not_16bit
	test	[cpu_capability_flags],CPU_CAPABILITY_V3
	jz	ERROR_requires_cpu_capability_arm_v3
	shl	edx,16
	or	ebp,edx
	shl	ecx,22-4
	or	ebp,ecx
	or	ebp,1 shl 25
	call	ARM_encode_immediate
	jc	ERROR_immediate_cannot_be_encoded
	or	ebp,[immediate_value]
	jmp	ARM_store_instruction
    .ARM_sysm:
	xor	ecx,ecx
	movzx	edx,[operand_register0]
	test	edx,edx					;APSR?
	jnz	ERROR_instruction_not_32bit
	mov	edx,0x0c				;APSR_nzcvqg
	jmp	.encode
    .THUMB_sysm:
	test	[code_type],CPU_ACTIVITY_ARM
	jnz	.ARM_sysm
	movzx	ecx,[operand_register0]
	or	[thumb32_instruction],ecx
	cmp	ecx,20					;control is 6M
	jz	.THUMB_check_6M
	cmp	ecx,17					;17+ are 7M
	jae	.THUMB_check_7M
    .THUMB_check_6M:
	test	[cpu_capability_flags],CPU_CAPABILITY_6M
	jz	ERROR_requires_cpu_capability_arm_6m
	xor	ecx,ecx
	mov	edx,8
	jmp	.THUMB_rd_psr.encode
    .THUMB_check_7M:
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	ERROR_requires_cpu_capability_arm_7m
	xor	ecx,ecx
	mov	edx,8
	jmp	.THUMB_rd_psr.encode
    .THUMB_psr_rm:
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	ERROR_requires_cpu_capability_arm_t2
    .THUMB_rd_psr.encode:
	mov	ebp,[thumb32_instruction]
	shl	edx,8
	or	ebp,edx
	shl	ecx,20-4
	or	ebp,ecx
	mov	[thumb32_error],ERROR_r13_r15_not_valid
	movzx	eax,[operand_register1]			;rn
	cmp	eax,0xf
	jz	THUMB_post_process
	cmp	eax,0xd
	jz	THUMB_post_process
	shl	eax,16
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process
    .banked:
	test	[cpu_capability_flags],CPU_CAPABILITY_VE
	jz	ERROR_requires_cpu_capability_arm_ve
	movzx	eax,[operand_register1]			;rn
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.second
	cmp	eax,0xd
	jz	ERROR_r13_not_valid.second
	movzx	ebp,[operand_register0]
	mov	ecx,ebp
	mov	edx,ebp
	and	ebp,0x0f				;get m1 field
	and	ecx,0x10				;get m field
	and	edx,0x20				;get R field
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_banked
	shl	ebp,16
	shl	ecx,8-4
	shl	edx,22-5
	or	ebp,ecx
	or	ebp,edx
	or	ebp,eax
	or	ebp,0x0120f200
	movzx	eax,[instruction_condition]
	and	eax,0xf0
	shl	eax,28-4
	or	ebp,eax
	jmp	ARM_store_instruction
    .THUMB_banked:
	shl	ebp,8
	shl	ecx,4-4
	shl	edx,20-5
	shl	eax,16
	or	ebp,ecx
	or	ebp,edx
	or	ebp,eax
	or	ebp,0xf3808020
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_rdlo_rdhi_rm_rs:
	;used by UMLAL, UMULL, SMLAL, SMULL, UMAAL
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg,TMPL_base_reg>	;0=rdlo,rdhi,rm,rs
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_rdlo_rdhi_rn_rm.7m
	mov	edx,[arm_instruction]
	and	edx,0xf shl 21 + 0xf shl 4
	cmp	edx,2 shl 21 + 9 shl 4			;UMAAL?
	jnz	.check_M
    ;check v6
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jz	ERROR_requires_cpu_capability_arm_v6
	jmp	.encode
    .check_M:
	test	[cpu_capability_flags],CPU_CAPABILITY_M
	jz	ERROR_requires_cpu_capability_arm_m
    .encode:
	movzx	eax,[operand_register0]			;rdlo
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.first
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;rdhi
	shl	eax,16
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.second
	or	ebp,eax
	movzx	eax,[operand_register2]			;rm
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.third
	or	ebp,eax
	movzx	eax,[operand_register3]			;rs
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.fourth
	shl	eax,8
	or	ebp,eax
	mov	ax,word[operand_registers+0]		;rdlo & rdhi
	cmp	al,ah
	jz	ERROR_destination_registers_must_differ
    ;version 6 and above relax the rd!=rm restriction
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jnz	ARM_store_instruction
	mov	cl,[operand_register2]			;rm
	cmp	cl,al
	jz	ERROR_source_rm_and_dest_must_differ
	cmp	cl,ah
	jz	ERROR_source_rm_and_dest_must_differ
	jmp	ARM_store_instruction
    .THUMB_rdlo_rdhi_rn_rm.7m:
	test	[thumb32_instruction],0x60		;UMAAL is T2
	jnz	.THUMB_rdlo_rdhi_rn_rm
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	ERROR_requires_cpu_capability_arm_7m
	jmp	.THUMB_rdlo_rdhi_rn_rm.encode
    .THUMB_rdlo_rdhi_rn_rm:
	;used by SMLALD, SMLALDX, SMLSLD, SMLSLDX, SMLALBB, SMLALBT, SMLALTB, SMLALTT
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	ERROR_requires_cpu_capability_arm_t2
    .THUMB_rdlo_rdhi_rn_rm.encode:
	mov	cl,0xff
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	[thumb32_error],ERROR_destination_registers_must_differ
	mov	ax,word[operand_registers+0]		;rdlo & rdhi
	cmp	al,ah
	jz	THUMB_post_process
	mov	ebp,[thumb32_instruction]
	movzx	eax,[operand_register0]			;rdlo
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;rdhi
	shl	eax,8
	or	ebp,eax
	movzx	eax,[operand_register2]			;rn
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register3]			;rm
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

;v4

ARM_rd_address3D:
	test	[code_type],CPU_ACTIVITY_ARM
	jz	THUMB2_rd_address
	;used by STRD, LDRD
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\								; 0=rd,[rn]
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_add_sub_reg>,\					; 1=rd,[rn],+-rm
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_imm>,\						; 2=rd,[rn],imm
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\						; 3=rd,[rn,imm]
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right,TMPL_modifier_exclaim>,\				; 4=rd,[rn,imm]!
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_bracket_right>,\					; 5=rd,[rn,+-rm]
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_bracket_right,TMPL_modifier_exclaim>,\			; 6=rd,[rn,+-rm]!
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address,TMPL_bracket_right>,\								; 7=rd,[imm]	PC relative
	<TMPL_base_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>,\								; 8=rd,[exp]	implicit reg from structure
	<TMPL_base_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right,TMPL_modifier_exclaim>,\					; 9=rd,[exp]!	implicit reg from structure
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\						;10=rd,rd2,[rn]
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_add_sub_reg>,\				;11=rd,rd2,[rn],+-rm
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_imm>,\					;12=rd,rd2,[rn],imm
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\					;13=rd,rd2,[rn,imm]
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right,TMPL_modifier_exclaim>,\		;14=rd,rd2,[rn,imm]!
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_bracket_right>,\				;15=rd,rd2,[rn,+-rm]
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_bracket_right,TMPL_modifier_exclaim>,\	;16=rd,rd2,[rn,+-rm]!
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_address,TMPL_bracket_right>,\						;17=rd,rd2,[imm]	PC relative
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>,\						;18=rd,rd2,[exp]	implicit reg from structure
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right,TMPL_modifier_exclaim>			;19=rd,rd2,[exp]!	implicit reg from structure
	cmp	al,10
	jb	.dest_registers_defined
	mov	ecx,[operand_registers]
	mov	dl,cl
	shr	ecx,8
	dec	cl
	mov	[operand_registers],ecx
	cmp	dl,cl
	jnz	ERROR_source_registers_must_be_consecutive
	sub	al,10
    .dest_registers_defined:
	test	[cpu_capability_flags],CPU_CAPABILITY_P
	jz	ERROR_requires_cpu_capability_arm_p
	mov	cl,[operand_register0]
	test	cl,1					;even register?
	jnz	ERROR_destination_register_must_be_even
	cmp	cl,14
	jz	ERROR_r14_not_valid.first
	jmp	ARM_rd_address3.version_check_okay

ARM_rd_address3:
	test	[code_type],CPU_ACTIVITY_ARM
	jz	THUMB2_rd_address
	;used by LDRH, LDRSH, LDRSB, STRH
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\						;0=rd,[rn]
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_add_sub_reg>,\			;1=rd,[rn],+-rm
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_imm>,\				;2=rd,[rn],imm
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\				;3=rd,[rn,imm]
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right,TMPL_modifier_exclaim>,\		;4=rd,[rn,imm]!
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_bracket_right>,\			;5=rd,[rn,+-rm]
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_bracket_right,TMPL_modifier_exclaim>,\	;6=rd,[rn,+-rm]!
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address,TMPL_bracket_right>,\						;7=rd,[imm]	PC relative
	<TMPL_base_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>,\						;8=rd,[exp]	implicit reg from structure
	<TMPL_base_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right,TMPL_modifier_exclaim>			;9=rd,[exp]!	implicit reg from structure
	test	[cpu_capability_flags],CPU_CAPABILITY_V4
	jz	ERROR_requires_cpu_capability_arm_v4
    .version_check_okay:
	mov	cl,[operand_register0]
	cmp	cl,15
	jz	ERROR_r15_not_valid.first
	cmp	al,0
	jz	.rd_q_rn_p
	cmp	al,1
	jz	.rd_q_rn_p_PMrm
	cmp	al,2
	jz	.rd_q_rn_p_imm
	cmp	al,3
	jz	.rd_q_rn_imm_p
	cmp	al,4
	jz	.rd_q_rn_imm_p!
	cmp	al,5
	jz	.rd_q_rn_PMrm_p
	cmp	al,6
	jz	.rd_q_rn_PMrm_p!
	cmp	al,7
	jz	.rd_q_imm_p
	cmp	al,8
	jz	.rd_q_exp_p
	cmp	al,9
	jz	.rd_q_exp_p!
	ud2
    .rd_q_imm_p:
	mov	[operand_register1],0xf			;rn=r15
	mov	eax,[addressing_space]
	mov	eax,[eax+0x00]
	add	eax,[immediate_value]
	sub	eax,edi
	sub	eax,8
	mov	[immediate_value],eax
    .rd_q_exp_p:
    .rd_q_rn_p:
    .rd_q_rn_imm_p:
	or	ebp,0x2 shl 21
	test	ebp,1 shl 21				;W=1 if T specified
	jnz	.encode_imm
	or	ebp,0x8 shl 21				;P
	jmp	.encode_imm
    .rd_q_rn_p_imm:
	cmp	[immediate_value],0
	jz	.rd_q_rn_p
	or	ebp,0x2 shl 21
	jmp	.check_rn
    .rd_q_exp_p!:
    .rd_q_rn_imm_p!:
	cmp	[immediate_value],0
	jz	.rd_q_rn_p
	or	ebp,0xb shl 21
    .check_rn:
	mov	ax,word[operand_register0]		;ah=rn, al=rd
	cmp	ah,0xf
	jz	ERROR_r15_not_valid.second
	cmp	[operand_size],8			;LDRD or STRD?
	setz	cl
	not	cl
	and	al,cl
	and	ah,cl
	cmp	al,ah
	jz	ERROR_base_and_dest_must_differ_with_writeback
    .encode_imm:
	or	ebp,1 shl 23
	mov	eax,[immediate_value]
	test	eax,eax
	jns	.positive_imm
	and	ebp,not (1 shl 23)
	neg	eax
    .positive_imm:
	cmp	eax,0xff
	ja	.out_of_range
	mov	ecx,eax
	and	ecx,0xf shl 4
	and	eax,0xf
	shl	ecx,4
	or	eax,ecx
	or	ebp,eax
	or	ebp,1 shl 22
    .encode_rd_rn:
	movzx	eax,[operand_register0]			;rd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;rn
	shl	eax,16
	or	ebp,eax
	jmp	ARM_store_instruction
    .rd_q_rn_PMrm_p:
	or	ebp,0x8 shl 21
	jmp	.encode_reg
    .rd_q_rn_PMrm_p!:
	or	ebp,0x9 shl 21
    .rd_q_rn_p_PMrm:
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jnz	.base_offset_okay
	mov	ax,word[operand_register1]
	and	ax,0x7f7f
	cmp	al,ah
	jz	ERROR_base_and_offset_must_differ_with_writeback
    .base_offset_okay:
	mov	al,[operand_register1]			;rn
	cmp	al,0xf
	jz	ERROR_r15_not_valid.second
	cmp	al,[operand_register0]			;rd
	jz	ERROR_base_and_dest_must_differ_with_writeback
	cmp	[operand_size],8			;LDRD or STRD?
	jnz	.encode_reg
	mov	ah,[operand_register0]			;rd
	and	ax,0xfefe
	cmp	al,ah
	jz	ERROR_base_and_dest_must_differ_with_writeback
    .encode_reg:
	cmp	[operand_size],8			;LDRD or STRD?
	jnz	.encode_reg.do
	test	ebp,1 shl 20				;STRD=0
	jz	.encode_reg.do
	mov	ah,[operand_register0]			;rd
	mov	al,[operand_register2]			;rm
	and	ax,0x7e7e
	cmp	al,ah
	jz	ERROR_offset_and_dest_must_differ_with_LDRD
    .encode_reg.do:
	or	ebp,1 shl 23
	movzx	eax,[operand_register2]			;rm
	test	al,al
	jns	.store_reg
	and	eax,0xf
	btr	ebp,23
    .store_reg:
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.third
	or	ebp,eax
	jmp	.encode_rd_rn
    .out_of_range:
	mov	ecx,ERROR_immediate_offset_out_of_range.0xff
	jmp	ARM_store_instruction_with_error

;v4T

THUMB2_check_bad_regs:
	;cl has the bitmap for registers to check
	;cl[3..0] check for reg[3..0] <> pc
	;cl[7..4] check for reg[3..0] <> sp
	mov	edx,[operand_registers]
	test	cl,0x01 shl 0
	jz	.reg0_pc_okay
	cmp	dl,0xf
	jz	.bad_reg
    .reg0_pc_okay:
	test	cl,0x10 shl 0
	jz	.reg0_sp_okay
	cmp	dl,0xd
	jz	.bad_reg
    .reg0_sp_okay:
	shr	cl,1
	test	cl,0x01
	jz	.reg1_pc_okay
	cmp	dh,0xf
	jz	.bad_reg
    .reg1_pc_okay:
	test	cl,0x10
	jz	.reg1_sp_okay
	cmp	dh,0xd
	jz	.bad_reg
    .reg1_sp_okay:
	shr	cl,1
	shr	edx,16
	test	cl,0x01
	jz	.reg2_pc_okay
	cmp	dl,0xf
	jz	.bad_reg
    .reg2_pc_okay:
	test	cl,0x10
	jz	.reg2_sp_okay
	cmp	dl,0xd
	jz	.bad_reg
    .reg2_sp_okay:
	shr	cl,1
	test	cl,0x01
	jz	.reg3_pc_okay
	cmp	dh,0xf
	jz	.bad_reg
    .reg3_pc_okay:
	test	cl,0x10
	jz	.reg3_sp_okay
	cmp	dh,0xd
	jz	.bad_reg
    .reg3_sp_okay:
	clc
	ret
    .bad_reg:
	and	cl,0x11
	cmp	cl,0x10
	mov	[thumb32_error],ERROR_r15_not_valid
	jb	.fail
	mov	[thumb32_error],ERROR_r13_not_valid
	jz	.fail
	mov	[thumb32_error],ERROR_r13_r15_not_valid
    .fail:
	stc
	ret

THUMB_rd_rn_shifter:
	mov	edx,[arm_instruction]
	and	edx,0xf shl 21
	shr	edx,21
	mov	ah,dl
	cmp	al,8					;reg,reg,reg?
	jnz	.register_swap_okay
	cmp	ah,ARM_INSTRUCTION_OPCODE_AND
	jz	.check_register_swap
	cmp	ah,ARM_INSTRUCTION_OPCODE_EOR
	jz	.check_register_swap
	cmp	ah,ARM_INSTRUCTION_OPCODE_ADD
	jz	.check_register_swap
	cmp	ah,ARM_INSTRUCTION_OPCODE_ADC
	jz	.check_register_swap
	cmp	ah,ARM_INSTRUCTION_OPCODE_ORR
	jnz	.register_swap_okay
    .check_register_swap:
	mov	cx,word[operand_registers+0]
	mov	dl,[operand_register2]
	cmp	cl,dl
	jnz	.register_swap_okay
	mov	[operand_register1],dl
	mov	[operand_register2],ch
    .register_swap_okay:
	cmp	al,10					;reg,reg,reg,shift imm?
	jnz	.shift_check_done
	mov	edx,[immediate_value]
	cmp	edx,0
	jnz	.shift_check_done
	mov	[instruction_shift_op],ARM_SHIFT_OPCODE_LSL
	mov	al,8					;change to reg_reg_reg
    .shift_check_done:
	cmp	ah,ARM_INSTRUCTION_OPCODE_AND
	jz	THUMB_AND_rd_rn_shifter
	cmp	ah,ARM_INSTRUCTION_OPCODE_EOR
	jz	THUMB_EOR_rd_rn_shifter
	cmp	ah,ARM_INSTRUCTION_OPCODE_SUB
	jz	THUMB_SUB_rd_rn_shifter
	cmp	ah,ARM_INSTRUCTION_OPCODE_RSB
	jz	THUMB_RSB_rd_rn_shifter
	cmp	ah,ARM_INSTRUCTION_OPCODE_ADD
	jz	THUMB_ADD_rd_rn_shifter
	cmp	ah,ARM_INSTRUCTION_OPCODE_ADC
	jz	THUMB_ADC_rd_rn_shifter
	cmp	ah,ARM_INSTRUCTION_OPCODE_SBC
	jz	THUMB_SBC_rd_rn_shifter
	cmp	ah,ARM_INSTRUCTION_OPCODE_RSC
	jz	THUMB_RSC_rd_rn_shifter
	cmp	ah,ARM_INSTRUCTION_OPCODE_TST
	jz	THUMB_TST_rd_rn_shifter
	cmp	ah,ARM_INSTRUCTION_OPCODE_TEQ
	jz	THUMB_TEQ_rd_rn_shifter
	cmp	ah,ARM_INSTRUCTION_OPCODE_CMP
	jz	THUMB_CMP_rd_rn_shifter
	cmp	ah,ARM_INSTRUCTION_OPCODE_CMN
	jz	THUMB_CMN_rd_rn_shifter
	cmp	ah,ARM_INSTRUCTION_OPCODE_ORR
	jz	THUMB_ORR_rd_rn_shifter
	cmp	ah,ARM_INSTRUCTION_OPCODE_MOV
	jz	THUMB_MOV_rd_rn_shifter
	cmp	ah,ARM_INSTRUCTION_OPCODE_BIC
	jz	THUMB_BIC_rd_rn_shifter
	cmp	ah,ARM_INSTRUCTION_OPCODE_MVN
	jz	THUMB_MVN_rd_rn_shifter
	ud2

THUMB_AND_rd_rn_shifter:
	mov	dx,THUMB_INSTRUCTION_OPCODE_AND
	jmp	THUMB_GEN_rd_rn_shifter

THUMB_EOR_rd_rn_shifter:
	mov	dx,THUMB_INSTRUCTION_OPCODE_EOR
	jmp	THUMB_GEN_rd_rn_shifter

THUMB_ADC_rd_rn_shifter:
	mov	dx,THUMB_INSTRUCTION_OPCODE_ADC
	jmp	THUMB_GEN_rd_rn_shifter

THUMB_SBC_rd_rn_shifter:
	mov	dx,THUMB_INSTRUCTION_OPCODE_SBC
	jmp	THUMB_GEN_rd_rn_shifter

THUMB_TST_rd_rn_shifter:
	mov	edx,[operand_registers]
	mov	dl,dh
	mov	[operand_registers],edx
	mov	dx,THUMB_INSTRUCTION_OPCODE_TST
	jmp	THUMB_GEN_rd_rn_shifter

THUMB_CMN_rd_rn_shifter:
	mov	edx,[operand_registers]
	movzx	ecx,dh
	shl	ecx,16
	or	[thumb32_instruction],ecx
	mov	cx,0x0800
	cmp	dh,13					;SP is okay here
	cmove	edx,ecx
	mov	dl,dh
	mov	word[operand_registers+0],dx
	mov	dx,THUMB_INSTRUCTION_OPCODE_CMN
	jmp	THUMB_GEN_rd_rn_shifter

THUMB_ORR_rd_rn_shifter:
	mov	dx,THUMB_INSTRUCTION_OPCODE_ORR
	jmp	THUMB_GEN_rd_rn_shifter

THUMB_BIC_rd_rn_shifter:
	mov	dx,THUMB_INSTRUCTION_OPCODE_BIC
	jmp	THUMB_GEN_rd_rn_shifter

THUMB_MVN_rd_rn_shifter:
	mov	edx,[operand_registers]
	mov	dh,dl
	mov	[operand_registers],edx
	mov	dx,THUMB_INSTRUCTION_OPCODE_MVN
	jmp	THUMB_GEN_rd_rn_shifter

THUMB_GEN_rd_rn_shifter:
	cmp	al,6
	jz	.reg_reg_imm
	cmp	al,7
	jz	ERROR_byte_rotation_in_thumb
	cmp	al,8
	jz	.reg_reg_reg
	cmp	al,9
	jz	.reg_reg_reg_rrx
	cmp	al,10
	jz	.reg_reg_reg_shift_imm
	cmp	al,11
	jz	.reg_reg_reg_shift_reg
	ud2
    .reg_reg_reg:
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	.reg_reg_reg.32
	mov	[thumb16_error],ERROR_dest_and_source_must_be_the_same
	mov	eax,[operand_registers]
	cmp	ah,al					;rd=rn?
	jnz	.reg_reg_reg.32
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	shr	eax,8
	test	ax,0x0808
	jnz	.reg_reg_reg.32
	shl	ah,3
	or	al,ah
	mov	ah,0x40
	shl	edx,6
	or	eax,edx
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
    .reg_reg_reg.32:
	mov	cl,0x77
    .reg_reg_reg.32.valid_regs_set:
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_7m
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	THUMB_post_process
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	edx,[thumb32_instruction]
	movzx	eax,[operand_register0]
	shl	eax,8
	or	edx,eax
	movzx	eax,[operand_register1]
	shl	eax,16
	or	edx,eax
	movzx	eax,[operand_register2]
	or	edx,eax
	test	[instruction_condition],FLAG_CONDITION_SET
	setnz	cl
	movzx	ecx,cl
	shl	ecx,20
	or	edx,ecx
	or	edx,1 shl 27 + 1 shl 25
	mov	[thumb32_instruction],edx
	mov	[thumb32_error],0
	jmp	THUMB_post_process
    .reg_reg_imm:
	mov	cl,0x33
    .reg_reg_imm.valid_regs_set:
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_7m
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	THUMB_post_process
	push	ecx
	call	THUMB_encode_immediate_with_opcode_swap
	pop	ecx
	mov	[thumb32_error],ERROR_immediate_cannot_be_encoded
	jc	THUMB_post_process
    .reg_reg_imm.encode:
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	edx,[thumb32_instruction]
	movzx	eax,[operand_register0]
	shl	eax,8
	or	edx,eax
	movzx	eax,[operand_register1]
	shl	eax,16
	or	edx,eax
	test	[instruction_condition],FLAG_CONDITION_SET
	setnz	cl
	movzx	ecx,cl
	shl	ecx,20
	or	edx,ecx
	mov	eax,[immediate_value]
	mov	ecx,eax
	and	ecx,7 shl 8
	shl	ecx,12-8
	or	edx,ecx
	mov	ecx,eax
	and	eax,0xff
	and	ecx,1 shl 11
	shl	ecx,26-11
	or	edx,eax
	or	edx,ecx
	or	edx,1 shl 28
	mov	[thumb32_instruction],edx
	mov	[thumb32_error],0
	jmp	THUMB_post_process
    .reg_reg_reg_rrx:
	mov	cl,0x77
    .reg_reg_reg_rrx.valid_regs_set:
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_7m
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	THUMB_post_process
	mov	[instruction_shift_op],ARM_SHIFT_OPCODE_ROR
	jmp	.reg_reg_reg_shift_imm.do
    .reg_reg_reg_shift_imm:
	mov	cl,0x77
    .reg_reg_reg_shift_imm.valid_regs_set:
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_7m
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	THUMB_post_process
	push	ecx
	call	ARM_check_shift_range
	pop	ecx
    .reg_reg_reg_shift_imm.do:
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	edx,[thumb32_instruction]
	movzx	ecx,[instruction_shift_op]
	mov	eax,[immediate_value]
	shl	ecx,4
	or	edx,ecx
	mov	ecx,eax
	and	eax,0x3
	and	ecx,0x7 shl 2
	shl	eax,6
	shl	ecx,12-2
	or	edx,eax
	or	edx,ecx
	movzx	eax,[operand_register0]
	shl	eax,8
	or	edx,eax
	movzx	eax,[operand_register1]
	shl	eax,16
	or	edx,eax
	movzx	eax,[operand_register2]
	or	edx,eax
	test	[instruction_condition],FLAG_CONDITION_SET
	setnz	cl
	movzx	ecx,cl
	shl	ecx,20
	or	edx,ecx
	or	edx,1 shl 27 + 1 shl 25
	mov	[thumb32_instruction],edx
	mov	[thumb32_error],0
	jmp	THUMB_post_process
    .reg_reg_reg_shift_reg:
	jmp	THUMB_post_process

THUMB_CMP_rd_rn_shifter:
	cmp	al,6
	jz	.reg_imm
	cmp	al,7
	jz	ERROR_byte_rotation_in_thumb
	cmp	al,8
	jz	.reg_reg
	cmp	al,9
	jz	.reg_transfer
	cmp	al,10
	jz	.reg_transfer
	cmp	al,11
	jz	THUMB_post_process
	ud2
    .reg_imm:
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	.reg_reg_imm.32
	mov	eax,[operand_registers]
	mov	[thumb16_error],ERROR_immediate_offset_out_of_range.0_0xff
	mov	edx,[immediate_value]
	cmp	edx,0xff
	ja	.reg_reg_imm.32
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	cmp	ah,7
	ja	.reg_reg_imm.32
	mov	al,dl
	or	ah,0x28
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
    .reg_reg_imm.32:
	mov	al,6
    .reg_transfer:
	mov	edx,[operand_registers]
	mov	dl,dh
	mov	word[operand_registers+0],dx
	mov	cl,0x02
	cmp	al,6
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_imm.valid_regs_set
	cmp	al,8
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_reg.32.valid_regs_set
	cmp	al,9
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_reg_rrx.valid_regs_set
	cmp	al,10
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_reg_shift_imm.valid_regs_set
	ud2
    .reg_reg:
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	.reg_transfer
	mov	edx,[operand_registers]
	shr	edx,8
	test	dx,0x0808
	jnz	.hreg1_hreg2
	mov	dx,THUMB_INSTRUCTION_OPCODE_CMP
	mov	eax,[operand_registers]
	shr	eax,8
	shl	ah,3
	or	al,ah
	mov	ah,0x40
	shl	edx,6
	or	eax,edx
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	mov	al,6
	jmp	.reg_transfer
    .hreg1_hreg2:
	mov	al,dl
	and	al,0x8
	and	dl,0x7
	shl	al,4
	or	al,dl
	shl	dh,3
	or	al,dh
	mov	ah,0x45
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	mov	al,6
	jmp	.reg_transfer

THUMB_RSB_rd_rn_shifter:
    ;NEG
	cmp	al,6
	jz	.reg_reg_imm
	cmp	al,7
	jz	ERROR_byte_rotation_in_thumb
	cmp	al,8
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_reg.32
	cmp	al,9
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_reg_rrx
	cmp	al,10
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_reg_shift_imm
	cmp	al,11
	jz	THUMB_post_process
	ud2
    .reg_reg_imm:
	cmp	[immediate_value],0
	jnz	THUMB_GEN_rd_rn_shifter.reg_reg_imm
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_imm
	mov	eax,[operand_registers]
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	test	ax,0x0808
	jnz	THUMB_GEN_rd_rn_shifter.reg_reg_imm
	shl	ah,3
	or	al,ah
	mov	ah,0x40
	mov	dx,THUMB_INSTRUCTION_OPCODE_NEG
	shl	edx,6
	or	eax,edx
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	THUMB_GEN_rd_rn_shifter.reg_reg_imm

THUMB_TEQ_rd_rn_shifter:
	cmp	al,6
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_imm
	cmp	al,7
	jz	ERROR_byte_rotation_in_thumb
	cmp	al,8
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_reg.32
	cmp	al,9
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_reg_rrx
	cmp	al,10
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_reg_shift_imm
	cmp	al,11
	jz	THUMB_post_process
	ud2

THUMB_RSC_rd_rn_shifter:
	jmp	THUMB_post_process

THUMB_SUB_rd_rn_shifter:
	cmp	al,6
	jz	.reg_reg_imm
	cmp	al,7
	jz	ERROR_byte_rotation_in_thumb
	cmp	al,8
	jz	.reg_reg_reg
	cmp	al,9
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_reg_rrx
	cmp	al,10
	jz	THUMB_ADD_rd_rn_shifter.reg_reg_reg.32
	cmp	al,11
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_reg_shift_reg
	ud2
    .reg_reg_imm:
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	.reg_reg_imm.32
	mov	eax,[operand_registers]
	cmp	ah,al					;rd=rn?
	jnz	.reg_reg_imm3
	cmp	al,13					;SP?
	jz	.sp_imm7
    ;reg,imm8
	mov	[thumb16_error],ERROR_immediate_offset_out_of_range.0_0xff
	mov	ch,0x38
	mov	edx,[immediate_value]
	test	edx,edx					;negative?
	jns	.reg_imm8_check_range
	neg	edx
	mov	ch,0x30					;change to add reg,imm8
    .reg_imm8_check_range:
	cmp	edx,0xff
	ja	.reg_reg_imm.32
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	cmp	al,7
	ja	.reg_reg_imm.32
	mov	al,dl
	or	ah,ch
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	.reg_reg_imm.32
    .reg_reg_imm3:
	mov	[thumb16_error],ERROR_immediate_value_out_of_range.0_7
	mov	edx,[immediate_value]
	mov	ch,0x1e
	test	edx,edx					;negative?
	jns	.reg_reg_imm3_check_range
	neg	edx
	mov	ch,0x1c					;change to add reg,reg,imm3
    .reg_reg_imm3_check_range:
	cmp	edx,7
	ja	.reg_reg_imm.32
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	test	ax,0x0808
	jnz	.reg_reg_imm.32
	shl	ah,3
	or	al,ah
	mov	ah,ch
	shl	edx,6
	or	eax,edx
	movzx	eax,ax
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	.reg_reg_imm.32
    .sp_imm7:
	mov	[thumb16_error],ERROR_immediate_offset_out_of_range.0_0x1fc
	mov	ecx,0xb080
	mov	eax,[immediate_value]
	test	eax,eax					;negative?
	jns	.sp_imm7_check_range
	neg	eax
	mov	cl,0					;change to add sp,imm7
    .sp_imm7_check_range:
	cmp	eax,0x7f*4
	ja	.reg_reg_imm.32
	test	eax,3
	jnz	.reg_reg_imm.32
	shr	eax,2
	or	eax,ecx
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
    .reg_reg_imm.32:
	cmp	word[operand_registers+0],0x0e0f	;sub pc,lr,imm?
	jnz	.reg_reg_imm.not_pc_lr
	test	[instruction_condition],FLAG_CONDITION_SET
	jnz	.subs_pc_lr
    .reg_reg_imm.not_pc_lr:
	mov	ebp,[thumb32_instruction]
	mov	[thumb32_instruction],0xf2a00000	;SUBW
	call	THUMB_check_12bit_immediate
	jnc	THUMB_rd_rn_imm12.encode
	cmp	[operand_register1],15			;PC?
	jz	THUMB_rd_rn_imm12.encode
	mov	[thumb32_instruction],ebp
	cmp	[operand_register1],13
	jnz	THUMB_GEN_rd_rn_shifter.reg_reg_imm
	mov	cl,0x03
	jmp	THUMB_GEN_rd_rn_shifter.reg_reg_imm.valid_regs_set
    .subs_pc_lr:
	or	[thumb_flags_16],THUMB_FLAG_ONLY_LAST_IT
	or	[thumb_flags_32],THUMB_FLAG_ONLY_LAST_IT
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_t2
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	THUMB_post_process
	mov	[thumb32_error],ERROR_immediate_value_out_of_range.0_0xff
	mov	eax,[immediate_value]
	cmp	eax,0xff
	ja	THUMB_post_process
	or	eax,0xf3de8f00
	mov	[thumb32_instruction],eax
	mov	[thumb32_error],0
	jmp	THUMB_post_process
    .reg_reg_reg:
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	THUMB_ADD_rd_rn_shifter.reg_reg_reg.32
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	mov	eax,[operand_registers]
	test	eax,0x080808
	jnz	THUMB_ADD_rd_rn_shifter.reg_reg_reg.32
	mov	edx,eax
	shr	edx,16
	shl	ah,3
	or	al,ah
	mov	ah,0
	mov	dh,0
	shl	edx,6
	or	eax,edx
	or	ah,0x1a
	movzx	eax,ax
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	THUMB_ADD_rd_rn_shifter.reg_reg_reg.32

THUMB_ADD_rd_rn_shifter:
	cmp	al,6
	jz	.reg_reg_imm
	cmp	al,7
	jz	ERROR_byte_rotation_in_thumb
	cmp	al,8
	jz	.reg_reg_reg
	cmp	al,9
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_reg_rrx
	cmp	al,10
	jz	.reg_reg_reg.32
	cmp	al,11
	jz	THUMB_post_process
	ud2
    .reg_reg_imm:
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	.reg_reg_imm.32
	mov	eax,[operand_registers]
	cmp	ax,13*0x100+13				;SP,SP,imm?
	jz	.sp_imm7
	cmp	ah,13					;SP?
	jz	.rd_sp_imm8
	cmp	ah,15					;PC?
	jz	.rd_pc_imm8
	cmp	ah,al					;rd=rn?
	jnz	.reg_reg_imm3
    ;reg,imm8
	mov	[thumb16_error],ERROR_immediate_offset_out_of_range.0_0xff
	mov	ch,0x30
	mov	edx,[immediate_value]
	test	edx,edx					;negative?
	jns	.reg_imm8_check_range
	neg	edx
	mov	ch,0x38					;change to sub reg,imm8
    .reg_imm8_check_range:
	cmp	edx,0xff
	ja	.reg_reg_imm.32
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	cmp	ah,7
	ja	.reg_reg_imm.32
	mov	al,dl
	or	ah,ch
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	.reg_reg_imm.32
    .reg_reg_imm3:
	mov	[thumb16_error],ERROR_immediate_value_out_of_range.0_7
	mov	edx,[immediate_value]
	mov	ch,0x1c
	test	edx,edx					;negative?
	jns	.reg_reg_imm3_check_range
	neg	edx
	mov	ch,0x1e					;change to sub reg,reg,imm3
    .reg_reg_imm3_check_range:
	cmp	edx,7
	ja	.reg_reg_imm.32
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	test	ax,0x0808
	jnz	.reg_reg_imm.32
	shl	ah,3
	or	al,ah
	mov	ah,ch
	shl	edx,6
	or	eax,edx
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	.reg_reg_imm.32
    .sp_imm7:
	mov	[thumb16_error],ERROR_immediate_offset_out_of_range.0_0x1fc
	mov	ecx,0xb000
	mov	eax,[immediate_value]
	test	eax,eax					;negative?
	jns	.sp_imm7_check_range
	neg	eax
	mov	cl,0x80					;change to sub sp,imm7
    .sp_imm7_check_range:
	cmp	eax,0x7f*4
	ja	.reg_reg_imm.32
	test	eax,3
	jnz	.reg_reg_imm.32
	shr	eax,2
	or	eax,ecx
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	.reg_reg_imm.32
    .rd_sp_imm8:
	mov	[thumb16_error],ERROR_immediate_offset_out_of_range.0_0x3fc
	mov	edx,[immediate_value]
	cmp	edx,0xff*4
	ja	.reg_reg_imm.32
	test	edx,3
	jnz	.reg_reg_imm.32
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	cmp	al,7
	ja	.reg_reg_imm.32
	mov	ah,al
	shr	edx,2
	mov	al,dl
	or	ah,0xa8
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	.reg_reg_imm.32
    .rd_pc_imm8:
	mov	[thumb16_error],ERROR_immediate_offset_out_of_range.0_0x3fc
	mov	edx,[immediate_value]
	cmp	edx,0xff*4
	ja	.reg_reg_imm.32
	test	edx,3
	jnz	.reg_reg_imm.32
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	cmp	al,7
	ja	.reg_reg_imm.32
	mov	ah,al
	shr	edx,2
	mov	al,dl
	or	ah,0xa0
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
    .reg_reg_imm.32:
	mov	ebp,[thumb32_instruction]
	mov	[thumb32_instruction],0xf2000000	;ADDW
	call	THUMB_check_12bit_immediate
	jnc	THUMB_rd_rn_imm12.encode
	cmp	[operand_register1],15			;PC?
	jz	THUMB_rd_rn_imm12.encode
	mov	[thumb32_instruction],ebp
	cmp	[operand_register1],13
	jnz	THUMB_GEN_rd_rn_shifter.reg_reg_imm
	mov	cl,0x03
	jmp	THUMB_GEN_rd_rn_shifter.reg_reg_imm.valid_regs_set
    .reg_reg_reg:
	mov	eax,[operand_registers]
	mov	edx,eax
	shr	edx,16
	cmp	ah,al					;rd=rn?
	jne	.reg1_reg2_reg3
	;special case for 'add lreg,lreg'
	;if:
	;1. UAL
	;2. no S
	;3. V6M
	;then use 'add hreg,hreg' version
	test	eax,0x080800
	jnz	.hreg1_hreg1_hreg2
	test	[code_type],CPU_ACTIVITY_THUMB_UAL
	jz	.reg1_reg2_reg3				;encode the lreg version 'adds lreg,lreg,lreg'
	test	[instruction_condition],FLAG_CONDITION_SET
	jnz	.reg1_reg2_reg3				;encode the lreg version 'adds lreg,lreg,lreg'
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_6m
	test	[cpu_capability_flags],CPU_CAPABILITY_6M
	jnz	.hreg1_hreg1_hreg2.do			;encode the hreg version in UAL without S
    .reg1_reg2_reg3:
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	.reg_reg_reg.32
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	test	eax,0x080808
	jnz	.reg_reg_reg.32
	shl	ah,3
	or	al,ah
	mov	ah,0
	mov	dh,0
	shl	edx,6
	or	eax,edx
	or	ah,0x18
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	.reg_reg_reg.32
    .hreg1_hreg1_hreg2:
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	.reg_reg_reg.32
    .hreg1_hreg1_hreg2.do:
	and	ax,0x0807
	shl	ah,4
	or	al,ah
	shl	dl,3
	or	al,dl
	mov	ah,0x44
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
    .reg_reg_reg.32:
	cmp	[operand_register1],13
	jnz	THUMB_GEN_rd_rn_shifter.reg_reg_reg_shift_imm
	cmp	[operand_register0],13
	jnz	.shift_checked
	mov	[thumb32_error],ERROR_shift_type_must_be_lsl
	cmp	[instruction_shift_op],ARM_SHIFT_OPCODE_LSL
	jnz	THUMB_post_process
	mov	[thumb32_error],ERROR_shift_value_out_of_range.0_3
	cmp	[immediate_value],3
	ja	THUMB_post_process
    .shift_checked:
	mov	cl,0x47
	jmp	THUMB_GEN_rd_rn_shifter.reg_reg_reg_shift_imm.valid_regs_set

THUMB_MOV_rd_rn_shifter:
	cmp	al,6
	jz	.reg_imm
	cmp	al,7
	jz	ERROR_byte_rotation_in_thumb
	cmp	al,8
	jz	.reg_reg
	cmp	al,9
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_reg_rrx
	cmp	al,10
	jz	.reg_reg_shift_imm
	cmp	al,11
	jz	.reg_reg_shift_reg
	ud2
    .reg_imm:
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_imm
	mov	[thumb16_error],ERROR_immediate_offset_out_of_range.0_0xff
	mov	ah,[operand_register0]
	mov	edx,[immediate_value]
	cmp	edx,0xff
	ja	THUMB_GEN_rd_rn_shifter.reg_reg_imm
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	cmp	ah,7
	ja	THUMB_GEN_rd_rn_shifter.reg_reg_imm
	mov	al,dl
	or	ah,0x20
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	THUMB_GEN_rd_rn_shifter.reg_reg_imm
    .reg_reg:
	;special case for 'mov lreg,lreg'
	;if:
	;1. UAL
	;2. no S
	;3. V6T
	;then use 'cpy reg,reg'
	mov	al,[operand_register0]
	mov	ah,[operand_register2]
	mov	word[operand_registers+0],ax
	test	ax,0x0808				;any high registers used?
	jnz	.hreg1_hreg2
	test	[code_type],CPU_ACTIVITY_THUMB_UAL
	jz	.lreg1_lreg2				;encode the lreg version 'adds reg,reg,0'
	test	[instruction_condition],FLAG_CONDITION_SET
	jnz	.lreg1_lreg2				;encode the lreg version 'adds reg,reg,0'
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v6t
	test	[cpu_capability_flags],CPU_CAPABILITY_V6T
	jnz	.hreg1_hreg2
    .lreg1_lreg2:
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_reg
	shl	ah,3
	or	al,ah
	mov	ah,ch
	or	ax,0x1c00
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	THUMB_GEN_rd_rn_shifter.reg_reg_reg.32
    .hreg1_hreg2:
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_reg
    .hreg1_hreg2.do:
	mov	dl,ah
	mov	ah,al
	and	ax,0x0807
	shl	ah,4
	or	al,ah
	shl	dl,3
	or	al,dl
	mov	ah,0x46
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	cmp	[operand_register0],0xf
	jnz	.T2
	or	[thumb_flags_16],THUMB_FLAG_ONLY_LAST_IT
    .T2:
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_7m
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	THUMB_post_process
	mov	cl,0x55
	call	THUMB2_check_bad_regs
	jnc	.T2.do
	test	[instruction_condition],FLAG_CONDITION_SET
	jnz	THUMB_post_process
	mov	cl,0x11					;mov rx,{sp|pc} is allowed
	call	THUMB2_check_bad_regs
	jnc	.T2.do
	mov	cl,0x45					;mov sp,rx is allowed
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
    .T2.do:
	mov	edx,[thumb32_instruction]
	movzx	eax,[operand_register0]
	shl	eax,8
	or	edx,eax
	movzx	eax,[operand_register2]
	or	edx,eax
	test	[instruction_condition],FLAG_CONDITION_SET
	setnz	cl
	movzx	ecx,cl
	shl	ecx,20
	or	edx,ecx
	or	edx,1 shl 27 + 1 shl 25
	mov	[thumb32_instruction],edx
	mov	[thumb32_error],0
	jmp	THUMB_post_process
    .reg_reg_shift_imm:
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_reg_shift_imm
	cmp	[instruction_shift_op],ARM_SHIFT_OPCODE_ROR
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_reg_shift_imm
	call	ARM_check_shift_range
	movzx	eax,[instruction_shift_op]
	mov	ecx,0 shl 11
	mov	edx,1 shl 11
	cmp	al,ARM_SHIFT_OPCODE_LSR
	cmovz	ecx,edx
	mov	edx,2 shl 11
	cmp	al,ARM_SHIFT_OPCODE_ASR
	cmovz	ecx,edx
	mov	al,[operand_register0]
	mov	ah,[operand_register2]
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	cmp	al,7
	ja	THUMB_GEN_rd_rn_shifter.reg_reg_reg_shift_imm
	cmp	ah,7
	ja	THUMB_GEN_rd_rn_shifter.reg_reg_reg_shift_imm
	shl	ah,3
	or	al,ah
	mov	ah,ch
	mov	edx,[immediate_value]
	and	edx,0x1f
	shl	edx,6
	or	eax,edx
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	THUMB_GEN_rd_rn_shifter.reg_reg_reg_shift_imm
    .reg_reg_shift_reg:
	mov	eax,[operand_registers]
	mov	ecx,eax
	shr	eax,8
	mov	al,cl
	mov	[operand_registers],eax
	movzx	eax,[instruction_shift_op]
	mov	dx,THUMB_INSTRUCTION_OPCODE_LSL
	mov	cx,THUMB_INSTRUCTION_OPCODE_LSR
	cmp	al,ARM_SHIFT_OPCODE_LSR
	cmovz	dx,cx
	mov	cx,THUMB_INSTRUCTION_OPCODE_ASR
	cmp	al,ARM_SHIFT_OPCODE_ASR
	cmovz	dx,cx
	mov	cx,THUMB_INSTRUCTION_OPCODE_ROR
	cmp	al,ARM_SHIFT_OPCODE_ROR
	cmovz	dx,cx
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	.reg_reg_shift_reg.32
	mov	[thumb16_error],ERROR_dest_and_source_must_be_the_same
	mov	eax,[operand_registers]
	cmp	ah,al					;rd=rn?
	jnz	.reg_reg_shift_reg.32
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	shr	eax,8
	cmp	al,7
	ja	.reg_reg_shift_reg.32
	cmp	ah,7
	ja	.reg_reg_shift_reg.32
	shl	ah,3
	or	al,ah
	mov	ah,0x40
	shl	edx,6
	or	eax,edx
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
    .reg_reg_shift_reg.32:
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_7m
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	THUMB_post_process
	mov	cl,0x77
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	edx,0xfa00f000
	movzx	eax,[instruction_shift_op]
	shl	eax,21
	or	edx,eax
	movzx	eax,[operand_register0]
	shl	eax,8
	or	edx,eax
	movzx	eax,[operand_register1]
	shl	eax,16
	or	edx,eax
	movzx	eax,[operand_register2]
	or	edx,eax
	test	[instruction_condition],FLAG_CONDITION_SET
	setnz	cl
	movzx	ecx,cl
	shl	ecx,20
	or	edx,ecx
	mov	[thumb32_instruction],edx
	mov	[thumb32_error],0
	jmp	THUMB_post_process

THUMB_rd_rn_shifter_ORN:
	;used by ORN
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_imm>,\						; 0=reg,imm		alternate for reg,reg,imm
	<TMPL_base_reg,TMPL_imm,TMPL_comma,TMPL_imm2>,\				; 1=reg,byte,rot	alternate for reg,reg,byte,rot
	<TMPL_base_reg,TMPL_base_reg>,\						; 2=reg,reg		alternate for reg,reg,reg
	<TMPL_base_reg,TMPL_base_reg,TMPL_rrx_op>,\				; 3=reg,reg,rrx		alternate for reg,reg,reg,rrx
	<TMPL_base_reg,TMPL_base_reg,TMPL_shift_op,TMPL_imm>,\			; 4=reg,reg,shift imm	alternate for reg,reg,reg,shift imm
	<TMPL_base_reg,TMPL_base_reg,TMPL_shift_op,TMPL_base_reg>,\		; 5=reg,reg,shift reg	alternate for reg,reg,reg,shift reg
	<TMPL_base_reg,TMPL_base_reg,TMPL_imm>,\				; 6=reg,reg,imm
	<TMPL_base_reg,TMPL_base_reg,TMPL_imm,TMPL_comma,TMPL_imm2>,\		; 7=reg,reg,byte,rot
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg>,\				; 8=reg,reg,reg
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg,TMPL_rrx_op>,\		; 9=reg,reg,reg,rrx
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg,TMPL_shift_op,TMPL_imm>,\	;10=reg,reg,reg,shift imm
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg,TMPL_shift_op,TMPL_base_reg>	;11=reg,reg,reg,shift reg
	cmp	al,6
	jae	.encode_instruction
	mov	edx,[operand_registers]
	movzx	ecx,dl
	shl	edx,8
	or	edx,ecx
	mov	[operand_registers],edx
	add	al,6
    .encode_instruction:
	test	[code_type],CPU_ACTIVITY_ARM
	jnz	ERROR_instruction_not_32bit
	cmp	al,6
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_imm
	cmp	al,7
	jz	ERROR_byte_rotation_in_thumb
	cmp	al,8
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_reg.32
	cmp	al,9
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_reg_rrx
	cmp	al,10
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_reg_shift_imm
	cmp	al,11
	jz	THUMB_post_process
	ud2

THUMB_MUL_rd_rm_rs:
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	ARM_rd_rm_rs_rn.THUMB_rd_rm_rs_rn.7m
	mov	al,[operand_register0]
	mov	cl,[operand_register1]
	mov	ah,[operand_register2]
	cmp	al,cl
	jz	.encode_al_ah
	xchg	cl,ah
    .encode_al_ah:
    ;v6t allows rd=rs
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v6t
	test	[cpu_capability_flags],CPU_CAPABILITY_V6T
	jnz	.encode
	cmp	al,ah
	jz	ARM_rd_rm_rs_rn.THUMB_rd_rm_rs_rn.7m
    .encode:
	mov	[thumb16_error],ERROR_dest_and_source_must_be_the_same
	cmp	al,cl
	jnz	ARM_rd_rm_rs_rn.THUMB_rd_rm_rs_rn.7m
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	test	ax,0x0808
	jnz	ARM_rd_rm_rs_rn.THUMB_rd_rm_rs_rn.7m
	shl	ah,3
	or	al,ah
	mov	ah,0x40
	or	eax,THUMB_INSTRUCTION_OPCODE_MUL shl 6
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	ARM_rd_rm_rs_rn.THUMB_rd_rm_rs_rn.7m

;	Branch Length	Condition	matched?	b.n cc	b.n	b.w cc	b.w	T16 is BCC	T32 is BCC
;	<0x7f		al		x		x	Plain	x	Plain	n		n
;	<0x7ff		al		x		x	Plain	x	Plain	n		n
;	<0xfffff	al		x		x	x	x	Plain	x		n
;	<0xffffff	al		x		x	x	x	Plain	x		n
;	<0x7f		mi		n		Plain	x	Plain	x	y		y
;	<0x7ff		mi		n		x	New IT	Plain	x	n		y
;	<0xfffff	mi		n		x	x	Plain	x	x		y
;	<0xffffff	mi		n		x	x	x	New IT	x		n
;	<0x7f		mi		y		x	Extend	x	Extend	n		n
;	<0x7ff		mi		y		x	Extend	x	Extend	n		n
;	<0xfffff	mi		y		x	x	x	Extend	x		n
;	<0xffffff	mi		y		x	x	x	Extend	x		n

THUMB_B_target:
	or	[thumb_flags_16],THUMB_FLAG_ONLY_LAST_IT
	or	[thumb_flags_32],THUMB_FLAG_ONLY_LAST_IT
	mov	ecx,[addressing_space]
	mov	ecx,[ecx+0x00]
	add	ecx,[immediate_value]
	sub	ecx,edi
	sub	ecx,4
	sar	ecx,1
	jc	.not_aligned
	mov	[thumb16_error],ERROR_relative_jump_out_of_range
	mov	[thumb32_error],ERROR_relative_jump_out_of_range
	mov	edx,[arm_instruction]
	and	edx,0xf shl 21
	cmp	edx,8 shl 21
	jz	.linked
	mov	al,4
	irp	v,7,10,19,23 {				;Bcc.n, B.n, Bcc.w, B.w
		cmp	ecx,1 shl v
		setl	dl
		cmp	ecx,-(1 shl v)
		setge	dh
		and	dl,dh
		sub	al,dl
	}						;al=branch distance 0-4
	cmp	al,4					;branch too far?
	jae	THUMB_post_process
	mov	ah,[instruction_condition]
	and	ah,0xf0
	cmp	ah,0xe0
	jae	.unconditional_16
	push	eax ecx
	call	THUMB_query_condition_match
	pop	ecx eax
	jnc	.unconditional_16
	cmp	al,2
	je	.conditional_32
	cmp	al,0
	je	.conditional_16
	dec	ecx					;adjust for IT block inclusion
	cmp	al,3
	je	.unconditional_32
	inc	ecx					;adjust for no IT block inclusion
	cmp	ecx,-(1 shl 10)				;can fit into narrow branch?
	jle	.conditional_32
	dec	ecx					;adjust for IT block inclusion
    ;set unconditional 16
	mov	edx,ecx
	and	edx,0x7ff
	or	dh,0xe0
	mov	[thumb16_instruction],dx
	mov	[thumb16_error],0
	inc	ecx					;adjust for no IT block inclusion
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jnz	.conditional_32
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	jmp	.conditional_32
    .conditional_16:
	or	[thumb_flags_16],THUMB_FLAG_NOT_INSIDE_IT + THUMB_FLAG_IS_BCC
	mov	dl,cl
	mov	dh,ah
	shr	dh,4
	or	dh,0xd0
	mov	[thumb16_instruction],dx
	mov	[thumb16_error],0
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jnz	.conditional_32
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
    .conditional_32:
	or	[thumb_flags_32],THUMB_FLAG_NOT_INSIDE_IT + THUMB_FLAG_IS_BCC
	xor	edx,edx
	mov	dl,ah
	shl	edx,22-4
    ;imm11
	mov	eax,ecx
	and	eax,0x7ff
	or	edx,eax
    ;imm6
	mov	eax,ecx
	and	eax,0x3f shl 11
	shl	eax,16-11
	or	edx,eax
    ;j1
	mov	eax,ecx
	and	eax,1 shl (11+6)
	shr	eax,11+6-13
	or	edx,eax
    ;j2
	mov	eax,ecx
	and	eax,1 shl (11+6+1)
	shr	eax,11+6+1-11
	or	edx,eax
    ;s
	mov	eax,ecx
	and	eax,1 shl (11+6+1+1)
	shl	eax,26-(11+6+1+1)
	or	edx,eax
	or	edx,0xf0008000
	mov	[thumb32_instruction],edx
	mov	[thumb32_error],0
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jnz	THUMB_post_process
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_7m
	jmp	THUMB_post_process
    .unconditional_16:
	cmp	al,1
	ja	.unconditional_32
	mov	edx,ecx
	and	edx,0x7ff
	or	dh,0xe0
	mov	[thumb16_instruction],dx
	mov	[thumb16_error],0
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jnz	.unconditional_32
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
    .unconditional_32:
	mov	edx,0xf0009000
	mov	[thumb32_error],0
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jnz	.encode_b_bl
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_7m
    .encode_b_bl:
    ;imm11
	mov	eax,ecx
	and	eax,0x7ff
	or	edx,eax
    ;imm10
	mov	eax,ecx
	and	eax,0x3ff shl 11
	shl	eax,16-11
	or	edx,eax
    ;j2
	mov	eax,ecx
	and	eax,1 shl (11+10)
	shr	eax,11+10-11
	or	edx,eax
    ;j1
	mov	eax,ecx
	and	eax,1 shl (11+10+1)
	shr	eax,11+10+1-13
	or	edx,eax
    ;s
	mov	eax,ecx
	and	eax,1 shl (11+10+1+1)
	shl	eax,26-(11+10+1+1)
	or	edx,eax
    ;fixup i1 and i2
	sar	ecx,31					;get the sign
	not	ecx
	and	ecx,1 shl 13 + 1 shl 11
	xor	edx,ecx
	mov	[thumb32_instruction],edx
	jmp	THUMB_post_process
    .linked:
	mov	ah,[instruction_condition]
	and	ah,0xf0
	cmp	ah,0xe0
	jae	.unconditional_linked
	push	eax ecx
	call	THUMB_query_condition_match
	pop	ecx eax
	setc	dl
	movzx	edx,dl
	sub	ecx,edx					;adjust for IT block
    .unconditional_linked:
	cmp	ecx,1 shl 23
	jge	THUMB_post_process			;use default error of out of range
	cmp	ecx,-(1 shl 23)
	jl	THUMB_post_process			;use default error of out of range
	mov	[thumb32_error],0
	mov	edx,0xf000d000
	cmp	ecx,1 shl 21
	setl	al
	cmp	ecx,-(1 shl 21)
	setge	ah
	and	al,ah
	jnz	.check_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_6M
	jnz	.encode_b_bl
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_6m
	jmp	.encode_b_bl
    .check_v4t:
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jnz	.encode_b_bl
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_v4t
	jmp	.encode_b_bl
    .not_aligned:
	mov	[thumb16_error],ERROR_relative_jump_not_aligned
	mov	[thumb32_error],ERROR_relative_jump_not_aligned
	jmp	THUMB_post_process

ARM_rm:
	;used by BX
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg>					;0=rm
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	ERROR_requires_cpu_capability_arm_v4t
	test	[code_type],CPU_ACTIVITY_ARM
	jz	THUMB_rm
	movzx	eax,[operand_register0]
	or	ebp,eax
	jmp	ARM_store_instruction

THUMB_rm:
	or	[thumb_flags_16],THUMB_FLAG_ONLY_LAST_IT
	or	[thumb_flags_32],THUMB_FLAG_ONLY_LAST_IT
	mov	al,[operand_register0]
	shl	al,3
	mov	ah,0x47
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	THUMB_post_process

THUMB_rn_address4:
	;used by LDMIA, STMIA
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	.T2
	cmp	[operand_register0],8			;<r8
	jae	.check_v4
	mov	[thumb16_error],ERROR_instruction_not_t2ee
	test	[code_type],CPU_ACTIVITY_THUMBEE
	jnz	.T2
    .check_v4:
	mov	[thumb16_error],ERROR_instruction_not_16bit
	;if LDM and reg is in set then must be rn,{..}
	;if STM then reg in set can only be first register
	test	[operand_register0],8			;high reg?
	jnz	.T2
	test	ebp,1 shl 20				;1=LDM
	jz	.check_list_stm
	movzx	ecx,[operand_register0]
	bt	[reg_list_bitmap],ecx
	jnc	.check_rn!_list
	test	[code_type],CPU_ACTIVITY_THUMB_UAL
	jz	.check_rn!_list
	cmp	al,0					;0=rn,{..}
	jz	.do_16
	jmp	.T2
    .check_rn!_list:
	cmp	al,2					;2=rn!,{..}
	jz	.do_16
	jmp	.T2
    .check_list_stm:
	cmp	al,2					;2=rn!,{..}
	jnz	.T2
	movzx	ecx,[operand_register0]
	bt	[reg_list_bitmap],ecx
	jnc	.do_16
	bsf	edx,[reg_list_bitmap]
	cmp	ecx,edx
	jnz	.T2
    .do_16:
	cmp	[operand_register0],13			;SP
	jz	THUMB_address4
	mov	edx,[arm_instruction]
	and	edx,0xf shl 21
	cmp	edx,4 shl 21				;IA version of LDM & STM?
	mov	[thumb16_error],ERROR_instruction_not_16bit
	jnz	.T2
	mov	cl,[operand_register0]
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	cmp	cl,7
	ja	.T2
	mov	edx,[reg_list_bitmap]
	test	edx,0xff00
	mov	[thumb16_error],ERROR_high_base_registers_not_allowed_in_list
	jnz	.T2
	mov	dh,cl
	or	dh,0xc0
	bt	ebp,20					;1=LDM, 0=STM
	jnc	.store
	or	dh,0x8
    .store:
	mov	[thumb16_instruction],dx
	mov	[thumb16_error],0
	jmp	.T2
    .T2_2:
	mov	al,2
    .T2:
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_7m
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	THUMB_post_process
	;only IA and DB are valid, bit24 <> bit23
	lea	ecx,[ebp*2]
	xor	ecx,ebp
	test	ecx,1 shl 24
	mov	[thumb32_error],ERROR_instruction_not_16bit
	jz	THUMB_post_process
	cmp	al,0
	jz	.rn_list
	cmp	al,1
	jz	.rn_list_t
	cmp	al,2
	jz	.rn!_list
	cmp	al,3
	jz	.rn!_list_t
	ud2
    .rn!_list:
	or	[thumb32_instruction],1 shl 21
	movzx	eax,[operand_register0]
	bt	[reg_list_bitmap],eax
	mov	[thumb32_error],ERROR_destination_register_not_allowed_in_list
	jc	THUMB_post_process
    .rn_list:
	mov	eax,[reg_list_bitmap]
	lea	ecx,[eax-1]
	and	eax,ecx
	jz	.small_set
	cmp	[operand_register0],0xf
	jz	ERROR_r15_not_valid.first
	test	[reg_list_bitmap],1 shl 13
	jnz	ERROR_sp_in_set
	test	ebp,1 shl 20				;1=LDM, 0=STM
	jnz	.T2_LDM_checks
	test	[reg_list_bitmap],1 shl 15
	jnz	ERROR_pc_in_set
	jmp	.encode
    .T2_LDM_checks:
	cmp	[reg_list_bitmap],0xc000
	jae	ERROR_invalid_set_with_lr_pc
	test	[reg_list_bitmap],1 shl 15
	jz	.encode
	or	[thumb_flags_16],THUMB_FLAG_ONLY_LAST_IT
	or	[thumb_flags_32],THUMB_FLAG_ONLY_LAST_IT
    .encode:
	mov	ebp,[thumb32_instruction]
	movzx	eax,[operand_register0]
	shl	eax,16
	or	ebp,eax
	or	ebp,[reg_list_bitmap]
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process
    .rn!_list_t:
    .rn_list_t:
	mov	[thumb32_error],ERROR_instruction_not_16bit
	jmp	THUMB_post_process
    .small_set:
	;convert to ldr/str for a register set of 1
	mov	eax,[reg_list_bitmap]
	mov	ebp,[thumb32_instruction]
	bsf	eax,eax
	mov	ah,[operand_register0]
	mov	word[operand_registers+0],ax
	;IA  = [reg]	 type 0
	;IA! = [reg],4	 type 1
	;DB  = [reg,-4]	 type 2
	;DB! = [reg,-4]! type 3
	mov	ecx,ebp
	and	ebp,1 shl 20
	or	ebp,0xf8400000				;LDR/STR
	mov	[thumb32_instruction],ebp
	and	ecx,1 shl 23 + 1 shl 21
	mov	[immediate_value],0
	mov	al,0
	cmp	ecx,1 shl 23 + 0 shl 21			;IA
	jz	THUMB2_rd_address.do
	mov	[immediate_value],4
	mov	al,1
	cmp	ecx,1 shl 23 + 1 shl 21			;IA!
	jz	THUMB2_rd_address.do
	mov	[immediate_value],-4
	mov	al,2
	cmp	ecx,0 shl 23 + 0 shl 21			;DB
	jz	THUMB2_rd_address.do
	mov	al,3					;DB!
	jmp	THUMB2_rd_address.do

THUMB_address4:
	;used by POP, PUSH
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	THUMB_rn_address4.T2_2
	mov	edx,[arm_instruction]
	and	edx,0xf shl 21
	shr	edx,21
	mov	eax,[reg_list_bitmap]
	bt	ebp,20					;1=POP, 0=PUSH
	jc	.pop
    ;push
	cmp	dl,0x8					;DB version of STM?
	mov	[thumb16_error],ERROR_instruction_not_16bit
	jnz	THUMB_rn_address4.T2_2
	test	ax,not 0x40ff				;LR with STM
	mov	[thumb16_error],ERROR_high_base_registers_not_allowed_in_list.not_lr
	jnz	THUMB_rn_address4.T2_2
	mov	ch,0xb4
	jmp	.encode
    .pop:
	cmp	dl,0x4					;IA version of LDM?
	mov	[thumb16_error],ERROR_instruction_not_16bit
	jnz	THUMB_rn_address4.T2_2
	test	ax,not 0x80ff				;PC with LDM
	mov	[thumb16_error],ERROR_high_base_registers_not_allowed_in_list.not_pc
	jnz	THUMB_rn_address4.T2_2
	mov	ch,0xbc
	test	[reg_list_bitmap],1 shl 15
	jz	.encode
	or	[thumb_flags_16],THUMB_FLAG_ONLY_LAST_IT
	or	[thumb_flags_32],THUMB_FLAG_ONLY_LAST_IT
    .encode:
	test	ah,ah
	setnz	cl
	or	ch,cl
	mov	ah,ch
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	THUMB_rn_address4.T2_2

THUMB_immediate8:
	;used by SWI
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	ERROR_requires_cpu_capability_arm_v4t
	mov	[thumb16_error],ERROR_immediate_offset_out_of_range.0_0xff
	mov	eax,[immediate_value]
	cmp	eax,0xff
	ja	THUMB_post_process
	mov	ah,0xdf
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	THUMB_post_process

;v5

ARM_immediate16:
	;used by BKPT
	call	decode_template
    TEMPLATE \
	<TMPL_imm>					;0=imm
	call	BKPT_force_condition_match
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_immediate8
	test	[cpu_capability_flags],CPU_CAPABILITY_V5
	jz	ERROR_requires_cpu_capability_arm_v5
	mov	eax,[immediate_value]
	cmp	eax,0x0000ffff
	ja	.out_of_range
	mov	ecx,eax
	and	eax,0xf
	and	ecx,0xfff0
	shl	ecx,4
	or	eax,ecx
	or	ebp,eax
	jmp	ARM_store_instruction
    .out_of_range:
	mov	ecx,ERROR_immediate_offset_out_of_range.0_0xffff
	jmp	ARM_store_instruction_with_error
    .THUMB_immediate8:
	test	[cpu_capability_flags],CPU_CAPABILITY_V5T
	jz	ERROR_requires_cpu_capability_arm_v5t
	mov	[thumb16_error],ERROR_immediate_offset_out_of_range.0_0xff
	mov	eax,[immediate_value]
	cmp	eax,0xff
	ja	THUMB_post_process
	mov	ah,0xbe
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	THUMB_post_process

ARM_Xtarget:
	;used by BLX
	call	decode_template
    TEMPLATE \
	<TMPL_address>,\				;0=imm	PC relative
	<TMPL_base_reg>					;1=rm
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_Xtarget
	test	[cpu_capability_flags],CPU_CAPABILITY_V5
	jz	ERROR_requires_cpu_capability_arm_v5
	cmp	al,0
	jz	.address
	cmp	al,1
	jz	.rm
	ud2
    .address:
	mov	eax,[addressing_space]
	mov	eax,[eax+0x00]
	add	eax,[immediate_value]
	sub	eax,edi
	sub	eax,8
	test	eax,1
	jnz	.not_aligned
	cmp	eax,1 shl 25
	jge	.jump_out_of_range
	cmp	eax,-(1 shl 25)
	jl	.jump_out_of_range
	shr	eax,2
	setc	cl
	or	cl,0xfa
	and	eax,0x00ffffff
	or	ebp,eax
	shl	ecx,24
	or	ebp,ecx
	jmp	ARM_store_instruction
    .jump_out_of_range:
	mov	ecx,ERROR_relative_jump_out_of_range
	jmp	ARM_store_instruction_with_error
    .not_aligned:
	mov	ecx,ERROR_relative_jump_not_aligned
	jmp	ARM_store_instruction_with_error
    .rm:
	movzx	eax,[operand_register0]
	cmp	eax,15					;PC?
	je	ERROR_r15_not_valid
	or	ebp,eax
	or	ebp,0x012fff30
	jmp	ARM_store_instruction
    .THUMB_Xtarget:
	cmp	al,1
	jz	.rm32
	mov	[thumb32_error],ERROR_instruction_not_t2ee
	test	[code_type],CPU_ACTIVITY_THUMBEE
	jnz	THUMB_post_process
	or	[thumb_flags_16],THUMB_FLAG_ONLY_LAST_IT
	or	[thumb_flags_32],THUMB_FLAG_ONLY_LAST_IT
	call	THUMB_query_condition_pc
	mov	eax,[addressing_space]
	sub	ecx,[eax+0x00]
	btr	ecx,1
	neg	ecx
	add	ecx,[immediate_value]
	sub	ecx,4
	test	ecx,3
	jnz	.not_aligned32
	sar	ecx,1
	mov	[thumb32_error],ERROR_relative_jump_out_of_range
	cmp	ecx,1 shl 23
	jge	THUMB_post_process
	cmp	ecx,-(1 shl 23)
	jl	THUMB_post_process
	mov	[thumb32_error],0
	mov	edx,0xf000c000
	cmp	ecx,1 shl 21
	setl	al
	cmp	ecx,-(1 shl 21)
	setge	ah
	and	al,ah
	jnz	.check_x
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jnz	THUMB_B_target.encode_b_bl
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_t2
	jmp	THUMB_B_target.encode_b_bl
    .check_x:
	test	[cpu_capability_flags],CPU_CAPABILITY_X
	jnz	THUMB_B_target.encode_b_bl
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_x
	jmp	THUMB_B_target.encode_b_bl
    .rm32:
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v5t
	test	[cpu_capability_flags],CPU_CAPABILITY_V5T
	jz	THUMB_post_process
	movzx	eax,[operand_register0]
	cmp	eax,15					;PC?
	je	ERROR_r15_not_valid
	shl	eax,3
	or	eax,0x4780
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	THUMB_post_process
    .not_aligned32:
	mov	[thumb32_error],ERROR_relative_jump_not_aligned
	jmp	THUMB_post_process

ARM_nop:
	;used by NOP
	call	decode_template
    TEMPLATE \
	<TMPL_EOL>					;0=empty
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_nop
	test	[cpu_capability_flags],CPU_CAPABILITY_K
	jnz	ARM_store_instruction
	test	[cpu_capability_flags],CPU_CAPABILITY_V1
	jz	ERROR_requires_cpu_capability_arm_v1
	movzx	ebp,[instruction_condition]
	and	ebp,0xf0
	shl	ebp,24
	or	ebp,0x01a00000				;movcc r0,r0
	jmp	ARM_store_instruction
    .THUMB_nop:
	mov	[thumb16_instruction],0xbf00		;nop.n
	mov	[thumb16_error],0
	test	[cpu_capability_flags],CPU_CAPABILITY_6M
	jnz	.32
	mov	[thumb16_instruction],0x46C0		;mov r8,r8
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jnz	.32
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
    .32:
	mov	[thumb32_instruction],0xf3af8000	;nop.w
	mov	[thumb32_error],0
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jnz	THUMB_post_process
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_7m
	jmp	THUMB_post_process

ARM_rd_rm_CLZ:
	;used by CLZ
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg>			;0=rd,rm
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_rd_rm_CLZ
	test	[cpu_capability_flags],CPU_CAPABILITY_V5
	jz	ERROR_requires_cpu_capability_arm_v5
	movzx	eax,[operand_register0]
	cmp	eax,15					;PC?
	je	ERROR_r15_not_valid.first
	movzx	ecx,[operand_register1]
	cmp	ecx,15					;PC?
	je	ERROR_r15_not_valid.second
	shl	eax,12
	or	ebp,eax
	or	ebp,ecx
	jmp	ARM_store_instruction
    .THUMB_rd_rm_CLZ:
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	ERROR_requires_cpu_capability_arm_7m
	mov	cl,0x33
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	edx,[thumb32_instruction]
	movzx	eax,[operand_register0]
	shl	eax,8
	or	edx,eax
	movzx	eax,[operand_register1]
	or	edx,eax
	shl	eax,16
	or	edx,eax
	mov	[thumb32_instruction],edx
	mov	[thumb32_error],0
	jmp	THUMB_post_process

;E

ARM_rd_rm_rn:
	;used by QADD, QSUB, QDADD, QDSUB
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg>,\			;0=rdm,rn	alternate for rd,rm,rn
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg>	;1=rd,rm,rn
	cmp	al,0
	jnz	.registers_defined
	mov	ecx,[operand_registers]
	shl	ecx,8
	mov	cl,ch
	mov	[operand_registers],ecx
    .registers_defined:
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_rd_rm_rn
	test	[cpu_capability_flags],CPU_CAPABILITY_E
	jz	ERROR_requires_cpu_capability_arm_e
	movzx	eax,[operand_register0]			;rd
	cmp	eax,15
	jz	ERROR_r15_not_valid.all
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;rm
	cmp	eax,15
	jz	ERROR_r15_not_valid.all
	or	ebp,eax
	movzx	eax,[operand_register2]			;rn
	cmp	eax,15
	jz	ERROR_r15_not_valid.all
	shl	eax,16
	or	ebp,eax
	jmp	ARM_store_instruction
    .THUMB_rd_rm_rn:
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	ERROR_requires_cpu_capability_arm_t2
	mov	cl,0x77
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	ebp,[thumb32_instruction]
	movzx	eax,[operand_register0]			;rd
	shl	eax,8
	or	ebp,eax
	movzx	eax,[operand_register1]			;rm
	or	ebp,eax
	movzx	eax,[operand_register2]			;rn
	shl	eax,16
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_rd_rm_rs_rn_E:
	;used by SMLABB, SMLABT, SMLATB, SMLATT, SMLAWB, SMLAWT
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg,TMPL_base_reg>	;0=rd,rm,rs,rn
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_rd_rm_rn_ra
	test	[cpu_capability_flags],CPU_CAPABILITY_E
	jz	ERROR_requires_cpu_capability_arm_e
	movzx	eax,[operand_register0]			;rd
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.first
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register1]			;rm
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.second
	or	ebp,eax
	movzx	eax,[operand_register2]			;rs
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.third
	shl	eax,8
	or	ebp,eax
	movzx	eax,[operand_register3]			;rn
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.fourth
	shl	eax,12
	or	ebp,eax
	jmp	ARM_store_instruction
    .THUMB_rd_rm_rn_ra:
	;used by SMULBB, SMULBT, SMULTB, SMULTT, SMULWB, SMULWT
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	ERROR_requires_cpu_capability_arm_t2
	mov	cl,0xff
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	ebp,[thumb32_instruction]
	movzx	eax,[operand_register0]			;rd
	shl	eax,8
	or	ebp,eax
	movzx	eax,[operand_register1]			;rm
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]			;rn
	or	ebp,eax
	movzx	eax,[operand_register3]			;ra
	shl	eax,12
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_rdlo_rdhi_rm_rs_E:
	;used by SMLALBB, SMLALBT, SMLALTB, SMLALTT
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg,TMPL_base_reg>	;0=rdlo,rdhi,rm,rs
	test	[code_type],CPU_ACTIVITY_ARM
	jz	ARM_rdlo_rdhi_rm_rs.THUMB_rdlo_rdhi_rn_rm
	test	[cpu_capability_flags],CPU_CAPABILITY_E
	jz	ERROR_requires_cpu_capability_arm_e
	movzx	eax,[operand_register0]			;rdlo
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.first
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;rdhi
	shl	eax,16
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.second
	or	ebp,eax
	movzx	eax,[operand_register2]			;rm
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.third
	or	ebp,eax
	movzx	eax,[operand_register3]			;rs
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.fourth
	shl	eax,8
	or	ebp,eax
	mov	al,[operand_register0]			;rdlo
	mov	ah,[operand_register1]			;rdhi
	cmp	al,ah
	jz	ERROR_destination_registers_must_differ
	jmp	ARM_store_instruction

ARM_rd_rm_rs_E:
	;used by SMULBB, SMULBT, SMULTB, SMULTT, SMULWB, SMULWT
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg>,\			;0=rdm,rs
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg>	;1=rd,rm,rs
	cmp	al,0
	jnz	.registers_defined
	mov	ax,word[operand_registers+0]
	mov	word[operand_registers+1],ax
    .registers_defined:
	test	[code_type],CPU_ACTIVITY_ARM
	jz	ARM_rd_rm_rs_rn_E.THUMB_rd_rm_rn_ra
	test	[cpu_capability_flags],CPU_CAPABILITY_E
	jz	ERROR_requires_cpu_capability_arm_e
	movzx	eax,[operand_register0]			;rd
	shl	eax,16
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.first
	or	ebp,eax
	movzx	eax,[operand_register1]			;rm
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.second
	or	ebp,eax
	movzx	eax,[operand_register2]			;rs
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.third
	shl	eax,8
	or	ebp,eax
	jmp	ARM_store_instruction

;P

ARM_copro_op1_rd_rn_crm:
	;used by MCRR, MRRC, MCRR2, MRRC2
	call	decode_template
    TEMPLATE \
	<TMPL_cpro_sel,TMPL_copro_opcode1,TMPL_base_reg,TMPL_base_reg,TMPL_cpro_reg>	;0=copro,op1,rd,rn,crm
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.check_v7m
	cmp	ebp,0xf shl 28				;MCRR2 or MRRC2?
	jae	.check_v6
	jmp	.check_p
    .check_v7m:
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	ERROR_requires_cpu_capability_arm_7m
	jmp	.version_check_okay
    .check_p:
	test	[cpu_capability_flags],CPU_CAPABILITY_P
	jz	ERROR_requires_cpu_capability_arm_p
	jmp	.version_check_okay
    .check_v6:
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jz	ERROR_requires_cpu_capability_arm_v6
    .version_check_okay:
	movzx	eax,[operand_register0]			;cpnum
	shl	eax,8
	or	ebp,eax
	movzx	eax,[copro_opcode1]			;op1
	cmp	eax,15
	ja	.out_of_range
	shl	eax,4
	or	ebp,eax
	movzx	eax,[operand_register1]			;rd
	cmp	eax,15
	jz	ERROR_r15_not_valid.third
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register2]			;rn
	cmp	eax,15
	jz	ERROR_r15_not_valid.fourth
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register3]			;crm
	or	ebp,eax
	test	ebp,1 shl 20				;MRRC?
	jz	ARM_post_process_copro
	movzx	eax,word[operand_register1]		;rd,rn
	cmp	al,ah
	jz	ERROR_destination_registers_must_differ
	jmp	ARM_post_process_copro
    .out_of_range:
	mov	ecx,ERROR_opcode4_out_of_range
	jmp	ARM_post_process_copro_with_error

ARM_address2:
	;used by PLD, PLDW, PLI
	test	[code_type],CPU_ACTIVITY_ARM
	jz	THUMB2_address
	call	decode_template
    TEMPLATE \
	<TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\						;0=[rn]
	<TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\					;1=[rn,imm]
	<TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_bracket_right>,\				;2=[rn,+-rm]
	<TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_shift_op,TMPL_imm,TMPL_bracket_right>,\	;3=[rn,+-rm,shift imm]
	<TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_rrx_op,TMPL_bracket_right>,\			;4=[rn,+-rm,rrx]
	<TMPL_bracket_left,TMPL_address,TMPL_bracket_right>,\							;5=[imm]	PC relative
	<TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>							;6=[exp]	implicit reg from structure
	test	ebp,1 shl 24				;PLI=1?
	jnz	.check_v7
	test	ebp,1 shl 22				;PLDW=0?
	jnz	.check_P
	test	[cpu_capability_flags],CPU_CAPABILITY_MP
	jz	ERROR_requires_cpu_capability_arm_mp
	cmp	[operand_register0],0xf
	jz	ERROR_r15_not_valid.base
	jmp	.version_check_okay
    .check_v7:
	test	[cpu_capability_flags],CPU_CAPABILITY_V7
	jz	ERROR_requires_cpu_capability_arm_v7
	jmp	.version_check_okay
    .check_P:
	test	[cpu_capability_flags],CPU_CAPABILITY_P
	jz	ERROR_requires_cpu_capability_arm_p
    .version_check_okay:
	or	ebp,0xf shl 28
	mov	ecx,[operand_registers]
	shl	ecx,8
	mov	cl,0xf
	mov	[operand_registers],ecx
	cmp	al,0
	jz	ARM_rd_address2.rd_q_rn_p
	cmp	al,1
	jz	ARM_rd_address2.rd_q_rn_imm_p
	cmp	al,2
	jz	ARM_rd_address2.rd_q_rn_PMrm_p
	cmp	al,3
	jz	ARM_rd_address2.rd_q_rn_PMrm_shift_imm_p
	cmp	al,4
	jz	ARM_rd_address2.rd_q_rn_PMrm_rrx_p
	cmp	al,5
	jz	ARM_rd_address2.rd_q_imm_p
	cmp	al,6
	jz	ARM_rd_address2.rd_q_exp_p
	ud2

THUMB2_address:
	;used by PLD
	call	decode_template
    TEMPLATE \
	<TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\						;0(0)=[rn]
	<TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\					;1(2)=[rn,imm]
	<TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_bracket_right>,\				;2(4)=[rn,rm]
	<TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_shift_op,TMPL_imm2,TMPL_bracket_right>,\	;3(5)=[rn,rm,shift imm]
	<TMPL_bracket_left,TMPL_address,TMPL_bracket_right>,\							;4(6)=[imm]	PC relative
	<TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>							;5(7)=[exp]	implicit reg from structure
	mov	ecx,[operand_registers]
	shl	ecx,8
	or	ecx,8					;fake rd as r8 to prevent 16bit code generation
	mov	[operand_registers],ecx
	add	al,2
	cmp	al,3
	ja	THUMB2_rd_address.do
	dec	al
	shr	al,1
	add	al,al
	jmp	THUMB2_rd_address.do

;J

ARM_rm_J:
	;used by BXJ
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg>					;0=rm
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_rm_j
	test	[cpu_capability_flags],CPU_CAPABILITY_J
	jz	ERROR_requires_cpu_capability_arm_j
	movzx	eax,[operand_register0]
	or	ebp,eax
	cmp	eax,15					;PC?
	jz	ERROR_r15_not_valid
	jmp	ARM_store_instruction
    .THUMB_rm_j:
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	ERROR_requires_cpu_capability_arm_t2
	mov	ecx,0x11
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	or	[thumb_flags_16],THUMB_FLAG_ONLY_LAST_IT
	or	[thumb_flags_32],THUMB_FLAG_ONLY_LAST_IT
	mov	ebp,[thumb32_instruction]
	movzx	eax,[operand_register0]
	shl	eax,16
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

;v6

ARM_mode:
	;used by CPS
	call	decode_template
    TEMPLATE \
	<TMPL_imm>					;0=imm5
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_mode
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jz	ERROR_requires_cpu_capability_arm_v6
	mov	eax,[immediate_value]
	cmp	eax,31
	ja	.out_of_range
	or	ebp,eax
	jmp	ARM_store_instruction
    .out_of_range:
	mov	ecx,ERROR_immediate_offset_out_of_range.0_0x1f
	jmp	ARM_store_instruction_with_error
    .THUMB_mode:
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	ERROR_requires_cpu_capability_arm_t2
	or	[thumb_flags_16],THUMB_FLAG_NOT_INSIDE_IT
	or	[thumb_flags_32],THUMB_FLAG_NOT_INSIDE_IT
	mov	ebp,[thumb32_instruction]
	mov	[thumb32_error],ERROR_immediate_offset_out_of_range.0_0x1f
	mov	eax,[immediate_value]
	cmp	eax,31
	ja	THUMB_post_process
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_iflags_mode:
	;used by CPSID, CPSIE
	call	decode_template
    TEMPLATE \
	<TMPL_iflags>,\					;0=flags
	<TMPL_iflags,TMPL_imm>				;1=flags,mode
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_iflags_mode
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jz	ERROR_requires_cpu_capability_arm_v6
	cmp	al,1
	jnz	.iflags
    .iflags_mode:
	mov	eax,[immediate_value]
	cmp	eax,31
	ja	.out_of_range
	or	ebp,1 shl 17
	or	ebp,eax
    .iflags:
	movzx	eax,[operand_register0]
	shl	eax,6
	or	ebp,eax
	jmp	ARM_store_instruction
    .out_of_range:
	mov	ecx,ERROR_immediate_offset_out_of_range.0_0x1f
	jmp	ARM_store_instruction_with_error
    .THUMB_iflags_mode:
	or	[thumb_flags_16],THUMB_FLAG_NOT_INSIDE_IT
	or	[thumb_flags_32],THUMB_FLAG_NOT_INSIDE_IT
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_7m
	cmp	al,1
	jz	.T2
	test	[cpu_capability_flags],CPU_CAPABILITY_ALIGN
	jnz	.THUMB_iflags_aif
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_align
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jnz	.THUMB_iflags_if
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_7m
	test	[cpu_capability_flags],CPU_CAPABILITY_6M
	jnz	.THUMB_iflags_i
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_6m
	jmp	.T2
    .THUMB_iflags_i:
	test	[operand_register0],101b
	jnz	.T2
    .THUMB_iflags_if:
	test	[operand_register0],100b
	jnz	.T2
    .THUMB_iflags_aif:
	mov	ebp,0xb660
	mov	ecx,[arm_instruction]
	and	ecx,1 shl 18
	shr	ecx,18-4
	or	ebp,ecx
	movzx	ecx,[operand_register0]
	or	ebp,ecx
	mov	[thumb16_instruction],bp
	mov	[thumb16_error],0
    .T2:
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_t2
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	THUMB_post_process
	mov	ebp,[thumb32_instruction]
	cmp	al,1
	jnz	.T2_iflags
    .T2_iflags_mode:
	mov	[thumb32_error],ERROR_immediate_offset_out_of_range.0_0x1f
	mov	eax,[immediate_value]
	cmp	eax,31
	ja	THUMB_post_process
	or	ebp,1 shl 8
	or	ebp,eax
    .T2_iflags:
	movzx	eax,[operand_register0]
	shl	eax,5
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_rd_rm_CPY:
	;used by CPY
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg>			;0=reg,reg
	test	[code_type],CPU_ACTIVITY_ARM
	mov	al,2
	jnz	ARM_rd_shifter.do
    .THUMB_rd_rm:
	mov	ax,word[operand_registers+0]
	mov	[operand_register2],ah
	test	ax,0x0808				;any high registers used?
	jnz	THUMB_MOV_rd_rn_shifter.hreg1_hreg2
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v6t
	test	[cpu_capability_flags],CPU_CAPABILITY_V6T
	jz	THUMB_GEN_rd_rn_shifter.reg_reg_reg
	jmp	THUMB_MOV_rd_rn_shifter.hreg1_hreg2.do

ARM_rd_q_rn_p:
	;used by LDREX, LDREXB, LDREXH
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\		;0=reg,[reg]
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\;1=reg,[reg,imm] (for T2 ldrex only)
	<TMPL_base_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>		;2=reg,[exp]	 (for T2 ldrex only)
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_rd_q_rn_imm_p
	test	ebp,3 shl 21				;LDREXB or LDREXH?
	jz	.check_v6
    .check_v6k:
	test	[cpu_capability_flags],CPU_CAPABILITY_K
	jz	ERROR_requires_cpu_capability_arm_k
	jmp	.version_okay
    .check_v6:
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jz	ERROR_requires_cpu_capability_arm_v6
    .version_okay:
	cmp	[immediate_value],0
	jnz	ERROR_immediate_cannot_be_encoded
	movzx	eax,[operand_register0]			;rd
	cmp	eax,15
	jz	ERROR_r15_not_valid.first
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;rn
	cmp	eax,15
	jz	ERROR_r15_not_valid.second
	shl	eax,16
	or	ebp,eax
	jmp	ARM_store_instruction
    .THUMB_rd_q_rn_imm_p:
	test	ebp,3 shl 21				;LDREXB or LDREXH (!=0)?
	jz	.check_7m
	test	[cpu_capability_flags],CPU_CAPABILITY_SYNC
	jz	ERROR_requires_cpu_capability_arm_sync
	cmp	[immediate_value],0
	jnz	ERROR_immediate_cannot_be_encoded
	jmp	.THUMB_version_okay
    .check_7m:
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	ERROR_requires_cpu_capability_arm_7m
    .THUMB_version_okay:
	mov	cl,0x13
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	ebp,[thumb32_instruction]
	movzx	eax,[operand_register0]			;rd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;rn
	shl	eax,16
	or	ebp,eax
	mov	eax,[immediate_value]
	mov	[thumb32_error],ERROR_immediate_offset_out_of_range.0_0x3fc
	test	eax,not (0xff shl 2)
	jnz	THUMB_post_process
	shr	eax,2
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_rd_rn_rm_shift_imm:
	;used by PKHBT, PKHTB
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg>,\						;0=reg,reg		alternate for reg,reg,reg
	<TMPL_base_reg,TMPL_base_reg,TMPL_shift_op,TMPL_imm>,\			;1=reg,reg,shift imm	alternate for reg,reg,reg,shift imm
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg>,\				;2=reg,reg,reg
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg,TMPL_shift_op,TMPL_imm>	;3=reg,reg,reg,shift imm
	cmp	al,2
	jae	.do
	mov	cx,word[operand_register0]
	mov	word[operand_register1],cx
    .do:
	test	ebp,1 shl 6
	jz	.lsl
	test	al,1
	jnz	.asr
    ;convert to pkhbt rd,rm,rn
    .convert:
	mov	ax,word[operand_register1]
	xchg	ah,al
	mov	word[operand_register1],ax
	and	ebp,not (1 shl 6)
	and	[thumb32_instruction],not (1 shl 5)
	jmp	.encode
    .asr:
	cmp	[instruction_shift_op],ARM_SHIFT_OPCODE_ASR
	jnz	ERROR_shift_must_be_ASR
	mov	eax,[immediate_value]
	sub	eax,1
	jc	.convert
	cmp	eax,31
	ja	ERROR_shift_value_out_of_range.1_32
	inc	eax
	and	eax,0x1f
	mov	[immediate_value],eax
	jmp	.encode
    .lsl:
	cmp	al,0
	jz	.encode
	cmp	[instruction_shift_op],ARM_SHIFT_OPCODE_LSL
	jnz	ERROR_shift_must_be_LSL
	mov	eax,[immediate_value]
	cmp	eax,31
	ja	ERROR_shift_value_out_of_range.0_31
    .encode:
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_encode
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jz	ERROR_requires_cpu_capability_arm_v6
	mov	eax,[immediate_value]
	shl	eax,7
	or	ebp,eax
	movzx	eax,[operand_register0]			;rd
	cmp	eax,15
	jz	ERROR_r15_not_valid.first
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;rn
	cmp	eax,15
	jz	ERROR_r15_not_valid.second
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]			;rm
	cmp	eax,15
	jz	ERROR_r15_not_valid.third
	or	ebp,eax
	jmp	ARM_store_instruction
    .THUMB_encode:
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	ERROR_requires_cpu_capability_arm_t2
	mov	cl,0x77
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	ebp,[thumb32_instruction]
	mov	eax,[immediate_value]
	mov	ecx,eax
	and	eax,0x3
	and	ecx,0x7 shl 2
	shl	eax,6
	shl	ecx,12-2
	or	ebp,eax
	or	ebp,ecx
	movzx	eax,[operand_register0]			;rd
	shl	eax,8
	or	ebp,eax
	movzx	eax,[operand_register1]			;rn
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]			;rm
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_rd_rn_rm:
	;used by QADD16, QADD8, QADDSUBX, QSUB16, QSUB8, QSUBADDX,
	;	 SADD16, SADD8, SADDSUBX, SSUB16, SSUB8, SSUBADDX,
	;	 SHADD16, SHADD8, SHADDSUBX, SHSUB16, SHSUB8, SHSUBADDX,
	;	 UADD16, UADD8, UADDSUBX, USUB16, USUB8, USUBADDX,
	;	 UHADD16, UHADD8, UHADDSUBX, UHSUB16, UHSUB8, UHSUBADDX,
	;	 UQADD16, UQADD8, UQADDSUBX, UQSUB16, UQSUB8, UQSUBADDX,
	;	 SEL
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg>,\			;0=reg,reg	alternate for reg,reg,reg
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg>	;1=reg,reg,reg
	cmp	al,0
	jnz	.encode
	mov	ax,word[operand_register0]
	mov	word[operand_register1],ax
    .encode:
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_rd_rn_rm
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jz	ERROR_requires_cpu_capability_arm_v6
	movzx	eax,[operand_register0]			;rd
	cmp	eax,15
	jz	ERROR_r15_not_valid.first
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;rn
	cmp	eax,15
	jz	ERROR_r15_not_valid.second
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]			;rm
	cmp	eax,15
	jz	ERROR_r15_not_valid.third
	or	ebp,eax
	jmp	ARM_store_instruction
    .THUMB_rd_rn_rm:
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	ERROR_requires_cpu_capability_arm_t2
	mov	cl,0x77
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	ebp,[thumb32_instruction]
	movzx	eax,[operand_register0]			;rd
	shl	eax,8
	or	ebp,eax
	movzx	eax,[operand_register1]			;rn
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]			;rm
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_rd_rm_REV:
	;used by RBIT, REV, REV16, REVSH
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg>			;0=rd,rm
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_rd_rm
	mov	ecx,ebp
	and	ecx,1 shl 22 + 1 shl 7
	cmp	ecx,1 shl 22 + 0 shl 7			;RBIT?
	jnz	.check_v6
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	ERROR_requires_cpu_capability_arm_t2
	jmp	.ARM_version_okay
    .check_v6:
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jz	ERROR_requires_cpu_capability_arm_v6
    .ARM_version_okay:
	movzx	eax,[operand_register0]			;rd
	cmp	eax,15
	jz	ERROR_r15_not_valid.first
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;rm
	cmp	eax,15
	jz	ERROR_r15_not_valid.second
	or	ebp,eax
	jmp	ARM_store_instruction
    .THUMB_rd_rm:
	mov	ecx,ebp
	and	ecx,1 shl 22 + 1 shl 7
	cmp	ecx,1 shl 22 + 0 shl 7			;RBIT?
	jz	.T2
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v6t
	test	[cpu_capability_flags],CPU_CAPABILITY_V6T
	jz	.T2
	mov	eax,ebp
	and	eax,1 shl 7
	and	ebp,1 shl 22
	shr	eax,7-6
	shr	ebp,22-7
	or	ebp,eax
	movzx	eax,[operand_register0]			;rd
	cmp	eax,7
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	ja	.T2
	or	ebp,eax
	movzx	eax,[operand_register1]			;rm
	cmp	eax,7
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	ja	.T2
	shl	eax,3
	or	ebp,eax
	or	ebp,0xba00
	mov	[thumb16_instruction],bp
	mov	[thumb16_error],0
    .T2:
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_7m
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	THUMB_post_process
	mov	cl,0x33
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	ebp,[thumb32_instruction]
	movzx	eax,[operand_register0]			;rd
	shl	eax,8
	or	ebp,eax
	movzx	eax,[operand_register1]			;rm
	or	ebp,eax
	shl	eax,16
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_rn:
	;used by RFE
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg>,\				;0=rn
	<TMPL_base_reg!>				;1=rn!
	movzx	eax,al
	shl	eax,21
	movzx	ecx,[operand_register0]			;rn
	cmp	ecx,15
	jz	ERROR_r15_not_valid.first
	shl	ecx,16
	or	ecx,eax					;define W bit
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_rn
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jz	ERROR_requires_cpu_capability_arm_v6
	or	ebp,ecx
	jmp	ARM_store_instruction
    .THUMB_rn:
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	ERROR_requires_cpu_capability_arm_t2
	mov	eax,ebp
	shl	eax,1
	xor	eax,ebp
	test	eax,1 shl 24				;bit23 <> bit24?
	jz	ERROR_instruction_not_16bit
	mov	ebp,[thumb32_instruction]
	or	ebp,ecx
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_endian:
	;used by SETEND
	call	decode_template
    TEMPLATE \
	<TMPL_endian>					;0=endian
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_endian
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jz	ERROR_requires_cpu_capability_arm_v6
	movzx	eax,[operand_register0]			;endian
	shl	eax,9
	or	ebp,eax
	jmp	ARM_store_instruction
    .THUMB_endian:
	test	[cpu_capability_flags],CPU_CAPABILITY_ALIGN
	jz	ERROR_requires_cpu_capability_arm_align
	or	[thumb_flags_16],THUMB_FLAG_NOT_INSIDE_IT
	or	[thumb_flags_32],THUMB_FLAG_NOT_INSIDE_IT
	movzx	ebp,[operand_register0]			;endian
	shl	ebp,3
	or	ebp,0xb650
	mov	[thumb16_instruction],bp
	mov	[thumb16_error],0
	jmp	THUMB_post_process

ARM_rd_rm_rs_rn_D:
	;used by SMLAD, SMLADX, SMLSD, SMLSDX, SMMLA, SMMLAR, SMMLS, SMMLSR, USADA8
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg,TMPL_base_reg>	;0=rd,rm,rs,rn
	test	[code_type],CPU_ACTIVITY_ARM
	jz	ARM_rd_rm_rs_rn.THUMB_rd_rm_rs_rn
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jz	ERROR_requires_cpu_capability_arm_v6
	jmp	ARM_rd_rm_rs_rn.encode

ARM_rdlo_rdhi_rm_rs_D:
	;used by SMLALD, SMLALDX, SMLSLD, SMLSLDX
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg,TMPL_base_reg>	;0=rdlo,rdhi,rm,rs
	test	[code_type],CPU_ACTIVITY_ARM
	jz	ARM_rdlo_rdhi_rm_rs.THUMB_rdlo_rdhi_rn_rm
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jz	ERROR_requires_cpu_capability_arm_v6
	jmp	ARM_rdlo_rdhi_rm_rs.encode

ARM_rd_rm_rs_M:
	;used by SMMUL, SMMULR, SMUAD, SMUADX, SMUSD, SMUSDX
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg>,\			;0=rd,rm
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg>	;1=rd,rm,rs
	mov	[operand_register3],0			;rn=0
	cmp	al,0
	jnz	.do
	mov	ax,word[operand_registers+0]
	mov	word[operand_registers+1],ax
    .do:
	test	[code_type],CPU_ACTIVITY_ARM
	jz	ARM_rd_rm_rs_rn.THUMB_rd_rm_rs_rn
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jz	ERROR_requires_cpu_capability_arm_v6
	jmp	ARM_rd_rm_rs_rn.encode

ARM_reg_mode:
	;used by SRS
	call	decode_template
    TEMPLATE \
	<TMPL_imm>,\					;0=mode
	<TMPL_imm,TMPL_modifier_exclaim>,\		;1=mode !
	<TMPL_base_reg,TMPL_imm>,\			;2=sp,mode
	<TMPL_base_reg!,TMPL_imm>			;3=sp!,mode
	cmp	al,2
	jb	.set_mode
	cmp	[operand_register0],13			;SP?
	jnz	ERROR_register_out_of_range.must_be_sp
    .set_mode:
	mov	ecx,eax
	and	ecx,1
	shl	ecx,21
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_reg_mode
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jz	ERROR_requires_cpu_capability_arm_v6
	or	ebp,ecx					;define W bit
	mov	eax,[immediate_value]
	cmp	eax,31
	ja	.out_of_range
	or	ebp,eax
	jmp	ARM_store_instruction
    .out_of_range:
	mov	ecx,ERROR_immediate_offset_out_of_range.0_0x1f
	jmp	ARM_store_instruction_with_error
    .THUMB_reg_mode:
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	ERROR_requires_cpu_capability_arm_t2
	mov	eax,ebp
	shl	eax,1
	xor	eax,ebp
	test	eax,1 shl 24				;bit23 <> bit24?
	jz	ERROR_instruction_not_16bit
	mov	ebp,[thumb32_instruction]
	or	ebp,ecx
	mov	[thumb32_error],ERROR_immediate_offset_out_of_range.0_0x1f
	mov	eax,[immediate_value]
	cmp	eax,31
	ja	THUMB_post_process
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_rd_imm_rm_shift:
	;used by SSAT, USAT
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_imm,TMPL_comma,TMPL_base_reg>,\				;0=reg,imm,reg
	<TMPL_base_reg,TMPL_imm,TMPL_comma,TMPL_base_reg,TMPL_shift_op,TMPL_imm2>	;1=reg,imm,reg,shift imm
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_rd_imm_rm_shift
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jz	ERROR_requires_cpu_capability_arm_v6
	movzx	ecx,[operand_register0]			;rd
	cmp	ecx,15
	jz	ERROR_r15_not_valid.first
	shl	ecx,12
	or	ebp,ecx
	movzx	ecx,[operand_register1]			;rm
	cmp	ecx,15
	jz	ERROR_r15_not_valid.third
	or	ebp,ecx
	mov	ecx,[immediate_value]
	test	ebp,1 shl 22				;USAT=1?
	setz	dl
	movzx	edx,dl
	sub	ecx,edx
	cmp	ecx,31
	ja	.immediate_value_out_of_range
	shl	ecx,16
	or	ebp,ecx
	cmp	al,1					;reg,imm,reg,shift imm?
	jnz	ARM_store_instruction
	mov	dl,[instruction_shift_op]
	cmp	dl,ARM_SHIFT_OPCODE_LSL
	jz	.lsl
	cmp	dl,ARM_SHIFT_OPCODE_ASR
	jnz	ERROR_shift_type_must_be_lsl_or_asr
	mov	ecx,ERROR_shift_value_out_of_range.1_32
	mov	edx,[immediate_value2]
	test	edx,edx
	jz	.lsl
	cmp	edx,32
	ja	ARM_store_instruction_with_error
	and	edx,0x1f
	shl	edx,7
	or	ebp,edx
	or	ebp,1 shl 6
	jmp	ARM_store_instruction
    .lsl:
	mov	ecx,ERROR_shift_value_out_of_range.0_31
	mov	edx,[immediate_value2]
	cmp	edx,31
	ja	ARM_store_instruction_with_error
	shl	edx,7
	or	ebp,edx
	jmp	ARM_store_instruction
    .immediate_value_out_of_range:
	test	ebp,1 shl 22				;USAT=1?
	mov	ecx,ERROR_immediate_value_out_of_range.1_32
	mov	edx,ERROR_immediate_value_out_of_range.0_31
	cmovnz	ecx,edx
	jmp	ARM_store_instruction_with_error
    .THUMB_rd_imm_rm_shift:
	mov	ebp,[thumb32_instruction]
	mov	cl,0x33
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	movzx	ecx,[operand_register0]			;rd
	shl	ecx,8
	or	ebp,ecx
	movzx	ecx,[operand_register1]			;rm
	shl	ecx,16
	or	ebp,ecx
	mov	ecx,ERROR_immediate_value_out_of_range.1_32
	mov	edx,ERROR_immediate_value_out_of_range.0_31
	test	ebp,1 shl 23				;USAT=1?
	cmovnz	ecx,edx
	mov	[thumb32_error],ecx
	mov	ecx,[immediate_value]
	setz	dl
	movzx	edx,dl
	sub	ecx,edx
	cmp	ecx,31
	ja	THUMB_post_process
	or	ebp,ecx
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	cmp	al,1					;reg,imm,reg,shift imm?
	jnz	THUMB_post_process
	mov	dl,[instruction_shift_op]
	cmp	dl,ARM_SHIFT_OPCODE_LSL
	jz	.THUMB_lsl
	cmp	dl,ARM_SHIFT_OPCODE_ASR
	jnz	ERROR_shift_type_must_be_lsl_or_asr
	mov	[thumb32_error],ERROR_shift_value_out_of_range.1_31
	mov	edx,[immediate_value2]
	test	edx,edx
	jz	.THUMB_encode_shift
	cmp	edx,31
	ja	THUMB_post_process
	or	ebp,1 shl 21
    .THUMB_encode_shift:
	mov	eax,edx
	and	eax,3
	and	edx,7 shl 2
	shl	eax,6
	shl	edx,12-2
	or	ebp,eax
	or	ebp,edx
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process
    .THUMB_lsl:
	mov	[thumb32_error],ERROR_shift_value_out_of_range.0_31
	mov	edx,[immediate_value2]
	cmp	edx,31
	ja	THUMB_post_process
	jmp	.THUMB_encode_shift

ARM_rd_imm_rm:
	;used by SSAT16, USAT16
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_imm,TMPL_comma,TMPL_base_reg>	;0=reg,imm,reg
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_rd_imm_rm
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jz	ERROR_requires_cpu_capability_arm_v6
	movzx	ecx,[operand_register0]			;rd
	cmp	ecx,15
	jz	ERROR_r15_not_valid.first
	shl	ecx,12
	or	ebp,ecx
	movzx	ecx,[operand_register1]			;rm
	cmp	ecx,15
	jz	ERROR_r15_not_valid.third
	or	ebp,ecx
	mov	ecx,[immediate_value]
	test	ebp,1 shl 22				;USAT16=1?
	setz	dl
	movzx	edx,dl
	sub	ecx,edx
	cmp	ecx,15
	ja	.immediate_value_out_of_range
	shl	ecx,16
	or	ebp,ecx
	jmp	ARM_store_instruction
    .immediate_value_out_of_range:
	test	ebp,1 shl 22				;USAT16=1?
	mov	ecx,ERROR_immediate_value_out_of_range.1_16
	mov	edx,ERROR_immediate_value_out_of_range.0_15
	cmovnz	ecx,edx
	jmp	ARM_store_instruction_with_error
    .THUMB_rd_imm_rm:
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	ERROR_requires_cpu_capability_arm_t2
	mov	ebp,[thumb32_instruction]
	mov	cl,0x33
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	movzx	ecx,[operand_register0]			;rd
	shl	ecx,8
	or	ebp,ecx
	movzx	ecx,[operand_register1]			;rm
	shl	ecx,16
	or	ebp,ecx
	mov	ecx,ERROR_immediate_value_out_of_range.1_16
	mov	edx,ERROR_immediate_value_out_of_range.0_15
	test	ebp,1 shl 23				;USAT16=1?
	cmovnz	ecx,edx
	mov	[thumb32_error],ecx
	mov	ecx,[immediate_value]
	setz	dl
	movzx	edx,dl
	sub	ecx,edx
	cmp	ecx,15
	ja	THUMB_post_process
	or	ebp,ecx
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_rd_rm_q_rn_p_STREX:
	;used by STREX, STREXB, STREXH
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\		;0=rd,rm,[rn]
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\	;1=rd,rm,[rn,imm]
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>		;2=rd,rm,[exp]
	test	[code_type],CPU_ACTIVITY_ARM
	jz	THUMB_rd_rm_q_rn_p_STREX
	mov	ecx,ERROR_immediate_offset_out_of_range.0
	cmp	[immediate_value],0
	jnz	ARM_store_instruction_with_error
	test	ebp,3 shl 21				;STREXB or STREXH?
	jz	.check_v6
    .check_v6k:
	test	[cpu_capability_flags],CPU_CAPABILITY_K
	jz	ERROR_requires_cpu_capability_arm_k
	jmp	.version_okay
    .check_v6:
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jz	ERROR_requires_cpu_capability_arm_v6
    .version_okay:
	movzx	eax,[operand_register0]			;rd
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.first
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;rm
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.second
	or	ebp,eax
	movzx	eax,[operand_register2]			;rn
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.third
	shl	eax,16
	or	ebp,eax
	mov	al,[operand_register0]			;rd
	mov	ah,[operand_register1]			;rm
	mov	cl,[operand_register2]			;rn
	cmp	al,cl
	jz	ERROR_dest_cannot_be_source_or_memory_address
	cmp	al,ah
	jz	ERROR_dest_cannot_be_source_or_memory_address
	jmp	ARM_store_instruction

THUMB_rd_rm_q_rn_p_STREX:
	mov	cl,0x37
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	[thumb32_error],ERROR_dest_cannot_be_source_or_memory_address
	movzx	eax,[operand_register0]			;rd
	movzx	ecx,[operand_register1]			;rm
	movzx	edx,[operand_register2]			;rn
	cmp	eax,ecx
	jz	THUMB_post_process
	cmp	eax,edx
	jz	THUMB_post_process
	test	ebp,3 shl 21				;STREXB or STREXH?
	mov	ebp,[thumb32_instruction]
	jz	.strex
	;STREXB or STREXH
	or	ebp,eax
	shl	ecx,12
	or	ebp,ecx
	shl	edx,16
	or	ebp,edx
	test	[cpu_capability_flags],CPU_CAPABILITY_SYNC
	jz	ERROR_requires_cpu_capability_arm_sync
	mov	[thumb32_error],ERROR_immediate_offset_out_of_range.0
	cmp	[immediate_value],0
	jnz	THUMB_post_process
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process
    .strex:
	shl	eax,8
	or	ebp,eax
	shl	ecx,12
	or	ebp,ecx
	shl	edx,16
	or	ebp,edx
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	ERROR_requires_cpu_capability_arm_7m
	mov	[thumb32_error],ERROR_immediate_offset_out_of_range.0_0x3fc
	mov	eax,[immediate_value]
	test	eax,not (0xff shl 2)
	jnz	THUMB_post_process
	shr	eax,2
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_rd_rn_rm_rotation:
	;used by SXTAB, SXTAB16, SXTAH, UXTAB, UXTAB16, UXTAH
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg>,\				;0=rd,rn,rm
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg,TMPL_shift_op,TMPL_imm>	;1=rd,rn,rm,ror imm
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_rd_rn_rm_rotation
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jz	ERROR_requires_cpu_capability_arm_v6
	movzx	ecx,[operand_register0]			;rd
	cmp	ecx,0xf
	jz	ERROR_r15_not_valid.first
	shl	ecx,12
	or	ebp,ecx
	movzx	ecx,[operand_register1]			;rn
	cmp	ecx,0xf
	jz	ERROR_r15_not_valid.second
	shl	ecx,16
	or	ebp,ecx
	movzx	ecx,[operand_register2]			;rm
	cmp	ecx,0xf
	jz	ERROR_r15_not_valid.third
	or	ebp,ecx
	cmp	al,1
	jnz	ARM_store_instruction
	mov	cl,[instruction_shift_op]
	cmp	cl,ARM_SHIFT_OPCODE_ROR
	jnz	ERROR_shift_must_be_ROR
	mov	ecx,[immediate_value]
	test	ecx,not (3 shl 3)
	jnz	ERROR_shift_value_out_of_range.0_24
	shl	ecx,10-3
	or	ebp,ecx
	jmp	ARM_store_instruction
    .THUMB_rd_rn_rm_rotation:
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	ERROR_requires_cpu_capability_arm_t2
	mov	cl,0x77
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	ebp,[thumb32_instruction]
	movzx	ecx,[operand_register0]			;rd
	shl	ecx,8
	or	ebp,ecx
	movzx	ecx,[operand_register1]			;rn
	shl	ecx,16
	or	ebp,ecx
	movzx	ecx,[operand_register2]			;rm
	or	ebp,ecx
	cmp	al,1
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jnz	THUMB_post_process
	mov	[thumb32_error],ERROR_shift_must_be_ROR
	mov	cl,[instruction_shift_op]
	cmp	cl,ARM_SHIFT_OPCODE_ROR
	jnz	THUMB_post_process
	mov	[thumb32_error],ERROR_shift_value_out_of_range.0_24
	mov	ecx,[immediate_value]
	test	ecx,not (3 shl 3)
	jnz	THUMB_post_process
	shl	ecx,4-3
	or	ebp,ecx
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_rd_rm_rotation:
	;used by SXTB, SXTB16, SXTH, UXTB, UXTB16, UXTH
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg>,\					;0=rdm
	<TMPL_base_reg,TMPL_shift_op,TMPL_imm>,\		;1=rdm,ror imm
	<TMPL_base_reg,TMPL_base_reg>,\				;2=rd,rm
	<TMPL_base_reg,TMPL_base_reg,TMPL_shift_op,TMPL_imm>	;3=rd,rm,ror imm
	sub	al,2
	jnc	.registers_okay
	mov	cl,[operand_register0]
	mov	[operand_register1],cl
    .registers_okay:
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_rd_rm
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jz	ERROR_requires_cpu_capability_arm_v6
	movzx	ecx,[operand_register0]			;rd
	cmp	ecx,0xf
	jz	ERROR_r15_not_valid.first
	shl	ecx,12
	or	ebp,ecx
	movzx	ecx,[operand_register1]			;rm
	cmp	ecx,0xf
	jz	ERROR_r15_not_valid.second
	or	ebp,ecx
	cmp	al,1
	jnz	ARM_store_instruction
	mov	cl,[instruction_shift_op]
	cmp	cl,ARM_SHIFT_OPCODE_ROR
	jnz	ERROR_shift_must_be_ROR
	mov	ecx,[immediate_value]
	test	ecx,not (3 shl 3)
	jnz	ERROR_shift_value_out_of_range.0_24
	shl	ecx,10-3
	or	ebp,ecx
	jmp	ARM_store_instruction
    .THUMB_rd_rm:
	test	ebp,1 shl 21
	jz	.THUMB_rd_rm_rotation.t2		;SXTB16, UXTB16 not encodable
	cmp	al,0
	jz	.rotation_okay
	cmp	[immediate_value],0
	jnz	.THUMB_rd_rm_rotation
    .rotation_okay:
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v6t
	test	[cpu_capability_flags],CPU_CAPABILITY_V6T
	jz	.THUMB_rd_rm_rotation
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	movzx	edx,word[operand_register0]		;rd,rm
	test	dx,0x0808
	jnz	.THUMB_rd_rm_rotation
	shl	dh,3
	or	dl,dh
	test	ebp,1 shl 20
	setz	cl
	shl	cl,6
	or	dl,cl
	test	ebp,1 shl 22
	setnz	cl
	shl	cl,7
	or	dl,cl
	mov	dh,0xb2
	mov	[thumb16_instruction],dx
	mov	[thumb16_error],0
    .THUMB_rd_rm_rotation.t2:
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_t2
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	THUMB_post_process
	jmp	.THUMB_rd_rm_rotation.do
    .THUMB_rd_rm_rotation:
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_7m
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	THUMB_post_process
    .THUMB_rd_rm_rotation.do:
	mov	cl,0x33
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	ebp,[thumb32_instruction]
	movzx	ecx,[operand_register0]			;rd
	shl	ecx,8
	or	ebp,ecx
	movzx	ecx,[operand_register1]			;rm
	or	ebp,ecx
	cmp	al,1
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jnz	THUMB_post_process
	mov	[thumb32_error],ERROR_shift_must_be_ROR
	mov	cl,[instruction_shift_op]
	cmp	cl,ARM_SHIFT_OPCODE_ROR
	jnz	THUMB_post_process
	mov	[thumb32_error],ERROR_shift_value_out_of_range.0_24
	mov	ecx,[immediate_value]
	test	ecx,not (3 shl 3)
	jnz	THUMB_post_process
	shl	ecx,4-3
	or	ebp,ecx
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_rd_rm_rs_SAD:
	;used by USAD8
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg>	;0=rd,rm,rs
	test	[code_type],CPU_ACTIVITY_ARM
	jz	ARM_rd_rn_rm.THUMB_rd_rn_rm
	test	[cpu_capability_flags],CPU_CAPABILITY_V6
	jz	ERROR_requires_cpu_capability_arm_v6
	movzx	eax,[operand_register0]			;rd
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.first
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register1]			;rm
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.second
	or	ebp,eax
	movzx	eax,[operand_register2]			;rs
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.third
	shl	eax,8
	or	ebp,eax
	jmp	ARM_store_instruction

;K

ARM_clrex:
	;used by CLREX
	call	decode_template
    TEMPLATE \
	<TMPL_EOL>					;0=empty
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_clrex
	test	[cpu_capability_flags],CPU_CAPABILITY_K
	jz	ERROR_requires_cpu_capability_arm_k
	jmp	ARM_store_instruction
    .THUMB_clrex:
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	ERROR_requires_cpu_capability_arm_7m
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_rt_rt2_q_rn_p:
	;used by LDREXD
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\			;0=rt,[rn]
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>	;1=rt,rt2,[rn]
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_rt_rt2_q_rn_p
	mov	ecx,[operand_registers]			;rt,rt2,rn
	cmp	al,1
	jnz	.check_rt
	sub	ch,1
	cmp	ch,cl
	jnz	ERROR_destination_registers_must_be_consecutive
	shr	ecx,8
    .check_rt:
	test	cl,1
	jnz	ERROR_destination_register_must_be_even
	cmp	cl,14
	jz	ERROR_r14_not_valid.first
	mov	[operand_registers],ecx			;rt,rn
	jmp	ARM_rd_q_rn_p.check_v6k
    .THUMB_rt_rt2_q_rn_p:
	test	[cpu_capability_flags],CPU_CAPABILITY_V7
	jz	ERROR_requires_cpu_capability_arm_v7
	cmp	al,1
	jz	.check_bad
	mov	ecx,[operand_registers]			;rt,rn
	shl	ecx,8
	mov	cl,ch
	inc	ch
	mov	[operand_registers],ecx			;rt,rt2,rn
    .check_bad:
	mov	cl,0x37
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	[thumb32_error],ERROR_destination_registers_must_differ
	mov	ecx,[operand_registers]
	cmp	ch,cl
	jz	THUMB_post_process
	mov	ebp,[thumb32_instruction]
	movzx	eax,[operand_register0]			;rt
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;rt2
	shl	eax,8
	or	ebp,eax
	movzx	eax,[operand_register2]			;rn
	shl	eax,16
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_rd_rt_rt2_q_rn_p:
	;used by STREXD
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\			;0=rd,rt,[rn]
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>	;1=rd,rt,rt2,[rn]
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_rd_rt_rt2_q_rn_p
	mov	ecx,[operand_registers]			;rd,rt,rt2,rn
	cmp	al,1
	jnz	.check_rt
	mov	eax,ecx
	shr	eax,8
	sub	ah,1
	cmp	ah,al
	jnz	ERROR_source_registers_must_be_consecutive
	mov	eax,ecx
	shr	ecx,8
	mov	ch,cl
	mov	cl,al
    .check_rt:
	test	ch,1
	jnz	ERROR_source_register_must_be_even
	cmp	ch,14
	jz	ERROR_r14_not_valid.second
	mov	[operand_registers],ecx			;rd,rt,rn
	inc	ch
	cmp	ch,cl
	jz	ERROR_source_and_dest_must_differ
	jmp	ARM_rd_rm_q_rn_p_STREX.check_v6k
    .THUMB_rd_rt_rt2_q_rn_p:
	test	[cpu_capability_flags],CPU_CAPABILITY_V7
	jz	ERROR_requires_cpu_capability_arm_v7
	cmp	al,1
	jz	.check_bad
	mov	ecx,[operand_registers]			;rd,rt,rn
	mov	edx,ecx
	inc	ch
	shl	ecx,8
	mov	cx,dx
	mov	[operand_registers],ecx
    .check_bad:
	mov	cl,0x7f
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	[thumb32_error],ERROR_source_and_dest_must_differ
	mov	cl,[operand_register0]
	cmp	cl,[operand_register1]
	jz	THUMB_post_process
	cmp	cl,[operand_register2]
	jz	THUMB_post_process
	cmp	cl,[operand_register3]
	jz	THUMB_post_process
	mov	ebp,[thumb32_instruction]
	movzx	eax,[operand_register0]			;rd
	or	ebp,eax
	movzx	eax,[operand_register1]			;rt
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register2]			;rt2
	shl	eax,8
	or	ebp,eax
	movzx	eax,[operand_register3]			;rn
	shl	eax,16
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

;Z

ARM_immediate4:
	;used by SMC, SMI
	call	decode_template
    TEMPLATE \
	<TMPL_imm>					;0=imm
	test	[cpu_capability_flags],CPU_CAPABILITY_Z
	jz	ERROR_requires_cpu_capability_arm_z
	mov	eax,[immediate_value]
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_immediate4
	cmp	eax,0xf
	ja	.out_of_range
	or	ebp,eax
	jmp	ARM_store_instruction
    .out_of_range:
	mov	ecx,ERROR_immediate_value_out_of_range.0_15
	jmp	ARM_store_instruction_with_error
    .THUMB_immediate4:
	or	[thumb_flags_16],THUMB_FLAG_ONLY_LAST_IT
	or	[thumb_flags_32],THUMB_FLAG_ONLY_LAST_IT
	mov	[thumb32_error],ERROR_immediate_value_out_of_range.0_15
	cmp	eax,0xf
	ja	THUMB_post_process
	mov	ebp,[thumb32_instruction]
	shl	eax,16
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

;v6T2

ARM_rd_address3T:
	test	[code_type],CPU_ACTIVITY_ARM
	jz	THUMB2_rd_address
	;used by LDRHT, LDRSHT, LDRSBT, STRHT
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\			;0=rd,[rn]
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_add_sub_reg>,\;1=rd,[rn],+-rm
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_imm>		;2=rd,[rn],imm
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	ERROR_requires_cpu_capability_arm_t2
	jmp	ARM_rd_address3.version_check_okay

THUMB_rd_rn_imm12:
	test	[code_type],CPU_ACTIVITY_ARM
	jnz	ARM_rd_rn_shifter
	;used by ADDW, SUBW
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_imm>,\			;0=reg,imm		alternate for reg,reg,imm
	<TMPL_base_reg,TMPL_base_reg,TMPL_imm>		;1=reg,reg,imm
	cmp	al,1
	jz	.encode
	mov	al,[operand_register0]
	mov	[operand_register1],al
    .encode:
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_7m
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	THUMB_post_process
	mov	[thumb32_error],ERROR_instruction_not_16bit
	test	[instruction_condition],FLAG_CONDITION_SET
	jnz	THUMB_post_process
	mov	[thumb32_error],ERROR_immediate_offset_out_of_range.0xfff
	mov	eax,[immediate_value]
	mov	edx,[thumb32_instruction]
	mov	ecx,eax
	mov	ebp,edx
	xor	ebp,5 shl 21
	neg	ecx
	cmovns	eax,ecx
	cmovns	edx,ebp
	mov	[thumb32_instruction],edx
	mov	[immediate_value],eax
	test	eax,not 0xfff
	jnz	THUMB_post_process
	mov	cl,0x31
	cmp	[operand_register1],13
	jnz	.check_bad_regs
	mov	cl,0x03
    .check_bad_regs:
	jmp	THUMB_GEN_rd_rn_shifter.reg_reg_imm.encode

ARM_rd_imm_imm:
	;used by BFC
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_imm,TMPL_comma,TMPL_imm2>	;0=reg,imm,imm
	jmp	ARM_rd_rn_imm_imm.do

ARM_rd_rn_imm_imm:
	;used by BFI
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg,TMPL_imm,TMPL_comma,TMPL_imm2>	;0=reg,reg,imm,imm
    .do:
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_rd_rn_imm_imm
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	ERROR_requires_cpu_capability_arm_t2
	movzx	eax,[operand_register0]
	cmp	eax,15
	jz	ERROR_r15_not_valid
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]
	cmp	eax,15
	jz	ERROR_r15_not_valid
	or	ebp,eax
	mov	eax,[immediate_value]
	cmp	eax,31
	mov	ecx,ERROR_lsb_out_of_range.0_31
	ja	ARM_store_instruction_with_error
	shl	eax,7
	or	ebp,eax
	mov	ecx,ERROR_width_out_of_range
	mov	edx,[immediate_value2]
	shr	eax,7
	add	eax,edx
	jc	ARM_store_instruction_with_error
	cmp	eax,32
	ja	ARM_store_instruction_with_error
	dec	eax
	shl	eax,16
	or	ebp,eax
	jmp	ARM_store_instruction
    .THUMB_rd_rn_imm_imm:
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	ERROR_requires_cpu_capability_arm_7m
	mov	ecx,0x33
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	ebp,[thumb32_instruction]
	movzx	eax,[operand_register0]
	shl	eax,8
	or	ebp,eax
	movzx	eax,[operand_register1]
	shl	eax,16
	or	ebp,eax
	mov	[thumb32_error],ERROR_lsb_out_of_range.0_31
	mov	eax,[immediate_value]
	cmp	eax,31
	ja	THUMB_post_process
	mov	ecx,eax
	mov	edx,eax
	and	ecx,3
	and	edx,0x1c
	shl	ecx,6
	shl	edx,12-2
	or	ebp,ecx
	or	ebp,edx
	mov	[thumb32_error],ERROR_width_out_of_range
	mov	edx,[immediate_value2]
	add	eax,edx
	jc	THUMB_post_process
	cmp	eax,32
	ja	THUMB_post_process
	dec	eax
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

THUMB_reg_address:
	;used by CBNZ, CBZ
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_address>			;0=reg,imm	PC relative
	test	[code_type],CPU_ACTIVITY_ARM
	jnz	ERROR_instruction_not_32bit
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	ERROR_requires_cpu_capability_arm_7m
	or	[thumb_flags_16],THUMB_FLAG_NOT_INSIDE_IT
	mov	ebp,[thumb32_instruction]
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	movzx	eax,[operand_register0]
	cmp	eax,7
	ja	THUMB_post_process
	or	ebp,eax
	mov	[thumb16_error],ERROR_relative_jump_not_aligned
	mov	ecx,edi
	mov	eax,[addressing_space]
	sub	ecx,[eax+0x00]
	neg	ecx
	add	ecx,[immediate_value]
	sub	ecx,4
	test	ecx,1
	jnz	THUMB_post_process
	sar	ecx,1
	mov	[thumb32_error],ERROR_relative_jump_out_of_range
	cmp	ecx,1 shl 6
	jae	THUMB_post_process
	mov	eax,ecx
	and	eax,0x1f
	and	ecx,0x20
	shl	eax,3
	shl	ecx,9-5
	or	ebp,eax
	or	ebp,ecx
	mov	[thumb16_instruction],bp
	mov	[thumb16_error],0
	jmp	THUMB_post_process

THUMB_condition:
	;used by IT
	call	decode_template
    TEMPLATE \
	<TMPL_condition>				;0=condition
	test	[code_type],CPU_ACTIVITY_ARM
	jnz	.version_check_okay
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	ERROR_requires_cpu_capability_arm_7m
    .version_check_okay:
	cmp	[explicit_IT_state],0
	jnz	ERROR_instruction_not_conditional
	or	[thumb_flags_16],THUMB_FLAG_NOT_INSIDE_IT
	or	[thumb_flags_32],THUMB_FLAG_NOT_INSIDE_IT
	mov	ebp,[thumb32_instruction]
	movzx	eax,[operand_register0]
	cmp	eax,0xe					;AL?
	jnz	.AL_okay
	bsf	ecx,ebp
	mov	edx,ebp
	btr	edx,ecx
	and	edx,0xf
	jnz	ERROR_al_has_no_inverse
    .AL_okay:
	shl	eax,4
	mov	edx,eax
	or	edx,1
	or	ebp,eax
	and	eax,0x10
	bsf	ecx,ebp
	cmp	ecx,3
	jz	.store
	mov	ebx,8
    .next_bit:
	shl	edx,8
	mov	dl,[operand_register0]
	test	ebp,ebx
	setnz	ch
	xor	dl,ch
	shl	dl,4
	or	dl,1
	shr	eax,1
	xor	ebp,eax
	shr	ebx,1
	inc	cl
	cmp	cl,3
	jnz	.next_bit
    .store:
	xor	eax,eax
    .swap_order:
	shl	eax,8
	mov	al,dl
	shr	edx,8
	jnz	.swap_order
	or	eax,2
	mov	[explicit_IT_state],eax
	mov	[thumb16_instruction],bp
	mov	[thumb16_error],0
    .finalise:
	test	[code_type],CPU_ACTIVITY_ARM
	jz	THUMB_post_process
	and	eax,not 2				;activate the block
	mov	[explicit_IT_state],eax
	jmp	instruction_assembled

THUMB2_rd_address_rm_translate_table:
	;bits 24..20 translate to 11..9
	;used for rd,[rn,rm] 16bit encoding
	db	00100b	;000	str
	db	00010b	;001	strh
	db	00000b	;010	strb
	db	10001b	;011	ldrsb
	db	00101b	;100	ldr
	db	00011b	;101	ldrh
	db	00001b	;110	ldrb
	db	10011b	;111	ldrsh

THUMB2_rd_address_imm_translate_table:
	;bits 22..20 translate to 15..11
	;used for rd,[rn,imm] 16bit encoding
	db	01110b	;strb	000
	db	01111b	;ldrb	001
	db	10000b	;strh	010
	db	10001b	;ldrh	011
	db	01100b	;str	100
	db	01101b	;ldr	101

THUMB2_rd_address:
	cmp	[operand_size],8			;LDRD or STRD?
	jz	THUMB2_rd_rd2_address
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\						;0=rd,[rn]
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_imm>,\				;1=rd,[rn],imm	POP {reg}
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\				;2=rd,[rn,imm]
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right,TMPL_modifier_exclaim>,\		;3=rd,[rn,imm]!	PUSH {reg}
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_bracket_right>,\			;4=rd,[rn,rm]
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_shift_op,TMPL_imm2,TMPL_bracket_right>,\;5=rd,[rn,rm,shift imm]
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address,TMPL_bracket_right>,\						;6=rd,[imm]	PC relative
	<TMPL_base_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>,\						;7=rd,[exp]	implicit reg from structure
	<TMPL_base_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right,TMPL_modifier_exclaim>			;8=rd,[exp]!	implicit reg from structure
	;	T2 encoding, *???(??) = available in T1, ??? = bits 11..9, ????? = bits 15..11, ^=modified behaviour in T2EE mode
	;
	;*	ldr	11111000u101<pc>ttttiiiiiiiiiiii if t=15 & InITBlock() & !LastInITBlock() then UNPREDICTABLE
	;	ldrb	11111000u001<pc>ttttiiiiiiiiiiii if BadReg(t) then UNPREDICTABLE; PLD if t=15 (6T2)
	;	ldrh	11111000u011<pc>ttttiiiiiiiiiiii if BadReg(t) then UNPREDICTABLE;
	;	ldrsb	11111001u001<pc>ttttiiiiiiiiiiii if BadReg(t) then UNPREDICTABLE; PLI if t=15 (v7)
	;	ldrsh	11111001u011<pc>ttttiiiiiiiiiiii if BadReg(t) then UNPREDICTABLE;
	;
	;*01101	ldr	111110001101nnnnttttiiiiiiiiiiii if t=15 & InITBlock() & !LastInITBlock() then UNPREDICTABLE
	;*01111	ldrb	111110001001nnnnttttiiiiiiiiiiii if BadReg(t) then UNPREDICTABLE; PLD if t=15 (6T2)
	;*10001	ldrh	111110001011nnnnttttiiiiiiiiiiii if BadReg(t) then UNPREDICTABLE; PLDW if t=15 (MP)
	;	ldrsb	111110011001nnnnttttiiiiiiiiiiii if BadReg(t) then UNPREDICTABLE; PLI if t=15 (v7)
	;	ldrsh	111110011011nnnnttttiiiiiiiiiiii if BadReg(t) then UNPREDICTABLE;
	;
	;	ldrt	111110000101nnnntttt1110iiiiiiii if BadReg(t) then UNPREDICTABLE;
	;	ldrbt	111110000001nnnntttt1110iiiiiiii if BadReg(t) then UNPREDICTABLE;
	;	ldrht	111110000011nnnntttt1110iiiiiiii if BadReg(t) then UNPREDICTABLE;
	;	ldrsbt	111110010001nnnntttt1110iiiiiiii if BadReg(t) then UNPREDICTABLE;
	;	ldrsht	111110010011nnnntttt1110iiiiiiii if BadReg(t) then UNPREDICTABLE;
	;
	;	ldr	111110000101nnnntttt1puwiiiiiiii if (wback & n=t) | (t=15 & InITBlock() & !LastInITBlock()) then UNPREDICTABLE; POP {reg}
	;	ldrb	111110000001nnnntttt1puwiiiiiiii if BadReg(t) | (wback & n=t) then UNPREDICTABLE; PLD if t=15 (6T2)
	;	ldrh	111110000011nnnntttt1puwiiiiiiii if BadReg(t) | (wback & n=t) then UNPREDICTABLE; PLDW if t=15 (MP)
	;	ldrsb	111110010001nnnntttt1puwiiiiiiii if BadReg(t) | (wback & n=t) then UNPREDICTABLE; PLI if t=15 (v7)
	;	ldrsh	111110010011nnnntttt1puwiiiiiiii if BadReg(t) | (wback & n=t) then UNPREDICTABLE;
	;
	;*100^	ldr	111110000101nnnntttt000000iimmmm if BadReg(m) then UNPREDICTABLE; if t=15 & InITBlock() & !LastInITBlock() then UNPREDICTABLE
	;*110	ldrb	111110000001nnnntttt000000iimmmm if BadReg(t) | n=15 | BadReg(m) then UNPREDICTABLE; PLD if t=15 (6T2)
	;*101^	ldrh	111110000011nnnntttt000000iimmmm if BadReg(t) | n=15 | BadReg(m) then UNPREDICTABLE; PLDW if t=15 (MP)
	;*011	ldrsb	111110010001nnnntttt000000iimmmm if BadReg(t) | n=15 | BadReg(m) then UNPREDICTABLE; PLI if t=15 (v7)
	;*111^	ldrsh	111110010011nnnntttt000000iimmmm if BadReg(t) | n=15 | BadReg(m) then UNPREDICTABLE;
	;
	;*01100	str	111110001100nnnnttttiiiiiiiiiiii if n=15 then UNDEFINED; if t=15 then UNPREDICTABLE;
	;*01110	strb	111110001000nnnnttttiiiiiiiiiiii if n=15 then UNDEFINED; if BadReg(t) then UNPREDICTABLE;
	;*10000	strh	111110001010nnnnttttiiiiiiiiiiii if n=15 then UNDEFINED; if BadReg(t) then UNPREDICTABLE;
	;
	;	strt	111110000100nnnntttt1110iiiiiiii if n=15 then UNDEFINED; if BadReg(t) then UNPREDICTABLE;
	;	strbt	111110000000nnnntttt1110iiiiiiii if n=15 then UNDEFINED; if BadReg(t) then UNPREDICTABLE;
	;	strht	111110000010nnnntttt1110iiiiiiii if n=15 then UNDEFINED; if BadReg(t) then UNPREDICTABLE;
	;
	;	str	111110000100nnnntttt1puwiiiiiiii if n=15 then UNDEFINED; if t=15 | (wback & n=t) then UNPREDICTABLE; PUSH {reg}
	;	strb	111110000000nnnntttt1puwiiiiiiii if n=15 then UNDEFINED; if BadReg(t) | (wback & n=t) then UNPREDICTABLE;
	;	strh	111110000010nnnntttt1puwiiiiiiii if n=15 then UNDEFINED; if BadReg(t) | (wback & n=t) then UNPREDICTABLE;
	;
	;*000^	str	111110000100nnnntttt000000iimmmm if n=15 then UNDEFINED; if t=15 | BadReg(m) then UNPREDICTABLE;
	;*010	strb	111110000000nnnntttt000000iimmmm if n=15 then UNDEFINED; if BadReg(t) | BadReg(m) then UNPREDICTABLE;
	;*001^	strh	111110000010nnnntttt000000iimmmm if n=15 then UNDEFINED; if BadReg(t) | BadReg(m) then UNPREDICTABLE;
	;
    .do:
	mov	ebp,[thumb32_instruction]
	test	ebp,1111b shl 8				;STRxT/LDRxT?
	jnz	.check_bad_rn
	test	ebp,1 shl 20				;0=STRx, 1=LDRx
	jnz	.check_rd
    .check_bad_rn:
	cmp	[operand_register1],0xf
	jz	ERROR_r15_not_valid.second
	cmp	al,6
	jz	ERROR_r15_not_valid.second
    .check_rd:
	test	ebp,1111b shl 8				;STRxT/LDRxT?
	jnz	.check_bad_rd
	test	ebp,1 shl 22				;word?
	jz	.check_bad_rd
	test	ebp,1 shl 20				;0=STRx, 1=LDRx
	jz	.check_rd_for_pc
	jmp	.check_rm				;word loads to PC are okay
    .check_bad_rd:
	cmp	[operand_register0],0xd
	jz	ERROR_dest_cannot_be_sp_pc
    .check_rd_for_pc:
	cmp	[operand_register0],0xf
	jz	ERROR_dest_cannot_be_sp_pc
    .check_rm:
	test	[operand_register2],0x80		;-rm?
	jnz	ERROR_instruction_not_16bit
	cmp	[operand_register2],0xd
	jz	ERROR_offset_register_cannot_be_sp_pc
	cmp	[operand_register2],0xf
	jz	ERROR_offset_register_cannot_be_sp_pc
	cmp	[operand_register0],0xf
	jnz	.IT_block_okay
	or	[thumb_flags_16],THUMB_FLAG_ONLY_LAST_IT
	or	[thumb_flags_32],THUMB_FLAG_ONLY_LAST_IT
    .IT_block_okay:
	mov	ecx,[operand_registers]
	cmp	al,0
	jz	.rd_q_rn_p
	cmp	al,1
	jz	.rd_q_rn_p_imm
	cmp	al,2
	jz	.rd_q_rn_imm_p
	cmp	al,3
	jz	.rd_q_rn_imm_p!
	cmp	al,4
	jz	.rd_q_rn_rm_p
	cmp	al,5
	jz	.rd_q_rn_rm_shift_imm_p
	cmp	al,6
	jz	.rd_q_imm_p
	cmp	al,7
	jz	.rd_q_exp_p
	cmp	al,8
	jz	.rd_q_exp_p!
	ud2
    .rd_q_rn_p_imm:
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	.rd_q_rn_p_imm.32
	;special case for ldr lreg|pc,[sp],4 to use pop {lreg}
	cmp	[immediate_value],4
	jnz	.rd_q_rn_p_imm.32
	cmp	ch,13
	jnz	.rd_q_rn_p_imm.not_sp
	test	ebp,1 shl 20				;0=STRx, 1=LDRx
	jz	.rd_q_rn_p_imm.32
	cmp	cl,15
	mov	dx,0xbd00
	jz	.rd_q_rn_p_imm.pc
	cmp	cl,7
	ja	.rd_q_rn_p_imm.32
	mov	dx,0xbc01
	shl	dl,cl
    .rd_q_rn_p_imm.pc:
	mov	[thumb16_instruction],dx
	mov	[thumb16_error],0
	jmp	.rd_q_rn_p_imm.32
    .rd_q_rn_p_imm.not_sp:
	;special case for ldr/str lreg1,[lreg2],4 to use ldmia/stmia lreg2,{lreg1}
	test	cx,0x0808
	jnz	.rd_q_rn_p_imm.32
	test	ebp,1 shl 20				;0=STRx, 1=LDRx
	jz	.rd_q_rn_p_imm.ldr_okay
	mov	[thumb16_error],ERROR_base_and_dest_must_differ_with_writeback
	cmp	cl,ch
	jz	.rd_q_rn_p_imm.32
    .rd_q_rn_p_imm.ldr_okay:
	mov	edx,ebp
	shr	edx,20-11
	and	dx,0x0800				;get L bit
	or	dx,0xc001
	shl	dl,cl
	or	dh,ch
	mov	[thumb16_instruction],dx
	mov	[thumb16_error],0
    .rd_q_rn_p_imm.32:
	mov	[thumb32_error],ERROR_base_and_dest_must_differ_with_writeback
	cmp	cl,ch
	jz	THUMB_post_process
	cmp	ch,0xf
	jz	ERROR_register_writeback_not_allowed
	cmp	[immediate_value],0
	jz	.rd_q_rn_imm_p
	mov	[thumb32_error],ERROR_instruction_not_16bit
	test	ebp,1111b shl 8				;STRxT/LDRxT?
	jnz	THUMB_post_process
	or	ebp,1 shl 8
	jmp	.encode_imm8
    .rd_q_exp_p!:
    .rd_q_rn_imm_p!:
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	.rd_q_rn_imm_p!.32
	;special case for str lreg|pc,[sp,-4]! to use push {lreg}
	test	ebp,1 shl 20				;0=STRx, 1=LDRx
	jnz	.rd_q_rn_imm_p!.32
	cmp	ch,13
	jnz	.rd_q_rn_imm_p!.32
	cmp	[immediate_value],-4
	jnz	.rd_q_rn_imm_p!.32
	cmp	cl,14
	mov	dx,0xb500
	jz	.rd_q_rn_imm_p!.lr
	cmp	cl,7
	ja	.rd_q_rn_imm_p!.32
	mov	dx,0xb401
	shl	dl,cl
    .rd_q_rn_imm_p!.lr:
	mov	[thumb16_instruction],dx
	mov	[thumb16_error],0
    .rd_q_rn_imm_p!.32:
	cmp	cl,ch
	jz	ERROR_base_and_dest_must_differ_with_writeback
	cmp	ch,0xf
	jz	ERROR_register_writeback_not_allowed
	mov	[thumb32_error],ERROR_instruction_not_16bit
	test	ebp,1111b shl 8				;STRxT/LDRxT?
	jnz	THUMB_post_process
	or	ebp,1 shl 10 + 1 shl 8
	jmp	.encode_imm8
    .rd_q_imm_p:
	mov	[operand_register1],0xf
	push	ecx
	call	THUMB_query_condition_pc
	mov	eax,ecx
	pop	ecx
	mov	ch,0xf
	mov	edx,[addressing_space]
	sub	eax,[edx+0x00]
	and	eax,not 3				;round down
	neg	eax
	add	eax,[immediate_value]
	sub	eax,4
	mov	[immediate_value],eax
    .rd_q_exp_p:
    .rd_q_rn_imm_p:
    .rd_q_rn_p:
	mov	[thumb16_error],ERROR_immediate_cannot_be_encoded
	test	[immediate_value],-1
	jns	.positive_offset
	test	ebp,1 shl 20				;LDRx=1?
	jz	.rd_q_rn_p.thumb
	test	ebp,1 shl 22				;LDR=1?
	jz	.rd_q_rn_p.thumb
	test	[code_type],CPU_ACTIVITY_THUMBEE
	jz	.rd_q_rn_p.thumb
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_t2ee
	test	[cpu_capability_flags],CPU_CAPABILITY_T2EE
	jz	.rd_q_rn_p.thumb
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	test	ecx,0x0808				;any high registers?
	jnz	.rd_q_rn_p.thumb
	mov	[thumb16_error],ERROR_immediate_offset_out_of_range.m28_0
	mov	edx,[immediate_value]
	neg	edx
	test	edx,not (7 shl 2)
	jnz	.rd_q_rn_p.thumb
	movzx	eax,ch
	shl	eax,3
	or	al,cl
	shl	edx,6-2
	or	eax,edx
	or	ax,0xc800
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
    .rd_q_rn_p.thumb:
	cmp	ch,0xf
	jz	.encode_imm12
	or	ebp,1 shl 10
	jmp	.encode_imm8
    .positive_offset:
	test	ebp,1111b shl 8				;STRxT/LDRxT?
	jnz	.encode_imm8
	or	ebp,1 shl 23
	mov	[thumb16_error],ERROR_instruction_not_16bit
	test	ebp,1 shl 24				;if it is a signed load then no 16bit form is available
	jnz	.encode_imm12
	test	[code_type],CPU_ACTIVITY_THUMBEE
	jz	.positive_offset.v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_T2EE
	jz	.positive_offset.v4t
	cmp	ch,9
	jz	.positive_offset.r9
	cmp	ch,10
	jnz	.positive_offset.v4t
	test	ebp,1 shl 20				;LDRx=1?
	jz	.encode_imm12
	test	ebp,1 shl 22				;LDR=1?
	jz	.encode_imm12
	;ldr rt,[r10,imm]
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	test	cl,0x08					;high register?
	jnz	.encode_imm12
	mov	[thumb16_error],ERROR_immediate_offset_out_of_range.0_0x7c
	mov	edx,[immediate_value]
	test	edx,not (0x1f shl 2)
	jnz	.encode_imm12
	shl	edx,3-2
	movzx	ecx,cl
	lea	edx,[edx+ecx+0xcb00]
	mov	[thumb16_instruction],dx
	mov	[thumb16_error],0
	jmp	.encode_imm12
    .positive_offset.r9:
	test	ebp,1 shl 22				;LDR or STR (=1)?
	jz	.encode_imm12
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	test	cl,0x08					;high register?
	jnz	.encode_imm12
	mov	[thumb16_error],ERROR_immediate_offset_out_of_range.0_0xfc
	mov	edx,[immediate_value]
	test	edx,not (0x3f shl 2)
	jnz	.encode_imm12
	shl	edx,3-2
	movzx	ecx,cl
	lea	edx,[edx+ecx+0xcc00]
	bt	ebp,20
	setnc	cl
	movzx	ecx,cl
	shl	ecx,9
	or	edx,ecx
	mov	[thumb16_instruction],dx
	mov	[thumb16_error],0
	jmp	.encode_imm12
    .positive_offset.v4t:
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	.encode_imm12
	mov	dx,0x9000
	cmp	ch,0xd					;SP?
	jz	.16bit_imm8
	mov	dx,0x4000
	cmp	ch,0xf					;PC?
	jz	.16bit_imm8
	mov	[thumb16_error],ERROR_register_out_of_range
	test	ecx,0x0808				;any high registers?
	jnz	.encode_imm12
	mov	[thumb16_error],ERROR_immediate_cannot_be_encoded
	mov	ecx,ebp
	shr	ecx,20
	and	ecx,6
	setz	dl
	add	cl,dl					;ecx=1, 2 or 4 for B, H, W
	imul	edx,ecx,31
	not	edx
	test	[immediate_value],edx
	jnz	.encode_imm12
	mov	edx,ebp
	shr	edx,20
	and	edx,111b
	movzx	edx,byte[edx+THUMB2_rd_address_imm_translate_table]
	shl	edx,11
	movzx	eax,[operand_register0]
	or	edx,eax
	movzx	eax,[operand_register1]
	shl	eax,3
	or	edx,eax
	mov	eax,[immediate_value]
	shl	eax,6
	shr	ecx,1
	shr	eax,cl
	or	edx,eax
	mov	[thumb16_instruction],dx
	mov	[thumb16_error],0
	jmp	.encode_imm12
    .16bit_imm8:
	mov	[thumb16_error],ERROR_instruction_not_16bit
	cmp	[operand_size],4			;LDR or STR?
	jnz	.encode_imm12
	mov	[thumb16_error],ERROR_register_out_of_range
	test	ecx,0x08				;a high register?
	jnz	.encode_imm12
	mov	[thumb16_error],ERROR_immediate_offset_out_of_range.0_0x3fc
	test	[immediate_value],not (0xff shl 2)
	jnz	.encode_imm12
	movzx	eax,[operand_register0]
	shl	eax,8
	or	edx,eax
	mov	eax,[immediate_value]
	shr	eax,2
	mov	dl,al
	mov	eax,ebp
	shr	eax,20-11
	and	eax,1 shl 11
	or	edx,eax
	mov	[thumb16_instruction],dx
	mov	[thumb16_error],0
	jmp	.encode_imm12
    .rd_q_rn_rm_p:
	test	ebp,1111b shl 8				;STRxT/LDRxT?
	jnz	.rd_q_rn_rm_shift_imm_p
	mov	[thumb16_error],ERROR_register_out_of_range.r0_r7
	test	ecx,0x080808				;any high registers?
	jnz	.rd_q_rn_rm_shift_imm_p
	test	[code_type],CPU_ACTIVITY_THUMBEE
	jz	.rd_q_rn_rm_p.thumb
	;only byte version encodings are available in T2EE mode
	mov	[thumb16_error],ERROR_instruction_not_t2ee
	test	ebp,01110b shl 20
	jnz	.rd_q_rn_rm_shift_imm_p
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_t2ee
	test	[cpu_capability_flags],CPU_CAPABILITY_T2EE
	jz	.rd_q_rn_rm_shift_imm_p
	jmp	.rd_q_rn_rm_p.thumbee
    .rd_q_rn_rm_p.thumb:
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_v4t
	test	[cpu_capability_flags],CPU_CAPABILITY_V4T
	jz	.rd_q_rn_rm_shift_imm_p
    .rd_q_rn_rm_p.thumbee:
    ;encode a 16bit form
	mov	eax,ebp
	shr	eax,20
	and	eax,11111b
	mov	edx,THUMB2_rd_address_rm_translate_table
    .find_16bit_rm_code:
	cmp	al,[edx]
	jz	.found_16bit_rm_code
	inc	edx
	cmp	edx,THUMB2_rd_address_rm_translate_table+8
	jb	.find_16bit_rm_code
	ud2
    .found_16bit_rm_code:
	sub	edx,THUMB2_rd_address_rm_translate_table
	shl	edx,9
	or	edx,0x5000
	movzx	eax,[operand_register0]
	or	edx,eax
	movzx	eax,[operand_register1]
	shl	eax,3
	or	edx,eax
	movzx	eax,[operand_register2]
	shl	eax,6
	or	edx,eax
	mov	[thumb16_instruction],dx
	mov	[thumb16_error],0
    .rd_q_rn_rm_shift_imm_p:
	mov	[thumb32_error],ERROR_r15_not_valid.base
	cmp	ch,0xf
	jz	THUMB_post_process
	mov	[thumb32_error],ERROR_shift_type_must_be_lsl
	cmp	[instruction_shift_op],ARM_SHIFT_OPCODE_LSL
	jnz	THUMB_post_process
	;T2EE
	cmp	[thumb16_error],0			;second time around?
	jz	.rd_q_rn_rm_shift_imm_p.thumb
	test	[code_type],CPU_ACTIVITY_THUMBEE
	jz	.rd_q_rn_rm_shift_imm_p.thumb
	test	ebp,1111b shl 8				;STRxT/LDRxT?
	jnz	.rd_q_rn_rm_shift_imm_p.thumb
	mov	[thumb16_error],ERROR_register_out_of_range
	test	ecx,0x080808				;any high registers?
	jnz	.rd_q_rn_rm_shift_imm_p.thumb
	;hword and word version encodings are available in T2EE mode
	mov	[thumb16_error],ERROR_instruction_not_t2ee
	test	ebp,01110b shl 20
	jz	.rd_q_rn_rm_shift_imm_p.thumb
	mov	[thumb16_error],ERROR_requires_cpu_capability_arm_t2ee
	test	[cpu_capability_flags],CPU_CAPABILITY_T2EE
	jz	.rd_q_rn_rm_shift_imm_p.thumb
	;check bits 22:21 match the shift count
	mov	ecx,ERROR_shift_value_out_of_range.1
	mov	edx,ERROR_shift_value_out_of_range.2
	test	ebp,1 shl 22
	cmovnz	ecx,edx
	mov	[thumb16_error],ecx
	mov	ecx,ebp
	shr	ecx,21
	and	ecx,3
	cmp	ecx,[immediate_value2]
	jz	.rd_q_rn_rm_p.thumbee
    .rd_q_rn_rm_shift_imm_p.thumb:
	mov	[thumb32_error],ERROR_shift_value_out_of_range.0_3
	cmp	[immediate_value2],3
	ja	THUMB_post_process
	mov	[thumb32_error],ERROR_instruction_not_16bit
	test	ebp,1111b shl 8				;STRxT/LDRxT?
	jnz	THUMB_post_process
	jmp	.encode_regs
    .encode_imm12:
	mov	eax,[immediate_value]
	mov	[thumb32_error],ERROR_immediate_offset_out_of_range.0_0xfff
	test	ebp,1 shl 23
	jnz	.imm12_positive_only
	mov	[thumb32_error],ERROR_immediate_offset_out_of_range.0xfff
	or	ebp,1 shl 23
	test	eax,eax
	jns	.imm12_positive_only
	and	ebp,not (1 shl 23)
	neg	eax
    .imm12_positive_only:
	cmp	eax,0xfff
	ja	THUMB_post_process
	or	ebp,eax
	jmp	.encode_regs
    .encode_imm8:
	or	ebp,1 shl 11
	mov	eax,[immediate_value]
	mov	[thumb32_error],ERROR_immediate_offset_out_of_range.0_0xff
	test	ebp,1 shl 9
	jnz	.imm8_positive_only
	mov	[thumb32_error],ERROR_immediate_offset_out_of_range.0xff
	or	ebp,1 shl 9
	test	eax,eax
	jns	.imm8_positive_only
	and	ebp,not (1 shl 9)
	neg	eax
    .imm8_positive_only:
	cmp	eax,0xff
	ja	THUMB_post_process
	or	ebp,eax
    .encode_regs:
	mov	ecx,ebp
	and	ecx,0x17 shl 20 + 0xf shl 12
	;if bits[24..20]=0x011 and bits[15..12]=1111 then check for MP (PLDW)
	cmp	ecx,0x03 shl 20 + 0xf shl 12
	jz	.check_PLDW
	;if bits[24..20]=1x001 and bits[15..12]=1111 then check for v7 (PLI)
	cmp	ecx,0x11 shl 20 + 0xf shl 12
	jz	.check_PLI
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_7m
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	THUMB_post_process
	jmp	.version_check_okay
    .check_PLDW:
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_mp
	test	[cpu_capability_flags],CPU_CAPABILITY_MP
	jz	THUMB_post_process
	mov	[thumb32_error],ERROR_r15_not_valid.base
	cmp	[operand_register1],0xf
	jz	THUMB_post_process
	jmp	.version_check_okay
    .check_PLI:
	mov	[thumb32_error],ERROR_requires_cpu_capability_arm_v7
	test	[cpu_capability_flags],CPU_CAPABILITY_V7
	jz	THUMB_post_process
    .version_check_okay:
	movzx	eax,[operand_register0]
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]
	or	ebp,eax
	mov	eax,[immediate_value2]
	shl	eax,4
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

THUMB2_rd_rd2_address:
	;used by STRD, LDRD
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\							; 0=rd,[rn]
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_imm>,\					; 1=rd,[rn],imm
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\					; 2=rd,[rn,imm]
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right,TMPL_modifier_exclaim>,\			; 3=rd,[rn,imm]!
	<TMPL_base_reg,TMPL_bracket_left,TMPL_address,TMPL_bracket_right>,\							; 4=rd,[imm]	PC relative
	<TMPL_base_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>,\							; 5=rd,[exp]	implicit reg from structure
	<TMPL_base_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right,TMPL_modifier_exclaim>,\				; 6=rd,[exp]!	implicit reg from structure
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\					; 7=rd,rd2,[rn]
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_imm>,\				; 8=rd,rd2,[rn],imm
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\				; 9=rd,rd2,[rn,imm]
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right,TMPL_modifier_exclaim>,\	;10=rd,rd2,[rn,imm]!
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_address,TMPL_bracket_right>,\					;11=rd,rd2,[imm]	PC relative
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>,\					;12=rd,rd2,[exp]	implicit reg from structure
	<TMPL_base_reg,TMPL_base_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right,TMPL_modifier_exclaim>		;13=rd,rd2,[exp]!	implicit reg from structure
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	ERROR_requires_cpu_capability_arm_7m
	mov	ebp,[thumb32_instruction]
	sub	al,7
	jae	.dest_registers_defined
	mov	ecx,[operand_registers]
	mov	dl,cl
	inc	cl
	shl	ecx,8
	mov	cl,dl
	mov	[operand_registers],ecx
	add	al,7
    .dest_registers_defined:
	mov	cl,0x33
	mov	dl,0x37
	test	ebp,1 shl 20
	cmovz	ecx,edx
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	ecx,[operand_registers]
	cmp	cl,ch
	mov	[thumb32_error],ERROR_destination_registers_must_differ
	jz	THUMB_post_process
	cmp	al,0
	jz	.rd_q_rn_p
	cmp	al,1
	jz	.rd_q_rn_p_imm
	cmp	al,2
	jz	.rd_q_rn_imm_p
	cmp	al,3
	jz	.rd_q_rn_imm_p!
	cmp	al,4
	jz	.rd_q_imm_p
	cmp	al,5
	jz	.rd_q_exp_p
	cmp	al,6
	jz	.rd_q_exp_p!
	ud2
    .rd_q_rn_p_imm:
	or	ebp,1 shl 21				;set W
	jmp	.check_dest_and_source_differ
    .rd_q_imm_p:
	mov	[operand_register2],0xf
	call	THUMB_query_condition_pc
	mov	eax,ecx
	mov	ch,0xf
	mov	edx,[addressing_space]
	sub	eax,[edx+0x00]
	and	eax,not 3				;round down
	neg	eax
	add	eax,[immediate_value]
	sub	eax,4
	mov	[immediate_value],eax
    .rd_q_exp_p:
    .rd_q_rn_imm_p:
    .rd_q_rn_p:
	or	ebp,1 shl 24				;set P
	jmp	.encode
    .rd_q_exp_p!:
    .rd_q_rn_imm_p!:
	or	ebp,1 shl 24 + 1 shl 21			;set P & W
    .check_dest_and_source_differ:
	mov	[thumb32_error],ERROR_base_and_dest_must_differ_with_writeback
	mov	cl,[operand_register2]
	cmp	cl,[operand_register1]
	jz	THUMB_post_process
	cmp	cl,[operand_register0]
	jz	THUMB_post_process
    .encode:
	mov	eax,[immediate_value]
	mov	[thumb32_error],ERROR_immediate_offset_out_of_range.0x3fc
	or	ebp,1 shl 23
	test	eax,eax
	jns	.imm8_positive
	and	ebp,not (1 shl 23)
	neg	eax
    .imm8_positive:
	test	eax,not (0xff shl 2)
	jnz	THUMB_post_process
	shr	eax,2
	or	ebp,eax
	movzx	eax,[operand_register0]
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]
	shl	eax,8
	or	ebp,eax
	movzx	eax,[operand_register2]
	shl	eax,16
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_rd_imm16:
	;used by MOVW, MOVT
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_imm>			;0=reg,imm
    .do:
	;entry from MOV
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_rd_imm16
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	ERROR_requires_cpu_capability_arm_t2
	movzx	eax,[operand_register0]
	cmp	eax,15
	jz	ERROR_r15_not_valid.first
	shl	eax,12
	or	ebp,eax
	mov	eax,[immediate_value]
	cmp	eax,0xffff
	mov	ecx,ERROR_immediate_value_out_of_range.0_0xffff
	ja	ARM_store_instruction_with_error
	mov	ecx,eax
	and	eax,0x0fff
	and	ecx,0xf000
	shl	ecx,16-12
	or	ebp,eax
	or	ebp,ecx
	jmp	ARM_store_instruction
    .THUMB_rd_imm16:
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	ERROR_requires_cpu_capability_arm_7m
	mov	ebp,[thumb32_instruction]
	mov	cl,0x11
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	movzx	eax,[operand_register0]
	shl	eax,8
	or	ebp,eax
	mov	eax,[immediate_value]
	cmp	eax,0xffff
	mov	[thumb32_error],ERROR_immediate_value_out_of_range.0_0xffff
	ja	THUMB_post_process
	mov	ecx,eax
	mov	edx,eax
	mov	ebx,eax
	and	eax,0x00ff
	and	ecx,0x0700
	and	edx,0x0800
	and	ebx,0xf000
	shl	ecx,12-8
	shl	edx,26-11
	shl	ebx,16-12
	or	ebp,eax
	or	ebp,ecx
	or	ebp,edx
	or	ebp,ebx
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_rd_rn_imm_imm_X:
	;used by SBFX, UBFX
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg,TMPL_imm,TMPL_comma,TMPL_imm2>	;0=reg,reg,imm,imm
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_rd_rn_imm_imm
	test	[cpu_capability_flags],CPU_CAPABILITY_T2
	jz	ERROR_requires_cpu_capability_arm_t2
	movzx	eax,[operand_register0]
	cmp	eax,15
	jz	ERROR_r15_not_valid
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]
	cmp	eax,15
	jz	ERROR_r15_not_valid
	or	ebp,eax
	mov	eax,[immediate_value]
	cmp	eax,31
	mov	ecx,ERROR_lsb_out_of_range.0_31
	ja	ARM_store_instruction_with_error
	shl	eax,7
	or	ebp,eax
	mov	ecx,ERROR_width_out_of_range
	mov	edx,[immediate_value2]
	shr	eax,7
	add	eax,edx
	jc	ARM_store_instruction_with_error
	cmp	eax,32
	ja	ARM_store_instruction_with_error
	dec	edx
	shl	edx,16
	or	ebp,edx
	jmp	ARM_store_instruction
    .THUMB_rd_rn_imm_imm:
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	ERROR_requires_cpu_capability_arm_7m
	mov	ecx,0x33
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	ebp,[thumb32_instruction]
	movzx	eax,[operand_register0]
	shl	eax,8
	or	ebp,eax
	movzx	eax,[operand_register1]
	shl	eax,16
	or	ebp,eax
	mov	[thumb32_error],ERROR_lsb_out_of_range.0_31
	mov	eax,[immediate_value]
	cmp	eax,31
	ja	THUMB_post_process
	mov	ecx,eax
	mov	edx,eax
	and	ecx,3
	and	edx,0x1c
	shl	ecx,6
	shl	edx,12-2
	or	ebp,ecx
	or	ebp,edx
	mov	[thumb32_error],ERROR_width_out_of_range
	mov	edx,[immediate_value2]
	add	eax,edx
	jc	THUMB_post_process
	cmp	eax,32
	ja	THUMB_post_process
	dec	edx
	or	ebp,edx
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

THUMB_q_rn_rm_p:
	;used by TBB
	call	decode_template
    TEMPLATE \
	<TMPL_bracket_left,TMPL_address_reg,TMPL_address_reg,TMPL_bracket_right>	;0=[rn,rm]
	test	[code_type],CPU_ACTIVITY_ARM
	jnz	ERROR_instruction_not_32bit
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	ERROR_requires_cpu_capability_arm_7m
	or	[thumb_flags_16],THUMB_FLAG_ONLY_LAST_IT
	or	[thumb_flags_32],THUMB_FLAG_ONLY_LAST_IT
	mov	ecx,0x32
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	ebp,[thumb32_instruction]
	movzx	eax,[operand_register0]
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register1]
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

THUMB_q_rn_rm_lsl_1_p:
	;used by TBH
	call	decode_template
    TEMPLATE \
	<TMPL_bracket_left,TMPL_address_reg,TMPL_address_reg,TMPL_shift_op,TMPL_imm,TMPL_bracket_right>	;0=[rn,rm]
	test	[code_type],CPU_ACTIVITY_ARM
	jnz	ERROR_instruction_not_32bit
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	ERROR_requires_cpu_capability_arm_7m
	or	[thumb_flags_16],THUMB_FLAG_ONLY_LAST_IT
	or	[thumb_flags_32],THUMB_FLAG_ONLY_LAST_IT
	mov	ecx,0x32
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	cmp	[instruction_shift_op],ARM_SHIFT_OPCODE_LSL
	jnz	ERROR_shift_type_must_be_lsl
	cmp	[immediate_value],1
	jnz	ERROR_immediate_offset_out_of_range.1
	mov	ebp,[thumb32_instruction]
	movzx	eax,[operand_register0]
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register1]
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

;thumbee

THUMBEE_enterx:
	;used by ENTERX, LEAVEX
	call	decode_template
    TEMPLATE \
	<TMPL_EOL>					;0=EOL
	test	[cpu_capability_flags],CPU_CAPABILITY_T2EE
	jz	ERROR_requires_cpu_capability_arm_t2ee
	test	[code_type],CPU_ACTIVITY_ARM
	jnz	ERROR_instruction_not_32bit
	mov	[thumb32_error],0
	jmp	THUMB_post_process

THUMBEE_rn_rm:
	;used by CHKA
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg>			;0=rn,rm
	test	[cpu_capability_flags],CPU_CAPABILITY_T2EE
	jz	ERROR_requires_cpu_capability_arm_t2ee
	test	[code_type],CPU_ACTIVITY_THUMBEE
	jz	ERROR_instruction_needs_t2ee
	mov	cl,0x23
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	movzx	eax,[operand_register0]
	movzx	ecx,[operand_register1]
	mov	edx,eax
	and	eax,7
	and	edx,8
	shl	edx,7-3
	or	eax,edx
	shl	ecx,3
	lea	eax,[eax+ecx+0xca00]
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	THUMB_post_process

THUMBEE_imm:
	;used by HB, HBL
	call	decode_template
    TEMPLATE \
	<TMPL_imm>					;0=imm
	test	[cpu_capability_flags],CPU_CAPABILITY_T2EE
	jz	ERROR_requires_cpu_capability_arm_t2ee
	test	[code_type],CPU_ACTIVITY_THUMBEE
	jz	ERROR_instruction_needs_t2ee
	or	[thumb_flags_16],THUMB_FLAG_ONLY_LAST_IT
	or	[thumb_flags_32],THUMB_FLAG_ONLY_LAST_IT
	mov	[thumb16_error],ERROR_immediate_value_out_of_range.0_0xff
	mov	eax,[immediate_value]
	cmp	eax,0xff
	ja	THUMB_post_process
	or	eax,ebp
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	THUMB_post_process

THUMBEE_imm5_imm5:
	;used by HBLP
	call	decode_template
    TEMPLATE \
	<TMPL_imm,TMPL_comma,TMPL_imm2>				;0=imm,imm
	test	[cpu_capability_flags],CPU_CAPABILITY_T2EE
	jz	ERROR_requires_cpu_capability_arm_t2ee
	test	[code_type],CPU_ACTIVITY_THUMBEE
	jz	ERROR_instruction_needs_t2ee
	or	[thumb_flags_16],THUMB_FLAG_ONLY_LAST_IT
	or	[thumb_flags_32],THUMB_FLAG_ONLY_LAST_IT
	mov	[thumb16_error],ERROR_immediate_value_out_of_range.0_31
	mov	eax,[immediate_value]
	cmp	eax,0x1f
	ja	THUMB_post_process
	shl	eax,5
	or	ebp,eax
	mov	eax,[immediate_value2]
	cmp	eax,0x1f
	ja	THUMB_post_process
	or	eax,ebp
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	THUMB_post_process

THUMBEE_imm3_imm5:
	;used by HBP
	call	decode_template
    TEMPLATE \
	<TMPL_imm,TMPL_comma,TMPL_imm2>				;0=imm,imm
	test	[cpu_capability_flags],CPU_CAPABILITY_T2EE
	jz	ERROR_requires_cpu_capability_arm_t2ee
	test	[code_type],CPU_ACTIVITY_THUMBEE
	jz	ERROR_instruction_needs_t2ee
	or	[thumb_flags_16],THUMB_FLAG_ONLY_LAST_IT
	or	[thumb_flags_32],THUMB_FLAG_ONLY_LAST_IT
	mov	[thumb16_error],ERROR_immediate_value_out_of_range.0_7
	mov	eax,[immediate_value]
	cmp	eax,7
	ja	THUMB_post_process
	shl	eax,5
	or	ebp,eax
	mov	[thumb16_error],ERROR_immediate_value_out_of_range.0_31
	mov	eax,[immediate_value2]
	cmp	eax,0x1f
	ja	THUMB_post_process
	or	eax,ebp
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	jmp	THUMB_post_process

;v7

ARM_imm4:
	;used by DBG
	call	decode_template
    TEMPLATE \
	<TMPL_imm>					;0=imm
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_imm4
	test	[cpu_capability_flags],CPU_CAPABILITY_V7
	jz	ERROR_requires_cpu_capability_arm_v7
	mov	eax,[immediate_value]
	cmp	eax,15
	ja	.out_of_range
	or	ebp,eax
	jmp	ARM_store_instruction
    .out_of_range:
	mov	ecx,ERROR_immediate_value_out_of_range.0_15
	jmp	ARM_store_instruction_with_error
    .THUMB_imm4:
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	ERROR_requires_cpu_capability_arm_7m
	mov	ebp,[thumb32_instruction]
	mov	[thumb32_error],ERROR_immediate_value_out_of_range.0_15
	mov	eax,[immediate_value]
	cmp	eax,15
	ja	THUMB_post_process
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_barrier:
	;used by DMB, DSB, ISB
	call	decode_template
    TEMPLATE \
	<TMPL_barrier>,\				;0=barrier
	<TMPL_EOL>					;1=default
	cmp	al,1
	jnz	.encode
	or	[operand_register0],0xf
    .encode:
	test	ebp,0x20				;ISB?
	jz	.do
	cmp	[operand_register0],0xf			;only SY is valid
	jnz	ERROR_parameter_n_not_valid.all
    .do:
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_barrier
	test	[cpu_capability_flags],CPU_CAPABILITY_V7
	jz	ERROR_requires_cpu_capability_arm_v7
	movzx	eax,[operand_register0]
	or	ebp,eax
	jmp	ARM_store_instruction
    .THUMB_barrier:
	test	[cpu_capability_flags],CPU_CAPABILITY_6M
	jz	ERROR_requires_cpu_capability_arm_6m
	mov	ebp,[thumb32_instruction]
	movzx	eax,[operand_register0]
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_nops:
	;used by SEV, WFE, WFI, YIELD
	call	decode_template
    TEMPLATE \
	<TMPL_EOL>					;0=empty
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_nops
	test	[cpu_capability_flags],CPU_CAPABILITY_K
	jz	ERROR_requires_cpu_capability_arm_k
	jmp	ARM_store_instruction
    .THUMB_nops:
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	setz	cl
	movzx	ecx,cl
	neg	ecx
	and	ecx,ERROR_requires_cpu_capability_arm_7m
	mov	[thumb32_error],ecx
	test	[cpu_capability_flags],CPU_CAPABILITY_6M
	setz	cl
	movzx	ecx,cl
	neg	ecx
	and	ecx,ERROR_requires_cpu_capability_arm_6m
	mov	[thumb16_error],ecx
	mov	eax,[thumb32_instruction]
	and	eax,0xf
	shl	eax,4
	or	eax,0xbf00
	mov	[thumb16_instruction],ax
	jmp	THUMB_post_process

;DIV

ARM_rd_rn_rm_DIV:
	;used by SDIV, UDIV
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg>,\			;0=reg,reg	alternate for reg,reg,reg
	<TMPL_base_reg,TMPL_base_reg,TMPL_base_reg>	;1=reg,reg,reg
	cmp	al,0
	jnz	.encode
	mov	ax,word[operand_register0]
	mov	word[operand_register1],ax
    .encode:
	test	[code_type],CPU_ACTIVITY_ARM
	jnz	.v7ve
	test	[cpu_capability_flags],CPU_CAPABILITY_DIV
	jz	ERROR_requires_cpu_capability_arm_div
	mov	cl,0x77
	call	THUMB2_check_bad_regs
	jc	THUMB_post_process
	mov	ebp,[thumb32_instruction]
	movzx	eax,[operand_register0]			;rd
	shl	eax,8
	or	ebp,eax
	movzx	eax,[operand_register1]			;rn
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]			;rm
	or	ebp,eax
	mov	[thumb32_instruction],ebp
	mov	[thumb32_error],0
	jmp	THUMB_post_process
    .v7ve:
	test	[cpu_capability_flags],CPU_CAPABILITY_VE
	jz	ERROR_requires_cpu_capability_arm_ve
	movzx	eax,[operand_register0]			;rd
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.all
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register1]			;rn
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.all
	or	ebp,eax
	movzx	eax,[operand_register2]			;rm
	cmp	eax,0xf
	jz	ERROR_r15_not_valid.all
	shl	eax,8
	or	ebp,eax
	jmp	ARM_store_instruction

;versionless

ARM_und:
	;used by UND
	call	decode_template
    TEMPLATE \
	<TMPL_imm>,\					;0=imm
	<TMPL_EOL>					;1=empty
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_und
	mov	eax,[immediate_value]
	cmp	eax,0xffff
	ja	ERROR_immediate_offset_out_of_range.0_0xffff
	mov	ecx,eax
	and	eax,0x000f
	and	ecx,0xfff0
	shl	ecx,4
	or	ebp,eax
	or	ebp,ecx
	jmp	ARM_store_instruction
    .THUMB_und:
	mov	eax,[immediate_value]
	mov	[thumb16_error],ERROR_immediate_offset_out_of_range.0_0xff
	cmp	eax,0xff
	ja	.T2
	mov	ah,0xde
	mov	[thumb16_instruction],ax
	mov	[thumb16_error],0
	mov	eax,[immediate_value]
    .T2:
	test	[cpu_capability_flags],CPU_CAPABILITY_7M
	jz	THUMB_post_process
	mov	[thumb32_error],ERROR_immediate_offset_out_of_range.0_0xfff
	cmp	eax,0xfff
	ja	THUMB_post_process
	mov	ecx,eax
	mov	edx,eax
	and	eax,0x00f
	and	ecx,0x0f0
	and	edx,0xf00
	shl	ecx,8-4
	shl	edx,16-8
	or	eax,ecx
	or	eax,edx
	or	[thumb32_instruction],eax
	mov	[thumb32_error],0
	jmp	THUMB_post_process

;FPA

FPA_fn_fm:
	;used by CMF, CMFE, CNF, CNFE
	call	decode_template
    TEMPLATE \
	<TMPL_fpa_freg,TMPL_fpa_freg>,\			;0=fn,fm
	<TMPL_fpa_freg,TMPL_imm_float>			;1=fn,const
	test	[copro_capability_flags],COPRO_CAPABILITY_FPA_V1
	jz	ERROR_requires_copro_capability_fpa_v1
	movzx	ecx,[operand_register0]			;fn
	shl	ecx,16
	or	ebp,ecx
	cmp	al,1
	jz	FPA_encode_constant
	movzx	ecx,[operand_register1]			;fm
	or	ebp,ecx
	jmp	ARM_post_process_copro

FPA_rd:
	;used by RFC, RFS, WFC, WFS
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg>					;0=rd
	test	[copro_capability_flags],COPRO_CAPABILITY_FPA_V1
	jz	ERROR_requires_copro_capability_fpa_v1
	movzx	eax,[operand_register0]			;rd
	shl	eax,12
	or	ebp,eax
	jmp	ARM_post_process_copro

FPA_fd_fn_fm:
	;used by ADFD, ADFDM, ADFDP, ADFDZ, ADFE, ADFEM, ADFEP, ADFEZ, ADFS, ADFSM, ADFSP, ADFSZ,
	;	 DVFD, DVFDM, DVFDP, DVFDZ, DVFE, DVFEM, DVFEP, DVFEZ, DVFS, DVFSM, DVFSP, DVFSZ,
	;	 FDVD, FDVDM, FDVDP, FDVDZ, FDVE, FDVEM, FDVEP, FDVEZ, FDVS, FDVSM, FDVSP, FDVSZ,
	;	 FMLD, FMLDM, FMLDP, FMLDZ, FMLE, FMLEM, FMLEP, FMLEZ, FMLS, FMLSM, FMLSP, FMLSZ,
	;	 FRDD, FRDDM, FRDDP, FRDDZ, FRDE, FRDEM, FRDEP, FRDEZ, FRDS, FRDSM, FRDSP, FRDSZ,
	;	 MUFD, MUFDM, MUFDP, MUFDZ, MUFE, MUFEM, MUFEP, MUFEZ, MUFS, MUFSM, MUFSP, MUFSZ,
	;	 POLD, POLDM, POLDP, POLDZ, POLE, POLEM, POLEP, POLEZ, POLS, POLSM, POLSP, POLSZ,
	;	 POWD, POWDM, POWDP, POWDZ, POWE, POWEM, POWEP, POWEZ, POWS, POWSM, POWSP, POWSZ,
	;	 RDFD, RDFDM, RDFDP, RDFDZ, RDFE, RDFEM, RDFEP, RDFEZ, RDFS, RDFSM, RDFSP, RDFSZ,
	;	 RMFD, RMFDM, RMFDP, RMFDZ, RMFE, RMFEM, RMFEP, RMFEZ, RMFS, RMFSM, RMFSP, RMFSZ,
	;	 RPWD, RPWDM, RPWDP, RPWDZ, RPWE, RPWEM, RPWEP, RPWEZ, RPWS, RPWSM, RPWSP, RPWSZ,
	;	 RSFD, RSFDM, RSFDP, RSFDZ, RSFE, RSFEM, RSFEP, RSFEZ, RSFS, RSFSM, RSFSP, RSFSZ,
	;	 SUFD, SUFDM, SUFDP, SUFDZ, SUFE, SUFEM, SUFEP, SUFEZ, SUFS, SUFSM, SUFSP, SUFSZ
	call	decode_template
    TEMPLATE \
	<TMPL_fpa_freg,TMPL_fpa_freg,TMPL_fpa_freg>,\	;0=fd,fn,fm
	<TMPL_fpa_freg,TMPL_fpa_freg,TMPL_imm_float>	;1=fd,fn,const
	test	[copro_capability_flags],COPRO_CAPABILITY_FPA_V1
	jz	ERROR_requires_copro_capability_fpa_v1
	movzx	ecx,[operand_register0]			;fd
	shl	ecx,12
	or	ebp,ecx
	movzx	ecx,[operand_register1]			;fn
	shl	ecx,16
	or	ebp,ecx
	cmp	al,1
	jz	FPA_encode_constant
	movzx	ecx,[operand_register2]			;fm
	or	ebp,ecx
	jmp	ARM_post_process_copro

FPA_fn_rd:
	;used by FLTD, FLTDM, FLTDP, FLTDZ, FLTE, FLTEM, FLTEP, FLTEZ, FLTS, FLTSM, FLTSP, FLTSZ
	call	decode_template
    TEMPLATE \
	<TMPL_fpa_freg,TMPL_base_reg>			;0=fn,rd
	test	[copro_capability_flags],COPRO_CAPABILITY_FPA_V1
	jz	ERROR_requires_copro_capability_fpa_v1
	movzx	eax,[operand_register0]			;fn
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register1]			;rd
	shl	eax,12
	or	ebp,eax
	jmp	ARM_post_process_copro

FPA_fd_imm_address5:
	;used by LFM, SFM
	call	decode_template
    TEMPLATE \
	<TMPL_fpa_freg,TMPL_imm2,TMPL_comma,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\					;0=fd,imm2,[rn]
	<TMPL_fpa_freg,TMPL_imm2,TMPL_comma,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_imm>,\				;1=fd,imm2,[rn],imm
	<TMPL_fpa_freg,TMPL_imm2,TMPL_comma,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_option>,\			;2=fd,imm2,[rn],{imm}
	<TMPL_fpa_freg,TMPL_imm2,TMPL_comma,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\				;3=fd,imm2,[rn,imm]
	<TMPL_fpa_freg,TMPL_imm2,TMPL_comma,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right,TMPL_modifier_exclaim>,\	;4=fd,imm2,[rn,imm]!
	<TMPL_fpa_freg,TMPL_imm2,TMPL_comma,TMPL_bracket_left,TMPL_address,TMPL_bracket_right>,\					;5=fd,imm2,[imm]	PC relative
	<TMPL_fpa_freg,TMPL_imm2,TMPL_comma,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>,\					;6=fd,imm2,[exp]	implicit reg from structure
	<TMPL_fpa_freg,TMPL_imm2,TMPL_comma,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right,TMPL_modifier_exclaim>			;7=fd,imm2,[exp]!	implicit reg from structure
	test	[copro_capability_flags],COPRO_CAPABILITY_FPA_V2
	jz	ERROR_requires_copro_capability_fpa_v2
	mov	ecx,[immediate_value2]
	dec	ecx
	cmp	ecx,4
	ja	ERROR_count_value_out_of_range.1_4
	inc	ecx
	and	ecx,3
	mov	edx,ecx
	and	ecx,1
	and	edx,2
	shl	ecx,15
	shl	edx,22-1
	or	ebp,ecx
	or	ebp,edx
	mov	ecx,[operand_registers]
	shl	ecx,8
	mov	[operand_registers],ecx
	jmp	ARM_copro_crd_address5.version_check_okay

FPA_fd_imm_rn:
	;used by LFMEA, LFMFD, SFMEA, SFMFD
	call	decode_template
    TEMPLATE \
	<TMPL_fpa_freg,TMPL_imm,TMPL_comma,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\			;0=fd,imm,[rn]
	<TMPL_fpa_freg,TMPL_imm,TMPL_comma,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_modifier_exclaim>	;1=fd,imm,[rn]!
	test	[copro_capability_flags],COPRO_CAPABILITY_FPA_V2
	jz	ERROR_requires_copro_capability_fpa_v2
	movzx	eax,al
	shl	eax,21
	or	ebp,eax					;set W bit
	movzx	eax,[operand_register0]			;fd
	shl	eax,12
	or	ebp,eax
	mov	ecx,[immediate_value]
	dec	ecx
	cmp	ecx,4
	ja	ERROR_count_value_out_of_range.1_4
	inc	ecx
	and	ecx,3
	mov	edx,ecx
	and	ecx,1
	and	edx,2
	shl	ecx,15
	shl	edx,22-1
	or	ebp,ecx
	or	ebp,edx
	movzx	eax,[operand_register1]			;rn
	test	ebp,1 shl 21				;W?
	jz	.rn_okay
	cmp	eax,15					;PC?
	jz	ERROR_r15_not_valid.third
    .rn_okay:
	shl	eax,16
	or	ebp,eax
	mov	ecx,ebp
	and	ecx,1 shl 24 + 1 shl 23 + 1 shl 21	;P, U, W
	cmp	ecx,0 shl 24 + 1 shl 23 + 0 shl 21	;0, 1, 0
	jz	ARM_post_process_copro
	mov	ecx,[immediate_value]
	imul	ecx,3
	or	ebp,ecx					;set the offset field
	jmp	ARM_post_process_copro

FPA_fd_address5:
	;used by LDFD, LDFE, LDFP, LDFS, STFD, STFE, STFP, STFS
	call	decode_template
    TEMPLATE \
	<TMPL_fpa_freg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\					;0=fd,[rn]
	<TMPL_fpa_freg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_imm>,\			;1=fd,[rn],imm
	<TMPL_fpa_freg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_option>,\			;2=fd,[rn],{imm}
	<TMPL_fpa_freg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\			;3=fd,[rn,imm]
	<TMPL_fpa_freg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right,TMPL_modifier_exclaim>,\	;4=fd,[rn,imm]!
	<TMPL_fpa_freg,TMPL_bracket_left,TMPL_address,TMPL_bracket_right>,\					;5=fd,[imm]	PC relative
	<TMPL_fpa_freg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>,\					;6=fd,[exp]	implicit reg from structure
	<TMPL_fpa_freg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right,TMPL_modifier_exclaim>		;7=fd,[exp]!	implicit reg from structure
	test	[copro_capability_flags],COPRO_CAPABILITY_FPA_V1
	jz	ERROR_requires_copro_capability_fpa_v1
	mov	ecx,[operand_registers]
	shl	ecx,8
	mov	[operand_registers],ecx
	jmp	ARM_copro_crd_address5.version_check_okay

FPA_fd_fm:
	;used by ABSD, ABSDM, ABSDP, ABSDZ, ABSE, ABSEM, ABSEP, ABSEZ, ABSS, ABSSM, ABSSP, ABSSZ,
	;	 ACSD, ACSDM, ACSDP, ACSDZ, ACSE, ACSEM, ACSEP, ACSEZ, ACSS, ACSSM, ACSSP, ACSSZ,
	;	 ASND, ASNDM, ASNDP, ASNDZ, ASNE, ASNEM, ASNEP, ASNEZ, ASNS, ASNSM, ASNSP, ASNSZ,
	;	 ATND, ATNDM, ATNDP, ATNDZ, ATNE, ATNEM, ATNEP, ATNEZ, ATNS, ATNSM, ATNSP, ATNSZ,
	;	 COSD, COSDM, COSDP, COSDZ, COSE, COSEM, COSEP, COSEZ, COSS, COSSM, COSSP, COSSZ,
	;	 EXPD, EXPDM, EXPDP, EXPDZ, EXPE, EXPEM, EXPEP, EXPEZ, EXPS, EXPSM, EXPSP, EXPSZ,
	;	 LGND, LGNDM, LGNDP, LGNDZ, LGNE, LGNEM, LGNEP, LGNEZ, LGNS, LGNSM, LGNSP, LGNSZ,
	;	 LOGD, LOGDM, LOGDP, LOGDZ, LOGE, LOGEM, LOGEP, LOGEZ, LOGS, LOGSM, LOGSP, LOGSZ,
	;	 MNFD, MNFDM, MNFDP, MNFDZ, MNFE, MNFEM, MNFEP, MNFEZ, MNFS, MNFSM, MNFSP, MNFSZ,
	;	 MVFD, MVFDM, MVFDP, MVFDZ, MVFE, MVFEM, MVFEP, MVFEZ, MVFS, MVFSM, MVFSP, MVFSZ,
	;	 NRMD, NRMDM, NRMDP, NRMDZ, NRME, NRMEM, NRMEP, NRMEZ, NRMS, NRMSM, NRMSP, NRMSZ,
	;	 RNDD, RNDDM, RNDDP, RNDDZ, RNDE, RNDEM, RNDEP, RNDEZ, RNDS, RNDSM, RNDSP, RNDSZ,
	;	 SIND, SINDM, SINDP, SINDZ, SINE, SINEM, SINEP, SINEZ, SINS, SINSM, SINSP, SINSZ,
	;	 SQTD, SQTDM, SQTDP, SQTDZ, SQTE, SQTEM, SQTEP, SQTEZ, SQTS, SQTSM, SQTSP, SQTSZ,
	;	 TAND, TANDM, TANDP, TANDZ, TANE, TANEM, TANEP, TANEZ, TANS, TANSM, TANSP, TANSZ,
	;	 URDD, URDDM, URDDP, URDDZ, URDE, URDEM, URDEP, URDEZ, URDS, URDSM, URDSP, URDSZ
	call	decode_template
    TEMPLATE \
	<TMPL_fpa_freg,TMPL_fpa_freg>,\			;0=fd,fm
	<TMPL_fpa_freg,TMPL_imm_float>			;1=fd,const
	test	[copro_capability_flags],COPRO_CAPABILITY_FPA_V1
	jz	ERROR_requires_copro_capability_fpa_v1
	movzx	ecx,[operand_register0]			;fd
	shl	ecx,12
	or	ebp,ecx
	cmp	al,1
	jz	FPA_encode_constant
	movzx	ecx,[operand_register1]			;fm
	or	ebp,ecx
	jmp	ARM_post_process_copro

FPA_rd_fm:
	;used by FIX, FIXM, FIXP, FIXZ
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_fpa_freg>			;0=rd,fm
	test	[copro_capability_flags],COPRO_CAPABILITY_FPA_V1
	jz	ERROR_requires_copro_capability_fpa_v1
	movzx	ecx,[operand_register0]			;rd
	shl	ecx,12
	or	ebp,ecx
	movzx	ecx,[operand_register1]			;fm
	or	ebp,ecx
	jmp	ARM_post_process_copro

	align 4
FPA_constants:
	dd	0.0
	dd	1.0
	dd	2.0
	dd	3.0
	dd	4.0
	dd	5.0
	dd	0.5
	dd	10.0

FPA_encode_constant:
	mov	eax,[immediate_value]
	mov	ecx,FPA_constants
	mov	edx,8
    .find_constant:
	dec	edx
	js	.not_found
	cmp	eax,[ecx+edx*4]
	jnz	.find_constant
	lea	eax,[edx+8]
	or	ebp,eax
	jmp	ARM_post_process_copro
    .not_found:
	mov	ecx,ERROR_constant_cannot_be_encoded
	jmp	ARM_post_process_copro_with_error

;MAVERICK

MAVERICK_crd_crn:
	;used by CFABS32, CFABS64, CFABSS, CFABSD, CFCPYS, CFCPYD, CFCVT32S, CFCVT32D, CFCVT64S, CFCVT64D, CFCVTD32,
	;	 CFCVTDS, CFCVTS32, CFCVTSD, CFNEG32, CFNEG64, CFNEGS, CFNEGD, CFTRUNCS32, CFTRUNCD32
	call	decode_template
    TEMPLATE \
	<TMPL_cpro_reg,TMPL_cpro_reg>			;0=crd,crn
	test	[copro_capability_flags],COPRO_CAPABILITY_MAVERICK
	jz	ERROR_requires_copro_capability_maverick
	movzx	eax,[operand_register0]			;crd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;crn
	shl	eax,16
	or	ebp,eax
	jmp	ARM_post_process_copro

MAVERICK_crd_crn_crm:
	;used by CFADD32, CFADD64, CFADDD, CFADDS, CFMAC32, CFMSC32, CFMUL32, CFMUL64, CFMULS, CFMULD,
	;	 CFSUB32, CFSUB64, CFSUBD, CFSUBS
	call	decode_template
    TEMPLATE \
	<TMPL_cpro_reg,TMPL_cpro_reg,TMPL_cpro_reg>	;0=crd,crn,crm
	test	[copro_capability_flags],COPRO_CAPABILITY_MAVERICK
	jz	ERROR_requires_copro_capability_maverick
	movzx	eax,[operand_register0]			;crd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;crn
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]			;crm
	or	ebp,eax
	jmp	ARM_post_process_copro

MAVERICK_rd_crn_crm:
	;used by CFCMP32, CFCMP64, CFCMPS, CFCMPD
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_cpro_reg,TMPL_cpro_reg>	;0=rd,crn,crm
	test	[copro_capability_flags],COPRO_CAPABILITY_MAVERICK
	jz	ERROR_requires_copro_capability_maverick
	movzx	eax,[operand_register0]			;rd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;crn
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]			;crm
	or	ebp,eax
	jmp	ARM_post_process_copro

MAVERICK_crd_address5:
	;used by CFLDR32, CFLDR64, CFLDRS, CFLDRD, CFSTR32, CFSTR64, CFSTRS, CFSTRD
	call	decode_template
    TEMPLATE \
	<TMPL_cpro_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\					;0=crd,[rn]
	<TMPL_cpro_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_imm>,\			;1=crd,[rn],imm
	<TMPL_cpro_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_option>,\			;2=crd,[rn],{imm}
	<TMPL_cpro_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\			;3=crd,[rn,imm]
	<TMPL_cpro_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right,TMPL_modifier_exclaim>,\	;4=crd,[rn,imm]!
	<TMPL_cpro_reg,TMPL_bracket_left,TMPL_address,TMPL_bracket_right>,\					;5=crd,[imm]	PC relative
	<TMPL_cpro_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>,\					;6=crd,[exp]	implicit reg from structure
	<TMPL_cpro_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right,TMPL_modifier_exclaim>		;7=crd,[exp]!	implicit reg from structure
	test	[copro_capability_flags],COPRO_CAPABILITY_MAVERICK
	jz	ERROR_requires_copro_capability_maverick
	mov	ecx,[operand_registers]
	shl	ecx,8
	mov	[operand_registers],ecx
	jmp	ARM_copro_crd_address5.version_check_okay

MAVERICK_aa_crd_crn_crm:
	;used by CFMADD32, CFMSUB32
	call	decode_template
    TEMPLATE \
	<TMPL_mvrk_areg,TMPL_cpro_reg,TMPL_cpro_reg,TMPL_cpro_reg>	;0=aa,crd,crn,crm
	test	[copro_capability_flags],COPRO_CAPABILITY_MAVERICK
	jz	ERROR_requires_copro_capability_maverick
	movzx	eax,[operand_register0]			;aa
	shl	eax,5
	or	ebp,eax
	movzx	eax,[operand_register1]			;crd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register2]			;crn
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register3]			;crm
	or	ebp,eax
	jmp	ARM_post_process_copro

MAVERICK_aa_ad_crn_crm:
	;used by CFMADDA32, CFMSUBA32
	call	decode_template
    TEMPLATE \
	<TMPL_mvrk_areg,TMPL_mvrk_areg,TMPL_cpro_reg,TMPL_cpro_reg>	;0=aa,ad,crn,crm
	test	[copro_capability_flags],COPRO_CAPABILITY_MAVERICK
	jz	ERROR_requires_copro_capability_maverick
	movzx	eax,[operand_register0]			;aa
	shl	eax,5
	or	ebp,eax
	movzx	eax,[operand_register1]			;ad
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register2]			;crn
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register3]			;crm
	or	ebp,eax
	jmp	ARM_post_process_copro

MAVERICK_crd_an:
	;used by CFMV32A, CFMV32AH, CFMV32AL, CFMV32AM, CFMV64A
	call	decode_template
    TEMPLATE \
	<TMPL_cpro_reg,TMPL_mvrk_areg>			;0=crd,an
	test	[copro_capability_flags],COPRO_CAPABILITY_MAVERICK
	jz	ERROR_requires_copro_capability_maverick
	movzx	eax,[operand_register0]			;crd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;an
	shl	eax,16
	or	ebp,eax
	jmp	ARM_post_process_copro

MAVERICK_crd_psc:
	;used by CFMV32SC
	call	decode_template
    TEMPLATE \
	<TMPL_cpro_reg,TMPL_mvrk_psc>			;0=crd,psc
	test	[copro_capability_flags],COPRO_CAPABILITY_MAVERICK
	jz	ERROR_requires_copro_capability_maverick
	movzx	eax,[operand_register0]			;crd
	shl	eax,12
	or	ebp,eax
	jmp	ARM_post_process_copro

MAVERICK_psc_crd:
	;used by CFMVSC32
	call	decode_template
    TEMPLATE \
	<TMPL_mvrk_psc,TMPL_cpro_reg>			;0=psc,crd
	test	[copro_capability_flags],COPRO_CAPABILITY_MAVERICK
	jz	ERROR_requires_copro_capability_maverick
	movzx	eax,[operand_register0]			;crd
	shl	eax,12
	or	ebp,eax
	jmp	ARM_post_process_copro

MAVERICK_crn_rd:
	;used by CFMV64HR, CFMV64LR, CFMVDHR, CFMVDLR, CFMVSR
	call	decode_template
    TEMPLATE \
	<TMPL_cpro_reg,TMPL_base_reg>			;0=crn,rd
	test	[copro_capability_flags],COPRO_CAPABILITY_MAVERICK
	jz	ERROR_requires_copro_capability_maverick
	movzx	eax,[operand_register0]			;crn
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register1]			;rd
	shl	eax,12
	or	ebp,eax
	jmp	ARM_post_process_copro

MAVERICK_ad_crn:
	;used by CFMVA32, CFMVA64, CFMVAH32, CFMVAL32, CFMVAM32
	call	decode_template
    TEMPLATE \
	<TMPL_mvrk_areg,TMPL_cpro_reg>			;0=ad,crn
	test	[copro_capability_flags],COPRO_CAPABILITY_MAVERICK
	jz	ERROR_requires_copro_capability_maverick
	movzx	eax,[operand_register0]			;ad
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;crn
	shl	eax,16
	or	ebp,eax
	jmp	ARM_post_process_copro

MAVERICK_rd_crn:
	;used by CFMVR64H, CFMVR64L, CFMVRDH, CFMVRDL, CFMVRS
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_cpro_reg>			;0=rd,crn
	test	[copro_capability_flags],COPRO_CAPABILITY_MAVERICK
	jz	ERROR_requires_copro_capability_maverick
	movzx	eax,[operand_register0]			;rd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;crn
	shl	eax,16
	or	ebp,eax
	jmp	ARM_post_process_copro

MAVERICK_crm_crn_rd:
	;used by CFRSHL32, CFRSHL64
	call	decode_template
    TEMPLATE \
	<TMPL_cpro_reg,TMPL_cpro_reg,TMPL_base_reg>	;0=crm,crn,rd
	test	[copro_capability_flags],COPRO_CAPABILITY_MAVERICK
	jz	ERROR_requires_copro_capability_maverick
	movzx	eax,[operand_register0]			;crm
	or	ebp,eax
	movzx	eax,[operand_register1]			;crn
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]			;rd
	shl	eax,12
	or	ebp,eax
	jmp	ARM_post_process_copro

MAVERICK_crd_crn_imm:
	;used by CFSH32, CFSH64
	call	decode_template
    TEMPLATE \
	<TMPL_cpro_reg,TMPL_cpro_reg,TMPL_imm>		;0=crm,crn,imm
	test	[copro_capability_flags],COPRO_CAPABILITY_MAVERICK
	jz	ERROR_requires_copro_capability_maverick
	movzx	eax,[operand_register0]			;crd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;crn
	shl	eax,16
	or	ebp,eax
	mov	eax,[immediate_value]			;imm
	cmp	eax,-64
	jl	.out_of_range
	cmp	eax,63
	jg	.out_of_range
	mov	ecx,eax
	and	eax,0x0f
	and	ecx,0x70
	shl	ecx,1
	or	ebp,eax
	or	ebp,ecx
	jmp	ARM_post_process_copro
    .out_of_range:
	mov	ecx,ERROR_immediate_value_out_of_range.m64_63
	jmp	ARM_post_process_copro_with_error

;vfp1

VFP_sd_sn_sm:
	;used by FMULS, FNMULS, FADDS, FSUBS, FDIVS
	call	decode_template
    TEMPLATE \
	<TMPL_vfps_reg,TMPL_vfps_reg>,\			;0=sdn,sm
	<TMPL_vfps_reg,TMPL_vfps_reg,TMPL_vfps_reg>	;1=sd,sn,sm
    .v1xd_test:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_V1xD
	jz	ERROR_requires_copro_capability_vfp_v1xd
    .do:
	cmp	al,0
	jnz	.registers_shifted
	mov	eax,[operand_registers]
	mov	cl,al
	shl	eax,8
	mov	al,cl
	mov	[operand_registers],eax
    .registers_shifted:
	movzx	eax,[operand_register0]			;fd
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	eax,12
	shl	ecx,22
	or	ebp,eax
	or	ebp,ecx
	movzx	eax,[operand_register1]			;fn
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	eax,16
	shl	ecx,7
	or	ebp,eax
	or	ebp,ecx
	movzx	eax,[operand_register2]			;fm
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	ecx,5
	or	ebp,eax
	or	ebp,ecx
	jmp	ARM_post_process_copro

VFP_dd_dn_dm:
	;used by FMULD, FNMULD, FADDD, FSUBD, FDIVD
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpd_reg>,\			;0=ddn,dm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg>	;1=dd,dn,dm
    .begin:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_V1
	jz	ERROR_requires_copro_capability_vfp_v1
    .do:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_D32
	jnz	VFP_sd_sn_sm.do
	mov	edx,[operand_registers]			;fd,fn,fm
	test	edx,0x010101
	jnz	ERROR_requires_copro_capability_vfp_d32
	jmp	VFP_sd_sn_sm.do

VFP_sd_sm:
	;used by FABSS, FCPYS, FNEGS, FSITOS, FSQRTS, FTOSIS, FTOSIZS, FTOUIS, FTOUIZS, FUITOS
	call	decode_template
    TEMPLATE \
	<TMPL_vfps_reg,TMPL_vfps_reg>			;0=sd,sm
    .v1xd_test:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_V1xD
	jz	ERROR_requires_copro_capability_vfp_v1xd
    .do:
	movzx	eax,[operand_register0]			;fd
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	eax,12
	shl	ecx,22
	or	ebp,eax
	or	ebp,ecx
	movzx	eax,[operand_register1]			;fm
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	ecx,5
	or	ebp,eax
	or	ebp,ecx
	jmp	ARM_post_process_copro

VFP_dd_dm:
	;used by FABSD, FCPYD, FNEGD, FSQRTD
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpd_reg>			;0=dd,dm
    .encode:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_V1
	jz	ERROR_requires_copro_capability_vfp_v1
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_D32
	jnz	VFP_sd_sm.do
	mov	edx,[operand_registers]			;fd,fm
	test	edx,0x0101
	jnz	ERROR_requires_copro_capability_vfp_d32
	jmp	VFP_sd_sm.do

VFP_sd_sm_zero:
	;used by FCMPS, FCMPES
	call	decode_template
    TEMPLATE \
	<TMPL_vfps_reg,TMPL_vfps_reg>,\			;0=sd,sm
	<TMPL_vfps_reg,TMPL_imm>			;1=sd,0.0
	test	al,al
	jz	VFP_sd_sm.v1xd_test
	cmp	[immediate_value],0
	mov	ecx,ERROR_immediate_offset_out_of_range.0
	jnz	ARM_post_process_copro_with_error
	or	ebp,1 shl 16				;set for FCMP(E)Z
	jmp	VFP_sd_sm.v1xd_test

VFP_dd_dm_zero:
	;used by FCMPD, FCMPED
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpd_reg>,\			;0=dd,dm
	<TMPL_vfpd_reg,TMPL_imm>			;1=dd,0.0
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_V1
	jz	ERROR_requires_copro_capability_vfp_v1
	test	al,al
	jnz	.zero
    .check_d32:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_D32
	jnz	VFP_sd_sm.do
	mov	edx,[operand_registers]			;fd,fm
	test	edx,0x0101
	jnz	ERROR_requires_copro_capability_vfp_d32
	jmp	VFP_sd_sm.do
    .zero:
	cmp	[immediate_value],0
	mov	ecx,ERROR_immediate_offset_out_of_range.0
	jnz	ARM_post_process_copro_with_error
	or	ebp,1 shl 16				;set for FCMP(E)Z
	jmp	.check_d32

VFP_sd:
	;used by FCMPZS, FCMPEZS
	call	decode_template
    TEMPLATE \
	<TMPL_vfps_reg>					;0=sd
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_V1xD
	jz	ERROR_requires_copro_capability_vfp_v1
	mov	[operand_register1],0			;fm
	jmp	VFP_sd_sm.do

VFP_dd:
	;used by FCMPZD, FCMPEZD
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg>					;0=dd
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_V1
	jz	ERROR_requires_copro_capability_vfp_v1
	mov	[operand_register1],0		;fm
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_D32
	jnz	VFP_sd_sm.do
	mov	edx,[operand_registers]			;fd
	test	edx,0x01
	jnz	ERROR_requires_copro_capability_vfp_d32
	jmp	VFP_sd_sm.do

VFP_dd_sm:
	;used by FCVTDS, FSITOD, FUITOD
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfps_reg>			;0=dd,sm
    .do:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_V1
	jz	ERROR_requires_copro_capability_vfp_v1
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_D32
	jnz	VFP_sd_sm.do
	mov	edx,[operand_registers]			;fd
	test	edx,0x01
	jnz	ERROR_requires_copro_capability_vfp_d32
	jmp	VFP_sd_sm.do

VFP_sd_dm:
	;used by FCVTSD, FTOSID, FTOSIZD, FTOUID, FTOUIZD
	call	decode_template
    TEMPLATE \
	<TMPL_vfps_reg,TMPL_vfpd_reg>			;0=sd,dm
    .do:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_V1
	jz	ERROR_requires_copro_capability_vfp_v1
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_D32
	jnz	VFP_sd_sm.do
	mov	edx,[operand_registers]			;fm
	test	edx,0x0100
	jnz	ERROR_requires_copro_capability_vfp_d32
	jmp	VFP_sd_sm.do

VFP_sd_sm_HP:
	;used by VCVTB, VCVTT
	call	decode_template
    TEMPLATE \
	<TMPL_vfps_reg,TMPL_vfps_reg>			;0=sd,sm
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_HP
	jz	ERROR_requires_copro_capability_vfp_hp
	jmp	VFP_sd_sm.do

VFP_sd_rn_offset:
	;used by FLDS, FSTS
	call	decode_template
    TEMPLATE \
	<TMPL_vfps_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\			;0=sd,[rn]
	<TMPL_vfps_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\	;1=sd,[rn,imm]
	<TMPL_vfps_reg,TMPL_bracket_left,TMPL_address,TMPL_bracket_right>,\			;2=sd,[imm]	PC relative
	<TMPL_vfps_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>			;3=sd,[exp]	implicit reg from structure
    .do:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_V1xD
	jz	ERROR_requires_copro_capability_vfp_v1xd
	cmp	al,0
	jz	.fd_q_rn_p
	cmp	al,1
	jz	.fd_q_rn_imm_p
	cmp	al,2
	jz	.fd_q_imm_p
	cmp	al,3
	jz	.fd_q_exp_p
	ud2
    .fd_q_imm_p:
	mov	[operand_register1],0xf
	call	THUMB_query_condition_pc
	mov	eax,[addressing_space]
	sub	ecx,[eax+0x00]
	and	ecx,not 3				;round down
	neg	ecx
	add	ecx,[immediate_value]
	test	[code_type],CPU_ACTIVITY_ARM
	setnz	al
	movzx	eax,al
	shl	eax,2
	add	eax,4					;offset=4 for THUMB and 8 for ARM
	sub	ecx,eax
	mov	[immediate_value],ecx
    .fd_q_exp_p:
    .fd_q_rn_p:
    .fd_q_rn_imm_p:
	or	ebp,1 shl 23
	mov	eax,[immediate_value]
	test	eax,eax
	jns	.positive_imm
	and	ebp,not (1 shl 23)
	neg	eax
    .positive_imm:
	test	eax,3
	jnz	.out_of_range
	shr	eax,2
	cmp	eax,0xff
	ja	.out_of_range
	or	ebp,eax
	movzx	eax,[operand_register0]			;fd
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	eax,12
	shl	ecx,22
	or	ebp,eax
	or	ebp,ecx
	movzx	eax,[operand_register1]			;rn
	shl	eax,16
	or	ebp,eax
	jmp	ARM_post_process_copro
    .out_of_range:
	mov	ecx,ERROR_immediate_offset_out_of_range.0_0x3fc
	jmp	ARM_post_process_copro_with_error

VFP_dd_rn_offset:
	;used by FLDD, FSTD
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\			;0=dd,[rn]
	<TMPL_vfpd_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\	;1=dd,[rn,imm]
	<TMPL_vfpd_reg,TMPL_bracket_left,TMPL_address,TMPL_bracket_right>,\			;2=dd,[imm]	PC relative
	<TMPL_vfpd_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>			;3=dd,[exp]	implicit reg from structure
    .do:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_D32
	jnz	VFP_sd_rn_offset.do
	mov	edx,[operand_registers]			;fd
	test	edx,0x01
	jnz	ERROR_requires_copro_capability_vfp_d32
	jmp	VFP_sd_rn_offset.do

VFP_rn_list_s:
	;used by FLDMIAS, FLDMDBS, FSTMIAS, FSTMDBS
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_vfp_reg_list_s>,\		;0=rn,{..}
	<TMPL_base_reg!,TMPL_vfp_reg_list_s>		;1=rn!,{..}
    .do:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_V1xD
	jz	ERROR_requires_copro_capability_vfp_v1xd
	cmp	al,1					;writeback?
	jnz	.rn_okay
	cmp	[operand_register0],15			;PC?
	jz	ERROR_r15_not_valid.first
	or	ebp,1 shl 21
    .rn_okay:
	mov	eax,ebp
	and	eax,1 shl 24 + 1 shl 21			;isolate P and W
	xor	eax,1 shl 24				;P=!P
	jz	ERROR_must_have_writeback_operator	;P=1 & W=0 is FLD/FST, not valid
	mov	edx,[reg_list_bitmap]
	bsf	eax,edx
	bsr	ecx,edx
    ;check for a contiguous set
	xor	ecx,31
	shl	edx,cl
	add	ecx,eax
	sar	edx,cl
	inc	edx
	jnz	ERROR_non_contiguous_set
	mov	ecx,[reg_list_bitmap]
	bsf	eax,ecx					;eax=start register
	bsr	edx,ecx
	sub	edx,eax
	inc	edx					;edx=register count
	test	ebp,1 shl 8				;FLDMD?
	jz	.encode
	cmp	edx,16
	ja	ERROR_large_set
    ;double everything
	add	eax,eax
	add	edx,edx
    ;shift high bit to LSb
	mov	ah,al
	shr	ah,5
	and	ax,0x011e
	or	al,ah
	movzx	eax,al
    .encode:
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	eax,12
	shl	ecx,22
	or	ebp,eax
	or	ebp,ecx
	or	ebp,edx
	movzx	eax,[operand_register0]			;rn
	shl	eax,16
	or	ebp,eax
	jmp	ARM_post_process_copro

VFP_rn_list_d:
	;used by FLDMIAD, FLDMDBD, FLDMIAX, FLDMDBX, FSTMIAD, FSTMDBD, FSTMIAX, FSTMDBX
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_vfp_reg_list_d>,\		;0=rn,{..}
	<TMPL_base_reg!,TMPL_vfp_reg_list_d>		;1=rn!,{..}
    .do:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_D32
	jnz	VFP_rn_list_s.do
	mov	edx,[reg_list_bitmap]
	test	edx,0xffff0000
	jnz	ERROR_requires_copro_capability_vfp_d32
	jmp	VFP_rn_list_s.do

VFP_dn_rd:
	;used by FMDHR, FMDLR
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_base_reg>			;0=dn,rd
    .test_version:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_D32
	jnz	.do
	mov	edx,[operand_registers]			;dd
	test	edx,0x01
	jnz	ERROR_requires_copro_capability_vfp_d32
    .do:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_V1xD
	jz	ERROR_requires_copro_capability_vfp_v1xd
	movzx	eax,[operand_register0]			;dn
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	eax,16
	shl	ecx,7
	or	ebp,ecx
	or	ebp,eax
	movzx	eax,[operand_register1]			;rd
	cmp	eax,15					;PC?
	jz	ERROR_r15_not_valid.second
	shl	eax,12
	or	ebp,eax
	jmp	ARM_post_process_copro

VFP_rd_dn:
	;used by FMRDH, FMRDL
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_vfpd_reg>			;0=rd,dn
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_D32
	jnz	.do
	mov	edx,[operand_registers]			;dd
	test	edx,0x0100
	jnz	ERROR_requires_copro_capability_vfp_d32
    .do:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_V1xD
	jz	ERROR_requires_copro_capability_vfp_v1xd
    .encode:
	movzx	eax,[operand_register0]			;rd
	cmp	eax,15					;PC?
	jz	ERROR_r15_not_valid.first
	and	eax,0xf
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;dn
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	eax,16
	shl	ecx,7
	or	ebp,ecx
	or	ebp,eax
	jmp	ARM_post_process_copro

VFP_rd_sn:
	;used by FMRS
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_vfps_reg>			;0=rd,sn
	jmp	VFP_rd_dn.do

VFP_sn_rd:
	;used by FMSR
	call	decode_template
    TEMPLATE \
	<TMPL_vfps_reg,TMPL_base_reg>			;0=sn,rd
    .do:
	mov	ax,word[operand_register0]
	xchg	ah,al
	mov	word[operand_register0],ax
	jmp	VFP_rd_dn.do

VFP_rd_sysreg:
	;used by FMRX
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_vfp_syst>			;0=rd,sysreg
	mov	ax,word[operand_registers+0]
	shl	ah,1					;set sysreg encoding
	cmp	ah,1 shl 1				;fpscr?
	setz	cl
	shl	cl,4
	or	al,cl					;disable r15 check, r15 is okay here if reading fpscr
	mov	word[operand_registers+0],ax
	jmp	VFP_rd_dn.do

VFP_sysreg_rd:
	;used by FMXR
	call	decode_template
    TEMPLATE \
	<TMPL_vfp_syst,TMPL_base_reg>			;0=sysreg,rd
	mov	ax,word[operand_registers+0]
	xchg	ah,al
	shl	ah,1					;set sysreg encoding
	mov	word[operand_registers+0],ax
	jmp	VFP_rd_dn.do

;vfp2

VFP_dm_rd_rn:
	;used by FMDRR
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_base_reg,TMPL_base_reg>	;0=dm,rd,rn
    .test_version:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_D32
	jnz	.do
	mov	edx,[operand_registers]			;dd
	test	edx,0x01
	jnz	ERROR_requires_copro_capability_vfp_d32
    .do:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_V2
	jz	ERROR_requires_copro_capability_vfp_v2
	movzx	eax,[operand_register0]			;dm
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	ecx,5
	or	ebp,ecx
	or	ebp,eax
	movzx	eax,[operand_register1]			;rd
	cmp	eax,15					;PC?
	jz	ERROR_r15_not_valid.second
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register2]			;rn
	cmp	eax,15					;PC?
	jz	ERROR_r15_not_valid.third
	shl	eax,16
	or	ebp,eax
	jmp	ARM_post_process_copro

VFP_rd_rn_dm:
	;used by FMRRD
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg,TMPL_vfpd_reg>	;0=rd,rn,dm
    .test_version:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_D32
	jnz	.do
	mov	edx,[operand_registers]			;dd
	test	edx,0x010000
	jnz	ERROR_requires_copro_capability_vfp_d32
    .do:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_V2
	jz	ERROR_requires_copro_capability_vfp_v2
	movzx	eax,[operand_register0]			;rd
	cmp	eax,15					;PC?
	jz	ERROR_r15_not_valid.first
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;rn
	cmp	eax,15					;PC?
	jz	ERROR_r15_not_valid.second
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]			;dm
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	ecx,5
	or	ebp,ecx
	or	ebp,eax
	jmp	ARM_post_process_copro

VFP_rd_rn_sm:
	;used by FMRRS
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg,TMPL_vfp_reg_list_s>	;0=rd,rn,{..}
    .do:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_V2
	jz	ERROR_requires_copro_capability_vfp_v2
	movzx	eax,[operand_register0]			;rd
	cmp	eax,15					;PC?
	jz	ERROR_r15_not_valid.first
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;rn
	cmp	eax,15					;PC?
	jz	ERROR_r15_not_valid.second
	shl	eax,16
	or	ebp,eax
	mov	ecx,[reg_list_bitmap]
	bsf	eax,ecx					;eax=start register
	bsr	edx,ecx
    ;check for a 2 register set
	dec	edx
	cmp	edx,eax
	jnz	ERROR_two_registers_required
	cmp	eax,30
	ja	ERROR_two_registers_required
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	ecx,5
	or	ebp,ecx
	or	ebp,eax
	jmp	ARM_post_process_copro

VFP_sm_rd_rn:
	;used by FMSRR
	call	decode_template
    TEMPLATE \
	<TMPL_vfp_reg_list_s,TMPL_comma,TMPL_base_reg,TMPL_base_reg>	;0={..},rd,rn
	jmp	VFP_rd_rn_sm.do

VFP_fmstat:
	;used by FMSTAT
	call	decode_template
    TEMPLATE \
	<TMPL_EOL>					;0=empty
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_V1xD
	jz	ERROR_requires_copro_capability_vfp_v1xd
	jmp	ARM_post_process_copro

VFP_convert_single_to_quarter:
	mov	eax,[immediate_value]
	mov	ecx,eax
	mov	edx,eax
	and	eax,1 shl 23-1				;eax=mantissa
	shr	ecx,23
	and	ecx,0xff				;ecx=exponent
	add	eax,1 shl 18
	shr	eax,19
	cmp	eax,0xf
	jbe	.rounding_done
	xor	eax,eax
	inc	ecx
    .rounding_done:
	cmp	cl,01111100b
	jb	.out_of_range
	cmp	cl,10000011b
	ja	.out_of_range
	shr	edx,31-7
	and	edx,0x80				;dl=sign
	and	ecx,7
	shl	ecx,4
	or	edx,ecx
	or	eax,edx
	mov	[immediate_value],eax
	ret
    .out_of_range:
	or	eax,-1
	mov	[immediate_value],eax
	ret

VFP_dm_imm:
	;used by FCONSTD
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_imm_float>,\		;0=dd,float
	<TMPL_vfpd_reg,TMPL_imm>			;1=dd,imm
    .vesion_test:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_D32
	jnz	.do
	mov	edx,[operand_registers]			;dd
	test	edx,0x01
	jnz	ERROR_requires_copro_capability_vfp_d32
    .do:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_V3
	jz	ERROR_requires_copro_capability_vfp_v3
	cmp	al,1
	jz	.constant_converted
	call	VFP_convert_single_to_quarter
    .constant_converted:
	movzx	eax,[operand_register0]			;dd
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	eax,12
	shl	ecx,22
	or	ebp,ecx
	or	ebp,eax
	mov	eax,[immediate_value]
	cmp	eax,0xff
	ja	.out_of_range
	mov	ecx,eax
	and	eax,0xf
	and	ecx,0xf0
	shl	ecx,16-4
	or	ebp,eax
	or	ebp,ecx
	jmp	ARM_post_process_copro
    .out_of_range:
	mov	ecx,ERROR_immediate_offset_out_of_range.0_0xff
	jmp	ARM_post_process_copro_with_error

VFP_sm_imm:
	;used by FCONSTS
	call	decode_template
    TEMPLATE \
	<TMPL_vfps_reg,TMPL_imm_float>,\		;0=sd,float
	<TMPL_vfps_reg,TMPL_imm>			;1=sd,imm
	jmp	VFP_dm_imm.do

VFP_dd_dd_imm:
	;used by FSHTOD, FSLTOD, FUHTOD, FULTOD, FTOSHD, FTOSLD, FTOUHD, FTOULD
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_imm>,\			;0=dd,imm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_imm>		;1=dd,dd,imm
    .test_version:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_D32
	jnz	.do
	mov	edx,[operand_registers]			;dd
	test	edx,0x0101
	jnz	ERROR_requires_copro_capability_vfp_d32
    .do:
	shr	al,1
	jc	.register_okay
	mov	cl,[operand_register0]
	mov	[operand_register1],cl
    .register_okay:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_V3
	jz	ERROR_requires_copro_capability_vfp_v3
	movzx	eax,word[operand_register0]		;dd,dd
	cmp	ah,al
	jnz	ERROR_dest_and_source_must_be_the_same
	mov	ah,0
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	eax,12
	shl	ecx,22
	or	ebp,ecx
	or	ebp,eax
	mov	ecx,[immediate_value]
	test	ebp,1 shl 7
	jz	.half
    ;long
	cmp	ecx,1
	jb	.out_of_range_long
	cmp	ecx,32
	ja	.out_of_range_long
	mov	eax,32
	jmp	.encode
    .half:
	cmp	ecx,16
	ja	.out_of_range_half
	mov	eax,16
    .encode:
	sub	eax,ecx
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	ecx,5
	or	ebp,ecx
	or	ebp,eax
	jmp	ARM_post_process_copro
    .out_of_range_long:
	mov	ecx,ERROR_immediate_value_out_of_range.1_32
	jmp	ARM_post_process_copro_with_error
    .out_of_range_half:
	mov	ecx,ERROR_immediate_value_out_of_range.0_16
	jmp	ARM_post_process_copro_with_error

VFP_sd_sd_imm:
	;used by FSHTOS, FSLTOS, FUHTOS, FULTOS, FTOSHS, FTOSLS, FTOUHS, FTOULS
	call	decode_template
    TEMPLATE \
	<TMPL_vfps_reg,TMPL_imm>,\			;0=sd,imm
	<TMPL_vfps_reg,TMPL_vfps_reg,TMPL_imm>		;1=sd,sd,imm
	jmp	VFP_dd_dd_imm.do

;xScale

XSCALE_acc_rm_rs:
	;used by MIA, MIAPH, MIABB, MIABT, MIATB, MIATT
	call	decode_template
    TEMPLATE \
	<TMPL_acc_40bt,TMPL_base_reg,TMPL_base_reg>	;0=acc,rm,rs
	test	[copro_capability_flags],COPRO_CAPABILITY_XSCALE
	jz	ERROR_requires_copro_capability_xscale
	movzx	eax,[operand_register0]			;acc
	shl	eax,5
	or	ebp,eax
	movzx	eax,[operand_register1]			;rm
	cmp	eax,15					;PC?
	jz	ERROR_r15_not_valid.second
	or	ebp,eax
	movzx	eax,[operand_register2]			;rs
	cmp	eax,15					;PC?
	jz	ERROR_r15_not_valid.third
	shl	eax,12
	or	ebp,eax
	jmp	ARM_post_process_copro

XSCALE_acc_rdlo_rdhi:
	;used by MAR
	call	decode_template
    TEMPLATE \
	<TMPL_acc_40bt,TMPL_base_reg,TMPL_base_reg>	;0=acc,rdlo,rdhi
    .do:
	test	[copro_capability_flags],COPRO_CAPABILITY_XSCALE
	jz	ERROR_requires_copro_capability_xscale
	movzx	eax,[operand_register0]			;acc
	or	ebp,eax
	movzx	eax,[operand_register1]			;rm
	cmp	eax,15					;PC?
	jz	ERROR_r15_not_valid.second
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register2]			;rs
	cmp	eax,15					;PC?
	jz	ERROR_r15_not_valid.third
	shl	eax,16
	or	ebp,eax
	jmp	ARM_post_process_copro

XSCALE_rdlo_rdhi_acc:
	;used by MRA
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg,TMPL_acc_40bt>	;0=rdlo,rdhi,acc
	test	[copro_capability_flags],COPRO_CAPABILITY_XSCALE
	jz	ERROR_requires_copro_capability_xscale
	mov	eax,[operand_registers]
	mov	ecx,eax
	shl	eax,8
	shr	ecx,16
	mov	al,cl
	mov	[operand_registers],eax
	jmp	XSCALE_acc_rdlo_rdhi.do

;IWMMXT V1

IWMMXT_r15:
	;used by TANDCB, TANDCH, TANDCW, TORCB, TORCH, TORCW
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg>					;0=r15
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V1
	jz	ERROR_requires_copro_capability_iwmmxt_v1
	movzx	eax,[operand_register0]			;r15
	cmp	eax,15					;PC?
	jnz	ERROR_must_be_r15.first
	jmp	ARM_post_process_copro

IWMMXT_wrd_rn:
	;used by TBCSTB, TBCSTH, TBCSTW
	call	decode_template
    TEMPLATE \
	<TMPL_iwmmx_wreg,TMPL_base_reg>			;0=wrd,rn
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V1
	jz	ERROR_requires_copro_capability_iwmmxt_v1
	movzx	eax,[operand_register0]			;wrd
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register1]			;rn
	shl	eax,12
	or	ebp,eax
	jmp	ARM_post_process_copro

IWMMXT_r15_imm:
	;used by TEXTRCB, TEXTRCH, TEXTRCW
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_imm>			;0=r15,imm
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V1
	jz	ERROR_requires_copro_capability_iwmmxt_v1
	movzx	eax,[operand_register0]			;r15
	cmp	eax,15					;PC?
	jnz	ERROR_must_be_r15.first
	mov	eax,[immediate_value]
	cmp	eax,7
	ja	.out_of_range
	or	ebp,eax
	jmp	ARM_post_process_copro
    .out_of_range:
	mov	ecx,ERROR_immediate_value_out_of_range.0_7
	jmp	ARM_post_process_copro_with_error

IWMMXT_rd_wrn_imm:
	;used by TEXTRMSB, TEXTRMSH, TEXTRMSW, TEXTRMUB, TEXTRMUH, TEXTRMUW
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_iwmmx_wreg,TMPL_imm>	;0=rd,wrn,imm
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V1
	jz	ERROR_requires_copro_capability_iwmmxt_v1
	movzx	eax,[operand_register0]			;rd
	cmp	eax,15					;PC?
	jz	ERROR_r15_not_valid.first
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;wrn
	shl	eax,16
	or	ebp,eax
	mov	eax,[immediate_value]
	cmp	eax,7
	ja	.out_of_range
	or	ebp,eax
	jmp	ARM_post_process_copro
    .out_of_range:
	mov	ecx,ERROR_immediate_value_out_of_range.0_7
	jmp	ARM_post_process_copro_with_error

IWMMXT_wrd_rn_imm:
	;used by TINSRB, TINSRH, TINSRW
	call	decode_template
    TEMPLATE \
	<TMPL_iwmmx_wreg,TMPL_base_reg,TMPL_imm>	;0=wrd,rn,imm
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V1
	jz	ERROR_requires_copro_capability_iwmmxt_v1
	movzx	eax,[operand_register0]			;wrd
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register1]			;rn
	shl	eax,12
	or	ebp,eax
	mov	eax,[immediate_value]
	cmp	eax,7
	ja	.out_of_range
	or	ebp,eax
	jmp	ARM_post_process_copro
    .out_of_range:
	mov	ecx,ERROR_immediate_value_out_of_range.0_7
	jmp	ARM_post_process_copro_with_error

IWMMXT_wcd_rn:
	;used by TMCR
	call	decode_template
    TEMPLATE \
	<TMPL_iwmmx_creg,TMPL_base_reg>			;0=wcd,rn
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V1
	jz	ERROR_requires_copro_capability_iwmmxt_v1
	movzx	eax,[operand_register0]			;wcd
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register1]			;rn
	shl	eax,12
	or	ebp,eax
	jmp	ARM_post_process_copro

IWMMXT_wrd_rdlo_rdhi:
	;used by TMCRR
	call	decode_template
    TEMPLATE \
	<TMPL_iwmmx_wreg,TMPL_base_reg,TMPL_base_reg>	;0=wrd,rdlo,rdhi
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V1
	jz	ERROR_requires_copro_capability_iwmmxt_v1
	movzx	eax,[operand_register0]			;wrd
	or	ebp,eax
	movzx	eax,[operand_register1]			;rdlo
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register2]			;rdhi
	shl	eax,16
	or	ebp,eax
	jmp	ARM_post_process_copro

IWMMXT_wrd_rm_rs:
	;used by TMIA, TMIAPH, TMIAxy
	call	decode_template
    TEMPLATE \
	<TMPL_iwmmx_wreg,TMPL_base_reg,TMPL_base_reg>	;0=wrd,rm,rs
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V1
	jz	ERROR_requires_copro_capability_iwmmxt_v1
	movzx	eax,[operand_register0]			;wrd
	shl	eax,5
	or	ebp,eax
	movzx	eax,[operand_register1]			;rm
	or	ebp,eax
	movzx	eax,[operand_register2]			;rs
	shl	eax,12
	or	ebp,eax
	jmp	ARM_post_process_copro

IWMMXT_rd_wrn:
	;used by TMOVMSKB, TMOVMSKH, TMOVMSKW
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_iwmmx_wreg>			;0=rd,wrn
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V1
	jz	ERROR_requires_copro_capability_iwmmxt_v1
	movzx	eax,[operand_register0]			;rd
	cmp	eax,15					;PC?
	jz	ERROR_r15_not_valid.first
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;wrn
	shl	eax,16
	or	ebp,eax
	jmp	ARM_post_process_copro

IWMMXT_rd_wcn:
	;used by TMRC
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_iwmmx_creg>			;0=rd,wcn
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V1
	jz	ERROR_requires_copro_capability_iwmmxt_v1
	movzx	eax,[operand_register0]			;rd
	cmp	eax,15					;PC?
	jz	ERROR_r15_not_valid.first
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;wcn
	shl	eax,16
	or	ebp,eax
	jmp	ARM_post_process_copro

IWMMXT_rdlo_rdhi_wrn:
	;used by TMRRC
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_base_reg,TMPL_iwmmx_wreg>	;0=rdlo,rdhi,wrn
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V1
	jz	ERROR_requires_copro_capability_iwmmxt_v1
	movzx	eax,word[operand_register0]
	cmp	ah,al
	jz	ERROR_destination_registers_must_differ
	movzx	eax,[operand_register0]			;rdlo
	cmp	eax,15					;PC?
	jz	ERROR_r15_not_valid.first
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;rdhi
	cmp	eax,15					;PC?
	jz	ERROR_r15_not_valid.second
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]			;wrn
	or	ebp,eax
	jmp	ARM_post_process_copro

IWMMXT_wrd_wrn:
	;used by WACCB, WACCH, WACCW, WUNPCKEHUB, WUNPCKEHUH, WUNPCKEHUW, WUNPCKEHSB, WUNPCKEHSH, WUNPCKEHSW
	;	 WUNPCKELUB, WUNPCKELUH, WUNPCKELUW, WUNPCKELSB, WUNPCKELSH, WUNPCKELSW
	call	decode_template
    TEMPLATE \
	<TMPL_iwmmx_wreg,TMPL_iwmmx_wreg>		;0=wrd,wrn
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V1
	jz	ERROR_requires_copro_capability_iwmmxt_v1
	movzx	eax,[operand_register0]			;wrd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;wrn
	shl	eax,16
	or	ebp,eax
	jmp	ARM_post_process_copro

IWMMXT_wrd_wrn_wrm:
	;used by WADDB, WADDH, WADDW, WADDBSS, WADDHSS, WADDWSS, WADDBUS, WADDHUS, WADDWUS,
	;	 WALIGNR0, WALIGNR1, WALIGNR2, WALIGNR3, WAND, WANDN, WAVG2B, WAVG2H, WAVG2BR, WAVG2HR,
	;	 WCMPEQB, WCMPEQH, WCMPEQW, WCMPGTSB, WCMPGTSH, WCMPGTSW, WCMPGTUB, WCMPGTUH, WCMPGTUW,
	;	 WMACS, WMACU, WMACSZ, WMACUZ, WMADDU, WMADDS, WMAXUB, WMAXUH, WMAXUW, WMAXSB, WMAXSH, WMAXSW,
	;	 WMINUB, WMINUH, WMINUW, WMINSB, WMINSH, WMINSW, WMULSM, WMULSL, WMULUM, WMULUL,
	;	 WPACKDSS, WPACKDUS, WPACKHSS, WPACKHUS, WPACKWSS, WPACKWUS, WSADB, WSADH, WSADBZ, WSADHZ,
	;	 WSUBB, WSUBH, WSUBW, WSUBBSS, WSUBHSS, WSUBWSS, WSUBBUS, WSUBHUS, WSUBWUS,
	;	 WUNPCKIHB, WUNPCKIHH, WUNPCKIHW, WUNPCKILB, WUNPCKILH, WUNPCKILW, WXOR
	call	decode_template
    TEMPLATE \
	<TMPL_iwmmx_wreg,TMPL_iwmmx_wreg,TMPL_iwmmx_wreg>	;0=wrd,wrn,wrm
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V1
	jz	ERROR_requires_copro_capability_iwmmxt_v1
	movzx	eax,[operand_register0]			;wrd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;wrn
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]			;wrm
	or	ebp,eax
	jmp	ARM_post_process_copro

IWMMXT_wrd_wrn_wrm_imm:
	;used by WALIGNI
	call	decode_template
    TEMPLATE \
	<TMPL_iwmmx_wreg,TMPL_iwmmx_wreg,TMPL_iwmmx_wreg,TMPL_imm>	;0=wrd,wrn,wrm,imm
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V1
	jz	ERROR_requires_copro_capability_iwmmxt_v1
	movzx	eax,[operand_register0]			;wrd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;wrn
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]			;wrm
	or	ebp,eax
	mov	eax,[immediate_value]
	cmp	eax,7
	ja	.out_of_range
	shl	eax,20
	or	ebp,eax
	jmp	ARM_post_process_copro
    .out_of_range:
	mov	ecx,ERROR_immediate_value_out_of_range.0_7
	jmp	ARM_post_process_copro_with_error

IWMMXT_wrd_address5:
	;used by WLDRB, WLDRH, WLDRW, WSTRB, WSTRH, WSTRW
	call	decode_template
    TEMPLATE \
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\				; 0=wrd,[rn]
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_imm>,\			; 1=wrd,[rn],imm
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_option>,\			; 2=wrd,[rn],{imm}
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\			; 3=wrd,[rn,imm]
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right,TMPL_modifier_exclaim>,\; 4=wrd,[rn,imm]!
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_address,TMPL_bracket_right>,\					; 5=wrd,[imm]	PC relative
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>,\				; 6=wrd,[exp]	implicit reg from structure
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right,TMPL_modifier_exclaim>,\		; 7=wrd,[exp]!	implicit reg from structure
	<TMPL_iwmmx_creg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\				; 8=crd,[rn]
	<TMPL_iwmmx_creg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_imm>,\			; 9=crd,[rn],imm
	<TMPL_iwmmx_creg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_option>,\			;10=crd,[rn],{imm}
	<TMPL_iwmmx_creg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\			;11=crd,[rn,imm]
	<TMPL_iwmmx_creg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right,TMPL_modifier_exclaim>,\;12=crd,[rn,imm]!
	<TMPL_iwmmx_creg,TMPL_bracket_left,TMPL_address,TMPL_bracket_right>,\					;13=crd,[imm]	PC relative
	<TMPL_iwmmx_creg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>,\				;14=crd,[exp]	implicit reg from structure
	<TMPL_iwmmx_creg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right,TMPL_modifier_exclaim>		;15=crd,[exp]!	implicit reg from structure
    .do:
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V1
	jz	ERROR_requires_copro_capability_iwmmxt_v1
	cmp	al,8					;wcr?
	jb	.wcr_okay
    ;check for W mode
	mov	ecx,ebp
	and	ecx,1 shl 22 + 1 shl 8
	cmp	ecx,1 shl 8
	jnz	ERROR_control_register_only_valid_with_word
	sub	al,8
	or	ebp,0xf shl 28
    .wcr_okay:
	cmp	al,5					;wrd,[imm]?
	jnz	.offset_okay
	mov	[operand_register1],0xf
	call	THUMB_query_condition_pc
	mov	eax,[addressing_space]
	sub	ecx,[eax+0x00]
	and	ecx,not 3				;round down
	neg	ecx
	add	ecx,[immediate_value]
	test	[code_type],CPU_ACTIVITY_ARM
	setnz	al
	movzx	eax,al
	shl	eax,2
	add	eax,4					;offset=4 for THUMB and 8 for ARM
	sub	ecx,eax
	mov	[immediate_value],ecx
	mov	eax,3					;wrd,[rn,imm]
    .offset_okay:
	test	ebp,1 shl 8				;WLDRB/WLDRH?
	jnz	.imm_adjusted
	cmp	al,2					;option?
	jz	.imm_adjusted
	shl	[immediate_value],2
    .imm_adjusted:
	mov	ecx,[operand_registers]
	shl	ecx,8
	mov	[operand_registers],ecx
	jmp	ARM_copro_crd_address5.version_check_okay

IWMMXT_wrd_wrn_WMOV:
	;used by WMOV
	call	decode_template
    TEMPLATE \
	<TMPL_iwmmx_wreg,TMPL_iwmmx_wreg>		;0=wrd,wrn
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V1
	jz	ERROR_requires_copro_capability_iwmmxt_v1
	movzx	eax,[operand_register0]			;wrd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;wrn
	or	ebp,eax
	shl	eax,16
	or	ebp,eax
	jmp	ARM_post_process_copro

IWMMXT_wrd_wrn_wcm:
	;used by  WRORHG, WRORWG, WRORDG, WSLLHG, WSLLWG, WSLLDG, WSRAHG, WSRAWG, WSRADG, WSRLHG, WSRLWG, WSRLDG
	call	decode_template
    TEMPLATE \
	<TMPL_iwmmx_wreg,TMPL_iwmmx_wreg,TMPL_iwmmx_creg>	;0=wrd,wrn,wcm
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V1
	jz	ERROR_requires_copro_capability_iwmmxt_v1
	movzx	eax,[operand_register0]			;wrd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;wrn
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]			;wcm
	or	ebp,eax
	jmp	ARM_post_process_copro

IWMMXT_wrd_wrn_imm:
	;used by WSHUFH
	call	decode_template
    TEMPLATE \
	<TMPL_iwmmx_wreg,TMPL_iwmmx_wreg,TMPL_imm>	;0=wrd,wrn,imm
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V1
	jz	ERROR_requires_copro_capability_iwmmxt_v1
	movzx	eax,[operand_register0]			;wrd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;wrn
	shl	eax,16
	or	ebp,eax
	mov	eax,[immediate_value]
	cmp	eax,0xff
	ja	.out_of_range
	mov	ecx,eax
	and	eax,0xf
	and	ecx,0xf0
	shl	ecx,20-4
	or	ebp,eax
	or	ebp,ecx
	jmp	ARM_post_process_copro
    .out_of_range:
	mov	ecx,ERROR_immediate_value_out_of_range.0_0xff
	jmp	ARM_post_process_copro_with_error

IWMMXT_wrd:
	;used by WZERO
	call	decode_template
    TEMPLATE \
	<TMPL_iwmmx_wreg>				;0=wrd
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V1
	jz	ERROR_requires_copro_capability_iwmmxt_v1
	movzx	eax,[operand_register0]			;wrd
	or	ebp,eax
	shl	eax,12
	or	ebp,eax
	shl	eax,4
	or	ebp,eax
	jmp	ARM_post_process_copro

;IWMMXT V2

IWMMXT_r15_v2:
	;used by TORVSCB, TORVSCH, TORVSCW
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg>					;0=r15
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V2
	jz	ERROR_requires_copro_capability_iwmmxt_v2
	movzx	eax,[operand_register0]			;r15
	cmp	eax,15					;PC?
	jnz	ERROR_must_be_r15.first
	jmp	ARM_post_process_copro

IWMMXT_rd_rn_rm_imm:
	;used by WMERGE
	call	decode_template
    TEMPLATE \
	<TMPL_iwmmx_wreg,TMPL_iwmmx_wreg,TMPL_iwmmx_wreg,TMPL_imm>	;0=wrd,wrn,wrm,imm
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V2
	jz	ERROR_requires_copro_capability_iwmmxt_v2
	movzx	eax,[operand_register0]			;wrd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;wrn
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]			;wrm
	or	ebp,eax
	mov	eax,[immediate_value]
	cmp	eax,7
	ja	.out_of_range
	shl	eax,21
	or	ebp,eax
	jmp	ARM_post_process_copro
    .out_of_range:
	mov	ecx,ERROR_immediate_value_out_of_range.0_7
	jmp	ARM_post_process_copro_with_error

IWMMXT_rd_rn_v2:
	;used by WABSB, WABSH, WABSW
	call	decode_template
    TEMPLATE \
	<TMPL_iwmmx_wreg,TMPL_iwmmx_wreg>		;0=wrd,wrn
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V2
	jz	ERROR_requires_copro_capability_iwmmxt_v2
	movzx	eax,[operand_register0]			;wrd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;wrn
	shl	eax,16
	or	ebp,eax
	jmp	ARM_post_process_copro

IWMMXT_rd_rn_rm_v2:
	;used by WABSDIFFB, WABSDIFFH, WABSDIFFW, WADDBHUSL, WADDBHUSM, WADDHC, WADDSUBHX, WADDWC,
	;	 WAVG4, WAVG4R, WMADDSN, WMADDSX, WMADDUN, WMADDUX, WMIABB, WMIABBN, WMIABT, WMIABTN,
	;	 WMIATB, WMIATBN, WMIATT, WMIATTN, WMIAWBB, WMIAWBBN, WMIAWBT, WMIAWBTN, WMIAWTB,
	;	 WMIAWTBN, WMIAWTT, WMIAWTTN, WMULSMR, WMULUMR, WMULWL, WMULWSM, WMULWSMR, WMULWUM,
	;	 WMULWUMR, WQMIABB, WQMIABBN, WQMIABT, WQMIABTN, WQMIATB, WQMIATBN, WQMIATT, WQMIATTN,
	;	 WQMULM, WQMULMR, WQMULWM, WQMULWMR, WSUBADDHX
	call	decode_template
    TEMPLATE \
	<TMPL_iwmmx_wreg,TMPL_iwmmx_wreg,TMPL_iwmmx_wreg>	;0=wrd,wrn,wrm
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V2
	jz	ERROR_requires_copro_capability_iwmmxt_v2
	movzx	eax,[operand_register0]			;wrd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;wrn
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]			;wrm
	or	ebp,eax
	jmp	ARM_post_process_copro

IWMMXT_wrd_wrn_param:
	;used by WSLLH, WSLLW, WSLLD, WSRAH, WSRAW, WSRAD, WSRLH, WSRLW, WSRLD, WRORH, WRORW, WRORD
	call	decode_template
    TEMPLATE \
	<TMPL_iwmmx_wreg,TMPL_iwmmx_wreg,TMPL_iwmmx_wreg>,\	;0=wrd,wrn,wrm
	<TMPL_iwmmx_wreg,TMPL_iwmmx_wreg,TMPL_imm>		;1=wrd,wrn,imm
	cmp	al,1
	jz	.wrd_wrn_imm
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V1
	jz	ERROR_requires_copro_capability_iwmmxt_v1
	movzx	eax,[operand_register0]			;wrd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;wrn
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]			;wrm
	or	ebp,eax
	jmp	ARM_post_process_copro
    .wrd_wrn_imm:
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V2
	jz	ERROR_requires_copro_capability_iwmmxt_v2
	or	ebp,0xf shl 28
	movzx	eax,[operand_register0]			;wrd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;wrn
	shl	eax,16
	or	ebp,eax
	mov	eax,[immediate_value]
	cmp	eax,32
	ja	.out_of_range
	cmp	eax,0
	jz	.shift_0
	mov	ecx,eax
	and	ecx,0x10
	and	eax,0x0f
	shl	ecx,8-4
	or	ebp,eax
	or	ebp,ecx
	jmp	ARM_post_process_copro
    .shift_0:
	or	ebp,3 shl 20				;force to WROR
	mov	ecx,ebp
	and	ecx,3 shl 22
	cmp	ecx,1 shl 22				;H?
	jz	.shift_0_H
	cmp	ecx,2 shl 22				;W?
	jz	ARM_post_process_copro			;WRORW wrd,wrn,32
    ;shift 0 D
	and	ebp,not (0x10f00ff0)			;force to WOR
	movzx	eax,[operand_register1]			;wrn
	or	ebp,eax					;WOR wrd,wrn,wrn
	jmp	ARM_post_process_copro
    .shift_0_H:
	or	ebp,1 shl 8				;WRORH wrd,wrn,16
	jmp	ARM_post_process_copro
    .out_of_range:
	mov	ecx,ERROR_shift_value_out_of_range.0_32
	jmp	ARM_post_process_copro_with_error

IWMMXT_wrd_address5_reg_offset_translate:
	db	0	;0	0=wrd,[rn]
	db	-1	;1
	db	-1	;2
	db	1	;3	1=wrd,[rn],imm
	db	2	;4	2=wrd,[rn],{imm}
	db	3	;5	3=wrd,[rn,imm]
	db	4	;6	4=wrd,[rn,imm]!
	db	-1	;7
	db	-1	;8
	db	-1	;9
	db	-1	;10
	db	5	;11	5=wrd,[imm]	PC relative
	db	6	;12	6=wrd,[exp]	implicit reg from structure
	db	7	;13	7=wrd,[exp]!	implicit reg from structure

IWMMXT_wrd_address5_reg_offset:
	;used by WLDRD, WSTRD
	call	decode_template
    TEMPLATE \
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\								; 0=wrd,[rn]
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_add_sub_reg>,\						; 1=wrd,[rn],+-rm
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_add_sub_reg,TMPL_shift_op,TMPL_imm>,\			; 2=wrd,[rn],+-rm,lsl imm
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_imm>,\							; 3=wrd,[rn],imm
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right,TMPL_option>,\							; 4=wrd,[rn],{imm}
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\							; 5=wrd,[rn,imm]
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right,TMPL_modifier_exclaim>,\				; 6=wrd,[rn,imm]!
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_bracket_right>,\						; 7=wrd,[rn,+-rm]
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_bracket_right,TMPL_modifier_exclaim>,\			; 8=wrd,[rn,+-rm]!
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_shift_op,TMPL_imm,TMPL_bracket_right>,\			; 9=wrd,[rn,+-rm,lsl imm]
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_address_reg,TMPL_add_sub_reg,TMPL_shift_op,TMPL_imm,TMPL_bracket_right,TMPL_modifier_exclaim>,\	;10=wrd,[rn,+-rm,lsl imm]!
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_address,TMPL_bracket_right>,\									;11=wrd,[imm]	PC relative
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>,\								;12=wrd,[exp]	implicit reg from structure
	<TMPL_iwmmx_wreg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right,TMPL_modifier_exclaim>						;13=wrd,[exp]!	implicit reg from structure
	mov	cl,al
	movzx	eax,al
	mov	al,[eax+IWMMXT_wrd_address5_reg_offset_translate]
	cmp	al,-1
	jnz	IWMMXT_wrd_address5.do
	test	[copro_capability_flags],COPRO_CAPABILITY_IWMMXT_V2
	jz	ERROR_requires_copro_capability_iwmmxt_v2
	or	ebp,0xf shl 28
	mov	al,[operand_register2]			;rm
	movzx	edx,al
	not	edx
	and	edx,0x80				;get '+' state
	shl	edx,23-7
	or	ebp,edx					;set U if '+'
	and	al,0xf
	cmp	al,0xf
	jz	ERROR_r15_not_valid.third
	mov	[operand_register2],al			;rm
	mov	eax,[immediate_value]
	cmp	eax,15
	ja	.out_of_range
	cmp	cl,1
	jz	.wrd_q_rn_p_PMrm
	cmp	cl,2
	jz	.wrd_q_rn_p_PMrm_lsl_imm
	cmp	cl,7
	jz	.wrd_q_rn_PMrm_p
	cmp	cl,8
	jz	.wrd_q_rn_PMrm_p!
	cmp	cl,9
	jz	.wrd_q_rn_PMrm_lsl_imm_p
	cmp	cl,10
	jz	.wrd_q_rn_PMrm_lsl_imm_p!
	ud2
    .wrd_q_rn_p_PMrm_lsl_imm:
	cmp	[instruction_shift_op],ARM_SHIFT_OPCODE_LSL
	jnz	ERROR_shift_must_be_LSL
    .wrd_q_rn_p_PMrm:
	or	ebp,1 shl 21				;set W
	jmp	.encode
    .wrd_q_rn_PMrm_lsl_imm_p!:
	cmp	[instruction_shift_op],ARM_SHIFT_OPCODE_LSL
	jnz	ERROR_shift_must_be_LSL
    .wrd_q_rn_PMrm_p!:
	or	ebp,1 shl 21 + 1 shl 24			;set W & P
	jmp	.encode
    .wrd_q_rn_PMrm_lsl_imm_p:
	cmp	[instruction_shift_op],ARM_SHIFT_OPCODE_LSL
	jnz	ERROR_shift_must_be_LSL
    .wrd_q_rn_PMrm_p:
	or	ebp,1 shl 24				;set P
    .encode:
	movzx	eax,[operand_register0]			;wrd
	shl	eax,12
	or	ebp,eax
	movzx	eax,[operand_register1]			;rn
	shl	eax,16
	or	ebp,eax
	movzx	eax,[operand_register2]			;rm
	or	ebp,eax
	mov	eax,[immediate_value]
	shl	eax,4
	or	ebp,eax
	jmp	ARM_post_process_copro
    .out_of_range:
	mov	ecx,ERROR_shift_value_out_of_range.0_15
	jmp	ARM_post_process_copro_with_error

;SIMD

SIMD_INT_vd_vn_vm:
	;used by VABA.S16, VABA.S32, VABA.S8, VABA.U16, VABA.U32, VABA.U8,
	;	 VMLA.S8, VMLA.U8, VMLA.I8, VMLS.S8, VMLS.U8, VMLS.I8
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg>,\	;0=dd,dn,dm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_simd_qreg>	;1=qd,qn,qm
    .int_test:
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_INT
	jz	ERROR_requires_copro_capability_simd_int
    .encode:
	movzx	eax,al
	shl	eax,6
	or	ebp,eax					;set Q bit
	movzx	eax,[operand_register0]			;vd
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	eax,12
	shl	ecx,22					;D bit
	or	ebp,eax
	or	ebp,ecx
	movzx	eax,[operand_register1]			;vn
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	eax,16
	shl	ecx,7					;N bit
	or	ebp,eax
	or	ebp,ecx
	movzx	eax,[operand_register2]			;vm
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	ecx,5					;M bit
	or	ebp,eax
	or	ebp,ecx
	jmp	ARM_post_process_simd

SIMD_INT_vd_vn_vm_alt:
	;used by VABD.S16, VABD.S32, VABD.S8, VABD.U16, VABD.U32, VABD.U8,
	;	 VADD.I8, VADD.I16, VADD.I32, VADD.I64, VAND, VBIC, VBIF, VBIT, VBSL,
	;	 VCGE.U8, VCGE.U16, VCGE.U32, VCGT.U8, VCGT.U16, VCGT.U32, VEOR,
	;	 VHADD.S8, VHADD.S16, VHADD.S32, VHADD.U8, VHADD.U16, VHADD.U32,
	;	 VHSUB.S8, VHSUB.S16, VHSUB.S32, VHSUB.U8, VHSUB.U16, VHSUB.U32,
	;	 VMAX.S8, VMAX.S16, VMAX.S32, VMAX.U8, VMAX.U16, VMAX.U32,
	;	 VMIN.S8, VMIN.S16, VMIN.S32, VMIN.U8, VMIN.U16, VMIN.U32,
	;	 VMUL.I8, VMUL.S8, VMUL.S16, VMUL.S32, VMUL.U8, VMUL.U16, VMUL.U32, VMUL.P8,
	;	 VQADD.S8, VQADD.S16, VQADD.S32, VQADD.S64, VQADD.U8, VQADD.U16, VQADD.U32, VQADD.U64,
	;	 VQRSHL.S8, VQRSHL.S16, VQRSHL.S32, VQRSHL.S64, VQRSHL.U8, VQRSHL.U16, VQRSHL.U32, VQRSHL.U64,
	;	 VQSUB.S8, VQSUB.S16, VQSUB.S32, VQSUB.S64, VQSUB.U8, VQSUB.U16, VQSUB.U32, VQSUB.U64,
	;	 VRHADD.S8, VRHADD.S16, VRHADD.S32, VRHADD.U8, VRHADD.U16, VRHADD.U32,
	;	 VRSHL.S16, VRSHL.S32, VRSHL.S64, VRSHL.S8, VRSHL.U16, VRSHL.U32, VRSHL.U64, VRSHL.U8,
	;	 VSHL.S16, VSHL.S32, VSHL.S64, VSHL.S8, VSHL.U16, VSHL.U32, VSHL.U64, VSHL.U8,
	;	 VSUB.I8, VSUB.I16, VSUB.I32, VSUB.I64, VTST.8, VTST.16, VTST.32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpd_reg>,\			;0=ddn,dm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg>,\	;1=dd,dn,dm
	<TMPL_simd_qreg,TMPL_simd_qreg>,\		;2=qdn,qm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_simd_qreg>	;3=qd,qn,qm
    .encode:
	shr	al,1
	jc	SIMD_INT_vd_vn_vm.int_test
	mov	cx,word[operand_registers+0]
	mov	word[operand_registers+1],cx
	jmp	SIMD_INT_vd_vn_vm.int_test

SIMD_INT_vd_vn_vm_alt_zero:
	;used by VCEQ.I8, VCEQ.I16, VCEQ.I32, VCGE.S8, VCGE.S16, VCGE.S32, VCGT.S8, VCGT.S16, VCGT.S32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_imm>,\			;0=ddm,0
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_imm>,\	;1=dd,dm,0
	<TMPL_vfpd_reg,TMPL_vfpd_reg>,\			;2=ddn,dm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg>,\	;3=dd,dn,dm
	<TMPL_simd_qreg,TMPL_imm>,\			;4=qdm,0
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_imm>,\	;5=qd,qm,0
	<TMPL_simd_qreg,TMPL_simd_qreg>,\		;6=qdn,qm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_simd_qreg>	;7=qd,qn,qm
	test	al,010b
	jnz	.not_zero
    .zero:
	cmp	[immediate_value],0
	mov	ecx,ERROR_immediate_offset_out_of_range.0
	jnz	ARM_post_process_simd_with_error
	movzx	ebp,[instruction_condition]
	and	ebp,0xf0
	shl	ebp,28-4
	or	ebp,[thumb32_instruction]
	mov	ah,al
	shr	al,2
	test	ah,1
	jnz	SIMD_INT_vd_vm.int_test
	mov	cl,[operand_register0]
	mov	[operand_register1],cl
	jmp	SIMD_INT_vd_vm.int_test
    .not_zero:
	mov	ah,al
	and	ax,0x0401
	shr	ah,1
	or	al,ah
	jmp	SIMD_INT_vd_vn_vm_alt.encode

SIMD_INT_vd_vn_vm_long:
	;used by VABAL.S16, VABAL.S32, VABAL.S8, VABAL.U16, VABAL.U32, VABAL.U8,
	;	 VABDL.S16, VABDL.S32, VABDL.S8, VABDL.U16, VABDL.U32, VABDL.U8,
	;	 VADDL.S16, VADDL.S32, VADDL.S8, VADDL.U16, VADDL.U32, VADDL.U8,
	;	 VSUBL.S16, VSUBL.S32, VSUBL.S8, VSUBL.U16, VSUBL.U32, VSUBL.U8,
	;	 VMLAL.S8, VMLAL.U8, VMLSL.S8, VMLSL.U8, VMULL.S8, VMULL.U8, VMULL.P8
	call	decode_template
    TEMPLATE \
	<TMPL_simd_qreg,TMPL_vfpd_reg,TMPL_vfpd_reg>	;0=qd,dn,dm
	jmp	SIMD_INT_vd_vn_vm.int_test

SIMD_INT_vd_vn_vm_wide:
	;used by VADDW.S16, VADDW.S32, VADDW.S8, VADDW.U16, VADDW.U32, VADDW.U8
	;	 VSUBW.S16, VSUBW.S32, VSUBW.S8, VSUBW.U16, VSUBW.U32, VSUBW.U8
	call	decode_template
    TEMPLATE \
	<TMPL_simd_qreg,TMPL_vfpd_reg>,\		;0=qdn,dm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_vfpd_reg>	;1=qd,qn,dm
	jmp	SIMD_INT_vd_vn_vm_alt.encode

SIMD_FLOAT_vd_vn_vm_alt:
	;used by VABD.F32, VACGE.F32, VACGT.F32, VMAX.F32, VMIN.F32, VRECPS.F32, VRSQRTS.F32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpd_reg>,\			;0=ddn,dm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg>,\	;1=dd,dn,dm
	<TMPL_simd_qreg,TMPL_simd_qreg>,\		;2=qdn,qm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_simd_qreg>	;3=qd,qn,qm
    .float_test:
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_FLOAT
	jz	ERROR_requires_copro_capability_simd_float
	shr	al,1
	jc	SIMD_INT_vd_vn_vm.encode
	mov	cx,word[operand_registers+0]
	mov	word[operand_registers+1],cx
	jmp	SIMD_INT_vd_vn_vm.encode

SIMD_FLOAT_vd_vn_vm_alt_zero:
	;used by VCEQ.F32, VCGE.F32, VCGT.F32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_imm>,\			;0=ddm,0
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_imm>,\	;1=dd,dm,0
	<TMPL_vfpd_reg,TMPL_vfpd_reg>,\			;2=ddn,dm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg>,\	;3=dd,dn,dm
	<TMPL_simd_qreg,TMPL_imm>,\			;4=qdm,0
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_imm>,\	;5=qd,qm,0
	<TMPL_simd_qreg,TMPL_simd_qreg>,\		;6=qdn,qm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_simd_qreg>	;7=qd,qn,qm
	test	al,010b
	jnz	.not_zero
    .zero:
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_FLOAT
	jz	ERROR_requires_copro_capability_simd_float
	cmp	[immediate_value],0
	mov	ecx,ERROR_immediate_offset_out_of_range.0
	jnz	ARM_post_process_simd_with_error
	movzx	ebp,[instruction_condition]
	and	ebp,0xf0
	shl	ebp,28-4
	or	ebp,[thumb32_instruction]
	mov	ah,al
	shr	al,2
	test	ah,1
	jnz	SIMD_INT_vd_vm.encode
	mov	cl,[operand_register0]
	mov	[operand_register1],cl
	jmp	SIMD_INT_vd_vm.encode
    .not_zero:
	mov	ah,al
	and	ax,0x0401
	shr	ah,1
	or	al,ah
	jmp	SIMD_FLOAT_vd_vn_vm_alt.float_test

SIMD_FLOAT_vd_vm_vn_alt:
	;used by VACLE.F32, VACLT.F32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpd_reg>,\			;0=ddm,dn
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg>,\	;1=dd,dm,dn
	<TMPL_simd_qreg,TMPL_simd_qreg>,\		;2=qdm,qn
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_simd_qreg>	;3=qd,qm,qn
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_FLOAT
	jz	ERROR_requires_copro_capability_simd_float
    .swap:
	mov	cx,word[operand_registers+1]
	xchg	ch,cl
	mov	word[operand_registers+1],cx
	shr	al,1
	jc	SIMD_INT_vd_vn_vm.encode
	mov	cl,ch
	mov	ch,[operand_register0]
	mov	word[operand_registers+1],cx
	jmp	SIMD_INT_vd_vn_vm.encode

SIMD_INT_vd_vn_vm_alt_swap:
	;used by VCLE.U8, VCLE.U16, VCLE.U32, VCLT.U8, VCLT.U16, VCLT.U32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpd_reg>,\			;0=ddn,dm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg>,\	;1=dd,dn,dm
	<TMPL_simd_qreg,TMPL_simd_qreg>,\		;2=qdn,qm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_simd_qreg>	;3=qd,qn,qm
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_INT
	jz	ERROR_requires_copro_capability_simd_int
	jmp	SIMD_FLOAT_vd_vm_vn_alt.swap

SIMD_INT_vd_vn_vm_alt_zero_swap:
	;used by VCLE.S8, VCLE.S16, VCLE.S32, VCLT.S8, VCLT.S16, VCLT.S32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_imm>,\			;0=ddm,0
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_imm>,\	;1=dd,dm,0
	<TMPL_vfpd_reg,TMPL_vfpd_reg>,\			;2=ddn,dm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg>,\	;3=dd,dn,dm
	<TMPL_simd_qreg,TMPL_imm>,\			;4=qdm,0
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_imm>,\	;5=qd,qm,0
	<TMPL_simd_qreg,TMPL_simd_qreg>,\		;6=qdn,qm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_simd_qreg>	;7=qd,qn,qm
	test	al,010b
	jz	SIMD_INT_vd_vn_vm_alt_zero.zero
	mov	ah,al
	and	ax,0x0401
	shr	ah,1
	or	al,ah
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_INT
	jz	ERROR_requires_copro_capability_simd_int
	jmp	SIMD_FLOAT_vd_vm_vn_alt.swap

SIMD_FLOAT_vd_vn_vm_alt_zero_swap:
	;used by VCLE.F32, VCLT.F32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_imm>,\			;0=ddm,0
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_imm>,\	;1=dd,dm,0
	<TMPL_vfpd_reg,TMPL_vfpd_reg>,\			;2=ddn,dm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg>,\	;3=dd,dn,dm
	<TMPL_simd_qreg,TMPL_imm>,\			;4=qdm,0
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_imm>,\	;5=qd,qm,0
	<TMPL_simd_qreg,TMPL_simd_qreg>,\		;6=qdn,qm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_simd_qreg>	;7=qd,qn,qm
	test	al,010b
	jz	SIMD_FLOAT_vd_vn_vm_alt_zero.zero
	mov	ah,al
	and	ax,0x0401
	shr	ah,1
	or	al,ah
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_FLOAT
	jz	ERROR_requires_copro_capability_simd_float
	jmp	SIMD_FLOAT_vd_vm_vn_alt.swap

SIMD_INT_vd_vm:
	;used by VABS.S8, VABS.S16, VABS.S32, VCLS.S8, VCLS.S16, VCLS.S32, VCLZ.S8, VCLZ.S16, VCLZ.S32, VCNT.8,
	;	 VMVN, VNEG.S8, VNEG.S16, VNEG.S32, VPADAL.S8, VPADAL.S16, VPADAL.S32, VPADAL.U8, VPADAL.U16, VPADAL.U32
	;	 VPADDL.S8, VPADDL.S16, VPADDL.S32, VPADDL.U8, VPADDL.U16, VPADDL.U32, VQABS.S8, VQABS.S16, VQABS.S32,
	;	 VQNEG.S8, VQNEG.S16, VQNEG.S32, VRECPE.U32, VREV16.8, VREV32.16, VREV32.8, VREV64.16, VREV64.32, VREV64.8,
	;	 VRSQRTE.U32, VSWP, VTRN.8, VTRN.16, VTRN.32, VUZP.8, VUZP.16, VUZP.32, VZIP.8, VZIP.16, VZIP.32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpd_reg>,\			;0=dd,dm
	<TMPL_simd_qreg,TMPL_simd_qreg>			;1=qd,qm
    .int_test:
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_INT
	jz	ERROR_requires_copro_capability_simd_int
    .encode:
	movzx	eax,al
	shl	eax,6
	or	ebp,eax					;set Q bit
	movzx	eax,[operand_register0]			;vd
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	eax,12
	shl	ecx,22					;D bit
	or	ebp,eax
	or	ebp,ecx
	movzx	eax,[operand_register1]			;vm
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	ecx,5					;M bit
	or	ebp,eax
	or	ebp,ecx
	jmp	ARM_post_process_simd

SIMD_FLOAT_vd_vm_imm:
	;used by VCVT.F32.S32, VCVT.F32.U32, VCVT.S32.F32, VCVT.U32.F32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpd_reg>,\			;0=dd,dm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_imm>,\	;1=dd,dm,imm
	<TMPL_simd_qreg,TMPL_simd_qreg>,\		;2=qd,qm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_imm>,\	;3=qd,qm,imm
	<TMPL_vfps_reg,TMPL_vfps_reg>,\			;4=sd,sm
	<TMPL_vfps_reg,TMPL_vfps_reg,TMPL_imm>,\	;5=sd,sm,imm
	<TMPL_vfps_reg,TMPL_imm>			;6=sdm,imm
	cmp	al,6
	jnz	.registers_okay
	mov	cl,[operand_register0]
	mov	[operand_register1],cl
	mov	al,5
    .registers_okay:
	shr	al,1
	jnc	SIMD_FLOAT_vd_vm_f32.encode
    ;recode for immediate forms
	mov	ecx,ebp
	mov	edx,ebp
	and	ecx,1 shl 7				;signed=0, unsigned=1
	and	edx,1 shl 8				;to float=0, to int=1
	shl	edx,(18-8)-(16-7)
	or	edx,ecx
	shl	edx,16-7
	or	edx,0x0eba0ac0				;fsltos
	mov	[thumb32_instruction],edx
	shl	ecx,24-7
	and	ebp,1 shl 8
	or	ebp,ecx
	or	ebp,0xf2a00e10				;vcvt.f32.s32 without imm
	cmp	al,2
	jz	.VFP_alias
	mov	ecx,[immediate_value]
	cmp	ecx,1
	jb	.out_of_range
	cmp	ecx,32
	ja	.out_of_range
	mov	edx,32
	sub	edx,ecx
	shl	edx,16
	or	ebp,edx
	jmp	SIMD_FLOAT_vd_vm_f32.float_test
    .VFP_alias:
	movzx	ebp,[instruction_condition]
	and	ebp,0xf0
	shl	ebp,28-4
	or	ebp,[thumb32_instruction]
	jmp	VFP_dd_dd_imm.do
    .out_of_range:
	mov	ecx,ERROR_immediate_value_out_of_range.1_32
	jmp	ARM_post_process_copro_with_error

SIMD_FLOAT_vd_vm_f32:
	;used by VABS.F32, VNEG.F32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpd_reg>,\			;0=dd,dm
	<TMPL_simd_qreg,TMPL_simd_qreg>,\		;1=qd,qm
	<TMPL_vfps_reg,TMPL_vfps_reg>			;2=sd,sm
    .encode:
	cmp	al,2
	jnz	.float_test
    ;VFP alias
	movzx	ebp,[instruction_condition]
	and	ebp,0xf0
	shl	ebp,28-4
	or	ebp,[thumb32_instruction]
	jmp	VFP_sd_sm.v1xd_test
    .float_test:
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_FLOAT
	jz	ERROR_requires_copro_capability_simd_float
	jmp	SIMD_INT_vd_vm.encode

SIMD_FLOAT_vd_vn_vm_f32:
	;used by VADD.F32, VSUB.F32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpd_reg>,\			;0=ddn,dm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg>,\	;1=dd,dn,dm
	<TMPL_simd_qreg,TMPL_simd_qreg>,\		;2=qdn,qm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_simd_qreg>,\;3=qd,qn,qm
	<TMPL_vfps_reg,TMPL_vfps_reg>,\			;4=sdn,sm
	<TMPL_vfps_reg,TMPL_vfps_reg,TMPL_vfps_reg>	;5=sd,sn,sm
    .do:
	cmp	al,4
	jb	SIMD_FLOAT_vd_vn_vm_alt.float_test
    ;VFP alias
	movzx	ebp,[instruction_condition]
	and	ebp,0xf0
	shl	ebp,28-4
	or	ebp,[thumb32_instruction]
	sub	al,4
	jmp	VFP_sd_sn_sm.v1xd_test

SIMD_INT_vd_vn_vm_narrow:
	;used by VADDHN.I16, VADDHN.I32, VADDHN.I64, VRADDHN.I16, VRADDHN.I32, VRADDHN.I64,
	;	 VRSUBHN.I16, VRSUBHN.I32, VRSUBHN.I64, VSUBHN.I16, VSUBHN.I32, VSUBHN.I64
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_simd_qreg,TMPL_simd_qreg>	;0=dd,qn,qm
	jmp	SIMD_INT_vd_vn_vm.int_test

SIMD_INT_vd_imm:
	;used by VAND.I32, VAND.I16, VBIC.I32, VBIC.I16, VORN.I32, VORN.I16, VORR.I32, VORR.I16
	call	decode_template
    TEMPLATE \
	<TMPL_simd_qreg,TMPL_imm>,\			;0=qd,imm
	<TMPL_vfpd_reg,TMPL_imm>			;1=dd,imm
	mov	ecx,[immediate_value]
	btr	ebp,0					;test for VAND
	jnc	.inversion_done
	not	ecx
	test	ebp,1000b shl 8				;test for .I16 instruction
	jz	.inversion_done
	cmp	ecx,0xffff0000
	jb	.inversion_done
	movzx	ecx,cx
    .inversion_done:
	test	ecx,not 0xff
	jz	.encode
	xor	ebp,0010b shl 8
	ror	ecx,8
	test	ecx,not 0xff
	jz	.encode
	test	ebp,1000b shl 8				;test for .I16 instruction
	jnz	.out_of_range
	xor	ebp,0110b shl 8
	ror	ecx,8
	test	ecx,not 0xff
	jz	.encode
	xor	ebp,0010b shl 8
	ror	ecx,8
	test	ecx,not 0xff
	jnz	.out_of_range
    .encode:
	and	eax,1
	xor	eax,1
	shl	eax,6
	or	ebp,eax					;set Q
	mov	eax,ecx
	mov	edx,ecx
	and	eax,0x0f
	and	ecx,0x70
	and	edx,0x80
	shl	ecx,16-4
	shl	edx,24-7
	or	ebp,eax					;imm4
	or	ebp,ecx					;imm3
	or	ebp,edx					;i
	movzx	eax,[operand_register0]			;vd
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	eax,12
	shl	ecx,22					;D bit
	or	ebp,eax
	or	ebp,ecx
	jmp	ARM_post_process_simd
    .out_of_range:
	mov	ecx,ERROR_immediate_cannot_be_encoded
	jmp	ARM_post_process_simd_with_error

SIMD_FLOAT_qd_dm:
	;used by VCVT.F32.F16
	call	decode_template
    TEMPLATE \
	<TMPL_simd_qreg,TMPL_vfpd_reg>			;0=qd,dm
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_HP
	jz	ERROR_requires_copro_capability_simd_hp
	jmp	SIMD_INT_vd_vm.encode

SIMD_FLOAT_dd_qm:
	;used by VCVT.F16.F32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_simd_qreg>			;0=dd,qm
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_HP
	jz	ERROR_requires_copro_capability_simd_hp
	jmp	SIMD_INT_vd_vm.encode

SIMD_INT_qd_dmx:
	;used by VDUP.8, VDUP.16, VDUP.32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpdx_reg>,\		;0=dd,dm[x]
	<TMPL_vfpd_reg,TMPL_base_reg>,\			;1=dd,rt
	<TMPL_simd_qreg,TMPL_vfpdx_reg>,\		;2=qd,dm[x]
	<TMPL_simd_qreg,TMPL_base_reg>			;3=qd,rt
	shr	al,1
	jc	.reg
	mov	ecx,ebp
	shr	ecx,16
	and	ecx,0xf					;ecx=1, 2 or 3 for 8, 16 or 32 size
	and	ebp,not (0xf shl 16)
	mov	edx,16
	shr	edx,cl
	cmp	edx,[immediate_value]
	jbe	.out_of_range
	mov	edx,[immediate_value]
	shl	edx,1
	or	edx,1
	add	ecx,16-1
	shl	edx,cl
	or	ebp,edx
	jmp	SIMD_INT_vd_vm.int_test
    .reg:
	movzx	ebp,[instruction_condition]
	and	ebp,0xf0
	shl	ebp,28-4
	or	ebp,[thumb32_instruction]
	shl	eax,21
	or	ebp,eax					;set Q
	movzx	eax,[operand_register0]			;vd
	xor	ecx,ecx
	shr	eax,1
	setc	cl
	shl	eax,16
	shl	ecx,7					;D bit
	or	ebp,eax
	or	ebp,ecx
	movzx	eax,[operand_register1]			;rt
	cmp	eax,15					;PC?
	jz	ERROR_r15_not_valid
	shl	eax,12
	or	ebp,eax
	test	[code_type],CPU_ACTIVITY_ARM
	jnz	ARM_post_process_copro
	cmp	eax,13 shl 12				;SP?
	jz	ERROR_r13_not_valid
	jmp	ARM_post_process_copro
    .out_of_range:
	mov	ecx,ERROR_immediate_cannot_be_encoded
	jmp	ARM_post_process_simd_with_error

SIMD_INT_vd_vn_vm_imm:
	;used by VEXT.8, VEXT.16, VEXT.32, VEXT.64
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_imm>,\		;0=ddn,dm,imm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_imm>,\	;1=dd,dn,dm,imm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_imm>,\		;2=qdn,qm,imm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_simd_qreg,TMPL_imm>	;3=qd,qn,qm,imm
	mov	edx,[immediate_value]
	mov	ecx,ebp
	shr	ecx,8
	and	ecx,0xf					;ecx=multiplier
	imul	edx,ecx
	mov	ecx,eax
	shr	ecx,1
	and	ecx,1
	xor	ecx,1
	shl	edx,cl
	cmp	edx,16
	jae	.out_of_range
	shr	edx,cl
	and	ebp,not (0xf shl 8)
	shl	edx,8
	or	ebp,edx
	shr	al,1
	jc	SIMD_INT_vd_vn_vm.int_test
	mov	cx,word[operand_registers+0]
	mov	word[operand_registers+1],cx
	jmp	SIMD_INT_vd_vn_vm.int_test
    .out_of_range:
	mov	ecx,ERROR_immediate_cannot_be_encoded
	jmp	ARM_post_process_simd_with_error

SIMD_INT_list_rn_rm_1:
	;used by VLD1.8, VLD1.16, VLD1.32, VLD1.64, VST1.8, VST1.16, VST1.32, VST1.64
	call	decode_template
    TEMPLATE \
	<TMPL_simd_reg_list,TMPL_bracket_left,TMPL_address_reg@,TMPL_bracket_right>,\				;0={dy[x]},[rn]
	<TMPL_simd_reg_list,TMPL_bracket_left,TMPL_address_reg@,TMPL_bracket_right,TMPL_modifier_exclaim>,\	;1={dy[x]},[rn]!
	<TMPL_simd_reg_list,TMPL_bracket_left,TMPL_address_reg@,TMPL_bracket_right,TMPL_base_reg>,\		;2={dy[x]},[rn],rm
	<TMPL_simd_reg_list,TMPL_bracket_left,TMPL_address_reg@,TMPL_bracket_right,TMPL_imm>			;3={dy[x]},[rn],imm
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_INT
	jz	ERROR_requires_copro_capability_simd_int
	cmp	[simd_reg_list_type],SIMD_REG_LIST_TYPE_ELEMENT
	jz	.element
	cmp	[simd_reg_list_type],SIMD_REG_LIST_TYPE_VECTOR
	jz	.vector
    ;register
	mov	ah,[simd_reg_list_count]
	xor	ecx,ecx
	mov	edx,7 shl 8				;type = 0111
	cmp	ah,1
	cmovz	ecx,edx
	mov	edx,10 shl 8				;type = 1010
	cmp	ah,2
	cmovz	ecx,edx
	mov	edx,6 shl 8				;type = 0110
	cmp	ah,3
	cmovz	ecx,edx
	mov	edx,2 shl 8				;type = 0010
	cmp	ah,4
	cmovz	ecx,edx
	or	ebp,ecx
	test	ecx,ecx
	mov	ecx,ERROR_register_list_invalid
	jz	ARM_post_process_simd_with_error
	mov	ah,[operand_register0]			;rn
	mov	ecx,0 shl 4				;align = 00
	shr	ah,4
	jz	.set_alignment
	mov	ecx,1 shl 4				;align = 01
	cmp	ah,3					;@64
	jz	.set_alignment
	mov	ecx,2 shl 4				;align = 10
	cmp	ah,4					;@128
	jz	.set_alignment
	cmp	ah,5					;@256
	mov	ecx,ERROR_alignment_invalid
	jnz	ARM_post_process_simd_with_error
	mov	ecx,3 shl 4				;align = 11
    .set_alignment:
	or	ebp,ecx
	movzx	ebx,[simd_reg_list_count]
	shl	ebx,3					;ebx=writeback offset
    .encode:
	mov	edx,0xf
	cmp	al,0
	jz	.set_rm
	mov	edx,0xd
	cmp	al,1
	jz	.set_rm
	movzx	edx,[operand_register1]			;rm
	mov	ecx,ERROR_r13_r15_not_valid.third
	cmp	edx,13
	jz	ARM_post_process_simd_with_error
	cmp	edx,15
	jz	ARM_post_process_simd_with_error
	cmp	al,2
	jz	.set_rm
    ;immediate post update
	cmp	ebx,[immediate_value]
	mov	ecx,ERROR_immediate_offset_out_of_range
	jnz	ARM_post_process_simd_with_error
	mov	edx,0xd
    .set_rm:
	or	ebp,edx
	movzx	eax,[simd_reg_list_first]		;vd
	mov	ecx,eax
	and	eax,0x0f
	and	ecx,0x10
	shl	eax,12
	shl	ecx,22-4				;D bit
	or	ebp,eax
	or	ebp,ecx
	movzx	eax,[operand_register0]			;rn
	and	eax,0xf
	cmp	eax,0xf
	mov	ecx,ERROR_r15_not_valid.second
	jz	ARM_post_process_simd_with_error
	shl	eax,16
	or	ebp,eax
	jmp	ARM_post_process_simd
    .element:
	or	ebp,1 shl 23
	cmp	[simd_reg_list_count],1
	mov	ecx,ERROR_register_list_invalid
	jnz	ARM_post_process_simd_with_error
	mov	edx,ebp
	and	edx,3 shl 6
	cmp	edx,3 shl 6				;.64 size?
	mov	ecx,ERROR_use_fldr_for_single_reg
	jz	ARM_post_process_simd_with_error
	and	ebp,not (3 shl 6)
	shl	edx,10-6
	or	ebp,edx
	mov	ah,[operand_register0]			;rn
	shr	ah,4
	cmp	edx,1 shl 10
	movzx	edx,[simd_reg_list_x]
	ja	.element.32
	jz	.element.16
   ;element.8
	test	ah,ah
	mov	ecx,ERROR_alignment_invalid
	jnz	ARM_post_process_simd_with_error
	cmp	edx,7
	mov	ecx,ERROR_element_value_out_of_range.0_7
	ja	ARM_post_process_simd_with_error
	shl	edx,4+1
	or	ebp,edx
	mov	ebx,1
	jmp	.encode
    .element.16:
	cmp	ah,1
	mov	ecx,ERROR_alignment_invalid
	ja	ARM_post_process_simd_with_error
	cmp	edx,3
	mov	ecx,ERROR_element_value_out_of_range.0_3
	ja	ARM_post_process_simd_with_error
	shl	edx,2
	or	dl,ah
	shl	edx,4
	or	ebp,edx
	mov	ebx,2
	jmp	.encode
    .element.32:
	cmp	ah,2
	mov	ecx,ERROR_alignment_invalid
	ja	ARM_post_process_simd_with_error
	cmp	ah,1
	mov	ecx,ERROR_alignment_invalid
	jz	ARM_post_process_simd_with_error
	cmp	edx,1
	mov	ecx,ERROR_element_value_out_of_range.0_1
	ja	ARM_post_process_simd_with_error
	shl	edx,3
	or	dl,ah
	shr	ah,1
	or	dl,ah
	shl	edx,4
	or	ebp,edx
	mov	ebx,4
	jmp	.encode
    .vector:
	mov	ecx,ERROR_register_list_invalid
	test	ebp,1 shl 21				;VST1=0
	jz	ARM_post_process_simd_with_error
	or	ebp,1 shl 23 + 3 shl 10
	movzx	edx,[simd_reg_list_count]
	cmp	edx,2
	mov	ecx,ERROR_register_list_invalid
	ja	ARM_post_process_simd_with_error
	dec	edx
	shl	edx,5
	or	ebp,edx					;set T bit
	mov	edx,ebp
	and	edx,3 shl 6
	cmp	edx,3 shl 6				;.64 size?
	mov	ecx,ERROR_reg_size_64_not_encodable
	jz	ARM_post_process_simd_with_error
	mov	ah,[operand_register0]			;rn
	shr	ah,4
	cmp	edx,1 shl 6
	movzx	edx,ah
	ja	.vector.32
	jz	.vector.16
   ;vector.8
	test	ah,ah
	mov	ecx,ERROR_alignment_invalid
	jnz	ARM_post_process_simd_with_error
	mov	ebx,1
	jmp	.encode
    .vector.16:
	cmp	ah,1
	mov	ecx,ERROR_alignment_invalid
	ja	ARM_post_process_simd_with_error
	shl	edx,4
	or	ebp,edx
	mov	ebx,2
	jmp	.encode
    .vector.32:
	cmp	ah,2
	mov	ecx,ERROR_alignment_invalid
	ja	ARM_post_process_simd_with_error
	cmp	ah,1
	mov	ecx,ERROR_alignment_invalid
	jz	ARM_post_process_simd_with_error
	shl	edx,3
	or	ebp,edx
	mov	ebx,4
	jmp	.encode

SIMD_INT_list_rn_rm_2:
	;used by VLD2.8, VLD2.16, VLD2.32, VST2.8, VST2.16, VST2.32
	call	decode_template
    TEMPLATE \
	<TMPL_simd_reg_list,TMPL_bracket_left,TMPL_address_reg@,TMPL_bracket_right>,\				;0={dy[x]},[rn]
	<TMPL_simd_reg_list,TMPL_bracket_left,TMPL_address_reg@,TMPL_bracket_right,TMPL_modifier_exclaim>,\	;1={dy[x]},[rn]!
	<TMPL_simd_reg_list,TMPL_bracket_left,TMPL_address_reg@,TMPL_bracket_right,TMPL_base_reg>,\		;2={dy[x]},[rn],rm
	<TMPL_simd_reg_list,TMPL_bracket_left,TMPL_address_reg@,TMPL_bracket_right,TMPL_imm>			;3={dy[x]},[rn],imm
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_INT
	jz	ERROR_requires_copro_capability_simd_int
	cmp	[simd_reg_list_type],SIMD_REG_LIST_TYPE_ELEMENT
	jz	.element
	cmp	[simd_reg_list_type],SIMD_REG_LIST_TYPE_VECTOR
	jz	.vector
    ;register
	mov	ah,[simd_reg_list_count]
	xor	ecx,ecx
	mov	edx,8 shl 8				;type = 1000
	cmp	ah,2
	cmovz	ecx,edx
	mov	edx,9 shl 8				;type = 1001
	cmp	ah,0x82
	cmovz	ecx,edx
	mov	edx,3 shl 8				;type = 0011
	cmp	ah,4
	cmovz	ecx,edx
	or	ebp,ecx
	test	ecx,ecx
	mov	ecx,ERROR_register_list_invalid
	jz	ARM_post_process_simd_with_error
	mov	ah,[operand_register0]			;rn
	mov	ecx,0 shl 4				;align = 00
	shr	ah,4
	jz	.set_alignment
	mov	ecx,1 shl 4				;align = 01
	cmp	ah,3					;@64
	jz	.set_alignment
	mov	ecx,2 shl 4				;align = 10
	cmp	ah,4					;@128
	jz	.set_alignment
	cmp	ah,5					;@256
	mov	ecx,ERROR_alignment_invalid
	jnz	ARM_post_process_simd_with_error
	cmp	[simd_reg_list_count],4
	jnz	ARM_post_process_simd_with_error
	mov	ecx,3 shl 4				;align = 11
    .set_alignment:
	or	ebp,ecx
	mov	bl,[simd_reg_list_count]
	and	ebx,0x7f
	shl	ebx,3					;ebx=writeback offset
	jmp	SIMD_INT_list_rn_rm_1.encode
    .element:
	or	ebp,1 shl 23 + 1 shl 8
	mov	cl,[simd_reg_list_count]
	test	ebp,3 shl 6				;.8?
	setz	ch
	shl	ch,7
	or	ch,0x7f
	and	cl,ch
	cmp	cl,2
	mov	ecx,ERROR_register_list_invalid
	jnz	ARM_post_process_simd_with_error
	mov	edx,ebp
	and	edx,3 shl 6
	and	ebp,not (3 shl 6)
	shl	edx,10-6
	or	ebp,edx
	mov	ah,[operand_register0]			;rn
	shr	ah,4
	cmp	edx,1 shl 10
	movzx	edx,[simd_reg_list_x]
	ja	.element.32
	jz	.element.16
   ;element.8
	cmp	ah,1
	mov	ecx,ERROR_alignment_invalid
	ja	ARM_post_process_simd_with_error
	cmp	edx,7
	mov	ecx,ERROR_element_value_out_of_range.0_7
	ja	ARM_post_process_simd_with_error
	shl	edx,1
	or	dl,ah
	shl	edx,4
	or	ebp,edx
	mov	ebx,2
	jmp	SIMD_INT_list_rn_rm_1.encode
    .element.16:
	cmp	ah,2
	mov	ecx,ERROR_alignment_invalid
	ja	ARM_post_process_simd_with_error
	cmp	ah,1
	mov	ecx,ERROR_alignment_invalid
	jz	ARM_post_process_simd_with_error
	cmp	edx,3
	mov	ecx,ERROR_element_value_out_of_range.0_3
	ja	ARM_post_process_simd_with_error
	shl	edx,1
	mov	cl,[simd_reg_list_count]
	shr	cl,7
	or	dl,cl
	shl	edx,1
	shr	ah,1
	or	dl,ah
	shl	edx,4
	or	ebp,edx
	mov	ebx,4
	jmp	SIMD_INT_list_rn_rm_1.encode
    .element.32:
	cmp	ah,3
	mov	ecx,ERROR_alignment_invalid
	ja	ARM_post_process_simd_with_error
	dec	ah
	cmp	ah,2
	mov	ecx,ERROR_alignment_invalid
	jb	ARM_post_process_simd_with_error
	cmp	edx,1
	mov	ecx,ERROR_element_value_out_of_range.0_1
	ja	ARM_post_process_simd_with_error
	shl	edx,1
	mov	cl,[simd_reg_list_count]
	shr	cl,7
	or	dl,cl
	shl	edx,2
	and	ah,1
	xor	ah,1
	or	dl,ah
	shl	edx,4
	or	ebp,edx
	mov	ebx,8
	jmp	SIMD_INT_list_rn_rm_1.encode
    .vector:
	mov	ecx,ERROR_register_list_invalid
	test	ebp,1 shl 21				;VST2=0
	jz	ARM_post_process_simd_with_error
	or	ebp,1 shl 23 + 0xd shl 8
	mov	dl,[simd_reg_list_count]
	mov	dh,dl
	and	dx,0x7f80
	cmp	dh,2
	mov	ecx,ERROR_register_list_invalid
	jnz	ARM_post_process_simd_with_error
	shr	edx,7-5
	and	edx,0x20
	or	ebp,edx					;set T bit
	mov	edx,ebp
	and	edx,3 shl 6
	mov	ah,[operand_register0]			;rn
	shr	ah,4
	cmp	edx,1 shl 6
	movzx	edx,ah
	ja	.vector.32
	jz	.vector.16
   ;vector.8
	cmp	ah,1
	mov	ecx,ERROR_alignment_invalid
	ja	ARM_post_process_simd_with_error
	shl	edx,4
	or	ebp,edx
	mov	ebx,2
	jmp	SIMD_INT_list_rn_rm_1.encode
    .vector.16:
	cmp	ah,2
	mov	ecx,ERROR_alignment_invalid
	ja	ARM_post_process_simd_with_error
	cmp	ah,1
	mov	ecx,ERROR_alignment_invalid
	jz	ARM_post_process_simd_with_error
	shl	edx,3
	or	ebp,edx
	mov	ebx,4
	jmp	SIMD_INT_list_rn_rm_1.encode
    .vector.32:
	cmp	ah,3
	mov	ecx,ERROR_alignment_invalid
	ja	ARM_post_process_simd_with_error
	dec	ah
	cmp	ah,2
	mov	ecx,ERROR_alignment_invalid
	jb	ARM_post_process_simd_with_error
	and	edx,1
	shl	edx,4
	or	ebp,edx
	mov	ebx,8
	jmp	SIMD_INT_list_rn_rm_1.encode

SIMD_INT_list_rn_rm_3:
	;used by VLD3.8, VLD3.16, VLD3.32, VST3.8, VST3.16, VST3.32
	call	decode_template
    TEMPLATE \
	<TMPL_simd_reg_list,TMPL_bracket_left,TMPL_address_reg@,TMPL_bracket_right>,\				;0={dy[x]},[rn]
	<TMPL_simd_reg_list,TMPL_bracket_left,TMPL_address_reg@,TMPL_bracket_right,TMPL_modifier_exclaim>,\	;1={dy[x]},[rn]!
	<TMPL_simd_reg_list,TMPL_bracket_left,TMPL_address_reg@,TMPL_bracket_right,TMPL_base_reg>,\		;2={dy[x]},[rn],rm
	<TMPL_simd_reg_list,TMPL_bracket_left,TMPL_address_reg@,TMPL_bracket_right,TMPL_imm>			;3={dy[x]},[rn],imm
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_INT
	jz	ERROR_requires_copro_capability_simd_int
	cmp	[simd_reg_list_type],SIMD_REG_LIST_TYPE_ELEMENT
	jz	.element
	cmp	[simd_reg_list_type],SIMD_REG_LIST_TYPE_VECTOR
	jz	.vector
    ;register
	mov	ah,[simd_reg_list_count]
	xor	ecx,ecx
	mov	edx,4 shl 8				;type = 0100
	cmp	ah,3
	cmovz	ecx,edx
	mov	edx,5 shl 8				;type = 0101
	cmp	ah,0x83
	cmovz	ecx,edx
	or	ebp,ecx
	test	ecx,ecx
	mov	ecx,ERROR_register_list_invalid
	jz	ARM_post_process_simd_with_error
	mov	ah,[operand_register0]			;rn
	mov	ecx,0 shl 4				;align = 00
	shr	ah,4
	jz	.set_alignment
	cmp	ah,3					;@64
	mov	ecx,ERROR_alignment_invalid
	jnz	ARM_post_process_simd_with_error
	mov	ecx,1 shl 4				;align = 01
    .set_alignment:
	or	ebp,ecx
	mov	ebx,24					;ebx=writeback offset
	jmp	SIMD_INT_list_rn_rm_1.encode
    .element:
	or	ebp,1 shl 23 + 2 shl 8
	mov	ah,[simd_reg_list_count]
	and	ah,0x7f
	cmp	ah,3
	mov	ecx,ERROR_register_list_invalid
	jnz	ARM_post_process_simd_with_error
	mov	edx,ebp
	and	edx,3 shl 6
	and	ebp,not (3 shl 6)
	shl	edx,10-6
	or	ebp,edx
	mov	ah,[simd_reg_list_count]
	shr	ah,7
	test	[operand_register0],0xf0
	mov	ecx,ERROR_alignment_invalid
	jnz	ARM_post_process_simd_with_error
	cmp	edx,1 shl 10
	movzx	edx,[simd_reg_list_x]
	ja	.element.32
	jz	.element.16
   ;element.8
	test	[simd_reg_list_count],0x80
	mov	ecx,ERROR_register_list_invalid
	jnz	ARM_post_process_simd_with_error
	cmp	edx,7
	mov	ecx,ERROR_element_value_out_of_range.0_7
	ja	ARM_post_process_simd_with_error
	shl	edx,4+1
	or	ebp,edx
	mov	ebx,3
	jmp	SIMD_INT_list_rn_rm_1.encode
    .element.16:
	cmp	edx,3
	mov	ecx,ERROR_element_value_out_of_range.0_3
	ja	ARM_post_process_simd_with_error
	shl	edx,1
	or	dl,ah
	shl	edx,4+1
	or	ebp,edx
	mov	ebx,6
	jmp	SIMD_INT_list_rn_rm_1.encode
    .element.32:
	cmp	edx,1
	mov	ecx,ERROR_element_value_out_of_range.0_1
	ja	ARM_post_process_simd_with_error
	shl	edx,1
	or	dl,ah
	shl	edx,4+2
	or	ebp,edx
	mov	ebx,12
	jmp	SIMD_INT_list_rn_rm_1.encode
    .vector:
	mov	ecx,ERROR_register_list_invalid
	test	ebp,1 shl 21				;VST3=0
	jz	ARM_post_process_simd_with_error
	or	ebp,1 shl 23 + 0xe shl 8
	mov	ah,[simd_reg_list_count]
	and	ah,0x7f
	cmp	ah,3
	mov	ecx,ERROR_register_list_invalid
	jnz	ARM_post_process_simd_with_error
	movzx	edx,[simd_reg_list_count]
	shr	edx,7-5
	or	ebp,edx					;set T bit
	test	[operand_register0],0xf0
	mov	ecx,ERROR_alignment_invalid
	jnz	ARM_post_process_simd_with_error
	mov	ecx,ebp
	shr	ecx,6
	and	ecx,3
	mov	ebx,3
	shl	ebx,cl
	jmp	SIMD_INT_list_rn_rm_1.encode

SIMD_INT_list_rn_rm_4:
	;used by VLD4.8, VLD4.16, VLD4.32, VST4.8, VST4.16, VST4.32
	call	decode_template
    TEMPLATE \
	<TMPL_simd_reg_list,TMPL_bracket_left,TMPL_address_reg@,TMPL_bracket_right>,\				;0={dy[x]},[rn]
	<TMPL_simd_reg_list,TMPL_bracket_left,TMPL_address_reg@,TMPL_bracket_right,TMPL_modifier_exclaim>,\	;1={dy[x]},[rn]!
	<TMPL_simd_reg_list,TMPL_bracket_left,TMPL_address_reg@,TMPL_bracket_right,TMPL_base_reg>,\		;2={dy[x]},[rn],rm
	<TMPL_simd_reg_list,TMPL_bracket_left,TMPL_address_reg@,TMPL_bracket_right,TMPL_imm>			;3={dy[x]},[rn],imm
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_INT
	jz	ERROR_requires_copro_capability_simd_int
	cmp	[simd_reg_list_type],SIMD_REG_LIST_TYPE_ELEMENT
	jz	.element
	cmp	[simd_reg_list_type],SIMD_REG_LIST_TYPE_VECTOR
	jz	.vector
    ;register
	mov	ah,[simd_reg_list_count]
	or	ecx,-1
	mov	edx,0 shl 8				;type = 0000
	cmp	ah,4
	cmovz	ecx,edx
	mov	edx,1 shl 8				;type = 0001
	cmp	ah,0x84
	cmovz	ecx,edx
	or	ebp,ecx
	inc	ecx
	mov	ecx,ERROR_register_list_invalid
	jz	ARM_post_process_simd_with_error
	mov	ah,[operand_register0]			;rn
	mov	ecx,0 shl 4				;align = 00
	shr	ah,4
	jz	.set_alignment
	mov	ecx,1 shl 4				;align = 01
	cmp	ah,3					;@64
	jz	.set_alignment
	mov	ecx,2 shl 4				;align = 10
	cmp	ah,4					;@128
	jz	.set_alignment
	cmp	ah,5					;@256
	mov	ecx,ERROR_alignment_invalid
	jnz	ARM_post_process_simd_with_error
	mov	ecx,3 shl 4				;align = 11
    .set_alignment:
	or	ebp,ecx
	mov	ebx,32
	jmp	SIMD_INT_list_rn_rm_1.encode
    .element:
	or	ebp,1 shl 23 + 3 shl 8
	mov	cl,[simd_reg_list_count]
	test	ebp,3 shl 6				;.8?
	setz	ch
	shl	ch,7
	or	ch,0x7f
	and	cl,ch
	cmp	cl,4
	mov	ecx,ERROR_register_list_invalid
	jnz	ARM_post_process_simd_with_error
	mov	edx,ebp
	and	edx,3 shl 6
	and	ebp,not (3 shl 6)
	shl	edx,10-6
	or	ebp,edx
	mov	ah,[operand_register0]			;rn
	shr	ah,4
	cmp	edx,1 shl 10
	movzx	edx,[simd_reg_list_x]
	ja	.element.32
	jz	.element.16
   ;element.8
	cmp	ah,2
	mov	ecx,ERROR_alignment_invalid
	ja	ARM_post_process_simd_with_error
	cmp	ah,1
	mov	ecx,ERROR_alignment_invalid
	jz	ARM_post_process_simd_with_error
	cmp	edx,7
	mov	ecx,ERROR_element_value_out_of_range.0_7
	ja	ARM_post_process_simd_with_error
	shl	edx,2
	or	dl,ah
	shl	edx,3
	or	ebp,edx
	mov	ebx,4
	jmp	SIMD_INT_list_rn_rm_1.encode
    .element.16:
	cmp	ah,3
	mov	ecx,ERROR_alignment_invalid
	ja	ARM_post_process_simd_with_error
	dec	ah
	cmp	ah,2
	mov	ecx,ERROR_alignment_invalid
	jb	ARM_post_process_simd_with_error
	cmp	edx,3
	mov	ecx,ERROR_element_value_out_of_range.0_3
	ja	ARM_post_process_simd_with_error
	shl	edx,1
	mov	cl,[simd_reg_list_count]
	shr	cl,7
	or	dl,cl
	shl	edx,1
	and	ah,1
	xor	ah,1
	or	dl,ah
	shl	edx,4
	or	ebp,edx
	mov	ebx,8
	jmp	SIMD_INT_list_rn_rm_1.encode
    .element.32:
	cmp	ah,4
	mov	ecx,ERROR_alignment_invalid
	ja	ARM_post_process_simd_with_error
	dec	ah
	cmp	ah,2
	mov	ecx,ERROR_alignment_invalid
	jb	ARM_post_process_simd_with_error
	cmp	edx,1
	mov	ecx,ERROR_element_value_out_of_range.0_1
	ja	ARM_post_process_simd_with_error
	shl	edx,1
	mov	cl,[simd_reg_list_count]
	shr	cl,7
	or	dl,cl
	shl	edx,2
	inc	ah
	shr	ah,1
	and	ah,3
	or	dl,ah
	shl	edx,4
	or	ebp,edx
	mov	ebx,16
	jmp	SIMD_INT_list_rn_rm_1.encode
    .vector:
	mov	ecx,ERROR_register_list_invalid
	test	ebp,1 shl 21				;VST4=0
	jz	ARM_post_process_simd_with_error
	or	ebp,1 shl 23 + 0xf shl 8
	mov	dl,[simd_reg_list_count]
	mov	dh,dl
	and	dx,0x7f80
	cmp	dh,4
	mov	ecx,ERROR_register_list_invalid
	jnz	ARM_post_process_simd_with_error
	shr	edx,7-5
	and	edx,0x20
	or	ebp,edx					;set T bit
	mov	edx,ebp
	and	edx,3 shl 6
	mov	ah,[operand_register0]			;rn
	shr	ah,4
	cmp	edx,1 shl 6
	movzx	edx,ah
	ja	.vector.32
	jz	.vector.16
   ;vector.8
	cmp	ah,2
	mov	ecx,ERROR_alignment_invalid
	ja	ARM_post_process_simd_with_error
	cmp	ah,1
	mov	ecx,ERROR_alignment_invalid
	jz	ARM_post_process_simd_with_error
	shl	edx,3
	or	ebp,edx
	mov	ebx,4
	jmp	SIMD_INT_list_rn_rm_1.encode
    .vector.16:
	cmp	ah,3
	mov	ecx,ERROR_alignment_invalid
	ja	ARM_post_process_simd_with_error
	dec	ah
	cmp	ah,2
	mov	ecx,ERROR_alignment_invalid
	jb	ARM_post_process_simd_with_error
	and	edx,1
	shl	edx,4
	or	ebp,edx
	mov	ebx,8
	jmp	SIMD_INT_list_rn_rm_1.encode
    .vector.32:
	cmp	ah,4
	mov	ecx,ERROR_alignment_invalid
	ja	ARM_post_process_simd_with_error
	dec	ah
	cmp	ah,2
	mov	ecx,ERROR_alignment_invalid
	jb	ARM_post_process_simd_with_error
	and	edx,4
	shl	edx,6-2
	not	ah
	and	ah,0x10
	or	dl,ah
	or	ebp,edx
	mov	ebx,16
	jmp	SIMD_INT_list_rn_rm_1.encode

SIMD_INT_rn_list:
	;used by VLDMIA, VLDMDB, VSTMIA, VSTMDB
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_vfp_reg_list_s>,\		;0=rn,{s..}
	<TMPL_base_reg,TMPL_vfp_reg_list_d>,\		;1=rn,{d..}
	<TMPL_base_reg!,TMPL_vfp_reg_list_s>,\		;2=rn!,{s..}
	<TMPL_base_reg!,TMPL_vfp_reg_list_d>		;3=rn!,{d..}
	shr	al,1
	jnc	VFP_rn_list_s.do
	or	ebp,1 shl 8
	jmp	VFP_rn_list_d.do

SIMD_INT_vd_rn_offset:
	;used by VLDR, VSTR
	call	decode_template
    TEMPLATE \
	<TMPL_vfps_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\			;0=sd,[rn]
	<TMPL_vfps_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\	;1=sd,[rn,imm]
	<TMPL_vfps_reg,TMPL_bracket_left,TMPL_address,TMPL_bracket_right>,\			;2=sd,[imm]	PC relative
	<TMPL_vfps_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>,\			;3=sd,[exp]	implicit reg from structure
	<TMPL_vfpd_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_bracket_right>,\			;4=dd,[rn]
	<TMPL_vfpd_reg,TMPL_bracket_left,TMPL_address_reg,TMPL_imm,TMPL_bracket_right>,\	;5=dd,[rn,imm]
	<TMPL_vfpd_reg,TMPL_bracket_left,TMPL_address,TMPL_bracket_right>,\			;6=dd,[imm]	PC relative
	<TMPL_vfpd_reg,TMPL_bracket_left,TMPL_expression,TMPL_bracket_right>			;7=dd,[exp]	implicit reg from structure
	cmp	al,3
	jbe	VFP_sd_rn_offset.do
	or	ebp,1 shl 8
	sub	al,4
	jmp	VFP_dd_rn_offset.do

SIMD_INT_vd_vn_vmx:
	;used by VMLA.S16, VMLA.S32, VMLA.U16, VMLA.U32, VMLA.I16, VMLA.I32,
	;	 VMLS.S16, VMLS.S32, VMLS.U16, VMLS.U32, VMLS.I16, VMLS.I32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg>,\	;0=dd,dn,dm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpdx_reg>,\	;1=dd,dn,dm[x]
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_simd_qreg>,\;2=qd,qn,qm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_vfpdx_reg>	;3=qd,qn,dm[x]
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_INT
	jz	ERROR_requires_copro_capability_simd_int
	shr	al,1
	jnc	SIMD_INT_vd_vn_vm.encode
	mov	ecx,ebp
	and	ebp,0x00300000
	and	ecx,0x01000000
	or	ebp,0xf2800040
	shr	ecx,24-10
	or	ebp,ecx
    .encode:
	mov	dl,[operand_register2]
	shr	dl,1
	sbb	cl,cl
	and	cl,0x10
	or	dl,cl
	mov	[operand_register2],dl
	movzx	eax,al
	shl	eax,24
	or	ebp,eax					;set Q
	mov	ecx,ebp
	shr	ecx,21
	and	ecx,1					;ecx=1 for size .32
	mov	eax,3
	mov	edx,8
	shr	eax,cl					;eax=maximum index value
	shl	edx,cl					;edx=maximum register + 1
	test	ecx,ecx
	mov	ecx,ERROR_element_value_out_of_range.0_1
	mov	ebx,ERROR_element_value_out_of_range.0_3
	cmovz	ecx,ebx
	cmp	[immediate_value],eax
	ja	ARM_post_process_simd_with_error
	mov	ecx,ERROR_scalar_register_out_of_range.0_7
	mov	ebx,ERROR_scalar_register_out_of_range.0_15
	cmp	edx,8
	cmovnz	ecx,ebx
	cmp	[operand_register2],dl
	jae	ARM_post_process_simd_with_error
	shr	edx,4
	mov	ecx,edx
	mov	eax,[immediate_value]
	shl	eax,cl
	mov	edx,eax
	and	edx,1
	shl	edx,3
	or	dl,[operand_register2]
	shr	eax,2
	rcl	edx,1
	mov	[operand_register2],dl
	xor	eax,eax
	jmp	SIMD_INT_vd_vn_vm.encode

SIMD_FLOAT_vd_vn_vmx_f32:
	;used by VMLA.F32, VMLS.F32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg>,\	;0=dd,dn,dm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpdx_reg>,\	;1=dd,dn,dm[x]
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_simd_qreg>,\;2=qd,qn,qm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_vfpdx_reg>,\;3=qd,qn,dm[x]
	<TMPL_vfps_reg,TMPL_vfps_reg,TMPL_vfps_reg>	;4=sd,sn,sm
	cmp	al,4
	jae	.vfp_alias
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_FLOAT
	jz	ERROR_requires_copro_capability_simd_float
	shr	al,1
	jnc	SIMD_INT_vd_vn_vm.encode
	and	ebp,0x00200000
	shr	ebp,21-10
	or	ebp,0xf2a00140
	jmp	SIMD_INT_vd_vn_vmx.encode
    .vfp_alias:
	movzx	ebp,[instruction_condition]
	and	ebp,0xf0
	shl	ebp,28-4
	or	ebp,[thumb32_instruction]
	mov	al,1
	jmp	VFP_sd_sn_sm.v1xd_test

SIMD_INT_vd_vn_vmx_long:
	;used by VMLAL.S16, VMLAL.S32, VMLAL.U16, VMLAL.U32, VMLSL.S16, VMLSL.S32, VMLSL.U16, VMLSL.U32,
	;	 VMULL.S16, VMULL.S32, VMULL.U16, VMULL.U32, VQDMLAL.S16, VQDMLAL.S32, VQDMLSL.S16, VQDMLSL.S32,
	;	 VQDMULL.S16, VQDMULL.S32
	call	decode_template
    TEMPLATE \
	<TMPL_simd_qreg,TMPL_vfpd_reg,TMPL_vfpd_reg>,\	;0=qd,dn,dm
	<TMPL_simd_qreg,TMPL_vfpd_reg,TMPL_vfpdx_reg>	;1=qd,dn,dm[x]
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_INT
	jz	ERROR_requires_copro_capability_simd_int
	shr	al,1
	jnc	SIMD_INT_vd_vn_vm.encode
	mov	ebp,[thumb32_instruction]
	jmp	SIMD_INT_vd_vn_vmx.encode

SIMD_INT_vd_imm_i8:
	;used by VMOV.I8, VMOV.I64
	call	decode_template
    TEMPLATE \
	<TMPL_simd_qreg,TMPL_imm>,\			;0=qd,imm
	<TMPL_vfpd_reg,TMPL_imm>			;1=dd,imm
	mov	ecx,[immediate_value]
	test	ecx,not 0xff
	jz	SIMD_INT_vd_imm.encode
	jmp	SIMD_INT_vd_imm.out_of_range

SIMD_INT_vd_imm_i16:
	;used by VMOV.I16, VMVN.I16
	call	decode_template
    TEMPLATE \
	<TMPL_simd_qreg,TMPL_imm>,\			;0=qd,imm
	<TMPL_vfpd_reg,TMPL_imm>			;1=dd,imm
	mov	ecx,[immediate_value]
	test	ecx,not 0xff
	jz	SIMD_INT_vd_imm.encode
	xor	ebp,0x00000200				;cmode=a
	ror	ecx,8
	test	ecx,not 0xff
	jz	SIMD_INT_vd_imm.encode
	xor	ebp,1 shl 5 + 0x00000200		;cmode=8, invert op, VMVN<-->VMOV
	mov	ecx,[immediate_value]
	not	cx
	test	ecx,not 0xff
	jz	SIMD_INT_vd_imm.encode
	xor	ebp,0x00000200				;cmode=a
	ror	ecx,8
	test	ecx,not 0xff
	jz	SIMD_INT_vd_imm.encode
	jmp	SIMD_INT_vd_imm.out_of_range

SIMD_INT_vd_imm_i32:
	;used by VMOV.I32, VMVN.I32
	call	decode_template
    TEMPLATE \
	<TMPL_simd_qreg,TMPL_imm>,\			;0=qd,imm
	<TMPL_vfpd_reg,TMPL_imm>			;1=dd,imm
	mov	ecx,[immediate_value]
	test	ecx,not 0xff
	jz	SIMD_INT_vd_imm.encode
	add	ebp,0x00000200				;cmode=2
	ror	ecx,8
	test	ecx,not 0xff
	jz	SIMD_INT_vd_imm.encode
	add	ebp,0x00000200				;cmode=4
	ror	ecx,8
	test	ecx,not 0xff
	jz	SIMD_INT_vd_imm.encode
	add	ebp,0x00000200				;cmode=6
	ror	ecx,8
	test	ecx,not 0xff
	jz	SIMD_INT_vd_imm.encode
	add	ebp,0x00000600				;cmode=c
	ror	ecx,8+8
	xor	ecx,0xff000000
	test	ecx,not 0xff
	jz	SIMD_INT_vd_imm.encode
	add	ebp,0x00000100				;cmode=d
	ror	ecx,8
	xor	ecx,0xff000000
	test	ecx,not 0xff
	jz	SIMD_INT_vd_imm.encode
	xor	ebp,1 shl 5 + 0x00000d00		;cmode=0, invert op, VMVN<-->VMOV
	mov	ecx,[immediate_value]
	not	ecx
	test	ecx,not 0xff
	jz	SIMD_INT_vd_imm.encode
	add	ebp,0x00000200				;cmode=2
	ror	ecx,8
	test	ecx,not 0xff
	jz	SIMD_INT_vd_imm.encode
	add	ebp,0x00000200				;cmode=4
	ror	ecx,8
	test	ecx,not 0xff
	jz	SIMD_INT_vd_imm.encode
	add	ebp,0x00000200				;cmode=6
	ror	ecx,8
	test	ecx,not 0xff
	jz	SIMD_INT_vd_imm.encode
	add	ebp,0x00000600				;cmode=c
	ror	ecx,8+8
	xor	ecx,0xff000000
	test	ecx,not 0xff
	jz	SIMD_INT_vd_imm.encode
	add	ebp,0x00000100				;cmode=d
	ror	ecx,8
	xor	ecx,0xff000000
	test	ecx,not 0xff
	jz	SIMD_INT_vd_imm.encode
	jmp	SIMD_INT_vd_imm.out_of_range

SIMD_FLOAT_dm_imm:
	;used by VMOV.F64
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_imm_float>,\		;0=dd,float
	<TMPL_vfpd_reg,TMPL_imm>,\			;1=dd,imm
	<TMPL_vfpd_reg,TMPL_vfpd_reg>			;2=dd,dm
	cmp	al,2
	jb	VFP_dm_imm.vesion_test
	or	ebp,0x00000040				;FCPYD
	jmp	VFP_dd_dm.encode

SIMD_FLOAT_sm_imm:
	;used by VMOV.F32
	call	decode_template
    TEMPLATE \
	<TMPL_vfps_reg,TMPL_imm_float>,\		;0=sd,float
	<TMPL_vfps_reg,TMPL_imm>,\			;1=sd,imm
	<TMPL_vfps_reg,TMPL_vfps_reg>,\			;2=sd,sm
	<TMPL_simd_qreg,TMPL_imm_float>,\		;3=qd,float
	<TMPL_simd_qreg,TMPL_imm>,\			;4=qd,imm
	<TMPL_vfpd_reg,TMPL_imm_float>,\		;5=dd,float
	<TMPL_vfpd_reg,TMPL_imm>			;6=dd,imm
	cmp	al,2
	jb	VFP_dm_imm.do
	lea	ebp,[ebp+0x00000040]			;FCPYS
	jz	VFP_sd_sm.v1xd_test
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_FLOAT
	jz	ERROR_requires_copro_capability_simd_float
	sub	al,3
	shr	al,1
	movzx	ebp,al
	jc	.constant_converted
	call	VFP_convert_single_to_quarter
    .constant_converted:
	mov	eax,ebp
	mov	ebp,[thumb32_instruction]
	mov	ecx,[immediate_value]
	test	ecx,not 0xff
	jz	SIMD_INT_vd_imm.encode
	jmp	SIMD_INT_vd_imm.out_of_range

SIMD_INT_rd_dnx:
	;used by VMOV.S8, VMOV.S16, VMOV.U8, VMOV.U16
	call	decode_template
    TEMPLATE \
	<TMPL_base_reg,TMPL_vfpdx_reg>			;0=rd,dn[x]
    .do:
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_INT
	jz	ERROR_requires_copro_capability_simd_int
	mov	edx,[immediate_value]
	mov	ecx,ebp
	shr	ecx,5
	and	ecx,1
	shl	edx,cl
	test	ecx,ecx
	mov	ecx,ERROR_element_value_out_of_range.0_3
	mov	ebx,ERROR_element_value_out_of_range.0_7
	cmovz	ecx,ebx
	cmp	edx,7
	ja	ARM_post_process_simd_with_error
	mov	ecx,edx
	and	edx,3
	and	ecx,4
	shl	edx,5
	shl	ecx,21-2
	or	ebp,edx
	or	ebp,ecx
	test	[code_type],CPU_ACTIVITY_ARM
	jnz	VFP_rd_dn.encode
	movzx	eax,[operand_register0]			;rd
	cmp	eax,13					;SP?
	jz	ERROR_r13_not_valid
	jmp	VFP_rd_dn.encode

SIMD_INT_ddx_rn:
	;used by VMOV.8, VMOV.16
	call	decode_template
    TEMPLATE \
	<TMPL_vfpdx_reg,TMPL_base_reg>			;0=dd[x],rn
	mov	ax,word[operand_registers+0]
	xchg	ah,al
	mov	word[operand_registers+0],ax
	jmp	SIMD_INT_rd_dnx.do

SIMD_INT_rdd_rdn:
	;used by VMOV.32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpdx_reg,TMPL_base_reg>,\		;0=dd[x],rn
	<TMPL_base_reg,TMPL_vfpdx_reg>			;1=rd,dn[x]
    .do:
	movzx	eax,al
	mov	cx,word[operand_registers+0]
	mov	dx,cx
	xchg	ch,cl
	shl	eax,20
	cmovz	ecx,edx
	mov	word[operand_registers+0],cx
	or	ebp,eax
	mov	edx,[immediate_value]
	cmp	edx,1
	mov	ecx,ERROR_element_value_out_of_range.0_1
	ja	ARM_post_process_simd_with_error
	shl	edx,21
	or	ebp,edx
	test	[code_type],CPU_ACTIVITY_ARM
	jnz	VFP_dn_rd.test_version
	movzx	eax,[operand_register0]			;rd
	cmp	eax,13					;SP?
	jz	ERROR_r13_not_valid
	jmp	VFP_dn_rd.test_version

	align 4
SIMD_INT_MOV_table:
	dd	0x0e000a10,VFP_sn_rd.do,0		;0=FMSR
	dd	0x0c400a10,SIMD_INT_MOV.msrr,0		;1=FMSRR
	dd	0x0e000b10,SIMD_INT_rdd_rdn.do,0	;2=VMOV.32
	dd	0xf2200110,SIMD_INT_MOV.vd_vm,0		;3=dd,dn
	dd	0x0c400b10,VFP_dm_rd_rn.test_version,0	;4=FMDRR
	dd	0x0e000b10,SIMD_INT_rdd_rdn.do,1	;5=VMOV.32
	dd	0x0e100a10,VFP_rd_dn.do,0		;6=FMRS
	dd	0x0c500a10,SIMD_INT_MOV.mrrs,0		;7=FMRRS
	dd	0x0c500b10,VFP_rd_rn_dm.test_version,0	;8=FMRRD
	dd	0xf2200150,SIMD_INT_MOV.vd_vm,0		;9=qd,qn

SIMD_INT_MOV:
	;used by VMOV
	call	decode_template
    TEMPLATE \
	<TMPL_vfps_reg,TMPL_base_reg>,\					;0=sn,rd
	<TMPL_vfps_reg,TMPL_vfps_reg,TMPL_base_reg,TMPL_base_reg>,\	;1=sd1,sd2,rn1,rn2
	<TMPL_vfpdx_reg,TMPL_base_reg>,\				;2=dd[x],rn
	<TMPL_vfpd_reg,TMPL_vfpd_reg>,\					;3=dd,dn
	<TMPL_vfpd_reg,TMPL_base_reg,TMPL_base_reg>,\			;4=dd,rn1,rn2
	<TMPL_base_reg,TMPL_vfpdx_reg>,\				;5=rd,dn[x]
	<TMPL_base_reg,TMPL_vfps_reg>,\					;6=rd,sn
	<TMPL_base_reg,TMPL_base_reg,TMPL_vfps_reg,TMPL_vfps_reg>,\	;7=rd1,rd2,sn1,sn2
	<TMPL_base_reg,TMPL_base_reg,TMPL_vfpd_reg>,\			;8=rd1,rd2,dn
	<TMPL_simd_qreg,TMPL_simd_qreg>					;9=qd,qn
	movzx	eax,al
	imul	eax,12
	movzx	ebp,[instruction_condition]
	and	ebp,0xf0
	shl	ebp,28-4
	or	ebp,[eax+SIMD_INT_MOV_table+0]
	mov	ecx,[eax+SIMD_INT_MOV_table+4]
	mov	eax,[eax+SIMD_INT_MOV_table+8]
	jmp	ecx
    .msrr:
	xor	edx,edx
	movzx	ecx,[operand_register0]
	bts	edx,ecx
	movzx	ecx,[operand_register1]
	bts	edx,ecx
	mov	[reg_list_bitmap],edx
	mov	ecx,[operand_registers]
	shr	ecx,16
	mov	[operand_registers],ecx
	jmp	VFP_rd_rn_sm.do
    .mrrs:
	xor	edx,edx
	movzx	ecx,[operand_register2]
	bts	edx,ecx
	movzx	ecx,[operand_register3]
	bts	edx,ecx
	mov	[reg_list_bitmap],edx
	jmp	VFP_rd_rn_sm.do
    .vd_vm:
	mov	cl,[operand_register1]
	mov	[operand_register2],cl
	jmp	SIMD_INT_vd_vn_vm.int_test

SIMD_INT_qd_dm:
	;used by VMOVL.S8, VMOVL.S16, VMOVL.S32, VMOVL.U8, VMOVL.U16, VMOVL.U32
	call	decode_template
    TEMPLATE \
	<TMPL_simd_qreg,TMPL_vfpd_reg>			;0=qd,dm
	jmp	SIMD_INT_vd_vm.int_test

SIMD_INT_dd_qm:
	;used by VMOVN.I8, VMOVN.I16, VMOVN.I32, VQMOVN.S16, VQMOVN.S32, VQMOVN.S64, VQMOVN.U16,
	;	 VQMOVN.U32, VQMOVN.U64, VQMOVUN.S16, VQMOVUN.S32, VQMOVUN.S64
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_simd_qreg>			;0=dd,qm
	jmp	SIMD_INT_vd_vm.int_test

SIMD_INT_vd_vn_vmx_alt:
	;used by VMUL.I16, VMUL.I32, VQDMULH.S16, VQDMULH.S32, VQRDMULH.S16, VQRDMULH.S32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpdx_reg>,\		;0=ddn,dm[x]
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpdx_reg>,\	;1=dd,dn,dm[x]
	<TMPL_vfpd_reg,TMPL_vfpd_reg>,\			;2=ddn,dm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg>,\	;3=dd,dn,dm
	<TMPL_simd_qreg,TMPL_vfpdx_reg>,\		;4=qdn,dm[x]
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_vfpdx_reg>,\;5=qd,qn,dm[x]
	<TMPL_simd_qreg,TMPL_simd_qreg>,\		;6=qdn,qm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_simd_qreg>	;7=qd,qn,qm
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_INT
	jz	ERROR_requires_copro_capability_simd_int
    .do:
	shr	al,1
	jc	.registers_okay
	mov	cx,word[operand_registers+0]
	mov	word[operand_registers+1],cx
    .registers_okay:
	shr	al,1
	jc	SIMD_INT_vd_vn_vm.encode
	mov	ebp,[thumb32_instruction]
	jmp	SIMD_INT_vd_vn_vmx.encode

SIMD_FLOAT_vd_vn_vmx_alt_f32:
	;used by VMUL.F32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpdx_reg>,\		;0=ddn,dm[x]
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpdx_reg>,\	;1=dd,dn,dm[x]
	<TMPL_vfpd_reg,TMPL_vfpd_reg>,\			;2=ddn,dm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg>,\	;3=dd,dn,dm
	<TMPL_simd_qreg,TMPL_vfpdx_reg>,\		;4=qdn,dm[x]
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_vfpdx_reg>,\;5=qd,qn,dm[x]
	<TMPL_simd_qreg,TMPL_simd_qreg>,\		;6=qdn,qm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_simd_qreg>,\;7=qd,qn,qm
	<TMPL_vfps_reg,TMPL_vfps_reg>,\			;8=sdn,sm
	<TMPL_vfps_reg,TMPL_vfps_reg,TMPL_vfps_reg>	;9=sd,sn,sm
	cmp	al,8
	jae	.vfp_alias
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_FLOAT
	jz	ERROR_requires_copro_capability_simd_float
	mov	[thumb32_instruction],0xf2a00940	;scalar
	jmp	SIMD_INT_vd_vn_vmx_alt.do
    .vfp_alias:
	movzx	ebp,[instruction_condition]
	and	ebp,0xf0
	shl	ebp,28-4
	or	ebp,[thumb32_instruction]
	sub	al,8
	jmp	VFP_sd_sn_sm.v1xd_test

SIMD_INT_dd_dn_dm_alt:
	;used by VPADD.I8, VPADD.I16, VPADD.I32, VPMAX.S8, VPMAX.S16, VPMAX.S32, VPMAX.U8, VPMAX.U16, VPMAX.U32
	;	 VPMIN.S8, VPMIN.S16, VPMIN.S32, VPMIN.U8, VPMIN.U16, VPMIN.U32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpd_reg>,\			;0=ddn,dm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg>	;1=dd,dn,dm
	jmp	SIMD_INT_vd_vn_vm_alt.encode

SIMD_FLOAT_dd_dn_dm_alt:
	;used by VPADD.F32, VPMAX.F32, VPMIN.F32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpd_reg>,\			;0=ddn,dm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg>	;1=dd,dn,dm
	jmp	SIMD_FLOAT_vd_vn_vm_alt.float_test

SIMD_INT_list:
	;used by VPOP, VPUSH
	call	decode_template
    TEMPLATE \
	<TMPL_vfp_reg_list_s>,\				;0={s..}
	<TMPL_vfp_reg_list_d>				;1={d..}
	cmp	al,0
	mov	[operand_register0],13			;SP
	mov	al,1					;rn!
	jz	VFP_rn_list_s.do
	or	ebp,0x00000100
	jmp	VFP_rn_list_d.do

SIMD_INT_dd_qm_imm:
	;used by VQRSHRN.S16, VQRSHRN.S32, VQRSHRN.S64, VQRSHRN.U16, VQRSHRN.U32, VQRSHRN.U64,
	;	 VQRSHRUN.S16, VQRSHRUN.S32, VQRSHRUN.S64, VQSHRN.S16, VQSHRN.S32, VQSHRN.S64,
	;	 VQSHRN.U16, VQSHRN.U32, VQSHRN.U64, VQSHRUN.S16, VQSHRUN.S32, VQSHRUN.S64,
	;	 VRSHRUN.I16, VRSHRUN.I32, VRSHRUN.I64, VSHRN.I16, VSHRN.I32, VSHRN.I64
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_simd_qreg,TMPL_imm>		;0=dd,qm,imm
    .encode:
	mov	ecx,ebp
	mov	edx,ebp
	shr	ecx,16
	shr	edx,7-6
	and	ecx,0x3f
	and	edx,0x40
	or	ecx,edx
	mov	edx,ecx
	sub	ecx,[immediate_value]
	shl	ecx,16
	or	ebp,ecx
	shr	ecx,16
	dec	edx
	not	edx
	test	ecx,edx
	jz	SIMD_INT_vd_vm.int_test
	mov	ecx,ERROR_shift_value_out_of_range.1_8
	mov	eax,ERROR_shift_value_out_of_range.1_16
	test	edx,1 shl 3
	cmovz	ecx,eax
	mov	eax,ERROR_shift_value_out_of_range.1_32
	test	edx,1 shl 4
	cmovz	ecx,eax
	mov	eax,ERROR_shift_value_out_of_range.1_64
	test	edx,1 shl 5
	cmovz	ecx,eax
	jmp	ARM_post_process_simd_with_error

SIMD_INT_vd_vn_vm_imm_alt:
	;used by VQSHL.S8, VQSHL.S16, VQSHL.S32, VQSHL.S64, VQSHL.U8, VQSHL.U16, VQSHL.U32, VQSHL.U64
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_imm>,\			;0=ddm,imm
	<TMPL_vfpd_reg,TMPL_vfpd_reg>,\			;1=ddn,dm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_imm>,\	;2=dd,dm,imm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg>,\	;3=dd,dn,dm
	<TMPL_simd_qreg,TMPL_imm>,\			;4=qdm,imm
	<TMPL_simd_qreg,TMPL_simd_qreg>,\		;5=qdn,qm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_imm>,\	;6=qd,qm,imm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_simd_qreg>	;7=qd,qn,qm
	shr	al,1
	jc	SIMD_INT_vd_vn_vm_alt.encode
	mov	ebp,[thumb32_instruction]
    .encode:
	shr	al,1
	jc	.registers_okay
	mov	cl,[operand_register0]
	mov	[operand_register1],cl
    .registers_okay:
	mov	ecx,ebp
	mov	edx,ebp
	shr	ecx,16
	shr	edx,7-6
	and	ecx,0x3f
	and	edx,0x40
	or	edx,ecx
	mov	ecx,[immediate_value]
	shl	ecx,16
	or	ebp,ecx
	cmp	edx,[immediate_value]
	ja	SIMD_INT_vd_vm.int_test
	mov	ecx,ERROR_shift_value_out_of_range.0_7
	mov	eax,ERROR_shift_value_out_of_range.0_15
	test	edx,1 shl 4
	cmovnz	ecx,eax
	mov	eax,ERROR_shift_value_out_of_range.0_31
	test	edx,1 shl 5
	cmovnz	ecx,eax
	mov	eax,ERROR_shift_value_out_of_range.0_63
	test	edx,1 shl 6
	cmovnz	ecx,eax
	jmp	ARM_post_process_simd_with_error

SIMD_INT_vd_vm_imm_alt:
	;used by VQSHLU.S8, VQSHLU.S16, VQSHLU.S32, VQSHLU.S64,
	;	 VSHL.I8, VSHL.I16, VSHL.I32, VSHL.I64, VSLI.8, VSLI.16, VSLI.32, VSLI.64
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_imm>,\			;0=ddm,imm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_imm>,\	;1=dd,dm,imm
	<TMPL_simd_qreg,TMPL_imm>,\			;2=qdm,imm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_imm>	;3=qd,qm,imm
	jmp	SIMD_INT_vd_vn_vm_imm_alt.encode

SIMD_INT_vd_vm_imm_alt_neg:
	;used by VRSHR.S8, VRSHR.S16, VRSHR.S32, VRSHR.S64, VRSHR.U8, VRSHR.U16, VRSHR.U32, VRSHR.U64,
	;	 VRSRA.S8, VRSRA.S16, VRSRA.S32, VRSRA.S64, VRSRA.U8, VRSRA.U16, VRSRA.U32, VRSRA.U64,
	;	 VSHR.S8, VSHR.S16, VSHR.S32, VSHR.S64, VSHR.U8, VSHR.U16, VSHR.U32, VSHR.U64,
	;	 VSRA.S8, VSRA.S16, VSRA.S32, VSRA.S64, VSRA.U8, VSRA.U16, VSRA.U32, VSRA.U64,
	;	 VSRI.8, VSRI.16, VSRI.32, VSRI.64
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_imm>,\			;0=ddm,imm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_imm>,\	;1=dd,dm,imm
	<TMPL_simd_qreg,TMPL_imm>,\			;2=qdm,imm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_imm>	;3=qd,qm,imm
	shr	al,1
	jc	.registers_okay
	mov	cl,[operand_register0]
	mov	[operand_register1],cl
    .registers_okay:
	jmp	SIMD_INT_dd_qm_imm.encode

SIMD_FLOAT_vd_vm:
	;used by VRECPE.F32, VRSQRTE.F32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpd_reg>,\			;0=dd,dm
	<TMPL_simd_qreg,TMPL_simd_qreg>			;1=qd,qm
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_FLOAT
	jz	ERROR_requires_copro_capability_simd_float
	jmp	SIMD_INT_vd_vm.encode

SIMD_INT_qd_dm_imm:
	;used by VSHLL.S8, VSHLL.S16, VSHLL.S32, VSHLL.U8, VSHLL.U16, VSHLL.U32
	call	decode_template
    TEMPLATE \
	<TMPL_simd_qreg,TMPL_vfpd_reg,TMPL_imm>		;0=qd,qm,imm
	mov	ecx,[immediate_value]
	mov	edx,ebp
	and	edx,0x3f shl 16
	rol	ecx,16
	or	ebp,ecx
	cmp	ecx,edx					;imm=size?
	jz	.imm_eq_size
	dec	edx
	and	edx,0x3f shl 16
	not	edx
	test	ecx,edx
	jz	SIMD_INT_vd_vm.int_test
	mov	ecx,ERROR_shift_value_out_of_range.0_8
	mov	eax,ERROR_shift_value_out_of_range.0_16
	test	edx,1 shl (3+16)
	cmovz	ecx,eax
	mov	eax,ERROR_shift_value_out_of_range.0_32
	test	edx,1 shl (4+16)
	cmovz	ecx,eax
	jmp	ARM_post_process_simd_with_error
    .imm_eq_size:
	mov	ebp,[thumb32_instruction]
	jmp	SIMD_INT_vd_vm.int_test

SIMD_INT_qd_dm_imm_I:
	;used by VSHLL.I8, VSHLL.I16, VSHLL.I32
	call	decode_template
    TEMPLATE \
	<TMPL_simd_qreg,TMPL_vfpd_reg,TMPL_imm>		;0=qd,qm,imm
	mov	ecx,ebp
	shr	ecx,18
	and	ecx,3
	mov	edx,8
	shl	edx,cl
	mov	ecx,[immediate_value]
	cmp	ecx,edx					;imm=size?
	jz	SIMD_INT_vd_vm.int_test
	mov	ecx,ERROR_shift_value_out_of_range.8
	mov	eax,ERROR_shift_value_out_of_range.16
	cmp	edx,16
	cmovz	ecx,eax
	mov	eax,ERROR_shift_value_out_of_range.32
	cmp	edx,32
	cmovz	ecx,eax
	jmp	ARM_post_process_simd_with_error

SIMD_dn_list_dm:
	;used by VTBL.8, VTBX.8
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfp_reg_list_d,TMPL_comma,TMPL_vfpd_reg>	;0=dd,{dn,..},dm
	mov	edx,[reg_list_bitmap]
	bsf	eax,edx
	bsr	ecx,edx
    ;check for a contiguous set
	xor	ecx,31
	shl	edx,cl
	add	ecx,eax
	sar	edx,cl
	inc	edx
	mov	ecx,ERROR_non_contiguous_set
	jnz	ARM_post_process_simd_with_error
	mov	ecx,[reg_list_bitmap]
	bsf	eax,ecx					;eax=start register
	bsr	edx,ecx
	sub	edx,eax					;edx=register count - 1
	cmp	edx,3
	mov	ecx,ERROR_register_list_invalid		;too many registers
	ja	ARM_post_process_simd_with_error
	shl	edx,8
	or	ebp,edx
	mov	cl,[operand_register1]			;dm
	mov	[operand_register2],cl			;dm
	add	eax,eax
	mov	ah,al
	shr	ah,5
	and	ax,0x011e
	or	al,ah
	mov	[operand_register1],al			;dn
	xor	eax,eax
	jmp	SIMD_INT_vd_vn_vm.int_test

SIMD_FLOAT_dd_sm_CVT:
	;used by VCVT.F64.S32, VCVT.F64.U32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfps_reg>,\			;0=dd,sm
	<TMPL_vfpd_reg,TMPL_imm>,\			;1=ddm,imm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_imm>		;2=dd,dm,imm
	sub	al,1
	jc	VFP_dd_sm.do
	movzx	ebp,[instruction_condition]
	and	ebp,0xf0
	shl	ebp,28-4
	or	ebp,[thumb32_instruction]
	jmp	VFP_dd_dd_imm.test_version

SIMD_FLOAT_sd_dm_CVT:
	;used by VCVT.S32.F64, VCVT.U32.F64,
	call	decode_template
    TEMPLATE \
	<TMPL_vfps_reg,TMPL_vfpd_reg>,\			;0=sd,dm
	<TMPL_vfpd_reg,TMPL_imm>,\			;1=ddm,imm
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_imm>		;2=dd,dm,imm
	sub	al,1
	jc	VFP_sd_dm.do
	movzx	ebp,[instruction_condition]
	and	ebp,0xf0
	shl	ebp,28-4
	or	ebp,[thumb32_instruction]
	jmp	VFP_dd_dd_imm.test_version

VFP_FLOAT_dd_dn_dm:
	;used by FMACD, FNMACD, FMSCD, FNMSCD, VNMLA.F64, VNMLS.F64
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg>	;0=dd,dn,dm
	mov	al,1
	jmp	VFP_dd_dn_dm.begin

VFP_FLOAT_sd_sn_sm:
	;used by FMACS, FNMACS, FMSCS, FNMSCS, VNMLA.F32, VNMLS.F32
	call	decode_template
    TEMPLATE \
	<TMPL_vfps_reg,TMPL_vfps_reg,TMPL_vfps_reg>	;0=sd,sn,sm
	mov	al,1
	jmp	VFP_sd_sn_sm.v1xd_test

;VFPv4 and SIMDv2

SIMD_FLOAT_vd_vn_vm:
	;used by VFMA.F32, VFMS.F32
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg>,\	;0=dd,dn,dm
	<TMPL_simd_qreg,TMPL_simd_qreg,TMPL_simd_qreg>,\;1=qd,qn,qm
	<TMPL_vfps_reg,TMPL_vfps_reg,TMPL_vfps_reg>	;2=sd,sn,sm
	cmp	al,2
	jb	.simdv2_test
    .vfpv4_test:
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_V4
	jz	ERROR_requires_copro_capability_vfp_v4
	movzx	ebp,[instruction_condition]
	and	ebp,0xf0
	shl	ebp,28-4
	or	ebp,[thumb32_instruction]
	jmp	VFP_sd_sn_sm.registers_shifted
    .simdv2_test:
	test	[copro_capability_flags],COPRO_CAPABILITY_SIMD_V2
	jz	ERROR_requires_copro_capability_simd_v2
	jmp	SIMD_INT_vd_vn_vm.encode

SIMD_FLOAT_dd_dn_dm:
	;used by VFMA.F64, VFMS.F64, VFNMA.F64, VFNMS.F64
	call	decode_template
    TEMPLATE \
	<TMPL_vfpd_reg,TMPL_vfpd_reg,TMPL_vfpd_reg>	;0=dd,dn,dm
	test	[copro_capability_flags],COPRO_CAPABILITY_VFP_D32
	jnz	SIMD_FLOAT_vd_vn_vm.vfpv4_test
	mov	edx,[operand_registers]			;fd,fn,fm
	test	edx,0x010101
	jnz	ERROR_requires_copro_capability_vfp_d32
	mov	al,1
	jmp	SIMD_FLOAT_vd_vn_vm.vfpv4_test

SIMD_FLOAT_sd_sn_sm:
	;used by VFNMA.F32, VFNMS.F32
	call	decode_template
    TEMPLATE \
	<TMPL_vfps_reg,TMPL_vfps_reg,TMPL_vfps_reg>	;0=sd,sn,sm
	mov	al,1
	jmp	SIMD_FLOAT_vd_vn_vm.vfpv4_test

;v7VE

ARM_eret:
	;used by ERET
	call	decode_template
    TEMPLATE \
	<TMPL_EOL>					;0=empty
	test	[cpu_capability_flags],CPU_CAPABILITY_VE
	jz	ERROR_requires_cpu_capability_arm_ve
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB_eret
	jmp	ARM_store_instruction
    .THUMB_eret:
	or	[thumb_flags_16],THUMB_FLAG_ONLY_LAST_IT
	or	[thumb_flags_32],THUMB_FLAG_ONLY_LAST_IT
	mov	[thumb32_error],0
	jmp	THUMB_post_process

ARM_immediate16_ve:
	;used by HVC
	call	decode_template
    TEMPLATE \
	<TMPL_imm>					;0=imm
	test	[cpu_capability_flags],CPU_CAPABILITY_VE
	jz	ERROR_requires_cpu_capability_arm_ve
	test	[code_type],CPU_ACTIVITY_ARM
	jz	.THUMB
	mov	eax,[immediate_value]
	cmp	eax,0x0000ffff
	ja	.out_of_range
	mov	ecx,eax
	and	eax,0xf
	and	ecx,0xfff0
	shl	ecx,4
	or	eax,ecx
	or	ebp,eax
	jmp	ARM_store_instruction
    .out_of_range:
	mov	ecx,ERROR_immediate_offset_out_of_range.0_0xffff
	jmp	ARM_store_instruction_with_error
    .THUMB:
	or	[thumb_flags_16],THUMB_FLAG_NOT_INSIDE_IT
	or	[thumb_flags_32],THUMB_FLAG_NOT_INSIDE_IT
	mov	[thumb32_error],ERROR_immediate_offset_out_of_range.0_0xffff
	mov	eax,[immediate_value]
	cmp	eax,0xffff
	ja	THUMB_post_process
	mov	ecx,eax
	and	eax,0xfff
	and	ecx,0xf000
	shl	ecx,4
	or	eax,ecx
	or	[thumb32_instruction],eax
	mov	[thumb32_error],0
	jmp	THUMB_post_process

;CRC

CRC32_polynomial_ISO3309	= 0xedb88320	;x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x^1 + x^0
CRC32_table_length		= 256*4*4	;a constant, don't adjust
CRC32_loop_size			= 4		;an optimising parameter, adjust if you want

CRC_32:
	;used by CRC32
	call	decode_template
    TEMPLATE \
	<TMPL_imm,TMPL_comma,TMPL_imm2>			;0=start,length
	call	CRC32_construct_table
	mov	edx,[immediate_value]			;edx=buffer
	mov	ecx,[immediate_value2]			;ecx=length
	mov	ebx,[addressing_space]
	add	edx,[ebx+0x00]
	cmp	edx,[ebx+0x18]
	jb	.out_of_range
	cmp	edx,edi
	ja	.out_of_range
	lea	eax,[edx+ecx]
	cmp	eax,edi
	ja	.out_of_range
	or	eax,-1					;eax=crc
	test	ecx,ecx
	jz	.done
	test	dl,3
	jne	.align
    .begin:
	sub	ecx,CRC32_loop_size*8
	jb	.tail
    .main_loop:
	 repeat CRC32_loop_size
	xor	eax,[edx+(%-1)*8]
	movzx	ebp,al
	mov	ebx,[edi+3*256*4+4*ebp]
	movzx	ebp,ah
	xor	ebx,[edi+2*256*4+4*ebp]
	shr	eax,16
	movzx	ebp,al
	xor	ebx,[edi+1*256*4+4*ebp]
	shr	eax,8
	xor	ebx,[edi+0*256*4+4*eax]
	xor	ebx,[edx+(%-1)*8+4]
	movzx	ebp,bl
	mov	eax,[edi+3*256*4+4*ebp]
	movzx	ebp,bh
	xor	eax,[edi+2*256*4+4*ebp]
	shr	ebx,16
	movzx	ebp,bl
	xor	eax,[edi+1*256*4+4*ebp]
	shr	ebx,8
	xor	eax,[edi+0*256*4+4*ebx]
	 end repeat
	add	edx,CRC32_loop_size*8
	sub	ecx,CRC32_loop_size*8
	jae	.main_loop
    .tail:
	sub	ecx,4-CRC32_loop_size*8
	jb	.finish
    .tail_loop:
	xor	eax,[edx]
	add	edx,4
	movzx	ebp,al
	mov	ebx,[edi+3*256*4+4*ebp]
	movzx	ebp,ah
	xor	ebx,[edi+2*256*4+4*ebp]
	shr	eax,16
	movzx	ebp,al
	xor	ebx,[edi+1*256*4+4*ebp]
	shr	eax,8
	xor	ebx,[edi+0*256*4+4*eax]
	mov	eax,ebx
	sub	ecx,4
	jae	.tail_loop
    .finish:
	sub	ecx,1-4
	jb	.done
    .prolog:
	movzx	ebx,byte[edx]
	xor	bl,al
	shr	eax,8
	xor	eax,[edi+4*ebx]
	inc	edx
	dec	ecx
	jns	.prolog
    .done:
	mov	[edi],eax
	add	edi,4
	jmp	instruction_assembled
    .align:
	movzx	ebx,byte[edx]
	xor	bl,al
	shr	eax,8
	xor	eax,[edi+4*ebx]
	inc	edx
	dec	ecx
	je	.done
	test	dl,3
	jne	.align
	jmp	.begin
    .out_of_range:
	mov	ecx,ERROR_immediate_offset_out_of_range
	jmp	ARM_store_instruction_with_error

CRC32_construct_table:
	lea	eax,[edi+CRC32_table_length]
	cmp	eax,[tagged_blocks]
	jae	out_of_memory
	mov	ebp,CRC32_polynomial_ISO3309
	xor	edx,edx
    .a:	mov	eax,edx
	mov	ebx,8
    .b:	shr	eax,1
	sbb	ecx,ecx
	and	ecx,ebp
	xor	eax,ecx
	dec	ebx
	jnz	.b
	mov	[edi+4*edx],eax
	inc	dl
	jnz	.a
	xor	edx,edx
    .c:	mov	eax,[edi+4*edx]
	mov	ebx,3
	lea	ebp,[edi+4*edx+1*256*4]
    .d:	movzx	ecx,al
	mov	ecx,[edi+4*ecx]
	shr	eax,8
	xor	eax,ecx
	mov	[ebp],eax
	add	ebp,1*256*4
	dec	ebx
	jnz	.d
	inc	dl
	jnz	.c
	ret

;shared errors

ERROR_illegal_instruction		=illegal_instruction
ERROR_value_out_of_range		=value_out_of_range
ERROR_relative_jump_out_of_range	=relative_jump_out_of_range
ERROR_operand_sizes_do_not_match	=operand_sizes_do_not_match

ERROR_code_is_not_in_a_section:
	call	assembler_error
	db	'Code placed before section defined',0
ERROR_expecting_section_name:
	call	assembler_error
	db	'Section directive must be followed by the section name',0
ERROR_duplicate_align_setting:
	call	assembler_error
	db	'An alignment setting was specified twice',0
ERROR_duplicate_flag_setting:
	call	assembler_error
	db	'A flag setting was specified twice',0
ERROR_invalid_use_of_symbol_in_align:
	call	assembler_error
	db	'Invalid use of symbol for "align" setting',0
ERROR_invalid_use_of_symbol_in_at:
	call	assembler_error
	db	'Invalid use of symbol for "at" setting',0
ERROR_section_flags_zero:
	call	assembler_error
	db	'At least one section flag (executable, readable or writeable) must be specified',0
ERROR_section_align_zero:
	call	assembler_error
	db	'Section alignment must be specified',0
ERROR_line_processing_error:
	call	fatal_error
	db	'Fatal: Failure when processing line numbers',0
ERROR_org_not_allowed:
	call	assembler_error
	db	'"ORG" not allowed in DWARF, use "SECTION ... at X" instead',0
ERROR_section_at_not_aligned:
	call	assembler_error
	db	'Section alignement and "AT" specified are not compatible',0
ERROR_instruction_not_aligned:
	call	assembler_error
	db	'Instruction origin not aligned',0
ERROR_relative_jump_not_aligned:
	call	assembler_error
	db	'Relative jump destination not aligned',0
ERROR_shift_value_out_of_range:
    .0_3:
	call	assembler_error
	db	'Shift value out of range (0-3)',0
    .0_7:
	call	assembler_error
	db	'Shift value out of range (0-7)',0
    .0_8:
	call	assembler_error
	db	'Shift value out of range (0-8)',0
    .0_15:
	call	assembler_error
	db	'Shift value out of range (0-15)',0
    .0_16:
	call	assembler_error
	db	'Shift value out of range (0-16)',0
    .0_24:
	call	assembler_error
	db	'Shift value out of range (0, 8, 16 or 24)',0
    .0_31:
	call	assembler_error
	db	'Shift value out of range (0-31)',0
    .0_32:
	call	assembler_error
	db	'Shift value out of range (0-32)',0
    .1:
	call	assembler_error
	db	'Shift value out of range (must be 1)',0
    .2:
	call	assembler_error
	db	'Shift value out of range (must be 2)',0
    .8:
	call	assembler_error
	db	'Shift value out of range (must be 8)',0
    .16:
	call	assembler_error
	db	'Shift value out of range (must be 16)',0
    .32:
	call	assembler_error
	db	'Shift value out of range (must be 32)',0
    .1_8:
	call	assembler_error
	db	'Shift value out of range (1-8)',0
    .1_16:
	call	assembler_error
	db	'Shift value out of range (1-16)',0
    .1_31:
	call	assembler_error
	db	'Shift value out of range (1-31)',0
    .1_32:
	call	assembler_error
	db	'Shift value out of range (1-32)',0
    .0_63:
	call	assembler_error
	db	'Shift value out of range (0-63)',0
    .1_64:
	call	assembler_error
	db	'Shift value out of range (1-64)',0
ERROR_immediate_offset_out_of_range:
	call	assembler_error
	db	'Immediate offset out of range',0
    .0:
	call	assembler_error
	db	'Immediate offset out of range (must be 0)',0
    .1:
	call	assembler_error
	db	'Immediate offset out of range (must be 1)',0
    .0xff:
	call	assembler_error
	db	'Immediate offset out of range (-0xff to 0xff)',0
    .0x3fc:
	call	assembler_error
	db	'Immediate offset out of range (-0xff to 0xff)*4',0
    .0xfff:
	call	assembler_error
	db	'Immediate offset out of range (-0xfff to 0xfff)',0
    .0_0xffffff:
	call	assembler_error
	db	'Immediate offset out of range (0 to 0xffffff)',0
    .0_0xff:
	call	assembler_error
	db	'Immediate offset out of range (0 to 0xff)',0
    .0_0xfff:
	call	assembler_error
	db	'Immediate offset out of range (0 to 0xfff)',0
    .0_0xffff:
	call	assembler_error
	db	'Immediate offset out of range (0 to 0xffff)',0
    .0_0x1f:
	call	assembler_error
	db	'Immediate offset out of range (0 to 0x1f)',0
    .0_0x3e:
	call	assembler_error
	db	'Immediate offset out of range (0 to 0x1f)*2',0
    .0_0x7c:
	call	assembler_error
	db	'Immediate offset out of range (0 to 0x1f)*4',0
    .0_0xfc:
	call	assembler_error
	db	'Immediate offset out of range (0 to 0x3f)*4',0
    .0_0x1fc:
	call	assembler_error
	db	'Immediate offset out of range (0 to 0x7f)*4',0
    .0_0x3fc:
	call	assembler_error
	db	'Immediate offset out of range (0 to 0xff)*4',0
    .m28_0:
	call	assembler_error
	db	'Immediate offset out of range (-7 to 0)*4',0
ERROR_immediate_value_out_of_range:
    .0_7:
	call	assembler_error
	db	'Immediate value out of range (0 to 7)',0
    .0_15:
	call	assembler_error
	db	'Immediate value out of range (0 to 15)',0
    .0_16:
	call	assembler_error
	db	'Immediate value out of range (0 to 16)',0
    .0_31:
	call	assembler_error
	db	'Immediate value out of range (0 to 31)',0
    .0_0xff:
	call	assembler_error
	db	'Immediate value out of range (0 to 0xff)',0
    .0_0xffff:
	call	assembler_error
	db	'Immediate value out of range (0 to 0xffff)',0
    .1_16:
	call	assembler_error
	db	'Immediate value out of range (1 to 16)',0
    .1_32:
	call	assembler_error
	db	'Immediate value out of range (1 to 32)',0
    .m64_63:
	call	assembler_error
	db	'Immediate value out of range (-64 to 63)',0
ERROR_byte_value_out_of_range:
	call	assembler_error
	db	'Byte value out of range (0 to 0xff)',0
ERROR_rotation_value_out_of_range:
	call	assembler_error
	db	'Rotation value out of range (0 to 0xf)*2',0
ERROR_byte_rotation_in_thumb:
	call	assembler_error
	db	'Explicit byte and rotation values not valid in THUMB mode',0
ERROR_count_value_out_of_range:
    .1_4:
	call	assembler_error
	db	'Count value out of range (1 to 4)',0
ERROR_lsb_out_of_range.0_31:
	call	assembler_error
	db	'LSB value out of range (0 to 31)',0
ERROR_width_out_of_range:
	call	assembler_error
	db	'Width field + LSB cannot exceed 32',0
ERROR_constant_cannot_be_encoded:
	call	assembler_error
	db	'Constant cannot be encoded (0.0, 0.5, 1.0, 2.0, 3.0, 4.0, 5.0 or 10.0 only)',0
ERROR_immediate_cannot_be_encoded:
	call	assembler_error
	db	'Immediate value cannot be encoded',0
ERROR_must_have_writeback_operator:
	call	assembler_error
	db	'Must have writeback operator "!"',0
ERROR_option_out_of_range:
	call	assembler_error
	db	'Option out of range (0x00-0xff)',0
ERROR_opcode4_out_of_range:
	call	assembler_error
	db	'Coprocessor opcode out of range (0-15) in second parameter',0
ERROR_opcode3_out_of_range:
	call	assembler_error
	db	'Coprocessor opcode out of range (0-7) in sixth parameter',0
ERROR_expecting_curly_bracket_close:
	call	assembler_error
	db	'Expecting curly bracket "}"',0
ERROR_expecting_square_bracket_close:
	call	assembler_error
	db	'Expecting square bracket "]"',0
ERROR_repeated_register_in_list:
	call	assembler_error
	db	'Repeated register in list',0
ERROR_instruction_not_16bit:
	call	assembler_error
	db	'Instruction not encodable in THUMB',0
ERROR_instruction_not_t2ee:
	call	assembler_error
	db	'Instruction not encodable in THUMBEE',0
ERROR_instruction_needs_t2ee:
	call	assembler_error
	db	'Instruction requires THUMBEE mode, use "THUMBEE" to select',0
ERROR_instruction_not_32bit:
	call	assembler_error
	db	'Instruction not encodable in ARM',0
ERROR_register_out_of_range:
    .r0_r7:
	call	assembler_error
	db	'Register out of range (R0-R7)',0
    .r0_r7_sp_pc:
	call	assembler_error
	db	'Register out of range (R0-R7, SP or PC)',0
    .must_be_sp:
	call	assembler_error
	db	'Register must be SP',0
    .sp:
	call	assembler_error
	db	'Register cannot be SP',0
    .pc:
	call	assembler_error
	db	'Register cannot be PC',0
    .sp_pc:
	call	assembler_error
	db	'Register cannot be SP or PC',0
ERROR_dest_cannot_be_sp_pc:
	call	assembler_error
	db	'Destination register cannot be SP or PC',0
ERROR_offset_register_cannot_be_sp_pc:
	call	assembler_error
	db	'Offset register cannot be SP or PC',0
ERROR_dest_and_source_must_be_the_same:
	call	assembler_error
	db	'Destination register and first source register must be the same',0
ERROR_r14_not_valid:
    .first:
	call	assembler_error
	db	'LR (R14) not valid as first parameter',0
    .second:
	call	assembler_error
	db	'LR (R14) not valid as second parameter',0
ERROR_r13_not_valid:
	call	assembler_error
	db	'SP (R13) not valid',0
    .second:
	call	assembler_error
	db	'SP (R13) not valid as second parameter',0
ERROR_r13_r15_not_valid:
	call	assembler_error
	db	'SP (R13) and PC (R15) not valid',0
    .third:
	call	assembler_error
	db	'SP (R13) and PC (R15) not valid as third parameter',0
ERROR_r15_not_valid:
	call	assembler_error
	db	'PC (R15) not valid',0
    .all:
	call	assembler_error
	db	'PC (R15) not valid for any parameter',0
    .first:
	call	assembler_error
	db	'PC (R15) not valid as first parameter',0
    .second:
	call	assembler_error
	db	'PC (R15) not valid as second parameter',0
    .third:
	call	assembler_error
	db	'PC (R15) not valid as third parameter',0
    .fourth:
	call	assembler_error
	db	'PC (R15) not valid as fourth parameter',0
    .base:
	call	assembler_error
	db	'PC (R15) not valid as base register',0
    .post:
	call	assembler_error
	db	'PC (R15) not valid as base register with post update',0
ERROR_register_writeback_not_allowed:
	call	assembler_error
	db	'Register writeback not allowed here',0
ERROR_destination_register_not_allowed_in_list:
	call	assembler_error
	db	'Destination register with writeback not allowed in source list',0
ERROR_source_and_dest_must_differ:
	call	assembler_error
	db	'Source and destination registers must differ',0
ERROR_base_and_dest_must_differ_with_writeback:
	call	assembler_error
	db	'Base and destination registers must differ when using writeback',0
ERROR_offset_and_dest_must_differ_with_LDRD:
	call	assembler_error
	db	'Offset and destination registers must differ with LDRD',0
ERROR_source_register_must_be_even:
	call	assembler_error
	db	'First source register must be even',0
ERROR_destination_register_must_be_even:
	call	assembler_error
	db	'First destination register must be even',0
ERROR_source_registers_must_be_consecutive:
	call	assembler_error
	db	'Source registers must be consecutive',0
ERROR_destination_registers_must_be_consecutive:
	call	assembler_error
	db	'Destination registers must be consecutive',0
ERROR_source_rm_and_dest_must_differ:
	call	assembler_error
	db	'Destination and first source must differ',0
ERROR_base_and_offset_must_differ_with_writeback:
	call	assembler_error
	db	'Base and offset registers must differ when using writeback',0
ERROR_must_use_full_psr:
	call	assembler_error
	db	'Must use full PSR here, either APSR, CPSR or SPSR',0
ERROR_must_specify_psr_bit_fields:
	call	assembler_error
	db	'Must specify PSR bit fields here, (APSR_nzcvqg, CPSR_fsxc or SPSR_fsxc)',0
ERROR_memory_address_cannot_be_source_or_dest:
	call	assembler_error
	db	'Address register cannot be source or destination',0
ERROR_dest_cannot_be_source_or_memory_address:
	call	assembler_error
	db	'Destination cannot be source or address register ',0
ERROR_destination_registers_must_differ:
	call	assembler_error
	db	'Destination registers must differ',0
ERROR_empty_set:
	call	assembler_error
	db	'Cannot use empty set',0
ERROR_sp_in_set:
	call	assembler_error
	db	'SP (R13) not valid in register list',0
ERROR_pc_in_set:
	call	assembler_error
	db	'PC (R15) not valid in register list',0
ERROR_invalid_set_with_lr_pc:
	call	assembler_error
	db	'Cannot have LR (R14) and PC (R15) both in the list',0
ERROR_shift_type_must_be_lsl:
	call	assembler_error
	db	'Shift type must be LSL',0
ERROR_shift_type_must_be_lsl_or_asr:
	call	assembler_error
	db	'Shift type must be LSL or ASR',0
ERROR_requires_cpu_capability_arm_26bit:
	call	assembler_error
	db	'Requires CPU capability 26BIT, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_v1:
	call	assembler_error
	db	'Requires CPU capability V1, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_v2:
	call	assembler_error
	db	'Requires CPU capability V2, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_v2a:
	call	assembler_error
	db	'Requires CPU capability A, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_v3:
	call	assembler_error
	db	'Requires CPU capability V3, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_m:
	call	assembler_error
	db	'Requires CPU capability M, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_v4:
	call	assembler_error
	db	'Requires CPU capability V4, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_v4t:
	call	assembler_error
	db	'Requires CPU capability V4T, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_v5:
	call	assembler_error
	db	'Requires CPU capability V5, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_v5t:
	call	assembler_error
	db	'Requires CPU capability V5T, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_p:
	call	assembler_error
	db	'Requires CPU capability P, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_e:
	call	assembler_error
	db	'Requires CPU capability E, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_j:
	call	assembler_error
	db	'Requires CPU capability J, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_x:
	call	assembler_error
	db	'Requires CPU capability X, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_v6:
	call	assembler_error
	db	'Requires CPU capability V6, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_v6t:
	call	assembler_error
	db	'Requires CPU capability V6T, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_align:
	call	assembler_error
	db	'Requires CPU capability ALIGN, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_k:
	call	assembler_error
	db	'Requires CPU capability K, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_z:
	call	assembler_error
	db	'Requires CPU capability Z, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_6m:
	call	assembler_error
	db	'Requires CPU capability 6M, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_7m:
	call	assembler_error
	db	'Requires CPU capability 7M, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_t2:
	call	assembler_error
	db	'Requires CPU capability T2, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_v7:
	call	assembler_error
	db	'Requires CPU capability V7, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_sync:
	call	assembler_error
	db	'Requires CPU capability SYNC, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_div:
	call	assembler_error
	db	'Requires CPU capability DIV, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_t2ee:
	call	assembler_error
	db	'Requires CPU capability T2EE, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_mp:
	call	assembler_error
	db	'Requires CPU capability MP, use directive "processor" to select',0
ERROR_requires_cpu_capability_arm_ve:
	call	assembler_error
	db	'Requires CPU capability VE, use directive "processor" to select',0
ERROR_requires_copro_capability_fpa_v1:
	call	assembler_error
	db	'Requires coprocessor capability FPA V1, use directive "coprocessor" to select',0
ERROR_requires_copro_capability_fpa_v2:
	call	assembler_error
	db	'Requires coprocessor capability FPA V2, use directive "coprocessor" to select',0
ERROR_requires_copro_capability_vfp_v1xd:
	call	assembler_error
	db	'Requires coprocessor capability VFP V1xD, use directive "coprocessor" to select',0
ERROR_requires_copro_capability_vfp_v1:
	call	assembler_error
	db	'Requires coprocessor capability VFP V1, use directive "coprocessor" to select',0
ERROR_requires_copro_capability_vfp_v2:
	call	assembler_error
	db	'Requires coprocessor capability VFP V2, use directive "coprocessor" to select',0
ERROR_requires_copro_capability_vfp_v3:
	call	assembler_error
	db	'Requires coprocessor capability VFP V3, use directive "coprocessor" to select',0
ERROR_requires_copro_capability_vfp_v4:
	call	assembler_error
	db	'Requires coprocessor capability VFP V4, use directive "coprocessor" to select',0
ERROR_requires_copro_capability_vfp_d32:
	call	assembler_error
	db	'Requires coprocessor capability VFP D32, use directive "coprocessor" to select',0
ERROR_requires_copro_capability_vfp_hp:
	call	assembler_error
	db	'Requires coprocessor capability VFP HP, use directive "coprocessor" to select',0
ERROR_requires_copro_capability_xscale:
	call	assembler_error
	db	'Requires coprocessor capability XSCALE, use directive "coprocessor" to select',0
ERROR_requires_copro_capability_iwmmxt_v1:
	call	assembler_error
	db	'Requires coprocessor capability IWMMXT V1, use directive "coprocessor" to select',0
ERROR_requires_copro_capability_iwmmxt_v2:
	call	assembler_error
	db	'Requires coprocessor capability IWMMXT V2, use directive "coprocessor" to select',0
ERROR_requires_copro_capability_maverick:
	call	assembler_error
	db	'Requires coprocessor capability MAVERICK, use directive "coprocessor" to select',0
ERROR_requires_copro_capability_simd_int:
	call	assembler_error
	db	'Requires coprocessor capability SIMD INT, use directive "coprocessor" to select',0
ERROR_requires_copro_capability_simd_float:
	call	assembler_error
	db	'Requires coprocessor capability SIMD FLOAT, use directive "coprocessor" to select',0
ERROR_requires_copro_capability_simd_hp:
	call	assembler_error
	db	'Requires coprocessor capability SIMD HP, use directive "coprocessor" to select',0
ERROR_requires_copro_capability_simd_v2:
	call	assembler_error
	db	'Requires coprocessor capability SIMD V2, use directive "coprocessor" to select',0
ERROR_narrow_instructions_are_not_encodable_in_arm_mode:
	call	assembler_error
	db	'Narrow instructions are not encodable in ARM mode',0
ERROR_unexpected_end_of_line:
	call	assembler_error
	db	'Unexpected end of line',0
ERROR_parameter_n_not_valid:
    .all:
	call	assembler_error
	db	'Parameter is invalid',0
    .first:
	call	assembler_error
	db	'First parameter is invalid',0
    .second:
	call	assembler_error
	db	'Second parameter is invalid',0
    .third:
	call	assembler_error
	db	'Third parameter is invalid',0
    .fourth:
	call	assembler_error
	db	'Fourth parameter is invalid',0
    .fifth:
	call	assembler_error
	db	'Fifth parameter is invalid',0
    .sixth:
	call	assembler_error
	db	'Sixth parameter is invalid',0
ERROR_high_base_registers_not_allowed_in_list:
	call	assembler_error
	db	'High registers (R8-R15) not allowed in list',0
    .not_pc:
	call	assembler_error
	db	'High registers (R8-R14) not allowed in list',0
    .not_lr:
	call	assembler_error
	db	'High registers (R8-R13,R15) not allowed in list',0
ERROR_instruction_not_conditional:
	call	assembler_error
	db	'Instruction cannot be conditional',0
ERROR_shift_must_be_LSL:
	call	assembler_error
	db	'Shift type must be LSL',0
ERROR_shift_must_be_ASR:
	call	assembler_error
	db	'Shift type must be ASR',0
ERROR_shift_must_be_ROR:
	call	assembler_error
	db	'Shift type must be ROR',0
ERROR_non_contiguous_set:
	call	assembler_error
	db	'Register set must be contiguous',0
ERROR_large_set:
	call	assembler_error
	db	'Register set cannot contain more than 16 registers',0
ERROR_two_registers_required:
	call	assembler_error
	db	'Register list must have two consecutive registers',0
ERROR_must_be_r15:
    .first:
	call	assembler_error
	db	'First parameter must be PC {R15}',0
ERROR_control_register_only_valid_with_word:
	call	assembler_error
	db	'Control register only valid with word size operations',0
ERROR_condition_does_not_match_IT_specifier:
	call	assembler_error
	db	'Condition does not match IT specifier',0
ERROR_instruction_must_be_last_in_IT_block:
	call	assembler_error
	db	'Instruction must be last in IT block',0
ERROR_al_has_no_inverse:
	call	assembler_error
	db	'No "else" condition available with AL',0
ERROR_label_inside_IT_block:
	call	assembler_error
	db	'Cannot access labels inside of an IT block',0
ERROR_mode_change_inside_IT_block:
	call	assembler_error
	db	'Cannot change CPU mode inside of an IT block',0
ERROR_register_list_invalid:
	call	assembler_error
	db	'The register list cannot be encoded',0
ERROR_alignment_invalid:
	call	assembler_error
	db	'The alignment cannot be encoded',0
ERROR_use_fldr_for_single_reg:
	call	assembler_error
	db	'Use VLDR or FLDR to load single 64 bit registers',0
ERROR_element_value_out_of_range:
    .0_1:
	call	assembler_error
	db	'Element value out of range (0 to 1)',0
    .0_3:
	call	assembler_error
	db	'Element value out of range (0 to 3)',0
    .0_7:
	call	assembler_error
	db	'Element value out of range (0 to 7)',0
ERROR_scalar_register_out_of_range:
    .0_7:
	call	assembler_error
	db	'Scalar register out of range (d0 to d7)',0
    .0_15:
	call	assembler_error
	db	'Scalar register out of range (d0 to d15)',0
ERROR_reg_size_64_not_encodable:
	call	assembler_error
	db	'Register size of 64 bits is not encodable',0
