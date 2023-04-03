// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/iloallocator.h
// --------------------------------------------------------------------------
// Licensed Materials - Property of IBM
//
// 5725-A06 5725-A29 5724-Y47 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
// Copyright IBM Corp. 2000, 2022
//
// US Government Users Restricted Rights - Use, duplication or
// disclosure restricted by GSA ADP Schedule Contract with
// IBM Corp.
// ---------------------------------------------------------------------------

#ifndef __CONCERT_iloallocatorH
#define __CONCERT_iloallocatorH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilosys.h>

class IloMemory;

#ifdef AFCDEBUG
#define ILOALLOCCPP
#endif

class IloAllocationList {

public:

	IloAllocationList();
	IloAllocationList( IloInt nbBytePerCell );
	~IloAllocationList();

	void* alloc( IloInt size );
	void free( void* block );

	IloInt getMemoryUsage();

	void end();

private:

	struct Cell {
		IloInt _size;
		Cell* _previous;
		Cell* _next;
		
		static size_t Sizeof() {
			size_t nd = (sizeof(Cell) + sizeof(double) - 1) / sizeof(double); 
			return nd * sizeof(double);
		}
	};

	void free( Cell* cell );

	void* _first;
	IloInt _usedMemory;

	IloInt _allocatorSize;
};

class IloFixedSizeAllocatorI {
private:
	IloFixedSizeAllocatorI operator=(IloFixedSizeAllocatorI const &); 
	struct Cell { Cell* _next; };
	IloBool _useIt;               
	const IloInt _nbBytesPerCell; 
	Cell* _firstCell;             
	Cell* _toFree;                
	void addFree(Cell *q) {
		assert(_useIt);
		q->_next=_toFree; _toFree=q;
	}
	void addCell(Cell *q) {
		assert(_useIt);
		q->_next=_firstCell; _firstCell=q;
	}
	IloInt _usedMemory;
	IloInt _totalMemory;
	IloInt getSize() const { return _nbBytesPerCell; }

	IloAllocationList* _allocationList;

public:
	IloBool useIt() const        { return _useIt; }
	void setUseIt(IloBool useIt) { 
		_useIt=useIt; 
		if( !_useIt && 0 == _allocationList ) {
			ILOPROTECTNEW( _allocationList, new IloAllocationList(_nbBytesPerCell) );
		}
	}
	IloInt getMemoryUsage() const      { 
		if( _useIt ) {
			return _usedMemory; 
		} else {
			return _allocationList->getMemoryUsage();
		}
	}
	IloInt getTotalMemoryUsage() const { return _totalMemory; }
	IloFixedSizeAllocatorI(const IloInt nbBytesPerCell);
	~IloFixedSizeAllocatorI();
#ifdef ILOALLOCCPP
	void* allocate(class IloMemory *mem);
#else
	inline void* allocate(class IloMemory *mem) {
		assert(_firstCell == 0 || _useIt);

		if( _useIt ) {
			Cell *p = _firstCell;
			if (!p)
				p = allocateBlock(mem); 
			else
				_firstCell = p->_next;
			_usedMemory += _nbBytesPerCell;
			return (void*)p;
		} else {
			assert(_allocationList!=0);
			void* p = _allocationList->alloc( _nbBytesPerCell );
			_totalMemory += _nbBytesPerCell;
			_usedMemory += _nbBytesPerCell;
			return p;
		}
	}
#endif
	inline void deallocate(void* p) {
#ifdef AFCDEBUG
		MemInfo meminfo(0,0);
		IloAssert(Table->find(p, meminfo) == MemoryHashTable::ILOHASHOK, "Deallocating unallocated cell");
		if (meminfo._size != _nbBytesPerCell) {
			IloInt stop = 0;
		}
		IloAssert(Table->remove(p) == MemoryHashTable::ILOHASHOK, "Cannot remove form allocate table");
#endif
		if (!_useIt) {
			_allocationList->free( p );
		}
		else {
			Cell* q = (Cell*)p;
			q->_next = _firstCell;
			_firstCell = q;
		}
		_usedMemory -= _nbBytesPerCell;
	}

	Cell* allocateBlock(IloMemory *mem);
};

