
?>
This is a Q-script testing blocks...

Testing function...
<?

Q@:
[
	A 0
	B 'This is a loop in a function [&A]\'
	[
		A++ <=5 ?
		B&
		@<
	]
	B 'This is in a function\'&
	[
		A=0?
		B 'This is in an if-block\'&
		@^
	]
	[
		A ! ?
		B 'This is in an if-block\'&
		|
		B 'This is in an if-else-block\'&
	]
]

Q@

?>
Testing calling function that include another script...
<?

M@:
[
	@#'math.q'
]

M@


?>
Testing comments (nothing should happen)...
<?

/* This is a comment
 * A ~String inside comment!\n~&
 */

?>
Testing expressions...
<?
[
	[
		A 1 B 0 C 0 D 1 E 1
		((A!! B!!) C!! (D!! E!!))
		?
		A 'TRUE\'&
		|
		A 'FALSE\'&
	]
]

?>
Testing executing script in string...
<?

A &>A 'Fibonacci &C:^&D^sqrt(&G)\'
B20 C0 D0 E1 F+
[
	BC++ <= ?
	DG: %% G//
	A&
	ED: FE: DEF+
	@<
]
<&

A@&

?>
Finished

<?
