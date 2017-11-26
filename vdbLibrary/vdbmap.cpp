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
// FILE NAME:       vdbMap.cpp
//
// INITIAL DATE:    11/23/96
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifdef USE_PRE_HDR
	#include "vdbHeader.h"
#else
	#include "vdbMap.h"
	#include <vdbAssert.h>
	#include "vdbException.h"
	#include "string.h"
#endif


/*
	variables as they appear in Sedgewick:

	_tail is z
	_head is head
	_rb is b
	_left is l
	_right is r

	the order of the first two constructor parameters is reversed
*/

//=============================================================================
// Essential vdbMap class functions
//=============================================================================

//--------------------------------------------
//^^ The constructor
//
template<class T>
vdbMap<T>::vdbMap()
	: _size( 0 ),
	  _head( 0 ),
	  _tail( 0 )
{
	_tail = new vdbMapNode<T>( 0, vdbString( "" ), vdbMapNode<T>::BLACK, 0, 0 );
	if ( _tail == 0 ) throw vdbMemoryException();
	_tail->_left = _tail;
	_tail->_right = _tail;

	// the head node must have a key value guaranteed to be the smallest because
	// all new nodes are added to the right-hand side of the head.  The left-hand
	// side is always 0
	_head = new vdbMapNode<T>( 0, vdbString( "" ), vdbMapNode<T>::BLACK, 0, _tail );	
	if ( _head == 0 ) throw vdbMemoryException();
}
	

//--------------------------------------------
//^^ The destructor
template<class T>
vdbMap<T>::~vdbMap()
{
	// Deleting the head causes a cascade delete of all nodes in the tree
	// furthermode, deleting a node deletes the item of type T that it points to.
	_head->CascadeDelete( _tail );
	delete _head; _head = 0;
	
	// finally, delete the tail node
	delete _tail; _tail = 0;

	// if there are any outstanding iterators, delete them
	for ( int i = _registeredIterators.Count(); i > 0; i-- )
		_registeredIterators.Remove( i - 1 );
}



//--------------------------------------------
//^^ The copy constructor
//^ Copies the contained objects.  
//
template<class T>
vdbMap<T>::vdbMap( const vdbMap<T>& rhs )
	: _size( rhs._size ),
	  _registeredIterators(),
	  _head( 0 ),
	  _tail( 0 )
{
	assert( false ); // Not yet implemented
}


//--------------------------------------------
//^^ The assignment operator
//^ Copies the contained objects.
//
template<class T>
vdbMap<T>& vdbMap<T>::operator=( const vdbMap<T>& rhs )
{
	if ( this == &rhs )
		return *this;
	
	assert( false ); // Not yet implemented

	return *this;
}


//=============================================================================
// Public vdbMap member functions
//=============================================================================
/*	Sedgewick	vdbMap
	insert()	Add()
	p			parent
	x			x
	v			keyValue
	info		pItem
*/
//--------------------------------------------
//^ The Add function adopts the item passed in the parameter list.  It is
//  associated with the string passed as the second parameter.
//> pItem is the object to add to the map.  This object must have been
//  created on the heap.
//> keyValue is the character string to associate with the object to
//  be used for finding and retrieving it again. 
//
template<class T>
void vdbMap<T>::Add( T* pItem, vdbString& keyValue )
{
	_Add( pItem, keyValue );
}


//--------------------------------------------
//^ The Add function adopts the item passed in the parameter list.  It is
//  associated with the string passed as the second parameter.
//> pItem is the object to add to the map.  This object must have been
//  created on the heap.
//> keyValue is the character string to associate with the object to
//  be used for finding and retrieving it again. 
//
template<class T>
void vdbMap<T>::Add( T* pItem, const char* keyValue )
{
	_Add( pItem, vdbString( keyValue ) );
}


