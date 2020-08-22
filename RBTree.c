/**
 * @file RBTree.c
 * @author  Ayelet Avraham <ayelet.avraham@mail.huji.ac.il> username ayeletavr
 * @date 03 june 2020
 * @brief RBTree implementation.
*/

#include <stdio.h>
#include "RBTree.h"
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
void fix(Node *node, RBTree *tree);
void deleteCase1(RBTree *tree, Node* node);

/**
 * @brief creates a new node.
 * @param data - node's data.
 * @return the new node.
 */
Node *newNode(void *data)
{
    Node *newNode = (Node *) calloc(1, sizeof(Node));
    if (newNode == NULL)
    {
        return NULL;
    }
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->data = data;
    newNode->parent = NULL; //?
    newNode->color = RED;
    return newNode;
}

/**
 * @brief constructs a new RBTree with the given CompareFunc.
 * @param compFunc - a function two compare two variables.
 * @param freeFunc - a function that frees the node's data.
 * @return a new RB tree. if creation failed, returns NULL.
 */
RBTree *newRBTree(CompareFunc compFunc, FreeFunc freeFunc)
{
    RBTree *newRBTree = (RBTree *) calloc(1, sizeof(RBTree));
    if (newRBTree == NULL)
    {
        return NULL;
    }
    newRBTree->root = NULL;
    newRBTree->size = 0; // INITIAL_SIZE?
    newRBTree->compFunc = compFunc;
    newRBTree->freeFunc = freeFunc;
    return newRBTree;
}

/**
 * @brief swap a node with his child (left or right).
 * @param tree: RB tree.
 * @param node: node to swap
 * @param child
 */
void replaceWithChild(RBTree *tree, Node *node, Node *child)
{
    if (node->parent == NULL)
    {
        tree->root = child;
    }
    else
    {
        if (node == node->parent->left)
        {
            node->parent->left = child;
        }
        else
        {
            node->parent->right = child;
        }
    }
    if (child != NULL)
    {
        child->parent = node->parent;
    }
}

/**
 * @brief finds a place to a new node according to BST rules, and inserts it.
 * @param root: tree's root
 * @param node: node to insert.
 * @param comparator: compare function that exist in RBTree.
 */
void findAndAdd(Node *root, Node* node, CompareFunc comparator)
{
    if (comparator(node->data, root->data) < 0)
    {
        if (root->left != NULL)
        {
            findAndAdd(root->left, node, comparator);
            return;
        }
        else
        {
            root->left = node;
        }
    }
    else
    {
        if (root->right != NULL)
        {
            findAndAdd(root->right, node, comparator);
            return;
        }
        else
        {
            root->right = node;
        }
    }
    node->parent = root;
}

/**
 * @brief get a node and returns its color (including leaves).
 * @param node: node to check color of.
 * @return: node color.
 */
Color getColor(Node* node)
{
    return (node == NULL ? BLACK : node->color);
}


/**
 * @brief check whether the tree RBTreeContains this item.
 * @param tree: the tree to add an item to.
 * @param data: item to check.
 * @return 0 if the item is not in the tree, other if it is.
 */
int RBTreeContains(const RBTree *tree, const void *data)
{
    if (tree == NULL || data == NULL || tree->compFunc == NULL || tree->root == NULL)
    {
        return false;
    }
    Node* nodePtr = tree->root;
    while (nodePtr != NULL)
    {
        if (tree->compFunc(nodePtr->data, data) == 0)
        {
            return true;
        }
        else if (tree->compFunc(nodePtr->data, data) < 0)
        {
            nodePtr = nodePtr->right;
        }
        else
        {
            nodePtr = nodePtr->left;
        }
    }
    return false;
}

/**
 * @brief assumes that RBTree contains node with given data (we check it before). returns a
 * pointer to a node with the given data in tree.
 * @param tree: RBtree we want to delete a node from.
 * @param data: data of a node that we want to delete.
 * @return a pointer to a node with the given data in tree.
 */
Node* findNodeToDelete (const RBTree *tree, const void *data)
{
    if (tree == NULL || data == NULL || tree->compFunc == NULL || tree->root == NULL)
    {
        return NULL;
    }
    Node* nodePtr = tree->root;
    while (nodePtr != NULL)
    {
        if (tree->compFunc(nodePtr->data, data) == 0)
        {
            return nodePtr;
        }
        else if (tree->compFunc(nodePtr->data, data) < 0)
        {
            nodePtr = nodePtr->right;
        }
        else
        {
            nodePtr = nodePtr->left;
        }
    }
    return false;
}


