//=============================================================================
//
// CLASS:           vdbMap
//
// OVERVIEW:		The vdbMap class is used to manage a collection of object
//                  pointers in a balanced red-black tree .  The pointed-to
//                  objects are adopted by this object when the Add function is
//                  called.  All objects of type T must be created on the heap.
//                  The vdbMap destructor deletes the pointed-to objects held
//                  by this object.
//
//                  Direct access to items are available through the Get
//                  function.  Sequential access is provided through the
//                  vdbMapIterator functions First, Last, Next and Prev.
//
// ALGORITHM:		"Algorithms in C++" by Robert Sedgewick, 1992,
//                  pp. 203-211, 221-226.
//
// FILE NAME:       vdbMap.h
//
// INITIAL DATE:    11/23/96
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifndef __vdbMap_h
#define __vdbMap_h

#ifndef __vdbString_h
	#include "vdbString.h"
#endif

#ifndef __vdbSet_h
	#include "vdbSet.h"
#endif

#ifndef __boolean_h
	#include "boolean.h"
#endif

template <class T> class vdbMap;
template <class T> class vdbMapNode;
template <class T> class vdbMapIterator;

#include <istream>
#include <ostream>


//=============================================================================
// vdbMap class
//=============================================================================
template <class T> class vdbMap
{
public:
	// essential class functions
	vdbMap();										// basic constructor
	~vdbMap();										// destructor
	vdbMap( const vdbMap<T>& rhs );					// copy constructor 
	vdbMap<T>& operator=( const vdbMap<T>& rhs );	// assignment operator 

public:
	// public member functions
	void Add( T* pItem, vdbString& keyValue );
	void Add( T* pItem, const char* szKeyValue );
	int Depth( vdbString& keyValue );
	T* Get( vdbString& keyValue );
	T* Get( const char* szKeyValue );
	T* operator[]( vdbString& keyValue );
	T* operator[]( const char* szKeyValue );
	T* Orphan( const char* szKeyValue );
	bool Remove( const char* szKeyValue );
	void ClearAll();
	inline int Count() const;
	int GetInexact( const char* szSearch, T** pMatches, int maxMatches );

public:
	// TraverseMapCallback is a callback function receiving from the traversal function
	// the current key, the current T item, and a user-defined item (a cookie),
	// and returning a bool of true to continue or false to stop traversing
	typedef bool (*TraverseMapCallback)( const char* szKeyValue, T* pItem, void* pCallbackCookie );
	bool TraverseMap( TraverseMapCallback pCallback, void* pCallbackCookie = NULL ); 

private:
	bool TraverseNext( vdbMapNode<T>* pNode, TraverseMapCallback pCallback, void* pCallbackCookie = NULL ); 

private:
	void Split( vdbString& keyValue, vdbMapNode<T>*& x, vdbMapNode<T>*& parent, vdbMapNode<T>*& grandparent,vdbMapNode<T>*& greatgrandparent );
	vdbMapNode<T>* Rotate( vdbString& keyValue, vdbMapNode<T>* y );
	void GetNextInexact( vdbMapNode<T>* pNode, const char* szSearch, T** pMatches, int maxMatches, int& countMatches );

private:
	void _Add( T* pItem, vdbString& keyValue );
	T* _Get( vdbString& keyValue );

public:
	// iterator management functions
	vdbMapIterator<T>* CreateIterator();
	bool DestroyIterator( vdbMapIterator<T>* pIterator );

private:
	// private member data
	vdbMapNode<T>* _head;
	vdbMapNode<T>* _tail;
	int _size;
	vdbSet< vdbMapIterator<T> > _registeredIterators;
	
	friend vdbMapIterator<T>;
	friend std::ostream& operator<< <T> ( std::ostream& os, const vdbMap<T>& obj );
	friend std::istream& operator>> <T> ( std::istream& is, vdbMap<T>& obj );
};


//=============================================================================
// vdbMapNode class
//=============================================================================
template <class T> class vdbMapNode
{
private:
	enum redBlackType { RED, BLACK };
	
private:
	// essential class functions
	vdbMapNode( T* pItem, vdbString& s, redBlackType rb, vdbMapNode<T>* left, vdbMapNode<T>* right );
	~vdbMapNode();

private:
	void CascadeDelete( vdbMapNode<T>* pTail );
	inline void SetToBlack();
	inline void SetToRed();
	inline bool IsBlack() const;
	inline bool IsRed() const;

private:
	T* _pItem;					// the item itself
	vdbString _keyValue;		// the search key
	vdbMapNode* _left;			// pointer to the node on the left
	vdbMapNode* _right;			// pointer to the node on the right
	redBlackType _rb;			// black node bind the 2-3-4 tree together, red nodes bind the 3-nodes and 4-nodes into small binary trees	

	friend vdbMap<T>;
	friend vdbMapIterator<T>;
};