//--------------------------------------------
template<class T>
void vdbMap<T>::_Add( T* pItem, vdbString& keyValue )
{
	assert( _head != 0 );

	vdbMapNode<T>* x = _head;			// the trial node
	vdbMapNode<T>* parent = _head;
	vdbMapNode<T>* grandparent = _head;
	vdbMapNode<T>* greatgrandparent = 0;

	while ( x != _tail )				// search until we reach the tail
	{
		greatgrandparent = grandparent;
		grandparent = parent;
		parent = x;

		x = ( keyValue < x->_keyValue ) ? x->_left : x->_right;
		// if this is a 4-node then split
		if ( x->_left->IsRed() && x->_right->IsRed() )
			Split( keyValue, x, parent, grandparent, greatgrandparent );
	}
	
	vdbMapNode<T>* newNode = new vdbMapNode<T>( pItem, keyValue, vdbMapNode<T>::RED, _tail, _tail );
	if ( newNode == 0 ) throw vdbMemoryException();
	if ( keyValue < parent->_keyValue )
		parent->_left = newNode;
	else
		parent->_right = newNode;

	Split( keyValue, newNode, parent, grandparent, greatgrandparent );
	_head->_right->SetToBlack();

	_size++;
}


/*	Sedgewick	vdbMap
	split()		Split()
	v			keyValue
	x			x
	p			parent
	g			grandparent
	gg			grandgrandparent
*/
//--------------------------------------------
//^ The Split function balances the links during an Add operation as follows:
//  Operation #1:  If we have a 2-node connected to a 4-node transform this
//  into a 3-node connected to two 2-nodes.  (Figure 15.9)
//  Operation #2:  If we have a 3-node connected to a 4-node (in the "right"
//  way), transform this into a 4-node connected to two 2-nodes.  (Figure 15.9)
//  Operation #3:  If we have a 3-node connected to a 4-node (in the "left"
//  way), transform this into a 4-node connected to two 2-nodes.  (Figure 15.10)
//  Operation #4:  If we have a 3-node connected to a 4-node (in the "middle"
//  way), transform this into a 4-node connected to two 2-nodes.  (Figure 15.10)
//
//> keyValue is the character string associated with the object to be added
//> x is the trial node
//> parent is the parent node of x
//> grandparent is the grandparent node of x
//> greatgrandparent is the greatgrandparent node of x
//
template<class T>
void vdbMap<T>::Split( vdbString& keyValue, vdbMapNode<T>*& x, vdbMapNode<T>*& parent, vdbMapNode<T>*& grandparent,vdbMapNode<T>*& greatgrandparent )
{
	// First take care of operations #1 and #2 through a simple color flip
	x->SetToRed();
	x->_left->SetToBlack();
	x->_right->SetToBlack();

	// If we've created an illegal tree with two consecutive red links
	// we need to do some further work.
	if ( parent->IsRed() )
	{
		grandparent->SetToRed();
		
		// If both the parent and grandparent are less than the keyValue OR
		// if both the parent and grandparent are greater than the keyValue
		// then we have operation #3, otherwise we have operation #4 in which
		// case we need to rotate the grandparent to make it into an operation #3.
		if ( keyValue < grandparent->_keyValue != keyValue < parent->_keyValue )
			parent = Rotate( keyValue, grandparent );	// operation #4

		// Operation #3
		x = Rotate( keyValue, greatgrandparent );
		x->SetToBlack();
	}
}


/*	Sedgewick	vdbMap
	rotate()	Rotate()
	v			keyValue
	y			y
	c			child
	gc			grandchild
*/
//--------------------------------------------
//^ The Rotate function performs a single rotation on a node during the split
//  operation.  First, rediscover the child and grandchild of the node to be
//  rotated.
//
//> keyValue is the character string associated with the object to be added
//> y is the node to rotate
//
template<class T>
vdbMapNode<T>* vdbMap<T>::Rotate( vdbString& keyValue, vdbMapNode<T>* y )
{
	vdbMapNode<T>* child;
	vdbMapNode<T>* grandchild;

	child = ( keyValue < y->_keyValue ) ? y->_left : y->_right;

	if ( keyValue < child->_keyValue )
	{
		grandchild = child->_left;
		child->_left = grandchild->_right;
		grandchild->_right = child;
	}
	else
	{
		grandchild = child->_right;
		child->_right = grandchild->_left;
		grandchild->_left = child;
	}

	if ( keyValue < y->_keyValue )
		y->_left = grandchild;
	else
		y->_right = grandchild;

	return grandchild;
}


