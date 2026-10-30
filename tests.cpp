#include <gtest/gtest.h>
#include <string>
#include "BinaryTree.hpp"

int doubleVal(int x) { return x * 2; }
bool isEven(int x) { return x % 2 == 0; }
bool isGreaterThan100(int x) { return x > 100; }

struct Student {
    int id;
    std::string name;
    Student(int i, std::string n) : id(i), name(n) {}
    
    friend std::ostream& operator<<(std::ostream& os, const Student& s) {
        return os << s.name << " (ID:" << s.id << ")";
    }
};

BinaryTree<int, int> createBasicTree() {
    BinaryTree<int, int> tree;
    tree.insert(10, 100);
    tree.insert(5, 50);
    tree.insert(15, 150);
    return tree;
}

TEST(BinaryTreeTest, EmptyTree) {
    BinaryTree<int, int> tree;
    EXPECT_FALSE(tree.contains(10));
    
    auto lkp = [](auto root, auto left, auto right) { left(); root(); right(); };
    Sequence<KeyValue<int, int>>* seq = tree.traverse(lkp);
    EXPECT_EQ(seq->GetLength(), 0);
    delete seq;
}

TEST(BinaryTreeTest, BaseTreeInsertAndContains) {
    BinaryTree<int, int> tree = createBasicTree();
    EXPECT_TRUE(tree.contains(10));
    EXPECT_TRUE(tree.contains(5));
    EXPECT_TRUE(tree.contains(15));
    EXPECT_FALSE(tree.contains(100));
}

TEST(BinaryTreeTest, AllTraversals) {
    BinaryTree<int, int> tree = createBasicTree();
    
    auto klp = [](auto root, auto left, auto right) { root(); left(); right(); };
    auto lkp = [](auto root, auto left, auto right) { left(); root(); right(); };
    
    Sequence<KeyValue<int, int>>* seqKlp = tree.traverse(klp);
    EXPECT_EQ(seqKlp->Get(0).key, 10); EXPECT_EQ(seqKlp->Get(0).value, 100);
    EXPECT_EQ(seqKlp->Get(1).key, 5);  EXPECT_EQ(seqKlp->Get(1).value, 50);
    EXPECT_EQ(seqKlp->Get(2).key, 15); EXPECT_EQ(seqKlp->Get(2).value, 150);
    delete seqKlp;
    
    Sequence<KeyValue<int, int>>* seqLkp = tree.traverse(lkp);
    EXPECT_EQ(seqLkp->Get(0).key, 5);  EXPECT_EQ(seqLkp->Get(0).value, 50);
    EXPECT_EQ(seqLkp->Get(1).key, 10); EXPECT_EQ(seqLkp->Get(1).value, 100);
    EXPECT_EQ(seqLkp->Get(2).key, 15); EXPECT_EQ(seqLkp->Get(2).value, 150);
    delete seqLkp;
}

TEST(BinaryTreeTest, MapWhere) {
    BinaryTree<int, int> tree = createBasicTree();
    
    BinaryTree<int, int>* mapped = tree.map(doubleVal);
    EXPECT_TRUE(mapped->contains(10));
    
    auto lkp = [](auto root, auto left, auto right) { left(); root(); right(); };
    Sequence<KeyValue<int, int>>* seq = mapped->traverse(lkp);
    EXPECT_EQ(seq->Get(1).value, 200);
    delete seq;
    delete mapped;
    
    BinaryTree<int, int>* filtered = tree.where(isGreaterThan100);
    EXPECT_TRUE(filtered->contains(15));
    EXPECT_FALSE(filtered->contains(10));
    EXPECT_FALSE(filtered->contains(5));
    delete filtered;
}

TEST(BinaryTreeTest, CopyAndAssignment) {
    BinaryTree<int, int> tree1 = createBasicTree();
    BinaryTree<int, int> tree2 = tree1;
    
    tree1.clear();
    EXPECT_FALSE(tree1.contains(10));
    EXPECT_TRUE(tree2.contains(10)); 
}

TEST(BinaryTreeTest, StudentByKey) {
    BinaryTree<int, Student> tree;
    
    tree.insert(3, Student(3, "Alice"));
    tree.insert(1, Student(1, "Bob"));
    
    EXPECT_TRUE(tree.contains(1));
    EXPECT_FALSE(tree.contains(2));
    
    auto lkp = [](auto root, auto left, auto right) { left(); root(); right(); };
    Sequence<KeyValue<int, Student>>* seq = tree.traverse(lkp);
    EXPECT_EQ(seq->Get(0).value.name, "Bob");
    EXPECT_EQ(seq->Get(1).value.name, "Alice");
    delete seq;
}