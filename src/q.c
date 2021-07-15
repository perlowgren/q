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

/* 
 * Qabalah Language interpreter - Per Löwgren 2016
 * Compile: gcc -o q q.c -lm
 * (to compile with CLI, add flag -DCLI)
 * (to compile with readline, add flags -DREADLINE -lreadline)
 */
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "q.h"
#include "var.h"
#include "str.h"

#define USAGE_HEADER "Usage: q [OPTIONS] [FILENAME]" STR_NL "\
FILENAME is the name of a Q-script. If omitted," STR_NL "\
interactive mode is enabled." STR_NL "\
OPTIONS include:" STR_NL

#define USAGE_FOOTER "" STR_NL "\
Report bugs to: %s" STR_NL "\
%s home page: <%s>" STR_NL

#define USAGE_VERSION "Qabalah Language Interpreter %s" STR_NL "\
Copyright (C) %s %s" STR_NL "\
License GPLv2+: GNU GPL version 2 or later <http://gnu.org/licenses/gpl.html>" STR_NL "\
This is free software: you are free to change and redistribute it." STR_NL "\
There is NO WARRANTY, to the extent permitted by law." STR_NL

#define INT_MOD_HEADER "Qabalah Language Interpreter ver. %s" STR_NL "\
Interactive mode enabled" STR_NL "\
Enter Q-code, type 'help' for instructions." STR_NL

#define INT_MOD_HELP "Commands:" STR_NL "\
  eof     End input and run script" STR_NL "\
  exit    Exit this program" STR_NL "\
  help    Print this list of cammands" STR_NL "\
  quit    Exit this program" STR_NL "\
  run     Run script, same as 'eof'" STR_NL "\
  show    Show entered data" STR_NL

#define ERR_FILE_IN "Could not open input file"

#define op_combine(a,b) arop[((a)&0xff)*17+((b)&0xff)-18]

int op[] = {
/* .0123456789                 101-111   Number
 * ABCDEFGHIJKLMNOPQRSTUVWXYZ  201-226   Variables
 * abcdefghijklmnopqrstuvwxyz  301-326   Variables
 * !#%&'()*+-/:<=>?@[]^|~      1000-     Operators */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                 // 0x00 - 0x1F
	0,        OP_NIS,   0,      OP_INDEX,  0,      OP_MOD,    OP_OUTPUT, OP_STR,                     //  !"#$%&'
	OP_LEXPR, OP_REXPR, OP_MUL, OP_ADD,    0,      OP_SUB,    OP_NUM,    OP_DIV,                     // ()*+,-./
	OP_NUM,   OP_NUM,   OP_NUM, OP_NUM,    OP_NUM, OP_NUM,    OP_NUM,    OP_NUM,                     // 01234567
	OP_NUM,   OP_NUM,   OP_SET, 0,         OP_LT,  OP_EQ,     OP_GT,     OP_IF,                      // 89:;<=>?
	OP_GOTO,  OP_VAR,   OP_VAR, OP_VAR,    OP_VAR, OP_VAR,    OP_VAR,    OP_VAR,                     // @ABCDEFG
	OP_VAR,   OP_VAR,   OP_VAR, OP_VAR,    OP_VAR, OP_VAR,    OP_VAR,    OP_VAR,                     // HIJKLMNO
	OP_VAR,   OP_VAR,   OP_VAR, OP_VAR,    OP_VAR, OP_VAR,    OP_VAR,    OP_VAR,                     // PQRSTUVW
	OP_VAR,   OP_VAR,   OP_VAR, OP_LBLOCK, 0,      OP_RBLOCK, OP_CARET,  0,                          // XYZ[\]^_
	0,        OP_VAR,   OP_VAR, OP_VAR,    OP_VAR, OP_VAR,    OP_VAR,    OP_VAR,                     // `abcdefg
	OP_VAR,   OP_VAR,   OP_VAR, OP_VAR,    OP_VAR, OP_VAR,    OP_VAR,    OP_VAR,                     // hijklmno
	OP_VAR,   OP_VAR,   OP_VAR, OP_VAR,    OP_VAR, OP_VAR,    OP_VAR,    OP_VAR,                     // pqrstuvw
	OP_VAR,   OP_VAR,   OP_VAR, 0,         OP_ELSE,0,         OP_NOT,    0,                          // xyz{|}~ 0x7F
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                 // 0x80 - 0x9F
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                 // 0xA0 - 0xBF
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, // 0xC0 - 0xDF Unicode
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, // 0xE0 - 0xFF
};

