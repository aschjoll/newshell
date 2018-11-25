#include "shellex.h"

unordered_map<string, string> variables;  // empty map container for setting variables
unordered_map<int, string> bgProcesses;   // empty map container for background processes
string currentDirectory;                  //stores current directory

// referenced by signal in main. if background process has ended,
// remove process from the map of processes
void endBackgroundProcesses(int signal)
{
  if(bgProcesses.empty())
    return;
  pid_t pid;
  unordered_map<int,string>::iterator it; // iterates through bgProcesses
  pid = wait(NULL);                       // waits for processes to end
  it = bgProcesses.find(pid);             // finds pid's position in the map
  bgProcesses.erase(it);                  //removes pid from the map of bgProcesses
}

// prints the processes currently running in the background
void backgroundProcesses(int tokenCount)
{
  if (tokenCount == 1)  // has the correct token count
  {
    cout << "Printing all processes in the backround processes map:" << endl;
    for (auto x : bgProcesses)  // prints processes
      cout << "  " << x.second << endl;
    if (bgProcesses.empty())    //displays if there are no processes in bgProcesses
      cerr << "  " << "No processes to display." << endl;
  }
  else  // correct format
    cerr << "Format for displaying background processes: bp" << endl << "No arguments." << endl;;
}

// converts a vector of strings to a vector of char*
bool vecStrToChar(vector<string> userTokens, vector<char *>& newTokens, int tokenCount)
{

  if(tokenCount >= 1) //gotta have arguments
  {
    for (int i = 0; i < tokenCount; i++)  //populates newTokens with the tokens from userTokens
      newTokens.push_back((char*)userTokens[i].c_str());
    newTokens.push_back(NULL);
    return 1;
  }
  else
    return 0;
}

// runs processes in the background
void runBackground(vector<string>& userTokens, int tokenCount)
{

  userTokens.pop_back();  //removes the '&' from the vector

  // '.' means the path name given by the user continues from the current path
  if (userTokens[0].front() == '.' && userTokens[0][1] == '/')
  {

    if(currentDirectory.back() == '/')                                //remove / to prepare for concatenatenation
      currentDirectory = currentDirectory.substr(0,currentDirectory.length()-1);
    userTokens[0] = userTokens[0].substr(1, userTokens[0].length());  //remove the . from the path given from input
    userTokens[0] = (currentDirectory)+(userTokens[0]);               //concatenate the current path with the given file path

    vector<char *> newTokens;
    if (vecStrToChar(userTokens, newTokens, tokenCount))
    {
      pid_t pid;                        // process ID
      int status;                       // status integer for waitpid to return specfic values
      char** command = &newTokens[0];   //user's input cmnd

      pid = fork();         //splitting the shell

      if (pid < 0)          // prints newsh error if there is an issue with forking
        perror("newsh");
      else if (pid == 0)    //child process executes the passed commands
      {
        int child = execvp(command[0],command);

        if (child == -1)      // if execvp() fails
        {
          perror("newsh");    // prints exec error to standard output
          exit(EXIT_FAILURE); // exit with EXIT_FAILURE after printing error
        }
      }
      else  //parent process is the shell
      {
        bgProcesses[pid] = command[0];
        waitpid(pid, &status, WNOHANG);
        return;
      }
    }
    return;
  }// end if
  // is given the full file path
  else if(userTokens[0].front() == '/')
  {
    vector<char *> newTokens;
    if (vecStrToChar(userTokens, newTokens, tokenCount))
    {
      pid_t pid;                        // process ID
      int status;                       // status integer for waitpid to return specfic values
      char** command = &newTokens[0];   //user's input cmnd

      pid = fork();         //splitting the shell

      if (pid < 0)          // prints newsh error if there is an issue with forking
          perror("newsh");
      else if (pid == 0)    //child process executes the passed commands
      {
        int child = execvp(command[0],command);

        if (child == -1)      // if execvp() fails
        {
          perror("newsh");    // prints exec error to standard output
          exit(EXIT_FAILURE); // exit with EXIT_FAILURE after printing error
        }
      }
      else  //parent process is the shell
      {
        bgProcesses[pid] = command[0];
        waitpid(pid, &status, WNOHANG);
        return;
      }
    }
  }// end else if
  // tries to run the cmd in all the different paths, if doesnt work, then doesnt exist
  else
  {
    int i = 0;
    vector<string> paths;
    string token;
    string stringPath = variables.find("PATH")->second;
    istringstream path(stringPath);

    while (getline(path, token, COLON))//loop tokenizes the PATH variable into it's different paths
    {
        if (token[0] != '\0')  //if we haven't reached the end
        {
            if(token.back() != '/')
              token += "/";
            paths.push_back(token);
        }
        else
          cerr << "Error with tokenizing" << endl;
    }//end while

    vector<char *> newTokens;
    bool good = vecStrToChar(userTokens, newTokens, tokenCount);
    string originalname = userTokens[0]; //saves the name of the program that the user wants to run
    if (good)
    {
      int child = -1;
      while(child == -1)
      {
        if(i == paths.size())
          break;

        userTokens[0] = paths[i]+originalname;  //adds the name of the program to the end of the directory to be tried

        pid_t pid;                        // process ID
        int status;                       // status integer for waitpid to return specfic values
        char** command = &newTokens[i];   //user's input cmnd
        pid = fork();         //splitting the shell

        if (pid < 0)          // prints newsh error if there is an issue with forking
          perror("newsh");
        else if (pid == 0)    //child process executes the passed commands
        {
          child = execvp(command[i],command);
          if (child == -1)      // if execvp() fails
          {
            perror("newsh");    // prints exec error to standard output
            exit(EXIT_FAILURE); // exit with EXIT_FAILURE after printing error
          }
        }
        else  //parents process is the shell
        {
          bgProcesses[pid] = command[0];
          waitpid(pid, &status, WNOHANG);
          return;
        }
        i++;
      }
    }
    return;
  }
  cerr << "Could not find program" << endl;
}

