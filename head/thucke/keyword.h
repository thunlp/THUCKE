#ifndef __keyword_h__
#define __keyword_h__

#include "thucke/trie.h"
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <utility>

class keyword{
public:
    //加载模型，两个参数分别为分词模型、关键词抽取模型
    bool init(std::string, std::string);
    std::string getKeyword(const std::string&, int);
private:
    trie stopwords, wikiwords, bookwords, lexicon;
    std::map< std::pair<int, int>, double > probabilityTable, inverseTable;
    std::vector<int> bookwordsTrieId2Id;
    std::vector<std::string> bookwordsId2Str;
    std::vector<std::string> booktagsId2Str;
    std::vector<int> lexiconTrieId2Tf, lexiconTrieId2Df;
    static const int tag_ns = 1, tag_np = 2, tag_nz = 4, tag_ni = 8, tag_id = 16, tag_j = 32, tag_npsz = 63;
    static const int tag_n = 64, tag_i = 128, tag_v = 256, tag_x = 512;
    int lexiconNumDocs, maxBookwordsId, maxBooktagsId;
    bool loadStopwords(std::ifstream&);
    bool loadWikiwords(std::ifstream&);
    bool loadModel(std::string);
    int tag2Int(std::string);
};

#endif