int arop[] = {
// ++         +-         +*         +/         +%         +#         +&         +:         +?         +=         +!         +<         +>         +@         +^         +|         +~
   OP_INC,    OP_NEG,    0,         0,         0,         0,         0,         OP_ADD2,   0,         0,         0,         0,         0,         0,         0,         0,         0,
// -+         --         -*         -/         -%         -#         -&         -:         -?         -=         -!         -<         ->         -@         -^         -|         -~
   OP_ABS,    OP_DEC,    0,         0,         0,         0,         0,         OP_SUB2,   0,         0,         0,         0,         0,         0,         0,         0,         0,
// *+         *-         **         */         *%         *#         *&         *:         *?         *=         *!         *<         *>         *@         *^         *|         *~
   0,         0,         OP_POW,    OP_CCLOSE, 0,         0,         0,         OP_MUL2,   0,         0,         0,         0,         0,         0,         0,         0,         0,
// /+         /-         /*         //         /%         /#         /&         /:         /?         /=         /!         /<         />         /@         /^         /|         /~
   0,         0,         OP_COPEN,  OP_SQRT,   0,         0,         0,         OP_DIV2,   0,         0,         0,         0,         0,         0,         0,         0,         0,
// %+         %-         %*         %/         %%         %#         %&         %:         %?         %=         %!         %<         %>         %@         %^         %|         %~
   OP_CEIL,   OP_FLOOR,  0,         0,         OP_FLOAT,  0,         0,         OP_MOD2,   0,         0,         0,         0,         0,         0,         0,         0,         0,
// #+         #-         #*         #/         #%         ##         #&         #:         #?         #=         #!         #<         #>         #@         #^         #|         #~
   0,         0,         0,         0,         OP_RED,    OP_INT,    0,         OP_INT2,   0,         0,         0,         0,         0,         0,         0,         0,         0,
// &+         &-         &*         &/         &%         &#         &&         &:         &?         &=         &!         &<         &>         &@         &^         &|         &~
   0,         0,         0,         0,         0,         0,         OP_AND,    OP_AND2,   0,         0,         0,         OP_INPUT,  OP_DSTR,   0,         0,         0,         0,
// :+         :-         :*         :/         :%         :#         :&         ::         :?         :=         :!         :<         :>         :@         :^         :|         :~
   0,         0,         0,         0,         0,         0,         0,         0,         0,         0,         0,         0,         0,         0,         0,         0,         0,
// ?+         ?-         ?*         ?/         ?%         ?#         ?&         ?:         ??         ?=         ?!         ?<         ?>         ?@         ?^         ?|         ?~
   0,         0,         0,         0,         0,         0,         0,         OP_ELVIS2, OP_ELVIS,  0,         0,         0,         OP_DOUT,   0,         0,         0,         0,
// =+         =-         =*         =/         =%         =#         =&         =:         =?         ==         =!         =<         =>         =@         =^         =|         =~
   0,         0,         0,         0,         0,         0,         0,         0,         0,         0,         0,         0,         0,         0,         0,         0,         0,
// !+         !-         !*         !/         !%         !#         !&         !:         !?         !=         !!         !<         !>         !@         !^         !|         !~
   0,         0,         0,         0,         0,         0,         0,         0,         0,         OP_NEQ,    OP_IS,     0,         0,         0,         0,         0,         0,
// <+         <-         <*         </         <%         <#         <&         <:         <?         <=         <!         <<         <>         <@         <^         <|         <~
   0,         0,         0,         0,         0,         0,         OP_DSTRE,  OP_LSHIFT2,OP_DOUTE,  OP_LTEQ,   0,         OP_LSHIFT, 0,         0,         0,         0,         0,
// >+         >-         >*         >/         >%         >#         >&         >:         >?         >=         >!         ><         >>         >@         >^         >|         >~
   0,         0,         0,         0,         0,         0,         0,         OP_RSHIFT2,0,         OP_GTEQ,   0,         0,         OP_RSHIFT, 0,         0,         0,         0,
// @+         @-         @*         @/         @%         @#         @&         @:         @?         @=         @!         @<         @>         @@         @^         @|         @~
   0,         0,         0,         0,         0,         OP_INCLUDE,OP_EXEC,   OP_POS,    0,         0,         0,         OP_LOOP,   0,         0,         OP_RETURN, 0,         0,
// ^+         ^-         ^*         ^/         ^%         ^#         ^&         ^:         ^?         ^=         ^!         ^<         ^>         ^@         ^^         ^|         ^~
   0,         0,         0,         0,         0,         0,         0,         OP_XOR2,   0,         0,         0,         0,         0,         0,         OP_XOR,    0,         0,
// |+         |-         |*         |/         |%         |#         |&         |:         |?         |=         |!         |<         |>         |@         |^         ||         |~
   0,         0,         0,         0,         0,         0,         0,         OP_OR2,    0,         0,         0,         0,         0,         0,         0,         OP_OR,     0,
// ~+         ~-         ~*         ~/         ~%         ~#         ~&         ~:         ~?         ~=         ~!         ~<         ~>         ~@         ~^         ~|         ~~
   0,         0,         0,         0,         0,         0,         0,         OP_NOT2,   0,         0,         0,         0,         0,         0,         0,         0,         0,
};

int debug = 0;
int verbose = 0;
int newline = 0;

static void run(char *src,int len,FILE *in,FILE *out);
static char *file_read(const char *file,int *len);

void q_outv(int nl,const char *f, ...) {
	va_list list;
	va_start(list,f);
	if(newline) fputs(STR_NL,stdout);
	fputs(ANSI_COLOR_VERBOSE,stdout);
	vfprintf(stdout,f,list);
	fputs(ANSI_COLOR_RESET,stdout);
	va_end(list);
	newline = nl;
}

void q_outd(int nl,const char *f, ...) {
	va_list list;
	va_start(list,f);
	if(newline) fputs(STR_NL,stderr);
	fputs(ANSI_COLOR_DEBUG,stderr);
	vfprintf(stderr,f,list);
	fputs(ANSI_COLOR_RESET,stderr);
	va_end(list);
	newline = nl;
}

void q_oute(int nl,const char *f, ...) {
	va_list list;
	va_start(list,f);
	if(newline) fputs(STR_NL,stderr);
	fputs(ANSI_COLOR_ERROR,stderr);
	vfprintf(stderr,f,list);
	fputs(ANSI_COLOR_RESET,stderr);
	va_end(list);
	newline = nl;
}

void q_outc(int c,FILE *out) {
	if(c=='\t' || (c>=32 && c<=127)) {
		fputc(c,out);
		newline = 1;
	} else if(c=='\n' || c==EOF) {
		fputs(STR_NL,out);
		newline = 0;
	}
}

void q_out_utf8(utf8_t **p,FILE *out) {
	utf8_t *s = *p;
	int i = *s;
//if(debug) q_outd(0,"q_out_utf8(len: %d)" STR_NL,utf8_len(i));
	for(i=utf8_len(i); i--; ++s) /*printf("[0x%02X]",(int)*s);*/fputc(*s,out);
	*p = s;
	newline = 1;
}

