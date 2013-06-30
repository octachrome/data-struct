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

TEST(HeapTest, PushLotsAndPeek) {
	Heap<int> h;
	h.push(5);
	h.push(8);
	h.push(3);
	h.push(2);
	h.push(15);
	h.push(1);
	ASSERT_EQ(15, h.peek()) << "Expected 15 to be at the top of the heap";
}

TEST(HeapTest, PopSingleElement) {
	Heap<int> h;
	h.push(5);
	ASSERT_EQ(1, h.size()) << "Expected size to be 1 after push";
	int i = h.pop();
	ASSERT_EQ(5, i) << "Expected 5 to be popped";
	ASSERT_EQ(0, h.size()) << "Expected size to be 0 after pop";
}

TEST(HeapTest, PopEmptyHeap) {
	Heap<int> h;
	ASSERT_THROW({
		h.pop();
	}, EmptyHeapException) << "Expected an exception";
}
