#include "btree.h"
#include "gtest/gtest.h"
#include <cstring>

static int totalDestroyed = 0;

class Data {
	char str_[100];
public:
	Data() {
		str_[0] = 0;
	}

	Data(const char *str) {
		strncpy(str_, str, 100);
		str_[99] = 0;
	}

	~Data() {
		totalDestroyed++;
	}

	const char* str() const { return str_; };

	int compare(Data& d2) const {
		return strcmp(str_, d2.str_);
	}
};

int compare(Data& d1, Data& d2) {
	return d1.compare(d2);
}

TEST(BTreeTest, AssignKeyValuePair) {
	BTree<int, Data> b;

	b[4] = Data("test");

	ASSERT_STREQ("test", b[4].str()) << "Expected 'test' to be in the BTree";
}

TEST(BTreeTest, AssignTwoKeyValuePairs) {
	BTree<int, Data> b;

	b[4] = Data("four");
	b[9] = Data("nine");

	ASSERT_STREQ("four", b[4].str()) << "Expected 4 to be mapped to 'four'";
	ASSERT_STREQ("nine", b[9].str()) << "Expected 9 to be mapped to 'nine'";
}

TEST(BTreeTest, Iterate) {
	BTree<int, Data> b;

	b[4] = Data("four");
	b[9] = Data("nine");
	b[3] = Data("three");
	b[5] = Data("five");

	BTree<int, Data>::iterator i = b.begin();

	ASSERT_EQ(3, i->key) << "Expected 3 to be next";
	ASSERT_STREQ("three", i->value.str()) << "Expected the correct value";
	++i;
	ASSERT_EQ(4, i->key) << "Expected 4 to be next";
	ASSERT_STREQ("four", i->value.str()) << "Expected the correct value";
	++i;
	ASSERT_EQ(5, i->key) << "Expected 5 to be next";
	ASSERT_STREQ("five", i->value.str()) << "Expected the correct value";
	++i;
	ASSERT_EQ(9, i->key) << "Expected 9 to be next";
	ASSERT_STREQ("nine", i->value.str()) << "Expected the correct value";
}

TEST(BTreeTest, Remove) {
	BTree<int, Data> b;

	b[4] = Data("four");
	b[9] = Data("nine");
	b[3] = Data("three");
	b[5] = Data("five");

	ASSERT_TRUE(b.contains(4)) << "Expected 4 to be present";

	// middle element
	b.remove(4);

	ASSERT_FALSE(b.contains(4)) << "Expected 4 to have been removed";

	// first element
	b.remove(3);

	ASSERT_FALSE(b.contains(3)) << "Expected 3 to have been removed";

	// last element
	b.remove(9);

	ASSERT_FALSE(b.contains(9)) << "Expected 9 to have been removed";

	// only element
	b.remove(5);

	ASSERT_FALSE(b.contains(5)) << "Expected 5 to have been removed";

	// unknown element
	b.remove(10);

	ASSERT_FALSE(b.contains(10)) << "Expected 10 to be absent";
}

TEST(BTreeTest, RecycleElements) {
	BTree<int, Data, 4> b;

	for (int i = 0; i < 20; i++) {
		b[4] = Data("four");
		b.remove(4);
	}
}

void allocateOnePageOfFour() {
	BTree<int, Data, 4> b;
}

TEST(BTreeTest, ValueDestructorCalled) {
	totalDestroyed = 0;
	allocateOnePageOfFour();
	ASSERT_EQ(4, totalDestroyed) << "Expected every element in the page to have been destroyed";
}

char TEST_DATA[20*2];

TEST(BTreeTest, OverflowOnePage) {
	for (int i = 0; i < 20; i++) {
		TEST_DATA[2*i] = 'A' + i;
		TEST_DATA[2*i+1] = 0;
	}

	BTree<int, Data, 16> b;

	for (int i = 0; i < 20; i++) {
		b[i] = Data(&TEST_DATA[i*2]);
	}

	for (int i = 0; i < 20; i++) {
		ASSERT_STREQ(&TEST_DATA[i*2], b[i].str()) << "Expected element " << i << " to have been stored";
	}
}

TEST(BTreeTest, ThreeLevelTree) {
	for (int i = 0; i < 20; i++) {
		TEST_DATA[2*i] = 'A' + i;
		TEST_DATA[2*i+1] = 0;
	}

	// Need 4^3 leaves to store 20 elements
	BTree<int, Data, 4> b;

	for (int i = 0; i < 20; i++) {
		b[i] = Data(&TEST_DATA[i*2]);
	}

	for (int i = 0; i < 20; i++) {
		ASSERT_STREQ(&TEST_DATA[i*2], b[i].str()) << "Expected element " << i << " to have been stored";
	}

	ASSERT_EQ(3, b.depth()) << "Expected depth to be 3";
}

TEST(BTreeTest, LotsOfNodes) {
	BTree<int, Data, 16> b;

	for (int i = 0; i < 200000; i++) {
		b[i] = Data("test");
	}

	for (int i = 0; i < 200000; i++) {
		ASSERT_TRUE(b.contains(i)) << "Expected element " << i << " to have been stored";
	}
}

TEST(BTreeTest, ReverseInsertion) {
	BTree<int, Data, 16> b;

	for (int i = 200000-1; i >=0; i--) {
		b[i] = Data("test");
	}

	for (int i = 0; i < 200000; i++) {
		ASSERT_TRUE(b.contains(i)) << "Expected element " << i << " to have been stored";
	}
}

TEST(BTreeTest, RandomInsertion) {
	BTree<int, Data, 16> b;

	for (int i = 200000-1; i >=0; i--) {
		int key = (i * 257) % 200000;
		b[key] = Data("test");
	}

	for (int i = 0; i < 200000; i++) {
		ASSERT_TRUE(b.contains(i)) << "Expected element " << i << " to have been stored";
	}
}
