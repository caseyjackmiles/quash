/************************
 *
 * Casey Miles
 * EECS 678
 * Project 1 - Quash Shell
 * Wed., March 13, 2013
 *
 * command.cpp
 * contains functions to parse
 * commands that DO require exec().
 *
************************/


/*
 * This function is called only for commands that
 * perform general program execution.
 * 
 * Parses the command vector to determine if pipes,
 * I/O redirection, or background processing (or all
 * of the above) are used.
 *
 * Then calls the appropriate function to execute the
 * desired program.
 */
void parseCommand(vector <char *> vec){

	//Default program execution variables
	string fdin = "0";					//default: "0", i.e., STD_IN
	string fdout = "1";					//default: "0", i.e., STD_OUT
	vector <char *> cmd1 = vec;			//default: original command
	vector <char *> cmd2;				//default: empty
	bool runBackground = false;			//default: false


	// Check if an ampersand has been appended to the end of the
	// command vector. If so, run the process in the background.
	if(strcmp(vec[vec.size() - 1], "&") == 0){
		runBackground = true;
		vec.pop_back();			//remove & from command vector
	}

	
	//Check the vector for < and > characters.
	//If found, check that they are in logical positions
	//And then save names of files to open.
	for(int i=0; i < vec.size(); ++i){
		
			//Redirect standard input if < is used
			if(strcmp(vec[i], "<") == 0){
				if(i == vec.size() - 1){		// '<' character at end of command, so there is no file named
					cout << "ERROR: No file specified for input redirection <" << endl;
					return;
				}

				//grab name of infile
				fdin = vec[i+1];

				//Now remove the '<' and the filename from the vector
				vec.erase(vec.begin() + i);
				vec.erase(vec.begin() + i);
			}

			//Redirect standard output if > is used
			else if(strcmp(vec[i], ">") == 0){
				if(i == vec.size() - 1){		// '>' character at end of command, so there is no file named
					cout << "ERROR: No file specified for output redirection >" << endl;
					return;
				}

				//grab name of outfile
				fdout = vec[i+1];

				//Now remove the '>' and the filename from the vector
				vec.erase(vec.begin() + i);
				vec.erase(vec.begin() + i);
			}
	}

	//Go through the vector *one* more time to check
	//for '|' (pipe) character. If found, split the command
	//vector into *two* command vectors, one for first command
	//and the other for the second command.
	for(int i=0; i < vec.size(); i++){
		//Search for '|' character: Piping command
		if(strcmp(vec[i], "|") == 0){

			//Clear both command vectors to make sure they don't have crap in them
			cmd1.clear();
			cmd2.clear();

			//i is the location of '|'. We don't want this character in our
			//command vectors.
			for (int cmdRebuild = 0; cmdRebuild < vec.size(); cmdRebuild++){
				if (cmdRebuild < i)
					cmd1.push_back(vec[cmdRebuild]);

				else if (cmdRebuild > i)
					cmd2.push_back(vec[cmdRebuild]);
			}
		}
	}


	//////////////////////////////
	// If we're not using a pipe, the original vec could
	// have very likely had items removed from it. Reassign
	// cmd1 = vec to make sure it is up-to-date.
	if (cmd2.empty()){
		cmd1 = vec;			// The second command vector has nothing in it (i.e., no pipe), so we need to adjust cmd1
	}


	if(runBackground)
		int dummyBackInt = executeBackgroundCommand(cmd1, cmd2, fdin, fdout);
	else
		int dummyint = executeCommand(cmd1, cmd2, fdin, fdout);
}

/*
 * Based on the parameters passed from parseCommand(), fork() a process and run
 * an executable.
 *
 * Processes may or may not be piped together and may or may not have redirected
 * input/output.
 */
