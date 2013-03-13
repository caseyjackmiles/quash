/************************
 *
 * Casey Miles
 * EECS 678
 * Project 1 - Quash Shell
 * Wed., March 13, 2013
 *
 * main.cpp
 * contains main interactive shell loop
 * and functions to parse user commands
 * that do NOT require exec().
 *
************************/
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <vector>
#include <signal.h>

//included for IO redirection
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std; 

vector <char *> splitUserInput(string input);
void parseUserInput(vector <char *> vec);
void setPathHome(vector <char *> vec);
void setWorkingDir(vector <char *> vec);
void displayJobs();
void parseCommand(vector <char *> vec);
int executeCommand(vector <char *> cmd1, vector <char *> cmd2, string fdin, string fdout);
int executeBackgroundCommand(vector <char *> cmd1, vector <char *> cmd2, string fdin, string fdout);
void readInputFromSTDIN();

#include "command.cpp"


int main (const int argc, char *argv[], char *envp[]){



	//Declare some variables for processing commands
	string userInput = "";
	vector <char *> cmdVec;
	pid_t returnedPid;			//used for checking on background processes
	int status;					//used for checking on background processes

//*************************************************
//  Print initial environment variables
//*************************************************
	printf("Quite a Shell, version 0.7:\n");
	char *envVar;

	cout << "PATH: " << getenv("PATH") << endl;
	cout << "HOME: " << getenv("HOME") << endl;
	cout << "CUR_DIR: " << getcwd(NULL, 0) << endl << endl;
//*************************************************

	
	//Check if Quash is being accessed from a terminal (interactively)
	//or from redirected stdin input
	if(isatty(fileno(stdin))){
		//terminal; do nothing
	}
	else {
		cout << "EXECUTING COMMANDS" << endl;
		readInputFromSTDIN();
	}


	//Check if the user has typed one of the exit words
	while ((userInput != "exit") && (userInput != "quit") && (userInput != "q")){


		//Every time before we print the prompt, check if a
		//background process has terminated. If so, print
		//the PID of the terminated process.
		returnedPid = waitpid(-1, &status, WNOHANG);
		while (returnedPid > 0){
			printf("PID [%d] finished execution\n", returnedPid);
			returnedPid = waitpid(-1, &status, WNOHANG);
		}

		//Take the user-input string and split it
		//into a vector of char *
		cmdVec = splitUserInput(userInput);

		//Read the contents of the command vector
		//And perform the appropriate actions
		parseUserInput(cmdVec);

		//Print a prompt to our user
		cout << "\033[1;31m" << getcwd(NULL, 0) << ">\033[0m";

		//Empty the vector from the
		//last time it was used
		cmdVec.clear();

		//Get a command from the user
		//And place it in userInput
		getline(cin, userInput);
	}

	//The user has entered an exit keyword
	cout << "Exiting Quash." << endl;
	return 0;

}

/*
 * Takes a string and splits it into
 * word tokens, each of which gets
 * placed into a vector.
 */
vector <char *> splitUserInput(string i){

	vector <char *> cmdVec;
	char *input = (char *)i.c_str();

	//Grab first word
	char *tokens = strtok(input, " ");
	//Grab remaining words
	while (tokens != NULL){
		cmdVec.push_back(tokens);
		tokens = strtok(NULL, " ");
	}

	//Return the completed command vector
	return cmdVec;
}

/*
 * Searches the command vector for keywords,
 * such as 'cd', 'home', 'jobs', and calls
 * functions accordingly.
 *
 * If command doesn't match any of these keywords
 * it is processed as a general command and sent
 * to parseCommand().
 */
