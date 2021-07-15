# Qabalah Language Interpreter

## Introduction

The Qabalah language (Q) is not only an esoteric programming language, but it is also
an occult programming language. The first version of the language was conceived
while in bed with my mobile device and a C compiler app, and the program was a fully
functional interpreter in 112 lines - with only a very narrow set of operators,
and a brainfuck type of variable selection. The code was then transferred to my
computer, and the language started developing into what it is today. It was actually
developed from brainfuck, even though nothing remain of its origins.

## About Qabalah

The idea to base the language on the Qabalah came from the fact that I've been
studying it for years, and the similarities between Qabalah and programming is so
many it's quite remarkable noone has invented something similar before.

## Syntax

All syntax is read from left to right, downwards. White space is ignored. The
only characters recognised by the interpreter are latin letters, and the
selection of 22 operator signs `!#%&()*+-/:;<=>?@[]^|~`. However, unicode
signs for hebrew letters are transcribed to the latin corresponding letters,
according to a defined set of correspondences, e.g. א (aleph) becomes 'A'
etc.

## Variables

There are no names for variables and function in Q, only letters and operators.
There are 22 variables, that's the limit. The number 22 come from the number of
letters in the Hebrew alphabet; and some latin letters share the same variable,
e.g. O, U, V and W all share the variable Vav, they are all references to the
same data and can be used interchangeably, thus all latin letters may be used.
Also Unicode Hebrew letters are accepted, and in the future there may include
Unicode letters of other alphabets, e.g. Greek and Arabic etc. Still, they will
still only refer to the 22 variables, which number won't change. The language
is case insensitive.

All variables are variants, they can contain either void, integer, float, string
or array values. Integers use the C long type, which depending on system is either
32 or 64 bits; float use the C double type, which is always 64 bits; strings use
a localy defined string struct; and arrays remain to be implemented but will simply
contain arrays of the other types, or arrays; void is an uninitiated variable.

Naming a variable in the code will give it focus, and can then be operated upon by
operators. Naming another variable then pushes the former back a step. There
are three "slots" where variables are placed in: **V0**, **V1**, and **V2**. When
first named, the variable is placed in **V0**; naming another pushes it back to **V1**,
and next time to **V2**, e.g. `A` - **A** is placed in **V0**; `AB` - **A** is
placed in **V1**, and **B** in **V0**; `ABC` - **A** is in **V2**, **B** in **V1**,
and **C** in **V0**.

*By default variable **A** is placed in all three **V0**, **V1**, and **V2**.*

There is also a temporary variable **Vt**, where constants are stored before
they are assigned to the variable in **V0**.

Variables that have been named, remain in focus (in **V0** etc.) until another
variable is named in the code. Therefore, they can be operated upon with many
operators. Constants in **Vt** are truly temporary, and only used once; the
next operator will return to using **V0**, **V1** and **V2**, e.g. `A2 B3
C+4*` would in C look like `A = 2, B = 3, C = B+4, C = A*B;`.

## Operators

There are 22 single [operators](Operators), but also [combined operators](Operators),
performing many functions found in other languages. The single operators are:

 * `!` - not equals
 * `#` - undefined at the moment
 * `%` - modulus, same as in C
 * `&` - output variable
 * `(` - begin expression
 * `)` - end expression
 * `*` - multiply
 * `+` - addition
 * `-` - subtraction
 * `/` - division
 * `:` - set variable, same as = in C
 * `<` - less than
 * `=` - equals to
 * `>` - greater than
 * `?` - if
 * `@` - goto
 * `[` - block start
 * `]` - block end
 * `^` - undefined at the moment
 * `|` - else
 * `~` - string, start and end

All of these operators - except block operators `[]()`, and the string
operator `~` - can be combined into new operators with other functionality,
though not all combinations are implemented. Some you may recognise from
other languages, such as `++`, `>=` and `/*`, `*/` - while others are
slightly different, e.g. `+:` is the same as `+=` - and then there are
entirely unique operators, such as `//` for sqrt.

Operators are read in such a fashion that if there are two operator chars
without space between, that can be combined into a defined double-operator,
the combined operator is chosen, otherwise the first char is used as a
single operator and the process is run again on the next char, e.g. for `+&`
there is no combined operator at the time of writing, they are run as separate
operators `+` then `&`, but `+:` can be combined; `++:` will be run `++`
then `:`; `&<<:` will be run `&<` then `<:` etc.

In a later version new combined operators may be added, it is therefore good
practice to add a space after single operators to be on the safe side, and
also for readability. Single operators next to variables and constants
can safely be written with no space between, e.g. `A+B-2`.

Placing single operators next to other operator signs is not
recommended; `+ -` does one thing, and `+-`does something else - the former
is two single char operators, and the latter one double char operator. The
parser will look at the first char, if it is an operator sign, it will look at
the next char, and if there is a defined combined operator for the two chars
it will use that, else it will use the single char sign.

There is a logic to the assignment of operators. The equal-sign is used
only for comparison, and never for assignment; instead the colon-sign is
used for assignment, and therefore the equal-sign has been replaced with
colon in common assignment operators such as `+=`, `-=`, `*=` etc.,
i.e. `+:`, `-:`, `*:` etc.