int q_str_len(q_env *e,const utf8_t *p) {
	int l,c,a;
	var *v1;
	for(l=0; (c=p[l]) && c!='\''; ++l)
		if(c=='&') {
			if(p[l+1]==':' && (c=p[l+2])) {
				if((c=='*' || 
					(c>='A' && c<='Z' && (a=l2h[c-'A'])>=0) ||
					(c>='a' && c<='z' && (a=l2h[c-'a'])>=0))) {
					if(c=='*') v1 = &e->vt;
					else v1 = &e->va[a];
					if(v1->type==INT) l += 22;
					else if(v1->type==FLOAT) l += 42;
					else if(v1->type==STR) l += v1->s->len;
					else ++l;
				}
			} else ++l;
		}
//if(debug) q_outd(0,"q_str_len(l: %d)" STR_NL,l);
	return l;
}

/*int q_str_match(q_env *e,const utf8_t *p) {
	int l,n,c;
	for(l=0,n=1; (c=p[l]); ++l) {
		if(c=='\'') {
			if(p[l+1]=='>') ++n,++l;
			else if(p[l+1]==':' && p[l+2]=='>') ++b,l += 2;
		} else if(c=='<' && p[l+1]=='\'' && !--n) return l+1;
	}
	return l;
}*/

void q_var_str(q_env *e,var *v,const utf8_t *p,int *len) {
	var_free(v);
	v->type = STR,v->s = NULL;
	if(*p=='\'') {
		int i,l = q_str_len(e,++p),c,a;
		var *v1;
		utf8_t *s = (utf8_t *)malloc(l+1);
		for(i=0,l=0; (c=p[i]) && c!='\''; ++i) {
			if(c=='&') {
				if(p[i+1]==':' && (c=p[i+2])) {
					if((c=='*' || 
						(c>='A' && c<='Z' && (a=l2h[c-'A'])>=0) ||
						(c>='a' && c<='z' && (a=l2h[c-'a'])>=0))) {
						if(c=='*') v1 = &e->vt;
						else v1 = &e->va[a];
						if(v1->type==INT) l += sprintf((char *)&s[l],"%ld",v1->i);
						else if(v1->type==FLOAT) l += sprintf((char *)&s[l],"%g",v1->f);
						else if(v1->type==STR) { strcpy((char *)&s[l],(char *)str_data(v1->s));l += v1->s->len; }
						else s[l++] = '?';
						i += 2;
						continue;
					}
				}
				s[l++] = p[i++];
			}
			s[l++] = p[i];
		}
		s[l] = '\0';
		v->s = str_new(s,l);
		if(len) *len += c=='\0'? i-1 : i+1;
//if(debug) q_outd(0,"q_var_str(l: %d, s: %s)" STR_NL,l,s);
if(verbose) q_outv(0,"%s: " ANSI_COLOR_YELLOW "\"%s\"" STR_NL,_("Created string"),(char *)str_data(v->s));
	}
}

void q_input(q_env *e,var *v,int l) {
	int c,i,n,m = l;
	char ln[81],*b;
	if(!v) return;
	if(m<=0) m = 1024;
	*ln = '\0',c = '\0',b = malloc(m+1);
	for(i=0; c!='\n'; i+=n) {
		if(fgets(ln,81,e->in)==NULL) break;
		n = strlen(ln);
//if(debug) q_outd(0,"ln[n: %d]: %s" STR_NL,n,ln);
		if(!n) break;
		if((c=ln[n-1])=='\n' || c=='\r') { // remove '\n' & '\r'
			if(n>1 && ((c=ln[n-2])=='\n' || c=='\r')) --n;
			ln[--n] = '\0',c = '\n';
		}
//if(debug) q_outd(0,"ln[cap: %d, p: %d, n: %d]: %s" STR_NL,(int)cap,p,n,ln);
		if(i+n>=m) {
			m = m*2<i+n? i+n+1 : m*2;
			if(!(b=realloc(b,m))) return;
		}
		strcpy(&b[i],ln);
	}
	if(v->type==STR && v->s) str_free(v->s);
	v->type = STR;
	v->s = str_new((utf8_t *)b,l<=0? m : l);
	newline = 0;
}

void q_output(q_env *e,var *v) {
	if(v->type==VOID) fputc('?',e->out);
	else if(v->type==INT) fprintf(e->out,"%ld",v->i);
	else if(v->type==FLOAT) fprintf(e->out,"%g",v->f);
	else {
		if(v->type==STR && v->s && str_data(v->s)) {
			utf8_t *p = str_data(v->s);
			int c = *p;
			if(c=='<') {
				q_input(e,v,0);
			} else {
				var *v1;
				int c0,n,a = 0;
//if(debug) q_outd(0,"q_output(\"%s\")" STR_NL,(char *)p);
				while((c=*p++)) {
					if(c=='&') {
						c0 = 0,c = *p,n = 0;
						if(/*c=='>' || */c=='<'/* || c=='|' || c=='%'*/) c0 = c,c = p[++n];
						if(isunicode(c)) {
							a = utf8_decode(&p[n],&n),--n;
							if(a>=0x5d0 && a<=0x5ea) c = uh2l[a-0x5d0];
						}
						if(c=='*' ||
						   (c>='A' && c<='Z' && (a=l2h[c-'A'])>=0) ||
							(c>='a' && c<='z' && (a=l2h[c-'a'])>=0)) {
							if(c=='*') v1 = &e->vt;
							else v1 = &e->va[a];
							if(c0) {
								if(c0=='<') q_input(e,v1,0);
							} else q_output(e,v1);
							p += n+1;
							continue;
						}
						if(c0) q_outc(c0,e->out);
						c = *p++;
					}
					else if(c=='\\') c = '\n';
					else if(c=='^') c = '\t';
					if(!isunicode(c)) q_outc(c,e->out);
					else {
						--p;
						q_out_utf8(&p,e->out);
					}
				}
			}
		}
		return;
	}
	newline = 1;
}

// Get next variable or operator char:
#define next(c,o,s,p) \
while((c=s[++p]) && !(o=op[c]))

