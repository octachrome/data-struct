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

TEST(HeapTest, PushInOrderThenPop) {
	Heap<int> h;
	h.push(1);
	h.push(2);
	h.push(3);

	int i = h.pop();
	ASSERT_EQ(3, i) << "Expected 3 to be popped";
	i = h.pop();
	ASSERT_EQ(2, i) << "Expected 2 to be popped";
	i = h.pop();
	ASSERT_EQ(1, i) << "Expected 1 to be popped";
}

TEST(HeapTest, PushReverseOrderThenPop) {
	Heap<int> h;
	h.push(3);
	h.push(2);
	h.push(1);

	int i = h.pop();
	ASSERT_EQ(3, i) << "Expected 3 to be popped";
	i = h.pop();
	ASSERT_EQ(2, i) << "Expected 2 to be popped";
	i = h.pop();
	ASSERT_EQ(1, i) << "Expected 1 to be popped";
}

TEST(HeapTest, PushLotsAndPop) {
	Heap<int> h;
	h.push(5);
	h.push(8);
	h.push(3);
	h.push(2);
	h.push(15);
	h.push(1);
	int i = h.pop();
	ASSERT_EQ(15, i) << "Expected 15 to be popped";
	i = h.pop();
	ASSERT_EQ(8, i) << "Expected 8 to be popped";
	i = h.pop();
	ASSERT_EQ(5, i) << "Expected 5 to be popped";
	i = h.pop();
	ASSERT_EQ(3, i) << "Expected 3 to be popped";
	i = h.pop();
	ASSERT_EQ(2, i) << "Expected 2 to be popped";
	i = h.pop();
	ASSERT_EQ(1, i) << "Expected 1 to be popped";
}

TEST(HeapTest, PushBeyondCapacity) {
	Heap<int> h(1);
	for (int i = 0; i < 1000; i++) {
		h.push(i);
	}
	int i = h.pop();
	ASSERT_EQ(999, i) << "Expected 999 to be popped";
	i = h.pop();
	ASSERT_EQ(998, i) << "Expected 998 to be popped";
}