//runs processes in the foreground and waits until completion to allow more input
void runForeground(vector<string>& userTokens, int tokenCount)
{
  // '.' means the path name given by the user continues from the current path.
  if (userTokens[0].front() == '.' && userTokens[0][1] == '/')
  {

    if(currentDirectory.back() == '/')                                //remove / to prepare for concatenatenation
      currentDirectory = currentDirectory.substr(0,currentDirectory.length()-1);
    userTokens[0] = userTokens[0].substr(1, userTokens[0].length());  //remove the . from the path given from input
    userTokens[0] = (currentDirectory)+(userTokens[0]);               //concatenate the current path with the given file path

    vector<char *> newTokens;
    if (vecStrToChar(userTokens, newTokens, tokenCount))
    {
      pid_t pid;                        // process ID
      int status;                       // status integer for waitpid to return specfic values
      char** command = &newTokens[0];   //user's input cmnd

      pid = fork();         //splitting the shell

      if (pid < 0)          // prints newsh error if there is an issue with forking
        perror("newsh");
      else if (pid == 0)    //child process executes the passed commands
      {
        int child = execvp(command[0],command);

        if (child == -1)      // if execvp() fails
        {
          perror("newsh");    // prints exec error to standard output
          exit(EXIT_FAILURE); // exit with EXIT_FAILURE after printing error
        }
      }
      else  //parent process is the shell
      {
        do {
          waitpid(pid, &status, WUNTRACED);
        } while (!WIFSIGNALED(status) && !WIFEXITED(status));
      }
    }
    return;
  }// end if
  else if(userTokens[0].front() == '/')
  {
    vector<char *> newTokens;
    if (vecStrToChar(userTokens, newTokens, tokenCount))
    {
      pid_t pid;                        // process ID
      int status;                       // status integer for waitpid to return specfic values
      char** command = &newTokens[0];   //user's input cmnd

      pid = fork();         //splitting the shell

      if (pid < 0)          // prints newsh error if there is an issue with forking
        perror("newsh");
      else if (pid == 0)    //child process executes the passed commands
      {
        int child = execvp(command[0],command);

        if (child == -1)      // if execvp() fails
        {
          perror("newsh");    // prints exec error to standard output
          exit(EXIT_FAILURE); // exit with EXIT_FAILURE after printing error
        }
      }
      else  //parents process is the shell
      {
        do {
          waitpid(pid, &status, WUNTRACED);
        } while (!WIFSIGNALED(status) && !WIFEXITED(status));
      }
    }
    return;
  }//end else if
  else
  {
    int i = 0;
    vector<string> paths;
    string token;
    string stringPath = variables.find("PATH")->second;
    istringstream path(stringPath);

    while (getline(path, token, COLON)) //loop tokenizes the PATH variable into it's different paths
    {
        if (token[0] != '\0')  //if we haven't reached the end
        {
            if(token.back() != '/')
              token += "/";
            paths.push_back(token);
        }
        else
          cerr << "Error with tokenizing" << endl;
    }//end while

    vector<char *> newTokens;
    bool good = vecStrToChar(userTokens, newTokens, tokenCount); // if conversion was successful
    string originalname = userTokens[0]; // saves the name of the program that the user wants to run
    if (good)
    {
      while(i < paths.size())
      {
        userTokens[0] = paths[i]+originalname;//adds the name of the program to the end of the directory to be tried

        pid_t pid;                        // process ID
        int status;                       // status integer for waitpid to return specfic values
        char** command = &newTokens[i];   //user's input cmnd

        pid = fork();         //splitting the shell

        if (pid < 0)          // prints newsh error if there is an issue with forking
            perror("newsh");
        else if (pid == 0)    //child process executes the passed commands
        {
          int child = execvp(command[i],command);

          if (child == -1)      // if execvp() fails
          {
            perror("newsh");    // prints exec error to standard output
            exit(EXIT_FAILURE); // exit with EXIT_FAILURE after printing error
          }
        }
        else  //parents process is the shell
        {
           do {
            waitpid(pid, &status, WUNTRACED);
          } while (!WIFSIGNALED(status) && !WIFEXITED(status));
          return;
        }
        i++;
      }
    }
  }
  cerr << "Could not find program" << endl;
}

