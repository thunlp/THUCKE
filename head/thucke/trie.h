#ifndef __trie_h__
#define __trie_h__

#include <string>
#include <map>
#include <stack>
#include <vector>
#include <utility>

class trie {
    friend class iterator;
    
public:
    trie();
    
    
    class iterator {
        friend class trie;
        friend bool operator == (const iterator&, const iterator&);
        friend bool operator != (const iterator&, const iterator&);
        friend std::string operator * (const iterator&);
        friend void operator ++ (iterator&, int);
    
    public:
        iterator();
        int getTag();
        
    private:
        std::stack< std::pair< int, std::map< char, int >::iterator > > route;
        std::string word;
        trie* pointer;
        int wordTag;
        void clear();
    };
    
    friend void operator ++ (iterator&, int);
    
    
    iterator begin();
    iterator end();
    int insert(const std::string &word);
    int find(const std::string &word);
    int countWord();

private:
    std::vector< std::map< char, int > > tree;
    std::vector< int > tag;
    int nodeCount;
    int wordCount;
    static const int root = 1;
    iterator beginIterator, endIterator;
    int insert(int id, const std::string &word, int depth);
    int find(int id, const std::string &word, int depth);
};

#endif
