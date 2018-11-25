#include "shellex.h"

int main()
{
  int shellRun = 0;       //is the shell running outside of the loop?
  int cmdlineSize = 0;
  int runShell = 0;       // integer to determine whether the shell exits out of its while loop and finishes. If it is anything other than 0, exit
  int tokenCount, returnStatus;
  string shellPrompt = "newsh";
  string cmdlineInput;    // raw user input
  string token;           // creates a string to hold each individual token

  signal(SIGCHLD, endBackgroundProcesses);
  vector<string> userTokens;   // string vector to hold all the tokens from the new cmdlineInput
  istringstream stream(cmdlineInput); // creates a stringstream named iss that now holds cmdlineInput.
  initPath();
  while (shellRun < 1)
  {
    userTokens.clear(); //sets userTokens back to 0

    cout << shellPrompt << "$ ";  // print the shellPrompt at the beginning of line

    getline(cin, cmdlineInput);   // read in input and throw it into cmdlineInput as a string

    if (cin.eof())      //check for EOF
    {
      shellRun++;
      cout << "\nBye" << endl;
    }
    else
    {
      tokenCount = tokenize(cmdlineInput, userTokens);  // tokenize

      if (tokenCount != 0) //handles clicking enter
          eval(userTokens, shellPrompt, shellRun, returnStatus, tokenCount);
    }
  }
}
