#include "heap.h"
#include "gtest/gtest.h"

TEST(HeapTest, CreateEmptyHeap) {
	Heap<int> h;
	ASSERT_EQ(0, h.size()) << "Expected heap to be empty";
}

TEST(HeapTest, PushOneItemAndPeek) {
	Heap<int> h;
	h.push(5);
	ASSERT_EQ(5, h.peek()) << "Expected 5 to be at the top of the heap";
}

TEST(HeapTest, PushLargeThenSmallAndPeek) {
	Heap<int> h;
	h.push(8);
	h.push(5);
	ASSERT_EQ(8, h.peek()) << "Expected 8 to be at the top of the heap";
}

TEST(HeapTest, PushSmallThenLargeAndPeek) {
	Heap<int> h;
	h.push(5);
	h.push(8);
	ASSERT_EQ(8, h.peek()) << "Expected 8 to be at the top of the heap";
}
