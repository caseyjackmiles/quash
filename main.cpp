#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
using namespace std; 

vector <char *> splitUserInput(string input);

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

		for(int i=0; i<cmdVec.size(); i++){
			printf("%d: %s\n", i+1, cmdVec.at(i));
		}

		cout << ">";
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
