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
};

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
