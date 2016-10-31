#include <cstring>
#include <cstdlib>
#include <sstream>
#include "thucke/keyword.h"
#include "thulac/segment_wrapper.h"

bool keyword::init(std::string modelSegment, std::string modelExtract) {
    if(!CThulacSegment_Init(modelSegment)) return false;

    if (modelExtract[modelExtract.size() - 1] != '/') modelExtract += '/';

    std::ifstream model;

    model.open((modelExtract + "wiki_entities_ch.txt").c_str(), std::ifstream::in);
    if(!loadWikiwords(model)) return false;
    model.close();

    model.open((modelExtract + "chinese_stop_word.txt").c_str(), std::ifstream::in);
    if(!loadStopwords(model)) return false;
    model.close();

    if (!loadModel(modelExtract)) return false;
    return true;
}

bool keyword::loadStopwords(std::ifstream &model) {
    if (!model.is_open()) return false;
    std::string phrase;
    while (model >> phrase) stopwords.insert(phrase);
    return true;
}

bool keyword::loadWikiwords(std::ifstream &model) {
    if (!model.is_open()) return false;
    std::string phrase;
    while (model >> phrase) wikiwords.insert(phrase);
    return true;
}

bool keyword::loadModel(std::string modelPath) {
    //Anounce all temporary variables
    std::ifstream model;
    std::string word;
    int id, count, trieId, first, second, tf, df;
    double probability;

    //Read backward.vcb
    maxBookwordsId = 0;
    model.open((modelPath + "content.vcb").c_str(), std::ifstream::in);
    bool flag = 0;
    if (!model.is_open()) return false;
    while (model >> id >> word >> count) {
        flag = 1;
        trieId = bookwords.insert(word);
        bookwordsId2Str.resize(std::max((int)bookwordsId2Str.size(), id + 1));
        bookwordsId2Str[id] = word;
        bookwordsTrieId2Id.resize(std::max((int)bookwordsTrieId2Id.size(), trieId + 1));
        bookwordsTrieId2Id[trieId] = id;
        maxBookwordsId = std::max(maxBookwordsId, id);
    }
    if (!flag) return 0;
    model.close();

    //Read keyword.vcb
    maxBooktagsId = 0;
    model.open((modelPath + "keyword.vcb").c_str(), std::ifstream::in);
    flag = 0;
    if (!model.is_open()) return false;
    while (model >> id >> word >> count) {
        flag = 1;
        booktagsId2Str.resize(std::max((int)booktagsId2Str.size(), id + 1));
        booktagsId2Str[id] = word;
        maxBooktagsId = std::max(maxBooktagsId, id);
    }
    if (!flag) return 0;
    model.close();

    //Read pro_forward
    model.open((modelPath + "pro_forward").c_str(), std::ifstream::in);
    if (!model.is_open()) return false;
    flag = 0;
    while (model >> first >> second >> probability) {
        flag = 1;
        if (!first || !second) continue;
        if (first > maxBookwordsId) continue;
        if (second > maxBooktagsId) continue;
        probabilityTable[std::make_pair(first, second)] = probability;
    }
    if (!flag) return 0;
    model.close();

    //Read pro_backward
    model.open((modelPath + "pro_backward").c_str(), std::ifstream::in);
    if (!model.is_open()) return false;
    flag = 0;
    while (model >> first >> second >> probability) {
        flag = 1;
        if (!first || !second) continue;
        if (first > maxBookwordsId) continue;
        if (second > maxBooktagsId) continue;
        inverseTable[std::make_pair(first, second)] = probability;
    }
    if (!flag) return 0;
    model.close();

    //read wordlex
    model.open((modelPath + "wordlex").c_str(), std::ifstream::in);
    if (!model.is_open()) return false;
    model >> lexiconNumDocs;
    std::string line;
    flag = 0;
    while (model >> line) {
        flag = 1;
        for (int i = 0; i < line.size(); i++)
            if (line[i] == ':') line[i] = ' ';
        std::istringstream linestream(line);
        if (linestream >> word >> tf >> df) {
            trieId = lexicon.insert(word);
            lexiconTrieId2Tf.resize(std::max((int)lexiconTrieId2Tf.size(), trieId + 1));
            lexiconTrieId2Tf[trieId] = tf;
            lexiconTrieId2Df.resize(std::max((int)lexiconTrieId2Df.size(), trieId + 1));
            lexiconTrieId2Df[trieId] = df;
        }
    }
    if (!flag) return 0;
    model.close();

    return true;
}
