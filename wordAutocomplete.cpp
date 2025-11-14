#include <iostream>
#include <functional>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;

// STEPS TO FOLLOW:
// 1. read the dictionary, insert words into AVL tree
// 2. find all words with the same prefix (autocomplete) as what the user typed (convert all to lowercase) by checking EVERY node 
//     for whether or not it starts with the prefix & prints the ones that do (full tree traversal)

// 3. challenge: do an optimized in-order traversal:
// Traverses the AVL tree starting from the root, but only explores subtrees that could contain words matching the prefix.
// Uses bounds (prefix → prefix + '{') to prune irrelevant subtrees and prints all matching words in lexicographic order.

// each node has a key, a left node, a right node, and a height.
struct Node {
    string key ;   
    Node* left ;
    Node* right ;
    int height ;   

    // constructor for every new node we make (& is for call by ref (dont copy), const is for 'dont change')
    Node (const string& word) : key(word), left(nullptr), right(nullptr), height(1) {}
};

// helper function: find node height (if null, h=0 ; else, height stored in n)
int nodeHeight(Node* n) {
    if (n == nullptr)
        return 0; 
    else
        return n->height; 
}

// helper function: keep track of (update) node height (theres no return bc its js making direct changes)
void updateHeight(Node* n) {
    if (n == nullptr) return;
    
    int leftHeight = nodeHeight(n->left);   
    int rightHeight = nodeHeight(n->right);

    // add 1 to the taller child for final height of node n
    if (leftHeight > rightHeight) 
        n->height = leftHeight + 1;
    else
        n->height = rightHeight + 1;
}

// helper function: balance factor (LHS height - RHS height)
int balanceFactor(Node* n) {
    if (n == nullptr) return 0;

    int leftHeight = nodeHeight(n->left);
    int rightHeight = nodeHeight(n->right);

    return leftHeight - rightHeight;
}

// right rotation for left-heavy trees (balance factor +2)
Node* rotateRight(Node* oldRoot) {

    // old root is the unbalanced parent node, which becomes the new roots right side
    // new root is the old roots left child
    // tempSubTree is any possible nodes on the new roots right side, which move to the old roots left 
    // the new roots left side remains in its place

    // point to new root & point to tempSubTree
    Node* newRoot = oldRoot->left;  
    Node* tempSubTree = newRoot->right; 

    //rotation steps
    newRoot->right = oldRoot; // the old root is now the new roots right side
    oldRoot->left = tempSubTree; // the new roots right side -> the old roots left side

    // old and new roots now have new heights
    updateHeight(oldRoot);
    updateHeight(newRoot);

    // return the new root so the parent of the unbalanced node knows who the new child is after rotation
    return newRoot; 
}

// left rotation for right-heavy trees (balance factor -2)
Node* rotateLeft(Node* oldRoot) {
    Node* newRoot = oldRoot->right;   // right child becomes new root
    Node* tempSubtree = newRoot->left; // this subtree moves to old roots right after rotation

    // old root to new roots left side & new roots left side -> old roots right side
    newRoot->left = oldRoot;
    oldRoot->right = tempSubtree;

    // new heights
    updateHeight(oldRoot);
    updateHeight(newRoot);

    // new root
    return newRoot; 
}

// and finallyyyy: recursively build the balanced BST (AVL) tree, word by word
Node* insertNode(Node* currentNode, const string& word) {
    // if the tree is empty, create a (dynamically allocated) new node, using the constructor in the struct
    if (currentNode == nullptr) 
        return new Node(word);

    // step 1: follow BST insertion rules
    if (word < currentNode->key) {
        // insert left if word is smaller than current node
        currentNode->left = insertNode(currentNode->left, word);
    }
    else if (word > currentNode->key) {
        // insert right if word is larger than current node
        currentNode->right = insertNode(currentNode->right, word);
    }
    else {
        // if they are equal (duplicates), don't do anything
        return currentNode;
    }

    // step 2 & 3: update height & check BF
    updateHeight(currentNode);
    int bf = balanceFactor(currentNode);


    // step 4: left-heavy (+2) imbalance, where:
    // 1. either LR imbalance if its in the node at node->left->right, so: L rotation -> R rotation (otherwise just do the R rotation)
    // or: (else) LL imbalance -> R rotation
    if (bf==2) {
        if (word > currentNode->left->key) // check if the word went into the left child of the node
            currentNode->left = rotateLeft(currentNode->left);
        return rotateRight(currentNode);
    }

    // step 5: Right-heavy (-2) imbalance, which follows similar rules, but with right and left switched.
    if (bf==-2) {
        // RL rotation where the imbalance node is in parent->right->left
        if (word < currentNode->right->key)
            currentNode->right = rotateRight(currentNode->right);

        // RR imbalance -> left rotation
        return rotateLeft(currentNode);
    }

    // step 6: if already balanced, return the current node directly
    return currentNode;
}