//=============================================================================
// vdbMapIterator class
//=============================================================================
template <class T> class vdbMapIterator
{
private:
	// essential class functions
	vdbMapIterator( vdbMap<T>& map );								// basic constructor
	~vdbMapIterator();												// destructor
	vdbMapIterator( const vdbMapIterator<T>& rhs );					// copy constructor 
	vdbMapIterator<T>& operator=( const vdbMapIterator<T>& rhs );	// assignment operator 

public:
	// public member functions
	void First();
	void Last();
	void Next();
	void Prev();
	T* Current();
	const char* CurrentKey();
	inline bool IsValid() const;

private:
	void Traverse( vdbMapNode<T>* pNode, int& index );

private:
	vdbMap<T>& _map;						// the map
	int _currentIndex;						// index of the current offset into the array of ordered nodes
	int _count;								// the number of nodes in the map
	vdbMapNode<T>** _pOrderedNodes;			// a pointer to an ordered list (built by the constructor) of all nodes in the map
	bool _valid;							// does the current index point to a valid item

	friend vdbMap<T>;
	friend vdbSet< vdbMapIterator<T> >;
};


//=============================================================================
// inline functions
//=============================================================================

//--------------------------------------------
//^ The Count function return a count of the number of objects currently in the
//  map.
//< returns the count.
//
template<class T>
inline
int vdbMap<T>::Count() const
{
	return _size;
}


//--------------------------------------------
template <class T>
inline
void vdbMapNode<T>::SetToBlack()
{
	_rb = BLACK;
}


//--------------------------------------------
template <class T>
inline
void vdbMapNode<T>::SetToRed()
{
	_rb = RED;
}


//--------------------------------------------
template <class T>
inline
bool vdbMapNode<T>::IsBlack() const
{
	return _rb == BLACK ? true : false;
}


//--------------------------------------------
template <class T>
inline
bool vdbMapNode<T>::IsRed() const
{
	return _rb == RED ? true : false;
}


//--------------------------------------------
//^ The IsValid function returns the status of the iterator.
//< returns true if the iterator points to a valid map item
//< returns false if the iterator does not point to a valid map item
template <class T>
inline
bool vdbMapIterator<T>::IsValid() const
{
	return _valid;
}


//=============================================================================
// non-member stream functions
//=============================================================================

//------------------------------------------
//^^ ostream << operator
//>  objects of type T must have an output stream operator that ends in with
//   the statement "os << endl;"
//

template<class T>
std::ostream& operator<< ( std::ostream& os, const vdbMap<T>& obj )
{
	os << obj._size << std::endl;

	// cast away const because iterators actually do modify the vdbMap
	vdbMap<T>& non_const_obj = const_cast< vdbMap<T>& >( obj );

	vdbMapIterator<T>* it = non_const_obj.CreateIterator();
	for ( it->First(); it->IsValid(); it->Next() )
	{
		os << it->CurrentKey() << std::endl;
		os << *( it->Current() );
	}
	non_const_obj.DestroyIterator( it );

	return os;
}


// Careful:  artificial limit on key value!
//const int maxMapKeySize = 81;	// prior to build 88
const int maxMapKeySize = 512;	// changed to support long URLs as the map key


//------------------------------------------
//^^ istream >> operator
//^  All classes of type T must have a default constructor. 
//^  Any items of type T currently owned by this set are left in the set.
//   Thus the extraction operator can be used to concatenate two sets.
//>  objects of type T must have an input stream operator that ends in with
//   the statement "is >> ws;"
//
template<class T>
std::istream& operator>> ( std::istream& is, vdbMap<T>& obj )
{
	int inputSize;
	is >> inputSize >> std::ws;
	assert( inputSize >= 0 );

	for ( int i = 0; i < inputSize; i++ )
	{
		char szKey[maxMapKeySize + 1];
		is.getline( szKey, sizeof(szKey) );

		T* pItem = new T;
		is >> *pItem;
		
		obj.Add( pItem, szKey );
	}


	return is;
}

#endif  // __vdbMap_h



