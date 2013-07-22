#include <iostream>
#include <cstring>
#include <assert.h>

#define DEFAULT_PAGE_SIZE 16

namespace BTree_private
{
	/**
	 * A basic doubly-linked storage element which holds a key and corresponding value. Page data is stored in Elements.
	 * @tparam K type of the key
	 * @tparam V type of the value
	 */
	template<class K, class V>
	struct BTree_Element
	{
		static BTree_Element* const FULL;

		BTree_Element* prev;
		BTree_Element* next;
		K key;
		V value;
	};

	/**
	 * A marker element retuned by Page member functions to indicate that the page is full.
	 */
	template<class K, class V>
	BTree_Element<K, V>* const BTree_Element<K, V>::FULL = (BTree_Element<K, V>*) 1;

	/**
	 * A doubly-linked key-value store. The storage elements are statically allocated within the page. The elements are
	 * stored in order of increasing key. Pages are used in the BTree to store both data (in Leaves) and pointers to
	 * other pages (in Indexes).
	 * @tparam K the type of key the elements in the page will be associated with
	 * @tparam V the type of value stored in the page
	 * @tparam PAGE_SIZE the number of elements stored within the page
	 */
	template<class K, class V, int PAGE_SIZE>
	class BTree_Page
	{
	private:
		typedef BTree_Element<K, V> Element;

		/**
		 * The number of elements holding data within the page.
		 */
		int size_;
		/**
		 * A pointer to the first free element within the page. The free list is singly-linked.
		 */
		Element* free_;
		/**
		 * A pointer to the first data-holding element within the page. The data list is doubly-linked.
		 */
		Element* first_;
		/**
		 * The elements of the page. Some may be free, others may contain data.
		 */
		Element data_[PAGE_SIZE];

	public:
		/**
		 * Page constructor. Initialises the free list to contain all the elements.
		 */
		BTree_Page()
		{
			size_ = 0;
			first_ = 0;

			free_ = &data_[0];

			for (int i = 0; i < PAGE_SIZE-1; ++i) {
				data_[i].next = &data_[i+1];
			}
			data_[PAGE_SIZE-1].next = 0;
		}

		/**
		 * @return the first data-holding element (the one with the lowest key)
		 */
		Element* first() const
		{
			return first_;
		}

		/**
		 * Locate an element with the given key.
		 * @param key the key to search on
		 * @return an element containing the key, or 0 if there is none
		 */
		Element* find(const K& key) const
		{
			for (Element* e = first_; e != 0; e = e->next) {
				if (e->key == key) {
					return e;
				}
			}
			return 0;
		}

		/**
		 * Assign an element to hold a value associated with the given key, and insert it into the linked list at the
		 * correct position.
		 * @param key the key with which the value will be associated
		 * @return the element which will hold the value
		 */
		Element* insert(const K& key)
		{
			if (size_ == PAGE_SIZE) {
				return Element::FULL;
			}
			++size_;

			Element* e = free_;
			free_ = free_->next;

			e->key = key;

			Element* i = findInsertPos(key);
			if (i == 0) {
				e->next = first_;
				if (first_ != 0) {
					first_->prev = e;
				}
				e->prev = 0;
				first_ = e;
				return e;
			} else {
				e->prev = i;
				if (i->next != 0) {
					i->next->prev = e;
				}
				e->next = i->next;
				i->next = e;
				return e;
			}
		}

		/**
		 * Find the element after which the given key should be inserted. If the key is less than the smallest key in
		 * the list, returns 0.
		 * @param key the key to be inserted
		 * @return the element after which given key should be inserted, or 0 if it should be inserted before the first
		 */
		Element* findInsertPos(const K& key) const
		{
			if (first_ == 0 || key < first_->key) {
				return 0;
			}
			Element *i = first_;
			while (i->next != 0 && i->next->key <= key) {
				i = i->next;
			}
			return i;
		}

		/**
		 * Search for an element with the given key; if found, return it, otherwise insert a new element, and return
		 * that instead.
		 * @param key the key to find or insert
		 * @return an element associated with the key
		 */
		Element* findOrInsert(const K& key)
		{
			Element* e = find(key);
			if (e == 0) {
				e = insert(key);
			}
			return e;
		}

		/**
		 * Remove the a element associated with the given key, if it exists.
		 * @param key the key to remove
		 */
		void remove(const K& key)
		{
			for (Element* e = first_; e != 0; e = e->next) {
				if (e->key == key) {
					if (e->prev == 0) {
						first_ = e->next;
						if (first_ != 0) {
							first_->prev = 0;
						}
					} else {
						e->prev->next = e->next;
						if (e->next != 0) {
							e->next->prev = e->prev;
						}
					}
					e->next = free_;
					free_ = e;
					size_--;
					return;
				}
			}
		}