/**
 * @brief helper to insertToRBTree that handles case 1 - node is the root.
 * @param tree: the tree to add an item to.
 * @param node: the node we want to add.
 */
void insertCase1(Node *node)
{
    node->color = BLACK;
}


/**
 * @brief Gets a pointer to node, and sets a pointer to its uncle, if there's any
 * @param node: node to find uncle of.
 * @return a pointer to node's uncle - null pointer if there in no uncle.
 */
Node* findUncle(Node *node)
{
    Node *uncle;
    // check if node.parent is a left child:
    if (node->parent->parent->left == node->parent)
    {
        // node.parent is a left child.
        if (node->parent->parent->right != NULL)
        {
            uncle = node->parent->parent->right;
            return uncle;
        }
    }
    // check if node.parent is a left child:
    else if (node->parent->parent->right == node->parent)
    {
        //node.parent is a right child.
        if (node->parent->parent->left != NULL)
        {
            uncle = node->parent->parent->left;
            return uncle;
        }
    }
    // if we got here, node has no uncle.
    uncle = NULL;
    return uncle;
}

/**
 * @brief helper to insertToRBTree that handles case 3 - node.parent is red and node.uncle is red.
 * @param tree: the tree to add an item to.
 * @param node: the node we want to add.
 * @return
 */
void insertCase3(Node *node, RBTree *tree)
{
    Node *uncle = findUncle(node);
    node->parent->color = BLACK;
    uncle->color = BLACK;
    node->parent->parent->color = RED;
    fix(node->parent->parent, tree);
}

/**
 * @brief left rotation (LR) to a node.
 * @param node: node of the rotation.
 */
void leftRotate(Node *node, RBTree* tree)
{
    Node* right = node->right;
    replaceWithChild(tree, node, right);
    node->right = right->left;
    if (right->left != NULL)
    {
        right->left->parent = node;
    }
    right->left = node;
    node->parent = right;
}

/**
 * @brief right rotation (RR) to a node.
 * @param node: node of the rotation.
 */
void rightRotate(Node *node, RBTree *tree)
{
    Node *left = node->left;
    replaceWithChild(tree, node, left);
    node->left = left->right;
    if (left->right != NULL)
    {
        left->right->parent = node;
    }
    left->right = node;
    node->parent = left;
}

/**
 * @brief helper to insertToRBTree that handles case 4b - node is a left child of a left child,
 * or a right child of a right child (3 nodes in a line).
 * @param node: the node that we inserted and needs a rotation.
 */
void insertCase4b(Node* node, RBTree *tree)
{
    Node* parentNode = node->parent;
    Node* grandParentNode = node->parent->parent;
    if (node == parentNode->left)
    {
        rightRotate(grandParentNode, tree);
    }
    else
    {
        leftRotate(grandParentNode, tree);
    }
    parentNode->color = BLACK;
    grandParentNode->color = RED;
}

/**
 * @brief helper to insertToRBTree that handles case 4b - node is a left child of a right child,
 * or a right child of a left child (3 nodes creates a triangle).
 * @param node: the node that we inserted and needs a rotation - leads to 4b case.
 */
void insertCase4a(Node* node, RBTree *tree)
{
    Node *parentNode = node->parent;
    Node *grandParentNode = node->parent->parent;
    if (node == parentNode->right && parentNode == grandParentNode->left)
    {
        leftRotate(parentNode, tree);
        node = node->left;
    }
    else if (node == parentNode->left && parentNode == grandParentNode->right)
    {
        rightRotate(parentNode, tree);
        node = node->right;
    }
    insertCase4b(node, tree);
}




/**
 * @brief checks if there is any case that needs a fix, and fixes.
 * @param tree tree to fix,
 * @param node we need to check.
 * @return true for proper RB tree, false for any failure.
 */
void fix(Node *node, RBTree *tree)
{
    if (node->parent == NULL)
    {
        insertCase1(node);
    }
    else if (node->parent->color == BLACK)
    {
        // pass
    }
    else if (findUncle(node) != NULL && findUncle(node)->color == RED)
    {
        insertCase3(node, tree);
    }
    else
    {
        insertCase4a(node, tree);
    }
}

/**
 * @brief add an item to the tree
 * @param tree: the tree to add an item to.
 * @param data: item to add to the tree.
 * @return 0 on failure, other on success. (if the item is already in the tree - failure).
 */
int insertToRBTree(RBTree *tree, void *data)
{
    if ((RBTreeContains(tree, data)) || tree == NULL || data == NULL)
    {
        return false;
    }
    Node *toAdd = newNode(data);
    if (tree->root == NULL)
    {
        toAdd->color = BLACK;
        tree->root = toAdd;
        tree->size++;
        return true;
    }
    else
    {
        findAndAdd(tree->root, toAdd, tree->compFunc);
    }
    fix(toAdd, tree);
    Node *temp = toAdd;
    while (temp->parent != NULL)
    {
        temp = temp->parent;
    }
    tree->root = temp;
    tree->size++;
    return true;
}

