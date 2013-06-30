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
	int size_;
	int capacity_;
	T* data_;

	inline void init(int capacity) {
		size_ = 0;
		capacity_ = capacity;
		data_ = new T[capacity];
	}

public:
	Heap() {
		init(INITIAL_CAPACITY);
	}

	Heap(int capacity) {
		init(capacity);
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
		if (data_[startIndex] > data_[parentIndex]) {
			swap(startIndex, parentIndex);
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
	return value1 < value2;
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