void q_block_end(q_env *e,int f,int r) {
	int n = 1,a,b,c,o;
	q_block *b0 = e->b0;
	while(1) {
		next(c,o,e->src,e->pos);
		if(!c) break; // EOF

		if(o>=0x1000 && (a=op[e->src[e->pos+1]])>=0x1000)
			if((b=op_combine(o,a))) ++e->pos,o = b;

		if(o==OP_LBLOCK) ++n;
		else if(o==OP_ELSE && n==1 && f) break;
		else if(o==OP_RBLOCK && !--n) {
			if(r) { // Return
				if(b0->end!=b0->pos) e->pos = b0->end;
				e->b0 = &e->stack[b0->end_block];
			}
			break;
		}
	}
	if(c=='\0') { // End of script, exit
		e->stack_index = 0;
		e->b0 = &e->stack[e->stack_index];
		e->pos = e->b0->end;
	}
}

void q_exec(q_env *e) {
	int a = 0,b,c,d,l,o;
	char *p,*q;
	double f;
	var *v0,*v1,*v2;
	q_block *b1;
	str *s = NULL;
//if(debug) q_outd(0,"exec:" STR_NL "%s" STR_NL,e->src);

exec_start:
	while(e) {
		next(c,o,e->src,e->pos);
		if(!c) goto exec_end; // EOF

//if(debug) q_outd(0,"exec: %c" STR_NL,c);

		if(isunicode(c)) { // UTF-8 unicode
			c = utf8_decode(&e->src[e->pos],&e->pos); // Get unicode
			--e->pos;
			if(c>=0x5d0 && c<=0x5ea) // Hebrew unicode
				c = uh2l[c-0x5d0],o = op[c]; // To latin
		}

		if(o>=0x1000 && (a=op[e->src[e->pos+1]])>=0x1000)
			if((b=op_combine(o,a))) ++e->pos,o = b;

if(debug && o>=0x1000) q_outd(0,"Operator: %c%c [0x%X]" STR_NL,c,a>=0x1000 && b? e->src[e->pos] : ' ',o);

		v0 = e->v0;
		v1 = e->v1;
		v2 = e->v2;

		if(o&0xE000) {
			b = e->pos;
			next(d,a,e->src,b);
			if(a==OP_STR && e->src[b+1]=='>') a = 0; // Ignore direct output strings: ~>...
			if(a==OP_NUM || a==OP_STR) { // Number or String
				e->pos = b;
				if(a==OP_NUM) // Number
					var_num(&e->vt,&e->src[e->pos],&e->pos);
				else // String
					q_var_str(e,&e->vt,&e->src[e->pos],&e->pos);
//if(debug) q_outd(1,"vt [%c, %d]: ",h2l[(int)e->vt.index],e->vt.type);
				if(o&0x2000) v0 = &e->vt;
				else if(o&0x10000) v2 = &e->vt;
				else {
					v1 = &e->vt;
					v2 = e->v1;
				}
			}
		}

		s = v0->type==STR && v0==e->v0? v0->s : NULL; // Store string in V0

//if(debug) q_outd(0,"exec(c: %c%c, o: 0x%X)  V0[%c, %d]  V1[%c, %d]  V2[%c, %d]" STR_NL,c,c0,o,h2l[(int)v0->index],v0->type,h2l[(int)v1->index],v1->type,h2l[(int)v2->index],v2->type);

		switch(o) {
			case OP_ADD2:
				v2 = v0;
			case OP_ADD:
//if(debug) q_outd(0,"OP_ADD [%c, %d] +  [%c, %d]" STR_NL,h2l[(int)e->v2->index],e->v2->type,h2l[(int)e->v1->index],e->v1->type);
					  if(v1->type==STR   || v2->type==STR)   v0->s = str_join(v2->s,v1->s),         v0->type = STR;
				else if(v1->type==FLOAT && v2->type==FLOAT) v0->f = v2->f         + v1->f,         v0->type = FLOAT;
				else if(v1->type==FLOAT && v2->type==INT)   v0->f = (double)v2->i + v1->f,         v0->type = FLOAT;
				else if(v1->type==INT   && v2->type==FLOAT) v0->f = v2->f         + (double)v1->i, v0->type = FLOAT;
				else if(v1->type==INT   && v2->type==INT)   v0->i = v2->i         + v1->i,         v0->type = INT;
				break;

			case OP_SUB2:
				v2 = v0;
			case OP_SUB:
					  if(v1->type==FLOAT && v2->type==FLOAT) v0->f = v2->f         - v1->f,         v0->type = FLOAT;
				else if(v1->type==FLOAT && v2->type==INT)   v0->f = (double)v2->i - v1->f,         v0->type = FLOAT;
				else if(v1->type==INT   && v2->type==FLOAT) v0->f = v2->f         - (double)v1->i, v0->type = FLOAT;
				else if(v1->type==INT   && v2->type==INT)   v0->i = v2->i         - v1->i,         v0->type = INT;
				break;

			case OP_MUL2:
				v2 = v0;
			case OP_MUL:
					  if(v1->type==FLOAT && v2->type==FLOAT) v0->f = v2->f         * v1->f,         v0->type = FLOAT;
				else if(v1->type==FLOAT && v2->type==INT)   v0->f = (double)v2->i * v1->f,         v0->type = FLOAT;
				else if(v1->type==INT   && v2->type==FLOAT) v0->f = v2->i         * (double)v1->i, v0->type = FLOAT;
				else if(v1->type==INT   && v2->type==INT)   v0->i = v2->i         * v1->i,         v0->type = INT;
				break;

			case OP_DIV2:
				v2 = v0;
			case OP_DIV:
					  if(v1->type==FLOAT && v2->type==FLOAT) v0->f = v2->f         / v1->f,         v0->type = FLOAT;
				else if(v1->type==FLOAT && v2->type==INT)   v0->f = (double)v2->i / v1->f,         v0->type = FLOAT;
				else if(v1->type==INT   && v2->type==FLOAT) v0->f = v2->f         / (double)v1->i, v0->type = FLOAT;
				else if(v1->type==INT   && v2->type==INT)   v0->i = v2->i         / v1->i,         v0->type = INT;
				break;

			case OP_MOD2:
				v2 = v0;
			case OP_MOD:
				if(v1->type==FLOAT || v2->type==FLOAT) {
					if(v1->type==FLOAT && v2->type==FLOAT)   v0->f = modf(v2->f/v1->f,&f),          v0->type = FLOAT;
					else if(v1->type==INT)                   v0->f = modf(v2->f/(double)v1->i,&f),  v0->type = FLOAT;
					else if(v2->type==INT)                   v0->f = modf((double)v2->i/v1->f,&f),  v0->type = FLOAT;
				} else if(v1->type==INT && v2->type==INT)   v0->i = v2->i         % v1->i,         v0->type = INT;
				break;

			case OP_OUTPUT:
//if(debug) q_outd(0,"OP_OUTPUT[%c, %d]" STR_NL,h2l[(int)v0->index],v0->type);
				q_output(e,v0);
				break;

			case OP_SET:
				var_set(v0,v1);
				s = NULL;
				break;

			case OP_NUM:
				var_num(&e->vt,&e->src[e->pos],&e->pos);
				var_set(v0,&e->vt);
				s = NULL;
//if(debug) {q_outd(1,"OP_NUM[%c, %d]: ",h2l[(int)v0->index],v0->type);
				break;

			case OP_STR:
				q_var_str(e,&e->vt,&e->src[e->pos],&e->pos);
				var_set(v0,&e->vt);
//if(debug) q_outd(0,"OP_STR[%c, %d]: %s" STR_NL,h2l[(int)v0->index],v0->type,(char *)str_data(v0->s));
				s = NULL;
				break;

			case OP_VAR:
				if((c>='A' && c<='Z' && (a=l2h[c-'A'])!=-1) ||
					(c>='a' && c<='z' && (a=l2h[c-'a'])!=-1)) {
					e->v2 = e->v1;
					e->v1 = e->v0;
					e->v0 = &e->va[a];
					s = NULL;
				}
				break;

			case OP_GOTO:
				a = var_int(v0);
				if(a<0) a = -1;
				else if(a>=e->len) a = e->len-1;
				if(e->stack_index+1==STACK) goto exec_err_stack_overflow;
				b1 = &e->stack[++e->stack_index];
				b1->pos         = a;
				b1->end         = e->pos; // Return after end of block
				b1->end_block   = e->b0->index;
				b1->ret         = e->pos; // Return on return operator
				b1->ret_block   = e->b0->index;
				b1->expr        = -1;
				b1->expr_state  = EXPR_AND;
				e->b0 = b1;
				e->pos = a;
if(debug) q_outd(0,"OP_GOTO[%d] pos: %d, end: %d, end_block: %d, ret: %d, ret_block: %d, expr: %d, expr_state: %d" STR_NL,e->b0->index,e->b0->pos,e->b0->end,e->b0->end_block,e->b0->ret,e->b0->ret_block,e->b0->expr,e->b0->expr_state);
				break;

			case OP_LEXPR:
				if(e->stack_index+1==STACK) goto exec_err_stack_overflow;
				b1 = &e->stack[++e->stack_index];
				*b1 = *e->b0;
				b1->expr = -1;
				if(e->b0->expr_state==EXPR_AND)     b1->expr_state = EXPR_OR;
				else if(e->b0->expr_state==EXPR_OR) b1->expr_state = EXPR_AND;
				e->b0 = b1;
if(debug) q_outd(0,"OP_LEXPR[%d] pos: %d, end: %d, end_block: %d, ret: %d, ret_block: %d, expr: %d, expr_state: %d" STR_NL,e->b0->index,e->b0->pos,e->b0->end,e->b0->end_block,e->b0->ret,e->b0->ret_block,e->b0->expr,e->b0->expr_state);
				break;

			case OP_REXPR:
				if(e->stack_index<=0) goto exec_end;
				b1 = &e->stack[--e->stack_index];
if(debug) q_outd(0,"OP_REXPR expr-a: %d, expr_state-a: %d, expr-b: %d, expr_state-b: %d" STR_NL,b1->expr,b1->expr_state,e->b0->expr,e->b0->expr_state);
				if(e->b0->expr!=-1) {
					if(b1->expr==-1) b1->expr = e->b0->expr;
					else if(b1->expr_state==EXPR_AND) b1->expr = (b1->expr && e->b0->expr);
					else if(b1->expr_state==EXPR_OR)  b1->expr = (b1->expr || e->b0->expr);
				}
				e->b0 = b1;
if(debug) q_outd(0,"OP_REXPR[%d] pos: %d, end: %d, end_block: %d, ret: %d, ret_block: %d, expr: %d, expr_state: %d" STR_NL,e->b0->index,e->b0->pos,e->b0->end,e->b0->end_block,e->b0->ret,e->b0->ret_block,e->b0->expr,e->b0->expr_state);
				break;

			case OP_LBLOCK:
				if(e->stack_index+1==STACK) goto exec_err_stack_overflow;
				b1 = &e->stack[++e->stack_index];
				b1->pos         = e->pos;
				b1->end         = b1->pos; // Continue after end of block
				b1->end_block   = e->b0->index;
				b1->ret         = e->b0->ret; // Return to parent block return position, on return operator
				b1->ret_block   = e->b0->ret_block;
				b1->expr        = -1;
				b1->expr_state  = EXPR_AND;
				e->b0 = b1;
if(debug) q_outd(0,"OP_LBLOCK[%d] pos: %d, end: %d, end_block: %d, ret: %d, ret_block: %d, expr: %d, expr_state: %d" STR_NL,e->b0->index,e->b0->pos,e->b0->end,e->b0->end_block,e->b0->ret,e->b0->ret_block,e->b0->expr,e->b0->expr_state);
				break;

			case OP_RBLOCK:
				b1 = &e->stack[e->b0->end_block];
				e->stack_index = b1->index;
				if(e->b0->end!=e->b0->pos) e->pos = e->b0->end;
				e->b0 = b1;
if(debug) q_outd(0,"OP_RBLOCK[%d] pos: %d, end: %d, end_block: %d, ret: %d, ret_block: %d, expr: %d, expr_state: %d" STR_NL,e->b0->index,e->b0->pos,e->b0->end,e->b0->end_block,e->b0->ret,e->b0->ret_block,e->b0->expr,e->b0->expr_state);
				break;

			case OP_ELSE:
				q_block_end(e,0,1);
if(debug) q_outd(0,"OP_ELSE[%d] pos: %d, end: %d, end_block: %d, ret: %d, ret_block: %d, expr: %d, expr_state: %d" STR_NL,e->b0->index,e->b0->pos,e->b0->end,e->b0->end_block,e->b0->ret,e->b0->ret_block,e->b0->expr,e->b0->expr_state);
				break;

			case OP_NIF:
				e->b0->expr = !e->b0->expr;
			case OP_IF:
				a = e->b0->expr;
				e->b0->expr = -1; // Reset expr for currect block
				if(a==0) q_block_end(e,1,1);
if(debug) q_outd(0,"OP_IF[%d] pos: %d, end: %d, end_block: %d, ret: %d, ret_block: %d, expr: %d, expr_state: %d" STR_NL,e->b0->index,e->b0->pos,e->b0->end,e->b0->end_block,e->b0->ret,e->b0->ret_block,e->b0->expr,e->b0->expr_state);
				break;

			case OP_IS: // Logical operators
			case OP_NIS:
			case OP_EQ:
			case OP_NEQ:
			case OP_LT:
			case OP_GT:
			case OP_LTEQ:
			case OP_GTEQ:
					  if(o==OP_IS)   a = !var_empty(v0);
				else if(o==OP_NIS)  a = var_empty(v0);
				else if(o==OP_EQ)   a = var_cmp(v0,v1)==0;
				else if(o==OP_NEQ)  a = var_cmp(v0,v1)!=0;
				else if(o==OP_LT)   a = var_cmp(v0,v1)<0;
				else if(o==OP_GT)   a = var_cmp(v0,v1)>0;
				else if(o==OP_LTEQ) a = var_cmp(v0,v1)<=0;
				else if(o==OP_GTEQ) a = var_cmp(v0,v1)>=0;
				else break;
if(debug) q_outd(0,"Logic: expr: %d, expr_state: %d, a: %d, o: 0x%X, v0type: %d, v0i: %ld" STR_NL,e->b0->expr,e->b0->expr_state,a,o,v0->type,v0->i);
				if(e->b0->expr==-1) e->b0->expr = a;
				else if(e->b0->expr_state==EXPR_AND) e->b0->expr = (e->b0->expr && a);
				else if(e->b0->expr_state==EXPR_OR)  e->b0->expr = (e->b0->expr || a);
				break;

			case OP_INT2:
				v1 = v0;
			case OP_INT:
				if(v1->type==STR) {
					if(str_is_int(v1->s))   v0->i = strtol((char *)str_data(v1->s),&p,0), v0->type = INT;
					else                    v0->i = str_val_sum(v1->s,0),                 v0->type = INT;
				} else if(v1->type==FLOAT) v0->i = (long)v1->f,                          v0->type = INT;
				  else if(v1->type==INT)   v0->i = var_ired(v1->i,10),                   v0->type = INT;
				break;

			case OP_FLOAT:
					  if(v0->type==FLOAT)   v0->f = round(v1->f);
				else if(v0->type==INT)     v0->f = (double)v1->i,                        v0->type = FLOAT;
				break;

			case OP_INC:
					  if(v0->type==INT)   ++v0->i;
				else if(v0->type==FLOAT) ++v0->f;
				break;

			case OP_DEC:
					  if(v0->type==INT)   --v0->i;
				else if(v0->type==FLOAT) --v0->f;
				break;

			case OP_POW:
					  if(v1->type==FLOAT && v2->type==FLOAT) v0->f = pow(v2->f,v1->f),              v0->type = FLOAT;
				else if(v1->type==FLOAT && v2->type==INT)   v0->f = pow((double)v2->i,v1->f),      v0->type = FLOAT;
				else if(v1->type==INT && v2->type==FLOAT)   v0->f = pow(v2->f,(double)v1->i),      v0->type = FLOAT;
				else if(v1->type==INT && v2->type==INT)     v0->f = var_ipow(v2->i,v1->i),         v0->type = INT;
				break;

			case OP_SQRT:
					  if(v1->type==FLOAT)                    v0->f = sqrt(v1->f),                   v0->type = FLOAT;
				else if(v1->type==INT)                      v0->i = var_isqrt(v1->i),              v0->type = INT;
				break;

			case OP_LSHIFT2:
				v2 = v0;
			case OP_LSHIFT:
				if(v1->type==INT && v2->type==INT)    v0->i = (v2->i << v1->i), v0->type = INT;
				break;

			case OP_RSHIFT2:
				v2 = v0;
			case OP_RSHIFT:
				if(v1->type==INT && v2->type==INT)    v0->i = (v2->i >> v1->i), v0->type = INT;
				break;

			case OP_AND2:
				v2 = v0;
			case OP_AND:
				if(v1->type==INT && v2->type==INT)    v0->i = (v2->i &  v1->i), v0->type = INT;
				break;

			case OP_OR2:
				v2 = v0;
			case OP_OR:
				if(v1->type==INT && v2->type==INT)    v0->i = (v2->i |  v1->i), v0->type = INT;
				break;

			case OP_XOR2:
				v2 = v0;
			case OP_XOR:
				if(v1->type==INT && v2->type==INT)    v0->i = (v2->i ^  v1->i), v0->type = INT;
				break;

			case OP_NOT2:
				v1 = v0;
			case OP_NOT:
				if(v1->type==INT)                     v0->i = ~v1->i,           v0->type = INT;
				break;

			case OP_ABS:
					  if(v1->type==FLOAT && v1->f<0.0) v0->f = -v1->f,           v0->type = FLOAT;
				else if(v1->type==INT   && v1->i<0)   v0->i = -v1->i,           v0->type = INT;
				break;

			case OP_NEG:
					  if(v1->type==FLOAT && v1->f>0.0) v0->f = -v1->f,           v0->type = FLOAT;
				else if(v1->type==INT &&   v1->i>0)   v0->i = -v1->i,           v0->type = INT;
				break;

			case OP_FLOOR:
				if(v0->type==FLOAT) v0->f = floor(v0->f);
				break;

			case OP_CEIL:
				if(v0->type==FLOAT) v0->f = ceil(v0->f);
				break;

			case OP_RED:
					  if(v2->type==STR && v1->type==INT) v0->i = var_ired(str_val_sum(v2->s,0),v1->i), v0->type = INT;
				else if(v2->type==INT && v1->type==INT) v0->i = var_ired(v2->i,v1->i),                v0->type = INT;
				break;

			case OP_ELVIS2:
				v2 = v1,v1 = v0;
			case OP_ELVIS:
				if(var_empty(v1)) v1 = v2;
				var_set(v0,v1);
				s = NULL;
				break;

			case OP_INPUT:
				q_input(e,v0,0);
				break;

			case OP_DOUT:
				for(l=0,p=(char *)&e->src[e->pos+1]; (c=p[l]); ++l) {
					if(c=='<' && p[l+1]=='?') break;
					fputc(c,e->out);
					newline = c!='\n' && c!='\r';
				}
				if(!c) goto exec_end;
				e->pos += l+2;
if(debug) q_outd(0,"OP_DOUT: %c" STR_NL,e->src[e->pos]);
				s = NULL;
				break;
			case OP_DOUTE:
				break;

			case OP_DSTR:
				for(l=0,p=(char *)&e->src[e->pos+1],a=1; (c=p[l]); ++l) {
					if(c=='&' && p[l+1]=='>') ++a,++l;
					else if(c=='<' && p[l+1]=='&') {
						if(!--a) break;
						else ++l;
					}
				}
				if(!c) goto exec_end;
				var_set_str(&e->vt,str_new_dup(&e->src[e->pos+1],l));
				var_set(v0,&e->vt);
				e->pos += l+2;
if(debug) q_outd(0,"OP_DSTR: %c, str: \"%s\"" STR_NL,e->src[e->pos],(char *)str_data(v0->s));
				s = NULL;
				break;
			case OP_DSTRE:
				break;

			case OP_POS:
				b = e->pos;
				next(d,a,e->src,b);
//if(debug) q_outd(0,"OP_POS: %c" STR_NL,d);
				if(a==OP_LBLOCK) { // Followed by a block
					e->pos = b;
					var_set_int(v0,e->pos);
					q_block_end(e,0,0);
//if(debug) q_outd(0,"OP_POS: pos = %d" STR_NL,e->pos);
				} else {
					var_set_int(v0,e->pos);
				}
				s = NULL;
				break;

			case OP_LOOP:
				e->b0->expr = -1;
				e->pos = e->b0->pos;
if(debug) q_outd(0,"OP_LOOP[%d] pos: %d, end: %d, end_block: %d, ret: %d, ret_block: %d, expr: %d, expr_state: %d" STR_NL,e->b0->index,e->b0->pos,e->b0->end,e->b0->end_block,e->b0->ret,e->b0->ret_block,e->b0->expr,e->b0->expr_state);
				break;

			case OP_RETURN:
				e->pos = e->b0->ret;
				e->b0 = &e->stack[e->b0->ret_block];
if(debug) q_outd(0,"OP_RETURN[%d] pos: %d, end: %d, end_block: %d, ret: %d, ret_block: %d, expr: %d, expr_state: %d" STR_NL,e->b0->index,e->b0->pos,e->b0->end,e->b0->end_block,e->b0->ret,e->b0->ret_block,e->b0->expr,e->b0->expr_state);
				break;

			case OP_INCLUDE:
				if(v0->type==STR) {
					a = 0,p = file_read((const char *)str_data(v0->s),&l);
					if(p && l>0) {
						if(*p=='#' && p[1]=='!') {
							q = strchr(p,'\n');
							if(!q) q = strchr(p,'\r');
							if(q) a = (int)(q-p)+1;
						}
if(debug) q_outd(0,"OP_INCLUDE: len: %d, src:" STR_NL "%s" STR_NL,l,p);
						if(l-a>0) e = q_open(p,a,l,e->in,e->out,e);
					}
				}
				break;

			case OP_EXEC:
if(debug) q_outd(0,"OP_EXEC: len: %d, src:" STR_NL "%s" STR_NL,v0->s->len,(char *)str_data(v0->s));
				if(v0->type==STR && v0->s->len>0) {
					p = (char *)malloc(v0->s->len+1);
					strcpy(p,(char *)str_data(v0->s));
					e = q_open(p,0,v0->s->len,e->in,e->out,e);
				}
				break;

			case OP_COPEN:
				for(a=1; (c=e->src[++e->pos]); ) {
					if(c=='/' && e->src[e->pos+1]=='*') ++a,++e->pos;
					else if(c=='*' && e->src[e->pos+1]=='/' && !--a) break;
				}
				if(c=='\0') --e->pos;
				else ++e->pos;
				break;
			case OP_CCLOSE:break;
		}
		if(s && (v0->type!=STR || v0->s!=s)) { // Free string if value of V0 has changed
//if(debug) q_outd(0,"str_free(%s)" STR_NL,(char *)str_data(s));
			str_free(s);
			s = NULL;
		}
	}
	if(0) {
exec_err_stack_overflow:
		q_oute(0,PACKAGE "[%d]: %s" STR_NL,e->pos,_("Stack overflow"));
	}
exec_end:
	e = q_close(e);
	if(e) goto exec_start;
}

