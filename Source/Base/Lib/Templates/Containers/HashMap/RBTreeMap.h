/*
=============================================================================
	File:	RBTreeMap.h
	Desc:	Template for associative arrays using a red-black tree.
	Note:	Swiped from Irrlicht.
=============================================================================
*/

#ifndef __RB_TREE_MAP_H__
#define __RB_TREE_MAP_H__

namespace abc {

//
//	RBTreeMap< class KeyType, class ValueType >
//
template< class KeyType, class ValueType >
class RBTreeMap
{
	// Red/Black tree for RBTreeMap.
	template< class KeyTypeRB, class ValueTypeRB >
	class RBTree
	{
	public:
		RBTree( const KeyTypeRB& k, const ValueTypeRB& v)
			: LeftChild(0), RightChild(0), Parent(0), Key(k),
				Value(v), IsRed(true)
		{}

		void setLeftChild(RBTree* p)
		{
			LeftChild=p;
			if (p)
				p->setParent(this);
		}

		void setRightChild(RBTree* p)
		{
			RightChild=p;
			if (p)
				p->setParent(this);
		}

		void setParent(RBTree* p)		{ Parent=p; }

		void setValue( const ValueTypeRB& v)	{ Value = v; }

		void setRed()			{ IsRed = true; }
		void setBlack()			{ IsRed = false; }

		RBTree* getLeftChild() const	{ return LeftChild; }
		RBTree* getRightChild() const	{ return RightChild; }
		RBTree* getParent() const	{ return Parent; }

		ValueTypeRB GetValue() const
		{
			return Value;
		}

		KeyTypeRB GetKey() const
		{
			return Key;
		}

		bool isRoot() const
		{
			return Parent==0;
		}

		bool isLeftChild() const
		{
			return (Parent != 0) && (Parent->getLeftChild()==this);
		}

		bool isRightChild() const
		{
			return (Parent!=0) && (Parent->getRightChild()==this);
		}

		bool isLeaf() const
		{
			return (LeftChild==0) && (RightChild==0);
		}

		mxUInt getLevel() const
		{
			if (isRoot())
				return 1;
			else
				return getParent()->getLevel() + 1;
		}


		bool isRed() const
		{
			return IsRed;
		}

		bool isBlack() const
		{
			return !IsRed;
		}

	private:
		RBTree();

		RBTree *	LeftChild;
		RBTree *	RightChild;

		RBTree *	Parent;

		KeyTypeRB	Key;
		ValueTypeRB	Value;

		bool	IsRed;
	}; // RBTree

public:

	typedef RBTree<KeyType,ValueType> Node;

	// Normal Iterator
	class Iterator
	{
	public:

		Iterator() : Root(0), Cur(0) {}

		// Constructor(Node*)
		Iterator(Node* root) : Root(root)
		{
			Reset();
		}

		// Copy constructor
		Iterator( const Iterator& src) : Root(src.Root), Cur(src.Cur) {}

		void Reset( bool atLowest = true )
		{
			if ( atLowest ) {
				Cur = getMin(Root);
			} else {
				Cur = getMax(Root);
			}
		}

		bool atEnd() const
		{
			
			return Cur==0;
		}

		Node* getNode()
		{
			return Cur;
		}

		Iterator& operator=( const Iterator& src)
		{
			Root = src.Root;
			Cur = src.Cur;
			return (*this);
		}

		void operator++(INT)
		{
			inc();
		}

		void operator--(INT)
		{
			dec();
		}


		Node* operator -> ()
		{
			return getNode();
		}

		Node& operator* ()
		{
			if (atEnd())
				throw "Iterator at end";

			return *Cur;
		}

	private:

		Node* getMin(Node* n)
		{
			while(n && n->getLeftChild())
				n = n->getLeftChild();
			return n;
		}

		Node* getMax(Node* n)
		{
			while(n && n->getRightChild())
				n = n->getRightChild();
			return n;
		}

