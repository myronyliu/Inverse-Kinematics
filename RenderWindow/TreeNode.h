#ifndef _TREENODE_H_
#define _TREENODE_H_

#include "stdafx.h"

template <class T>
class TreeNode
{
public:
    TreeNode() : _parent(NULL), _children(std::set<TreeNode*>()), _depth(0) {}
    
    TreeNode(const T& data, TreeNode* parent = NULL, const std::set<TreeNode*>& children = std::set<TreeNode<T>*>());

    void suicide() {
        if (_parent != NULL) _parent->_children.erase(this);
        delete this;
    }

    int nChildren() const { return _children.size(); }

    TreeNode* root();
    TreeNode* leftMostLeaf() const;
    std::vector<TreeNode*> pathToLeftMostLeaf() const;
    std::vector<TreeNode*> leaves() const;
    void setDepth(const int&);
    void pruneToLeafset(const std::set<TreeNode*> leaves);

    void insertParent(TreeNode* parent);
    void insertChild(TreeNode* child);

    // in DFS, leaf nodes should only appear once. Internal nodes should appear "their number of children plus one" times
    std::vector<TreeNode*> DFSsequence();
    std::vector<TreeNode*> BFSsequence();

    TreeNode* parent() const { return _parent; }
    std::set<TreeNode*> children() const { return _children; }
    T data() const { return _data; }
    int depth() const { return _depth; }
    int nDescendantGenerations() const;

    std::vector<TreeNode*> findPathTo(TreeNode*) const;
    std::vector<TreeNode*> upstreamForks() const;

    TreeNode* invertedBranch() const;
    TreeNode<std::vector<TreeNode*>>* buildBranchTree() const;

private:
    TreeNode* _parent;
    std::set<TreeNode*> _children;
    T _data;
    int _depth;

    ~TreeNode() { for (auto child : _children) delete child; }
};

#endif