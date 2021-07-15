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
 * @file opt.c  
 * @author Per Löwgren
 * @date Modified: 2016-02-10
 * @date Created: 2016-02-06
 */ 

/* 
 * Parsing of command line arguments
 * 
 * This file has been designed to be included with #include
 * and can be controlled with macros at build time. If it's
 * not included but linked in, it will use default
 * functionality.
 * 
 * If linked in, the non-static functions need definitions.
 * 
 * The options parser follows the POSIX recommended conventions
 * for command line arguments; the implementation however differs
 * from getopt and argp in a few ways.
 * 
 * Accepted arguments can be in the format:
 *  "-f"           short, flag
 *  "-fVALUE"      short, value
 *  "-f VALUE"     short, value
 *  "-fabc"        short, flags; where a, b, c are also recognised flags
 *  "--foo"        long, flag
 *  "--foo=VALUE"  long, value
 *  "--foo VALUE"  long, value
 * 
 * The parser expects an array of opt-objects. All values should
 * be set appropriately prior to calling opt_parse. The const-
 * properties flag, arg, type, val and desc will not be changed
 * during parsing.
 * 
 * If there is a matching argument for an option the match-property
 * will be set to the value found, and only values are accepted
 * that are declared in the type-property, e.g. if type is set to
 * OPT_INT, then only a numeric value will be accepted as a match.
 * If however type is set to OPT_FLAG|OPT_STRING, depending on the
 * format of the argument either flag or string values will be
 * accepted matches.
 * 
 * Matches are accepted in the order they are found in the argv
 * array. If more than one match is found in argv for the same
 * option, then the first is used and the number returned is
 * incremented.
 * 
 * Matched options and values are removed from argv, and argc is
 * adjusted accordingly. To keep argv untouched use a copy of argv.
 * The strings in argv remain untouched.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define OPT_TAB_MIN   8       //!< Minimum tab width for option descriptions
#define OPT_TAB_MAX   27      //!< Maximum tab width for option descriptions
#define OPT_TAB_SEP   4       //!< Minimum spaces between declaration and description
#define OPT_DESC_MAX  55      //!< Maximum width for option descriptions

/** Accepted option char flags: */
#define opt_flag_char(c) ((c)!='-' && (c)>='!' && (c)<='~')

typedef struct opt opt;

struct opt {
	const int id;         //!< Short format of option, e.g. "-f", or option id. For last option in array, id is set to zero and is ignored
	const char *arg;      //!< Long format of option, e.g. "--foo"
	const int type;       //!< Accepted types, can be one or many
	const char *val;      //!< Value name when printed, e.g. in this example val is "BAR": "  --foo BAR   foo has value of BAR"
	const char *desc;     //!< Description of option when printed
	int match;            //!< If a match, set to the type of the value of arg, e.g. if type is (OPT_FLAG|OPT_INT) and arg is "--foo=123", then match is set to OPT_INT
	union {
		long i;            //!< Value of option is stored as a long integer
		double f;          //!< Value of option is stored as a double float
		const char *s;     //!< Value of option is stored as a string
	};
};

/** Flags for the type-property of the opt struct. Flags can be combined
 * and then several values will be accepted, but only one is set when matched.
 */
enum {
	OPT_FLAG   = 1,    //!< Value is a flag
	OPT_INT    = 2,    //!< Value is a long integer, or if value is a float then only the integer part is parsed
	OPT_FLOAT  = 4,    //!< Value is a double float
	OPT_NUM    = 6,    //!< Value is a numeric (integer or float)
	OPT_STR    = 8,    //!< Value is a string
	OPT_VAL    = 14,   //!< Value is a string or numeric
};

static int opt_check_num(const char *s) {
	int c,r = 0;
	if(*s=='-' || *s=='+') ++s;
	for(; (c=*s); ++s) {
		if(c>='0' && c<='9' && !r) r = OPT_INT;
		else if(c=='.' && !r) r = OPT_FLOAT;
		else if(r && (c=='.' || c=='e' || c=='E')) r = OPT_FLOAT;
		else break;
	}
	return r;
}

/** Parse options from the main() arguments array
 * @param argc The argc in main(), if there are matched options, this is set to new number of arguments
 * @param argv The argv in main(), matched options are removed from this array
 * @param opts Options to match in argv
 * @param clear Clear matches in opts array (boolean)
 * @return The number of options where more than one match is found, if >0 re-run opt_parse to parse those args too
 */