// read the file and build the tree using insertNode
Node* buildAVL(const string& dict_file) {
    // start with an empty tree
    Node* avlTree = nullptr;  

    // open file using input file stream (ifstream)
    ifstream dict(dict_file);  
    // temp place for every word read
    string word;  

    // read line by line, convert to lowercase, then insertNode
    while (getline(dict, word)) {
        if (word.empty()) continue;  // skip empty lines

        // convert
        for (int i = 0; i < (int)word.size(); i++) {
            if (word[i] >= 'A' && word[i] <= 'Z')
                word[i] = word[i] - 'A' + 'a';
        }

        // insert 
        avlTree = insertNode(avlTree, word);
    }

    // close file & return tree
    dict.close();  
    return avlTree;  
}

// autocomplete: check every node in the tree recursively for the prefix, no skipping nodes
void autocomplete(Node* currentNode, const string& prefix) {
    // do nothing if string is empty
    if (currentNode == nullptr) 
        return; 

    // check if the current node's key (word) starts with the prefix (substring from 0 to size of prefix), print the word
    if (currentNode->key.substr(0, (int)prefix.size()) == prefix) {
        cout << currentNode->key << ", " ;  
    }

    // check left and right children
    autocomplete(currentNode->left, prefix);
    autocomplete(currentNode->right, prefix);
}

// challenge: modified in-order traversal: visits left subtree → current node → right subtree, but prunes subtrees that cannot contain matches.
void challenge(Node* currentNode, const string& prefix) {
    // base case: stop when reached null subtree
    if (currentNode == nullptr) {
        cout << "no matches found" << endl;
        return;
    }

    const string lowerBound = prefix;        // any match is > or = to prefix so prefix is lo 
    const string upperBound = prefix + "{";  // '{' is after 'z' in ASCII , hi is stricly bigger than any word containing prefix


    // helper to traverse only relevant parts of the tree
    function<void(Node*)> traverse = [&](Node* node) {
        // base case null node
        if (node == nullptr) return;

        // only explore left side if keys could be >= lo
        if (node->key >= lowerBound) traverse(node->left);

        // print key if within bounds [lo, hi) (match)
        if (node->key >= lowerBound && node->key < upperBound) {
            cout << node->key << ", ";
        }

        // only explore right side if keys could be < hi
        if (node->key < upperBound) traverse(node->right);
    };

    traverse(currentNode);
}

// recursively delete all nodes in the AVL tree to free up memory
void deleteTree(Node* currentNode) {
    if (currentNode == nullptr) return;

    deleteTree(currentNode->left);
    deleteTree(currentNode->right);

    delete currentNode;
}

// call all functions
int main() {
    // 1. build AVL tree using dictionary file
    Node* avlTree = buildAVL("dictionary.txt");

    // 2. ask user for prefix
    string prefix;
    cout << "Enter prefix: ";
    cin >> prefix;

    // 3. convert prefix to lowercase 
    for (int i = 0; i < (int)prefix.size(); i++) {
        if (prefix[i] >= 'A' && prefix[i] <= 'Z')
            prefix[i] = prefix[i] - 'A' + 'a';
    }

    // 4. call both autocomplete functions
    cout << "Autocomplete suggestions using autocomplete function: ";
    autocomplete(avlTree, prefix);
    cout << endl << "Autocomplete suggestions using in-order traversal: ";
    challenge(avlTree, prefix);

    // 5. free up the dynamically allocated memory
    deleteTree(avlTree); 

    return 0;
}
