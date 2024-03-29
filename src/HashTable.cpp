/*
 * HashTable.cpp
 *
 *  Created on: 15-dic-2008
 *      Author: stefanocadario
 */

#include "HashTable.h"
#include <iostream>
using namespace std;

HashTable::HashTable(int size) {
	hashTableSize = nextPrime(size);		// Obtain next prime number (it works better no one knows why)

	hashTable = new hashElement*[hashTableSize];		// Allocating (istance heroic etekno) table size

	for(int i=0; i<hashTableSize; i++) {	// Set all cells to -1
		//hashTable[i] = NULL;
		hashTable[i] = new hashElement;
		hashTable[i]->index = -1;
		hashTable[i]->nextRecord = NULL;
	}

	emptyCell = 0;
	collision = 0;
	elementsAdded = 0;
}

HashTable::~HashTable() {
	// TODO: delete all elements
	//delete hashTable;
}

int HashTable::addElement(int key, int index) {

	int candidate = key%hashTableSize;	// computation of the index of hash array

	hashElement *current = hashTable[candidate];

	// TODO: optimize this part: a new struct is created every add element...

	while(current->index!=-1) {
		collision++;
		current = current->nextRecord;
	}
	current->index = index;
	current->nextRecord = new hashElement;
	current->nextRecord->index=-1;

	elementsAdded++;

	return candidate;

}

// TODO: it should not return the hashElement array...it's not incapsulated !
hashElement* HashTable::getElement(int key) {
	return hashTable[key%hashTableSize];
}

void HashTable::elaborateStats() {

	for(int i=0; i<hashTableSize; i++)
	{
		if(hashTable[i]->index == -1)
		{
			emptyCell++;
		}
		//cout << "Hash[" << i << "] = " << hashTable[i]->index;
		hashElement *current = hashTable[i]->nextRecord;
		while(current!=NULL) {
			//cout << ";" <<current->index;
			current = current->nextRecord;
		}
		//cout << endl;
	}
	cout << "Collisions: " << collision << endl;
	cout << "Coverture: " << (float)(hashTableSize-emptyCell)*100.0f/(float)hashTableSize << "%" << endl;
	cout << "Mean numbers of elements per cell: " << (float)elementsAdded/(float)(hashTableSize-emptyCell) << endl;

}
//Checks if n is a prime number. Return 1 if yes, return 0 if not.
int checkPrimer(int n) {
	for(int m = 2; m*m <= n; m++)
		if(n % m == 0)
			return 0;
	return 1;
}

int HashTable::nextPrime(int number) { // TODO: maybe nextPrime function can be optimized (precompiled table) ?
	if(number%2==0) number++;
	while(checkPrimer(number))
		number = number+2;
	return number-2;
}