q_env *q_open(char *src,int pos,int len,FILE *in,FILE *out,q_env *pe) {
	q_env *e = NULL;
	if(src && *src) {
		int i;
		q_block *stack  = (q_block *)malloc(sizeof(q_block)*STACK);
		var     *vars   = (var *)malloc(sizeof(var)*VARS);

		e = (q_env *)malloc(sizeof(q_env));
		*e = (q_env){
			parent:      pe,
			src:         (utf8_t *)src,
			len:         len? len : strlen(src),
			pos:         pos-1, // Position before first char in src
			stack:       stack,
			stack_index: 0, // Position before first block in stack
			b0:          &stack[0],
			va:          vars,
			va_len:      VARS,
			v0:          &vars[ALEPH],
			v1:          &vars[ALEPH],
			v2:          &vars[ALEPH],
			vt:          { index: 0, type: VOID, i: 0 },
			in:          in,
			out:         out
		};

		for(i=0; i<STACK; ++i)
			e->stack[i] = (q_block){
				index:      i,
				pos:        e->pos,
				end:        e->len-1,
				end_block:  0,
				ret:        e->len-1,
				ret_block:  0,
				expr:       -1,
				expr_state: EXPR_AND
			};

		for(i=0; i<VARS; ++i)
			e->va[i] = (var){
				index: i,
				type:  VOID,
				i:     0
			};

	}
	return e;
}

