#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <vector>

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

#include "command.cpp"

int main (const int argc, char *argv[], char *envp[]){


	string userInput = "";
	int loopNum = 0;
	vector <char *> cmdVec;

//*************************************************
//  Print initial environment variables
//*************************************************
	printf("Quite a Shell, version 0.6:\n");
	char *envVar;

	cout << "PATH: " << getenv("PATH") << endl;
	cout << "HOME: " << getenv("HOME") << endl;
	cout << "CUR_DIR: " << getcwd(NULL, 0) << endl << endl;

	//for(int i=0; environ[i]!=NULL; i++){
	//	cout << environ[i] << endl;
	//	cout << "****************************************************" << endl;
	//}

//*************************************************
	


	while ((userInput != "exit") && (userInput != "quit") && (userInput != "q")){

		cmdVec = splitUserInput(userInput);

		//for(int i=0; i<cmdVec.size(); i++){
		//	printf("%d: %s\n", i+1, cmdVec.at(i));
		//}

		parseUserInput(cmdVec);

		cout << "\033[1;31m" << getcwd(NULL, 0) << ">\033[0m";

		//empty the vector?
		cmdVec.clear();

		getline(cin, userInput);
		loopNum++;
	}

	cout << "Exiting quash." << endl;
	return 0;

}

vector <char *> splitUserInput(string i){

	//Clear all elements from the command vector
	vector <char *> cmdVec;

	char *input = (char *)i.c_str();

	char *tokens = strtok(input, " ");
	while (tokens != NULL){
	//	cout << tokens << endl;
		cmdVec.push_back(tokens);
		tokens = strtok(NULL, " ");
	}

	return cmdVec;
}

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

	parseCommand(vec);

	return;
}

void setPathHome(vector <char *> vec){
	
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
				

	if(vec.size() > 2){
		cout << "ERROR: " << vec[0] << " only takes one argument." << endl;
		return;
	}

	if ((strcmp(vec[0], "path")) == 0){
		if ((setenv("PATH", vec[1], 1)) == -1) {
			fprintf(stderr, "Error occurred setting PATH. Error #%d\n", errno);
			return;
		}
		cout << "Path set to: " << getenv("PATH") << endl;
	}

	if((strcmp(vec[0], "home")) == 0){
		if ((setenv("HOME", vec[1], 1)) == -1){
			fprintf(stderr, "Error occurred setting HOME. Error #%d\n", errno);
			return;
		}
		cout << "Home set to: " << getenv("HOME") << endl;
	}
}

void setWorkingDir(vector <char *> vec){
	if(vec.size() > 2){
		cout << "ERROR: " << vec[0] << " only takes one argument." << endl;
		return;
	}

	if(vec.size() == 1){
		vec.push_back(getenv("HOME"));

		if ((chdir(vec[1])) == -1){
			fprintf(stderr, "Error occurred on cd to HOME. Error #%d\n", errno);
			return;
		}
		return;
	}

	if((chdir(vec[1])) == -1){
		fprintf(stderr, "Error occurred on cd to %s. Error #%d\n", vec[1], errno);
		return;
	}
	return;
}

void displayJobs(){
	cout << "'Jobs' feature not yet implemented." << endl;
}
