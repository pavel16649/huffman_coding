#include <string>
#include <fstream>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <stack>
#include "json.hpp"

#define ll int64_t
#define sht short
#define mp(a, b) make_pair(a, b)
#define forn(a, b) for (int i = a; i < b; ++i)

struct TreeNode {
    TreeNode()
        : ch(0)
        , left(nullptr)
        , right(nullptr) 
    {

    }

    TreeNode(char c, TreeNode* l, TreeNode* r)
        : ch(c)
        , left(l)
        , right(r)
    {

    }

    char ch;
    TreeNode* left; //0
    TreeNode* right; //1
};

struct Cmp {
    bool operator() (const std::pair <char, int> & lhs, const std::pair <char, int> & rhs) const {
        return lhs.second < rhs.second;
    }
};

struct SetNode {
    SetNode(int cnt, TreeNode* nd)
        : count(cnt)
        , node(nd)
    {

    }

    int count;
    TreeNode* node;
};

bool operator<(const SetNode& l, const SetNode& r) {
    return l.count < r.count;
}

TreeNode* generate() {
    std::ifstream input("input.txt");    
    
    std::map <char, int> m;
    char ch;
    while (input) {
        input.get(ch);
        if (m.count(ch)) {
            ++m[ch];
        } else {
            m[ch] = 1;
        }
    }
    input.close();

    std::multiset <SetNode> s;
    for (auto elem : m) {
        TreeNode* tmp = new TreeNode(elem.first, nullptr, nullptr);
        s.insert(SetNode(elem.second, tmp));
    }

    if (s.size() == 1) {
        return new TreeNode(0, s.begin()->node, nullptr);
    }

    while (s.size() != 1) {
        int new_count = 0;

        SetNode tmp = *s.begin();
        s.erase(s.begin());
        new_count += tmp.count;
        TreeNode* new_node = new TreeNode(0, tmp.node, nullptr);
        
        tmp = *s.begin();
        s.erase(s.begin());
        new_count += tmp.count;
        new_node->right = tmp.node;

        s.insert(SetNode(new_count, new_node));
    }
    
    return s.begin()->node;
}

std::map <char, std::string> decoder;

void make_decoder_recursive(TreeNode* current, std::string code) {
    if (current->ch != 0) {
        decoder[current->ch] = code;
        return;
    }
    make_decoder_recursive(current->left, code + "0");
    if (current->right != nullptr) make_decoder_recursive(current->right, code + "1");
}

void decode(TreeNode* root) {
    std::ifstream input("out.bin", std::ios::binary);

    input.seekg(0, std::ios::end);
    int length = input.tellg();
    input.seekg(0, std::ios::beg);
    char* buffer = new char[length];
    input.read(buffer, length);
    input.close();

    unsigned char skip1 = *((unsigned char*)buffer);

    char skip = skip1;

    unsigned char byte;
    unsigned char ind;
    unsigned char one = 1;
    TreeNode* current = root;
    std::ofstream out("decoded.txt");
    
    
    for (int i = 1; i < length - 1; ++i) {
        byte = buffer[i];
        ind = 7;
        while (true) {
            if (current->ch != 0) {
                out << current->ch;
                current = root;
                
            } else {
                if (byte & (one << ind)) {
                    current = current->right;
                } else {
                    current = current->left;
                }
                if (ind == 0) break;
                --ind;               
            }
        }
    }

    ind = 7;
    byte = buffer[length - 1];
    while ((char)ind >= skip - 1) {
        if (current->ch != 0) {
            out << current->ch;
            current = root;
        } else {
            if (byte & (one << ind)) {
                current = current->right;
            } else {
                current = current->left;
            }
            --ind;
        }
    }
      
    out.close();
}

void make_decoder(TreeNode* tree) {
    decoder.clear();
    make_decoder_recursive(tree, "");
}

void json_to_tree_recursive(TreeNode* current, nlohmann::json cur_j) {
    auto it = cur_j.begin();
    if (it.key() == "char") {
        current->ch = it.value().get<std::string>()[0];
        return;
    }
    if (it.key() == "0") {
        current->left = new TreeNode(0, nullptr, nullptr);
        json_to_tree_recursive(current->left, it.value());
        ++it;
    }

    if (it != cur_j.end() && it.key() == "1") {
        current->right = new TreeNode(0, nullptr, nullptr);
        json_to_tree_recursive(current->right, it.value());
    }
}

TreeNode* json_to_tree() {
    std::ifstream input("tree.json");
    nlohmann::json j;
    input >> j;
    input.close();

    TreeNode* root = new TreeNode(0, nullptr, nullptr);
    TreeNode* current = root;

    json_to_tree_recursive(root, j);
    return root;
}

void tree_to_json_recursive(TreeNode* tree, nlohmann::json &current) {
    if (tree->left != nullptr) {
        current["0"] = nullptr;
        tree_to_json_recursive(tree->left, current["0"]);
    }
    if (tree->right != nullptr) {
        current["1"] = nullptr;
        tree_to_json_recursive(tree->right, current["1"]);
    }
    if (tree->ch != 0) {
        std::string tmp = "";
        tmp += tree->ch;
        current["char"] = tmp;
    }
}

void tree_to_json(TreeNode* tree) {
    nlohmann::json j;
    tree_to_json_recursive(tree, j);
    std::ofstream tree_json("tree.json");
    tree_json << std::setw(2) << j;
    tree_json.close();
}

void encode(std::map <char, std::string> &table) { 
    std::ifstream input("input.txt");
    
    input.seekg(0, std::ios::end);
    int length = input.tellg();
    input.seekg(0, std::ios::beg);
    char* buffer = new char[length];
    input.read(buffer, length);
    input.close();

    unsigned char bits = 0;
    for (int i = 0; i < length; ++i) {
        bits += table[buffer[i]].size();
    }
    bits = (bits % 8 == 0) ? 0 : 8 - bits % 8;

    std::ofstream out("out.bin", std::ios::binary);
    out << bits;
    
    unsigned char ind_b = 0;
    unsigned char max = 255;
    unsigned char byte = 0; 
    
    unsigned char ch;
    for (int i = 0; i < length; ++i) {
        ch = buffer[i];
        std::string s = table[ch];
        
        for(int ind_s = 0; ind_s < s.size(); ) {
            if (s[ind_s] == '1') {
                byte |= 1;
            }

            ++ind_s;
            ++ind_b;

            if (ind_b == 8) {
                ind_b = 0;
                out << byte;
                byte = 0;
            } else {
                byte <<= 1;
            }
        }
    }
    if (bits != 0) {
        byte <<= bits - 1;
        out << byte;
    }

    input.close();
    out.close();
}

int main() {
    TreeNode* root = generate();
    tree_to_json(root);
    TreeNode* new_root = json_to_tree();
    make_decoder(new_root);
    encode(decoder);
    decode(new_root);

    return 0;
}
