void parseCommand(vector <char *> vec){

	//Default program execution variables
	int fdin = 0;						//default: STD_IN
	int fdout = 1;						//default: STD_OUT
	vector <char *> cmd1 = vec;			//default: original command
	vector <char *> cmd2;				//default: empty
	bool runBackground = false;			//default: false

	//cout << "Parsing command " << vec[0] << endl;

	// Check if an ampersand has been appended to the end of the
	// command vector. If so, run the process in the background.
	if(strcmp(vec[vec.size() - 1], "&") == 0){
		cout << "Background functionality not yet implemented." << endl;
		return;
	}

	
	for(int i=0; i < vec.size(); ++i){
		
		//Search for '<' and '>' characters: IO Redirection
		if((strcmp(vec[i], "<") == 0) || (strcmp(vec[i], ">") == 0)){

			//Redirect standard input
			if(strcmp(vec[i], "<") == 0){
				if(i == vec.size() - 1){		// '<' character at end of command, so there is no file named
					cout << "ERROR: No file specified for input redirection <" << endl;
					return;
				}

				//grab name of infile
				char *inFile = vec[i+1];

				//create a file descriptor for fdin
				fdin = open(inFile, O_RDONLY);

				//Now remove the '<' and the filename from the vector
				vec.erase(vec.begin() + i);
				vec.erase(vec.begin() + i);

			}

			//Redirect standard output
			else if(strcmp(vec[i], ">") == 0){
				if(i == vec.size() - 1){		// '>' character at end of command, so there is no file named
					cout << "ERROR: No file specified for output redirection >" << endl;
					return;
				}

				//grab name of outfile
				char *outFile = vec[i+1];

				//create a file descriptor for fdout
				fdout = open(outFile, O_WRONLY ^ O_CREAT);

				//Now remove the '>' and the filename from the vector
				vec.erase(vec.begin() + i);
				vec.erase(vec.begin() + i);

			}

		}
	}

	for(int i=0; i < vec.size(); i++){
		//Search for '|' character: Piping command
		if(strcmp(vec[i], "|") == 0){
			cout << "Piping functionality not yet implemented." << endl;
			return;
		}
	}


	/////////// PREPARE THE COMMAND VECTORS ///////////////////
	cmd1 = vec;


	//We should try to have all the parameters and stuff set by this point.
	//This function will execute the requested command.
	int dummyint = executeCommand(cmd1, fdin, fdout);
}

int executeCommand(vector <char *> vec, int fdin, int fdout){ 

	int status;

	int pid1, pid2;

	vec.push_back(NULL);

	pid1 = fork();
	if(pid1 == 0){

		//REWRITE std_in to fdin
		//dup2(fdin, 0);
		//close(fdin);

		//REWRITE std_out to fdout
		dup2(fdout, 1);



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
