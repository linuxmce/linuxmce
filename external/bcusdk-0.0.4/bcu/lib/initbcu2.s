#    BCU SDK bcu development enviroment
#    Copyright (C) 2005-2008 Martin Koegler <mkoegler@auto.tuwien.ac.at>
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    In addition to the permissions in the GNU General Public License, 
#    you may link the compiled version of this file into combinations
#    with other programs, and distribute those combinations without any 
#    restriction coming from the use of this file. (The General Public 
#    License restrictions do apply in other respects; for example, they 
#    cover modification of the file, and distribution when not linked into 
#    a combine executable.)
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	.section .register
	.global SP
SP:
	.byte 0
	

	.section .init
	.global _initstack
	.global _initmem
_initmem:
	clra
	ldx $_bss_size
lp1:	
	beq wt1
	sta %X,_bss_start-1
	decx
	bra lp1
wt1:
	ldx $_bss_hi_size
lp1a:	
	beq wt1a
	sta %X,_bss_hi_start-1
	decx
	bra lp1a
wt1a:
	ldx $_data_size
lp2:	
	beq wt2
	lda %X,_text_end-1
	sta %X,_data_start-1
	decx
	bra lp2
wt2:		
	ldx $_data_hi_size
lp2a:	
	beq wt2a
	lda %X,_data_hi_img-1
	sta %X,_data_hi_start-1
	decx
	bra lp2a
wt2a:		
	jmp _initstack
_initstack:
	lda $SPINIT
	sta SP
	rts