int opt_parse(int *argc,char *argv[],opt *opts,int clear) {
	/* c = argc, i = argv index, m = already matched args,
	 * n = opts index, s = is (0)short/(1)long arg,
	 * l = option arg strlen,
	 * k = number of opt-flags in arg, t = temp, q = match */
	int c = *argc,i,m = 0,n,s,l,k,t,q;
	char *a = NULL,*v;
	opt *o,*o1 = NULL;
	if(clear)
		for(n=0; (o=&opts[n])->id; ++n) // Reset matches
			o->match = 0,o->i = 0;
	if(argc==0 || argv==NULL) return m;
	for(i=0,k=0; 1; ) {
//if(debug) q_outd(0,"opt_parse(i: %d, k: %d, a: %s)\n",i,k,a);
		if(!k) ++i; // Next arg
		else { // Arg is a series of short opt-flags, e.g. "-abc"
			++a; // Next flag
			if(*a=='\0' || k==1) ++i,k = 0; // Last flag, next arg
		}
		if(i>=c) break; // Last arg
		if(k || ((a=argv[i])!=NULL && *a=='-')) { // Get arg and check it's an option
//if(debug) q_outd(0,"opt_parse(i: %d, k: %d, a: %s)\n",i,k,a);
			if((s=(!k && *(++a)=='-')) && // Check if arg is long
			   *(++a)=='\0') break; // Stop parsing on "--"
			if(!s && !k) { // If short, test if all flags match, or at least first
				for(v=a; *v!='\0'; ++v,++k) {
					for(n=0,t=0; (o=&opts[n])->id; ++n) { // Iterate options
						if(!o->type) continue;
						if(!opt_flag_char(o->id) || *v!=o->id) continue; // Not a match
						if(k>0 && !(o->type&OPT_FLAG)) continue; // Not a flag option
						t = 1; // Match
						break;
					}
					if(!t && k>1) k = 1; // First flag is a match
					if(!t) break;
					if(!k) o1 = o; // Store first match
				}
				if(k==1 && a[1]!='\0' && !(o1->type&OPT_VAL)) k = 0; // "-fVAL" where option isn't accepting a value
				if(!k) continue;
			}
//			if(!s && !k) k = 1; // If short, set k to first flag
			for(n=0; (o=&opts[n])->id; ++n) { // Iterate options
				if(!o->type) continue;
				if(s) { // Argument is long format "--foo"
					if(o->arg==NULL || *o->arg=='\0') continue; // Ignore options with empty arg-proptery
//if(debug) q_outd(0,"opt_parse(long: a: %s, o->arg: %s)\n",a,o->arg);
					l = strlen(o->arg);
//if(debug) q_outd(0,"opt_parse(long: l: %d, a: %s, o->arg: %s)\n",l,a,o->arg);
					if(*a!=*o->arg || strncmp(a,o->arg,l)) continue; // Not a match
				} else { // Argument is short format "-f"
					if(!opt_flag_char(o->id) || *a!=o->id) continue; // Not a match
//if(debug) q_outd(0,"opt_parse(short: a: %c, o->id: %c)\n",*a,o->id);
					if(k>1 && !(o->type&OPT_FLAG)) continue; // Not a flag option
				}
//if(debug) q_outd(0,"opt_parse(match: o->arg: %s, s: %d)\n",o->arg,s);
				if(o->match) {
					++m; // Option alread matched
					continue;
				}
				if((o->type&OPT_VAL) && (s || k==1)) { // If accepting value
					v = NULL,t = 0,q = 0;
					if(s && a[l]=='=') v = a+l+1; // If long with value "--foo=value"
					else if(!s && a[1]!='\0') v = a+1; // If short with value "-fvalue"
					if(o->type&OPT_NUM) {
						if(v!=NULL) t = opt_check_num(v);
						else if(i+1<c) {
							t = opt_check_num(argv[i+1]);
							if(t) v = argv[i+1],argv[i+1] = NULL;
						}
					} else if(v==NULL && i+1<c && *argv[i+1]!='-') {
						v = argv[i+1],argv[i+1] = NULL; // If long or short of type "-f value" or "--foo value"
					}
//if(debug) q_outd(0,"opt_parse(match: value: %s, t: %d)\n",v,t);
					if(v==NULL) {
						if((o->type&OPT_FLAG)) q = OPT_FLAG;
					} else if((o->type&OPT_FLOAT) && (t==OPT_FLOAT || // Value is a float
					          (!(o->type&OPT_INT) && t==OPT_INT))) { // or an int, but only accepting float
						q = OPT_FLOAT; // Double float value
						o->f = strtod(v,&v);
					} else if((o->type&OPT_INT) && t) { // Accepting int and value is int or float
						q = OPT_INT; // Long integer value
						o->i = strtol(v,&v,0);
					} else if((o->type&OPT_STR)) {
						q = OPT_STR; // String value
						o->s = v;
					}
					k = 0;
					if(q) {
						argv[i] = NULL;
						o->match = q;
					}
				} else if((o->type&OPT_FLAG)) { // If accepting flag value
					o->match = OPT_FLAG;
					if(s || a[1]=='\0') argv[i] = NULL;
				}
				break;
			}
		}
	}

	for(i=0,n=0; i+n<c; )
		if(argv[i+n]==NULL) ++n;
		else argv[i] = argv[i+n],++i;
	c -= n;

//if(debug) {printf(ANSI_COLOR_DEBUG "argc: %d" STR_NL,c);
//for(i=0; i<c; ++i)
//printf("argv[%d]: %s" STR_NL,i,argv[i]);
//for(n=0; (o=&opts[n])->id; ++n) {
//if(!o->type) continue;
//printf("opt[%d, '%c', \"%s\", type: %d]: match=%d",n,opt_flag_char(o->id)? o->id : '-',o->arg!=NULL? o->arg : "",o->type,o->match);
//if(o->match==OPT_INT) printf(", i=%ld",o->i);
//if(o->match==OPT_FLOAT) printf(", f=%g",o->f);
//else if(o->match==OPT_STR) printf(", s=%s",o->s);
//fputs(STR_NL,stdout);
//}
//printf("m: %d" STR_NL ANSI_COLOR_RESET,m);newline=0;}

	*argc = c;
	return m;
}

