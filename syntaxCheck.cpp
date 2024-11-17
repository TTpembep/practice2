#include "syntaxCheck.h"

bool isServiceWord(string word){
    string serviceWords = "INSERT INTO VALUES WHERE OR AND DELETE SELECT EXIT FROM";
    if (serviceWords.find(word) == string::npos) return 0;
    return 1;
}
SQLQuery syntaxCheck(string query){
    SQLQuery temp;
    temp.isRight = false;
    stringstream ss (query);
    getline(ss, temp.action, ' ');
    string token;   //Проверка синтаксиса команды вставки
    if (temp.action == "INSERT"){
        getline(ss, token, ' ');
        if (token!="INTO"){
            return temp;
        }
        getline(ss, token, ' ');
        if (isServiceWord(token)){
            return temp;
        }
        temp.tableName = token;
        getline(ss, token, ' ');
        if (token!="VALUES"){
            return temp;
        }
        getline(ss, token);
        if (token[0] != '(' || token[token.size()-1] != ')'){
            return temp;
        }
        temp.values = new fList();
        stringstream valueSS (token);
        string value;
        while (getline(valueSS,value, '\'')){
            if (value[0] != '(' && value[0] != ',' && value[0] != ')'){
                temp.values->push_back(value);
            }
        }temp.isRight = true;
        return temp;
    }   //Проверка синтаксиса команды удаления
    if (temp.action == "DELETE"){ 
        getline(ss, token, ' ');    
        if (token!="FROM"){         
            return temp;
        }
        getline(ss, token, ' ');
        if (isServiceWord(token)){
            return temp;
        }
        temp.tableName = token;
        getline(ss, token, ' ');
        if (token!="WHERE"){
            return temp;
        }
        temp.line = token;
        getline(ss,token);  //PARCER START
        stringstream valueSS (token);
        string value;
        int counter = 1;
        while (getline(valueSS,value, ' ')){
            temp.isRight = false;
            if (value.find('.') != string::npos && (counter == 1 || counter == 3)){
                temp.line += ' ' + value;
                temp.isRight = true;
                stringstream tempSS (value);
                string tempVal;
                getline(tempSS,tempVal,'.');
                if (tempVal != temp.tableName){
                    temp.isRight = false;
                    return temp;
                }
            }
            if (value == "=" && counter == 2){
                temp.line += ' ' + value;
                temp.isRight = true;
            }
            if (value[0] == '\'' && counter == 3){
                value.erase(0,1);
                value.erase(value.size()-1,1);
                temp.line += ' ' + value;
                temp.isRight = true;
            }
            if ((value == "AND" || value == "OR") && (counter == 4)){
                temp.line += ' ' + value;
                counter = 0;
                temp.isRight = true;
            }
            counter++;
        }//PARCER END
        return temp;
    }   //Проверка синтаксиса функции выборки
    if (temp.action == "SELECT"){
        temp.tables = new fList();
        temp.columns = new fList();
        while (getline(ss, token, ' ') && token != "FROM"){
            if (token.find('.') != string::npos){
                stringstream tempSS(token);
                string tempVal;
                getline(tempSS,tempVal,'.');
                if (isServiceWord(tempVal)){
                    return temp;
                }temp.tables->push_back(tempVal);
                getline(tempSS,tempVal);
                temp.columns->push_back(tempVal);
            }
        }
        if (token!="FROM"){         
            return temp;
        }getline(ss, token, ' ');
        while (getline(ss, token, ' ') && token != "WHERE"){
            temp.isRight = false;
            if (token.find(',') != string::npos){
                token.erase(token.size()-1,1);
            }if (temp.tables->find(token) == nullptr){
                return temp;
            }
            temp.isRight = true;
        }
        if (token!="WHERE"){
            return temp;
        }
        temp.line = token;
        getline(ss,token);  //PARCER START
        stringstream valueSS (token);
        string value;
        int counter = 1;
        while (getline(valueSS,value, ' ')){
            temp.isRight = false;
            if (value.find('.') != string::npos && (counter == 1 || counter == 3)){
                temp.line += ' ' + value;
                temp.isRight = true;
            }
            if (value == "=" && counter == 2){
                temp.line += ' ' + value;
                temp.isRight = true;
            }
            if (value[0] == '\'' && counter == 3){
                value.erase(0,1);
                value.erase(value.size()-1,1);
                temp.line += ' ' + value;
                temp.isRight = true;
            }
            if ((value == "AND" || value == "OR") && (counter == 4)){
                temp.line += ' ' + value;
                counter = 0;
                temp.isRight = true;
            }
            counter++;
        }//PARCER END

        return temp;
    }
    if (temp.action == "EXIT" and query == "EXIT"){ 
        temp.isRight = true;
        return temp;
    }temp.isRight = false;
    return temp;
}