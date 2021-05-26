#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <algorithm>

using namespace std;

class Node {
public:
    int name;
    int count;
    Node *parent;
    Node *link;
    vector<Node *> children;
    Node(int name, int count, Node *parent=NULL, Node *link=NULL) {
        this->name = name;
        this->count = count;
        this->parent = parent;
        this->link = link;
    }
};

class Entry {
public:
    int name;
    int count;
    Node *link;
    
    Entry(int name, int count, Node *link=NULL) {
        this->name = name;
        this->count = count;
        this->link = link;
    }
};

int *getFreqTable(vector<vector<int>> *transactions, int largestItem) {
    int *table = new int[largestItem+1];
    for (int i = 0; i <= largestItem; ++i) {
        table[i] = 0;
    }
    for (vector<int> tx: *transactions) {
        for (int item: tx) {
            ++table[item];
        }
    }
    return table;
}

bool checkItem(vector<int> *uselessItems, int item) {
    for (int useless: *uselessItems) {
        if (item == useless)
            return true;
    }
    return false;
}

void clean(vector<vector<int>> *transactions, int freqTable[], double minSupport, int largestItem) {
    vector<int> uselessItems;
    for (int i = 0; i <= largestItem; ++i) {
        if (freqTable[i] < minSupport) {
            uselessItems.push_back(i);
        }
    }
    for (vector<vector<int>>::iterator tx = transactions->begin(); tx != transactions->end();) {
        for (vector<int>::iterator it = tx->begin(); it != tx->end();) {
            if (checkItem(&uselessItems, *it)) {
                tx->erase(it);
            } else {
                ++it;
            }
        }
        if (tx->empty()) {
            transactions->erase(tx);
        } else {
            sort(tx->begin(), tx->end(), [&freqTable](int a, int b) {return freqTable[a] > freqTable[b];});
            ++tx;
        }
    }
    
}

vector<Entry *> constructHeader(int freqTable[], int largestItem, double minSupport) {
    int *name = new int[largestItem+1];
    for (int i = 0; i <= largestItem; ++i) {
        name[i] = i;
    }
    sort(name, name+largestItem+1, [&freqTable](int a, int b) {return freqTable[a] > freqTable[b];});
    sort(freqTable, freqTable+largestItem+1, greater<int>());
    
    vector<Entry *> header;
    for (int i = 0; i <= largestItem; ++i) {
        if (freqTable[i] < minSupport) {
            return header;
        }
        header.push_back(new Entry(name[i], freqTable[i]));
    }
    return header;
}

vector<vector<int>> *constructConditionalTree(Node *head) {
    vector<vector<int>> *data = new vector<vector<int>>();
    while (head != NULL) {
        Node *cur = head;
        vector<int> tx;
        while (cur->parent->name != -1) {
            tx.push_back(cur->parent->name);
            cur = cur->parent;
        }
        if (tx.size() == 0) {
            head = head->link;
            continue;
        }
        for (int i = 0; i < head->count; ++i) {
            data->push_back(tx);
        }
        head = head->link;
    }
    return data;
}

Node *constructTree(vector<Entry *> *header, vector<vector<int>> *data) {
    Node *root = new Node(-1, 0);
    for (vector<int> tx: *data) {
        Node *cur = root;
        for (int item: tx) {
            bool found = false;
            for (Node *child: cur->children) {
                if (child->name == item) {
                    found = true;
                    child->count += 1;
                    cur = child;
                    break;
                }
            }
            if (!found) {
                Node *tmp = new Node(item, 1, cur);
                cur->children.push_back(tmp);
                cur = tmp;
                for (Entry *e: *header) {
                    if (e->name == item) {
                        if (e->link == NULL) {
                            e->link = cur;
                        } else {
                            tmp = e->link;
                            while (tmp->link != NULL) {
                                tmp = tmp->link;
                            }
                            tmp->link = cur;
                        }
                        break;
                    }
                }
            }
        }
    }
    return root;
}

void mineTree(vector<Entry *> *header, int minSupport, vector<int> *prefix, vector<vector<int>> *frequentItems) {
    reverse(header->begin(), header->end());

    for (Entry *e: *header) {
        vector<int> tmp;
        for (int item: *prefix) {
            tmp.push_back(item);
        }
        tmp.push_back(e->name);
        sort(tmp.begin(), tmp.end());
        frequentItems->push_back(tmp);
        vector<vector<int>> *data = constructConditionalTree(e->link);
        int largestItem = 0;
        for (vector<int> tx: *data) {
            for (int item: tx) {
                if (largestItem < item) {
                    largestItem = item;
                }
            }
        }
        int *table = getFreqTable(data, largestItem);
        vector<Entry *> newHeader = constructHeader(table, largestItem, minSupport);
        if (newHeader.size() != 0) {
            // Construct conditional tree
            Node *conditionalTree = constructTree(&newHeader, data);
            // Recursively mine conditional tree
            mineTree(&newHeader, minSupport, &tmp, frequentItems);
        }
    }
}

/*
First argument is the minimum support ratio
Second argument is the name of the input file
Third argument is the name of the output file
*/
int main(int argc, char *argv[]) {
    double minSupportRatio = stod(argv[1]);
    string in = argv[2];
    string out = argv[3];
    ifstream input(in);
    ofstream outfile;
    string line;
    vector<vector<int>> data;
    vector<vector<int>> transactions;

    int num = 0;
    int largestItem = 0;
    while (getline(input, line)) {
        stringstream ss(line);
        vector<int> tmp;
        for (int j; ss >> j;) {
            tmp.push_back(j);
            if (largestItem < j)
                largestItem = j;
            if (ss.peek() == ',') {
                ss.ignore();
            }
        }
        data.push_back(tmp);
        transactions.push_back(tmp);
        ++num;
    }
    input.close();
    double length = data.size();
    
    double minSupport = minSupportRatio * num;
    int *table = getFreqTable(&data, largestItem);
    clean(&data, table, minSupport, largestItem);

    vector<Entry *> header = constructHeader(table, largestItem, minSupport);
    Node *FPtree = constructTree(&header, &data);

    vector<vector<int>> frequentItems;
    vector<int> prefix;

    mineTree(&header, minSupport, &prefix, &frequentItems);

    sort(frequentItems.begin(), frequentItems.end(), [](const vector<int> & a, const vector<int> & b){ 
        if (a.size() != b.size()) {
            return a.size() < b.size(); 
        }
        int i = 0;
        for (i = 0; i < a.size(); ++i) {
            if (a[i] != b[i]) 
                break;
                
        }
        return a[i] < b[i];
    });

    for (vector<vector<int>>::iterator tx = transactions.begin(); tx != transactions.end(); ++tx) {
        sort(tx->begin(), tx->end());
    }

    outfile.open(out);
    
    for (vector<int> items: frequentItems) {
        double total = 0;
        for (vector<int> tx: transactions) {
            int j = 0;
            for (int i = 0; i < tx.size(); ++i) {
                if (tx[i] == items[j]) {
                    ++j;
                }
                if (j == items.size()) {
                    ++total;
                    break;
                }
            }
        }

        double sup = total/length;
        for (int j = 0; j < items.size(); ++j) {
            if (j == items.size()-1) outfile << items[j];
            else outfile << items[j] << ",";
        }
        outfile << ":" << fixed << setprecision(4) << sup;
        outfile << endl;
    }
    outfile.close();
}
