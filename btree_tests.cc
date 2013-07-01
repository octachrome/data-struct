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
};

TEST(BTreeTest, AssignKeyValuePair) {
	BTree<int, Data> b;

	b[4] = Data("test");
}
