#pragma once
#include <iostream>
#include "Sequence.hpp"
#include "MutableListSequence.hpp"
#include "Exceptions.hpp"

template <typename T>
struct TreeNode
{
    T data;
    TreeNode *left;
    TreeNode *right;
    TreeNode(T val) : data(val), left(nullptr), right(nullptr) {}
};

template <typename T>
class BinaryTree
{
private:
    TreeNode<T> *root;

    void destroyTree(TreeNode<T> *node)
    {
        if (node)
        {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

    TreeNode<T> *copyTree(TreeNode<T> *node) const
    {
        if (!node)
            return nullptr;
        TreeNode<T> *newNode = new TreeNode<T>(node->data);
        newNode->left = copyTree(node->left);
        newNode->right = copyTree(node->right);
        return newNode;
    }

    TreeNode<T> *insertRec(TreeNode<T> *node, T val)
    {
        if (!node)
            return new TreeNode<T>(val);
        if (val < node->data)
            node->left = insertRec(node->left, val);
        else if (val > node->data)
            node->right = insertRec(node->right, val);
        return node;
    }

    void traverseKLP(TreeNode<T> *node, Sequence<T> *seq) const
    {
        if (!node)
            return;
        seq->Append(node->data);
        traverseKLP(node->left, seq);
        traverseKLP(node->right, seq);
    }

    void traverseKPL(TreeNode<T> *node, Sequence<T> *seq) const
    {
        if (!node)
            return;
        seq->Append(node->data);
        traverseKPL(node->right, seq);
        traverseKPL(node->left, seq);
    }

    void traverseLKP(TreeNode<T> *node, Sequence<T> *seq) const
    {
        if (!node)
            return;
        traverseLKP(node->left, seq);
        seq->Append(node->data);
        traverseLKP(node->right, seq);
    }

    void printRec(TreeNode<T> *node, const std::string &prefix, bool isTail) const
    {
        if (!node)
            return;

        std::cout << prefix << (isTail ? "└── " : "├── ") << node->data << "\n";

        bool hasLeft = (node->left != nullptr);
        bool hasRight = (node->right != nullptr);

        if (hasLeft)
        {
            printRec(node->left, prefix + (isTail ? "    " : "│   "), !hasRight);
        }

        if (hasRight)
        {
            printRec(node->right, prefix + (isTail ? "    " : "│   "), true);
        }
    }

public:
    BinaryTree() : root(nullptr) {}

    BinaryTree(const BinaryTree &other)
    {
        root = copyTree(other.root);
    }

    BinaryTree &operator=(const BinaryTree &other)
    {
        if (this != &other)
        {
            destroyTree(root);
            root = copyTree(other.root);
        }
        return *this;
    }

    ~BinaryTree()
    {
        destroyTree(root);
    }

    void insert(T val)
    {
        root = insertRec(root, val);
    }

    bool contains(T val) const
    {
        TreeNode<T> *current = root;
        while (current)
        {
            if (val == current->data)
                return true;
            if (val < current->data)
                current = current->left;
            else
                current = current->right;
        }
        return false;
    }

    Sequence<T> *traverseKLP() const
    {
        Sequence<T> *seq = new MutableListSequence<T>();
        traverseKLP(root, seq);
        return seq;
    }

    Sequence<T> *traverseKPL() const
    {
        Sequence<T> *seq = new MutableListSequence<T>();
        traverseKPL(root, seq);
        return seq;
    }

    Sequence<T> *traverseLKP() const
    {
        Sequence<T> *seq = new MutableListSequence<T>();
        traverseLKP(root, seq);
        return seq;
    }

    template <typename U>
    BinaryTree<U> *map(U (*func)(T)) const
    {
        BinaryTree<U> *newTree = new BinaryTree<U>();
        Sequence<T> *elements = traverseLKP();
        for (size_t i = 0; i < elements->GetLength(); ++i)
        {
            newTree->insert(func(elements->Get(i)));
        }
        delete elements;
        return newTree;
    }

    BinaryTree<T> *where(bool (*predicate)(T)) const
    {
        BinaryTree<T> *newTree = new BinaryTree<T>();
        Sequence<T> *elements = traverseLKP();
        for (size_t i = 0; i < elements->GetLength(); ++i)
        {
            if (predicate(elements->Get(i)))
            {
                newTree->insert(elements->Get(i));
            }
        }
        delete elements;
        return newTree;
    }

    void merge(BinaryTree<T> *other)
    {
        Sequence<T> *elements = other->traverseLKP();
        for (size_t i = 0; i < elements->GetLength(); ++i)
        {
            insert(elements->Get(i));
        }
        delete elements;
    }

    void clear()
    {
        destroyTree(root);
        root = nullptr;
    }

    void print() const
    {
        if (!root)
        {
            std::cout << "(Дерево пусто)" << std::endl;
            return;
        }
        std::cout << root->data << "\n";

        bool hasLeft = (root->left != nullptr);
        bool hasRight = (root->right != nullptr);

        if (hasLeft)
            printRec(root->left, "", !hasRight);
        if (hasRight)
            printRec(root->right, "", true);
    }
};