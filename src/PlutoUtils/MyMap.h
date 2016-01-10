/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/**
 * @file MyMap.h
 Header file for ??? .
 */
#ifndef __MyMap_h
#define __MyMap_h

#include <e32base.h>
#include <s32btree.h>


/** @class TLongMap
 Class for Mapping
 */
template <class EntryType> class TLongMap: public CBase {
	private:
		int size;

		class GenKey : public MBtreeKey {
		public:
			virtual void Between(const TAny* aLeft,const TAny* aRight,TBtreePivot& aPivot) const {
				long left = *(long*)aLeft;
				long right = *(long*)aRight;

				long mid = left + (right - left) / 2;
				aPivot.Copy((TUint8*)&mid, sizeof(mid));
			}
			virtual TInt Compare(const TAny* aLeft,const TAny* aRight) const {
				long left = *(long*)aLeft;
				long right = *(long*)aRight;

				if (left < right)
					return -1;

				if (left > right)
					return 1;

				return 0;

			}
			virtual const TAny* Key(const TAny* anEntry) const {
				Entry* e=(Entry*)anEntry;
				return (TAny*)&(e->key);
			}
		};

		struct Entry {
			long	key;
			EntryType data;
		};

		TBtreeFix<Entry, long>* iTree;
		TBtreePos   currentPos;
		GenKey iKey;
		CMemPagePool* iPool;
		TBool wasLast;

	public:
		TLongMap();
		void AddData(long Key, EntryType data);
		EntryType GetData(long Key);
		~TLongMap();
		int Count() const;
		void Delete(long Key);

		EntryType GetFirst();
		EntryType GetCurrent();
		EntryType GetNext();
		TBool IsLast();

};


template <class EntryType> void TLongMap<EntryType>::AddData(long Key, EntryType data) {
	TBtreePos pos;
	Entry e;
	e.key = Key;
	e.data = data;

	iTree->InsertL(pos, e);
	size++;
}

template <class EntryType> EntryType TLongMap<EntryType>::GetData(long Key) {
	TBtreePos pos;
	TBool found = iTree->FindL(pos, Key);
	if (found) {
		Entry e;
		iTree->ExtractAtL(pos, e);
		return e.data;
	} else {
		// Not a good solution
		return NULL;
	}
}

template <class EntryType> void TLongMap<EntryType>::Delete(long Key) {
	TBtreePos pos;
	TBool found = iTree->FindL(pos, Key);
	if (found) {
		iTree->DeleteAtL(pos);
		size--;
	}
}

template <class EntryType> int TLongMap<EntryType>::Count() const {
	return size;
}


template <class EntryType> TLongMap<EntryType>::TLongMap():size(0) {
	iTree = new TBtreeFix<Entry, long>(EBtreeFast);
	iPool = CMemPagePool::NewL();
	iTree->Connect(iPool, &iKey);
	wasLast = ETrue;
}

template <class EntryType> TLongMap<EntryType>::~TLongMap() {
	delete iTree;
	delete iPool;
}

template <class EntryType> EntryType TLongMap<EntryType>::GetFirst()
{
	if(!size)
		return NULL;

	TBool found = iTree->FirstL(currentPos);
	wasLast = EFalse;
	if (found) {
		Entry e;
		iTree->ExtractAtL(currentPos, e);
		return e.data;
	} else {
		// Not a good solution
		return NULL;
	}
}

template <class EntryType> EntryType TLongMap<EntryType>::GetCurrent()
{
	if(!size)
		return NULL;

	if (!wasLast) {
		Entry e;
		iTree->ExtractAtL(currentPos, e);
		return e.data;
	}
	else
		return 0;
}


template <class EntryType> EntryType TLongMap<EntryType>::GetNext()
{
	if(!size)
		return NULL;

	TBool found;
	if (!wasLast)
		found = iTree->NextL(currentPos);
	else
		return NULL;

	if (found) {
		Entry e;
		iTree->ExtractAtL(currentPos, e);
		return e.data;
	} else {
		wasLast = ETrue;
		// Not a good solution
		return NULL;
	}
}

template <class EntryType> TBool TLongMap<EntryType>::IsLast()
{
	if(!size)
		return true;

	TBool found = iTree->NextL(currentPos);
	if (found) {
		Entry e;
		iTree->PrevisiousL(currentPos);
		return EFalse;
	} else {
		return ETrue;
	}
}

#endif
