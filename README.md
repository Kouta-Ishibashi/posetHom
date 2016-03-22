# posetHom

## #0 Overview of this software.
This software posetHom compute the poset Z_2 homology and the whitney classes if the poset satisfies "Euler condition". 

The posetHom computes homology and Whitney classe using a poset data in text file. The poset data has to be written with the following rules:

1. The relation "A < B and there is no point between A and B" is written as {A,B}  one per line. 
2. When the point P is isolated, we use {P,P} as the exception. 
3. We use a integer to represent a point.
4. If a line is started with a semicolon (;) , the line is ignored as a comment.
5. An empty line must not be included.
For example
```
;this poset represents circumference S1
{1,3}
{2,3}
{1,4}
{2,4}
```
The simpleHom computes homology and Whitney classe using a maximal simplex data in text file. The data has to be written with the following rules:

1. The maximal simplex which is constructed with points A, B, C, and D is written as {A,B,C,D} one per line.
2. We use a integer to represent a point.
3. If a line is started with a semicolon (;) , the line is ignored as a comment.

For example
```
;this represents sphere S2
{1,2,3}
{1,2,6}
{1,5,3}
{1,5,6}
{4,2,3}
{4,2,6}
{4,5,3}
{4,5,6}
```



## #1 How to compile.
### Mac/Linux
All you have to do is to command "make" in top directory of this project. It is recommended to make a symbolic link to a path-opened directory such as /usr/bin .

### Win
We strongly recommend to use some linux virtual environment such as Cygwin or msys2 and to command "make" .