int executeCommand(vector <char *> cmd1, vector <char *> cmd2, string fdin, string fdout){ 

	int status;
	int pid1, pid2;
	int pipefds[2];

	//if cmd2 is still empty, we're not using a pipe
	bool usePipe = !cmd2.empty();

	//Push NULL to the ends of both command vectors because
	//execvp() requires arguments to end with a (char *) NULL.
	cmd1.push_back(NULL);
	cmd2.push_back(NULL);

	//Create a pipe if we're using one.
	if (usePipe){
		pipe(pipefds);
	}

	//Fork a new process
	pid1 = fork();
	if(pid1 == 0){			//First child process

		
		//Rewrite std_in to fdin. fdin is either "0" (in which case, it's std_in
		//and we don't need to dup2, or it's some user-specified file.
		if(fdin != "0"){
			int in = open(fdin.c_str(), O_RDONLY);
			dup2(in, 0);
			close(in);
		}

		if(usePipe){	//Using a pipe, so redirect std_out to write end of the pipe.
			//Close read-end of pipe
			//Process should read from fdin specified earlier
			close(pipefds[0]);
			dup2(pipefds[1], 1);
			close(pipefds[1]);

		}
		else {			//No pipe, so redirect std_out to whatever's specified in fdout
			if(fdout != "1"){
				int out = open(fdout.c_str(), O_WRONLY ^ O_CREAT, 00644);
				dup2(out, 1);
				close(out);
			}
		}

		//Execute the command specified in cmd1[0]
		if((execvp(cmd1[0], &cmd1[0]) < 0)){
			fprintf(stderr, "\nError while executing %s. Error #%d.\n", cmd1[0], errno);
			exit(EXIT_FAILURE);
		}
	}
	//end child 1
	

	//Fork a second process, if we're using a pipe
	if(usePipe)
		pid2 = fork();

	if(pid2 == 0){			//Second child process
			
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
	//end child 2
	
	//Close the unneeded pipes if we made them
	//Also wait for second process
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


/*
 * Based on the parameters passed from parseCommand(), fork() a process and run
 * an executable *in the background*.
 *
 * Parent process does NOT call wait(). Instead, some simple output describing the
 * process is printed to the screen.
 *
 * Processes may or may not be piped together and may or may not have redirected
 * input/output.
 */
int executeBackgroundCommand(vector <char *> cmd1, vector <char *> cmd2, string fdin, string fdout){ 

	int status;
	int pid1, pid2;
	int pipefds[2];

	//If cmd2 is empty then we're not using a pipe
	bool usePipe = !cmd2.empty();

	//Push NULL to the end of both command vectors
	//because execvp() requires arguments to end in (char *) NULL
	cmd1.push_back(NULL);
	cmd2.push_back(NULL);

	//Create a pipe if we need one
	if (usePipe){
		pipe(pipefds);
	}

	//Create a child process
	pid1 = fork();
	if(pid1 == 0){			//First child process


		//Get information about process
		pid_t backgroundPid1 = getpid();
		char *backCmd1 = cmd1[0];

		//Print creation of new process to screen
		printf("PID [%d] running %s in background\n", backgroundPid1, backCmd1);



		//REWRITE std_in to fdin
		if(fdin != "0"){
			int in = open(fdin.c_str(), O_RDONLY);
			dup2(in, 0);
			close(in);
		}

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

		//Execute the program specified in cmd1[0].
		if((execvp(cmd1[0], &cmd1[0]) < 0)){
			fprintf(stderr, "\nError while executing %s. Error #%d.\n", cmd1[0], errno);
			exit(EXIT_FAILURE);
		}
	}
	//end child 1
	

	//Fork a second child if we're using a pipe
	if(usePipe)
		pid2 = fork();

	if(pid2 == 0){			//Second child process

		//Gather information about process
		pid_t backgroundPid2 = getpid();
		char *backCmd2 = cmd2[0];

		//Print creation of new process to screen
		printf("PID [%d] running %s in background\n", backgroundPid2, backCmd2);



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
	//end child 2
	
	//Close the unneeded pipes if we made them
	//Also wait for second process
	if(usePipe){
		close(pipefds[1]);
		
	//	if((waitpid(pid2, &status, 0)) == -1){
	//		fprintf(stderr, "\nError on process '%s'. Error #%d.\n", cmd1[0], errno);
	//		return EXIT_FAILURE;
	//	}
	}

	//if((waitpid(pid1, &status, 0)) == -1){
	//	fprintf(stderr, "\nError on process '%s'. Error #%d.\n", cmd1[0], errno);
	//	return EXIT_FAILURE;
	//}

}