		void inc()
		{
			// Already at end?
			if (Cur==0)
				return;

			if (Cur->getRightChild())
			{
				// If current node has a right child, the next higher node is the
				// node with lowest key beneath the right child.
				Cur = getMin(Cur->getRightChild());
			}
			else if (Cur->isLeftChild())
			{
				// No right child? Well if current node is a left child then
				// the next higher node is the parent
				Cur = Cur->getParent();
			}
			else
			{
				// Current node neither is left child nor has a right child.
				// Ie it is either right child or root
				// The next higher node is the parent of the first non-right
				// child (ie either a left child or the root) up in the
				// hierarchy. Root's parent is 0.
				while(Cur->isRightChild())
					Cur = Cur->getParent();
				Cur = Cur->getParent();
			}
		}

		void dec()
		{
			// Already at end?
			if (Cur==0)
				return;

			if (Cur->getLeftChild())
			{
				// If current node has a left child, the next lower node is the
				// node with highest key beneath the left child.
				Cur = getMax(Cur->getLeftChild());
			}
			else if (Cur->isRightChild())
			{
				// No left child? Well if current node is a right child then
				// the next lower node is the parent
				Cur = Cur->getParent();
			}
			else
			{
				// Current node neither is right child nor has a left child.
				// Ie it is either left child or root
				// The next higher node is the parent of the first non-left
				// child (ie either a right child or the root) up in the
				// hierarchy. Root's parent is 0.

				while(Cur->isLeftChild())
					Cur = Cur->getParent();
				Cur = Cur->getParent();
			}
		}

		Node* Root;
		Node* Cur;
	}; // Iterator



	// Parent First Iterator.
	/** Traverses the tree from top to bottom. Typical usage is
	when storing the tree structure, because when reading it
	later (and inserting elements) the tree structure will
	be the same. */
	class ParentFirstIterator
	{
	public:


	ParentFirstIterator() : Root(0), Cur(0)
	{
	}


	explicit ParentFirstIterator(Node* root) : Root(root), Cur(0)
	{
		Reset();
	}

	void Reset()
	{
		Cur = Root;
	}


	bool atEnd() const
	{
		
		return Cur==0;
	}

	Node* getNode()
	{
		return Cur;
	}


	ParentFirstIterator& operator=( const ParentFirstIterator& src)
	{
		Root = src.Root;
		Cur = src.Cur;
		return (*this);
	}


	void operator++(INT)
	{
		inc();
	}


	Node* operator -> ()
	{
		return getNode();
	}

	Node& operator* ()
	{
		if (atEnd())
			throw "ParentFirstIterator at end";
		return *getNode();
	}

	private:

	void inc()
	{
		// Already at end?
		if (Cur==0)
			return;

		// First we try down to the left
		if (Cur->getLeftChild())
		{
			Cur = Cur->getLeftChild();
		}
		else if (Cur->getRightChild())
		{
			// No left child? The we go down to the right.
			Cur = Cur->getRightChild();
		}
		else
		{
			// No children? Move up in the hierarcy until
			// we either reach 0 (and are finished) or
			// find a right uncle.
			while (Cur!=0)
			{
				// But if parent is left child and has a right "uncle" the parent
				// has already been processed but the uncle hasn't. Move to
				// the uncle.
				if (Cur->isLeftChild() && Cur->getParent()->getRightChild())
				{
					Cur = Cur->getParent()->getRightChild();
					return;
				}
				Cur = Cur->getParent();
			}
		}
	}

	Node* Root;
	Node* Cur;

	}; // ParentFirstIterator


	// Parent Last Iterator
	/** Traverse the tree from bottom to top.
	Typical usage is when deleting all elements in the tree
	because you must delete the children before you delete
	their parent. */
	class ParentLastIterator
	{
	public:

		ParentLastIterator() : Root(0), Cur(0) {}

		explicit ParentLastIterator(Node* root) : Root(root), Cur(0)
		{
			Reset();
		}

		void Reset()
		{
			Cur = getMin(Root);
		}

		bool atEnd() const
		{
			
			return Cur==0;
		}

		Node* getNode()
		{
			return Cur;
		}

		ParentLastIterator& operator=( const ParentLastIterator& src)
		{
			Root = src.Root;
			Cur = src.Cur;
			return (*this);
		}

		void operator++(INT)
		{
			inc();
		}