/**
 * @brief gets a node and find the minimal node in its sub-tree.
 * @param node: this node is the root of the sub-tree we find the minimal node in.
 * @return a pointer to the minimal node.
 */
Node* minNode(Node *node)
{
    if (node == NULL)
    {
        return NULL;
    }
    else
    {
        Node* curr = node;
        // the minimal node is the leftmost leaf in the subtree.
        while (curr->left != NULL)
        {
            curr = curr->left;
        }
        return curr;
    }

}

/**
 * @brief find's a given node its successor node, in-order traversal.
 * @param root: tree's root.
 * @param node: node to fins its successor.
 * @return a pointer to successor.
 */
Node* successor(Node *node)
{
    if (node->right != NULL)
    {
        return minNode(node->right);
    }
    Node *ptr = node->parent;
    while (ptr != NULL && node == ptr->right)
    {
        node = ptr;
        ptr = ptr->parent;
    }
    return ptr;
}

/**
 * @brief helper to forEachRBTree - tracks every node in order traversal anf implies func on
 * every node.
 * @param tree: tree to traverse.
 * @param func: func to imply.
 * @param args: arg of func - can be null.
 * @param min: current node, we start at min.
 * @return: true if succeeded, else false.
 */
int forEachRBTreeHelper(const RBTree *tree, forEachFunc func, void *args, Node* min)
{
    if (min == NULL || min->data == NULL)
    {
        return true;
    }
    if (!func(min->data, args))
    {
        return false;
    }
    if (successor(min) != NULL)
    {
        forEachRBTreeHelper(tree, func, args, successor(min));
    }
    // if we got here, we are at the right-most node.
    return true;

}

 /**
 * @brief Activate a function on each item of the tree. the order is an ascending order. if one of the activations of the
 * function returns 0, the process stops.
 * @param tree: the tree with all the items.
 * @param func: the function to activate on all items.
 * @param args: more optional arguments to the function (may be null if the given function support it).
 * @return: 0 on failure, other on success.
 */
int forEachRBTree(const RBTree *tree, forEachFunc func, void *args)
{
    if (tree == NULL || tree->compFunc == NULL)
    {
        return false;
    }
    Node* min = minNode(tree->root);
    return forEachRBTreeHelper(tree, func, args, min);
}

/**
 * @brief gets a single node and a free function, and frees its sub-tree.
 * @param node: root.
 * @param freeFunc: tree's free func.
 */
void freeSubTree(Node* node, FreeFunc freeFunc)
{
    if (node == NULL)
    {
        return;
    }
    if (node->left != NULL)
    {
        freeSubTree(node->left, freeFunc);
    }
    if (node->right != NULL)
    {
        freeSubTree(node->right, freeFunc);
    }
    freeFunc(node->data);
    free(node);
}

/**
 * @brief free all memory of the data structure.
 * @param tree: pointer to the tree to free.
 */
void freeRBTree(RBTree **tree)
{
    if ((*tree) == NULL || (*tree)->freeFunc == NULL)
    {
        return;
    }
    freeSubTree((*tree)->root, (*tree)->freeFunc);
    free(*tree);
    *tree = NULL;
}

//-------------- delete from RB-tree. ----------------

/**
 * @brief returns node's sibling
 * @param node: node to find sibling of.
 * @return node's sibling
 */
Node* getSibling(Node *node)
{
    // root has no sibling
    if (node->parent == NULL)
    {
        return NULL;
    }
    //if node is a left child:
    if (node->parent->left == node)
    {
        return node->parent->right;
    }
    else
    {
        return node->parent->left;
    }
}

/**
 * @brief helper to deleteFromRBTree that handles case 6.
 * case 6 = case 3e from notes: the child that far from C is RED.
 * @param tree: the tree to remove an item from.
 * @param node: the node we want to remove.
 */
void deleteCase6(RBTree* tree, Node* node)
{
    getSibling(node)->color = getColor(node->parent);
    node->parent->color = BLACK;
    if (node == node->parent->left)
    {
        getSibling(node)->right->color = BLACK;
        leftRotate(node->parent, tree);
    }
    else
    {
        getSibling(node)->left->color = BLACK;
        rightRotate(node->parent, tree);
    }
}

/**
 * @brief helper to deleteFromRBTree that handles case 5.
 * case 5 = case 3d from notes: the child that far from C is BLACK.
 * @param tree: the tree to remove an item from.
 * @param node: the node we want to remove.
 */