class ILO_EXPORTED IloMemory {
private:
	IloMemory &operator=(IloMemory const &); 
	enum { ILO_NB_ALLOC = 60 };
	IloFixedSizeAllocatorI* _array[ILO_NB_ALLOC];
	IloFixedSizeAllocatorI* getAllocator(IloInt s) const { return _array[s]; }
	
	inline IloInt getIndex(IloInt s) const {
		s = (s - 1) >> 3;
		if (s < 8) {
			return s;
		}
		else {
			s >>= 4; 
			IloInt index = 8;
#ifdef ILO64
			assert(sizeof(s) == 8);
			if (s >= 0x100000000) { index += 32; s >>= 32; }
#else
			assert(sizeof(s) == 4);
#endif
			if (s >= 0x10000) { index += 16; s >>= 16; }
			if (s >= 0x100)   { index += 8; s >>= 8; }
			if (s >= 0x10)    { index += 4; s >>= 4; }
			static const IloInt countBitsUnder16[16] = {
				0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4
			};
			index += countBitsUnder16[s];
			if (index >= _nbAllocator)
				throw IloMemoryException();
			return index;
		}
	}

	friend class IloFixedSizeAllocatorI;

	IloAllocationList* _defaultAllocator;
	const IloInt _fixedSizeAllocatorLimit;
	IloInt _nbAllocator;

public:
	~IloMemory();
	IloMemory(IloBool);
	IloBool useIt() const { return _array[OL]->useIt(); }
	void setUseIt(IloBool useIt) const;
	inline void* alloc(const IloInt s) {
		if( s < _fixedSizeAllocatorLimit ) {
			return (s != 0) ? getAllocator(getIndex(s))->allocate(this) : (void *)0;
		} else {
			return _defaultAllocator->alloc( s );
		}
	}
	inline void free(void* p, const IloInt s) {
		if (p != 0) {
			if( s < _fixedSizeAllocatorLimit ) {
				getAllocator(getIndex(s))->deallocate(p);
			} else {
				_defaultAllocator->free( p );	
			}
		}
	}
	IloInt getTotalMemoryUsage() const;
	IloInt getMemoryUsage() const;

	void snapshot();
};

inline void* operator new (size_t sz, const IloMemory* h) {
	return ((IloMemory*)h)->alloc(sz);
}
#ifdef ILODELETEOPERATOR
inline void operator delete(void*, const IloMemory*) { }
#endif

class ILO_EXPORTED IloAllocationStack {
private:
	class Part {
		friend class IloAllocationStack;
	private:
		IloAny* _data;      
		IloInt  _size;      
		IloInt _firstIndex; 
		Part*  _prevPart; 
		Part*  _nextPart;
	public:
		Part();
		Part (IloUInt size, IloInt first=0, Part* prev=0);
		~Part();
		IloInt getSize(); 
	};

private:
	Part*   _currentPart;
	Part*   _firstPart;
	IloAny* _top; 
	IloInt  _topIndex; 
	IloInt  _size;     
	IloInt  _lost;
        IloAllocationList* _allocationList; 

	IloBool isAboveEndOfCurrentPart(IloInt index) {
		
		return (IloBool)(index >=
			(_currentPart->_firstIndex + _currentPart->_size));
	}
	void safeGoToNextPart(IloUInt size);

public:
	IloAllocationStack();
	~IloAllocationStack();
	IloInt getSize() const { return _size; }
	IloInt getIndex() const {return _topIndex;}
	IloAny allocate(IloUInt size);  
	void free(IloAny p, const IloUInt s);
	void deallocate (Part*);
	void reset();
public:
	class NullIndex : public IloException {
	public:
		NullIndex();
		void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	};
};

void* operator new (size_t s, IloAllocationStack* heap);

#if defined (ILONEWOPERATOR)
inline void* operator new[] (size_t s, IloAllocationStack* heap){
	return operator new (s, heap);
}
#endif

#if defined(ILODELETEOPERATOR)
inline void operator delete(void*, size_t, IloAllocationStack*){}
inline void operator delete(void*, IloAllocationStack*){}
# if defined(ILONEWOPERATOR)
inline void operator delete [] (void*, IloAllocationStack*){}
# endif
#endif

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
