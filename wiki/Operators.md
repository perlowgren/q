# Operators

In the descriptions of operators below, there appear a formula
similar to this `V2 [V1] <V0> +`. The variable slots **V0**, **V1**
and **V2** are listed if they are used by the operator; all
operators must comply with its defined formula - for all types.

An **x** represents the expression result, which is stored
in the block in which the expression is evaluated.

When **V0** or **V1** appear in angle brackets, it means that the result
from the operation will be placed in that variable. The variable **x**
can also appear in angle brackets, which means the result is calculated as
an expression, and stored in the expressions variable **x**.

When **V0** or **V1** is named in square brackets, it means that the
operator will accept a constant. All constants must be written directly
after the operator, but there may be space between. If the constant is
placed in **V1**, the previous **V1** is temporarily pushed to **V2**,
e.g. `V2 [V1] <V0> +` means that a constant after `+` is positioned in
**V1** and previous **V1** is pushed to **V2**: `ABC +3 +` here **C** is
in **V0**, **B** is in **V1** and **A** is in **V2**; then the first
plus push **B** to **V2** and temporarily put the constant '3' in
**V1**, then the second plus return to previously named variables -
adding **A** (**V2**) and **B** (**V1**), and put the result in **C**
(**V0**), and would in C look like `C = B+3, C = A+B;`.

## Single Operators

