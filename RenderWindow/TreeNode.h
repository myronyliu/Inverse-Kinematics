#ifndef _TREENODE_H_
#define _TREENODE_H_

#include "stdafx.h"

template <class T>
class TreeNode
{
public:
    TreeNode() : _parent(NULL), _children(std::set<TreeNode*>()), _depth(0) {}
    
    TreeNode(const T& data, TreeNode* parent = NULL, const std::set<TreeNode*>& children = std::set<TreeNode<T>*>());

    ~TreeNode() { for (auto child : _children) delete child; }

    TreeNode* root();
    TreeNode* leftMostLeaf() const;
    void setDepth(const int&);

    // in DFS, leaf nodes should only appear once. Internal nodes should appear "their number of children plus one" times
    std::vector<TreeNode*> depthFirstSearchSequence() const;
    std::vector<TreeNode*> iterativeRecursionSequence() const;

    TreeNode* parent() const { return _parent; }
    std::set<TreeNode*> children() const { return _children; }
    T data() const { return _data; }
    int depth() const { return _depth; }
    int nDescendantGenerations() const;

private:
    TreeNode* _parent;
    std::set<TreeNode*> _children;
    T _data;
    int _depth;
};

#endif