		Node* operator -> ()
		{
			return getNode();
		}

		Node& operator* ()
		{
			if (atEnd())
				throw "ParentLastIterator at end";
			return *getNode();
		}
	private:

		Node* getMin(Node* n)
		{
			while(n!=0 && (n->getLeftChild()!=0 || n->getRightChild()!=0))
			{
				if (n->getLeftChild())
					n = n->getLeftChild();
				else
					n = n->getRightChild();
			}
			return n;
		}

		void inc()
		{
			// Already at end?
			if (Cur==0)
				return;

			// Note: Starting point is the node as far down to the left as possible.

			// If current node has an uncle to the right, go to the
			// node as far down to the left from the uncle as possible
			// else just go up a level to the parent.
			if (Cur->isLeftChild() && Cur->getParent()->getRightChild())
			{
				Cur = getMin(Cur->getParent()->getRightChild());
			}
			else
				Cur = Cur->getParent();
		}

		Node* Root;
		Node* Cur;
	}; // ParentLastIterator


	// AccessClass is a temporary class used with the [] operator.
	// It makes it possible to have different behavior in situations like:
	// myTree["Foo"] = 32;
	// If "Foo" already exists update its value else Insert a new element.
	// INT i = myTree["Foo"]
	// If "Foo" exists return its value, else throw an exception.
	class AccessClass
	{
		// Let RBTreeMap be the only one who can instantiate this class.
		friend class RBTreeMap<KeyType, ValueType>;

	public:

		// Assignment operator. Handles the myTree["Foo"] = 32; situation
		void operator=( const ValueType& value)
		{
			// Just use the Set method, it handles already exist/not exist situation
			Tree.set( Key, value );
		}

		// ValueType operator
		operator ValueType()
		{
			Node* node = Tree.Find(Key);

			// Not found
			if ( node == 0 ) {
				//throw "Item not found";
				sys::Error( "Item not found" );
			}
			
			return node->GetValue();
		}

	private:

		AccessClass(RBTreeMap& tree, const KeyType& key) : Tree(tree), Key(key) {}

		AccessClass();

		RBTreeMap& Tree;
		const KeyType& Key;
	}; // AccessClass


	// Constructor.
	RBTreeMap() : Root(0), Size(0) {}

	// Destructor
	~RBTreeMap()
	{
		Clear();
	}

	//------------------------------
	// Public Commands
	//------------------------------

	// Inserts a new node into the tree
	/** \param keyNew: the index for this value
	\param v: the value to Insert
	\return True if successful, false if it fails (already exists) */
	bool Insert( const KeyType& keyNew, const ValueType& v)
	{
		// First Insert node the "usual" way (no fancy balance logic yet)
		Node* newNode = new Node(keyNew,v);
		if (!Insert(newNode))
		{
			delete newNode;
			
			return false;
		}

		// Then attend a balancing party
		while (!newNode->isRoot() && (newNode->getParent()->isRed()))
		{
			if (newNode->getParent()->isLeftChild())
			{
				// If newNode is a left child, get its right 'uncle'
				Node* newNodesUncle = newNode->getParent()->getParent()->getRightChild();
				if ( newNodesUncle!=0 && newNodesUncle->isRed())
				{
					// case 1 - change the colours
					newNode->getParent()->setBlack();
					newNodesUncle->setBlack();
					newNode->getParent()->getParent()->setRed();
					// Move newNode up the tree
					newNode = newNode->getParent()->getParent();
				}
				else
				{
					// newNodesUncle is a black node
					if ( newNode->isRightChild())
					{
						// and newNode is to the right
						// case 2 - move newNode up and rotate
						newNode = newNode->getParent();
						rotateLeft(newNode);
					}
					// case 3
					newNode->getParent()->setBlack();
					newNode->getParent()->getParent()->setRed();
					rotateRight(newNode->getParent()->getParent());
				}
			}
			else
			{
				// If newNode is a right child, get its left 'uncle'
				Node* newNodesUncle = newNode->getParent()->getParent()->getLeftChild();
				if ( newNodesUncle!=0 && newNodesUncle->isRed())
				{
					// case 1 - change the colours
					newNode->getParent()->setBlack();
					newNodesUncle->setBlack();
					newNode->getParent()->getParent()->setRed();
					// Move newNode up the tree
					newNode = newNode->getParent()->getParent();
				}
				else
				{
					// newNodesUncle is a black node
					if (newNode->isLeftChild())
					{
						// and newNode is to the left
						// case 2 - move newNode up and rotate
						newNode = newNode->getParent();
						rotateRight(newNode);
					}
					// case 3
					newNode->getParent()->setBlack();
					newNode->getParent()->getParent()->setRed();
					rotateLeft(newNode->getParent()->getParent());
				}

			}
		}
		// Color the root black
		Root->setBlack();
		return true;
	}

