#ifndef BST_H
#define BST_H

// binary search tree
// simple design
// not balanced
// basically a dictionary

template<class T>
class bst {
    private:

        class bst_node {

            public:
                bst_node *left, *right;
                T data;
                
                bst_node(T value) : left(0), right(0), data(value) {}
                
                ~bst_node()
                {
                    if (left) delete left;
                    if (right) delete right;
                }
        };

        bst_node *root;
    
    public:
        bst() : root(0) {}
        
        ~bst() 
        {
            if (root) delete root;
        }

        void insert(T value)
        {
            bst_node *nn, *tmp, *parent;
            // Unlike malloc, new throws exceptions. We don't need to check.
            nn = new bst_node(value);

            if(!root){
                root = nn;
                return;
            }

            for (tmp = root; tmp;){
                parent = tmp;

                if(value < tmp->data){
                    tmp = tmp->left;
                }else{
                    tmp = tmp->right;
                }
            }

            if (value < parent->data){
                parent->left = nn;
            }else{
                parent->right = nn;
            }
        }

        bool contains(T value)
        {
            bst_node *tmp;

            for (tmp = root; tmp && tmp->data != value;){
                if (value < tmp->data){
                    tmp = tmp->left;
                }else{
                    tmp = tmp->right;
                }
            }

            return tmp;
        }
};




#endif