* [Addition](#markdown-header-plus):  
  `V2 [V1] <V0> +`
* [Subtraction](#markdown-header-hyphen):  
  `V2 [V1] <V0> -`
* [Multiplication](#markdown-header-asterisk):  
  `V2 [V1] <V0> *`
* [Division](#markdown-header-slash):  
  `V2 [V1] <V0> /`
* [Modulus](#markdown-header-percent):  
  `V2 [V1] <V0> %`
* [Hash](#markdown-header-hash):  
  `#`
* [Output](#markdown-header-ampersand):  
  `V0 &`
* [Set](#markdown-header-colon):  
  `V1 <V0> :`
* [If](#markdown-header-question-mark):  
  `x ?`
* [Equals](#markdown-header-equals-sign):  
  `[V1] V0 <x> =`
* [Logical Not](#markdown-header-exclamation-mark):  
  `V0 <x> !`
* [Less than](#markdown-header-left-angle-bracket):  
  `[V1] V0 <x> <`
* [Greater than](#markdown-header-right-angle-bracket):  
  `[V1] V0 <x> >`
* [Goto](#markdown-header-at-sign):  
  `[V0] @`
* [Index](#markdown-header-caret):  
  `[V1] V0 ^`
* [Bitwise NOT](#markdown-header-tilde):  
  `[V1] <V0> ~`
* [Else](#markdown-header-pipe):  
  `|`
* [Left expression bracket](#markdown-header-left-round-bracket):  
  `<x> (`
* [Right expression bracket](#markdown-header-right-round-bracket):  
  `<x> )`
* [Left block bracket](#markdown-header-left-square-bracket):  
  `<x> [`
* [Right block bracket](#markdown-header-right-square-bracket):  
  `<x> ]`
* [String constant](#markdown-header-apostrophe):  
  `<V0> '...'`

---

#### Plus

`V2 [V1] <V0> +`

1. (int) addition
2. (float) addition
3. (str) concatenation, join strings

C: `V0 = V2 + V1;`

Example: `A5 B2 C+` (result: **C** is 7)

---

#### Hyphen

`V2 [V1] <V0> -`

1. (int) subtraction
2. (float) subtraction

C: `V0 = V2 - V1;`

Example: `A5 B2 C-` (result: **C** is 3)

---

#### Asterisk

`V2 [V1] <V0> *`

1. (int) multiplication
2. (float) multiplication

C: `V0 = V2 * V1;`

Example: `A5 B2 C*` (result: **C** is 10)

---

#### Slash

`V2 [V1] <V0> /`

1. (int) division
2. (float) division

C: `V0 = V2 / V1;`

Example: `A5 B2 C+` (result: **C** is 2)

---

#### Percent

`V2 [V1] <V0> %`

1. (int) modulus
2. (float) modulus, C function fmod

C: `V0 = V2 % V1;`

Example: `A5 B2 C%` (result: **C** is 1)

---

#### Hash

`#`

Not yet implemented, only used in combined operators

---

#### Ampersand

`V0 &`

Output **V0**

PHP: `echo("$V0");`

Example: `A&` (outputs contents of **A**)

---

#### Colon

`V1 <V0> :`

Set **V0** to **V1**

C: `V0 = V1;`

Example: `AB:` (**B** is set to **A**)

---

#### Question mark

`x ?`

Checks the block's expression value **x**

1. if TRUE move forward
2. if FALSE goto next *else* or *end of block*, or **EOF**

**x** is by default TRUE, if no logical expression before `?`

C: `if(x) ...`

Example: `[AB=? C&]` (if **B** equals **A** then output **C**)

---

#### Equals sign

`[V1] V0 <x> =`

Equals; comparison is done with mixed types so that:

1. (int, float) `5 == 5.0`
2. (int, float) `5 != 5.5`
3. (int, str) `5 == "5"`
4. (int, str) `5 != "A"`
5. (float, str) `2.5 == "2.5"`
6. (float, str) `2.5 != "A"`

C: `x = (V0 == V1);`

Example: `AB=` (test if **B** equals **A**)

---

#### Exclamation mark

`V0 <x> !`

Set **x** to TRUE if **V0** is empty: VOID, 0, 0.0, "", or "0"

C: `x = !V0;`

Example: `A!` (**x** is TRUE if **A** is empty, i.e. VOID, 0, 0.0, "", or "0")

---

#### Left angle bracket

`[V1] V0 <x> <`

Less than (see `=` for mixed type comparison)

C: `x = (V0 < V1);`

Example: `AB<` (**x** is TRUE if **B** is less than **A**)

---

#### Right angle bracket

`[V1] V0 <x> >`

Greater than (see `=` for mixed type comparison)

C: `x = (V0 > V1);`

Example: `AB>` (**x** is FALSE if **B** is greater than **A**)

---

#### At sign

`[V0] @`

1. (int) goto; position at **V0**, and continue
   at next char; if out of bounds, nothing happens

C: `goto V0;`

Example: `A@` (goto position in script **A** is pointing to)

---

#### Caret

`[V1] V0 ^`

1. (arr) index operator for array (not yet implemented)

C: `V0[V1];`

---

#### Tilde

`[V1] <V0> ~`

1. (int) bitwise NOT

C: `V0 = ~V1;`

Example: `A1 B~` (**B** is set to NOT **A**, and since integers are signed, result is -2)

---

#### Pipe

`|`

Else; delimiter within blocks, failed expressions goto
next `|` until end of block

C: `if... else...`

Example: `[A! ? B& | C&]` (if **A** is empty output **B**, else output **C**)

---

#### Left round bracket

`<x> (`

Left expression bracket; expressions return a boolean
value, which is stored in the block's expressions value (**x**)

Example: `(AB=CD<)` (**x** is set to TRUE if **B** equals **A** **OR** **D** is less than **C**)

---

#### Right round bracket

`<x> )`

Right expression bracket

---

#### Left square bracket

`<x> [`

Left block bracket

C: `{ ...`

Example: `A0B10 [?A+:B--@<] A&` (calculate and print sum of 10-1, result: 55)

---

#### Right square bracket

`<x> ]`

Right block bracket

C: `... }`

---

#### Apostrophe

`<V0> '...'`

String constant

C: `V0 = "Abc";`

Example: `A 'Hello, world!` (Set **A** to the string "Hello, World!")

---

## Combined Operators

* [Integer cast / Reduce](#markdown-header-hash-hash):  
  `V1 <V0> ##`
* [Float cast](#markdown-header-percent-percent):  
  `V1 <V0> %%`
* [Addition 2](#markdown-header-plus-colon):  
  `[V1] <V0> +:`
* [Subtraction 2](#markdown-header-hyphen-colon):  
  `[V1] <V0> -:`
* [Multiplication 2](#markdown-header-asterisk-colon):  
  `[V1] <V0> *:`
* [Division 2](#markdown-header-slash-colon):  
  `[V1] <V0> /:`
* [Modulus 2](#markdown-header-percent-colon):  
  `[V1] <V0> %:`
* [Increase by one](#markdown-header-plus-plus):  
  `<V0> ++`
* [Decrease by one](#markdown-header-hyphen-hyphen):  
  `<V0> --`
* [Power](#markdown-header-asterisk-asterisk):  
  `V2 [V1] <V0> **`
* [Square root](#markdown-header-slash-slash):  
  `[V1] <V0> //`
* [Binary left shift](#markdown-header-left-angle-bracket-left-angle-bracket):  
  `V2 [V1] <V0> <<`
* [Binary right shift](#markdown-header-right-angle-bracket-right-angle-bracket):  
  `V2 [V1] <V0> >>`
* [Binary AND](#markdown-header-ampersand-ampersand):  
  `V2 [V1] <V0> &&`
* [Binary OR](#markdown-header-pipe-pipe):  
  `V2 [V1] <V0> ||`
* [Binary XOR](#markdown-header-caret-caret):  
  `V2 [V1] <V0> ^^`
* [Binary left shift 2](#markdown-header-left-angle-bracket-colon):  
  `[V1] <V0> <:`
* [Binary right shift 2](#markdown-header-right-angle-bracket-colon):  
  `[V1] <V0> >:`
* [Binary AND 2](#markdown-header-ampersand-colon):  
  `[V1] <V0> &:`
* [Binary OR 2](#markdown-header-pipe-colon):  
  `[V1] <V0> |:`
* [Binary XOR 2](#markdown-header-caret-colon):  
  `[V1] <V0> ^:`
* [Binary NOT 2](#markdown-header-exclamation-mark-colon):  
  `<V0> ~:`
* [Absolute](#markdown-header-hyphen-plus):  
  `[V1] <V0> -+`
* [Negative](#markdown-header-plus-hyphen):  
  `[V1] <V0> +-`
* [Floor](#markdown-header-percent-hyphen):  
  `[V1] <V0> %-`
* [Ceil](#markdown-header-percent-plus):  
  `[V1] <V0> %+`
* [Integer cast 2 / Reduce 2](#markdown-header-hash-colon):  
  `<V0> #:`
* [Reduce 3](#markdown-header-hash-percent):  
  `V2 [V1] <V0> #%`
* [If not](#markdown-header-exclamation-mark-question-mark):  
  `x !?`
* [Is not empty](#markdown-header-exclamation-mark-exclamation-mark):  
  `V0 <x> !!`
* [Not equals](#markdown-header-exclamation-mark-equals-sign):  
  `[V1] V0 <x> !=`
* [Less than or equals](#markdown-header-left-angle-bracket-equals-sign):  
  `[V1] V0 <x> <=`
* [Greater than or equals](#markdown-header-right-angle-bracket-equals-sign):  
  `[V1] V0 <x> >=`
* [Ternary](#markdown-header-question-mark-question-mark):  
  `[V2] V1 <V0> ??`
* [Ternary 2](#markdown-header-question-mark-colon):  
  `[V1] <V0> ?:`
* [Input](#markdown-header-ampersand-left-angle-bracket):  
  `<V0> &<`
* [Direct output](#markdown-header-question-mark-right-angle-bracket):  
  `?>`
* [Direct output end](#markdown-header-left-angle-bracket-question-mark):  
  `<?`
* [Unformatted string](#markdown-header-ampersand-right-angle-bracket):  
  `&>`
* [Unformatted string end](#markdown-header-left-angle-bracket-ampersand):  
  `<&`
* [Set position / Function](#markdown-header-at-sign-colon):  
  `<V0> @:`
* [Loop / Continue](#markdown-header-at-sign-left-angle-bracket):  
  `@<`
* [Return](#markdown-header-at-sign-caret):  
  `@^`
* [Include](#markdown-header-at-sign-hash):  
  `[V0] @#`
* [Execute](#markdown-header-at-sign-ampersand):  
  `[V0] @&`
* [Comment](#markdown-header-slash-asterisk):  
  `/*`
* [Comment end](#markdown-header-asterisk-slash):  
  `*/`

---

#### Hash-Hash

`V1 <V0> ##`

For integer values, the simpler version of reduction is used, with the default
maximum number 10, e.g. 12345 = 1+2+3+4+5 = 15 = 1+5 = 6. For reduction to a
specific maximum number, use the `#%` operator; see [reduce 3](#markdown-header-hash-percent).

1. (int) reduce to 0-10
2. (float) integer cast
3. (str) if number cast, else sum of letters

C: `V0 = (long)V1;`

Example: `A 123456789 B## C5.5 D## E '4' F## G 'Foo' H##` (result: **B** is 9, **D** is 5, **F** is 4, **H** is 312)

---

#### Percent-Percent

`V1 <V0> %%`

1. (int) float cast
2. (float) round
3. (str) float cast

C: `V0 = (double)V1;`

Example: `A5 B%% C5.5 D%% E '5.5' F%% G 'Foo' H%%` (result: **B** is 5.0, **D** is 6.0, **F** is 5.5, **H** is 0.0)

---

#### Plus-Colon

`V1 <V0> +:`

1. (int) addition
2. (float) addition
3. (str) concatenation, join strings

C: `V0 += V1;`

Example: `A3 B5 +:` (result: **B** is 8)

---

#### Hyphen-Colon

`V1 <V0> -:`

1. (int) subtraction
2. (float) subtraction

C: `V0 -= V1;`

Example: `A3 B5 -:` (result: **B** is 2)

---

#### Asterisk-Colon

`V1 <V0> *:`

1. (int) multiplication
2. (float) multiplication

C: `V0 *= V1;`

Example: `A3 B5 *:` (result: **B** is 15)

---

#### Slash-Colon

`V1 <V0> /:`

1. (int) division
2. (float) division

C: `V0 /= V1;`

Example: `A3 B5 /:` (result: **B** is 1)

---

#### Percent-Colon

`V1 <V0> %:`

1. (int) modulus
2. (float) modulus, C function fmod

C: `V0 %= V1;`

Example: `A3 B5 %:` (result: **B** is 2)

---

#### Plus-Plus

`<V0> ++`

1. (int) increase by one
2. (float) increase by one

C: `V0++;`

Example: `A5 ++` (result: **A** is 6)

---

#### Hyphen-Hyphen

`<V0> --`

1. (int) decrease by one
2. (float) decrease by one

C: `V0--;`

Example: `A5 --` (result: **A** is 4)

---

#### Asterisk-Asterisk

`V2 [V1] <V0> **`

1. (int) power
2. (float) power

C: `V0 = pow(V2, V1);`

Example: `A3 B2 C**` (result: **C** is 9)

---

#### Slash-Slash

`[V1] <V0> //`

1. (int) square root
2. (float) square root

C: `V0 = sqrt(V1);`

Example: `A9 B//` (result: **B** is 3)

---

#### Left angle bracket-Left angle bracket

`V2 [V1] <V0> <<`

1. (int) binary left shift

C: `V0 = V2 << V1;`

Example: `A1 B10 C<<` (result: **C** is 1024)

---

#### Right angle bracket-Right angle bracket

`V2 [V1] <V0> >>`

1. (int) binary right shift

C: `V0 = V2 >> V1;`

Example: `A1024 B10 C>>` (result: **C** is 1)

---

#### Ampersand-Ampersand

`V2 [V1] <V0> &&`

1. (int) binary AND

C: `V0 = V2 & V1;`

Example: `A3 B5 C&&` (result: **C** is 1)

---

#### Pipe-Pipe

`V2 [V1] <V0> ||`

1. (int) binary OR

C: `V0 = V2 | V1;`

Example: `A3 B5 C||` (result: **C** is 7)

---

#### Caret-Caret

`V2 [V1] <V0> ^^`

1. (int) binary XOR

C: `V0 = V2 ^ V1;`

Example: `A3 B5 C^^` (result: **C** is 6)

---

#### Left angle bracket-Colon

`[V1] <V0> <:`

1. (int) binary left shift

C: `V0 <<= V1;`

Example: `A1 <: 10` (result: **A** is 1024)

---

#### Right angle bracket-Colon

`[V1] <V0> >:`

1. (int) binary right shift

C: `V0 >>= V1;`

Example: `A1024 >: 10` (result: **A** is 1)

---

#### Ampersand-Colon

`[V1] <V0> &:`

1. (int) binary AND; **DO NOT** confuse with string operator `&:`, used inside of strings

C: `V0 &= V1;`

Example: `A3 B5 &:` (result: **B** is 1)

---

#### Pipe-Colon

`[V1] <V0> |:`

1. (int) binary OR

C: `V0 |= V1;`

Example: `A3 B5 |:` (result: **B** is 7)

---

#### Caret-Colon

`[V1] <V0> ^:`

1. (int) binary XOR

C: `V0 ^= V1;`

Example: `A3 B5 ^:` (result: **B** is 6)

---

#### Tilde-Colon

`<V0> ~:`

1. (int) binary NOT, flip all bits

C: `V0 = ~V0;`

Example: `A1 ~:` (result: **A** is -2)

---

#### Hyphen-Plus

`[V1] <V0> -+`

1. (int) make a number positive
2. (float) make a number positive

C: `V0 = abs(V1);`

Example: `A3 B5 C- D-+` (result: **D** is 5)

---

#### Plus-Hyphen

`[V1] <V0> +-`

1. (int) make a number negative
2. (float) make a number negative

C: `V0 = -abs(V1);`

Example: `A5 B+-` (result: **B** is -5)

---

#### Percent-Hyphen

`[V1] <V0> %-`

1. (float) floor

C: `V0 = floor(V1);`

Example: `A5.5 %-` (result: **A** is 5.0)

---

#### Percent-Plus

`[V1] <V0> %+`

1. (float) ceil

C: `V0 = ceil(V1);`

Example: `A5.5 %+` (result: **A** is 6.0)

---

#### Hash-Colon

`<V0> #:`

Same as `##` except applied on **V0** instead of **V1**; see [integer cast](#markdown-header-hash-hash)

C: `V0 = (long)V0;`

Example: `A123456789 #:` (result: **A** is 9)

---

#### Hash-Percent

`V2 [V1] <V0> #%`

1. (int) reduce to sum of **V2** digits 1-**V1**
2. (str) reduce sum of letters

C: `V0 = red(V2,V1);`

Example: `A35566778 B #%12` (result: **B** is 3+5+5+6+6+7+7+8 = 47 = 4+7 = 11, maximum is 12 - so 11 is not reduced to 2)

---

#### Exclamation mark-Question mark

`x !?`

Negative version of `?`; see [if](#markdown-header-question-mark)

C: `if(!x) ...`

Example: `A5 B2 [AB= !? ?>foo<? | ?>bar<? ]` (result: output "foo", since **x** is FALSE)

---

#### Exclamation mark-Exclamation mark

`V0 !!`

Set **x** to TRUE if **V0** is not empty, i.e. 1, 1.0, '1', 'foo'.

C: `x = !!V0;`

Example: `A5 [A!! ? ?>foo<? | ?>bar<? ]` (result: output "foo", since **A** is not empty)

---

#### Exclamation mark-Equals sign

`[V1] V0 !=`

Set **x** to TRUE is **V0** not equals **V1**

C: `x = (V0 != V1);`

Example: `A5 [A!=3? ?>foo<? | ?>bar<? ]` (result: output "foo", since **A** not equals 3)

---

#### Left angle bracket-Equals sign

`[V1] V0 <=`

Set **x** to TRUE is **V0** is less than or equal to **V1**

C: `x = (V0 <= V1);`

Example: `A5 [A<=3? ?>foo<? | ?>bar<? ]` (result: output "bar" since **A** is not less than or equal to 3)

---

#### Right angle bracket-Equals sign

`[V1] V0 >=`

Set **x** to TRUE is **V0** is greater than or equal to **V1**

C: `x = (V0 >= V1);`

Example: `A5 [A>=3? ?>foo<? | ?>bar<? ]` (result: output "foo", since **A** is greater than or equal to 3)

---

#### question mark-Question mark

`[V2] V1 <V0> ??`

Ternary operator; assigns **V0** value of **V1** if not empty; else value of **V2**

C: `V0 = V1? V1 : V2;`

Example: `A'foo' B ?? 'bar'` (result: **B** is "foo", since **A** is not empty)

---

#### Question mark-Colon

`[V1] <V0> ?:`

So called "Elvis operator"; keep value of **V0** if not empty; else set to value of **V1**

C: `V0 = V0? V0 : V1;`

Example: `A'foo' ?: 'bar'` (result: **A** is "foo", since **A** is not empty)

---

#### Ampersand-Left angle bracket

`<V0> &<`

Read string from in-stream to **V0**

C: `V0 = fgets(stdin);`

Example: `A &<` (result: **A** receive input)

---

#### Question mark-Right angle bracket

`?>`

Direct output to out-stream; anything after until closing operator `<?`,
or **EOF** will be printed. It is not considered a string, and no string
is generated. *Cannot be nested!*

PHP: `?>Foo bar<?php`

Example: `?>Foo bar<?` (result: output "Foo bar")

---

#### Left angle bracket-Question mark

`<?`

End of direct output; see [direct output](#markdown-header-question-mark-right-angle-bracket)

---

#### Ampersand-Right angle bracket

`<V0> &>`

Unformatted string; the string is generated without processing any
string operators and assigned to **V0**. *Can be nested!*

PHP: `V0 = <<<'END' ... 'END';`

Example: `A &>Foo && &>bar<& &:A<&` (result: **A** is "Foo && &>bar<& &:A")

---

#### Left angle bracket-Ampersand

`<V0> <&`

End of unformatted string; see [unformatted string](#markdown-header-ampersand-right-angle-bracket)

---

#### At sign-Colon

`<V0> @:`

Set **V0** to the position in the script at `:` in `@:`, so that if goto is called on the variable,
it will continue at the next char. A block following directly after is regarded a function (white
space between is permitted), then **V0** is set to the position of the `[`, and the script jumps
to the matching `]`.

Note that the position stored in **V0** is an ordinary integer, and can be manipulated as such.

Example: `A@:[B&] B'Foo'A@ B' bar'A@` (result: output "Foo bar", first call function **A** where **B** is "Foo", then again where **B** is " bar")

---

#### At sign-Left angle bracket

`@<`

Return to start of block; or if not in block, start of script. This operators is
used to make loops.

C: `continue;`

Example: `A0 B'&A, ' C'&A' [A++<5? B& @<| C&]` (result: output "1, 2, 3, 4, 5")

---

#### At sign-Caret

`@^`

Return to the position calling last goto; or if not called: goto end of block, or
**EOF**.

C: `return;`

Example: `A@:[B=0? @^| C&B-- @<] B5 C'&B, ' A@ ?>end` (result: output "5, 4, 3, 2, 1, end")

---

#### At sign-Hash

`[V0] @#`

If **V0** is a string containing a file name of an existing file: it is read and executed; else ignored

PHP: `include(V0);`

Example: `@# 'script.q'` (result: execute code in "script.q", if file exists)

---

#### At sign-Ampersand

`[V0] @&`

If **V0** is a string: it's executed as a script; else ignored

PHP: `exec(V0);`

Example: `@& 'A2B3+:&'` (result: execute string constant, which output "5")

---

#### Slash-Asterisk

`/*`

Comment opening; anything after is ignored until a closing comment operator
`*/` or **EOF**. Comments can be nested.

C: `/* ...`

Example: `A'Foo' /* Q-comments can be /* nested, */ 'but must match' */ &` (result: output "Foo", nested comments ignored)

---

#### Asterisk-Slash

`*/`

Comment closing

