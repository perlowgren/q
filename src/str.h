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
 * @file str.h  
 * @author Per Löwgren
 * @date Modified: 2016-02-14
 * @date Created: 2016-02-06
 */ 

/* 
 * Q language string handling; struct and functions
 * 
 * This file has been designed to be included with #include
 * and can be controlled with macros at build time. If it's
 * not included but linked in, it will use default
 * functionality.
 * 
 * If linked in, the non-static functions need prototype definitions.
 */
#ifndef _Q_STR_H_
#define _Q_STR_H_

#if defined _MSDOS || defined _WIN32
#define STR_NL "\r\n"
#elif defined macintosh // OS 9
#define STR_NL "\r"
#else
#define STR_NL "\n" // Mac OS X uses \n
#endif


enum {
/*             Nr Latin MDS     Value   UTF-8         */
	ALEPH,   //  0 A     Mother    1     0xD790        
	BETH,    //  1 B     Double    2     0xD791        
	GIMEL,   //  2 G     Double    3     0xD792        
	DALETH,  //  3 D     Double    4     0xD793        
	HEH,     //  4 H     Single    5     0xD794        
	VAV,     //  5 OUVW  Single    6     0xD795        
	ZAIN,    //  6 Z     Single    7     0xD796        
	CHETH,   //  7 X     Single    8     0xD797        
	TETH,    //  8 J     Single    9     0xD798        
	YOD,     //  9 IY    Single   10     0xD799        
	KAPH,    // 10 K     Double   20,500 0xD79B,0xD79A 
	LAMED,   // 11 L     Single   30     0xD79C        
	MEM,     // 12 M     Mother   40,600 0xD79E,0xD79D 
	NUN,     // 13 N     Single   50,700 0xD7A0,0xD79F 
	SAMEKH,  // 14 S     Single   60     0xD7A1        
	AYIN,    // 15 E     Single   70     0xD7A2        
	PEH,     // 16 P     Double   80,800 0xD7A4,0xD7A3 
	TZADDI,  // 17 C     Single   90,900 0xD7A6,0xD7A5 
	QOPH,    // 18 Q     Single  100     0xD7A7        
	RESH,    // 19 R     Double  200     0xD7A8        
	SHIN,    // 20 F     Mother  300     0xD7A9        
	TAU      // 21 T     Double  400     0xD7AA        
};

// Unicode Hebrew to Latin:
extern int uh2l[];

// Hebrew to Latin:
extern int h2l[];

// Latin to Hebrew:
extern int l2h[];

#define isunicode(c) (((c)&0xc0)==0xc0)
#define utf8_len(u)  (((u)&0x20)? (((u)&0x10)? (((u)&0x08)? (((u)&0x04)? (((u)&0x02)? 0 : 6) : 5) : 4) : 3) : 2)

typedef unsigned char utf8_t;
typedef struct str str;

struct str {
	char ref;      // Reference count
	utf8_t *data;  // String data
	int len;       // Length
};

int utf8_decode(const utf8_t *s,int *i);

#define str_data(s) ((s)->data)

str *str_new(utf8_t *s,int l);
str *str_new_dup(const utf8_t *s,int l);
void str_free(str *s);
str *str_dup(str *s);
int str_len(const utf8_t *p);
str *str_join(str *s1,str*s2);
int str_apply(utf8_t *p,str *s);
int str_val_sum(str *s,int l);

int str_is_int(str *s);
int str_is_float(str *s);

#endif /* _Q_STR_H_ */

