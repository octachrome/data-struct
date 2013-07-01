template<class K, class V>
class BTree {
	V value_;

public:
	V& operator[](const K& key) {
		return value_;
	}
};
