Quite a Shell
=============

For EECS 678.

Version History
---------------

* 0.1 Added quit functionality, stored command strings into vectors, and allowed setting HOME and PATH variables.
* 0.2 Added cd functionality.
* 0.3 Added basic program execution functionality.
* 0.4 Got program execution to function properly, with addition of `vec.pushback(NULL)` to null-terminate the argument variables. Also removed unnecessary environment variables, and changed execution from `execvpe` to `execvp`.
* 0.5 I/O redirection functionality provided via the '<' and '>' characters. See bug/issue 1 below.
* 0.6 Added pipe functionality, allowing two programs to be piped together with the use of the '|' character.


Bugs / Issues
-------------

##### 1) Redirection Functionality Problems
When using I/O redirection with the > and < characters, Quash can be picky about where redirection files may be specified within the command. Input redirection seems to work only when < is provided at the beginning of the command. File names for redirection _must_ immediately follow the redirection characters < and > .
##### 2) File permissions on redirected output
When redirecting output to a file, Quash would originally write the file so that the user would only have read permissions on the file. I have specified that Quash should create files that the user can read and write, but not execute. This could potentially cause problems if, for example, the user was compiling executable programs from the Quash interface.