	// Replaces the value if the key already exists, otherwise inserts a new element.
	/** \param k The index for this value
	\param v The new value of */
	void set( const KeyType& k, const ValueType& v)
	{
		Node* p = find(k);
		if (p)
			p->setValue(v);
		else
			Insert(k,v);
	}

	// Removes a node from the tree and returns it.
	/** The returned node must be deleted by the user
	\param k the key to remove
	\return A pointer to the node, or 0 if not found */
	Node* delink( const KeyType& k)
	{
		Node* p = find(k);
		if (p == 0)
			return 0;

		// Rotate p down to the left until it has no right child, will get there
		// sooner or later.
		while(p->getRightChild())
		{
			// "Pull up my right child and let it knock me down to the left"
			rotateLeft(p);
		}
		// p now has no right child but might have a left child
		Node* left = p->getLeftChild();

		// Let p's parent point to p's child instead of point to p
		if (p->isLeftChild())
			p->getParent()->setLeftChild(left);

		else if (p->isRightChild())
			p->getParent()->setRightChild(left);

		else
		{
			// p has no parent => p is the root.
			// Let the left child be the new root.
			setRoot(left);
		}

		// p is now gone from the tree in the sense that
		// no one is pointing at it, so return it.

		--Size;
		return p;
	}

	// Removes a node from the tree and deletes it.
	/** \return True if the node was found and deleted */
	bool remove( const KeyType& k)
	{
		Node* p = find(k);
		if (p == 0)
		{
			
			return false;
		}

		// Rotate p down to the left until it has no right child, will get there
		// sooner or later.
		while(p->getRightChild())
		{
			// "Pull up my right child and let it knock me down to the left"
			rotateLeft(p);
		}
		// p now has no right child but might have a left child
		Node* left = p->getLeftChild();

		// Let p's parent point to p's child instead of point to p
		if (p->isLeftChild())
			p->getParent()->setLeftChild(left);

		else if (p->isRightChild())
			p->getParent()->setRightChild(left);

		else
		{
			// p has no parent => p is the root.
			// Let the left child be the new root.
			setRoot(left);
		}

		// p is now gone from the tree in the sense that
		// no one is pointing at it. Let's get rid of it.
		delete p;

		--Size;
		return true;
	}

	// Clear the entire tree
	void Clear()
	{
		ParentLastIterator i(getParentLastIterator());

		while(!i.atEnd())
		{
			Node* p = i.getNode();
			i++; // Increment it before it is deleted
				// else iterator will get quite confused.
			delete p;
		}
		Root = 0;
		Size= 0;
	}

	// Is the tree empty?
	// \return Returns true if empty, false if not
	bool IsEmpty() const
	{
		
		return ( Root == 0 );
	}

	// Search for a node with the specified key.
	// \param keyToFind: The key to find
	// \return Returns 0 if node couldn't be found.
	Node* Find( const KeyType& keyToFind ) const
	{
		Node* pNode = Root;
		while( pNode != NULL )
		{
			KeyType  key( pNode->GetKey() );

			if ( keyToFind == key ) {
				return pNode;
			}
			else if ( keyToFind < key ) {
				pNode = pNode->getLeftChild();
			}
			else // keyToFind > key
			{
				pNode = pNode->getRightChild();
			}
		}
		return 0;
	}

	// Gets the root element.
	// \return Returns a pointer to the root node, or
	// 0 if the tree is empty.
	Node* getRoot() const
	{
		return Root;
	}