//--------------------------------------------
//^ The Depth function returns a count of the number of nodes deep the
//  specified key Value is.
//> keyValue is the character string lookup value
//
template<class T>
int vdbMap<T>::Depth( vdbString& keyValue )
{
	vdbMapNode<T>* x = _head->_right;		// all nodes are to the right of the head
	assert( x != 0 );

	_tail->_keyValue = keyValue;			// search will terminate if it reaches the tail

	int depth = 0;
	while ( keyValue != x->_keyValue )
	{
		x = ( keyValue < x->_keyValue ) ? x->_left : x->_right;
		depth++;
	}
	
	return depth;
}


//--------------------------------------------
//^ The Get function return a pointer to the object found by the lookup key.
//> keyValue is the character string lookup value
//< returns a pointer to the object associated with the search string if found.
//< returns a null pointer if not found.
//
template<class T>
T* vdbMap<T>::Get( vdbString& keyValue )
{
	return _Get( keyValue );
}


//--------------------------------------------
//^ The Get function return a reference to the object found by the lookup key.
//> keyValue is the character string lookup value
//< returns a pointer to the object associated with the search string if found.
//< returns a null pointer if not found.
//
template<class T>
T* vdbMap<T>::Get( const char* keyValue )
{
	return _Get( vdbString( keyValue ) );
}


//--------------------------------------------
template<class T>
T* vdbMap<T>::_Get( vdbString& keyValue )
{
	vdbMapNode<T>* x = _head->_right;		// all nodes are to the right of the head
	assert( x != 0 );

	_tail->_keyValue = keyValue;			// search will terminate if it reaches the tail

	while ( keyValue != x->_keyValue )
	{
		assert( _tail->_keyValue == keyValue );			// 1/28/2004 OptimizeIndex.exe (adding this line has fixed the problem so it was probably a compiler optimization issue)
		x = ( keyValue < x->_keyValue ) ? x->_left : x->_right;
	}
	
	return x->_pItem;
}


//--------------------------------------------
//^^ The operator[] function
//^ This function returns a reference to the object found by the
//  lookup key.
//> keyValue is the string lookup value
//< returns a reference to the object found
//
template<class T>
T* vdbMap<T>::operator[]( vdbString& keyValue )
{
	return Get( keyValue );
}


//--------------------------------------------
//^^ The operator[] function
//^ This function returns a reference to the object found by the
//  lookup key.
//> keyValue is the string lookup value
//< returns a reference to the object found
//
template<class T>
T* vdbMap<T>::operator[]( const char* keyValue )
{
	return Get( vdbString( keyValue ) );
}


//--------------------------------------------
//^ The Orphan function removes a single object from the map, reconnects its
//  parent, left and right nodes and returns a pointer to the item.
//> szKeyValue is the key to find and delete
//< returns a pointer to the item if found.  The caller should manage the
//  item, deleting it when finished.
//> returns null if not found
//
template<class T>
T* vdbMap<T>::Orphan( const char* szKeyValue )
{
	// make a local copy of the key value because the caller's pointer may be a
	// member of the item to be deleted (which would cause it to point to an
	// invalid location).
	vdbString keyValue = vdbString( szKeyValue );

	// page 210		successorNode is c
	//				parent is p
	//				x is x
	//				connectChild is x
	//				pendingDelete is t

	_tail->_keyValue = keyValue;	// if the user-supplied key is not found, we end up at the tail

	vdbMapNode<T>* parent = _head;		// keep track of the parent of the trial node
	assert( parent != 0 );

	vdbMapNode<T>* x = _head->_right;	// all nodes are to the right of the head
	assert( x != 0 );

	while ( keyValue != x->_keyValue )
	{
		parent = x;
		x = ( keyValue < x->_keyValue ) ? x->_left : x->_right;
	}
	
	// if not found, just return
	if ( x == _tail )
		return NULL;

	vdbMapNode<T>* pendingDelete = x;			// the node that matches the user-supplied key
	assert( pendingDelete != _head );
	assert( pendingDelete != _tail );

	vdbMapNode<T>* connectChild = 0;			// the node to hook into the parent 

	// if the node to delete has no right child, just hook the left node into the parent
	if ( pendingDelete->_right == _tail )
		connectChild = pendingDelete->_left;

	// if the node to delete has a right child which has no left child, hook the
	// right node to the parent and hook the left node connecting child's left node.
	else if ( pendingDelete->_right->_left == _tail )
	{
		connectChild = pendingDelete->_right;
		connectChild->_left = pendingDelete->_left;
	}
	
	// all other cases, find the first node higher than the node to delete, copy that node's right link
	// to the left link of it's parent, and both of its links are assigned the pending delete node' links
	else
	{
		vdbMapNode<T>* successorNode = pendingDelete->_right;	// the first node after the one to be deleted
		while ( successorNode->_left->_left != _tail )
			successorNode = successorNode->_left;
		
		connectChild = successorNode->_left;
		successorNode->_left = connectChild->_right;
		connectChild->_left = pendingDelete->_left;
		connectChild->_right = pendingDelete->_right;
	}
	
	// now delete the node but not the underlying item
	T* pItem = pendingDelete->_pItem;
	pendingDelete->_pItem = 0;			// <-- Orphan
	delete pendingDelete; pendingDelete = 0;

	// finally, connect the child to the parent
	if ( keyValue < parent->_keyValue )
		parent->_left = connectChild;
	else
		parent->_right = connectChild;

	_size--;
	return pItem;
}


