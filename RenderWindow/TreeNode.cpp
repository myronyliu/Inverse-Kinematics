#include "TreeNode.h"

template <class T>
TreeNode<T>::TreeNode(const T& data, TreeNode* parent, const std::set<TreeNode*>& children = std::set<TreeNode<T>*>()) :
_data(data), _parent(parent), _children(children)
{
    parent->_children.insert(this);
    for (auto child : children)
        child->_parent = this;
    if (parent == NULL)
        setDepth(0);
    else
        setDepth(parent->_depth + 1);
}

template <class T>
TreeNode<T>* TreeNode<T>::root() {
    TreeNode* node = this;
    TreeNode* parent = node->_parent;
    while (parent != NULL) {
        parent = node->_parent;
    }
    return node;
}

template <class T>
TreeNode<T>* TreeNode<T>::leftMostLeaf() const {
    TreeNode* node = this;
    while (true) {
        if (_children.size() == 0) return node;
        node = *(node->_children.begin());
    }
    return NULL;
}


template <class T>
std::vector<TreeNode<T>*> TreeNode<T>::iterativeRecursionSequence() const {
    std::vector<TreeNode*> seqn;
    std::vector<TreeNode*> stack({ this });
    TreeNode* node;
    do {
        node = stack.back();
        stack.pop_back();
        seqn.push_back(node);

        for (auto child : node->_children) {
            if (child == NULL) continue;
            stack.push_back(child);
        }
    } while (stack.size() > 0);
    return seqn;
}

template <class T>
std::vector<TreeNode<T>*> TreeNode<T>::depthFirstSearchSequence() const {
    std::vector<TreeNode*> seqn;
    std::set<TreeNode*> traversed;
    TreeNode* node = this;
    do {
        seqn.push_back(node);

        bool foundUncharted = false;
        for (auto child : node->_children) {
            if (child == NULL) continue;
            if (traversed.find(child) == traversed.end()) {
                node = child;
                foundUncharted = true;
                break;
            }
        }
        if (foundUncharted) continue;

        traversed.insert(node);
        for (auto child : node->_children) {
            // Although the children have all been traversed, since we have added "node" to the set of traversed nodes
            // we have zero chance of entering the subtree rooted at "node", so we might as truncate "traversed"
            traversed.erase(child);
        }
        node = node->_parent;
    } while (node != this);
    if (seqn.size() == 1) seqn.push_back(this);
    return seqn;
}

template <class T>
void TreeNode<T>::setDepth(const int& depth) {
    if (_parent != NULL && _parent->_depth != depth - 1) {
        TreeNode* node = this;
        TreeNode* parent = _parent;
        int generationGap = 0;
        while (parent != NULL) {
            parent = node->_parent;
            generationGap++;
        }
        parent->setDepth(depth - generationGap);
    }
    else {
        _depth = depth;
        std::vector<TreeNode*> stack;
        for (auto child : _children) {
            if (child == NULL) continue;
            stack.push_back(child);
        }
        TreeNode* node;
        while (stack.size() > 0) {
            node = stack.back();
            stack.pop_back();

            node->_depth = node->_parent->_depth + 1;

            for (auto child : node->_children) {
                if (child == NULL) continue;
                stack.push_back(child);
            }
        }
    }
}

template <class T>
int TreeNode<T>::nDescendantGenerations() const {
    return 0;
}