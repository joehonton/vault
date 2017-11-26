//=============================================================================
//
// CLASS:           vdbBinaryTree
//
// OVERVIEW:		The vdbBinaryTree class is used to manage a balanced red-black
//                  tree.  The class used for template parameter K must have 
//                  a copy constructor, an operator <, and an operator !=.
//
//                  Direct access to items are available through the Get
//                  function.  Sequential access is provided through the
//                  vdbBinaryTreeIterator functions First, Last, Next and Prev.
//
//                  The default constructor for K must produce an object which is
//                  guaranteed to be at the top of the sort order.  Typically this
//                  means setting the key value to NULL or 0 or using a zero-length
//                  string.
//
// ALGORITHM:		"Algorithms in C++" by Robert Sedgewick, 1992,
//                  pp. 203-211, 221-226.
//
// CODE BASE:       Taken from vdbMap and modified.
//
// FILE NAME:       vdbBinaryTree.h
//
// INITIAL DATE:    1/13/99
//
// COPYRIGHT:		Copyright © 1999 by Joe Honton
//
//=============================================================================

#ifndef __vdbBinaryTree_h
#define __vdbBinaryTree_h

#ifndef __vdbSet_h
	#include "vdbSet.h"
#endif

#ifndef __boolean_h
	#include "boolean.h"
#endif

template <class K> class vdbBinaryTree;
template <class K> class vdbBinaryTreeNode;
template <class K> class vdbBinaryTreeIterator;

#include <istream>
#include <ostream>


//=============================================================================
// vdbBinaryTree class
//=============================================================================
template <class K> class vdbBinaryTree
{
public:
	// essential class functions
	vdbBinaryTree();											// basic constructor
	~vdbBinaryTree();											// destructor
	vdbBinaryTree( const vdbBinaryTree<K>& rhs );				// copy constructor 
	vdbBinaryTree<K>& operator=( const vdbBinaryTree<K>& rhs );	// assignment operator 

public:
	// public member functions
	void Add( K& keyValue );
	bool Exists( K& keyValue );
	int Depth( K& keyValue );
	bool Remove( K& keyValue );
	void ClearAll();
	inline int Count() const;

private:
	void Split( K& keyValue, vdbBinaryTreeNode<K>*& x, vdbBinaryTreeNode<K>*& parent, vdbBinaryTreeNode<K>*& grandparent,vdbBinaryTreeNode<K>*& greatgrandparent );
	vdbBinaryTreeNode<K>* Rotate( K& keyValue, vdbBinaryTreeNode<K>* y );

public:
	// iterator management functions
	vdbBinaryTreeIterator<K>* CreateIterator();
	bool DestroyIterator( vdbBinaryTreeIterator<K>* pIterator );

private:
	// private member data
	vdbBinaryTreeNode<K>* _head;
	vdbBinaryTreeNode<K>* _tail;
	int _size;
	vdbSet< vdbBinaryTreeIterator<K> > _registeredIterators;
	
	friend vdbBinaryTreeIterator<K>;
	friend std::ostream& operator<< <K> ( std::ostream& os, const vdbBinaryTree<K>& obj );
	friend std::istream& operator>> <K> ( std::istream& is, vdbBinaryTree<K>& obj );
};


//=============================================================================
// vdbBinaryTreeNode class
//=============================================================================
template <class K> class vdbBinaryTreeNode
{
private:
	enum redBlackType { RED, BLACK };
	
private:
	// essential class functions
	vdbBinaryTreeNode( K& keyValue, redBlackType rb, vdbBinaryTreeNode<K>* left, vdbBinaryTreeNode<K>* right );
	~vdbBinaryTreeNode();

private:
	void CascadeDelete( vdbBinaryTreeNode<K>* pTail );
	inline void SetToBlack();
	inline void SetToRed();
	inline bool IsBlack() const;
	inline bool IsRed() const;

private:
	K _keyValue;				// the search key
	vdbBinaryTreeNode* _left;	// pointer to the node on the left
	vdbBinaryTreeNode* _right;	// pointer to the node on the right
	redBlackType _rb;			// black node bind the 2-3-4 tree together, red nodes bind the 3-nodes and 4-nodes into small binary trees	

	friend vdbBinaryTree<K>;
	friend vdbBinaryTreeIterator<K>;
};


