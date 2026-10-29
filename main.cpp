#include <iostream>
#include <chrono>
#include <random>
#include <fstream>
#include "BinaryTree.hpp"

using namespace std;

int doubleVal(int x) { return x * 2; }
bool isEven(int x) { return x % 2 == 0; }

void runBenchmark()
{
    cout << "\n=== Запуск подробного бенчмарка ===" << endl;
    cout << "Результаты сохраняются в файл benchmark.csv" << endl;

    ofstream file("benchmark.csv");
    file << "N,Insert_Time_Sec,Traverse_Time_Sec\n";

    int sizes[] = {10000, 100000, 1000000};

    for (int n : sizes)
    {
        cout << "\nТестирование " << n << " элементов..." << endl;
        BinaryTree<int> tree;
        mt19937 rng(42);
        uniform_int_distribution<int> dist(1, 100000000);

        auto start = chrono::high_resolution_clock::now();
        for (int i = 0; i < n; ++i)
        {
            tree.insert(dist(rng));
        }
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> insert_diff = end - start;

        start = chrono::high_resolution_clock::now();
        Sequence<int> *seq = tree.traverseKLP();
        end = chrono::high_resolution_clock::now();
        chrono::duration<double> traverse_diff = end - start;

        cout << "  Вставка: " << insert_diff.count() << " сек." << endl;
        cout << "  Обход:   " << traverse_diff.count() << " сек." << endl;

        file << n << "," << insert_diff.count() << "," << traverse_diff.count() << "\n";

        delete seq;
    }

    file.close();
    cout << "\nБенчмарк завершен! Данные сохранены в benchmark.csv" << endl;
}

void printMenu()
{
    cout << "\n--- Меню ---" << endl;
    cout << "1. Вставить элемент" << endl;
    cout << "2. Проверить наличие элемента" << endl;
    cout << "3. Вывести обходы (КЛП, КПЛ, ЛКП)" << endl;
    cout << "4. Демонстрация map (умножить на 2)" << endl;
    cout << "5. Демонстрация where (только четные)" << endl;
    cout << "6. Вывести дерево (графически)" << endl;
    cout << "7. Очистить дерево" << endl;
    cout << "8. Запустить подробный бенчмарк" << endl;
    cout << "0. Выход" << endl;
    cout << "Выбор: ";
}

int main()
{
    BinaryTree<int> tree;
    int choice, val;

    do
    {
        printMenu();
        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Ошибка ввода. Введите число." << endl;
            continue;
        }

        try
        {
            switch (choice)
            {
            case 1:
                cout << "Введите число: ";
                if (!(cin >> val))
                {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Ошибка ввода. Введите целое число." << endl;
                }
                else
                {
                    tree.insert(val);
                    cout << "Добавлено." << endl;
                }
                break;

            case 2:
                cout << "Введите число: ";
                if (!(cin >> val))
                {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Ошибка ввода. Введите целое число." << endl;
                }
                else
                {
                    cout << (tree.contains(val) ? "Есть в дереве" : "Не найдено") << endl;
                }
                break;
            case 3:
            {
                Sequence<int> *klp = tree.traverseKLP();
                Sequence<int> *kpl = tree.traverseKPL();
                Sequence<int> *lkp = tree.traverseLKP();

                cout << "КЛП: ";
                for (std::size_t i = 0; i < klp->GetLength(); ++i)
                    cout << klp->Get(i) << " ";
                cout << "\nКПЛ: ";
                for (std::size_t i = 0; i < kpl->GetLength(); ++i)
                    cout << kpl->Get(i) << " ";
                cout << "\nЛКП: ";
                for (std::size_t i = 0; i < lkp->GetLength(); ++i)
                    cout << lkp->Get(i) << " ";
                cout << endl;

                delete klp;
                delete kpl;
                delete lkp;
                break;
            }
            case 4:
            {
                BinaryTree<int> *mappedTree = tree.map(doubleVal);
                Sequence<int> *seq = mappedTree->traverseLKP();
                cout << "Дерево после map (ЛКП): ";
                for (std::size_t i = 0; i < seq->GetLength(); ++i)
                    cout << seq->Get(i) << " ";
                cout << endl;
                delete seq;
                delete mappedTree;
                break;
            }
            case 5:
            {
                BinaryTree<int> *filteredTree = tree.where(isEven);
                Sequence<int> *seq = filteredTree->traverseLKP();
                cout << "Дерево после where (ЛКП): ";
                for (std::size_t i = 0; i < seq->GetLength(); ++i)
                    cout << seq->Get(i) << " ";
                cout << endl;
                delete seq;
                delete filteredTree;
                break;
            }
            case 6:
                cout << "\nДерево:" << endl;
                tree.print();
                cout << endl;
                break;
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
                cout << "Неверный пункт меню." << endl;
            }
        }
        catch (const IException &e)
        {
            cout << "Ошибка: " << e.what() << endl;
        }
    } while (choice != 0);

    return 0;
}