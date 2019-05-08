#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

typedef struct Block
{
    vector<int> keys;
    vector<Block *> bps;
    int filled;
    bool is_leaf;
    Block * parent_block;
}Block;

class BPlusTree
{
    public:

    Block * root;
    Block * first_leaf;
    int order;
    
    BPlusTree(int order)
    {
        this->order = order;
        this->root = NULL;
    }
    
    Block * create_block(vector<int> keys, vector<Block *> bps, bool is_leaf)
    {
        Block * new_block = new Block;
        new_block->keys = keys;
        new_block->bps = bps;
        new_block->filled = keys.size();
        new_block->is_leaf = is_leaf;
        new_block->parent_block = NULL;
    }

    void insert_internal_node(Block * curr_block, int key, Block * left_child, Block * right_child, bool make_root_block)
    {
        vector<int>::iterator it = curr_block->keys.begin();
        int i=0;
        for( ; it!=curr_block->keys.end(); ++it)
        {
            if(*it > key)
            {
                break;
            }
            i++;
        }
        curr_block->keys.insert(it,key);
        vector<Block *>::iterator bit = curr_block->bps.begin();
        for(int j=0; j<i; j++)
        {
            ++bit;
        }
        vector<Block *> new_bps;
        vector<Block *>::iterator bit2 = curr_block->bps.begin();
        for( ; bit2!=bit; ++bit2)
        {
            new_bps.push_back(*bit2);
        }
        new_bps.push_back(left_child);
        new_bps.push_back(right_child);
        if(bit!=curr_block->bps.end())
        {
            for( bit2 = bit+1; bit2!=curr_block->bps.end(); ++bit2)
            {
                new_bps.push_back(*bit2);
            }
        }
        curr_block->bps = new_bps;

        if(make_root_block)
        {
            root = curr_block;
        }
            
        curr_block->filled++;
        if(curr_block->filled>=order)
        {
            int mid = curr_block->filled/2;
            vector<int> left_keys;
            vector<int> right_keys;
            vector<Block *> left_children;
            vector<Block *> right_children;
            int parent_key = curr_block->keys[mid];
            for(int i=0; i<mid; i++)
            {
                left_keys.push_back(curr_block->keys[i]);
                left_children.push_back(curr_block->bps[i]);
            }
            left_children.push_back(curr_block->bps[mid]);
            for(int i=mid+1; i<curr_block->filled; i++)
            {
                right_keys.push_back(curr_block->keys[i]);
                if(i<curr_block->bps.size())
                {
                    right_children.push_back(curr_block->bps[i]);
                }
            }
            if(curr_block->filled < curr_block->bps.size())
            {
                right_children.push_back(curr_block->bps[curr_block->filled]);
            }
            Block * left_child = create_block(left_keys, left_children, false);
            Block * right_child = create_block(right_keys, right_children, false);
            for(int i=0; i<left_children.size(); i++)
            {
                left_children[i]->parent_block = left_child;
            }
            for(int i=0; i<right_children.size(); i++)
            {
                right_children[i]->parent_block = right_child;
            }
            Block * parent_block = curr_block->parent_block;
            if(parent_block==NULL)
            {
                vector<int> parent_keys;
                vector<Block *> parent_children;
                parent_block = create_block(parent_keys, parent_children, false);
            }
            left_child->parent_block = parent_block;
            right_child->parent_block = parent_block;
            bool make_root_block = (root == curr_block);
            insert_internal_node(parent_block, parent_key,left_child,right_child, make_root_block);
        }
        
    }

    void insert_leaf(Block * curr_block, int key)
    {
        if(root == NULL)
        {
            vector<int> root_keys;
            root_keys.push_back(key);
            vector<Block *> root_children;
            root = create_block(root_keys,root_children,true);
            return;
        }
        if(curr_block == NULL)
        {
            return;
        }
        vector<int>::iterator it = curr_block->keys.begin();
        int i=0;
        for( ; it!=curr_block->keys.end(); ++it)
        {
            if(*it > key)
            {
                break;
            }
            i++;
        }
        
        if(curr_block->is_leaf)
        {
            curr_block->keys.insert(it, key);
            curr_block->filled++;
            if(curr_block->filled >= order)
            {
                int mid = curr_block->filled/2;
                vector<int> left_keys;
                vector<int> right_keys;
                vector<Block *> left_children;
                vector<Block *> right_children;
                int parent_key = curr_block->keys[mid];
                for(int i=0; i<mid; i++)
                {
                    left_keys.push_back(curr_block->keys[i]);
                }
                for(int i=mid; i<curr_block->filled; i++)
                {
                    right_keys.push_back(curr_block->keys[i]);
                }
                Block * left_child = create_block(left_keys, left_children, true);
                Block * right_child = create_block(right_keys, right_children, true);
                Block * parent_block = curr_block->parent_block;
                if(parent_block==NULL)
                {
                    vector<int> parent_keys;
                    vector<Block *> parent_children;
                    parent_block = create_block(parent_keys, parent_children, false);
                }
                left_child->parent_block = parent_block;
                right_child->parent_block = parent_block;
                bool make_root_block = (root == curr_block);
                insert_internal_node(parent_block, parent_key,left_child,right_child, make_root_block);
            }
        }
        else
        {
            insert_leaf(curr_block->bps[i],key);
        }

    }

