all: run_heap_tests run_btree_tests

run_heap_tests: heap_tests
	./heap_tests

run_btree_tests: btree_tests
	./btree_tests

heap_tests: heap_tests.o gtest_main.a
	g++ -g -o $@ $^ -lpthread

btree_tests: btree_tests.o gtest_main.a
	g++ -g -o $@ $^ -lpthread

heap_tests.o: heap.h

btree_tests.o: btree.h

clean:
	-rm *.o *.a heap_tests btree_tests

GTEST_DIR = /home/chris/code/gtest-1.6.0

CXXFLAGS += -g -Wall -Wextra -fstack-protector-all
CPPFLAGS += -I$(GTEST_DIR)/include


# Builds gtest.a and gtest_main.a.

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^
