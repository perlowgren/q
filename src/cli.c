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
 * @file cli.c  
 * @author Per Löwgren
 * @date Modified: 2016-02-10
 * @date Created: 2016-02-06
 */ 

/* 
 * Command Line interface functions
 * 
 * This file has been designed to be included with #include
 * and can be controlled with macros at build time. If it's
 * not included but linked in, it will use default
 * functionality.
 * 
 * If linked in, the non-static functions need prototype definitions.
 * 
 * To use readline-functions, define READLINE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __unix__
#include <unistd.h>
#endif
#ifdef READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#ifndef cli_prompt
#define cli_prompt "> " // Default prompt, define cli_prompt elsewhere for other prompt
#endif /* cli_prompt */

#ifndef cli_command
// Default commands function, define cli_command elsewhere for other function
#define cli_command command
// Default commands, only shebang ("#!") and "exit" handled by default
static int command(FILE *in,int tty,char *src,char *ln,int l) {
	int r = 1;
	if(l==1 && !strncmp(ln,"#!",2)) r = 1;
   else if(!strcmp(ln,"exit")) exit(0);
	else return 0;
	*ln = '\0';
	return r;
}
#endif /* cli_command */

// Resize buffer for input data
static char *cli_resize(char *p,size_t *n,size_t i) {
	if(i>=*n) {
		size_t l = (*n)*2;
		l = l<i? i+1 : l;
		p = realloc(p,l);
		*n = l;
	}
	return p;
}

#ifdef READLINE
void cli_init() {
	rl_gnu_readline_p = 1;
	rl_instream = NULL;
	rl_bind_key('\t',rl_insert);
//	rl_erase_empty_line = 1;
}
#else
#define cli_init() ()
#endif

/** Read input from command line until EOF, and handling commands
 * @param in Input stream
 * @param tty If input is read from terminal
 * @param len Is set to length of input data in bytes
 * @return An allocated buffer containing input data; should be freed with free()
 */
char *cli_read(FILE *in,int tty,int *len) {
	int i,n,l,c,p,p1;
	char buf[81],*ln = NULL,*src,*s;
	size_t cap = 1024;
	if(len) *len = 0;
	if((src=malloc(cap+1))==NULL)
		return src;
	for(i=0,l=1,p=0,*src='\0'; 1; p+=i,++l) {
#ifdef READLINE
		if(in==stdin && tty) {
//printf("buffer: \"%s\"\n",rl_line_buffer);
			if(ln) free(ln);
			ln = readline(cli_prompt);
//printf("line: \"%s\"\n",ln);
			if(!ln) { printf("\n");break; } // EOF
			if(*ln) add_history(ln);
			i = strlen(ln);
			if((src=cli_resize(src,&cap,p+i+1))==NULL) return src;
			strcpy(&src[p],ln);
		} else {
#else
		if(in==stdin && tty) printf(cli_prompt);
#endif
		ln = buf,*ln = '\0',src[p] = c = '\0';
		for(p1=p,i=0; c!='\n'; p1+=n) {
			if(fgets(ln,81,in)==NULL) break;
			n = strlen(ln);
//printf("ln[n: %d]: %s\n",n,ln);
			if(!n) break;
			if((c=ln[n-1])=='\n' || c=='\r') { // remove '\n' & '\r'
				if(n>1 && ((c=ln[n-2])=='\n' || c=='\r')) --n;
				ln[--n] = '\0',c = '\n';
			}
//printf("ln[cap: %d, p: %d, n: %d]: %s\n",(int)cap,p,n,ln);
			if((src=cli_resize(src,&cap,p+i+n+1))==NULL) return src;
			strcpy(&src[p1],ln);
			i += n;
		}
#ifdef READLINE
		}
#endif
		s = &src[p];
		c = cli_command(in,tty,src,s,l);
//printf("ln[cap: %d, p: %d, i: %d, c: %d]: %s\n",(int)cap,p,i,c,s);
		if(c==0) s[i] = '\n',s[++i] = '\0';
		else i = 0;
		if(c==-1 || feof(in)) break;
	}
	if(ln && ln!=buf) free(ln);
	if(len) *len = p;
//printf("src:\n%s\n",src);
	return src;
}