q_env *q_close(q_env *e) {
	q_env *pe = NULL;
	if(e) {
		int i;
//if(verbose) q_outv(0,"%s:" STR_NL "%s" STR_NL "EOF" STR_NL,_("Executed program",s);
		pe = e->parent;
		if(!pe || e->src!=pe->src)
			free(e->src);
		if(!pe || e->stack!=pe->stack)
			free(e->stack);
		if(!pe || e->va!=pe->va) {
			for(i=0; i<VARS; ++i)
				var_free(&e->va[i]);
			free(e->va);
		}
		var_free(&e->vt);
		free(e);
	}
	return pe;
}

static void run(char *src,int len,FILE *in,FILE *out) {
	q_env *e = q_open(src,0,len,in,out,NULL);
	if(e) q_exec(e);
}

static char *file_read(const char *file,int *len) {
	int l = 0,r;
	char *src = NULL;
	FILE *fp = fopen(file,"rb");
	if(fp) {
		fseek(fp,0,SEEK_END);
		if((l=ftell(fp))>0) {
			src = (char *)malloc(l+1);
			fseek(fp,0,SEEK_SET);
			r = fread(src,1,l,fp);
			(void)r;
			src[l] = '\0';
if(verbose) q_outv(0,"%s [%d]:" STR_NL ANSI_COLOR_YELLOW "%s" ANSI_COLOR_VERBOSE STR_NL "EOF" STR_NL,_("Read file"),l,src);
		}
		fclose(fp);
	}
	if(len) *len = l;
	return src;
}


