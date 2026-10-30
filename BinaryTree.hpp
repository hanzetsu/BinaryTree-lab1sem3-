#pragma once
#include <iostream>
#include <concepts>
#include <string>
#include <utility>
#include "Sequence.hpp"
#include "MutableListSequence.hpp"
#include "Exceptions.hpp"
#include "UniquePtr.hpp"

template <typename K, typename V>
struct TreeNode {
    K key;
    V value;
    UniquePtr<TreeNode<K, V>> left;
    UniquePtr<TreeNode<K, V>> right;
    
    TreeNode(K k, V v) : key(k), value(v) {}
};

template <typename K, typename V>
struct KeyValue {
    K key;
    V value;
};

template <typename K, typename V>
class BinaryTree {
private:
    UniquePtr<TreeNode<K, V>> root;

    UniquePtr<TreeNode<K, V>> copyTree(const TreeNode<K, V>* node) const {
        if (!node) return nullptr;
        
        UniquePtr<TreeNode<K, V>> newNode(new TreeNode<K, V>(node->key, node->value));
        newNode->left = copyTree(node->left.get());
        newNode->right = copyTree(node->right.get());
        return newNode;
    }

    void insertRec(UniquePtr<TreeNode<K, V>>& node, K key, V value) {
        if (!node.get()) {
            node.reset(new TreeNode<K, V>(key, value));
            return;
        }
        
        if (key < node->key) {
            insertRec(node->left, key, value);
        } else if (key > node->key) {
            insertRec(node->right, key, value);
        } else {
            node->value = value;
        }
    }

    template <typename Func>
    void traverseRec(const UniquePtr<TreeNode<K, V>>& node, Sequence<KeyValue<K, V>>* seq, Func order) const {
        if (!node.get()) return;

        auto processRoot = [&]() { seq->Append(KeyValue<K, V>{node->key, node->value}); };
        auto goLeft = [&]() { traverseRec(node->left, seq, order); };
        auto goRight = [&]() { traverseRec(node->right, seq, order); };

        order(processRoot, goLeft, goRight);
    }

    void printRec(std::ostream& os, const UniquePtr<TreeNode<K, V>>& node, const std::string& prefix, bool isTail) const {
        if (!node.get()) return;
        
        os << prefix << (isTail ? "└── " : "├── ") << node->key << " : " << node->value << "\n";
        
        bool hasLeft = (node->left.get() != nullptr);
        bool hasRight = (node->right.get() != nullptr);
        
        if (hasLeft) printRec(os, node->left, prefix + (isTail ? "    " : "│   "), !hasRight);
        if (hasRight) printRec(os, node->right, prefix + (isTail ? "    " : "│   "), true);
    }

public:
    BinaryTree() : root(nullptr) {}
    
    BinaryTree(const BinaryTree& other) : root(nullptr) { 
        root = copyTree(other.root.get()); 
    }
    
    BinaryTree& operator=(const BinaryTree& other) {
        if (this != &other) {
            root = copyTree(other.root.get());
        }
        return *this;
    }
    
    ~BinaryTree() = default;

    void insert(K key, V value) { 
        insertRec(root, key, value);
    }

    bool contains(K key) const {
        const TreeNode<K, V>* current = root.get();
        while (current) {
            if (key == current->key) return true;
            
            if (key < current->key) current = current->left.get();
            else current = current->right.get();
        }
        return false;
    }

    template <typename Func>
    Sequence<KeyValue<K, V>>* traverse(Func order) const {
        Sequence<KeyValue<K, V>>* seq = new MutableListSequence<KeyValue<K, V>>();
        traverseRec(root, seq, order);
        return seq;
    }

    template <typename Func>
    auto map(Func func) const {
        using U = decltype(func(std::declval<V>()));
        BinaryTree<K, U>* newTree = new BinaryTree<K, U>();
        auto klpOrder = [](auto root, auto left, auto right) { root(); left(); right(); };
        Sequence<KeyValue<K, V>>* elements = traverse(klpOrder);
        
        for (size_t i = 0; i < elements->GetLength(); ++i) {
            auto kv = elements->Get(i);
            newTree->insert(kv.key, func(kv.value)); 
        }
        delete elements;
        return newTree;
    }

    template <typename Predicate>
    BinaryTree<K, V>* where(Predicate pred) const {
        BinaryTree<K, V>* newTree = new BinaryTree<K, V>();
        Sequence<KeyValue<K, V>>* elements = traverse([](auto r, auto l, auto rr){ l(); r(); rr(); });
        
        for (size_t i = 0; i < elements->GetLength(); ++i) {
            auto kv = elements->Get(i);
            if (pred(kv.value)) {
                newTree->insert(kv.key, kv.value);
            }
        }
        delete elements;
        return newTree;
    }

    void merge(BinaryTree<K, V>* other) {
        Sequence<KeyValue<K, V>>* elements = other->traverse([](auto r, auto l, auto rr){ l(); r(); rr(); });
        for (size_t i = 0; i < elements->GetLength(); ++i) {
            auto kv = elements->Get(i);
            insert(kv.key, kv.value);
        }
        delete elements;
    }
    
    void clear() {
        root.reset();
    }

    friend std::ostream& operator<<(std::ostream& os, const BinaryTree& tree) {
        if (!tree.root.get()) {
            os << "(Дерево пусто)";
            return os;
        }
        os << tree.root->key << " : " << tree.root->value << "\n";
        
        bool hasLeft = (tree.root->left.get() != nullptr);
        bool hasRight = (tree.root->right.get() != nullptr);
        
        if (hasLeft) tree.printRec(os, tree.root->left, "", !hasRight);
        if (hasRight) tree.printRec(os, tree.root->right, "", true);
        
        return os;
    }
};