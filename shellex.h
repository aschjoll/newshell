#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <queue>
#include <cstdio>
#include <experimental/filesystem>
#include <stdlib.h>
#include <unistd.h> //for forking and stuff
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

static const char SPACE = ' ';
static const char COLON = ':';
static const string QUIT = "quit";

/* Function prototypes */
void endBackgroundProcesses(int signal);
void backgroundProcesses(int tokenCount);
bool vecStrToChar(vector<string> userTokens, vector<char *>& newTokens, int tokenCount);
void runBackground(vector<string>& userTokens, int tokenCount);
void runForeground(vector<string>& userTokens, int tokenCount);
void initPath();
void changeDirectory(vector<string>& userTokens, int tokenCount);
void setPrompt(vector<string>& userTokens, string& shellPrompt, int tokenCount);
void setVariable(vector<string>& userTokens, int tokenCount);
bool checkAlNum(string token);
int tokenize (string cmdlineInput, vector<string>& userTokens);
void eval(vector<string>& userTokens, string& shellPrompt, int& shellRun, int& returnStatus, int tokenCount);
