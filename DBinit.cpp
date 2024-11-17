#include "DBinit.h"

bool createDir (string name) {  //Функция для создания директории
 if (mkdir(name.c_str(), 0755) == -1) {
    cout << "Error creating directory: Directory exists " << endl;
    return 0;
  } else {
    cout << "Directory " << name << " created successfully!" << endl;
  return 1;
  }
}
void dbInit(Schema& schema){  //Основная функция создания и проверки наличия БД
  ifstream file("schema.json");
  json j;
  file >>j;
  for ( const auto& data:j){
  schema.name = data["name"].get<string>();
  createDir(schema.name);
  schema.tuples_limit=data["tuples_limit"].get<int>();
  for (const auto& [tableName, columns] : data["structure"].items()){
    createDir(schema.name+"/"+tableName);
    ifstream fin(schema.name+"/"+tableName+"/"+"1.csv");
    if (fin.good()){
      cout << "1.csv already exist!" << endl;
      }else {
        ofstream fout;
        fout.open (schema.name+"/"+tableName+"/"+"1.csv"); 
        fout << tableName+"_" <<"pk" <<",";
        int i=0;
    
        for( const auto& column : columns) {
          fout << column.get<string>();
          if (i<columns.size()-1) fout << ",";
          i++;
        }
        fout << endl;
        fout.close();
        cout << "Created "+schema.name+"/"+tableName+"/"+tableName+"/1.csv" << endl;
        }
        fin.close();
        fin.open(schema.name+"/"+tableName+"/"+tableName+"_pk_sequence");
        if (fin.good()) {
          cout << schema.name+"/"+tableName+"/"+tableName+"_pk_sequence is already exist!" << endl;
        } else{
          ofstream fout;
          fout.open(schema.name+"/"+tableName+"/"+tableName+"_pk_sequence");
          fout << 1;
          fout.close();
          cout << "Created "+schema.name+"/"+tableName+"/"+tableName+"_pk_sequence" << endl;
        }
        fin.close();
        fin.open(schema.name+"/"+tableName+"/"+tableName+"_lock");
        if (fin.good()){
          cout << schema.name+"/"+tableName+"/"+tableName+"_lock is already exist!" << endl;
        }else{
          ofstream fout;
          fout.open(schema.name+"/"+tableName+"/"+tableName+"_lock");
          fout << "0";
          fout.close();
          cout << "Created "+schema.name+"/"+tableName+"/"+tableName+"_lock" << endl;
          }fin.close();
        }   
    }
}