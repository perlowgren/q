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
#include "q.h"
#include "str.h"

int utf8_decode(const utf8_t *s,int *i) {
	int u = *s,l = 1;
	if(isunicode(u)) {
		int a = utf8_len(u);
		if(a>0) {
			int b;
			u = ((u<<(a+1))&0xff)>>(a+1);
			for(b=1; b<a; ++b)
				u = (u<<6)|(s[l++]&0x3f);
		}
	}
	if(i) *i += l;
	return u;
}

// Unicode Hebrew to Latin:
int uh2l[] = {'A','B','G','D','H','V',
              'Z','X','J','I','K','K','L',
              'M','M','N','N','S','E','P','P','C','C',
              'Q','R','F','T' };

// Hebrew to Latin:
int h2l[]  = {'A','B','G','D','H','W',
              'Z','X','J','I','K','L',
              'M','N','S','E','P','C',
              'Q','R','F','T' };

// Latin to Hebrew:
int l2h[]  = {0,1,17,3,15,20,2,4,9,8,
              10,11,12,13,5,16,18,19,
              14,21,5,5,5,7,9,6};

/* Hebrew values for latin letters A-Z */
int hval[]  = {  1,  2, 90,  4, 70,300,  3,  5, 10, /* A-I */
                 9, 20, 30, 40, 50,  6, 80,100,200, /* J-R */
                60,400,  6,  6,  6,  8, 10,  7 };   /* S-Z */

/* Hebrew final values for latin letters A-Z */
int hvalf[] = {  1,  2,900,  4, 70,300,  3,  5, 10, /* A-I */
                 9,500, 30,600,700,  6,800,100,200, /* J-R */
                60,400,  6,  6,  6,  8, 10,  7 };   /* S-Z */

/* Latin values for letters A-Z */
int lval[]  = {  1,  2, 90,  4, 70,300,  3,  5, 10, /* A-I */
                 9, 20, 30, 40, 50,  6, 80,100,200, /* J-R */
                60,400,  6,  6,  6,  8, 10,  7 };   /* S-Z */


str *str_new(utf8_t *s,int l) {
	str *r = malloc(sizeof(str));
	r->ref = 1,r->data = NULL,r->len = 0;
	if(s) {
		r->len = l>0? l : strlen((char *)s);
		r->data = s;
		r->data[r->len] = '\0';
	}
//if(debug) q_outd(0,"str_new(%s)" STR_NL,r->data);
	return r;
}

str *str_new_dup(const utf8_t *s,int l) {
	str *r = malloc(sizeof(str));
	r->ref = 1,r->data = NULL,r->len = 0;
	if(s) {
		r->len = l>0? l : strlen((char *)s);
		r->data = (utf8_t *)malloc(r->len+1);
		memcpy(r->data,s,r->len);
		r->data[r->len] = '\0';
	}
//if(debug) q_outd(0,"str_new_dup(%s)" STR_NL,r->data);
	return r;
}

void str_free(str *s) {
	if(s && !--s->ref) {
//if(debug) q_outd(0,"str_free(%s)" STR_NL,s->data);
		if(s->data) free(s->data);
		free(s);
	}
}

str *str_dup(str *s) {
	if(s) ++s->ref;
	return s;
}

int str_len(const utf8_t *p) {
	int i,c;
	for(i=0; (c=p[i]) && c!='~'; ++i)
		if(c=='&') ++i;
//if(debug) q_outd(0,"str_len(%d)" STR_NL,i);
	return i;
}

str *str_join(str *s1,str*s2) {
	return str_dup(s1);
}

int str_apply(utf8_t *p,str *s) {
	strncpy((char *)p,(char *)s->data,s->len);
	return s->len;
}

int str_val_sum(str *s,int l) {
	int n = 0;
	if(s && s->data && s->len) {
		int i,j,k = 0,c0 = s->data[0],c1 = s->data[1];
		int *v = hval;
		int *vf = hvalf;
if(verbose) q_outv(0,"%s: \"%s\"" STR_NL,_("Calculate value sum of string"),s->data);
		for(i=0; i<s->len && (!l || i<l) && c0; ++i,c0=c1,c1=s->data[i+1]) {
			if(isunicode(c0)) {
				c0 = utf8_decode(&s->data[i],&i);
				if(c0>=0x5d0 && c0<=0x5ea) c0 = uh2l[c0-0x5d0]; // Hebrew unicode to latin
				j = i,--i;
				c1 = s->data[j];
				if(isunicode(c1)) {
					c1 = utf8_decode(&s->data[j],&j);
					if(c1>=0x5d0 && c1<=0x5ea) c1 = uh2l[c1-0x5d0]; // Hebrew unicode to latin
				}
			}
			if(c0>='A' && c0<='Z') c0 -= 'A';
			else if(c0>='a' && c0<='z') c0 -= 'a';
			else {
				if(c0>='0' && c0<='9') {
if(verbose) q_outv(0,"%c = %d" STR_NL,c0,c0);
					n += c0-'0',++k;
				}
				continue;
			}
			if(!vf || (c1>='A' && c1<='Z') || (c1>='a' && c1<='z')) j = v[c0];
			else j = vf[c0];
			if(j>0) {
if(verbose) q_outv(0,"%c = %d" STR_NL,c0+'A',j);
				n += j,++k;
			}
		}
if(verbose) q_outv(0,"%s: %d" STR_NL,_("Result"),n);
	}
	return n;
}

int str_is_int(str *s) {
	const utf8_t *p = s->data;
	int c = *p;
	if(c=='-') c = *++p;
	do {
		if(c<'0' || c>'9') return 0;
	} while((c=*p++));
	return 1;
}

int str_is_float(str *s) {
	const utf8_t *p = s->data;
	int c = *p,n = 0;
	if(c=='-') c = *++p;
	do { 
		if(n==1 && (c=='e' || c=='E')) {
			if(*p=='-') ++p;
			n = 2;
		} else if((c>='0' && c<='9') || (c=='.' && n==1)) {
			if(n==0) n = 1;
		} else return 0;
	} while((c=*p++));
	return 1;
}