//--------------------------------------------
//^ The Remove function removes a single object from the map, deletes its
//  memory, and reconnects its parent, left and right nodes.
//> szKeyValue is the key to find and delete
//< returns true if deleted
//> returns false if not found
//
template<class T>
bool vdbMap<T>::Remove( const char* szKeyValue )
{
	T* pItem = Orphan( szKeyValue );
	if ( pItem == NULL )
		return false;
	else
	{
		delete pItem; pItem = 0;
		return true;
	}
}


//--------------------------------------------
//^ The ClearAll function deletes all objects from the map and leaves it ready
//  to accept new additions.
//
template<class T>
void vdbMap<T>::ClearAll()
{
	_head->CascadeDelete( _tail );

	_head->_right = _tail;

	_size = 0;	
}

	
//--------------------------------------------
//^ The GetInexact function finds items whose leading characters match the given
//  search string.
//> pMatches is an array of pointers to the matching items
//> maxMatches is the size of the pMatches array
//< returns the number of valid items in pMatches
//
template<class T>
int vdbMap<T>::GetInexact( const char* szSearch, T** pMatches, int maxMatches )
{
	int count = 0;
	vdbMapNode<T>* x = _head->_right;		// first real node is to the right of the head
	assert( x != 0 );

	// is the map empty?
	if ( x == _tail )
		return 0;

	// traverse finding all matches
	GetNextInexact( x, szSearch, pMatches, maxMatches, count );

	return count;
}


//--------------------------------------------
//^ The GetNextInexact function traverses the tree to get the next node
//> pNode is the current node
//
template<class T>
void vdbMap<T>::GetNextInexact( vdbMapNode<T>* pNode, const char* szSearch, T** pMatches, int maxMatches, int& countMatches )
{
	assert( pNode != 0 );
	assert( pNode != _head );
	assert( pNode != _tail );
	
	int result = strncmp( pNode->_keyValue, szSearch, strlen(szSearch) );

	if ( pNode->_left != _tail && result >= 0 && countMatches < maxMatches )
		GetNextInexact( pNode->_left, szSearch, pMatches, maxMatches, countMatches );

	if ( result == 0 && countMatches < maxMatches )
	{
		pMatches[countMatches] = pNode->_pItem;
		countMatches++;
	}
		
	if ( pNode->_right != _tail && result <= 0 && countMatches < maxMatches )
		GetNextInexact( pNode->_right, szSearch, pMatches, maxMatches, countMatches );

	assert( countMatches <= maxMatches );
}


//=============================================================================
// Non-iterator traversal functions
//=============================================================================

//-------------------------------------
//^ The TraverseMap function is called by the user to loop through the entire map
//  from start to finish.  At each node, the function callx back to the user
//  suplying the current key value, current T item, and a user-defined callback value
//> pCallback is th user-defined function
//> pCallbackCookie is the optional user value to pass through
//< returns true if everything was traversed, false if the callback function
//  stopped the traversal.
//
template<class T>
bool vdbMap<T>::TraverseMap( TraverseMapCallback pCallback, void* pCallbackCookie )
{
	assert( pCallback != NULL);

	vdbMapNode<T>* x = _head->_right;		// first real node is to the right of the head
	
	// check for an empty map
	if ( x == _tail )
		return true;

	return TraverseNext( x, pCallback, pCallbackCookie );
}