	// Returns the number of nodes in the tree.
	mxUInt size() const
	{
		return Size;
	}

	//------------------------------
	// Public Iterators
	//------------------------------

	// Returns an iterator
	Iterator getIterator()
	{
		Iterator it(getRoot());
		return it;
	}
	// Returns a ParentFirstIterator.
	// Traverses the tree from top to bottom. Typical usage is
	// when storing the tree structure, because when reading it
	// later (and inserting elements) the tree structure will
	// be the same.
	ParentFirstIterator getParentFirstIterator()
	{
		ParentFirstIterator it(getRoot());
		return it;
	}
	// Returns a ParentLastIterator to traverse the tree from
	// bottom to top.
	// Typical usage is when deleting all elements in the tree
	// because you must delete the children before you delete
	// their parent.
	ParentLastIterator getParentLastIterator()
	{
		ParentLastIterator it(getRoot());
		return it;
	}

	//------------------------------
	// Public Operators
	//------------------------------

	// operator [] for access to elements
	/** for example myMap["key"] */
	AccessClass operator[]( const KeyType& k)
	{
		return AccessClass(*this, k);
	}
	private:

	//------------------------------
	// Disabled methods
	//------------------------------
	// Copy constructor and assignment operator deliberately
	// defined but not implemented. The tree should never be
	// copied, pass along references to it instead.
	explicit RBTreeMap( const RBTreeMap& src);
	RBTreeMap& operator = ( const RBTreeMap& src);

	// Set node as new root.
	/** The node will be set to black, otherwise core dumps may arise
	(patch provided by rogerborg).
	\param newRoot Node which will be the new root
	*/
	void setRoot( Node* newRoot )
	{
		Root = newRoot;
		if ( Root != NULL )
		{
			Root->setParent(0);
			Root->setBlack();
		}
	}

	// Insert a node into the tree without using any fancy balancing logic.
	/** \return false if that key already exist in the tree. */
	bool Insert( Node* newNode )
	{
		bool result = true; // Assume success

		if ( Root == NULL )
		{
			setRoot(newNode);
			Size = 1;
		}
		else
		{
			Node* pNode = Root;
			KeyType keyNew = newNode->GetKey();
			while ( pNode )
			{
				KeyType key( pNode->GetKey() );

				if ( keyNew == key ) {
					result = false;
					pNode = 0;
				}
				else if ( keyNew < key)
				{
					if ( pNode->getLeftChild() == 0 )
					{
						pNode->setLeftChild( newNode );
						pNode = 0;
					}
					else
						pNode = pNode->getLeftChild();
				}
				else // keyNew > key
				{
					if ( pNode->getRightChild() == 0 )
					{
						pNode->setRightChild( newNode );
						pNode = 0;
					}
					else
					{
						pNode = pNode->getRightChild();
					}
				}
			}

			if ( result ) {
				++Size;
			}
		}

		return result;
	}

	// Rotate left.
	// Pull up node's right child and let it knock node down to the left
	void rotateLeft( Node* p )
	{
		Node* right = p->getRightChild();

		p->setRightChild( right->getLeftChild() );

		if ( p->isLeftChild() ) {
			p->getParent()->setLeftChild( right );
		}
		else if ( p->isRightChild() ) {
			p->getParent()->setRightChild( right );
		}
		else {
			setRoot( right );
		}

		right->setLeftChild( p );
	}

	// Rotate right.
	// Pull up node's left child and let it knock node down to the right
	void rotateRight( Node* p )
	{
		Node* left = p->getLeftChild();

		p->setLeftChild( left->getRightChild() );

		if (p->isLeftChild()) {
			p->getParent()->setLeftChild( left );
		}
		else if ( p->isRightChild() ) {
			p->getParent()->setRightChild( left );
		}
		else {
			setRoot( left );
		}

		left->setRightChild( p );
	}

	//------------------------------
	// Private Members
	//------------------------------
	Node *	Root; // The top node. 0 if empty.
	mxUInt	Size; // Number of nodes in the tree
};

}//End of namespace abc

#endif // ! __RB_TREE_MAP_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
