# -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
#
# The contents of this file are subject to the Mozilla Public
# License Version 1.1 (the "License"); you may not use this file
# except in compliance with the License. You may obtain a copy of
# the License at http://www.mozilla.org/MPL/
# 
# Software distributed under the License is distributed on an "AS
# IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
# implied. See the License for the specific language governing
# rights and limitations under the License.
#
# The Original Code is the Netscape Portable Runtime (NSPR).
# 
# The Initial Developer of the Original Code is Netscape
# Communications Corporation.  Portions created by Netscape are
# Copyright (C) 2003 Netscape Communications Corporation.  All
# Rights Reserved.
#
# Contributor(s):
#   Josh Aas <josh@mozilla.com>
#
# Alternatively, the contents of this file may be used under the
# terms of the GNU General Public License Version 2 or later (the
# "GPL"), in which case the provisions of the GPL are applicable
# instead of those above.  If you wish to allow use of your
# version of this file only under the terms of the GPL and not to
# allow others to use your version of this file under the MPL,
# indicate your decision by deleting the provisions above and
# replace them with the notice and other provisions required by
# the GPL.  If you do not delete the provisions above, a recipient
# may use your version of this file under either the MPL or the
# GPL.
#

#
# Based on atomic_linux_x86.s
#

#
# int32_t atomic_increment(int32_t *ptr);
#
# Atomically increment the integer pointed to by 'ptr' and return
# the result of the increment.
#
    .text
    .globl atomic_increment
    .align 4
atomic_increment:
    movl 4(%esp), %ecx
    movl $1, %eax
    lock
    xaddl %eax, (%ecx)
    incl %eax
    ret

#
# int32_t atomic_decrement(int32_t *ptr);
#
# Atomically decrement the integer pointed to by 'ptr' and return
# the result of the decrement.
#
    .text
    .globl atomic_decrement
    .align 4
atomic_decrement:
    movl 4(%esp), %ecx
    movl $-1, %eax
    lock
    xaddl %eax, (%ecx)
    decl %eax
    ret

#
# int32_t atomic_set(int32_t *ptr, int32_t val);
#
# Atomically set the integer pointed to by 'ptr' to the new
# value 'val' and return the old value.
#
    .text
    .globl atomic_set
    .align 4
atomic_set:
    movl 4(%esp), %ecx
    movl 8(%esp), %eax
    xchgl %eax, (%ecx)
    ret

#
# int32_t atomic_add(int32_t *ptr, int32_t val);
#
# Atomically add 'val' to the integer pointed to by 'ptr'
# and return the result of the addition.
#
    .text
    .globl atomic_add
    .align 4
atomic_add:
    movl 4(%esp), %ecx
    movl 8(%esp), %eax
    movl %eax, %edx
    lock
    xaddl %eax, (%ecx)
    addl %edx, %eax
    ret
