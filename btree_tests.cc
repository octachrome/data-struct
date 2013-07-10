#include "btree.h"
#include "gtest/gtest.h"
#include <cstring>

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

	const char* str() { return str_; }

	int compare(Data& d2) {
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

	BTree<int, Data>::iterator i = b.begin();

	ASSERT_EQ(3, i->key) << "Expected 3 to be next";
	++i;
	ASSERT_EQ(4, i->key) << "Expected 4 to be next";
	++i;
	ASSERT_EQ(9, i->key) << "Expected 9 to be next";
}
