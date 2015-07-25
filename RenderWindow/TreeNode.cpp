#include "TreeNode.h"

template <class T>
void TreeNode<T>::insertParent(TreeNode* parent) {
    parent->_parent = _parent;
    parent->_children.push_back(this);
    _parent = parent;
    parent->setDepth(_depth);
}

template <class T>
void TreeNode<T>::insertChild(TreeNode* child) {
    child->_parent = this;
    _children.push_back(child);
    child->setDepth(_depth + 1);
}

template <class T>
TreeNode<T>::TreeNode(const T& data, TreeNode* parent, const std::list<TreeNode*>& children) :
_data(data), _parent(parent), _children(children)
{
    if (!_children.empty()) {
        for (auto child : children) {
            child->_parent = this;
        }
    }
    if (parent == NULL)
        setDepth(0);
    else {
        parent->_children.push_back(this);
        setDepth(parent->_depth + 1);
    }
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
        if (node->children().empty()) return node;
        node = *(node->children().begin());
    }
}

template <class T>
std::vector<TreeNode<T>*> TreeNode<T>::pathToLeftMostLeaf() const {
    std::vector<TreeNode*> path({ const_cast<TreeNode*>(this) });
    const TreeNode* node = this;
    while (true) {
        if (node->children().empty()) return path;
        TreeNode* child = *(node->children().begin());
        path.push_back(child);
        node = child;
    }
}


template <class T>
std::vector<TreeNode<T>*> TreeNode<T>::leaves() const {
    std::vector<TreeNode*> leaves;
    std::vector<TreeNode*> seqn = BFSsequence();
    for (auto node : seqn) {
        if (node->children().empty()) {
            leaves.push_back(node);
        }
    }
    return seqn;
}


template <class T>
std::vector<TreeNode<T>*> TreeNode<T>::BFSsequence() const {
    std::vector<TreeNode*> seqn;
    std::vector<TreeNode*> stack({ const_cast<TreeNode*>(this) });
    TreeNode* node;
    do {
        node = stack.back();
        stack.pop_back();
        seqn.push_back(node);

        for (auto child : node->children()) {
            if (child == NULL) continue;
            stack.push_back(child);
        }
    } while (stack.size() > 0);
    return seqn;
}

template <class T>
std::vector<TreeNode<T>*> TreeNode<T>::DFSsequence() {
    std::vector<TreeNode*> seqn;
    std::set<TreeNode*> traversed;
    TreeNode* node = this;
    do {
        seqn.push_back(node);

        bool foundUncharted = false;
        for (auto child : node->children()) {
            if (child == NULL) continue;
            if (traversed.find(child) == traversed.end()) {
                node = child;
                foundUncharted = true;
                break;
            }
        }
        if (foundUncharted) continue;

        traversed.insert(node);
        for (auto child : node->children()) {
            // Although the children have all been traversed, since we have added "node" to the set of traversed nodes
            // we have zero chance of entering the subtree rooted at "node", so we might as truncate "traversed"
            traversed.erase(child);
        }
        node = node->_parent;
    } while (node != _parent);
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

            for (auto child : node->children()) {
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

template <class T>
void TreeNode<T>::pruneToLeafset(const std::set<TreeNode*> leaves) {
    std::vector<TreeNode*> seqn = DFSsequence();
    for (auto node : seqn) {
        if (node->children().empty() && leaves.find(node) == leaves.end()) {
            node->suicide();
        }
    }
}

template <class T>
std::vector<TreeNode<T>*> TreeNode<T>::findPathTo(TreeNode* target) const {
    TreeNode* node0 = this;
    TreeNode* node1 = target;
    std::list<TreeNode*> path0;
    std::list<TreeNode*> path1;

    while (node0->depth() > node1->depth()) {
        path0.push_back(node0);
        node0 = node0->parent();
    }
    while (node1->depth() > node0->depth()) {
        path1.push_front(node1);
        node1 = node1->parent();
    }

    do {
        if (node0 == node1) break;
        path0.push_back(node0);
        path1.push_front(node1);
        node0 = node0->parent();
        node1 = node1->parent();
    } while (node0 != NULL || node1 != NULL);

    if (node0 != node1) return std::list<TreeNode*>();
    path0->push_back(node0);
    path0->splice(path0.end(), path1);
    return std::vector<TreeNode*>(path0.begin(), path0.end());
}

template <class T>
std::vector<TreeNode<T>*> TreeNode<T>::upstreamForks() const {

}

template <class T>
TreeNode<T>* TreeNode<T>::invertedBranch() const {
    TreeNode* node = this;
    int depthCounter = 0;
    while (true) {
        std::list<TreeNode*> children = node->children();
        if (children.size() == 1) {
            node = *(children.begin());
            depthCounter++;
        }
        else break;
    }

    TreeNode* root = new TreeNode(node->data);
    for (int i = 0; i < depthCounter; i++) {
        new TreeNode(node->parent()->data(), node);
        node = node->parent();
    }
    return root;
}

template <class T>
// Let "mother-tree" refer to the tree that calls this function
// Let "branch-tree" refer to the tree that this function returns.
//
// ///////////////////////////////////////////////
// //// The branch-tree will be as follows... ////
// ///////////////////////////////////////////////
//
// The root of branch-tree will be the left-most-branch of the mother-tree
// ... i.e the branch starting at the mother-tree's root and ending at the mother-tree's left-most-leaf
// Along the mother-tree's left-most-branch will be some other branches splitting off to the right
// These branches (that split off) will be the children of the the branch-tree's root
// The rest of the tree is recursively constructed, treating the aforementioned children as the new roots
//
// Put another way, consider the literal-plant analogy
// Usually, we treat the root as the "base" of the plant nearest to the soil
// (NOTE how the trunk is considered no differently from the rest of the plant's branches in this paradigm)
// (... also how the graph-theoretic nodes lie at the points where literal-plant-branches split apart)
//
// The difference now is the the "trunk" itself becomes the graph-theoretic root
// The branches that grow off the trunk become children of the trunk
// (NOTE how the plant-branches themselves are now graph-nodes)
TreeNode<TreeNode<T>*>* TreeNode<T>::buildBranchTree() const {
    typedef std::vector<TreeNode*> Branch;

    TreeNode<TreeNode*>* root = new TreeNode<TreeNode*>(const_cast<TreeNode*>(this));

    std::vector<TreeNode<TreeNode*>*> stack({ root });

    do {
        TreeNode<TreeNode*>* branchNode = stack.back();
        stack.pop_back();

        std::vector<TreeNode*> branchNode_nodeSeqn = branchNode->data()->pathToLeftMostLeaf();

        for (auto node : branchNode_nodeSeqn) {
            std::list<TreeNode*> children = node->children();
            if (children.size() > 1) {
                std::list<TreeNode*>::iterator it = children.begin();
                it++;
                for (it; it != children.end(); it++) {
                    stack.push_back(new TreeNode<TreeNode*>(*it, branchNode));
                }
            }
        }
    } while (stack.size() > 0);

    return root;
}

template <class T>
std::vector<T> TreeNode<T>::BFSdataSequence() const {
    std::vector<TreeNode*> nodes = BFSsequence();
    std::vector<T> datas(nodes.size());
    for (int i = 0; i<nodes.size(); i++) {
        datas[i] = nodes[i]->data();
    }
    return datas;
}