//=============================================================================
// vdbBinaryTreeIterator class
//=============================================================================
template <class K> class vdbBinaryTreeIterator
{
private:
	// essential class functions
	vdbBinaryTreeIterator( vdbBinaryTree<K>& map );								// basic constructor
	~vdbBinaryTreeIterator();													// destructor
	vdbBinaryTreeIterator( const vdbBinaryTreeIterator<K>& rhs );				// copy constructor 
	vdbBinaryTreeIterator<K>& operator=( const vdbBinaryTreeIterator<K>& rhs );	// assignment operator 

public:
	// public member functions
	void First();
	void Last();
	void Next();
	void Prev();
	K& Current();
	inline bool IsValid() const;

private:
	void Traverse( vdbBinaryTreeNode<K>* pNode, int& index );

private:
	vdbBinaryTree<K>& _map;					// the map
	int _currentIndex;						// index of the current offset into the array of ordered nodes
	int _count;								// the number of nodes in the map
	vdbBinaryTreeNode<K>** _pOrderedNodes;	// a pointer to an ordered list (built by the constructor) of all nodes in the map
	bool _valid;							// does the current index point to a valid item

	friend vdbBinaryTree<K>;
	friend vdbSet< vdbBinaryTreeIterator<K> >;
};


//=============================================================================
// inline functions
//=============================================================================

//--------------------------------------------
//^ The Count function return a count of the number of objects currently in the
//  map.
//< returns the count.
//
template<class K>
inline
int vdbBinaryTree<K>::Count() const
{
	return _size;
}


//--------------------------------------------
template <class K>
inline
void vdbBinaryTreeNode<K>::SetToBlack()
{
	_rb = BLACK;
}


//--------------------------------------------
template <class K>
inline
void vdbBinaryTreeNode<K>::SetToRed()
{
	_rb = RED;
}


//--------------------------------------------
template <class K>
inline
bool vdbBinaryTreeNode<K>::IsBlack() const
{
	return _rb == BLACK ? true : false;
}


//--------------------------------------------
template <class K>
inline
bool vdbBinaryTreeNode<K>::IsRed() const
{
	return _rb == RED ? true : false;
}


//--------------------------------------------
//^ The IsValid function returns the status of the iterator.
//< returns true if the iterator points to a valid map item
//< returns false if the iterator does not point to a valid map item
template <class K>
inline
bool vdbBinaryTreeIterator<K>::IsValid() const
{
	return _valid;
}


//=============================================================================
// non-member stream functions
//=============================================================================

//------------------------------------------
//^^ ostream << operator
//>  objects of type K must have an output stream operator that ends with
//   the statement "os << endl;"
//
template<class K>
std::ostream& operator<< ( std::ostream& os, const vdbBinaryTree<K>& obj )
{
	os << obj._size << std::endl;

	// cast away const because iterators actually do modify the vdbBinaryTree
	vdbBinaryTree<K>& non_const_obj = const_cast< vdbBinaryTree<K>& >( obj );

	vdbBinaryTreeIterator<K>* it = non_const_obj.CreateIterator();
	for ( it->First(); it->IsValid(); it->Next() )
	{
		os << it->Current();
	}
	non_const_obj.DestroyIterator( it );

	return os;
}


//------------------------------------------
//^^ istream >> operator
//^  All classes of type K must have a default constructor. 
//>  objects of type K must have an input stream operator that ends with
//   the statement "is >> ws;"
//
template<class K>
std::istream& operator>> ( std::istream& is, vdbBinaryTree<K>& obj )
{
	int inputSize;
	is >> inputSize >> std::ws;
	assert( inputSize >= 0 );

	for ( int i = 0; i < inputSize; i++ )
	{
		K item;
		is >> item;
		
		obj.Add( item );
	}

	return is;
}

#endif  // __vdbBinaryTree_h