// initializes path to a default, called by main
void initPath()
{
  variables["PATH"] = "/bin:/usr/bin";
}

// changes your directory
void changeDirectory(vector<string>& userTokens, int tokenCount)
{
  int complete;
  if (tokenCount == 2)
  {
    const char* directoryName = ((char*)userTokens[1].c_str());
    complete = chdir(directoryName);

    if(complete < 0)
      cerr << "chdir PATH not set" << endl;
  }
  else
    cerr << "Format for changing directory: cd directoryName" << endl;
}

//sets user's prompt
void setPrompt(vector<string>& userTokens, string& shellPrompt, int tokenCount)
{
  if (tokenCount == 2)  // gotta have enouhg parameters
    shellPrompt = userTokens[1]; // reassigns the passed value of shellPrompt to the second element of userInput
  else
    cerr << "Format for setting prompts: PROMPT value" << endl;
}

//sets a variable using an unordered map
void setVariable(vector<string>& userTokens, int tokenCount)
{
  bool alNum = checkAlNum(userTokens.at(1));

  if (tokenCount == 3 && alNum == true)
    variables[userTokens.at(1)] = userTokens.at(2);
  else
    cerr << "Format for setting variables: set variable value\n" << "Variable must be alphanumeric and first character must be alpha" << endl;
}

// checks that the input string is alphanumeric
bool checkAlNum(string token)
{
  int i = 0;
  while (isalnum(token[i])) i++;
  if (i == token.length() && isalpha(token[0]))
    return 1;
  else
    return 0;
}

// passes in a string of user's input and tokenizes the input to a vector string
int tokenize (string cmdlineInput, vector<string>& userTokens)
{
  istringstream input(cmdlineInput);
  int tokenCount = 0;
  string token;

  while (getline(input, token, SPACE))
  {
      if (token == "%") // handles comments
        break; // disregard the rest of the line
      else if (token[0] == '$') // is a variable
      {
        token = token.substr(1, token.length()); //removes the $ from the token

        auto search = variables.find(token);
        //if the variable does not exist in the variables map
        if (search == variables.end())
          cerr << "There is not a variable called " << token << endl;
        else
        {
          token = variables[token]; // resets the variable value to the given
          userTokens.push_back(token); // fills the userTokens vector with tokens
        }
          token = variables[token]; // resets the variable value to the given
      }
      else if (token[0] != '\0')
          userTokens.push_back(token); // fills the userTokens vector with tokens
      else
        cerr << "Error with tokenizing" << endl;
  }
  tokenCount = userTokens.size();
  return tokenCount;
}

/* eval - Evaluate what program should do next */
void eval(vector<string>& userTokens, string& shellPrompt, int& shellRun, int& returnStatus, int tokenCount)
{

  string firstToken = userTokens.front();
  string lastToken = userTokens.back();

  if (firstToken == QUIT) // quit command
  {
    shellRun++;
    cout << "Bye" << endl;
  }
  else if(firstToken == "set")
    setVariable(userTokens, tokenCount);
  else if (firstToken == "PROMPT")
    setPrompt(userTokens, shellPrompt, tokenCount);
  else if(firstToken == "cd")
    changeDirectory(userTokens, tokenCount);
  else if(firstToken == "bp")
    backgroundProcesses(tokenCount);
  else if(lastToken.front() == '&')
    runBackground(userTokens, tokenCount);
  else //if(firstToken.front() == '/' || firstToken.front() == '.')
    runForeground(userTokens, tokenCount);

  return;
}
