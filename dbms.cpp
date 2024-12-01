#include "DBinit.h"
#include "syntaxCheck.h"
#include "actions.h"

string dbms(string query){
    Schema schema;
    dbInit(schema);   //Функция создания и проверки наличия БД
    cout << "Database ready. \n";
    SQLQuery parsedQuery;
    parsedQuery = syntaxCheck(query);
    if (!parsedQuery.isRight) {
        parsedQuery.message = "Command is not defined. Try again. ";
        return parsedQuery.message;
    }
    if (parsedQuery.action == "INSERT"){    //Вызов функции вставки
        if (isUnlocked(schema.name, parsedQuery.tableName)){    //Проверка что таблица доступна
            lock(schema.name,parsedQuery.tableName);
            insertCSV(schema, parsedQuery);
            unlock(schema.name,parsedQuery.tableName);
        }else{  //Отправка клиенту сообщение о не доступности таблицы
            parsedQuery.message = "The table is not avaible now. Try again later. ";
        }
        return parsedQuery.message;
    }if (parsedQuery.action == "DELETE"){   //Вызов функции удаления
        if (isUnlocked(schema.name, parsedQuery.tableName)){
            lock(schema.name,parsedQuery.tableName);
            deleteFromCSV(schema, parsedQuery);
            unlock(schema.name,parsedQuery.tableName);
        }else{
            parsedQuery.message = "The table is not avaible now. Try again later. ";
        }
        return parsedQuery.message;
    }if (parsedQuery.action == "SELECT"){   //Вызов функции выборки
        Node* curTab = parsedQuery.tables->head;
        bool unlockTrigger = true;
        while (curTab != nullptr){  //Проверка блокировки двух таблиц
            unlockTrigger = isUnlocked(schema.name, curTab->data);
            curTab = curTab->next;
            if (!unlockTrigger) break;
        }
        if (unlockTrigger){ 
            curTab = parsedQuery.tables->head;
            while (curTab != nullptr){  //Блокировка двух таблиц
                lock(schema.name, curTab->data);
                curTab = curTab->next;
            }
            selectTables(schema, parsedQuery);
            curTab = parsedQuery.tables->head;
            while (curTab != nullptr){  //Разблокировка двух таблиц
                unlock(schema.name, curTab->data);
                curTab = curTab->next;
            }
        }else{
            parsedQuery.message = "The table is not avaible now. Try again later. ";
        }
        return parsedQuery.message;
    }
    //return parsedQuery.message;
    return "This is fatal logical error message";
}

/*
INSERT INTO table1 VALUES ('somedata', '12345', 'hello', 'melon')
INSERT INTO table1 VALUES ('chicken', 'world', '123', 'peace')
INSERT INTO table2 VALUES ('17', 'chicken')
DELETE FROM table1 WHERE table1.column3 = '123' AND table1.column1 = 'chicken'
DELETE FROM table1 WHERE table1.table1_pk = '4'
DELETE FROM table1 WHERE table2.column1 = '17'
SELECT table1.column1 table2.column1 FROM table1, table2
WHERE table1.column1 = table2.column2 AND table1.column2 = 'alice'
EXIT
DELETE FROM table2 WHERE table2_pk = '3'
INSERT INTO table1 VALUES ('disco', 'alice', 'is', 'world')
DELETE FROM table1 WHERE table1.table1_pk = '3'
SELECT table1.column2 table2.column2 FROM table1, table2
DELETE FROM table1 WHERE table1.column3 = table1.column4

1,somedata,12345,hello,hello
6,disco,alice,is,world

SELECT table1.column1 table2.column1 FROM table1, table2 WHERE table1.column2 = 'alice'
SELECT table1.column1 table2.column1 FROM table1, table2 WHERE table1.column3 = table1.column4 AND table2.column1 = '17'
SELECT table1.column1 table2.column1 FROM table1, table2 WHERE table2.column1 = '17' OR table2.column1 = 'wiki'
SELECT table1.column1 table2.column1 FROM table1, table2 WHERE table1.column1 = table2.column2 AND table2.column1 = '17'

SELECT table1.column1 table2.column1 FROM table1, table2 WHERE table1.column1 = 'disco' OR table2.column1 = 'wiki'

g++ client.cpp -o client
g++ server.cpp dbms.cpp DBinit.cpp syntaxCheck.cpp actions.cpp -o server
*/