		/**
		 * Remove PAGE_SIZE/2 elements from this page and insert them into the given page.
		 * @param newPage the page into which the removed elements should be inserted
		 */
		void split(BTree_Page& newPage)
		{
			assert(size_ == PAGE_SIZE);

			Element* firstToRemove = this->first_;
			for (int i = 0; i < PAGE_SIZE/2; ++i) {
				firstToRemove = firstToRemove->next;
			}
			firstToRemove->prev->next = 0;

			Element* lastToRemove;
			for (Element *e = firstToRemove; e != 0; e = e->next) {
				Element* newElement = newPage.insert(e->key);
				newElement->value = e->value;
				lastToRemove = e;
			}

			lastToRemove->next = this->free_;
			free_ = firstToRemove;
			size_ = PAGE_SIZE/2;
		}

		void addAll(BTree_Page& page)
		{
			Element* e = page.first_;
			while (e != 0) {
				Element* newElement = insert(e->key);
				newElement->value = e->value;
				e = e->next;
			}
		}

		void borrow(BTree_Page& page)
		{
			Element* e = page.first_;
			Element* newElement = insert(e->key);
			newElement->value = e->value;
			page.remove(e->key);
		}

		/**
		 * @return true if the page is full
		 */
		bool full() const
		{
			return size_ == PAGE_SIZE;
		}

		/**
		 * @return the number of elements stored in the page
		 */
		int size() const
		{
			return size_;
		}

		bool valid() const
		{
			return size_ >= PAGE_SIZE/2 && size_ <= PAGE_SIZE;
		}
	};

	template<class K, class V, int PAGE_SIZE>
	class BTree_Node
	{
	private:
		typedef BTree_Element<K, V> Element;

	public:
		virtual ~BTree_Node() {}

		virtual Element* find(const K& key) const = 0;

		virtual Element* findOrInsert(const K& key) = 0;

		virtual void remove(const K& key) = 0;

		virtual Element* first() const = 0;

		virtual BTree_Node* split() = 0;

		virtual void merge(BTree_Node* node) = 0;

		virtual void borrow(BTree_Node* node) = 0;

		virtual void print(int indent) const = 0;

		virtual int depth() const = 0;

		/**
		 * The number of elements stored in *this* node. For indexes, this is the number of child nodes. For leaves,
		 * the number of data elements.
		 */
		virtual int count() const = 0;

		/**
		 * Used to reduce the height of the tree when enough elements are removed from it. If this function returns a
		 * value, it means that this node has a single child, which can be a replacement for the node itself.
		 */
		virtual BTree_Node* replaceChild() = 0;

		virtual bool valid(int depth) const = 0;
	};

	template<class K, class V, int PAGE_SIZE>
	class Leaf : public BTree_Node<K, V, PAGE_SIZE>
	{
	private:
		typedef BTree_Element<K, V> Element;
		typedef BTree_Page<K, V, PAGE_SIZE> Page;
		typedef BTree_Node<K, V, PAGE_SIZE> Node;

		Page page;

	public:
		Element* find(const K& key) const
		{
			return page.find(key);
		}

		Element* findOrInsert(const K& key)
		{
			return page.findOrInsert(key);
		}

		void remove(const K& key)
		{
			page.remove(key);
		}

		Element* first() const
		{
			return page.first();
		}

		Leaf* split()
		{
			Leaf* newLeaf = new Leaf;
			page.split(newLeaf->page);
			return newLeaf;
		}

		void merge(Node* node)
		{
			Leaf* leaf = static_cast<Leaf*>(node);
			page.addAll(leaf->page);
		}

		void borrow(Node* node)
		{
			Leaf* leaf = static_cast<Leaf*>(node);
			page.borrow(leaf->page);
		}

		void print(int indent) const
		{
			const Element* e = page.first();
			while (e != 0) {
				for (int i = 0; i < indent; ++i) {
					std::cout << "  ";
				}
				std::cout << e->key << ": (value)" << std::endl;
				e = e->next;
			}
		}

		int depth() const
		{
			return 1;
		}

		int count() const
		{
			return page.size();
		}

		Node* replaceChild()
		{
			// Leaves have no children - they cannot be replaced
			return 0;
		}

		bool valid(int depth) const
		{
			return depth == 0 || page.valid();
		}
	};

	template<class K, class V, int PAGE_SIZE>
	class Index : public BTree_Node<K, V, PAGE_SIZE>
	{
	private:
		typedef BTree_Page<K, BTree_Node<K, V, PAGE_SIZE>*, PAGE_SIZE> Page;
		typedef BTree_Element<K, BTree_Node<K, V, PAGE_SIZE>*> NodeElement;

		typedef BTree_Node<K, V, PAGE_SIZE> Node;
		typedef BTree_Element<K, V> Element;

		Page page;

	public:
		~Index()
		{
			for (NodeElement* e = page.first(); e != 0; e = e->next) {
				delete e->value;
			}
		}

		Element* find(const K& key) const
		{
			NodeElement *el = page.findInsertPos(key);
			if (el == 0) {
				el = page.first();
			}
			return el->value->find(key);
		}

