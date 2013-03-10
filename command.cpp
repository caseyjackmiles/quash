void parseCommand(vector <char *> vec){

	bool redirIn = false;
	bool redirOut = false;
	bool usePipe = false;
	int firstCmdEnd = vec.size() - 1;
	int secondCmdEnd = -1;
	string inFile = "";
	string outFile = "";
	bool runBackground = false;

	//cout << "Parsing command " << vec[0] << endl;

	// Check if an ampersand has been appended to the end of the
	// command vector. If so, run the process in the background.
	if(strcmp(vec[vec.size() - 1], "&") == 0){
		cout << "Background functionality not yet implemented." << endl;
		return;
	}

	
	for(int i=0; i < vec.size(); i++){
		
		//Search for '<' and '>' characters: IO Redirection
		if((strcmp(vec[i], "<") == 0) || (strcmp(vec[i], ">") == 0)){
			cout << "IO redirection not yet implemented." << endl;
			return;
		}

		//Search for '|' character: Piping command
		if(strcmp(vec[i], "|") == 0){
			cout << "Piping functionality not yet implemented." << endl;
			return;
		}
	
	}

	//We should try to have all the parameters and stuff set by this point.
	//This function will execute the requested command.
	int dummyint = executeCommand(vec, redirIn, inFile, redirOut, outFile, usePipe, firstCmdEnd, secondCmdEnd, runBackground);
}

int executeCommand(vector <char *> vec, bool redirIn, string inFile, bool redirOut, string outFile, bool usePipe, int firstCmdEnd, int secondCmdEnd, bool runBackground){

	int status;

	int pid1, pid2;

	vec.push_back(NULL);

	pid1 = fork();
	if(pid1 == 0){

		//if((execvpe(vec[0], &vec[0], env) < 0)){ 
		if((execvp(vec[0], &vec[0]) < 0)){
			fprintf(stderr, "\nError while executing %s. Error #%d.\n", vec[0], errno);
			exit(EXIT_FAILURE);
		}
	}
	//end child 1

	if((waitpid(pid1, &status, 0)) == -1){
		fprintf(stderr, "\nError on process '%s'. Error #%d.\n", vec[0], errno);
		return EXIT_FAILURE;
	}


}
