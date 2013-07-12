#define DEFAULT_PAGE_SIZE 16

template<class K, class V>
struct Element {
	Element<K, V>* next;
	K key;
	V value;
};

template<class K, class V, int PAGE_SIZE>
struct Node {
	virtual ~Node() {}

	typedef Element<K, V> element;

	virtual void init() = 0;

	virtual element* find(const K& key) = 0;

	virtual V& findOrInsert(const K& key) = 0;

	virtual void remove(const K& key) = 0;

	virtual element* first() = 0;
};

template<class K, class V, int PAGE_SIZE>
struct Page {
	typedef Element<K, V> element;

	static element* FULL;

	int size_;
	element* free_;
	element* first_;
	element data_[PAGE_SIZE];

	virtual ~Page() {}

	void p_init() {
		size_ = 0;
		first_ = 0;

		free_ = &data_[0];
		for (int i = 0; i < PAGE_SIZE; i++) {
			data_[i].next = &data_[i+1];
		}
		data_[PAGE_SIZE-1].next = 0;
	}

	element* p_find(const K& key) {
		for (element* e = first_; e != 0; e = e->next) {
			if (e->key == key) {
				return e;
			}
		}
		return 0;
	}

	element* insert(const K& key) {
		if (size_ == PAGE_SIZE-1) {
			return FULL;
		}
		size_++;

		element* e = free_;
		free_ = free_->next;
		e->key = key;

		element* i = p_findInsertPos(key);
		if (i == 0) {
			e->next = first_;
			first_ = e;
			return e;
		} else {
			e->next = i->next;
			i->next = e;
			return e;
		}
	}

	element* p_findInsertPos(const K& key) {
		if (first_ == 0 || key < first_->key) {
			return 0;
		}
		element *i = first_;
		while (i->next != 0 && i->next->key < key) {
			i = i->next;
		}
		return i;
	}

	V& p_findOrInsert(const K& key) {
		element* e = p_find(key);
		if (e == 0) {
			e = insert(key);
		}
		return e->value;
	}

	void p_remove(const K& key) {
		element* last = 0;
		for (element* e = first_; e != 0; e = e->next) {
			if (e->key == key) {
				if (last == 0) {
					first_ = e->next;
				} else {
					last->next = e->next;
				}
				e->next = free_;
				free_ = e;
				size_--;
				return;
			}
		}
	}
};

template<class K, class V, int PAGE_SIZE>
Element<K, V>* Page<K, V, PAGE_SIZE>::FULL = (Element<K, V>*) 1;

template<class K, class V, int PAGE_SIZE>
struct Leaf : Node<K, V, PAGE_SIZE>, Page<K, V, PAGE_SIZE> {
	typedef Element<K, V> element;

	void init() {
		this->p_init();
	}

	element* find(const K& key) {
		return p_find(key);
	}

	V& findOrInsert(const K& key) {
		return p_findOrInsert(key);
	}

	void remove(const K& key) {
		p_remove(key);
	}

	element* first() {
		return this->first_;
	}
};

template<class K, class V, int PAGE_SIZE>
struct Index : Node<K, V, PAGE_SIZE>, Page<K, Node<K, V, PAGE_SIZE>*, PAGE_SIZE> {
	typedef Element<K, Node<K, V, PAGE_SIZE>*> indexelement_t;
	typedef Element<K, V> element_t;

	Node<K, V, PAGE_SIZE>* infimum_;

	Index(Node<K, V, PAGE_SIZE>* infimum) {
		infimum_ = infimum;
	}

	void init() {
		this->p_init();
	}

	element_t* find(const K& key) {
		indexelement_t *el = p_findInsertPos(key);
		if (el == 0) {
			return infimum_->find(key);
		} else {
			return el->value->find(key);
		}
	}

	V& findOrInsert(const K& key) {
		indexelement_t* el = p_findInsertPos(key);
		if (el == 0) {
			return infimum_->findOrInsert(key);
		} else {
			return el->value->findOrInsert(key);
		}
	}

	void remove(const K& key) {
		indexelement_t* el = p_findInsertPos(key);
		if (el == 0) {
			return infimum_->remove(key);
		} else {
			return el->value->remove(key);
		}
	}

	element_t* first() {
		return infimum_->first();
	}
};

template<class K, class V>
class BTree_iterator {
private:
	Element<K, V>* curr_;

public:
	BTree_iterator(Element<K, V>* first) {
		curr_ = first;
	}

	const Element<K, V>& operator*() {
		return *curr_;
	}

	const BTree_iterator& operator++() {
		curr_ = curr_->next;
		return *this;
	}

	const Element<K, V>* operator->() {
		return curr_;
	}
};

template<class K, class V, int PAGE_SIZE = DEFAULT_PAGE_SIZE>
class BTree {
	Node<K, V, PAGE_SIZE>* root_;
	Node<K, V, PAGE_SIZE>* leaf_;

	void init() {
		leaf_ = new Leaf<K, V, PAGE_SIZE>;
		leaf_->init();
		root_ = new Index<K, V, PAGE_SIZE>(leaf_);
		root_->init();
	}

public:
	typedef BTree_iterator<K, V> iterator;

	BTree() {
		init();
	}

	~BTree() {
		delete leaf_;
		delete root_;
	}

	V& operator[](const K& key) {
		return root_->findOrInsert(key);
	}

	bool contains(const K& key) {
		return root_->find(key) != 0;
	}

	const iterator begin() {
		return iterator(root_->first());
	}

	void remove(const K& key) {
		root_->remove(key);
	}
};
