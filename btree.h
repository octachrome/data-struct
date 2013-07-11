#define DEFAULT_PAGE_SIZE 16

template<class K, class V>
struct Element {
	Element<K, V>* next;
	K key;
	V value;
};

template<class K, class V, int PAGE_SIZE>
struct Page {
	typedef Element<K, V> element;

	element* free_;
	element* first_;
	element data_[PAGE_SIZE];

	void init() {
		first_ = 0;

		free_ = &data_[0];
		for (int i = 0; i < PAGE_SIZE; i++) {
			data_[i].next = &data_[i+1];
		}
		data_[PAGE_SIZE-1].next = 0;
	}

	element* find(const K& key) {
		for (element* e = first_; e != 0; e = e->next) {
			if (e->key == key) {
				return e;
			}
		}
		return 0;
	}

	void insert(element *e) {
		if (first_ == 0 || e->key < first_->key) {
			e->next = first_;
			first_ = e;
			return;
		}
		element *i = first_;
		while (i->next != 0 && i->next->key < e->key) {
			i = i->next;
		}
		e->next = i->next;
		i->next = e;
	}

	V& findOrInsert(const K& key) {
		element* e = find(key);
		if (e == 0) {
			e = free_;
			free_ = free_->next;
			e->key = key;
			insert(e);
		}
		return e->value;
	}

	void remove(const K& key) {
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
				return;
			}
		}
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
	typedef Page<K, V, PAGE_SIZE> page_t;

	page_t* page_;

	void init() {
		page_ = new page_t;
		page_->init();
	}

public:
	typedef BTree_iterator<K, V> iterator;

	BTree() {
		init();
	}

	~BTree() {
		delete page_;
	}

	V& operator[](const K& key) {
		return page_->findOrInsert(key);
	}

	bool contains(const K& key) {
		return page_->find(key) != 0;
	}

	const iterator begin() {
		return iterator(page_->first_);
	}

	void remove(const K& key) {
		page_->remove(key);
	}
};