void parseUserInput(vector <char *> vec){

	//Can't parse an empty command vector
	if(vec.size() == 0){
		return;
	}

	//Check for 'path' or 'home' commands
	if ((strcmp(vec[0], "path") == 0) || (strcmp(vec[0], "home") == 0)){
		setPathHome(vec);
		return;
	}

	//Check for 'cd' command
	if (strcmp(vec[0], "cd") == 0){
		setWorkingDir(vec);
		return;
	}

	//Check for 'jobs' command
	if (strcmp(vec[0], "jobs") == 0){
		displayJobs();
		return;
	}

	//Parse the command as a general command
	parseCommand(vec);

	return;
}

/*
 * Sets or displays the "HOME" or "PATH" environment variables.
 *
 * "PATH" is where Quash searches for executable files.
 * "HOME" is where the user is taken if he issues the
 * "cd" command with no arguments.
 */
void setPathHome(vector <char *> vec){
	
	//Display requested environment variable
	if(vec.size() == 1){
		switch (strcmp(vec[0], "path")){
			case 0:
				cout << "PATH=" << getenv("PATH") << endl;
				break;
			default:
				cout << "HOME=" << getenv("HOME") << endl;
				break;
		}
		return;
	}
				
	//Can't process more than one argument
	if(vec.size() > 2){
		cout << "ERROR: " << vec[0] << " only takes one argument." << endl;
		return;
	}

	//Set "PATH" variable
	if ((strcmp(vec[0], "path")) == 0){
		if ((setenv("PATH", vec[1], 1)) == -1) {
			fprintf(stderr, "Error occurred setting PATH. Error #%d\n", errno);
			return;
		}
		cout << "Path set to: " << getenv("PATH") << endl;
	}

	//Set "HOME" variable
	if((strcmp(vec[0], "home")) == 0){
		if ((setenv("HOME", vec[1], 1)) == -1){
			fprintf(stderr, "Error occurred setting HOME. Error #%d\n", errno);
			return;
		}
		cout << "Home set to: " << getenv("HOME") << endl;
	}
}

/*
 * Changes the working directory to a file
 * specified by the user when using the 
 * "cd" command.
 *
 * If no argument is passed to "cd", then
 * the function changes the working directory
 * to the directory specified by "HOME"
 * environment variable.
 */
void setWorkingDir(vector <char *> vec){

	//Can't process more than one argument to cd
	if(vec.size() > 2){
		cout << "ERROR: " << vec[0] << " only takes one argument." << endl;
		return;
	}

	//cd with no arguments; switch to HOME directory
	if(vec.size() == 1){
		vec.push_back(getenv("HOME"));

		if ((chdir(vec[1])) == -1){
			fprintf(stderr, "Error occurred on cd to HOME. Error #%d\n", errno);
			return;
		}
		return;
	}

	//Change working directory to directory specified
	//in vec[1]
	if((chdir(vec[1])) == -1){
		fprintf(stderr, "Error occurred on cd to %s. Error #%d\n", vec[1], errno);
		return;
	}
	return;
}

/*
 * Should display a list of any currently running background
 * processes. Was not able to implement.
 */
void displayJobs(){
	cout << "'Jobs' feature not implemented." << endl;
}

/*
 * If std_in has been redirected to a file (by calling quash < file),
 * then read commands from the file, execute them, and exit Quash.
 *
 * This is essentially a copy of the loop in main(), but without
 * any sort of user prompt displayed.
 */
void readInputFromSTDIN(){

	string userInput = "";
	pid_t returnedPid;
	vector <char *> cmdVec;
	int status;

	while(!feof(stdin)){

		returnedPid = waitpid(-1, &status, WNOHANG);
		while (returnedPid > 0){
			printf("PID [%d] finished execution\n", returnedPid);
			returnedPid = waitpid(-1, &status, WNOHANG);
		}

		cmdVec = splitUserInput(userInput);

		parseUserInput(cmdVec);

		//empty the vector
		cmdVec.clear();
		
		getline(cin, userInput);

	}
	//Exit, because I don't know how to redirect stdin to
	//the ACTUAL stdin. (Which would allow us to go back to main()
	//and continue processing interactively)
	exit(1);
}
