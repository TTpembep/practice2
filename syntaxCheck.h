#ifndef INCLUDE_SYNTAXCHECK_H
#define INCLUDE_SYNTAXCHECK_H

#include <iostream>
#include <string>
#include <sstream>
#include "structures.h"
using namespace std;

bool isServiceWord(string word);
SQLQuery syntaxCheck(string query);

#endif