    void insert(int key)
    {
        insert_leaf(root,key);
    }

    bool find(Block * curr_block, int key)
    {
        int i=0;
        for( ; i<curr_block->keys.size(); i++)
        {
            if(curr_block->keys[i]==key)
            {
                return true;
            }
            if(curr_block->keys[i]>key)
            {
                return find(curr_block->bps[i],key);
            }
        }
        if(i < curr_block->bps.size())
        {
            return find(curr_block->bps[i],key);
        }
        return false;
    }
    bool find(int key)
    {
        return find(root,key);
    }

    void print_tree(Block * curr_block)
    {
        if(curr_block == NULL)
        {
            return;
        }
        if(curr_block->is_leaf)
        {
            for(int i=0; i<curr_block->keys.size(); i++)
            {
                cout<<curr_block->keys[i]<<" ";
            }
        }
        else
        {
            int i=0;
            for( ; i<curr_block->filled; i++)
            {
                cout<<"(";
                print_tree(curr_block->bps[i]);
                cout<<")";
                cout<<" "<<curr_block->keys[i]<<" ";
            }
            cout<<"(";
            print_tree(curr_block->bps[i]);
            cout<<")";
        }
    }

    void print_tree()
    {
        if(root == NULL)
        {
            cout<<"\nTree is empty\n";
        }
        cout<<endl;
        print_tree(root);
        cout<<endl;
    }
    int traverse_leaves(Block * curr_node, int op, vector<int> param, int count)
    {
        if(curr_node->is_leaf)
        {
            for(int i=0; i<curr_node->keys.size(); i++)
            {
                int key = curr_node->keys[i];
                if(op == 1 && key==param[0])
                {
                    count++;
                }
                else if(op == 2 && key >= param[0] && key <= param[1])
                {
                    count++;
                }
            }
        }
        else
        {
            for(int i=0; i<curr_node->bps.size(); i++)
            {
                count = traverse_leaves(curr_node->bps[i], op, param, count);
            }
        }
        return count;
        
    }
    int count(int x)
    {
        vector<int> param;
        param.push_back(x);
        return traverse_leaves(root,1,param,0);
    }
    int range(int x, int y)
    {
        vector<int> param;
        param.push_back(x);
        param.push_back(y);
        return traverse_leaves(root,2,param,0);

    }

};

vector<string> string_split(string str)
{
    vector<string> split;
    string curr_string = "";
    for(int i=0; i<str.length(); i++)
    {
        char c = str[i];
        if(c==' ')
        {
            split.push_back(curr_string);
            curr_string = "";
        }
        else
        {
            curr_string+=c;
        }
    }
    if(!curr_string.empty())
    {
        split.push_back(curr_string);        
    }
    return split;
}


int main(int argc, char ** argv)
{
    if(argc < 4)
    {
        cout<<endl<<"INVALID NUMBER OF ARGUEMENTS"<<endl;
        exit(1);
    }
    ifstream infile(argv[1]);
    int M = stoi(argv[2]);
    int B = stoi(argv[3]);
    string line;
    BPlusTree bp = BPlusTree(5);
    string output = "";
    int iter = 0;
    while (getline(infile, line))
    {
        vector<string> command = string_split(line);
        if(!command[0].compare("INSERT"))
        {
            int num = stoi(command[1]);
            bp.insert(num);
        }
        else if(!command[0].compare("FIND"))
        {
            int num = stoi(command[1]);
            if(bp.find(num))
            {
                output = output.append("YES\n");
            }
            else
            {
                output = output.append("NO\n");
            }
            
        }
        else if(!command[0].compare("COUNT"))
        {
            int num = stoi(command[1]);
            output = output.append(to_string(bp.count(num)));
            output = output.append("\n");
        }
        else if(!command[0].compare("RANGE"))
        {
            int num1 = stoi(command[1]);
            int num2 = stoi(command[2]);
            output = output.append(to_string(bp.range(num1,num2)));
            output = output.append("\n");
        }
        else
        {
            cout<<"\nINVALID COMMAND :::: "<<command[0]<<endl;
            exit(1);
        }
        if(iter >= B)
        {
            iter = 0;
            cout<<output;
            output = "";
        }
        else
        {
            iter++;
        }
        
    }
    if(!output.empty())
    {
        cout<<output;
    }
    return 0;
}

