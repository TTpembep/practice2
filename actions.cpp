#include "actions.h"

bool isUnlocked(const string& schemaName, const string& tableName){
    ifstream fin(schemaName+"/"+tableName+"/"+tableName+"_lock");
    if(!fin.is_open()) {    
        cout << "File "+ schemaName+"/"+tableName+"/"+tableName+"_lock not found. " <<endl;
        return 0;
    };
    string lockStatus;
    getline(fin,lockStatus);
    if (lockStatus == "0"){
        return 1;
    }else {
        //cout << "The table is not avaible now. Try again later. " << endl;
        return 0;
    }
}
void lock(const string& schemaName, const string& tableName){
    std::ofstream outputFile(schemaName+"/"+tableName+"/"+tableName+"_lock");
    outputFile << "1";
    outputFile.close();
}
void unlock(const string& schemaName, const string& tableName){
    std::ofstream outputFile(schemaName+"/"+tableName+"/"+tableName+"_lock");
    outputFile << "0";
    outputFile.close();
}

int getPrimaryKey(const string& tablePath ){
    string filePath = tablePath + "_pk_sequence";
    ifstream infile(filePath);
    int primaryKey = 0;
    if (infile.is_open()) {
        infile >> primaryKey;
        infile.close();
    }
    return primaryKey;
}
void updatePrimaryKey(const string& tableName, int newPrimaryKey) {
    string filePath = tableName + "_pk_sequence";
    ofstream outfile(filePath);
    if (outfile.is_open()) {
        outfile << newPrimaryKey;
        outfile.close();
    }
}
int getRowCount(const string& filePath) {
    ifstream infile(filePath);
    int count = 0;
    string line;
    while (getline(infile, line)) {
        count++;
    }
    infile.close();
    return count;
}
void insertCSV(const Schema& schema, SQLQuery& query) {
    int primaryKey = getPrimaryKey(schema.name+"/"+query.tableName+"/"+query.tableName);
    int fileCount= 1;
    string filePath = schema.name+"/"+query.tableName+"/"+to_string(fileCount)+".csv";
    int rowCount = getRowCount(filePath);
    while (rowCount >= schema.tuples_limit) {   //Проверка количества строк
        fileCount++;    //Если больше предела, переходим в новый файл
        rowCount = 0;   //Сбрасываем счетчик для нового файла
        filePath = schema.name+"/"+query.tableName+"/"+to_string(fileCount)+".csv";
        rowCount = getRowCount(filePath);
    }
    ifstream fin(filePath);  
    if(!fin.is_open()) {    
        query.message = "File " + filePath + " not found. ";
        return;
    };
    int columnCount =0;
    string columnName;
    getline(fin,columnName);
    stringstream ss(columnName);
    while(getline(ss,columnName,',')) { //Проверка на совпадение количества добавляемых
        columnCount++;                  //элементов с количеством колонок в таблице
    }
    for (Node * current = query.values->head;current!=nullptr;current=current->next,columnCount--);
    if (columnCount!=1) {
        query.message = "Insert error. Wrong amount of arguments. ";
        return;
    }
    //ios::app: Это флаг, который указывает, что файл должен быть открыт в режиме добавления (append mode).
    //Это означает, что любые данные, записанные в файл, будут добавлены в конец существующего содержимого файла,
    //а не перезаписывать его.
    ofstream outfile(filePath, ios::app);   //Открываем файл для добавления
    if (outfile.is_open()) {
        outfile << primaryKey << ",";   //Добавляем первичный ключ как первую колонку
        for (Node* current = query.values->head; current != nullptr; current = current->next) {
            outfile << current->data;   //Добавляем остальные значения
            if (current->next != nullptr) {
                outfile << ",";
            }
        }
        outfile << endl;    //Закрываем файл
        outfile.close();    //Обновляем первичный ключ в файле
        updatePrimaryKey(schema.name+"/"+query.tableName+"/"+query.tableName, primaryKey + 1);
        query.message = "Database updated succesfully. Path: " + filePath;
    } else {
        query.message = "An error occured when opening file " + filePath;
    }
    return;
}

string openedSchemeName;    //Отслеживание текущей схемы 
string openedTableName;     //и таблицы, а также
int fileCount;              //файла для выражения WHERE