//-------------------------------------
//^ The TraverseNext function is the private internal function for traversals.
//  It is called recursively until it reaches the ned of the map or until the
//  user-defined callback returns false
//> pNode is the current map node
template<class T>
bool vdbMap<T>::TraverseNext( vdbMapNode<T>* pNode, TraverseMapCallback pCallback, void* pCallbackCookie )
{
	assert( pNode != 0 );
	assert( pNode != _head );
	assert( pNode != _tail );
	
	if ( pNode->_left != _tail )
	{
		if ( TraverseNext( pNode->_left, pCallback, pCallbackCookie ) == false )
			return false;
	}

	if ( pCallback( pNode->_keyValue, pNode->_pItem, pCallbackCookie ) == false )
		return false;
	
	if ( pNode->_right != _tail )
	{
		if ( TraverseNext( pNode->_right, pCallback, pCallbackCookie ) == false )
			return false;
	}
	
	return true;
}


//=============================================================================
// vdbMap iterator management functions
//=============================================================================

//--------------------------------------------
//^ The CreateIterator function creates an iterator suitable for traversing the
//  map.  It must be destroyed by calling DestroyIterator.
//
template<class T>
vdbMapIterator<T>* vdbMap<T>::CreateIterator()
{
	vdbMapIterator<T>* pIterator = new vdbMapIterator<T>( *this );
	if ( pIterator == 0 ) throw vdbMemoryException();
	_registeredIterators.Add( pIterator );
	return pIterator;	
}


//--------------------------------------------
//^ The DestroyIterator function destroys an iterator created by the
//  CreateIterator function.
//< returns true if deleted, returns false if not found in the list of
//  registered iterators.
//
template<class T>
bool vdbMap<T>::DestroyIterator( vdbMapIterator<T>* pIterator )
{
	for ( int i = 0; i < _registeredIterators.Count(); i++ )
	{
		if ( &(_registeredIterators[i]) == pIterator )
		{
			_registeredIterators.Remove(i);	
			return true;
		}
	}
	return false;
}


//=============================================================================
// vdbMapIterator essential functions
//=============================================================================

//--------------------------------------------
//^^ vdbMapIterator constructor
template <class T>
vdbMapIterator<T>::vdbMapIterator( vdbMap<T>& map )
	: _map( map ),
	  _currentIndex( 0 ),
	  _valid( false ),
	  _count( 0 ),
	  _pOrderedNodes( 0 )
{
	_count = map.Count();
	if ( _count == 0 )
		return;

	// create an array of vdbMapNode*'s to hold the addresses of all nodes in the map
	_pOrderedNodes = new vdbMapNode<T>*[_count];
	if ( _pOrderedNodes == 0 ) throw vdbMemoryException();

	// traverse the tree and fill the array with the node addresses
	vdbMapNode<T>* x = _map._head->_right;		// first real node is to the right of the head
	int index = 0;
	Traverse( x, index );
	
	_valid = true;
}


//--------------------------------------------
//^^ vdbMapIterator destructor
template <class T>
vdbMapIterator<T>::~vdbMapIterator()
{
	delete[] _pOrderedNodes; _pOrderedNodes = 0;
}


//--------------------------------------------
//^^ vdbMapIterator copy constructor
template <class T>
vdbMapIterator<T>::vdbMapIterator( const vdbMapIterator<T>& rhs )
	: _map( rhs._map )
{
	// Copy constructor for vdbMapIterator is disabled.  Call vdbMap::CreateIterator
	assert( false );
}


//--------------------------------------------
//^^ vdbMapIterator assignment operator
template <class T>
vdbMapIterator<T>& vdbMapIterator<T>::operator=( const vdbMapIterator<T>& rhs )
{
	// Assignment operator for vdbMapIterator is disabled.  Call vdbMap::CreateIterator
	assert( false );
	return *this;
}


//=============================================================================
// vdbMapIterator public member functions
//=============================================================================