		Element* findOrInsert(const K& key)
		{
			NodeElement* el = page.findInsertPos(key);
			if (el == 0) {
				el = page.first();
			}
			Node* node = el->value;
			Element* e = node->findOrInsert(key);
			if (e == Element::FULL) {
				if (page.full()) {
					return Element::FULL;
				} else {
					Node* newNode = node->split();
					page.insert(newNode->first()->key)->value = newNode;
					e = findOrInsert(key);
				}
			}
			if (key < el->key) {
				el->key = key;
			}
			return e;
		}

		void remove(const K& key)
		{
			NodeElement* el = page.findInsertPos(key);
			if (el != 0) {
				Node* node = el->value;
				node->remove(key);
				if (node->count() < PAGE_SIZE/2) {
					if (el->next != 0) {
						Node* toMerge = el->next->value;
						if (toMerge->count() > PAGE_SIZE/2) {
							node->borrow(toMerge);
							el->next->key = toMerge->first()->key;
						} else {
							page.remove(el->next->key);
							node->merge(toMerge);
							delete toMerge;
						}
					} else if (el->prev != 0) {
						Node* toMerge = el->prev->value;
						if (toMerge->count() > PAGE_SIZE/2) {
							node->borrow(toMerge);
							el->prev->key = toMerge->first()->key;
							el->key = node->first()->key;
						} else {
							page.remove(el->key);
							toMerge->merge(node);
							delete node;
						}
					}
				}
			}
		}

		Element* first() const
		{
			return page.first()->value->first();
		}

		Index* split()
		{
			Index* newIndex = new Index;
			page.split(newIndex->page);
			return newIndex;
		}

		void merge(Node*)
		{

		}

		void borrow(Node*)
		{

		}

		void addPage(Node* p)
		{
			NodeElement* el = page.insert(p->first()->key);
			el->value = p;
		}

		void print(int indent) const
		{
			NodeElement* ie = page.first();
			while (ie != 0) {
				for (int i = 0; i < indent; i++) {
					std::cout << "  ";
				}
				std::cout << ie->key << ":" << std::endl;
				ie->value->print(indent + 1);
				ie = ie->next;
			}
		}

		int depth() const
		{
			return 1 + page.first()->value->depth();
		}

		int count() const
		{
			return page.size();
		}

		Node* replaceChild()
		{
			if (page.size() == 1) {
				NodeElement* el = page.first();
				page.remove(el->key);
				return el->value;
			} else {
				return 0;
			}
		}

		bool valid(int depth) const
		{
			if (depth > 0 && !page.valid()) {
				return false;
			}

			NodeElement* ie = page.first();
			while (ie != 0) {
				if (!ie->value->valid(depth + 1)) {
					return false;
				}
				ie = ie->next;
			}

			return true;
		}
	};

	template<class K, class V>
	class BTree_Iterator
	{
	private:
		typedef BTree_Element<K, V> Element;

		Element* curr_;

	public:
		BTree_Iterator(Element* first)
		{
			curr_ = first;
		}

		const Element& operator*()
		{
			return *curr_;
		}

		const BTree_Iterator& operator++()
		{
			curr_ = curr_->next;
			return *this;
		}

		const Element* operator->()
		{
			return curr_;
		}
	};
}; // namespace BTree_private

template<class K, class V, int PAGE_SIZE = DEFAULT_PAGE_SIZE>
class BTree
{
	typedef BTree_private::BTree_Node<K, V, PAGE_SIZE> Node;
	typedef BTree_private::Leaf<K, V, PAGE_SIZE> Leaf;
	typedef BTree_private::Index<K, V, PAGE_SIZE> Index;
	typedef BTree_private::BTree_Element<K, V> Element;

	Node* root_;
	bool asserts_;

	void assertValid()
	{
		if (asserts_) {
			assert(valid());
		}
	}

public:
	typedef BTree_private::BTree_Iterator<K, V> Iterator;

	BTree()
	{
		root_ = new Leaf;
		asserts_ = false;
	}

	~BTree()
	{
		delete root_;
	}

	V& operator[](const K& key)
	{
		Element *e = root_->findOrInsert(key);
		if (e == Element::FULL) {
			Node* newPage = root_->split();
			Index* newRoot = new Index;
			newRoot->addPage(root_);
			newRoot->addPage(newPage);
			root_ = newRoot;
			e = root_->findOrInsert(key);
		}
		assertValid();
		return e->value;
	}

	bool contains(const K& key)
	{
		return root_->find(key) != 0;
	}

	const Iterator begin()
	{
		return Iterator(root_->first());
	}

	void remove(const K& key)
	{
		root_->remove(key);
		Node* newRoot = root_->replaceChild();
		if (newRoot != 0) {
			delete root_;
			root_ = newRoot;
		}
		assertValid();
	}

	void print()
	{
		root_->print(0);
	}

	int depth()
	{
		return root_->depth();
	}

	void enableAsserts(bool enabled)
	{
		asserts_ = enabled;
	}

	bool valid()
	{
		return root_->valid(0);
	}
};
