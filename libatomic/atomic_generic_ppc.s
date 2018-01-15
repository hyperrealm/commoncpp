/* ---------------------------------------------------------------------------
   commonc++ - A C++ Common Class Library
   Copyright (C) 2005-2011  Mark A Lindner

   This file is part of commonc++.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   ---------------------------------------------------------------------------
 */
#
# Based on the programming examples in The PowerPC Architecture:
# A Specification for A New Family of RISC Processors, 2nd Ed.,
# Book I, Section E.1, "Synchronization," pp. 249-256, May 1994.
#

.text

#
# int32_t atomic_increment(int32_t *ptr);
#
	.align  2
	.globl  _atomic_increment
_atomic_increment:
	lwarx   r4,0,r3
	addi    r0,r4,1
	stwcx.  r0,0,r3
	bne-    _atomic_increment
	mr      r3,r0
	blr

#
# int32_t atomic_decrement(int32_t *ptr);
#
	.align  2
	.globl  _atomic_decrement
_atomic_decrement:
	lwarx   r4,0,r3
	addi    r0,r4,-1
	stwcx.  r0,0,r3
	bne-    _atomic_decrement
	mr      r3,r0
	blr

#
# int32_t atomic_set(int32_t *ptr, int32_t val);
#
	.align  2
.globl  _atomic_set
_atomic_set:
	lwarx   r5,0,r3
	stwcx.  r4,0,r3
	bne-    _atomic_set
	mr      r3,r5
	blr

#
# int32_t atomic_add(int32_t *ptr, int32_t val);
#
	.align  2
	.globl  _atomic_add
_atomic_add:
	lwarx   r5,0,r3
	add     r0,r4,r5
	stwcx.  r0,0,r3
	bne-    _atomic_add
	mr      r3,r0
	blr

#
# int32_t atomic_cas(int32_t *ptr, int32_t val, int32_t cmp);
#

	.align  2
	.globl  _atomic_cas
_atomic_cas:
	lwarx   r0,0,r3
	cmpw    r5,r0
	bne-    exit
	stwcx.  r4,0,r3
	bne-    _atomic_cas
exit:
	mr      r3,r0
	blr

#
# int32_t atomic_swap(int32_t *ptr, int32_t val)
#

	.align	2
	.globl	_atomic_swap
_atomic_swap:
	lwarx	r0,0,r3
	stwcx.	r4,0,r3
	bne-	_atomic_swap
	mr	r3,r0
	blr

# eof
