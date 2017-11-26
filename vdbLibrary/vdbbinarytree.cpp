//=============================================================================
//
// CLASS:           vdbBinaryTree
//
// OVERVIEW:		The vdbBinaryTree class is used to manage a balanced red-black
//                  tree.  The class used for template parameter K must have 
//                  a copy constructor, an operator <, and an operator !=.
//
//                  The default constructor for K must produce an object which is
//                  guaranteed to be at the top of the sort order.  Typically this
//                  means setting the key value to NULL or 0 or using a zero-length
//                  string.
//
//                  Direct access to items are available through the Get
//                  function.  Sequential access is provided through the
//                  vdbBinaryTreeIterator functions First, Last, Next and Prev.
//
// ALGORITHM:		"Algorithms in C++" by Robert Sedgewick, 1992,
//                  pp. 203-211, 221-226.
//
// CODE BASE:       Taken from vdbBinaryTree and modified.
//
// FILE NAME:       vdbBinaryTree.cpp
//
// INITIAL DATE:    1/13/99
//
// COPYRIGHT:		Copyright © 1999 by Joe Honton
//
//=============================================================================

#include "vdbBinaryTree.h"
#include <vdbAssert.h>
#include "vdbException.h"


/*
	variables as they appear in Sedgewick:

	_tail is z
	_head is head
	_rb is b
	_left is l
	_right is r
*/

//=============================================================================
// Essential vdbBinaryTree class functions
//=============================================================================

//--------------------------------------------
//^^ The constructor
//
template<class K>
vdbBinaryTree<K>::vdbBinaryTree()
	: _size( 0 ),
	  _head( 0 ),
	  _tail( 0 )
{
	_tail = new vdbBinaryTreeNode<K>( K(), vdbBinaryTreeNode<K>::BLACK, 0, 0 );
	if ( _tail == 0 ) throw vdbMemoryException();
	_tail->_left = _tail;
	_tail->_right = _tail;

	// the head node must have a key value guaranteed to be the smallest because
	// all new nodes are added to the right-hand side of the head.  The left-hand
	// side is always 0
	_head = new vdbBinaryTreeNode<K>( K(), vdbBinaryTreeNode<K>::BLACK, 0, _tail );	
	if ( _head == 0 ) throw vdbMemoryException();
}
	

