#include <iostream>
#include <chrono>
#include <random>
#include <fstream>
#include <string>
#include <sstream>
#include "BinaryTree.hpp"

using namespace std;

int doubleVal(int x) { return x * 2; }
bool isEven(int x) { return x % 2 == 0; }

int getIntInput(const string& prompt) {
    string input;
    int value;
    while (true) {
        cout << prompt;
        getline(cin, input);
        istringstream iss(input);
        char extra;
        if (iss >> value && !(iss >> extra)) return value;
        cout << "Ошибка ввода! Пожалуйста, введите только целое число." << endl;
    }
}

void runBenchmark() {
    cout << "\n=== Запуск подробного бенчмарка ===" << endl;
    ofstream file("benchmark.csv");
    file << "N,Insert_Time_Sec,Traverse_Time_Sec\n";

    int sizes[] = {10000, 100000, 1000000};
    auto klpOrder = [](auto root, auto left, auto right) { root(); left(); right(); };
    
    for (int n : sizes) {
        cout << "\nТестирование " << n << " элементов..." << endl;
        BinaryTree<int, int> tree;
        mt19937 rng(42); 
        uniform_int_distribution<int> dist(1, 100000000);

        auto start = chrono::high_resolution_clock::now();
        for (int i = 0; i < n; ++i) {
            int val = dist(rng);
            tree.insert(val, val);
        }
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> insert_diff = end - start;
        
        start = chrono::high_resolution_clock::now();
        Sequence<KeyValue<int, int>>* seq = tree.traverse(klpOrder);
        end = chrono::high_resolution_clock::now();
        chrono::duration<double> traverse_diff = end - start;
        
        cout << "  Вставка: " << insert_diff.count() << " сек." << endl;
        cout << "  Обход:   " << traverse_diff.count() << " сек." << endl;
        file << n << "," << insert_diff.count() << "," << traverse_diff.count() << "\n";
        
        delete seq;
    }
    file.close();
}

void printMenu() {
    cout << "\n--- Меню ---" << endl;
    cout << "1. Вставить элемент" << endl;
    cout << "2. Проверить наличие элемента" << endl;
    cout << "3. Вывести дерево (графически)" << endl;
    cout << "4. Вывести обход (КЛП)" << endl;
    cout << "5. Демонстрация map (умножить на 2)" << endl;
    cout << "6. Демонстрация where (только четные)" << endl;
    cout << "7. Очистить дерево" << endl;
    cout << "8. Запустить бенчмарк" << endl;
    cout << "0. Выход" << endl;
}

int main() {
    BinaryTree<int, int> tree;
    
    tree.insert(10, 10); 
    tree.insert(5, 5); 
    tree.insert(15, 15);
    tree.insert(3, 3); 
    tree.insert(7, 7); 
    tree.insert(12, 12); 
    tree.insert(20, 20);
    
    cout << "Дерево инициализировано начальными значениями." << endl;
    int choice;
    
    do {
        printMenu();
        choice = getIntInput("Выбор: ");
        
        try {
            switch (choice) {
                case 1: {
                    int val = getIntInput("Введите число: ");
                    tree.insert(val, val);
                    cout << "Добавлено." << endl;
                    break;
                }
                case 2: {
                    int val = getIntInput("Введите число для поиска: ");
                    cout << (tree.contains(val) ? "Есть в дереве" : "Не найдено") << endl;
                    break;
                }
                case 3:
                    cout << "\nДерево:" << endl;
                    cout << tree << endl;
                    break;
                case 4: {
                    auto klpOrder = [](auto root, auto left, auto right) { root(); left(); right(); };
                    Sequence<KeyValue<int, int>>* seq = tree.traverse(klpOrder);
                    cout << "КЛП: ";
                    for (size_t i = 0; i < seq->GetLength(); ++i) {
                        cout << seq->Get(i).value << " ";
                    }
                    cout << endl;
                    delete seq;
                    break;
                }
                case 5: {
                    BinaryTree<int, int>* mappedTree = tree.map(doubleVal);
                    cout << "Дерево после map:" << endl;
                    cout << *mappedTree << endl;
                    delete mappedTree;
                    break;
                }
                case 6: {
                    BinaryTree<int, int>* filteredTree = tree.where(isEven);
                    cout << "Дерево после where:" << endl;
                    cout << *filteredTree << endl;
                    delete filteredTree;
                    break;
                }
                case 7:
                    tree.clear();
                    cout << "Дерево очищено." << endl;
                    break;
                case 8:
                    runBenchmark();
                    break;
                case 0:
                    cout << "Выход..." << endl;
                    break;
                default:
                    cout << "Неверный пункт меню. Попробуйте снова." << endl;
            }
        } catch (const IException& e) {
            cout << "Ошибка: " << e.what() << endl;
        }
    } while (choice != 0);

    return 0;
}