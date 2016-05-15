#include <iostream>
#include <stack>
#include <queue>

using namespace std;
template <class T> class binaryNode;
template <class T> class binaryTree;


template <class T> class binaryNode
{
	friend class binaryTree <T>;
	T data;
	binaryNode <T> *lChild, *rChild;

	public:
		binaryNode(T& inpdata)
		{
			data = inpdata;
			lChild = NULL;
			rChild = NULL;
		}

		binaryNode <T> * getLeftChild() { return lChild;}
		binaryNode <T> * getRightChild() { return rChild;}

		void setLeftChild(binaryNode <T> *inp){ lChild = inp;};
		void setRighttChild(binaryNode <T> *inp){ rChild = inp;};
		void print(){ cout << data << endl;} 
};


template <class T> class binaryTree
{
	binaryNode <T> *root = NULL;

	public:
		binaryTree()
		{
			root = NULL;
		}

		binaryTree(T data)
		{
			root = new binaryNode <T> (data);
		}

		// Copy constructor
		binaryTree( const binaryTree & cp)
		{

			if(cp.root != NULL)
			{
				// Do a BFS walk on the tree, copy nodes
				queue < binaryNode<T> *> q;
				q.push(cp.root);

				while(!q.empty())
				{
					binaryNode <T> *localNode = q.front();
					q.pop();

					if ((*localNode).lChild != NULL)
					{
						q.push((*localNode).lChild);
					}

					if ((*localNode).rChild != NULL)
					{
						q.push((*localNode).rChild);
					}

					add((*localNode).data);
				}
			}
		}

		binaryTree & operator = (const binaryTree & cp)
		{
			if(root!=NULL)
			{
				// Do a BFS walk on the tree, delete nodes
				queue < binaryNode<T> *> q;
				q.push(root);

				while(!q.empty())
				{
					binaryNode <T> *localNode = q.front();
					q.pop();

					if ((*localNode).lChild != NULL)
					{
						q.push((*localNode).lChild);
					}

					if ((*localNode).rChild != NULL)
					{
						q.push((*localNode).rChild);
					}

					delete localNode;
				}
			}
			root = NULL;

			if (cp.root != NULL)
			{
				// Do a BFS walk on the tree, copy nodes
				queue < binaryNode<T> *> q;
				q.push(cp.root);

				while(!q.empty())
				{
					binaryNode <T> *localNode = q.front();
					q.pop();

					if ((*localNode).lChild != NULL)
					{
						q.push((*localNode).lChild);
					}

					if ((*localNode).rChild != NULL)
					{
						q.push((*localNode).rChild);
					}

					add((*localNode).data);
				}
			}
		
			return *this;
		}



		// Destructor
		~binaryTree()
		{
			if(root!=NULL)
			{
				// Do a BFS walk on the tree, delete nodes
				queue < binaryNode<T> *> q;
				q.push(root);

				while(!q.empty())
				{
					binaryNode <T> *localNode = q.front();
					q.pop();

					if ((*localNode).lChild != NULL)
					{
						q.push((*localNode).lChild);
					}

					if ((*localNode).rChild != NULL)
					{
						q.push((*localNode).rChild);
					}

					delete localNode;
				}
			}
		}


		void add(T data)
		{
			binaryNode <T> * dataNode = new binaryNode <T> (data);
			if (root == NULL)
			{	
				root = dataNode;
			}

			else
			{
				// Do a BFS walk on the tree, and add the node when you find the first null leaf.
				queue <binaryNode<T> *> q;
				q.push(root);

				while(!q.empty())
				{
					binaryNode <T> *localNode = q.front(); q.pop();

					if ((*localNode).lChild == NULL)
					{
						(*localNode).lChild = dataNode;
						return;
					}
					else if((*localNode).rChild == NULL)
					{
						(*localNode).rChild = dataNode;
						return;
					}
					else
					{
						q.push((*localNode).lChild);
						q.push((*localNode).rChild);
					}
				}
			}
		}


		void bfsPrint()
		{	
			// Do a BFS walk on the tree, print nodes
			queue < binaryNode<T> *> q;
			q.push(root);

			while(!q.empty())
			{
				binaryNode <T> *localNode = q.front(); q.pop();

				if ((*localNode).lChild != NULL)
				{
					q.push((*localNode).lChild);
				}

				if ((*localNode).rChild != NULL)
				{
					q.push((*localNode).rChild);
				}

				(*localNode).print();
			}
		}
};



int main()
{
	binaryTree <int> tr;	
	int i = 0;
	for(i=0; i<10; i++)
	{
		tr.add(i);
	}
	tr.bfsPrint();

	// Test copy constructor
	binaryTree <int> tr1(tr);
	tr1.bfsPrint();
	
	// Test copy assignment
	binaryTree <int> tr2;
	tr2 = tr1;
	tr2.bfsPrint();

	binaryTree <int> tr3;
	tr2 = tr3;
	binaryTree <int> tr4(tr3);
	return 1;
}