Some operators have different functions depending on type, but still
resembling; e.g. `##` which cast float to integer, reduce integer to sum
of digits, and calculate sum of char values in strings unless the string
contains an integer, in which case it's converted to integer. Integer
and float values can be cast to string with string operators, e.g. `~&:A~`
which inserts the variable A directly into the string during parsing.

The first char in a combined operator tells which the area the operator
is used for, and the second char which operation to perform; e.g. `%-`
tells that it's for float values, and the minus tells to remove the decimal
part of the float value, i.e. it performs a floor-operation.

The abs `-+` and neg `+-` operators can be analysed too, the first means
that a negative value is turned positive, and the other that a positive
value becomes negative. To invert a value, use a minus operation: `AB0-:`,
here **B** is initially set to zero, then `B -= A` thus **B** becomes **-A**.

*Strings have a specific set of string-operators.*

Operators always operate on previously declared variables, with the exception
of constants - which are then positioned after the operator, e.g. `ABC* DE*4`
would in C translate to `C = A*B, E = D*4;`.

## Constants

Constants can be declared anywhere in Q. Normaly constants are assigned
to **V0**, i.e. the last named variable - but if positioned after certain
operators the constant remains in the temporary variable **Vt**,
and depending on operator, **Vt** is used instead of **V0** or **V1**. For
example: `A+:5`, here 5 is placed in **Vt**, which is added to **A**, `+:`
being equal to the common operator `+=`, where the colon replaces the equals
sign as assignment operator.

If there is a constant after an operator that doesn't make use of
temporary constants, the constant will be assigned to **V0** - which
may generate unwanted results, e.g. `A&'Hello'` will print **A**, then
assign "Hello" to **A**.

Constants can be of all the Q types - integer, float, string, and array.
Integer constants can be of all formats parsed by the C function strtol,
and float constants can be of all formats parsed by the C function
strtod. Strings are described in a separate section.

## Strings

Q strings are surrounded by single quotation marks 'string'.

The backslash \ represents a line break in Q strings, and when printed it
adjusts to local operating system's line breaks, e.g. on Windows it's
"\r\n", on Unix and Linux it's "\n" and on earlier versions of Mac OS "\r".
Also a caret mark '^' represents a tab '\t'.

