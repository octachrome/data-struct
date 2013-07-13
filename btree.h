#define DEFAULT_PAGE_SIZE 16

template<class K, class V>
struct Element {
	static Element* FULL;

	Element<K, V>* next;
	K key;
	V value;
};

template<class K, class V>
Element<K, V>* Element<K, V>::FULL = (Element<K, V>*) 1;

template<class K, class V, int PAGE_SIZE>
struct Node {
	virtual ~Node() {}

	typedef Element<K, V> element;

	virtual element* find(const K& key) = 0;

	virtual element* findOrInsert(const K& key) = 0;

	virtual void remove(const K& key) = 0;

	virtual element* first() = 0;

	virtual Node<K, V, PAGE_SIZE>* split() = 0;
};

template<class K, class V, int PAGE_SIZE>
struct Page {
	typedef Element<K, V> element_t;

	int size_;
	element_t* free_;
	element_t* first_;
	element_t data_[PAGE_SIZE];

	Page() {
		size_ = 0;
		first_ = 0;

		free_ = &data_[0];
		for (int i = 0; i < PAGE_SIZE; i++) {
			data_[i].next = &data_[i+1];
		}
		data_[PAGE_SIZE-1].next = 0;
	}

	virtual ~Page() {}

	element_t* p_find(const K& key) {
		for (element_t* e = first_; e != 0; e = e->next) {
			if (e->key == key) {
				return e;
			}
		}
		return 0;
	}

	element_t* insert(const K& key) {
		if (size_ == PAGE_SIZE-1) {
			return element_t::FULL;
		}
		size_++;

		element_t* e = free_;
		free_ = free_->next;
		e->key = key;

		element_t* i = p_findInsertPos(key);
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

	element_t* p_findInsertPos(const K& key) {
		if (first_ == 0 || key < first_->key) {
			return 0;
		}
		element_t *i = first_;
		while (i->next != 0 && i->next->key < key) {
			i = i->next;
		}
		return i;
	}

	element_t* p_findOrInsert(const K& key) {
		element_t* e = p_find(key);
		if (e == 0) {
			e = insert(key);
		}
		return e;
	}

	void p_remove(const K& key) {
		element_t* last = 0;
		for (element_t* e = first_; e != 0; e = e->next) {
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

	void p_split(Page* newPage) {
		element_t* middle = this->first_;
		for (int i = 0; i < PAGE_SIZE/2; i++) {
			middle = middle->next;
		}

		element_t* last;
		for (element_t *e = middle; e != 0; e = e->next) {
			last = e;
			newPage->insert(e->key)->value = e->value;
		}

		last->next = this->free_;
		free_ = middle;
		size_ = PAGE_SIZE/2;
	}
};

template<class K, class V, int PAGE_SIZE>
struct Leaf : Node<K, V, PAGE_SIZE>, Page<K, V, PAGE_SIZE> {
	typedef Element<K, V> element_t;

	element_t* find(const K& key) {
		return p_find(key);
	}

	element_t* findOrInsert(const K& key) {
		return p_findOrInsert(key);
	}

	void remove(const K& key) {
		p_remove(key);
	}

	element_t* first() {
		return this->first_;
	}

	Leaf* split() {
		Leaf* newLeaf = new Leaf;
		p_split(newLeaf);
		return newLeaf;
	}
};

template<class K, class V, int PAGE_SIZE>
struct Index : Node<K, V, PAGE_SIZE>, Page<K, Node<K, V, PAGE_SIZE>*, PAGE_SIZE> {
	typedef Element<K, Node<K, V, PAGE_SIZE>*> indexelement_t;
	typedef Element<K, V> element_t;

	element_t* find(const K& key) {
		indexelement_t *el = p_findInsertPos(key);
		if (el == 0) {
			el = this->first_;
		}
		return el->value->find(key);
	}

	element_t* findOrInsert(const K& key) {
		indexelement_t* el = p_findInsertPos(key);
		if (el == 0) {
			el = this->first_;
		}
		Node<K, V, PAGE_SIZE>* node = el->value;
		element_t* e = node->findOrInsert(key);
		if (e == element_t::FULL) {
			if (this->size_ == PAGE_SIZE-1) {
				return element_t::FULL;
			} else {
				Node<K, V, PAGE_SIZE>* newNode = node->split();
				insert(newNode->first()->key)->value = newNode;
				return findOrInsert(key);
			}
		}
		return e;
	}

	void remove(const K& key) {
		indexelement_t* el = p_findInsertPos(key);
		if (el == 0) {
			el = this->first_;
		}
		return el->value->remove(key);
	}

	element_t* first() {
		return this->first_->value->first();
	}

	Index* split() {
		Index* newIndex = new Index;
		p_split(newIndex);
		return newIndex;
	}

	void addPage(Node<K, V, PAGE_SIZE>* page) {
		insert(page->first()->key)->value = page;
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

public:
	typedef BTree_iterator<K, V> iterator;

	BTree() {
		root_ = new Leaf<K, V, PAGE_SIZE>;
	}

	~BTree() {
		delete root_;
	}

	V& operator[](const K& key) {
		Element<K, V> *e = root_->findOrInsert(key);
		if (e == Element<K, V>::FULL) {
			Node<K, V, PAGE_SIZE>* newPage = root_->split();
			Index<K, V, PAGE_SIZE>* newRoot = new Index<K, V, PAGE_SIZE>();
			newRoot->addPage(root_);
			newRoot->addPage(newPage);
			root_ = newRoot;
			e = root_->findOrInsert(key);
		}
		return e->value;
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
