#ifndef INCLUDE_STRUCTURES_H
#define INCLUDE_STRUCTURES_H

#include <iostream>
#include <string>
using namespace std;

struct Node {   //Структура данных для узла односвязанного списка
    string data;
    Node* next;
    Node(const string& data) : data(data), next(nullptr) {}
};
struct fList {  //Структура данных для односвязанного списка
    Node* head;

    fList() : head(nullptr) {}

    ~fList() {  //Деструктор
        clear();
    }

    void insert(const string& data) {   //Добавление элемента в начало списка
        Node* newNode = new Node(data);
        newNode->next = head;
        head = newNode;
    }
    void push_back(const string& data) {//Добавление элемента в конец списка
        Node* newNode = new Node(data);
        if (head == nullptr) {
            head = newNode;
            return;
        }
        Node* current = head;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newNode;
    }
    Node* find(const string& data) {    //Поиск элемента в списке
        Node* current = head;
        while (current != nullptr) {
            if (current->data == data) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }    
    void remove(const string& data) {   //Удаление элемента из списка
        if (head == nullptr) {
            return;
        }

        if (head->data == data) {
            Node* temp = head;
            head = head->next;
            delete temp;
            return;
        }

        Node* current = head;
        while (current->next != nullptr) {
            if (current->next->data == data) {
                Node* temp = current->next;
                current->next = current->next->next;
                delete temp;
                return;
            }
            current = current->next;
        }
    }
    void print() {  //Вывод всех элементов списка
        Node* current = head;
        while (current != nullptr) {
            cout << current->data << " ";
            current = current->next;
        }
        cout << endl;
    }
    void clear() {  //Освобождение памяти
        Node* current = head;
        while (current != nullptr) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
        head = nullptr;
    }
    Node* operator[] (const int index) {    //Обращение по индексу
        if (head == nullptr) return nullptr;
        Node* p = head;
        for (int i = 0; i < index; i++) {
            p = p->next;
            if (!p) return nullptr;
        }
        return p;
    }
};
struct Schema { //Структура данных для хранения информации о схеме
    string name;
    int tuples_limit;
};
struct SQLQuery {
    string action;  //Запись действия
    string tableName;   //Запись имени таблицы при добавлении
    fList* values;  //Запись значений INSERT
    string line; //Запись строки WHERE
    fList* tables;  //Запись таблиц SELECT
    fList* columns; //Запись колонок SELECT
    bool isRight;   //Проверка синтаксиса
};

#endif