#include <gtest/gtest.h>
#include <string>
#include "BinaryTree.hpp"

int doubleVal(int x) { return x * 2; }
bool isEven(int x) { return x % 2 == 0; }

struct Student {
    int id;
    std::string name;
    Student(int i, std::string n) : id(i), name(n) {}
    
    friend std::ostream& operator<<(std::ostream& os, const Student& s) {
        return os << s.name << " (ID:" << s.id << ")";
    }
};

int extractStudentId(const Student& s) {
    return s.id;
}

BinaryTree<int> createBasicTree() {
    BinaryTree<int> tree;
    tree.insert(10);
    tree.insert(5);
    tree.insert(15);
    return tree;
}

TEST(BinaryTreeTest, EmptyTree) {
    BinaryTree<int> tree;
    EXPECT_FALSE(tree.contains(10));
    
    auto lkp = [](auto root, auto left, auto right) { left(); root(); right(); };
    Sequence<int>* seq = tree.traverse(lkp);
    EXPECT_EQ(seq->GetLength(), 0);
    delete seq;
}

TEST(BinaryTreeTest, BaseTreeInsertAndContains) {
    BinaryTree<int> tree = createBasicTree();
    EXPECT_TRUE(tree.contains(10));
    EXPECT_TRUE(tree.contains(5));
    EXPECT_TRUE(tree.contains(15));
    EXPECT_FALSE(tree.contains(100));
}

TEST(BinaryTreeTest, AllTraversals) {
    BinaryTree<int> tree = createBasicTree();
    
    auto klp = [](auto root, auto left, auto right) { root(); left(); right(); };
    auto kpl = [](auto root, auto left, auto right) { root(); right(); left(); };
    auto lkp = [](auto root, auto left, auto right) { left(); root(); right(); };
    auto lpk = [](auto root, auto left, auto right) { left(); right(); root(); };
    auto plk = [](auto root, auto left, auto right) { right(); left(); root(); };
    auto pkl = [](auto root, auto left, auto right) { right(); root(); left(); };
    
    Sequence<int>* seqKlp = tree.traverse(klp);
    EXPECT_EQ(seqKlp->Get(0), 10); EXPECT_EQ(seqKlp->Get(1), 5); EXPECT_EQ(seqKlp->Get(2), 15);
    delete seqKlp;
    
    Sequence<int>* seqKpl = tree.traverse(kpl);
    EXPECT_EQ(seqKpl->Get(0), 10); EXPECT_EQ(seqKpl->Get(1), 15); EXPECT_EQ(seqKpl->Get(2), 5);
    delete seqKpl;
    
    Sequence<int>* seqLkp = tree.traverse(lkp);
    EXPECT_EQ(seqLkp->Get(0), 5); EXPECT_EQ(seqLkp->Get(1), 10); EXPECT_EQ(seqLkp->Get(2), 15);
    delete seqLkp;
    
    Sequence<int>* seqLpk = tree.traverse(lpk);
    EXPECT_EQ(seqLpk->Get(0), 5); EXPECT_EQ(seqLpk->Get(1), 15); EXPECT_EQ(seqLpk->Get(2), 10);
    delete seqLpk;
    
    Sequence<int>* seqPlk = tree.traverse(plk);
    EXPECT_EQ(seqPlk->Get(0), 15); EXPECT_EQ(seqPlk->Get(1), 5); EXPECT_EQ(seqPlk->Get(2), 10);
    delete seqPlk;
    
    Sequence<int>* seqPkl = tree.traverse(pkl);
    EXPECT_EQ(seqPkl->Get(0), 15); EXPECT_EQ(seqPkl->Get(1), 10); EXPECT_EQ(seqPkl->Get(2), 5);
    delete seqPkl;
}

TEST(BinaryTreeTest, MapWhere) {
    BinaryTree<int> tree = createBasicTree();
    
    BinaryTree<int>* mapped = tree.map(doubleVal);
    EXPECT_TRUE(mapped->contains(20));
    EXPECT_TRUE(mapped->contains(10));
    EXPECT_TRUE(mapped->contains(30));
    delete mapped;
    
    BinaryTree<int>* filtered = tree.where(isEven);
    EXPECT_TRUE(filtered->contains(10));
    EXPECT_FALSE(filtered->contains(5));
    EXPECT_FALSE(filtered->contains(15));
    delete filtered;
}

TEST(BinaryTreeTest, CopyAndAssignment) {
    BinaryTree<int> tree1 = createBasicTree();
    
    BinaryTree<int> tree2 = tree1;
    EXPECT_TRUE(tree2.contains(10));
    EXPECT_TRUE(tree2.contains(5));
    
    tree1.clear();
    EXPECT_FALSE(tree1.contains(10));
    EXPECT_TRUE(tree2.contains(10)); 
    
    BinaryTree<int> tree3;
    tree3.insert(99);
    tree3 = tree2;
    EXPECT_FALSE(tree3.contains(99));
    EXPECT_TRUE(tree3.contains(10));
}

TEST(BinaryTreeTest, MergeTrees) {
    BinaryTree<int> tree1 = createBasicTree();
    
    BinaryTree<int> tree2;
    tree2.insert(20);
    tree2.insert(7);
    
    tree1.merge(&tree2);
    
    EXPECT_TRUE(tree1.contains(10));
    EXPECT_TRUE(tree1.contains(20));
    EXPECT_TRUE(tree1.contains(7));
}

TEST(BinaryTreeTest, ClearTree) {
    BinaryTree<int> tree = createBasicTree();
    tree.clear();
    
    EXPECT_FALSE(tree.contains(10));
    auto lkp = [](auto root, auto left, auto right) { left(); root(); right(); };
    Sequence<int>* seq = tree.traverse(lkp);
    EXPECT_EQ(seq->GetLength(), 0);
    delete seq;
}

TEST(BinaryTreeTest, StudentByKeyLambda) {
    auto idExtractor = [](const Student& s) { return s.id; };
    BinaryTree<Student, decltype(idExtractor)> tree(idExtractor);
    
    tree.insert(Student(3, "Alice"));
    tree.insert(Student(1, "Bob"));
    tree.insert(Student(1, "Duplicate Bob")); 
    
    EXPECT_TRUE(tree.contains(Student(1, "AnyName")));
    EXPECT_FALSE(tree.contains(Student(2, "Bob")));
    
    auto lkp = [](auto root, auto left, auto right) { left(); root(); right(); };
    Sequence<Student>* seq = tree.traverse(lkp);
    EXPECT_EQ(seq->GetLength(), 2); 
    delete seq;
}

TEST(BinaryTreeTest, MapToDifferentType) {
    auto idExtractor = [](const Student& s) { return s.id; };
    BinaryTree<Student, decltype(idExtractor)> tree(idExtractor);
    tree.insert(Student(3, "Alice"));
    tree.insert(Student(1, "Bob"));
    
    BinaryTree<int>* idTree = tree.map(extractStudentId);
    
    EXPECT_TRUE(idTree->contains(1));
    EXPECT_TRUE(idTree->contains(3));
    EXPECT_FALSE(idTree->contains(2));
    
    delete idTree;
}