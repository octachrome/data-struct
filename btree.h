#include <iostream>

#define DEFAULT_PAGE_SIZE 16

namespace BTree_private
{
	template<class K, class V>
	struct BTree_Element {
		static BTree_Element* FULL;

		BTree_Element* next;
		K key;
		V value;
	};

	template<class K, class V>
	BTree_Element<K, V>* BTree_Element<K, V>::FULL = (BTree_Element<K, V>*) 1;

	template<class K, class V, int PAGE_SIZE>
	struct Page {
		typedef BTree_Element<K, V> Element;

		int size_;
		Element* free_;
		Element* first_;
		Element data_[PAGE_SIZE];

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

		Element* p_find(const K& key) {
			for (Element* e = first_; e != 0; e = e->next) {
				if (e->key == key) {
					return e;
				}
			}
			return 0;
		}

		Element* insert(const K& key) {
			if (size_ == PAGE_SIZE) {
				return Element::FULL;
			}
			size_++;

			Element* e = free_;
			free_ = free_->next;
			e->key = key;

			Element* i = p_findInsertPos(key);
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

		Element* p_findInsertPos(const K& key) {
			if (first_ == 0 || key < first_->key) {
				return 0;
			}
			Element *i = first_;
			while (i->next != 0 && i->next->key <= key) {
				i = i->next;
			}
			return i;
		}

		Element* p_findOrInsert(const K& key) {
			Element* e = p_find(key);
			if (e == 0) {
				e = insert(key);
			}
			return e;
		}

		void p_remove(const K& key) {
			Element* last = 0;
			for (Element* e = first_; e != 0; e = e->next) {
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
			Element* lastToKeep;
			Element* firstToRemove = this->first_;
			for (int i = 0; i < PAGE_SIZE/2; i++) {
				lastToKeep = firstToRemove;
				firstToRemove = firstToRemove->next;
			}
			lastToKeep->next = 0;

			Element* lastToRemove;
			for (Element *e = firstToRemove; e != 0; e = e->next) {
				newPage->insert(e->key)->value = e->value;
				lastToRemove = e;
			}

			lastToRemove->next = this->free_;
			free_ = firstToRemove;
			size_ = PAGE_SIZE/2;
		}
	};

	template<class K, class V, int PAGE_SIZE>
	class BTree_Node {
	private:
		typedef BTree_Element<K, V> Element;
		typedef BTree_Node<K, V, PAGE_SIZE> Self;

	public:
		virtual ~BTree_Node() {}

		virtual Element* find(const K& key) = 0;

		virtual Element* findOrInsert(const K& key) = 0;

		virtual void remove(const K& key) = 0;

		virtual Element* first() = 0;

		virtual Self* split() = 0;

		virtual void print(int indent) = 0;

		virtual int depth() = 0;
	};

	template<class K, class V, int PAGE_SIZE>
	struct Leaf : BTree_Node<K, V, PAGE_SIZE>, Page<K, V, PAGE_SIZE> {
		typedef BTree_Element<K, V> Element;

		Element* find(const K& key) {
			return p_find(key);
		}

		Element* findOrInsert(const K& key) {
			return p_findOrInsert(key);
		}

		void remove(const K& key) {
			p_remove(key);
		}

		Element* first() {
			return this->first_;
		}

		Leaf* split() {
			Leaf* newLeaf = new Leaf;
			p_split(newLeaf);
			return newLeaf;
		}

		void print(int indent) {
			Element* e = this->first_;
			while (e != 0) {
				for (int i = 0; i < indent; i++) {
					std::cout << "  ";
				}
				std::cout << e->key << ": (value)" << std::endl;
				e = e->next;
			}
		}

		int depth() {
			return 1;
		}
	};

	template<class K, class V, int PAGE_SIZE>
	struct Index : BTree_Node<K, V, PAGE_SIZE>, Page<K, BTree_Node<K, V, PAGE_SIZE>*, PAGE_SIZE> {
		typedef BTree_Node<K, V, PAGE_SIZE> Node;
		typedef BTree_Element<K, BTree_Node<K, V, PAGE_SIZE>*> NodeElement;
		typedef BTree_Element<K, V> Element;

		Element* find(const K& key) {
			NodeElement *el = p_findInsertPos(key);
			if (el == 0) {
				el = this->first_;
			}
			return el->value->find(key);
		}

		Element* findOrInsert(const K& key) {
			NodeElement* el = p_findInsertPos(key);
			if (el == 0) {
				el = this->first_;
			}
			Node* node = el->value;
			Element* e = node->findOrInsert(key);
			if (e == Element::FULL) {
				if (this->size_ == PAGE_SIZE) {
					return Element::FULL;
				} else {
					Node* newNode = node->split();
					insert(newNode->first()->key)->value = newNode;
					e = findOrInsert(key);
				}
			}
			if (key < el->key) {
				el->key = key;
			}
			return e;
		}

		void remove(const K& key) {
			NodeElement* el = p_findInsertPos(key);
			if (el == 0) {
				el = this->first_;
			}
			return el->value->remove(key);
		}

		Element* first() {
			return this->first_->value->first();
		}

		Index* split() {
			Index* newIndex = new Index;
			p_split(newIndex);
			return newIndex;
		}

		void addPage(Node* page) {
			insert(page->first()->key)->value = page;
		}

		void print(int indent) {
			NodeElement* ie = this->first_;
			while (ie != 0) {
				for (int i = 0; i < indent; i++) {
					std::cout << "  ";
				}
				std::cout << ie->key << ":" << std::endl;
				ie->value->print(indent + 1);
				ie = ie->next;
			}
		}

		int depth() {
			return 1 + this->first_->value->depth();
		}
	};

	template<class K, class V>
	class BTree_Iterator {
	private:
		typedef BTree_Element<K, V> Element;

		Element* curr_;

	public:
		BTree_Iterator(Element* first) {
			curr_ = first;
		}

		const Element& operator*() {
			return *curr_;
		}

		const BTree_Iterator& operator++() {
			curr_ = curr_->next;
			return *this;
		}

		const Element* operator->() {
			return curr_;
		}
	};
}; // namespace BTree_private

template<class K, class V, int PAGE_SIZE = DEFAULT_PAGE_SIZE>
class BTree {
	typedef BTree_private::BTree_Node<K, V, PAGE_SIZE> Node;
	typedef BTree_private::Leaf<K, V, PAGE_SIZE> Leaf;
	typedef BTree_private::Index<K, V, PAGE_SIZE> Index;
	typedef BTree_private::BTree_Element<K, V> Element;

	Node* root_;

public:
	typedef BTree_private::BTree_Iterator<K, V> Iterator;

	BTree() {
		root_ = new Leaf;
	}

	~BTree() {
		delete root_;
	}

	V& operator[](const K& key) {
		Element *e = root_->findOrInsert(key);
		if (e == Element::FULL) {
			Node* newPage = root_->split();
			Index* newRoot = new Index;
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

	const Iterator begin() {
		return Iterator(root_->first());
	}

	void remove(const K& key) {
		root_->remove(key);
	}

	void print() {
		root_->print(0);
	}

	int depth() {
		return root_->depth();
	}
};
