//this is a double linked list implementation

#ifndef D_LINKLIST_H //prevent header files form being included multple times
#define D_LINKLIST_H

#include <deque>
#include <list>
#include <set>  // set and multiset
#include <map> //map and multimap
#include <unordered_set> // unordered set/multiset
#include <unordered_map> //unordered map/multimap
#include <iterator>
#include <algorithm>
#include <numeric> //some numeric algorithm
#include <functional>
#include <stack>

#include <sstream>
#include <memory>
#include <iostream>
#include <stdexcept>
using namespace std;

namespace dsa {
	template <class t> //forward declaration
	class DoublyLinkedList;

	template <typename t>
	class DoublyLinkedList {
	public:
		template <typename u>//internal node clas to represent sata
		class Node {
		private:
			t data_;
			Node<t>* prev_, * next_;

			//DoublyLinkedList class methods nedd
			//to access node information
			friend class DoublyLinkedList<t>;
		public:
			Node() :prev_(nullptr), next_(nullptr) {}
			Node(const t& data, Node<t>* prev, Node<t>* next) :data_(data), prev_(prev), next_(next) {}

			string toString() const {
				stringstream os;
				os << "Node (" << data_ << ")";
				return os.str();
			}

			friend ostream& operator << (ostream& strm, const Node<t>& a) {
				return strm << a.toString();
			}
		};
	private:
		int size_ = 0;
		Node<t>* head_;
		Node<t>* tail_;
	public:
		DoublyLinkedList() :size_(0), head_(nullptr), tail_(nullptr) {}

		virtual ~DoublyLinkedList() {
			clear();
		}

		//iterator class can be used to sequentially access nodes of linked list
		class Iterator {
		public:
			Iterator() noexcept : currNode_(head_) {}
			Iterator(const Node<t>* pNode) noexcept : currNode_(pNode) {}

			Iterator& operator = (Node<t>* pNode) {
				this->currNode_ = pNode;
				return *this;
			}
			//prefix ++ overload
			Iterator& operator++() {
				if (currNode_) currNode_ = currNode_->next_;
				return *this;
			}

			//postfix ++ overload
			Iterator operator++(int) {
				Iterator iterator = *this;
				++* this;
				return iterator;
			}

			bool operator != (const Iterator& iterator) {
				return currNode_ != iterator.currNode_;
			}

			t operator*() {
				return currNode_->data_;
			}
		private:
			const Node<t>* currNode_;
		};

		//empty this linked list
		void clear() {
			Node<t>* trav = head_;
			while (trav != nullptr)
			{
				Node<t>* next = trav->next_;
				delete trav;
				trav = nullptr;
				trav = next;
			}
			head_ = nullptr;
			tail_ = nullptr;
			trav = nullptr;
			size_ = 0;
		}

		//return the size of tis linked list
		int size() {
			return size_;
		}

		//return the size of this linked list
		bool isEmpty() {
			return size() == 0;
		}

		//add an element to the tail of the linked list
		void add(const t& elem) {
			addLast(elem);
		}

		//add a node to the tail of the linked list
		void addLast(const t& elem) {
			if (isEmpty()) {
				head_ = new Node<t>(elem, nullptr, nullptr);
				tail_ = head_;
			}
			else {
				tail_->next_ = new Node<t>(elem, tail_, nullptr);
				tail_ = tail_->next_;
			}
			size++;
		}

		//add an element to the beginning of this linked list
		void addFirst(const t& elem) {
			if (isEmpty()) {
				head_ = new Node<t>(elem, nullptr, nullptr);
				tail_ = head_;
			}
			else {
				head_->prev_ = new Node<t>(elem, nullptr, head_);
				head_ = head_->prev_;
			}
			size++;
		}

		//add an element at a specified index
		void addAt(int index, const t& data) {
			if (index < 0) {
				throw ("Illegal Index")
			}
			if (index == 0) {
				addFirst(data);
				return;
			}

			if (index == size_) {
				addLast(data);
				return;
			}

			Node<t>* temp = head_;
			for (int i = 0; i < index - 1; i++)
			{
				temp = temp->next_;
			}
			Node<t>* newNode = new Node<t>(data, temp, temp->next_);
			temp->next_->prev_ = newNode;
			temp->next_ = newNode;

			size_++;
		}

		//check the value of the first node if it exists
		t peekFirst() {
			if (isEmpty()) throw runtime_error("empty list");
			return head_->data_;
		}


		//check the value of the last node if it exists
		t peekLast() {
			if (isEmpty()) throw runtime_error("empty list");
			return tail_->data_;
		}