/** Print options, i.e. to be used when printing "--help" message
 * @param out Output stream, if NULL stdout is used
 * @param opts Options to be printed
 */
void opt_print(FILE *out,opt *opts) {
	int n,s,t,l,vl,dl,br;
	opt *o;
	const char *v,*d,*p1,*p2;
	if(out==NULL) out = stdout;
	for(n=0,s=0; (o=&opts[n])->id && !s; ++n) // Seek for occurence of short option "-f"
		if(o->type && opt_flag_char(o->id)) s = 4;
	for(n=0,t=OPT_TAB_MIN; (o=&opts[n])->id; ++n) { // Calculate tab width:
		if(!o->type) continue;
		if(!opt_flag_char(o->id) && o->arg==NULL) continue;
		l = 4; // "  -f"
		if(o->arg!=NULL) l += strlen(o->arg)+s; // "  --foo" and "  -f, --foo"
		if(o->val!=NULL) l += 1+strlen(o->val); // "  -f, --foo VAL"
		l += OPT_TAB_SEP;
		if(l>t) t = l;
	}
	if(t>OPT_TAB_MAX) t = OPT_TAB_MAX;
	for(n=0; (o=&opts[n])->id; ++n) {
		if(!o->type) {
			if(o->desc!=NULL) fprintf(out,"%s\n",o->desc);
			continue;
		}
		if(!opt_flag_char(o->id) && o->arg==NULL) continue;
		l = o->arg!=NULL? strlen(o->arg) : 0;
		v = o->val!=NULL? o->val : "";
		vl = strlen(v);
		d = o->desc!=NULL? o->desc : "";
		dl = strlen(d);
		br = 0;
		if(o->arg==NULL) fprintf(out,"  -%c %s%*s",o->id,v,(dl && !(br=(t<5+vl))? t-5-vl : 0),"");
		else if(!opt_flag_char(o->id)) fprintf(out,"%s--%s%c%s%*s",s? "      " : "  ",o->arg,*v? '=' : ' ',v,(dl && !(br=(t<5+s+l+vl))? t-5-s-l-vl : 0),"");
		else fprintf(out,"  -%c, --%s%c%s%*s",o->id,o->arg,*v? '=' : ' ',v,(dl && !(br=(t<9+l+vl))? t-9-l-vl : 0),"");
		if(!dl || br) fputc('\n',out);
		for(; dl>0; d=p1+1) {
			if(dl>OPT_DESC_MAX) {
				for(p1=d; (p2=strchr(p1+1,' '))!=NULL && (int)(p2-d)<OPT_DESC_MAX; p1=p2);
				if(p1==d && p2!=NULL) p1 = p2;
				if(p1==NULL) break;
				fprintf(out,"%.*s\n",(int)(p1-d),d);
				dl -= (int)(p1-d+1);
				if(dl>0) fprintf(out,"%*s",t,"");
			} else {
				fprintf(out,"%s\n",d);
				dl = 0;
			}
		}
	}
}

