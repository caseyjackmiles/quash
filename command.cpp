void parseCommand(vector <char *> vec){

	//Default program execution variables
	string fdin = "0";						//default: STD_IN
	string fdout = "1";						//default: STD_OUT
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
		//if((strcmp(vec[i], "<") == 0) || (strcmp(vec[i], ">") == 0)){

			//Redirect standard input
			if(strcmp(vec[i], "<") == 0){
				if(i == vec.size() - 1){		// '<' character at end of command, so there is no file named
					cout << "ERROR: No file specified for input redirection <" << endl;
					return;
				}

				//grab name of infile
				//char *inFile = vec[i+1];
				fdin = vec[i+1];

				//create a file descriptor for fdin
				//fdin = open(inFile, O_RDONLY);

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
				//char *outFile = vec[i+1];
				fdout = vec[i+1];

				//create a file descriptor for fdout
				//fdout = open(outFile, O_WRONLY ^ O_CREAT);

				//Now remove the '>' and the filename from the vector
				vec.erase(vec.begin() + i);
				vec.erase(vec.begin() + i);

			}

		//}
	}

	for(int i=0; i < vec.size(); i++){
		//Search for '|' character: Piping command
		if(strcmp(vec[i], "|") == 0){
			//cout << "Piping functionality not yet implemented." << endl;
			//return;

			//Clear both command vectors to make sure they don't have crap in them
			cmd1.clear();
			cmd2.clear();

			for (int cmdRebuild = 0; cmdRebuild < vec.size(); cmdRebuild++){

				if (cmdRebuild < i)
					cmd1.push_back(vec[cmdRebuild]);

				else if (cmdRebuild > i)
					cmd2.push_back(vec[cmdRebuild]);

			}



		}
	}


	/////////// PREPARE THE COMMAND VECTORS ///////////////////
	if (cmd2.empty()){
		cmd1 = vec;			// The second command vector has nothing in it (i.e., no pipe), so we need to adjust cmd1
	}


	//We should try to have all the parameters and stuff set by this point.
	//This function will execute the requested command.
	int dummyint = executeCommand(cmd1, cmd2, fdin, fdout);
}

int executeCommand(vector <char *> cmd1, vector <char *> cmd2, string fdin, string fdout){ 

	int status;
	int pid1, pid2;
	int pipefds[2];

	bool usePipe = !cmd2.empty();

	cmd1.push_back(NULL);
	cmd2.push_back(NULL);

	if (usePipe){
		pipe(pipefds);
	}

	pid1 = fork();
	if(pid1 == 0){

		//REWRITE std_in to fdin
		if(fdin != "0"){
			int in = open(fdin.c_str(), O_RDONLY);
			dup2(in, 0);
			close(in);
		}

		//BEFORE PIPING IMPLEMENTED
		//REWRITE std_out to fdout
		//if(fdout != "1"){
		//	int out = open(fdout.c_str(), O_WRONLY ^ O_CREAT, 00644);
		//	dup2(out, 1);
		//	close(out);
		//}

		if(usePipe){		//Using a pipe, so redirect std_out to the write end of the pipe.
			//Close read-end of pipe
			//Process should read from fdin specified earlier
			close(pipefds[0]);
			dup2(pipefds[1], 1);
			close(pipefds[1]);

		}
		else {				//No pipe, so redirect std_out to whatever's specified in fdout
			if(fdout != "1"){
				int out = open(fdout.c_str(), O_WRONLY ^ O_CREAT, 00644);
				dup2(out, 1);
				close(out);
			}
		}

		if((execvp(cmd1[0], &cmd1[0]) < 0)){
			fprintf(stderr, "\nError while executing %s. Error #%d.\n", cmd1[0], errno);
			exit(EXIT_FAILURE);
		}
	}
	//end child 1
	

	if(usePipe)
		pid2 = fork();

	if(pid2 == 0){

		//We know we're using a pipe at this point, so redirect
		//process input to the read end of the pipe
		close(pipefds[1]);
		dup2(pipefds[0], 0);
		close(pipefds[0]);


		//Now check to see what we should output the results to:
		//std_out or something specified in fdout

		if(fdout != "1"){
			int out = open(fdout.c_str(), O_WRONLY ^ O_CREAT, 00644);
			dup2(out, 1);
			close(out);
		}

		//Now execute the second command
		if((execvp(cmd2[0], &cmd2[0]) < 0)){
			fprintf(stderr, "\nError while executing %s. Error #%d.\n", cmd2[0], errno);
			exit(EXIT_FAILURE);
		}
	}
	//end child 2, if piped IE, THE PIPED CHILD
	
	//close(fdin);
	//close(fdout);

	//Close the unneeded pipes if we made them
	if(usePipe){
		close(pipefds[1]);
		
		if((waitpid(pid2, &status, 0)) == -1){
			fprintf(stderr, "\nError on process '%s'. Error #%d.\n", cmd1[0], errno);
			return EXIT_FAILURE;
		}
	}

	if((waitpid(pid1, &status, 0)) == -1){
		fprintf(stderr, "\nError on process '%s'. Error #%d.\n", cmd1[0], errno);
		return EXIT_FAILURE;
	}

}
