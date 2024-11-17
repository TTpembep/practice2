#ifndef INCLUDE_DBINIT_H
#define INCLUDE_DBINIT_H

#include <iostream>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include "structures.h"
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

bool createDir (string name);
void dbInit(Schema& schema);

#endif