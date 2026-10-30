#pragma once
#include <iostream>
#include <concepts>
#include <string>
#include <utility>
#include "Sequence.hpp"
#include "MutableListSequence.hpp"
#include "Exceptions.hpp"
#include "UniquePtr.hpp"

template <typename T>
struct TreeNode {
    T data;
    UniquePtr<TreeNode<T>> left;
    UniquePtr<TreeNode<T>> right;
    
    TreeNode(T val) : data(val) {}
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
    UniquePtr<TreeNode<T>> root;
    KeyExtractor getKey;

    UniquePtr<TreeNode<T>> copyTree(const TreeNode<T>* node) const {
        if (!node) return nullptr;
        
        UniquePtr<TreeNode<T>> newNode(new TreeNode<T>(node->data));
        newNode->left = copyTree(node->left.get());
        newNode->right = copyTree(node->right.get());
        return newNode;
    }

    void insertRec(UniquePtr<TreeNode<T>>& node, T val) {
        if (!node.get()) {
            node.reset(new TreeNode<T>(val));
            return;
        }
        
        auto valKey = getKey(val);
        auto nodeKey = getKey(node->data);
        
        if (valKey < nodeKey) {
            insertRec(node->left, val);
        } else if (valKey > nodeKey) {
            insertRec(node->right, val);
        }
    }

    template <typename Func>
    void traverseRec(const UniquePtr<TreeNode<T>>& node, Sequence<T>* seq, Func order) const {
        if (!node.get()) return;

        auto processRoot = [&]() { seq->Append(node->data); };
        auto goLeft = [&]() { traverseRec(node->left, seq, order); };
        auto goRight = [&]() { traverseRec(node->right, seq, order); };

        order(processRoot, goLeft, goRight);
    }

    void printRec(std::ostream& os, const UniquePtr<TreeNode<T>>& node, const std::string& prefix, bool isTail) const {
        if (!node.get()) return;
        
        os << prefix << (isTail ? "└── " : "├── ") << node->data << "\n";
        
        bool hasLeft = (node->left.get() != nullptr);
        bool hasRight = (node->right.get() != nullptr);
        
        if (hasLeft) printRec(os, node->left, prefix + (isTail ? "    " : "│   "), !hasRight);
        if (hasRight) printRec(os, node->right, prefix + (isTail ? "    " : "│   "), true);
    }

public:
    BinaryTree(KeyExtractor extractor = KeyExtractor()) : root(nullptr), getKey(extractor) {}
    
    BinaryTree(const BinaryTree& other) : root(nullptr), getKey(other.getKey) { 
        root = copyTree(other.root.get()); 
    }
    
    BinaryTree& operator=(const BinaryTree& other) {
        if (this != &other) {
            getKey = other.getKey;
            root = copyTree(other.root.get());
        }
        return *this;
    }
    
    ~BinaryTree() = default;

    void insert(T val) { 
        insertRec(root, val);
    }

    bool contains(T val) const {
        const TreeNode<T>* current = root.get();
        auto valKey = getKey(val);
        
        while (current) {
            auto nodeKey = getKey(current->data);
            if (valKey == nodeKey) return true;
            
            if (valKey < nodeKey) current = current->left.get();
            else current = current->right.get();
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
        auto klpOrder = [](auto root, auto left, auto right) { root(); left(); right(); };
        Sequence<T>* elements = traverse(klpOrder);
        for (size_t i = 0; i < elements->GetLength(); ++i) {
            newTree->insert(func(elements->Get(i))); 
        }
        delete elements;
        return newTree;
    }

    template <typename Predicate>
    BinaryTree<T, KeyExtractor>* where(Predicate pred) const {
        BinaryTree<T, KeyExtractor>* newTree = new BinaryTree<T, KeyExtractor>(getKey);
        Sequence<T>* elements = traverse([](auto r, auto l, auto rr){ l(); r(); rr(); });
        
        for (size_t i = 0; i < elements->GetLength(); ++i) {
            if (pred(elements->Get(i))) {
                newTree->insert(elements->Get(i));
            }
        }
        delete elements;
        return newTree;
    }

    void merge(BinaryTree<T, KeyExtractor>* other) {
        Sequence<T>* elements = other->traverse([](auto r, auto l, auto rr){ l(); r(); rr(); });
        for (size_t i = 0; i < elements->GetLength(); ++i) {
            insert(elements->Get(i));
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
        os << tree.root->data << "\n";
        
        bool hasLeft = (tree.root->left.get() != nullptr);
        bool hasRight = (tree.root->right.get() != nullptr);
        
        if (hasLeft) tree.printRec(os, tree.root->left, "", !hasRight);
        if (hasRight) tree.printRec(os, tree.root->right, "", true);
        
        return os;
    }
};