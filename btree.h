template<class K, class V>
struct Element {
	Element* next;
	K key;
	V value;
};

template<class K, class V>
class BTree {
	Element<K, V>* first_;

public:
	BTree() {
		first_ = 0;
	}

	V& operator[](const K& key) {
		Element<K, V>* e = find(key);
		if (e == 0) {
			e = new Element<K, V>;
			e->key = key;
			insert(e);
		}
		return e->value;
	}

private:
	Element<K, V>* find(const K& key) {
		for (Element<K, V> *e = first_; e != 0; e = e->next) {
			if (e->key == key) {
				return e;
			}
		}
		return 0;
	}

	void insert(Element<K, V> *e) {
		e->next = first_;
		first_ = e;
	}
};
