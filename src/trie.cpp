#include "thucke/trie.h"

#ifdef TESTER
#include <iostream>
#endif

trie::trie() {
    tree.resize(2);
    tag.resize(2);
    nodeCount = 1;
    wordCount = 0;
}

int trie::insert(int id, const std::string &word, int depth) {
    if (depth == word.size()) {
        if (!tag[id]) tag[id] = ++wordCount;
        return tag[id];
    } else {
        if (tree[id].find(word[depth]) == tree[id].end()) {
            tree[id][ word[depth] ] = ++nodeCount;
            tree.push_back(std::map< char, int >());
            tag.push_back(0);
        }
        return insert(tree[id][ word[depth] ], word, depth + 1);
    }
}

int trie::insert(const std::string &word) {
    beginIterator.clear();
    endIterator.clear();
    if (word.size())
        return insert(root, word, 0);
    else
        return 0;
}

int trie::find(int id, const std::string &word, int depth) {
    if (depth == word.size())
        return tag[id];
    if (tree[id].find(word[depth]) != tree[id].end())
        return find(tree[id][ word[depth] ], word, depth + 1);
    return 0;

}

int trie::find(const std::string &word) {
    return find(root, word, 0);
}

int trie::countWord() {
    return wordCount;
}

trie::iterator trie::begin() {
    if (beginIterator.pointer == NULL) {
        beginIterator.clear();
        beginIterator.pointer = this;
        if (this->wordCount) {
            int i = root;
            while (!tag[i]) {
                beginIterator.route.push(std::make_pair(i, tree[i].begin()));
                beginIterator.word += tree[i].begin()->first;
                i = tree[i].begin()->second;
            }
            beginIterator.wordTag = tag[i];
        }
    }
    return beginIterator;
}

trie::iterator trie::end() {
    if (endIterator.pointer == NULL) {
        endIterator.clear();
        endIterator.pointer = this;
    }
    return endIterator;
}

trie::iterator::iterator() {
    clear();
}

int trie::iterator::getTag() {
    return wordTag;
}

void trie::iterator::clear() {
    pointer = NULL;
    wordTag = 0;
    word = "";
    while (!route.empty()) route.pop();
}

bool operator != (const trie::iterator& a, const trie::iterator& b) {
    return a.pointer != b.pointer || a.wordTag != b.wordTag;
}

bool operator == (const trie::iterator& a, const trie::iterator& b) {
    return a.pointer == b.pointer && a.wordTag == b.wordTag;
}

std::string operator * (const trie::iterator& a) {
    return a.word;
}

void operator ++ (trie::iterator& a, int _) {
    trie *T = a.pointer;
    if (a == T->end()) return;
    int nowID = a.route.top().second->second;

    do {
        if (T->tree[nowID].size()) {
            a.route.push(make_pair(nowID, T->tree[nowID].begin()));
            a.word += T->tree[nowID].begin()->first;

            nowID = T->tree[nowID].begin()->second;
        } else {
            do {
                std::map<char, int>::iterator ii = a.route.top().second;
                nowID = a.route.top().first;
                a.word.resize(a.word.size() - 1);
                a.route.pop();
                if (++ii != T->tree[nowID].end()) {
                    a.route.push(std::make_pair(nowID, ii));
                    a.word += ii->first;
                    nowID = ii->second;
                    break;
                }
            } while (a.route.size());
        }
    } while (a.route.size() && !T->tag[nowID]);
    if (!a.route.size()) {
        a.clear();
        a.pointer = T;
    } else
        a.wordTag = T->tag[nowID];
}


#ifdef TESTER
#include <iostream>

using namespace std;

int main() {
    trie T;
    while (1) {
        char fun;
        std::cin >> fun;
        std::string str;
        switch (fun) {
            case 'I':
                std::cin >> str;
                T.insert(str);
                break;
            case 'F':
                std::cin >> str;
                std::cout << T.find(str) << std::endl;
                break;
            case 'P':
                for (trie::iterator ii = T.begin(); ii != T.end(); ii++)
                    std::cout << *ii << std::endl;
                break;
            case 'C':
                std::cout << T.countWord() << std::endl;
                break;
            default:
                exit(0);
                break;
        }
    }
}

#endif
