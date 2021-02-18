
//base class for hashtables with an open adressing collision resolution method such as linear
//probing, quadratic probing and double hashing

#ifndef D_HASHTABLEOPENADRESSING_H
#define D_HASHTABLEOPENADRESSING_H

#include <vector>
#include <deque>
#include <list>
#include <set> //set and multiset
#include <map>//map and multimap
#include <unordered_set>//unordered set/map
#include <unordered_map>//unordered map/multimap
#include <iterator>
#include <algorithm>
#include <numeric> //some numeric algotithm
#include <functional>
#include <stack>

#include <sstream>
#include <memory>
#include <cmath>

using namespace std;

namespace dsa {
	static const int DEFAULT_CAPACITY = 7;
	static const double DEFAULT_LOAD_FACTOR = 0.65;

	template<class KEY, class VALUE> class HashTableOpenAdressingBase
	{
	protected:
		//this is the linear constant used in the linear probing, it can be
		//any positive number/ Tha table capacity will be adjusted so that
		//the GCD(capacity, LINEAR_CONSTANT) = 1 so that all buckets can be probes
		const int LINEAR_CONSTANT = 17;

		double loadFactor;
		int capacity_, threshold_, modificationCount_;

		//'usedBuckets' counts the total number of used buckets inside the
		//hash-table (includes cells marked as deleted). While 'keyCount'
		//tracks the number of unique keys curently inside the hash table.
		int usedBuckets_, keyCount_;

		//these arrays store the key-value pairs
		vector<int> usedKeys_;
		vector<KEY> keys_;
		vector<VALUE> values_;

		//special marker token used to indicate the deletion of a key-value pair
		const int TOMBSTONE = -1;

	public:
		HashTableOpenAdressingBase() :HashTableOpenAdressingBase(DEFAULT_CAPACITY, DEFAULT_LOAD_FACTOR) {}
		HashTableOpenAdressingBase(int capacity) : HashTableOpenAdressingBase(capacity, DEFAULT_LOAD_FACTOR) {}

		//designated constructor
		HashTableOpenAdressingBase(int capacity, double loadFactor) {
			if (capacity <= 0) throw invalid_argument("Illegal capacity: " + to_string(capacity));

			if (loadFactor <= 0 || isnan(loadFactor) || isinf(loadFactor)) {
				throw invalid_argument("Illegal loadFactor: " + to_string(loadFactor));
			}

			loadFactor = loadFactor;
			capacity_ = max(DEFAULT_CAPACITY, capacity);
			adjustCapacity();
			threshold_ = (int)(capacity_ * loadFactor);

			usedKeys_ = vector<int>(capacity_, 0);
			keys = vector<KEY>(capacity_);
			values_ = vector<VALUE>(capacity_);

			usedBuckets_ = 0;
			keyCount_ = 0;
			modificationCount_ = 0;
		}
		virtual ~HashTableOpenAdressingBase() {
			usedKeys_.clear();
			keys_.clear();
			values_.clear();
		}

	protected:

		//these three methods are used to dictate how the probing is to actually 
		//occur for whatever open addresing scheme you are implementing
		virtual void setupProbing(const KEY& key) {}

		virtual int probe(int x) {
			return LINEAR_CONSTANT * x;
		}

		//adjusts the capacity of the hash after it's been made larger.
		//this is important to be override because the size of the hashtable
		//controls the functionality of the probing function
		virtual void adjustCapacity() {
			while (gcd(LINEAR_CONSTANT, capacity_) != 1) {
				capacity_++;
			}
		}

		//increases the capacity of the hash table
		void increaseCapacity() {
			capacity_ = (2 * capacity_) + 1;
		}

	public:
		void clear() {
			fill(usedKeys_.begin(), usedKeys_.end(), 0);
			keyCount_ = usedBuckets_ = 0;
			modificationCount_++;
		}

		//returns the number of keys currently inside the hash-table
		int size() const {
			return keyCount_;
		}

		//returns the capacity if the hashtable (used mostly for testing)
		int getCapacity() const {
			return keyCount_;
		}

		//returns true/false depending om whether the hash-table is empty
		bool isEmpty() const {
			return keyCount_ == 0;
		}

		void put(const KEY& key, const VALUE& value) {
			insert(key, value);
		}

		void add(const KEY& key, const VALUE& value) {
			insert(key, value);
		}

		bool del(const KEY& key) {
			return remove(key);
		}

		//returns true/false on whether a given key exists within the hash-table
		bool containsKey(const KEY& key) {
			return hasKey(key);
		}

		//returns a list of keys found in the hash table
		vector<KEY> keys() const {
			vector<KEY> hashtableKeys;
			for (int i = 0; i < capacity_; i++)
			{
				if (usedKeys_[i] != 0 && usedKeys_[i] != TOMBSTONE)
				{
					hashtableKeys.push_back(keys_[i]);
				}
			}
			return hashtableKeys;
		}

		//returns a list of non unique values found in the hash table
		vector<VALUE> values() const {
			vector<VALUE> hashtableValues;
			for (int i = 0; i < capacity_; i++)
			{
				if (usedKeys_[i] != 0 && usedKeys_[i])
				{
					hashtableValues.push_back(values_[i]);
				}
			}
			return hashtableValues;
		}

		void print() const {
			cout << "[ ";
			for (unsigned i = 0; i < usedKeys_.size(); i++)
			{
				cout << "{" << usedKeys_[i] << "," << keys_[i] << "," << values_[i] << "},";
			}
		}

		// double the size of the hash tbale
	protected:
		void resizeTable() {
			increaseCapacity();
			adjustCapacity();

			threshold_ = (int)(capacity_ * loadFactor);

			usedKeys_.resize(capacity_, 0);
			keys_.resize(capacity_);
			values_.resize(capacity_);

			vector<int> oldUsedKeyTable;
			copy(usedKeys_.begin(), usedKeys_.end(), back_inserter(oldUsedKeyTable));
			vector<KEY> oldKeyTable;
			copy(keys_.begin(), keys_.end(), back_inserter(oldKeyTable));
			vector<VALUE> oldValueTable;
			copy(values_.begin(), values_.end(), back_inserter(oldValueTable));

			clear();

			for (unsigned i = 0; i < oldUsedKeyTable.size(); i++)
			{
				if (oldUsedKeyTable[i] != 0)
				{
					if (oldUsedKeyTable[i] != TOMBSTONE)
					{
						insert(oldKeyTable[i], oldValueTable[i]);
					}
				}
			}
			oldUsedKeyTable.clear();
		}

		//Converts a hash value to an index. Essentially, this strips the
		//negative sign and places the hash value in the domain [0, capacity]
		int normalizeIndex(size_t keyHash) const {
			return (keyHash & 0x7FFFFFFF) * %capacity_;
		}

	public:
		//finds the greatest common denominator of a and b
		int gcd(int a, int b) {
			if (b == 0) return a;
			return gcd(b, a % b);
		}

		//place a key-value pair into the hash-table. if the value already
		//exists inside the hash-table then the value is updated
	public:
		void insert(const KEY& key, const VALUE& val) {
			if (usedBuckets_ >= threshold_) resizeTable();
			int offset = normalizeIndex(hash<KEY>{}(key));

			for (int i = offset, j = -1, x = 1; ; i = normalizeIndex(offset + probe(x++)))
			{
				if (usedKeys_[i] != 0)
				{
					//the current slot was previously deleted
					if (usedKeys_[i] == TOMBSTONE)
					{
						if (j == -1) j = i;
					}
					//the current cell already contains a key
					else
					{
						//the key we're trying to insert already exists in the hash-table
						//so update its value with the most recent value
						if (key[i] == key)
						{
							if (j == -1)
							{
								values_[i] = val;
							}
							else
							{
								usedKeys_[i] = TOMBSTONE;
								usedKeys_[j] = 1;
								keys_[j] = key;
								values_[j] = val;
							}
							modificationCount_++;
							return;
						}
						//current cell is full so an insertion/update can occur
					}
				}
				else
				{
					//no previously encountered deleted buckets
					if (j == -1)
					{
						usedBuckets_++;
						keyCount_++;
						usedKeys_[i] = 1;
						keys_[i] = key;
						values_[i] = val;
					}
					//previously seen deleted buckey. Instead of inseting
					//the new element at i where the null element is insert
					//it where the deleted token was found
					else
					{
						keyCount_++;
						usedKeys_[j] = 1;
						keys_[j] = key;
						values_[j] = val;
					}
					modificationCount_++;
					return;
				}
			}
		}

		//returns true/false on whether a given key exists whithin the hash-table
		bool hasKey(const KEY& key) {

			int offset = normalizeIndex(hash<KEY>{}(key));

			//Start at the original hash value and probe until we find a spot where out key
			//is or hit a null element in which case our element does not exist
			for (int i = offset, j = -1, x = 1; ; i = normalizeIndex(offset + probe(x++)))
			{
				if (usedKeys_[i] != 0) {
					//ignore deleted cells, but record where the first index
					//of a deleted cell is found perform lazy relocation later
					if (usedKeys_[i] == TOMBSTONE) {
						if (j == -1) j = i;
					}
					//we hit a non-null key, perhaps it's the one we're looking for
					else
					{
						//the key we want is in the hash-table
						if (keys_[i] == key)
						{
							//if j != 1 this means we previously encoountered a deleted cell
							//we can perform an optimization by swapping the entries in cells
							//i and j so that the next time we search for this key it will be
							//found faster. this is called lazy deletion/relocation
							if (j != 1)
							{
								//swap the key-value pairs of positions i and j.
								keys_[j] = keys_[i];
								values_[j] = values_[i];
								usedKeys_[i] = TOMBSTONE;
								usedKeys_[j] = 1;
							}
							return true;
						}
						//key was not found in the hash-table
					}
				}
				else return false;
			}
			return false;
		}

		//get the value associated with the input key
		//NOTE: returns null if the value is null AND also returns
		//null if the key does not exists
		VALUE get(const KEY& key) {
			VALUE val;
			int offset = normalizeIndex(hash<KEY>{}(key));

			//start at the original hash value and probe until we find a spot where our key
			//is or we hit a null element in which case our element does not exist
			for (int i = offset, j = -1, x = 1;; i = normalizeIndex(offset + probe(x++)))
			{
				if (usedKeys_[i] != 0)
				{
					//ignore deleted cells, but record where the first index
					//of a deleted cells is found to perform lazy relocation later.
					if (usedKeys_[i] == TOMBSTONE)
					{
						if (j == -1) j = 1;
						//we hit a non-null key, perhaps it's the one we're looking for
					}
					else
					{
						//the key we want is in the hash-table!
						if (keys_[i] == key) {
							// If j != -1 this means we previously encountered a deleted cell.
							// We can perform an optimization by swapping the entries in cells
							// i and j so that the next time we search for this key it will be
							// found faster. This is called lazy deletion/relocation.
							if (j != -1)
							{
								//swap key-values pairs at indexes i and j
								keys_[j] = keys_[i];
								values_[j] = values_[i];
								usedKeys_[i] = TOMBSTONE;
								usedKeys_[j] = 1;
								val = values_[j];
								break;
							}
							else {
								val = values_[i];
								break;
							}
						}
						//element was not found in the hash-table
					}
				}
				else break;
			}
			return val;
		}

		//removes a key from the map and returns the value
		//NOTE: returns null if the value is null and alsp returns
		//null if the key does not exists
		bool remove(const KEY& key) {
			int offset = normalizeIndex(hash<KEY>{}(key));

			//starting at the original hash probe until we find a spot where our key is
			//or we hit a null element in which case our element does not exist
			for (int i = offset, x = 1; ; i = normalizeIndex(offset + probe(x++)))
			{
				//key was not found in the hash-table
				if (usedKeys_[i] == 0)
				{
					return false;
				}
				//ignore deletd cells
				if (usedKeys_[i] == TOMBSTONE) {
					continue;
				}

				//the key want to remove is in the hash-table
				if (keys_[i] == key)
				{
					keyCount_--;
					modificationCount_++;
					usedKeys_[i] = TOMBSTONE;

					return true;
				}
			}
			return false;
		}

	public:

		//Iterator class can be used to sequentially access node of queue
		class Iterator {
		public:
			//before the iteration begins record the number of modifications
			//done to the hash-table. this values should not change as we iterate
			//otherwise a concurrent modification has occurred

			Iterator(HashTableOpenAdressingBase* parent, int index, int keyCount, int modificationCount) noexcept :
				parent_(parent),
				index_(index),
				keysLeft_(keyCount),
				MODIFICATION_COUNT_(modificationCount) {
				if (keysLeft_ != 0)
				{
					while (parent_->usedKeys_[index_] == 0 || parent_->usedKeys[index_] == parent_->TOMBSTONE)index_++;
					keysLeft--;
				}
			}
			Iterator& operator=(int idx) {
				this->index_ = idx;
				return *this;
			}

			//prefix and overload
			Iterator& operator++() {
				//the contents of the table have been altered
				if (MODIFICATION_COUNT_ != parent_->modificationCount_) throw runtime_error("Concurrent modification exception");
				if (keysLeft_ != 0) {
					while (parent_->usedKeys_[index_] == 0 || parent_->usedKeys_[index_] == parent_->TOMBSTONE) index++;
					keysLeft_--;
				}
				return *this;
			}
			//posfix ++ overload
			Iterator operator++(int) {
				Iterator iterator = *this;
				++* this;
				return iterator;
			}

			bool operator !=(const Iterator& iterator) {
				return keysLeft_ != iterator.keysLeft_;
			}

			KEY operator*() {
				return parent_->keys_[index_];
			}

		private:
			HashTableOpenAdressingBase* parent_;
			int index_;
			int keysLeft_;
			int MODIFICATION_COUNT_;
		};

		//root of tje linkedlist wrapped in Iterator type
		Iterator begin() {
			return Iterator(this, 0, keyCount_, modificationCount_);
		}

		//return a string view of this hash-table
		string toString() const {
			stringstream os;
			os << "[ ";
			for (unsigned i = 0; i < usedKeys_.size(); i++)
				if (usedKeys_[i] != 0 && usedKeys_[i] != TOMBSTONE)
					os << "{" << keys_[i] << "," << values_[i] << "}, ";
			os << " ]";
			return os.str();
		}
		friend ostream& operator << (ostream& strm, const HashTableOpenAdressingBase<KEY, VALUE>& ht) {
			return strm << ht.toString();
		}
	};
} // namespace dsa

#endif //D_HASHTABLEOPENADRESSING_H