//--------------------------------------------
//^ The First function positions the iterator to the first record in the map.
//< The iterator points to the first item in the map unless the map is empty,
//  in which case it is flagged as invalid.
//
template <class T>
void vdbMapIterator<T>::First()
{
	_currentIndex = 0;
	
	_valid = _count > 0 ? true : false;
}


//--------------------------------------------
//^ The Last function positions the iterator to the last record in the map.
//< The iterator points to the last item in the map unless the map is empty,
//  in which case it is flagged as invalid.
//
template <class T>
void vdbMapIterator<T>::Last()
{
	_currentIndex = _count - 1;
	
	_valid = _count > 0 ? true : false;
}


//--------------------------------------------
//^ The Next function positions the iterator at the next map item.
//^ Always call IsValid after calling this function and before using
//  the iterator.
//< If the iterator is beyond the end of the map, the iterator becomes invalid.
template <class T>
void vdbMapIterator<T>::Next()
{
	_currentIndex++;
	
	_valid = _currentIndex < _count ? true : false;
}


//--------------------------------------------
//^ The Prev function positions the iterator at the previous map item.
//^ Always call IsValid after calling this function and before using
//  the iterator.
//< If the iterator is beyond the beginning of the map, the iterator becomes
//  invalid.
template <class T>
void vdbMapIterator<T>::Prev()
{
	_currentIndex--;
	
	_valid = _currentIndex >= 0 ? true : false;
}


//--------------------------------------------
//^ The Current function returns a pointer to the current map item.
//< returns 0 if the iterator does not point to a valid item (i.e. it is past
//  the end of the map.)
template <class T>
T* vdbMapIterator<T>::Current()
{
	if ( _valid == false )
		return 0;

	return _pOrderedNodes[_currentIndex]->_pItem;	
}


//--------------------------------------------
//^ The CurrentKey function returns the character string key associated with
//  the currnent map item.
//< returns 0 if the iterator does not point to a valid item (i.e. it is past
//  the end of the map.)
template <class T>
const char* vdbMapIterator<T>::CurrentKey()
{
	if ( _valid == false )
		return 0;

	return _pOrderedNodes[_currentIndex]->_keyValue;	
}


//=============================================================================
// vdbMapIterator private member functions
//=============================================================================

//--------------------------------------------
//^ The Traverse function is called to recursively traverse the tree in order to
//  build the ordered array of nodes.  This function is called only by the
//  constructor.
//
template <class T>
void vdbMapIterator<T>::Traverse( vdbMapNode<T>* pNode, int& index )
{
	assert( pNode != 0 );
	assert( pNode != _map._head );
	assert( pNode != _map._tail );
	
	if ( pNode->_left != _map._tail )
		Traverse( pNode->_left, index );

	_pOrderedNodes[index] = pNode;
	index++;
	
	if ( pNode->_right != _map._tail )
		Traverse( pNode->_right, index );
}



//=============================================================================
// Essential vdbMapNode class functions
//=============================================================================

//--------------------------------------------
//^^ vdbMapNode constructor
//
template<class T>
vdbMapNode<T>::vdbMapNode( T* pItem, vdbString& s, redBlackType rb, vdbMapNode<T>* left, vdbMapNode<T>* right )
	: _pItem( pItem ),
	  _keyValue( s ),
	  _rb( rb ),
	  _left( left ),
	  _right( right )
	{}


//--------------------------------------------
//^^ vdbMapNode destructor
//^  The destructor for a node deletes the item of type T that it owns.
//
template<class T>
vdbMapNode<T>::~vdbMapNode()
{
	delete _pItem; _pItem = 0;
}


//--------------------------------------------
//^ The CascadeDelete function deletes all nodes to the left and right of this
//  node but not itself.  This is accomplished through a recusive set of calls.
//> pTail tail node.  This tail node is used to terminate the recursion.
//
template<class T>
void vdbMapNode<T>::CascadeDelete( vdbMapNode<T>* pTail )
{
	if ( _left != 0 && _left != pTail )
	{
		_left->CascadeDelete( pTail );
		delete _left; _left = 0;
	}
	if ( _right != 0 && _right != pTail )
	{
		_right->CascadeDelete( pTail );
		delete _right; _right = 0;
	}
}		


