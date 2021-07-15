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
 * @file var.h  
 * @author Per Löwgren
 * @date Modified: 2016-02-14
 * @date Created: 2016-02-06
 */ 

/* 
 * Q language variable handling; struct and functions
 * 
 * Variables are variants, they can be of many types: void, integer,
 * float, string; and are handled independently.
 * 
 * Operations are performed depending on type. There are many operators
 * in Q. Most arithmetic functions have corresponding operators,
 * and also string functions. Operators operate on one to three
 * variables: v0 = op v0, v0 = op v1, v0 = v2 op v1
 * 
 * Operators are described below. Operations on mixed types are
 * performed in such a way that: if one variable used by the operator
 * is a string, then the operator for strings is used; if one variable
 * used by the operator is a float, then the operator for floats is
 * used; if all variables used by the operator are integer, then
 * the integer operator is used. For example: in the code "ABC+"
 * A=v2, B=v1, C=v0 - so th addition operation translates
 * to C = A + B. No operation is performed on C, so the type of C
 * is ignored, A and B however are tested for type and if A or B
 * is a string, then string concatenation is performed; if A or B
 * otherwise is a float, then C becomes a float containing A+B;
 * and if both A and B are integer, then C becomes an integer
 * containing A+B.
 */
#ifndef _Q_VAR_H_
#define _Q_VAR_H_

#include "str.h"

enum {
	VOID,
	INT,
	FLOAT,
	STR
};

typedef struct var var;

struct var {
	char index;   // Variable index
	char type;    // Variable type
	union {
		long i;    // Integer value
		double f;  // Float value
		str *s;    // String value
	};
};

void var_free(var *v);

/** Integer power
 * @param b 
 * @param e 
 * @return b^e (b*b*b...[e]*b)
 */
long var_ipow(long b,long e);

/** Integer square root
 * @param n 
 * @return 
 */
long var_isqrt(long n);

/** Integer reduction (123 = 1+2+3 = 6)
 * @param n Integer to reduce
 * @param m Maximum reduction value, least number is 9
 * @return Reduced value
 */
long var_ired(long n,long m);

/** Integer value of variable, independent of type
 * @param v Variable
 * @return 
 */
long var_int(var *v);

double var_float(var *v);
void var_num(var *v,const utf8_t *p,int *q);
void var_str(var *v,const utf8_t *p,int *q);
void var_set(var *v,var *v1);
void var_set_int(var *v,long i);
void var_set_float(var *v,double f);
void var_set_str(var *v,str *s);

int var_empty(var *v);
int var_cmp(var *v,var *v1);

#endif /* _Q_VAR_H_ */