		//remove the first value at the head of the linked list
		t removeFirst() {
			if (isEmpty()) throw runtime_error("empty list"); //cant remove data from an empty list

			//extract the data at the head and move
			//the head pointer forwards one node
			t data = head_->data_;
			head_ = head_->next_;
			--size;

			if (isEmpty()) tail_ = nullptr; // if the list is empty set the tail to NULL
			else head_->prev_ = nullptr; // do a memory cleanup of the previous ndoe

			return data; // return the data that was at the first node we just removed
		}

		//remove the last value at the tail of the linked list
		t removeLast() {
			if (isEmpty()) throw runtime_error("empty error"); // cant remove data from an empty list

			//extract the data at the tail and move
			//the tail pointer backwards one node
			t data = tail_->data_;
			tail_ = tail_->prev_;
			--size_;

			if (isEmpty()) head_ = nullptr; // if the list is now empty set the head to null
			else tail_->next_ = nullptr;//do a memory cleanup if the node that was just removed

			return data; //return the data that was in the last node we just removed
		}

		//function to reverse the linked list
		void reverse() {
			{
				//initialize current, previous and
				//next pointers
				Node<t>* current = head_;
				Node<t>* prev = nullptr, * next = nullptr;

				while (current != nullptr)
				{
					next = current->next_; //store next
					current->next_ = prev; //reverse current node's pointer

					//move pointers one position ahead.
					prev = current;
					current = next;
				}
				head_ = prev;
			}
			{
				//initialize current, previous and 
				//next pointers
				Node<t>* current = tail_;
				Node<t>* prev = nullptr, * next = nullptr;

				while (current != nullptr) {
					prev = current->prev_; //store next
					current->prev_ = next; //reverse current node's pointer

					//move pointers one position ahead.
					next = current;
					current = prev;
				}
				tail_ = next;
			}
		}
	private:
		t remove_(Node<t>* node) { //remove an arbitrary node from the linked list
			//if the node to remove is somewhere either at the 
			//head or the tail handle those independently
			if (node->prev_ == nullptr) return removeFirst();
			if (node->next_ == nullptr) return removeLast();

			//make the pointers of adjacent node skip over 'node'
			node->next_->prev_ = node->prev_;
			node->prev_->next_ = node->next_;

			//temporarily store the data we want to return
			t data = node->data_;

			//memory cleanup
			//node->data=nullptr
			node->prev_ = nullptr;
			node->next_ = nullptr;
			node = nullptr;

			--size_;

			return data;
		}
	public:
		//remove a node at a particular index,
		t removeAt(int index) {
			//make sure the index provided is valid
			if (index < 0 || index >= size_) {
				throw invalid_argument("Invalid index");
			}

			int i;
			Node<t>* trav;

			//search from the front of the list
			if (index < size_ / 2) {
				for (i = 0, trav = head_; i != index; i++)
				{
					trav = trav->next_;
				}
				// search from the back of the list
			}
			else {
				for (i = size_ - 1, trav = tail_; i != index; i--)
				{
					trav = trav->prev_;
				}
			}
			return remove_(trav);
		}

		//remove a particular value in the linked list
		bool remove(const t& obj) {
			Node<t>* trav = head_;

			for (trav = head_; trav != nullptr; trav = trav->next_)
			{
				if (obj == trav->data_)
				{
					remove_(trav);
					return true;
				}
			}
			return false;
		}

		//find the index of a particular value in the linked list
		int indexOf(const t& obj) {
			int index = 0;
			Node<t>* trav = head_;

			for (; trav != nullptr; trav = trav->next_, index++)
			{
				if (obj == trav->data_)
				{
					return index;
				}
			}
			return -1;
		}

		//check if a value is contained within the linked list
		bool contains(const t& obj) {
			return indexOf(obj) != -1;
		}

		//root of linked list wrapped in iterator type
		Iterator begin() {
			return Iterator(head_);
		}

		//end of linkedlist wrapped in Iterator type
		Iterator end() {
			return Iterator(nullptr);
		}

		string toString() const {
			stringstream os;
			os << "[ ";
			Node<t>* trav = head_;
			while (trav != nullptr)
			{
				os << trav->data_;
				trav = trav->next_;
				if (trav) os << ", ";
			}
			os << " ]";
			return os.str();
		}

		friend ostream& operator<<(ostream& strm, const DoublyLinkedList<t>& a) {
			return strm << a.toString();
		}

	};
}
#endif