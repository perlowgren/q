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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "q.h"
#include "var.h"
#include "str.h"

void var_free(var *v) {
	if(v) {
		if(v->type==STR && v->s) str_free(v->s);
	}
}

long var_ipow(long b,long e) {
	int n = 0;
	if(b && e>=0)
		for(n=1; e; e>>=1,b*=b)
			if(e&1) n *= b;
	return n;
}

long var_isqrt(long n) {
	if(n<2) return n;
	long l2 = 0,u = n,v,u2,v2,uv2,n1;
	while(u>>=1) ++l2;
	l2 >>= 1,u = 1<<l2,u2 = u<<l2;
	while(l2--) {
		v = 1<<l2;
		v2 = v<<l2;
		uv2 = u<<(l2+1);
		n1 = u2+uv2+v2;
		if(n1<=n) u += v,u2 = n1;
	}
	return u;
}

long var_ired(long n,long m) {
	long i = 0,r = 0;
	if(n!=0) {
		if(m<9) m = 9;
		for(i=n; i>m; i=r,r=0) {
//if(debug) q_outd(0,"var_ired(1 n: %ld, r: %ld)" STR_NL,n,r);
			while(i!=0) {
				r += (i%10),i /= 10;
//if(debug) q_outd(0,"var_ired(2 n: %ld, r: %ld)" STR_NL,n,r);
			}
		}
	}
if(verbose) q_outv(0,"%s: %d [0..%d] = %d" STR_NL,_("Reduce value"),n,m,i);
	return i;
}

long var_int(var *v) {
	switch(v->type) {
		case VOID:return 0;
		case INT:return v->i;
		case FLOAT:return (long)v->f;
		case STR:return 0;
		default:return 0;
	}
}

double var_float(var *v) {
	switch(v->type) {
		case VOID:return 0.0;
		case INT:return (double)v->i;
		case FLOAT:return v->f;
		case STR:return 0.0;
		default:return 0.0;
	}
}

void var_num(var *v,const utf8_t *p,int *q) {
	int c;
	char *n = (char *)&p[1];
	var_free(v);
	v->type = INT,v->i = 0;
	for(; (c=*n); ++n) {
		if(c>='0' && c<='9') continue;
		if(c=='.' || c=='e') v->type = FLOAT;
		break;
	}
	if(v->type==INT) {
		v->i = strtol((char *)p,&n,0);
	} else if(v->type==FLOAT) {
		v->f = strtod((char *)p,&n);
	}
	if(q) *q += (int)(n-(char *)p)-1;
}

void var_str(var *v,const utf8_t *p,int *q) {
	var_free(v);
	v->type = STR,v->s = NULL;
	if(*p=='~') {
		int l = str_len(++p);
		v->s = str_new_dup(p,l);
		if(q) *q += l+1;
	}
}

void var_set(var *v,var *v1) {
	if(v==v1 || (v->type==STR && v1->type==STR && v->s==v1->s)) return;
	var_free(v);
	v->type = v1->type;
	if(v1->type==VOID) v->i = 0;
	else if(v1->type==INT) v->i = v1->i;
	else if(v1->type==FLOAT) v->f = v1->f;
	else if(v1->type==STR) v->s = str_dup(v1->s);
}

void var_set_int(var *v,long i) {
	var_free(v);
	v->type = INT;
	v->i = i;
}

void var_set_float(var *v,double f) {
	var_free(v);
	v->type = FLOAT;
	v->f = f;
}

void var_set_str(var *v,str *s) {
	if(v->type==STR && v->s==s) return;
	var_free(v);
	v->type = STR;
	v->s = str_dup(s);
}

int var_empty(var *v) {
	if(!v || v->type==VOID) return 1;
	else if(v->type==INT) return v->i==0;
	else if(v->type==FLOAT) return v->f==0.0;
	else if(v->type==STR) {
		if(v->s==NULL || v->s->len==0 || str_data(v->s)==NULL) return 1;
		else {
			char *p = (char *)str_data(v->s);
			return *p=='0' && p[1]=='\0';
		}
	}
	return 1;
}

int var_cmp(var *v,var *v1) {
	if(v==v1) return 0;
	else if((v->type==VOID || v->type==INT) && (v1->type==VOID || v1->type==INT)) return v->i - v1->i;
	else if((v->type==VOID || v->type==INT) && v1->type==FLOAT) return (int)ceil((double)v->i - v1->f);
	else if(v->type==FLOAT && (v1->type==VOID || v1->type==INT)) return (int)ceil(v->f - (double)v1->i);
	else if(v->type==FLOAT && v1->type==FLOAT) return (int)ceil(v->f - v1->f);
	else if(v->type==STR && v1->type==STR) return strcmp((char *)str_data(v->s),(char *)str_data(v1->s));
	else if(v->type==STR) {
		if(v1->type==VOID || v1->type==INT) {
			if(str_is_int(v->s)) {
				char *n;
				long i = strtol((char *)str_data(v->s),&n,0);
				return i - v1->i;
			} else return 0 - v1->i;
		} else if(v1->type==FLOAT) {
			if(str_is_float(v->s)) {
				char *n;
				long i = strtod((char *)str_data(v->s),&n);
				return i - v1->i;
			} else return (int)ceil(0.0 - v1->f);
		}
	} else if(v1->type==STR) {
		if(v->type==VOID || v->type==INT) {
			if(str_is_int(v1->s)) {
				char *n;
				long i = strtol((char *)str_data(v1->s),&n,0);
				return v->i -i;
			} else return v->i - 0;
		} else if(v->type==FLOAT) {
			if(str_is_float(v1->s)) {
				char *n;
				long i = strtod((char *)str_data(v1->s),&n);
				return v->i - i;
			} else return (int)ceil(v->f - 0.0);
		}
	}
	return v->type - v1->type;
}