string buildConditionString(Node* node) {
    stringstream ss;    //преобразуем ноду в строку
    while (node != nullptr) {
        ss << node->data << " ";
        node = node->next;
    }
    return ss.str();
}
bool isConditionTrue(const string& row, const string& columnNames, const string& line) {
    stringstream ss(line);
    string token;
    fList* tokens = new fList();    //Разбиваем запрос на отдельные части
    while (getline(ss, token, ' ')) {
        tokens->push_back(token);
    }
    if (tokens->head->data == "WHERE") {    //Проверка на "WHERE"
        Node* andOrNode = nullptr;  //Ищем "AND" или "OR" в списке
        for (Node* current = tokens->head->next; current != nullptr; current = current->next) {
            if (current->data == "AND" || current->data == "OR") {
                andOrNode = current;
                break;
            }
        }
        if (andOrNode != nullptr) { //Найден оператор "AND" или "OR"
            string operatorToken = andOrNode->data;
            //Разделяем условие на две части
            Node* leftPart = tokens->head->next;
            Node* rightPart = andOrNode->next;
            //Рекурсивно проверяем левую и правую части
            bool leftResult = isConditionTrue(row, columnNames, buildConditionString(leftPart));
            bool rightResult = isConditionTrue(row, columnNames, buildConditionString(rightPart));
            //Возвращаем результат в соответствии с оператором
            if (operatorToken == "AND") {
                return leftResult && rightResult;
            } else if (operatorToken == "OR") {
                return leftResult || rightResult;
            }
        }
    }if(tokens->head->data == "WHERE") tokens->remove("WHERE");

    if (tokens->head->next->data == "=") {
        string column = tokens->head->data;
        string table1;  //Запоминаем таблицу левой половины неравенства
        if (column.find('.') != string::npos){
            stringstream tempSS (column);
            string tempVal;
            getline(tempSS,tempVal,'.');
            table1 = tempVal;   //Записываем таблицу
            getline(tempSS,tempVal);
            column = tempVal;   //Записываем колонку
        }else{ cout << "syntax error in WHERE condition" << endl; }
        string value = tokens->head->next->next->data;  //Достаем нужное значение
        string table2 = ""; //Проверяем, это выражение колонка = колонке 
        if (value.find('.') != string::npos){   //Либо колонка = значению
            stringstream tempSS (value);
            string tempVal;
            getline(tempSS,tempVal,'.');
            table2 = tempVal;
            getline(tempSS,tempVal);
            value = tempVal;    //Записываем значение либо колонку
        }
        if (table1 == openedTableName){ //Если сравнение открытой таблицы, проверяем условие
            stringstream ss(columnNames);
            string columnName;
            int count=0;
            while(getline(ss,columnName,',') && columnName!=column){
                count++;    //Пока не дошли до нужной колонки считаем
            }
            if (table2 == openedTableName || table2 == ""){ //Случай сравнения внутри одной таблицы
                ss.str(""); //Очищаем поток
                ss << columnNames;
                columnName = " ";
                int tempCount=0;
                while(getline(ss,columnName,',')){
                    tempCount++;    
                    if (value == columnName){   //Проверка для случая колонка=колонке
                        stringstream temps(row);
                        string tempVal;
                        while (getline(temps, tempVal, ',') && count!=0){
                            tempCount--;
                        }value = tempVal;
                    }
                }
                stringstream sss(row);
                string curVal;
                while (getline(sss, curVal,',') && count!=0){
                    count--;    //Доходим до нужной колонки
                }
                if (curVal == value) {   //Проверяем совпадения значений
                    return curVal == value;
                }
            }else{  //Случай сравнения из разных таблиц
                ifstream infile (openedSchemeName+"/"+table2+"/" +to_string(fileCount)+ ".csv");
                if (infile.is_open()) {
                    string row2, columnNames2;
                    getline(infile,columnNames2);
                    while (getline(infile, row2)){
                        stringstream inss(columnNames2);
                        columnName = " ";
                        int tempCount=0;
                            while(getline(inss,columnName,',')){
                                tempCount++;    
                                if (value == columnName){  
                                    stringstream temps(row2);
                                    string tempVal;
                                while (getline(temps, tempVal, ',') && count!=0){
                                    tempCount--;
                                }value = tempVal;
                            }
                        }
                        stringstream sss(row);
                        string curVal;
                        while (getline(sss, curVal,',') && count!=0){
                            count--;    //Доходим до нужной колонки
                        }   
                        if (curVal == value) {   //Проверяем совпадения значений
                            return curVal == value;
                        }
                    }infile.close();
                }else {cout << "Syntaxys error in WHERE condition "<< endl;}
            }
        }else{  //Если открыта другая таблица пропускаем 
            tokens->clear();
            delete tokens;  //Из-за особенности построения логики OR при
            return true;    //сравнении разных таблиц работает некорректно
        }
    }
    tokens->clear();
    delete tokens;
    return false;
}
void deleteFromCSV(const Schema& schema, SQLQuery& query){
    openedSchemeName = schema.name;
    openedTableName = query.tableName;
    fileCount=1;
    string filePath = schema.name+"/"+query.tableName+"/" +to_string(fileCount)+ ".csv";
    ifstream infile(filePath);
    while(infile.is_open()){
        ofstream outfile(filePath + ".tmp", ios::out); //Открытие временного файла
        if (infile.is_open() && outfile.is_open()) {
            string row, columnNames;
            getline(infile,columnNames);    //Запись строки колонок
            outfile << columnNames << endl;
            bool isChanged = false;
            while (getline(infile, row)) {
                if (!isConditionTrue(row, columnNames, query.line)) { 
                    outfile << row << endl;    //Если условие не выполняется, записываем строку в временный файл
                }else isChanged = true;
            }
            infile.close();
            outfile.close();
            remove(filePath.c_str());  //Удаляем прошлый основной файл
            rename((filePath + ".tmp").c_str(), filePath.c_str());  //Переименовываем временный в основной
            if (isChanged) query.message = "Database updated succesfully. Path: " + filePath;
            else query.message = "Nothing has changed. ";
        }else {
            query.message = "An error occured when opening file " + filePath;
        }
        fileCount++;    //Если файлов несколько переходит к следующему
        filePath = schema.name+"/"+query.tableName+"/" +to_string(fileCount)+ ".csv";
        infile.open(filePath);
    }
    return;
}