#ifdef CLI
#define cli_prompt PACKAGE " > "
static int command(FILE *in,int tty,char *src,char *ln,int l) {
	int r = 1;

	if(l==1 && !strncmp(ln,"#!",2)) r = 1;
	else if(!strcmp(ln,"eof" ) ||
			  !strcmp(ln,"run" )) r = -1;
	else if(!strcmp(ln,"exit") ||
	        !strcmp(ln,"quit")) r = 2;
	else if(tty && !strcmp(ln,"show")) r = 3;
	else if(tty && !strcmp(ln,"help")) r = 4;
	else return 0;

	*ln = '\0';

	     if(r==2) exit(0);
	else if(r==3) printf("%s",src);
	else if(r==4) printf(_(INT_MOD_HELP));
	return r;
}
#define cli_command command
#include "cli.c"

#include "opt.c"

static opt opts[] = {
	{   'd', "debug",    OPT_FLAG,  NULL, "execute with debugging" },
	{ 0x101, "verbose",  OPT_FLAG,  NULL, "verbose output" },
	{   'v', "version",  OPT_FLAG,  NULL, "show program version" },
	{   'h', "help",     OPT_FLAG,  NULL, "show this message" },
{0}};

int main(int argc,char **argv) {
	FILE *in  = stdin;
	FILE *out = stdout;
	char *src;
	int i,tty   = 1,len;
	opt *o;
#ifdef __unix__
	tty = isatty(0);
#endif
	opt_parse(&argc,argv,opts,1);
	for(i=0; (o=&opts[i])->id; ++i)
		if(o->match)
			switch(o->id) {
				case 'd':debug = 1;break;
				case 0x101:verbose = 1;break;
				case 'v':
					printf(_(USAGE_VERSION),PACKAGE_VERSION,PACKAGE_YEAR,PACKAGE_MAINTAINER);
					return 0;
				case 'h':
					printf(_(USAGE_HEADER));
					opt_print(stdout,opts);
					printf(_(USAGE_FOOTER),PACKAGE_BUGREPORT,PACKAGE_NAME,PACKAGE_URL);
					return 0;
			}
	if(argc>=2 && (in=fopen(argv[argc-1],"rb"))==NULL) {
		fprintf(stderr,"%s: %s" STR_NL,_(ERR_FILE_IN),argv[argc-1]);
		return 1;
	}
	cli_init();
	if(in==stdin && tty)
		fprintf(out,_(INT_MOD_HEADER),PACKAGE_VERSION);
	if(in!=stdin || !tty) {
		src = cli_read(in,tty,&len);
		if(in!=stdin) fclose(in);
		if(src!=NULL && len>0) {
			run(src,len,stdin,out);
			if(out==stdout && newline) q_outc(EOF,out);
		}
	} else {
		while(1) {
			if((src=cli_read(in,tty,&len))!=NULL && len>0) {
				run(src,len,stdin,out);
				if(out==stdout && newline) q_outc(EOF,out);
			}
		}
	}
	return 0;
}
#endif /* CLI */

