#pragma once
#include <iostream>
#include <concepts>
#include <string>
#include <utility>
#include "Sequence.hpp"
#include "MutableListSequence.hpp"
#include "Exceptions.hpp"

template <typename T>
struct TreeNode {
    T data;
    TreeNode* left;
    TreeNode* right;
    TreeNode(T val) : data(val), left(nullptr), right(nullptr) {}
};

template <typename T>
struct DefaultKeyExtractor {
    T operator()(const T& item) const { return item; }
};

template <typename E, typename T>
concept KeyExtractorConcept = requires(E extractor, T item) {
    extractor(item); 
};

template <typename T, typename KeyExtractor = DefaultKeyExtractor<T>>
requires KeyExtractorConcept<KeyExtractor, T>
class BinaryTree {
private:
    TreeNode<T>* root;
    KeyExtractor getKey;

    void destroyTree(TreeNode<T>* node) {
        if (node) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

    TreeNode<T>* copyTree(TreeNode<T>* node) const {
        if (!node) return nullptr;
        TreeNode<T>* newNode = new TreeNode<T>(node->data);
        newNode->left = copyTree(node->left);
        newNode->right = copyTree(node->right);
        return newNode;
    }

    TreeNode<T>* insertRec(TreeNode<T>* node, T val) {
        if (!node) return new TreeNode<T>(val);
        
        auto valKey = getKey(val);
        auto nodeKey = getKey(node->data);
        
        if (valKey < nodeKey) {
            node->left = insertRec(node->left, val);
        } else if (valKey > nodeKey) {
            node->right = insertRec(node->right, val);
        }
        return node;
    }

    template <typename Func>
    void traverseRec(TreeNode<T>* node, Sequence<T>* seq, Func order) const {
        if (!node) return;

        auto processRoot = [&]() { seq->Append(node->data); };
        auto goLeft = [&]() { traverseRec(node->left, seq, order); };
        auto goRight = [&]() { traverseRec(node->right, seq, order); };

        order(processRoot, goLeft, goRight);
    }

    void printRec(std::ostream& os, TreeNode<T>* node, const std::string& prefix, bool isTail) const {
        if (!node) return;
        
        os << prefix << (isTail ? "└── " : "├── ") << node->data << "\n";
        
        bool hasLeft = (node->left != nullptr);
        bool hasRight = (node->right != nullptr);
        
        if (hasLeft) printRec(os, node->left, prefix + (isTail ? "    " : "│   "), !hasRight);
        if (hasRight) printRec(os, node->right, prefix + (isTail ? "    " : "│   "), true);
    }

public:
    BinaryTree(KeyExtractor extractor = KeyExtractor()) : root(nullptr), getKey(extractor) {}
    
    BinaryTree(const BinaryTree& other) : root(nullptr), getKey(other.getKey) { 
        root = copyTree(other.root); 
    }
    
    BinaryTree& operator=(const BinaryTree& other) {
        if (this != &other) {
            destroyTree(root);
            getKey = other.getKey;
            root = copyTree(other.root);
        }
        return *this;
    }
    
    ~BinaryTree() { 
        destroyTree(root); 
    }

    void insert(T val) { 
        root = insertRec(root, val); 
    }

    bool contains(T val) const {
        TreeNode<T>* current = root;
        auto valKey = getKey(val);
        
        while (current) {
            auto nodeKey = getKey(current->data);
            if (valKey == nodeKey) return true;
            
            if (valKey < nodeKey) current = current->left;
            else current = current->right;
        }
        return false;
    }

    template <typename Func>
    Sequence<T>* traverse(Func order) const {
        Sequence<T>* seq = new MutableListSequence<T>();
        traverseRec(root, seq, order);
        return seq;
    }

    template <typename Func>
    auto map(Func func) const {
        using U = decltype(func(std::declval<T>()));
        BinaryTree<U>* newTree = new BinaryTree<U>();
        
        auto lkpOrder = [](auto root, auto left, auto right) { left(); root(); right(); };
        Sequence<T>* elements = traverse(lkpOrder); 
        
        for (size_t i = 0; i < elements->GetLength(); ++i) {
            newTree->insert(func(elements->Get(i))); 
        }
        delete elements;
        return newTree;
    }

    template <typename Predicate>
    BinaryTree<T, KeyExtractor>* where(Predicate pred) const {
        BinaryTree<T, KeyExtractor>* newTree = new BinaryTree<T, KeyExtractor>(getKey);
        
        auto lkpOrder = [](auto root, auto left, auto right) { left(); root(); right(); };
        Sequence<T>* elements = traverse(lkpOrder);
        
        for (size_t i = 0; i < elements->GetLength(); ++i) {
            if (pred(elements->Get(i))) {
                newTree->insert(elements->Get(i));
            }
        }
        delete elements;
        return newTree;
    }

    void merge(BinaryTree<T, KeyExtractor>* other) {
        auto lkpOrder = [](auto root, auto left, auto right) { left(); root(); right(); };
        Sequence<T>* elements = other->traverse(lkpOrder);
        for (size_t i = 0; i < elements->GetLength(); ++i) {
            insert(elements->Get(i));
        }
        delete elements;
    }

    void clear() {
        destroyTree(root);
        root = nullptr;
    }

    friend std::ostream& operator<<(std::ostream& os, const BinaryTree& tree) {
        if (!tree.root) {
            os << "(Дерево пусто)";
            return os;
        }
        os << tree.root->data << "\n";
        
        bool hasLeft = (tree.root->left != nullptr);
        bool hasRight = (tree.root->right != nullptr);
        
        if (hasLeft) tree.printRec(os, tree.root->left, "", !hasRight);
        if (hasRight) tree.printRec(os, tree.root->right, "", true);
        
        return os;
    }
};