#include <gtest/gtest.h>
#include <libset/treeset.hpp>

TEST(TestNode, compare) {
    treeset::detail::Node<int> node1(10);
    treeset::detail::Node<int> node2(100);
    treeset::detail::Node<int> node3(10);

    ASSERT_EQ(node1, node3);
    ASSERT_TRUE(node1 == node3);
    ASSERT_TRUE(node1 < node2);
    ASSERT_FALSE(node3 != node1);
    ASSERT_TRUE(node1 >= node3);
    ASSERT_TRUE(node2 >= node3);
    ASSERT_TRUE(node3 <= node2);
    ASSERT_FALSE(node3 > node1);
}

TEST(TestSet, constructors) {
    treeset::Set<int> set1;
    treeset::Set<int> set2(10);

    ASSERT_EQ(set1.size(), set2.size() - 1);

    for (int i = 0; i < 100; i++) {
        set1.insert(i);
    }

    auto copy_set(set1);
    auto copy_iter = copy_set.begin();
    for (const auto& setElem : set1) {
        ASSERT_EQ(setElem, *copy_iter);
        copy_iter++;
    }

    auto moved_set(std::move(set1));
    auto move_iter = moved_set.begin();

    for (const auto& setElem : copy_set) {
        ASSERT_EQ(setElem, *move_iter);
        move_iter++;
    }
}

TEST(TestSet, assigments) {
    treeset::Set<int> set1;

    for (int i = 0; i < 100; i++) {
        set1.insert(i);
    }

    auto copy_set = set1;

    auto copy_iter = copy_set.begin();
    for (const auto& setElem : set1) {
        ASSERT_EQ(setElem, *copy_iter);
        copy_iter++;
    }

    auto moved_set = std::move(set1);
    auto move_iter = moved_set.begin();

    for (const auto& setElem : copy_set) {
        ASSERT_EQ(setElem, *move_iter);
        move_iter++;
    }
}

TEST(TestSet, insert) {
    treeset::Set set(5);
    set.insert(3);
    set.insert(1);
    set.insert(4);
    auto res = set.insert(2);
    ASSERT_TRUE(res.second);
    res = set.insert(2);
    ASSERT_FALSE(res.second);
    set.insert(0);

    auto iter = set.begin();

    for (int i = 0; i <= 5; i++) {
        ASSERT_EQ(i, *iter);
        iter++;
    }
}

TEST(TestSet, erase) {
    treeset::Set<int> set;

    for (int i = 0; i < 10; i++) {
        set.insert(i);
    }

    for (int i = 7; i > 2; i--) {
        set.erase(i);
    }
    set.erase(7);
    set.erase(7);
    const std::vector<int> expected_result = {0, 1, 2, 8, 9};

    int index = 0;
    for (const auto& setElem : set) {
        ASSERT_EQ(setElem, expected_result[index]);
        index++;
    }
}

TEST(TestSet, clear) {
    treeset::Set<int> set;

    for (int i = 0; i < 10; i++) {
        set.insert(i);
    }

    ASSERT_FALSE(set.empty());
    ASSERT_EQ(set.size(), 10);

    set.clear();

    ASSERT_TRUE(set.empty());
    ASSERT_EQ(set.size(), 0);

    set.clear();

    ASSERT_EQ(set.size(), 0);
}

TEST(TestSet, contains) {
    treeset::Set<int> set;

    for (int i = 0; i < 10; i++) {
        set.insert(i);
    }

    ASSERT_TRUE(set.contains(6));
    ASSERT_FALSE(set.contains(1232));
    ASSERT_FALSE(set.contains(-100000));
}

TEST(TestSet, empty) {
    treeset::Set<int> set;

    ASSERT_TRUE(set.empty());

    for (int i = 0; i < 10; i++) {
        set.insert(i);
    }

    ASSERT_FALSE(set.empty());

    set.clear();

    ASSERT_TRUE(set.empty());
}

TEST(TestSet, size) {
    treeset::Set<int> set;

    ASSERT_EQ(set.size(), 0);

    for (int i = 0; i < 10; i++) {
        set.insert(i);
    }

    ASSERT_EQ(set.size(), 10);

    set.insert(200);

    ASSERT_EQ(set.size(), 11);

    set.erase(0);
    set.erase(1);

    ASSERT_EQ(set.size(), 9);

    set.clear();

    ASSERT_EQ(set.size(), 0);
}

TEST(TestSet, find) {
    treeset::Set<int> set;

    auto res = set.find(100);

    ASSERT_EQ(res, set.end());

    for (int i = 0; i < 10; i++) {
        set.insert(i);
    }

    res = set.find(9);

    ASSERT_EQ(*res, 9);

    res = set.find(-222);

    ASSERT_EQ(res, set.end());
}

TEST(TestSet, lowerBound) {
    treeset::Set<int> set;
    for (int i = 0; i < 10; i++) {
        set.insert(i);
    }

    auto iter1 = set.find(8);
    auto iter2 = set.lower_bound(8);

    ASSERT_EQ(*iter1, *iter2);

    iter2 = set.lower_bound(200);
    ASSERT_EQ(iter2, set.end());
}

TEST(TestIterator, compare) {
    treeset::Set<int> set;

    set.insert(0);
    set.insert(1);
    set.insert(2);

    auto iter1 = set.begin();
    auto iter2 = iter1 + 2;
    auto iter3 = set.begin();

    ASSERT_TRUE(iter1 == iter3);
    ASSERT_TRUE(iter1 != iter2);
    ASSERT_FALSE(iter1 == iter2);
    ASSERT_FALSE(iter1 != iter3);
    ASSERT_TRUE(iter1 < iter2);
    ASSERT_FALSE(iter2 <= iter3);
    ASSERT_FALSE(iter1 > iter3);
    ASSERT_TRUE(iter2 >= iter1);
}

TEST(TestIterator, increment1) {
    treeset::Set<int> set;
    for (int i = 0; i < 10; i++) {
        set.insert(i);
    }

    auto iter1 = set.begin();
    auto iter2 = set.begin();

    const std::vector<int> expected_result = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    for (int i = 0; i < 10; i++) {
        ASSERT_TRUE(iter1 == iter2);
        ASSERT_FALSE(iter1 != iter2);
        ASSERT_EQ(*iter1, expected_result[i]);
        ++iter1;
        iter2++;
    }
}

TEST(TestIterator, increment2) {
    treeset::Set<int> set;
    for (int i = 0; i < 10; i++) {
        set.insert(i);
    }

    auto iter1 = set.begin();
    auto iter2 = iter1 + 5;
    iter1 += 5;
    ASSERT_EQ(*iter1, *iter2);
}

TEST(TestIterator, decrement) {
    treeset::Set<int> set;
    for (int i = 0; i < 10; i++) {
        set.insert(i);
    }

    auto iter1 = set.begin() + 6;
    auto iter2 = iter1;
    auto iter3 = iter1;

    iter1--;
    --iter1;
    iter2 -= 2;
    iter3 = iter3 - 2;

    ASSERT_TRUE(iter1 == iter2);
    ASSERT_TRUE(iter1 == iter3);
    ASSERT_FALSE(iter1 != iter2);
}

TEST(TestIterator, bidirIterator) {
    treeset::Set<int> set;
    for (int i = 0; i < 10; i++) {
        set.insert(i);
    }

    auto iter1 = set.begin() + 5;
    auto tmp = iter1;
    ASSERT_TRUE(tmp == --(++iter1));
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}