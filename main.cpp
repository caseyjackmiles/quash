#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
using namespace std; 

void parseUserInput(string input);

int main (const int argc, char *argv[], char *envp[]){

	string userInput = "";
	int loopNum = 0;
	vector <string> cmdVec;

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

		if (loopNum > 0)
			parseUserInput(userInput);

		getline(cin, userInput);
		loopNum++;
	}

	cout << "Exiting quash." << endl;
	return 0;

}

void parseUserInput(string input){
	cout << "You're parsing " << input << endl;
}
