Quite a Shell (Quash)
=====================

For EECS 678.

How to use Quash
----------------

In your favorite shell, navigate to the quash folder and enter `make`. After compilation finishes, Quash can be invoked by typing `./quash`.

###### Some handy commands and features
* Execute commands by typing them at Quash's user prompt (e.g., `ls -lAh`)
* Set HOME directory by typing `home <directory>`. Display the current HOME variable with the `home` command.
* Set PATH directories by typing `path <directory1>:<directory2>...`. Display the current PATH with the `path` command.
* Create a pipe between two commands like this: `command1 | command2`.
* Redirect output of a command (or a pipe) by typing `command1 > file`.
* Redirect input to a command or pipe by typing `< file command` *Must be typed at the beginning of a command.*
* Execute a command in the background by adding `&` to the very end of the command.
* Quash can perform a batch of jobs saved in a file. Invoke Quash from a shell with `./quash < commands.txt`.
* Quit Quash with `quit`, `exit`, or `q` (if you must).

Version History
---------------

* 0.1 Added quit functionality, stored command strings into vectors, and allowed setting HOME and PATH variables.
* 0.2 Added cd functionality.
* 0.3 Added basic program execution functionality.
* 0.4 Got program execution to function properly, with addition of `vec.pushback(NULL)` to null-terminate the argument variables. Also removed unnecessary environment variables, and changed execution from `execvpe` to `execvp`.
* 0.5 I/O redirection functionality provided via the '<' and '>' characters. See bug/issue 1 below.
* 0.6 Added pipe functionality, allowing two programs to be piped together with the use of the '|' character.
* 0.7 Added background processing functionality and the ability to send Quash a batch of commands by invoking '$ quash < commands.txt'.


Bugs / Issues
-------------

##### 1) Redirection Functionality Problems
When using I/O redirection with the > and < characters, Quash can be picky about where redirection files may be specified within the command. Input redirection seems to work only when < is provided at the beginning of the command. File names for redirection _must_ immediately follow the redirection characters < and > .
##### 2) File permissions on redirected output
When redirecting output to a file, Quash would originally write the file so that the user would only have read permissions on the file. I have specified that Quash should create files that the user can read and write, but not execute. This could potentially cause problems if, for example, the user was compiling executable programs from the Quash interface.
