template<class K, class V>
struct Element {
	Element* next;
	K key;
	V value;
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

template<class K, class V>
class BTree {
	Element<K, V> data_[4];
	Element<K, V>* free_;
	Element<K, V>* first_;

public:
	typedef BTree_iterator<K, V> iterator;
	typedef Element<K, V> element;

	BTree() {
		first_ = 0;

		free_ = &data_[0];
		for (int i = 0; i < 3; i++) {
			data_[i].next = &data_[i+1];
		}
		data_[3].next = 0;
	}

	V& operator[](const K& key) {
		element* e = find(key);
		if (e == 0) {
			e = free_;
			free_ = free_->next;
			e->key = key;
			insert(e);
		}
		return e->value;
	}

	bool contains(const K& key) {
		return find(key) != 0;
	}

	const iterator begin() {
		return iterator(first_);
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

private:
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
};