A string can contain operator codes, making them more dynamic than
ordinary C strings - and these are handled either during parsing, or when
printed (similar to how C's printf handles the format string, e.g. "%s").
String operators start with the '&' sign and names the variable to operate
on, and may have an extra operator for how to handle the variable, e.g.
'What is your name: &<B' is a string that reads input into the variable
**B** when printed.

To insert a '&' in a string, write '&&', e.g. 'A&&B' will print to
"A&B"; also, to insert a single quotation mark write "&'", so 'A&'B'
will print to "A'B". Be careful with the number of '&' though, because
'A&&&B' will print to "A&[B variable]". So, ampersand is both a string
operator and an escape sign.

Variables can be inserted during parsing of the string instead of when
printing; to do this add a colon to the ampersand, e.g. '&:A' will set the
string to the current contents of **A**. 

*Q does not require a string to be ended. If a string continues to the end of
the script, it's just parsed to the end of the script.*

There is also direct output and unformatted strings. Direct output
works like in PHP `?> ... <?php`, the difference to PHP is that Q expects
to start the script with Q code, while PHP expects to start with HTML;
therefore if you want to direct output HTML in Q, your script must
start with `?>`. Q doesn't expect a `q` after `<?`, and so `<?q` will
set **V0** to variable **Q**. The most simple hello world script looks
like this: `?>Hello world!` - notice that no ending `<?` is needed since
the script simply ends at the end of the string.

Unformatted strings are similar to direct output, but instead of printing
the string, it's stored in **V0**. The unformatted string use `&> ... <&`
instead of `?> ... <?`. Unformatted strings cannot be used as string
constants, e.g. `AB= &>hello<&` will compare **B** and **A** for equality,
then assign "hello" to **B**. Unformatted string does not parse any string
operators, or special signs, e.g. `&>Hello &:A!\<&` will set **V0** to
"Hello &:A\".

*Unformatted strings can be nested, e.g. `&>A nested &>unformatted<& string<&` (ok),
but direct output cannot, e.g. `?>A nested ?>direct<? output <?` (fail).*

Some string examples:

```
/* Ask for name and greet, string version */
A 'What is your name: &<N' &
A 'Hello &N!\' &

/* Ask for name and greet, direct output version */
?>What is your name: <?
N &<
?>Hello <? & ?>!
<?

/* Ask for name and greet, unformatted string version */
A &>What is your name: <& &
N &<
A &>Hello <& &
N&
A &>!
<& &

A 'ABC'
B 123
C '&:A &:B &A &B\'
A 'DEF'
B 456
C& /* Output: "ABC 123 DEF 456" */
```

## Blocks

A block is created with the square bracket operators: `[]`. Blocks
are simply blocks, with a start position and an end position, and
they can be nested. There is no evaluation for if nested blocks
match - reaching the end of the script is simply the end of the
script. There are no if, while, for, or function blocks; only
blocks. A block can be made to work as an if-block, a while-block,
or a function-block depending on what is placed inside.

Nested blocks and goto-statements including function calls, pushes
the block to a stack. Each block contains a start position, and
which position and block to return to at the end of the block
and at the *return* operator `@^`, and the state of logical
expressions. Nested blocks inherit the parent block's data. There is
a maximum of 55 blocks in the stack, then a stack overflow error
is reported and the program ends.

### If blocks

If-blocks are constructed with an expression inside, e.g. `[AB= ? C&]`
in C translates to `if(A==B) printf(C);` and `[AB= ? C& | D&]`
translates to `if(A==B) printf(C);else printf(D)`.

### While and for blocks

A while/for-block is constructed the same as an if block, except with
a loop operator at the end, e.g. `A0 [<5? & ++ @<]` will translate
to `for(A=0; A<5; A++) printf("%d",A);`.

### Function blocks

Functions can be constructed by storing the position before a block
with the `@:` operator, and then using the goto operator on that
stored position, e.g. `A@:[B& @^] B'foo bar' A@` translates to:

```
static const char *B;

static void A() {
  printf("%s",B);
  return;
}

void main() {
  B = "foo bar";
  A();
}
```

When calling a function, a block is pushed to the stack with the return
position, so the function will return to where it has been called from.
The return operator is only necessary when a return is specifically
required. A function returns at the matching closing bracket `]`;
therefore, the example above can as well be written: `A@:[B&] B'foo bar' A@`.

A return can be required for example inside a loop:

```
A@:[                      /* function declaration */
  B'C is &C, D is &D\'    /* initialising variables */
  C2
  [
    D5 [D!!? B& D-- @<]   /* for(D=5; D; D--) output(B) */
    [C--! ? @^]           /* if(!--C) return */
    @<                    /* loop */
  ]
]

A@
```

Since Q doesn't have local variables, functions cannot return values,
and cannot be sent arguments. Instead, use the 22 global variables with
care.

## Expressions

All logical operations use AND by default, e.g. `AB= CD>` evaluates
to `(B==A && D>C)`. To use OR in an expression, place within round
brackets, e.g. `(AB= CD>)` instead evaluates to `(B==A || D>C)`.
Nested expressions swap between OR and AND in evaluation, starting with OR;
e.g. `((AB= CD>) E! (FG< HI=))` evaluates to `((B==A && D>C) || !E || (G<F && I==H))`.

All expressions evaluate from left to right in appearing order. Other
operations are performed normally between expressions, and expressions
work on a per block basis, i.e. when leaving a block, any evaluated
logical operations are discarded.

## Comments

Comments in Q use the common multi-line comment operators used in
C and many other languages `/* ... */`. Nested comments are permitted.

## Programming practices

The limitations of the language, makes it important to use variables
in a clear and consecutive manner, e.g. some variables may span the
entire script, while others may be volatile. Make sure to comment
how variables should be used for readability. Q is however designed
to be easily obfuscated, with its minimalism.

## Examples

The fibonacci sequence:

```
A 'Fibonacci &C:^&D\'
B20 C0 D0 E1 F+
[
  BC++ <= ?
  A&
  ED: FE: DF+:
  @<
]
```

Some math operations:

```
A '&B&E&C = &D (&F)\'
B 123.
C 45.
BC D+  E' +  '     F'add'           A&
BC D-  E' -  '     F'sub'           A&
BC D*  E' *  '     F'mul'           A&
BC D/  E' /  '     F'div'           A&
BC D/ #:           F'div int'       A&
BC D/ D%%          F'div round'     A&

B 256
C 4
BC D<< E' << '     F'lshift'        A&
BC D>> E' >> '     F'rshift'        A&
BC D&& E' &&  '    F'bitwise and'   A&
BC D|| E' |  '     F'bitwise or'    A&
BC D^^ E' &^  '    F'xor'           A&

A '&B&E&* = &D (&F)\'
B 157486989
E ' [%10] '
F 'reduce'
B D#%  0                            A&
B D#% 32                            A&
```

## Not yet implemented

There are a few functions not yet implemented; the most urgent is
arrays.

### Arrays

The syntax of arrays isn't entirely worked out, but will
probably work similar to arrays in PHP: elements are linked and in a
hashtable.

At the current state of planning; declaring an array will be done by adding
many constant values separated by space, e.g. `A 1 2 3 'foo' 'bar'`
generates an array with five elements, with integer keys, JSON would look
like: `A = [1, 2, 3, "foo", "bar"]`. Arbitrary integer or string keys
would be created with the hash sign `#`, e.g. `A 'foo'#'bar' 2#3`
which in JSON would look like: `A = { "foo": "bar", 2: 3 }`.

Retrieving a value in the array is done with the caret sign `^`,
e.g. `A^'foo'&` would output "bar", if the array in the previous
example were used. Indexing a value in an array would store that
value in **V0**.

