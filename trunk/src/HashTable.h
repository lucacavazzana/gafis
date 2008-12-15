/*
 * HashTable.h
 *
 *  Created on: 15-dic-2008
 *      Author: stefanocadario
 */

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

struct hashElement {
	int index;
	hashElement* nextRecord;
};

class HashTable {
public:
	HashTable(int size);
	virtual ~HashTable();
	void addElement(int key, int index);
	hashElement* getElement(int key);
	void elaborateStats();
protected:
	hashElement **hashTable;
	int hashTableSize;
	int nextPrime(int number);
	int emptyCell;
	int collision;
	int elementsAdded;
};

#endif /* HASHTABLE_H_ */
