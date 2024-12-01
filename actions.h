#ifndef INCLUDE_ACTIONS_H
#define INCLUDE_ACTIONS_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "structures.h"
using namespace std;

bool isUnlocked(const string& schemaName, const string& tableName);
void lock(const string& schemaName, const string& tableName);
void unlock(const string& schemaName, const string& tableName);

int getPrimaryKey(const string& tablePath );
void updatePrimaryKey(const string& tableName, int newPrimaryKey);
int getRowCount(const string& filePath);
void insertCSV(const Schema& schema, SQLQuery& query);

//string openedSchemeName;
//string openedTableName;

string buildConditionString(Node* node);
bool isConditionTrue(const string& row, const string& columnNames, const string* line);
void deleteFromCSV(const Schema& schema, SQLQuery& query);

string superPrintFunc(const string& row, const string& columnNames, const string& line);
void selectTables(const Schema& schema, SQLQuery& query);

#endif