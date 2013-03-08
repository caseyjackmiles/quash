#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <vector>
using namespace std; 

vector <char *> splitUserInput(string input);
void parseUserInput(vector <char *> vec);
void setPathHome(vector <char *> vec);

int main (const int argc, char *argv[], char *envp[]){

	string userInput = "";
	int loopNum = 0;
	vector <char *> cmdVec;

//	cout << "Quash is a shell. NO WARRANTY." << endl << "NONE WHATSOEVER." << endl;

	printf("Quite a Shell, version 0.1:\n");
	char *envVar;

	envVar = getenv("PATH");
	cout << "PATH: " << envVar << endl;
	envVar = getenv("HOME");
	cout << "HOME: " << envVar << endl;
	envVar = getcwd(NULL, 0);
	cout << "CUR_DIR: " << envVar << endl << endl;
	


	while ((userInput != "exit") && (userInput != "quit") && (userInput != "q")){

		cmdVec = splitUserInput(userInput);

		//for(int i=0; i<cmdVec.size(); i++){
		//	printf("%d: %s\n", i+1, cmdVec.at(i));
		//}

		parseUserInput(cmdVec);

		cout << "\033[1;31m>\033[0m";
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

	if ((strcmp(vec[0], "path") == 0) || (strcmp(vec[0], "home") == 0)){
		setPathHome(vec);
		return;
	}

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
