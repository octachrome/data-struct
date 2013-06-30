#define INITIAL_ALLOC 64

template <class T>
class Heap {
	int size_;
	T* data_;

public:
	Heap() {
		size_ = 0;
		data_ = new T[INITIAL_ALLOC];
	}

	inline int size() { return size_; }

	void push(T value);

	T peek();

private:
	inline int computeParentIndex(int index) { return (index - 1) / 2; }

	void swap(int index1, int index2);

	void bubbleUp(int startIndex);
};

template<class T>
void Heap<T>::push(T value)
{
	data_[size_] = value;
	size_++;

	bubbleUp(size_ - 1);
}

template<class T>
inline void Heap<T>::swap(int index1, int index2)
{
	T temp = data_[index1];
	data_[index1] = data_[index2];
	data_[index2] = temp;
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
T Heap<T>::peek()
{
	return data_[0];
}
