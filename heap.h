#include <cstring>
#include <exception>

#define INITIAL_CAPACITY 64

class EmptyHeapException : public std::exception {
	virtual const char* what() const throw() {
		return "Cannot pop() an empty Heap";
	}
} the_EmptyHeapException;

template <class T>
class Heap {
	typedef bool (*Comparator)(T value1, T value2);

	int size_;
	int capacity_;
	T* data_;
	Comparator comparator_;

	inline void init(int capacity, Comparator comparator) {
		size_ = 0;
		capacity_ = capacity;
		data_ = new T[capacity];
		comparator_ = comparator;
	}

	void populate(const T* data, int size) {
		size_ = size;
		memcpy(data_, data, size * sizeof(T));

		int deepestNodeWithChildren = (size + 1) / 2 - 1;

		for (int i = deepestNodeWithChildren; i >= 0; i--) {
			bubbleDown(i);
		}
	}

public:
	Heap() {
		init(INITIAL_CAPACITY, defaultComparator);
	}

	Heap(int capacity) {
		init(capacity, defaultComparator);
	}

	Heap(Comparator comparator) {
		init(INITIAL_CAPACITY, comparator);
	}

	Heap(int capacity, Comparator comparator) {
		init(capacity, comparator);
	}

	Heap(const T* data, int size) {
		init(size, defaultComparator);
		populate(data, size);
	}

	inline int size() const { return size_; }

	void push(T value);

	T peek() const;

	T pop() throw (EmptyHeapException);

private:
	void growIfNeeded();
	inline int computeParentIndex(int index) { return (index - 1) / 2; }
	inline int computeFirstChildIndex(int index) { return (index + 1) * 2 - 1; }
	inline int computeSecondChildIndex(int index) { return (index + 1) * 2; }

	bool lessThan(int index1, int index2);

	void swap(int index1, int index2);

	void bubbleUp(int startIndex);

	void bubbleDown(int startIndex);

	static bool defaultComparator(T value1, T value2);
};

template<class T>
void Heap<T>::push(T value)
{
	growIfNeeded();

	data_[size_] = value;
	size_++;

	bubbleUp(size_ - 1);
}

template<class T>
inline void Heap<T>::growIfNeeded() {
	if (size_ == capacity_) {
		int newCapacity = capacity_ * 2;
		T* newData = new T[newCapacity];
		memcpy(newData, data_, capacity_ * sizeof(T));
		capacity_ = newCapacity;
		data_ = newData;
	}
}

template<class T>
void Heap<T>::bubbleUp(int startIndex)
{
	while (startIndex > 0) {
		int parentIndex = computeParentIndex(startIndex);
		if (lessThan(parentIndex, startIndex)) {
			swap(parentIndex, startIndex);
		}
		startIndex = parentIndex;		
	}
}

template<class T>
inline void Heap<T>::swap(int index1, int index2)
{
	T temp = data_[index1];
	data_[index1] = data_[index2];
	data_[index2] = temp;
}

template<class T>
void Heap<T>::bubbleDown(int startIndex)
{
	while (true) {
		int firstChildIndex = computeFirstChildIndex(startIndex);
		int secondChildIndex = computeSecondChildIndex(startIndex);

		if (lessThan(startIndex, firstChildIndex)) {
			if (lessThan(startIndex, secondChildIndex)) {
				if (lessThan(firstChildIndex, secondChildIndex)) {
					swap(startIndex, secondChildIndex);
					startIndex = secondChildIndex;
				} else {
					swap(startIndex, firstChildIndex);
					startIndex = firstChildIndex;
				}
			} else {
				swap(startIndex, firstChildIndex);
				startIndex = firstChildIndex;
			}
		} else if (lessThan(startIndex, secondChildIndex)) {
			swap(startIndex, secondChildIndex);
			startIndex = secondChildIndex;
		} else {
			break;
		}
	}
}

template<class T>
inline bool Heap<T>::lessThan(int index1, int index2)
{
	if (index2 >= size_) {
		return false;
	}
	T value1 = data_[index1];
	T value2 = data_[index2];
	return !comparator_(value1, value2);
}

template<class T>
inline T Heap<T>::peek() const
{
	return data_[0];
}

template<class T>
inline T Heap<T>::pop() throw (EmptyHeapException)
{
	if (size_ == 0) {
		throw the_EmptyHeapException;
	}

	T head = data_[0];
	size_--;
	data_[0] = data_[size_];

	bubbleDown(0);

	return head;
}

template<class T>
inline bool Heap<T>::defaultComparator(T value1, T value2) {
	return value1 > value2;
}
