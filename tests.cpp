#include <gtest/gtest.h>
#include <string>
#include "BinaryTree.hpp"

int doubleVal(int x) { return x * 2; }
bool isEven(int x) { return x % 2 == 0; }

struct Student {
    int id;
    std::string name;
    Student(int i, std::string n) : id(i), name(n) {}
};

struct StudentIdComparator {
    bool operator()(const Student& a, const Student& b) const {
        return a.id < b.id;
    }
};

struct StudentNameComparator {
    bool operator()(const Student& a, const Student& b) const {
        return a.name < b.name;
    }
};

TEST(BinaryTreeTest, EmptyTree) {
    BinaryTree<int> tree;
    EXPECT_FALSE(tree.contains(10));
    
    Sequence<int>* seq = tree.traverseKLP();
    EXPECT_EQ(seq->GetLength(), 0);
    delete seq;
}

TEST(BinaryTreeTest, InsertAndContains) {
    BinaryTree<int> tree;
    tree.insert(10);
    tree.insert(5);
    tree.insert(15);
    tree.insert(5);
    
    EXPECT_TRUE(tree.contains(10));
    EXPECT_TRUE(tree.contains(5));
    EXPECT_TRUE(tree.contains(15));
    EXPECT_FALSE(tree.contains(100));
}

TEST(BinaryTreeTest, Traversals) {
    BinaryTree<int> tree;
    tree.insert(10);
    tree.insert(5);
    tree.insert(15);
    
    Sequence<int>* klp = tree.traverseKLP();
    EXPECT_EQ(klp->GetLength(), 3);
    EXPECT_EQ(klp->Get(0), 10);
    EXPECT_EQ(klp->Get(1), 5);
    EXPECT_EQ(klp->Get(2), 15);
    delete klp;
    
    Sequence<int>* lkp = tree.traverseLKP();
    EXPECT_EQ(lkp->GetLength(), 3);
    EXPECT_EQ(lkp->Get(0), 5);
    EXPECT_EQ(lkp->Get(1), 10);
    EXPECT_EQ(lkp->Get(2), 15);
    delete lkp;
    
    Sequence<int>* kpl = tree.traverseKPL();
    EXPECT_EQ(kpl->GetLength(), 3);
    EXPECT_EQ(kpl->Get(0), 10);
    EXPECT_EQ(kpl->Get(1), 15);
    EXPECT_EQ(kpl->Get(2), 5);
    delete kpl;
}

TEST(BinaryTreeTest, Map) {
    BinaryTree<int> tree;
    tree.insert(10);
    tree.insert(5);
    tree.insert(15);
    
    BinaryTree<int>* mapped = tree.map(doubleVal);
    EXPECT_TRUE(mapped->contains(20));
    EXPECT_TRUE(mapped->contains(10));
    EXPECT_TRUE(mapped->contains(30));
    EXPECT_FALSE(mapped->contains(5));
    
    delete mapped;
}

TEST(BinaryTreeTest, Where) {
    BinaryTree<int> tree;
    tree.insert(10);
    tree.insert(5);
    tree.insert(15);
    tree.insert(20);
    
    BinaryTree<int>* filtered = tree.where(isEven);
    EXPECT_TRUE(filtered->contains(10));
    EXPECT_TRUE(filtered->contains(20));
    EXPECT_FALSE(filtered->contains(5));
    EXPECT_FALSE(filtered->contains(15));
    
    delete filtered;
}

TEST(BinaryTreeTest, CopyAndAssignment) {
    BinaryTree<int> tree1;
    tree1.insert(10);
    tree1.insert(5);
    
    BinaryTree<int> tree2 = tree1;
    EXPECT_TRUE(tree2.contains(10));
    EXPECT_TRUE(tree2.contains(5));
    
    BinaryTree<int> tree3;
    tree3.insert(99);
    tree3 = tree1;
    EXPECT_FALSE(tree3.contains(99));
    EXPECT_TRUE(tree3.contains(10));
}

TEST(BinaryTreeTest, Merge) {
    BinaryTree<int> tree1;
    tree1.insert(10);
    tree1.insert(5);
    
    BinaryTree<int> tree2;
    tree2.insert(15);
    tree2.insert(20);
    
    tree1.merge(&tree2);
    
    EXPECT_TRUE(tree1.contains(10));
    EXPECT_TRUE(tree1.contains(5));
    EXPECT_TRUE(tree1.contains(15));
    EXPECT_TRUE(tree1.contains(20));
}

TEST(BinaryTreeTest, DoubleType) {
    BinaryTree<double> tree;
    tree.insert(5.5);
    tree.insert(2.3);
    tree.insert(8.1);
    
    EXPECT_TRUE(tree.contains(5.5));
    EXPECT_FALSE(tree.contains(2.2));
}

TEST(BinaryTreeTest, StringType) {
    BinaryTree<std::string> tree;
    tree.insert("banana");
    tree.insert("apple");
    tree.insert("cherry");
    
    EXPECT_TRUE(tree.contains("apple"));
    EXPECT_FALSE(tree.contains("orange"));
}

TEST(BinaryTreeTest, CustomComparatorStudentId) {
    BinaryTree<Student, StudentIdComparator> tree;
    tree.insert(Student(3, "Alice"));
    tree.insert(Student(1, "Bob"));
    
    EXPECT_TRUE(tree.contains(Student(1, "AnyName"))); 
    EXPECT_FALSE(tree.contains(Student(2, "Bob")));
}

TEST(BinaryTreeTest, CustomComparatorStudentName) {
    BinaryTree<Student, StudentNameComparator> tree;
    tree.insert(Student(3, "Alice"));
    tree.insert(Student(1, "Bob"));
    
    EXPECT_TRUE(tree.contains(Student(99, "Alice"))); 
    EXPECT_FALSE(tree.contains(Student(1, "Charlie")));
}