string superPrintFunc(const string& row, const string& columnNames, const string& line) {
    stringstream ss(line);
    string token;
    fList* tokens = new fList();    //Разбиваем запрос на отдельные части
    while (getline(ss, token, ' ')) {
        tokens->push_back(token);
    }
    
    string column = tokens->head->data;
    stringstream requiredColumn(columnNames);
    string columnName;
    int count=0;
    while(getline(requiredColumn,columnName,',') && columnName!=column){
        count++;    //Пока не дошли до нужной колонки считаем
    }
    stringstream sss(row);
    sss << row; //Текущую строку вносим в поток
    string curVal;
    string curPk;
    getline(sss,curPk,',');
    count--;
    while (getline(sss, curVal,',') && count!=0){
        count--;    //Доходим до нужной колонки
    }
    tokens->clear();
    delete tokens;
    return curPk + "," + curVal;
}
void selectTables(const Schema& schema, SQLQuery& query){
    openedSchemeName = schema.name;
    string tmPath = schema.name+"/"+"tmp.csv";
    string iterPath = schema.name+"/"+"iter.csv";
    
    Node* curTab = query.tables->head;
    Node* curCol = query.columns->head;
    while (curTab != nullptr){
        openedTableName = curTab->data;
        ofstream tmpfile (tmPath, ios::out); //Создание временного файла для отборки
        fileCount=1;
        string filePath = schema.name+"/"+curTab->data+"/" +to_string(fileCount)+ ".csv";
        ifstream infile(filePath);
        ifstream iterfile(iterPath);
        while(infile.is_open()){
            if (infile.is_open() && iterfile.is_open()){    //Обработка второй и последующих таблиц
                string iterRow;
                getline(iterfile, iterRow);
                tmpfile << iterRow + "," + curTab->data + "_pk," + curCol->data << endl;
                infile.close();
                string row, columnNames;
                while(getline(iterfile,iterRow)){
                    infile.open(filePath);
                    getline(infile,columnNames);
                    while (getline(infile, row)) { 
                        if (isConditionTrue(row, columnNames, query.line)){
                            tmpfile << iterRow + "," + superPrintFunc(row, columnNames, curCol->data) << endl;
                        }
                    }
                    infile.close();
                }
                infile.close();
                iterfile.close();
            }else if(infile.is_open()){ //Обработка первой таблицы
                string row, columnNames;
                getline(infile,columnNames);
                tmpfile << curTab->data + "_pk," + curCol->data << endl;
                while (getline(infile, row)) { 
                    if (isConditionTrue(row, columnNames, query.line)){
                        tmpfile << superPrintFunc(row, columnNames, curCol->data) << endl;
                    }
                }   infile.close();
            }else {query.message = "An error occured when opening file " + filePath;}
        
            fileCount++;    //Если файлов несколько переходит к следующему
            filePath = schema.name+"/"+query.tableName+"/" +to_string(fileCount)+ ".csv";
            infile.open(filePath);
        }
        curTab = curTab->next;
        curCol = curCol->next;
        tmpfile.close();
        rename(tmPath.c_str(), iterPath.c_str());  //Переименовываем временный в основной
    }
    remove(tmPath.c_str()); //Удаление временного файла
    ifstream file(iterPath);    //Вывод итерационного файла в консоль
    string result;
    getline(file,result);
    stringstream ss (result);
    string temp;
    string msg; //Временная переменная сообщения клиенту
    while (getline(ss, temp, ',')){
        msg += temp + " ";
        getline(ss, temp, ',');
        msg += temp + "\t";
    }msg += "\n";
    while (getline(file, result)){
        stringstream ss (result);
        string temp;
        while (getline(ss, temp, ',')){
            msg += "  " + temp + "      ";
            getline(ss, temp, ',');
            msg += temp + "\t";
        }msg += "\n";
    }
    file.close();
    remove(iterPath.c_str());   //Удаление итерационного файла
    query.message = msg;
    return;
}