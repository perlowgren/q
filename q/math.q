#!/usr/local/bin/q
?>Showing some examples of math:
<?
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