void deleteCase5(RBTree *tree, Node* node)
{
    if (node == node->parent->left &&
        getColor(getSibling(node)) == BLACK &&
        getColor(getSibling(node)->left) == RED &&
        getColor(getSibling(node)->right) == BLACK)
    {
        getSibling(node)->color = RED;
        getSibling(node)->left->color = BLACK;
        rightRotate(getSibling(node), tree);
    }
    else if (node == node->parent->right &&
             getColor(getSibling(node)) == BLACK &&
             getColor(getSibling(node)->right) == RED &&
             getColor(getSibling(node)->left) == BLACK)
    {
        getSibling(node)->color = RED;
        getSibling(node)->right->color = BLACK;
        leftRotate(getSibling(node), tree);
    }
    deleteCase6(tree, node);
}

/**
 * @brief helper to deleteFromRBTree that handles case 4.
 * case 4 = case 3bi from notes: node->parent is RED.
 * @param tree: the tree to remove an item from.
 * @param node: the node we want to remove.
 */
void deleteCase4(RBTree *tree, Node* node)
{
    if (getColor(node->parent) == RED &&
        getColor(getSibling(node)) == BLACK &&
        getColor(getSibling(node)->left) == BLACK &&
        getColor(getSibling(node)->right) == BLACK)
    {
        getSibling(node)->color = RED;
        node->parent->color = BLACK;
    }
    else
    {
        deleteCase5(tree, node);
    }
}

/**
 * @brief helper to deleteFromRBTree that handles case 3.
 * case 3 = case 3bii from notes: node->parent is BLACK.
 * @param tree: the tree to remove an item from.
 * @param node: the node we want to remove.
 */
void deleteCase3(RBTree *tree, Node* node)
{
    if (getColor(node->parent) == BLACK &&
        getColor(getSibling(node)) == BLACK &&
        getColor(getSibling(node)->left) == BLACK &&
        getColor(getSibling(node)->right) == BLACK)
    {
        getSibling(node)->color = RED;
        deleteCase1(tree, node->parent);
    }
    else
    {
        deleteCase4(tree, node);
    }
}

/**
 * @brief helper to deleteFromRBTree that handles case 2.
 * case 2 = case 3c from notes: sibliing is a non-leaf node and its color is RED.
 * @param tree: the tree to remove an item from.
 * @param node: the node we want to remove.
 */
void deleteCase2(RBTree *tree, Node *node)
{
    if (getColor(getSibling(node)) == RED)
    {
        node->parent->color = RED;
        getSibling(node)->color = BLACK;
        if (node == node->parent->left)
        {
            leftRotate(node->parent, tree);
        }
        else
        {
            rightRotate(node->parent, tree);
        }
    }
    deleteCase3(tree, node);
}

/**
 * @brief helper to deleteFromRBTree that handles case 1.
 * case 1 = case 3a from notes: node is the root.
 * @param tree: the tree to remove an item from.
 * @param node: the node we want to remove.
 */
void deleteCase1(RBTree *tree, Node* node)
{
    if (node->parent == NULL)
    {
        return;
    }
    else
    {
        deleteCase2(tree, node);
    }
}

/**
 * remove an item from the tree
 * @param tree: the tree to remove an item from.
 * @param data: item to remove from the tree.
 * @return: 0 on failure, other on success. (if data is not in the tree - failure).
 */
int deleteFromRBTree(RBTree *tree, void *data)
{
    if ((!RBTreeContains(tree, data)) || tree == NULL || data == NULL)
    {
        return false;
    }
    // get a pointer to the node:
    Node *toDelete = findNodeToDelete(tree, data);
    Node *child;
    if (toDelete->left == NULL || toDelete->right == NULL)
    {
        free(toDelete->data);
    }
    // if node has two non-leaf children:
    if (toDelete->left != NULL && toDelete->right != NULL) // if node has to children
    {
        Node *successorNode = successor(toDelete);
        free(toDelete->data);
        toDelete->data = successorNode->data;
        toDelete = successorNode;
    }
    // now node has at most 1 non-leaf child.
    assert(toDelete->left == NULL || toDelete->right == NULL);
    child = toDelete->right == NULL ? toDelete->left : toDelete->right;
    if (getColor(toDelete) == BLACK)
    {
        toDelete->color = getColor(child);
        deleteCase1(tree, toDelete);
    }
    replaceWithChild(tree, toDelete, child);
    if (toDelete->parent == NULL && child != NULL)
    {
        child->color = BLACK;

    }
    free(toDelete);
    tree->size--;
    return true;
}