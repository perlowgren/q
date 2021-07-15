/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

/**
 * @file q.h  
 * @author Per Löwgren
 * @date Modified: 2016-02-06
 * @date Created: 2016-02-06
 */ 
#ifndef _Q_Q_H_
#define _Q_Q_H_

#include <stdio.h>
#include "config.h"
#include "var.h"
#include "str.h"

#define _(s)                s

#define ANSI_COLOR_RED      "\x1b[31m"
#define ANSI_COLOR_GREEN    "\x1b[32m"
#define ANSI_COLOR_YELLOW   "\x1b[33m"
#define ANSI_COLOR_BLUE     "\x1b[34m"
#define ANSI_COLOR_MAGENTA  "\x1b[35m"
#define ANSI_COLOR_CYAN     "\x1b[36m"
#define ANSI_COLOR_RESET    "\x1b[0m"

#define ANSI_COLOR_VERBOSE  ANSI_COLOR_MAGENTA
#define ANSI_COLOR_DEBUG    ANSI_COLOR_CYAN
#define ANSI_COLOR_ERROR    ANSI_COLOR_RED

#define VARS                22    // Number of letters in hebrew aplhabet
#define STACK               55    // 1+2+3+4+5+6+7+8+9+10 - Sum of all sephirot

enum {
	EXPR_AND,
	EXPR_OR
};

extern int debug;
extern int verbose;
extern int newline;

extern int op[];
extern int arop[];


typedef struct q_block q_block;

struct q_block {
	int index;
	int pos;
	int end;
	int end_block;
	int ret;
	int ret_block;
	int expr;
	int expr_state;
};

typedef struct q_env q_env;

struct q_env {
	q_env *parent;
	utf8_t *src;
	int len;
	int pos;
	q_block *stack;
	int stack_index;
	q_block *b0;
	var *va;
	int va_len;
	var *v0;
	var *v1;
	var *v2;
	var vt;
	FILE *in;
	FILE *out;
};

enum {
/* 0x100-0x1FF  Block operators
 * 0x200-0x2FF  Constants and variables
 * 0x1000+      Combined operators, or single char operators that can be combined: + - * / % # & : ? = ! < > @ ^|
 * Flags:
 * 0x1000       Do not look for constant after operator
 * 0x2000       Only operating on V0; look for constant after operator, and if found use as V0
 * 0x4000       Operating on V0 and V1; look for constant after operator, and if found use as V1
 * 0x8000       Operating on V0, V1 and V2; look for constant after operator, and if found use as V1, pushing V1 to V2
 */
	OP_ADD     =  0xC001,  // +    V0 = V2 + V1
	OP_SUB     =  0xC002,  // -    V0 = V2 - V1
	OP_MUL     =  0xC003,  // *    V0 = V2 * V1
	OP_DIV     =  0xC004,  // /    V0 = V2 / V1
	OP_MOD     =  0xC005,  // %    V0 = V2 % V1

	OP_INDEX   =  0x4006,  // #                
	OP_OUTPUT  =  0x1007,  // &    echo(V0)
	OP_SET     =  0x1008,  // :    V0 = V1
	OP_IF      =  0x1009,  // ?    if(x)
	OP_EQ      =  0x400A,  // =    x = (V0 == V1)
	OP_NIS     =  0x100B,  // !    x = !V0
	OP_LT      =  0x400C,  // <    x = (V0 < V1)
	OP_GT      =  0x400D,  // >    x = (V0 > V1)
	OP_GOTO    =  0x200E,  // @    goto V0
	OP_CARET   =  0x100F,  // ^    V0[V1]
	OP_ELSE    =  0x1010,  // |    if... else...
	OP_NOT     =  0x4011,  // ~    V0 = ~V1

	OP_LEXPR   =   0x101,  // (
	OP_REXPR   =   0x102,  // )
	OP_LBLOCK  =   0x103,  // [    { ...
	OP_RBLOCK  =   0x104,  // ]    ... }

	OP_NUM     =   0x201,  // 0-9  V0 = 123
	OP_STR     =   0x202,  // '    V0 = "Abc"
	OP_VAR     =   0x203,  // A-Z  V0 = &A

	OP_INT     =  0x1101,  // ##   V0 = (long)V1
	OP_FLOAT   =  0x1102,  // %%   V0 = (double)V1

