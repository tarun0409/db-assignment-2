#include <cstdlib>
#include <iostream>
#include <queue>
#include <fstream>
#include <cmath>

using namespace std;


class Hash {
    int n_r, n_b;
    vector<Block *> bs;
    int B;

    public:
    Hash(int B) 
    {
        n_r = 0;
        n_b = 1;
        this->B = B;
        bs.push_back(new Block(B));
        bs.push_back(new Block(B));
    }

    int give_hash(int x) 
    {
        int mod = (1 << n_b);
        int res = x%mod;
        res = res + mod;
        res = res%mod;
        return (unsigned int)(x % mod + mod) % mod;
    }

    bool available(int x) 
    {
        unsigned int k = give_hash(x);
        if(k >= bs.size()) {
            k = k - (1 << (n_b - 1));
        }
        if(bs[k]->available(x)) {
            return true;
        }
        return false;
    }

    int presence() 
    {
        double ratio = 1.0 * n_r / bs.size();
        return (int)(100 * (ratio / (B / 4)));
    }

    void insert(int x) 
    {
        int k = give_hash(x);
        if(k >= bs.size()) {
            k -= (1 << (n_b - 1));
        }
        n_r++;
        bs[k]->add(x);
        while(presence() >= 75) 
        {
            bs.push_back(new Block(B));
            n_b = ceil(log2((double)bs.size()));
            k = bs.size() - 1 - (1 << (n_b - 1));
            vector<int> v;
            bs[k]->flush(v);
            for(unsigned int i = 0; i < v.size(); i++) 
            {
                bs[give_hash(v[i])]->add(v[i]);
            }
        }
    }

};

class Block 
{
    int B;
    Block * ofw;
    vector<int> records;
    
    public:
    
    Block(int B) 
    {
        ofw = NULL;
        this->B = B;
        records.clear();
    }

    void add(int x) 
    {
        if(records.size() < (B / 4)) 
        {
            records.push_back(x);
        }
        else 
        {
            if(ofw == NULL) 
            {
                ofw = new Block(B);
            }
            ofw->add(x);
        }
    }

    void flush(vector<int> v) 
    {
        for(int i = 0; i < records.size(); i++) 
        {
            v.push_back(records[i]);
        }
        records.clear();
        if(ofw) 
        {
            ofw->flush(v);
            delete ofw;
            ofw = NULL;
        }
    }

    bool available(int x) 
    {
        Block *node = this;
        while(node) 
        {
            for(unsigned int i = 0; i < node->records.size(); i++) 
            {
                if(node->records[i] == x) 
                {
                    return true;
                }
            }
            node = node->ofw;
        }
        return false;
    }

};

void flust_out(queue<int> obf) 
{
    int o;
    while(!obf.empty()) 
    {
        o = obf.front();
        obf.pop();
        cout << o << '\n';
    }
}

void flust_inp(Hash h, queue<int> inbf, queue<int> obf, int ops) 
{
    int i;
    while(!inbf.empty()) 
    {
        i = inbf.front();
        inbf.pop();
        if(!h.available(i)) 
        {
            h.insert(i);
            if(obf.size() == ops) 
            {
                flust_out(obf);
            }
            obf.push(i);
        }
    }
}

int main(int argc, char **argv) {
    if(argc < 4) 
    {
        cout<<"\nINVALID NUMBER OF ARGUEMENTS!\n";
        exit(1);
    }
    ifstream input(argv[1]);
    int M = atoi(argv[2]);
    int B = atoi(argv[3]); 
    queue<int> inbf, obf;
    Hash h = Hash(B);

    int inputSize = (M - 1) * (B / 4);
    int ops = B / 4;
    int x;
    while(input >> x) {
        if(inbf.size() < inputSize) {
            inbf.push(x);
        }
        else {
            flust_inp(h, inbf,obf, ops);
            inbf.push(x);
        }
    }
    flust_inp(h, inbf,obf,ops);
    flust_out(obf);
    return 0;
}