#pragma once
#include <iostream>
#include <queue> 
using namespace std;
template<class W>
struct HuffmanTreeNode
{
	HuffmanTreeNode<W>* _left;
	HuffmanTreeNode<W>* _right;
	HuffmanTreeNode<W>* _parent;
	W _data;
	HuffmanTreeNode(W x)
	{
		_left = nullptr;
		_right = nullptr;
		_parent = nullptr;
		_data = x;
	}
};

template<class W>
class HuffmanTree
{
typedef HuffmanTreeNode<W> Node;
public:
	HuffmanTree()
		:_root(nullptr)
	{}
	Node* GetRoot()
	{
		return _root;
	}
	struct NodeCompare  //仿函数
	{
		bool operator()(const Node* l, const Node* r)
		{
			return l->_data > r->_data;
		}
	};
	HuffmanTree(W* w,size_t n, const W& invalid)
	{
		//这边用小堆
		priority_queue<Node*,vector<Node*>,NodeCompare> minheap; //若用堆,则用优先级队列,默认为大堆
		for (size_t i = 0; i < n; i++){
			if (w[i] != invalid)
				minheap.push(new Node(w[i]));
		}
		while (minheap.size() > 1){
			Node* left = minheap.top();
			minheap.pop();
			Node* right = minheap.top();
			minheap.pop();
			Node* parent = new Node(left->_data + right->_data);
			parent->_left = left;
			left->_parent = parent;
			parent->_right = right;
			right->_parent = parent;
			minheap.push(parent);
		}
		_root = minheap.top();
		_root->_parent = nullptr;
	}
	~HuffmanTree()
	{
		Destory(_root);
		_root = nullptr;
	}
	void Destory(Node* root)
	{
		if (root == nullptr) return;
		Destory(root->_left);
		Destory(root->_right);
		delete root;
	}
public:
	HuffmanTree(const HuffmanTree& hf) = delete;
	HuffmanTree& operator=(const HuffmanTree& hf) = delete;
protected:
	Node* _root;
};