//--------------------------------------------
//^^ The destructor
template<class K>
vdbBinaryTree<K>::~vdbBinaryTree()
{
	// Deleting the head causes a cascade delete of all nodes in the tree.
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
template<class K>
vdbBinaryTree<K>::vdbBinaryTree( const vdbBinaryTree<K>& rhs )
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
template<class K>
vdbBinaryTree<K>& vdbBinaryTree<K>::operator=( const vdbBinaryTree<K>& rhs )
{
	if ( this == &rhs )
		return *this;
	
	assert( false ); // Not yet implemented

	return *this;
}


//=============================================================================
// Public vdbBinaryTree member functions
//=============================================================================
/*	Sedgewick	vdbBinaryTree
	insert()	Add()
	p			parent
	x			x
	v			keyValue
	info		pItem
*/
//--------------------------------------------
//^ The Add function adds a new node to the binary tree.  Duplicates are permissible.
//> keyValue is the object to add to the tree
//
template<class K>
void vdbBinaryTree<K>::Add( K& keyValue )
{
	assert( _head != 0 );

	vdbBinaryTreeNode<K>* x = _head;			// the trial node
	vdbBinaryTreeNode<K>* parent = _head;
	vdbBinaryTreeNode<K>* grandparent = _head;
	vdbBinaryTreeNode<K>* greatgrandparent = 0;

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
	
	vdbBinaryTreeNode<K>* newNode = new vdbBinaryTreeNode<K>( keyValue, vdbBinaryTreeNode<K>::RED, _tail, _tail );
	if ( newNode == 0 ) throw vdbMemoryException();
	if ( keyValue < parent->_keyValue )
		parent->_left = newNode;
	else
		parent->_right = newNode;

	Split( keyValue, newNode, parent, grandparent, greatgrandparent );
	_head->_right->SetToBlack();

	_size++;
}


/*	Sedgewick	vdbBinaryTree
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
template<class K>
void vdbBinaryTree<K>::Split( K& keyValue, vdbBinaryTreeNode<K>*& x, vdbBinaryTreeNode<K>*& parent, vdbBinaryTreeNode<K>*& grandparent,vdbBinaryTreeNode<K>*& greatgrandparent )
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


/*	Sedgewick	vdbBinaryTree
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
template<class K>
vdbBinaryTreeNode<K>* vdbBinaryTree<K>::Rotate( K& keyValue, vdbBinaryTreeNode<K>* y )
{
	vdbBinaryTreeNode<K>* child;
	vdbBinaryTreeNode<K>* grandchild;

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
//^ The Exists returns true if the keyValue exists in the tree or false if it
//  does not.
//> keyValue is the object to lookup
//
template<class K>
bool vdbBinaryTree<K>::Exists( K& keyValue )
{
	vdbBinaryTreeNode<K>* x = _head->_right;	// all nodes are to the right of the head
	assert( x != 0 );

	_tail->_keyValue = keyValue;				// search will terminate if it reaches the tail

	while ( keyValue != x->_keyValue )
		x = ( keyValue < x->_keyValue ) ? x->_left : x->_right;
	
	if ( x == _tail )
		return false;
	else
		return true;
}


//--------------------------------------------
//^ The Depth function returns a count of the number of nodes deep the
//  specified key Value is.
//> keyValue is the character string lookup value
//
template<class K>
int vdbBinaryTree<K>::Depth( K& keyValue )
{
	vdbBinaryTreeNode<K>* x = _head->_right;	// all nodes are to the right of the head
	assert( x != 0 );

	_tail->_keyValue = keyValue;				// search will terminate if it reaches the tail

	int depth = 0;
	while ( keyValue != x->_keyValue )
	{
		x = ( keyValue < x->_keyValue ) ? x->_left : x->_right;
		depth++;
	}
	
	return depth;
}


//--------------------------------------------
//^ The Remove function removes a single object from the map, deletes its
//  memory, and reconnects its parent, left and right nodes.
//> keyValue is the key to find and delete
//< returns true if deleted
//> returns false if not found
//
template<class K>
bool vdbBinaryTree<K>::Remove( K& keyVal )
{
	// make a local copy of the key value because the caller's reference may be a
	// member of this binary tree (which would cause the referenced item to point to an
	// invalid location near the end of this function).
	K keyValue( keyVal );

	// page 210		successorNode is c
	//				parent is p
	//				x is x
	//				connectChild is x
	//				pendingDelete is t

	_tail->_keyValue = keyValue;	// if the user-supplied key is not found, we end up at the tail

	vdbBinaryTreeNode<K>* parent = _head;		// keep track of the parent of the trial node
	assert( parent != 0 );

	vdbBinaryTreeNode<K>* x = _head->_right;	// all nodes are to the right of the head
	assert( x != 0 );

	while ( keyValue != x->_keyValue )
	{
		parent = x;
		x = ( keyValue < x->_keyValue ) ? x->_left : x->_right;
	}
	
	// if not found, just return
	if ( x == _tail )
		return false;

	vdbBinaryTreeNode<K>* pendingDelete = x;		// the node that matches the user-supplied key
	assert( pendingDelete != _head );
	assert( pendingDelete != _tail );

	vdbBinaryTreeNode<K>* connectChild = 0;			// the node to hook into the parent 

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
		vdbBinaryTreeNode<K>* successorNode = pendingDelete->_right;	// the first node after the one to be deleted
		while ( successorNode->_left->_left != _tail )
			successorNode = successorNode->_left;
		
		connectChild = successorNode->_left;
		successorNode->_left = connectChild->_right;
		connectChild->_left = pendingDelete->_left;
		connectChild->_right = pendingDelete->_right;
	}
	
	// now delete the node
	delete pendingDelete; pendingDelete = 0;

	// finally, connect the child to the parent
	if ( keyValue < parent->_keyValue )
		parent->_left = connectChild;
	else
		parent->_right = connectChild;

	_size--;
	return true;
}


//--------------------------------------------
//^ The ClearAll function deletes all objects from the map and leaves it ready
//  to accept new additions.
//
template<class K>
void vdbBinaryTree<K>::ClearAll()
{
	_head->CascadeDelete( _tail );

	_head->_right = _tail;

	_size = 0;	
}

	
//=============================================================================
// vdbBinaryTree iterator management functions
//=============================================================================

//--------------------------------------------
//^ The CreateIterator function creates an iterator suitable for traversing the
//  map.  It must be destroyed by calling DestroyIterator.
//
template<class K>
vdbBinaryTreeIterator<K>* vdbBinaryTree<K>::CreateIterator()
{
	vdbBinaryTreeIterator<K>* pIterator = new vdbBinaryTreeIterator<K>( *this );
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
template<class K>
bool vdbBinaryTree<K>::DestroyIterator( vdbBinaryTreeIterator<K>* pIterator )
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
// vdbBinaryTreeIterator essential functions
//=============================================================================

//--------------------------------------------
//^^ vdbBinaryTreeIterator constructor
template <class K>
vdbBinaryTreeIterator<K>::vdbBinaryTreeIterator( vdbBinaryTree<K>& map )
	: _map( map ),
	  _currentIndex( 0 ),
	  _valid( false ),
	  _count( 0 ),
	  _pOrderedNodes( 0 )
{
	_count = map.Count();
	if ( _count == 0 )
		return;

	// create an array of vdbBinaryTreeNode*'s to hold the addresses of all nodes in the map
	_pOrderedNodes = new vdbBinaryTreeNode<K>*[_count];
	if ( _pOrderedNodes == 0 ) throw vdbMemoryException();

	// traverse the tree and fill the array with the node addresses
	vdbBinaryTreeNode<K>* x = _map._head->_right;		// first real node is to the right of the head
	int index = 0;
	Traverse( x, index );
	
	_valid = true;
}


//--------------------------------------------
//^^ vdbBinaryTreeIterator destructor
template <class K>
vdbBinaryTreeIterator<K>::~vdbBinaryTreeIterator()
{
	delete[] _pOrderedNodes; _pOrderedNodes = 0;
}


//--------------------------------------------
//^^ vdbBinaryTreeIterator copy constructor
template <class K>
vdbBinaryTreeIterator<K>::vdbBinaryTreeIterator( const vdbBinaryTreeIterator<K>& rhs )
	: _map( rhs._map )
{
	// Copy constructor for vdbBinaryTreeIterator is disabled.  Call vdbBinaryTree::CreateIterator
	assert( false );
}


//--------------------------------------------
//^^ vdbBinaryTreeIterator assignment operator
template <class K>
vdbBinaryTreeIterator<K>& vdbBinaryTreeIterator<K>::operator=( const vdbBinaryTreeIterator<K>& rhs )
{
	// Assignment operator for vdbBinaryTreeIterator is disabled.  Call vdbBinaryTree::CreateIterator
	assert( false );
	return *this;
}


//=============================================================================
// vdbBinaryTreeIterator public member functions
//=============================================================================

//--------------------------------------------
//^ The First function positions the iterator to the first record in the map.
//< The iterator points to the first item in the map unless the map is empty,
//  in which case it is flagged as invalid.
//
template <class K>
void vdbBinaryTreeIterator<K>::First()
{
	_currentIndex = 0;
	
	_valid = _count > 0 ? true : false;
}


//--------------------------------------------
//^ The Last function positions the iterator to the last record in the map.
//< The iterator points to the last item in the map unless the map is empty,
//  in which case it is flagged as invalid.
//
template <class K>
void vdbBinaryTreeIterator<K>::Last()
{
	_currentIndex = _count - 1;
	
	_valid = _count > 0 ? true : false;
}


//--------------------------------------------
//^ The Next function positions the iterator at the next map item.
//^ Always call IsValid after calling this function and before using
//  the iterator.
//< If the iterator is beyond the end of the map, the iterator becomes invalid.
template <class K>
void vdbBinaryTreeIterator<K>::Next()
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
template <class K>
void vdbBinaryTreeIterator<K>::Prev()
{
	_currentIndex--;
	
	_valid = _currentIndex >= 0 ? true : false;
}


//--------------------------------------------
//^ The Current function returns a pointer to the current binary tree item.
//< returns a default K if the iterator does not point to a valid item
//  (i.e. it is past the end of the map.)
template <class K>
K& vdbBinaryTreeIterator<K>::Current()
{
	if ( _valid == false )
		throw vdbException( "Attempt to access invalid tree item" );

	return _pOrderedNodes[_currentIndex]->_keyValue;	
}


//=============================================================================				
// vdbBinaryTreeIterator private member functions
//=============================================================================

//--------------------------------------------
//^ The Traverse function is called to recursively traverse the tree in order to
//  build the ordered array of nodes.  This function is called only by the
//  constructor.
//
template <class K>
void vdbBinaryTreeIterator<K>::Traverse( vdbBinaryTreeNode<K>* pNode, int& index )
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
// Essential vdbBinaryTreeNode class functions
//=============================================================================

//--------------------------------------------
//^^ vdbBinaryTreeNode constructor
//
template<class K>
vdbBinaryTreeNode<K>::vdbBinaryTreeNode( K& keyValue, redBlackType rb, vdbBinaryTreeNode<K>* left, vdbBinaryTreeNode<K>* right )
	: _keyValue( keyValue ),
	  _rb( rb ),
	  _left( left ),
	  _right( right )
	{}


//--------------------------------------------
//^^ vdbBinaryTreeNode destructor
//^  The destructor for a node
//
template<class K>
vdbBinaryTreeNode<K>::~vdbBinaryTreeNode()
{
}


//--------------------------------------------
//^ The CascadeDelete function deletes all nodes to the left and right of this
//  node but not itself.  This is accomplished through a recusive set of calls.
//> pTail tail node.  This tail node is used to terminate the recursion.
//
template<class K>
void vdbBinaryTreeNode<K>::CascadeDelete( vdbBinaryTreeNode<K>* pTail )
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