	OP_ADD2    =  0x4001,  // +:   V0 += V1
	OP_SUB2    =  0x4002,  // -:   V0 -= V1
	OP_MUL2    =  0x4003,  // *:   V0 *= V1
	OP_DIV2    =  0x4004,  // /:   V0 /= V1
	OP_MOD2    =  0x4005,  // %:   V0 %= V1

	OP_INC     =  0x1201,  // ++   V0++
	OP_DEC     =  0x1202,  // --   V0--
	OP_POW     =  0xC208,  // **   V0 = pow(V2, V1)
	OP_SQRT    =  0x4209,  // //   V0 = sqrt(V1)

	OP_LSHIFT  =  0xC301,  // <<   V0 = V2 << V1
	OP_RSHIFT  =  0xC302,  // >>   V0 = V2 >> V1
	OP_AND     =  0xC303,  // &&   V0 = V2 & V1
	OP_OR      =  0xC304,  // ||   V0 = V2 | V1
	OP_XOR     =  0xC305,  // ^^   V0 = V2 ^ V1

	OP_LSHIFT2 =  0x4301,  // <:   V0 <<= V1
	OP_RSHIFT2 =  0x4302,  // >:   V0 >>= V1
	OP_AND2    =  0x4303,  // &:   V0 &= V1
	OP_OR2     =  0x4304,  // |:   V0 |= V1
	OP_XOR2    =  0x4305,  // ^:   V0 ^= V1
	OP_NOT2    =  0x1306,  // ~:   V0 = ~V0

	OP_ABS     =  0x4501,  // -+   V0 = abs(V1)
	OP_NEG     =  0x4502,  // +-   V0 = -abs(V1)

	OP_FLOOR   =  0x4601,  // %-   V0 = floor(V1)
	OP_CEIL    =  0x4603,  // %+   V0 = ceil(V1)

	OP_INT2    =  0x1701,  // #:   V0 = (long)V0
	OP_RED     =  0xC702,  // #%   V0 = red(V2,V1)

	OP_NIF     =  0x1781,  // !?   if(!x) ...

	OP_IS      =  0x1791,  // !!   x = !!V0
	OP_NEQ     =  0x4792,  // !=   x = (V0 != V1)
	OP_LTEQ    =  0x4793,  // <=   x = (V0 <= V1)
	OP_GTEQ    =  0x4794,  // >=   x = (V0 >= V1)

	OP_ELVIS   = 0x1C7A1,  // ??   V0 = V1 ?: V2
	OP_ELVIS2  =  0x47A1,  // ?:   V0 = V0 ?: V1

	OP_INPUT   =  0x1901,  // &<   V0 = input()
	OP_DOUT    =  0x1902,  // ?>   direct output (PHP: ?> ... <?php)
	OP_DOUTE   =  0x1903,  // <?   direct output end
	OP_DSTR    =  0x1904,  // &>   unformatted string (PHP: $a = <<<'END' ... END;)
	OP_DSTRE   =  0x1905,  // <&   unformatted string end

	OP_POS     =  0x1A01,  // @:   V0 = position
	OP_LOOP    =  0x1A02,  // @<   continue
	OP_RETURN  =  0x1A03,  // @^   return

	OP_INCLUDE =  0x2A11,  // @#   include(V0)
	OP_EXEC    =  0x2A12,  // @&   exec(V0)

	OP_COPEN   =  0x1FFE,  // /*   /* ...
	OP_CCLOSE  =  0x1FFF,  // */   ... */
};

void q_outv(int nl,const char *f, ...);
void q_outd(int nl,const char *f, ...);
void q_oute(int nl,const char *f, ...);

void q_outc(int c,FILE *out);
void q_out_utf8(utf8_t **p,FILE *out);

int q_str_len(q_env *e,const utf8_t *p);
void q_var_str(q_env *e,var *v,const utf8_t *p,int *q);

void q_input(q_env *e,var *v,int l);
void q_output(q_env *e,var *v);

void q_block_end(q_env *e,int f,int r);

void q_exec(q_env *e);

q_env *q_open(char *src,int pos,int len,FILE *in,FILE *out,q_env *pe);
q_env *q_close(q_env *e);

#endif /* _Q_Q_H_ */

