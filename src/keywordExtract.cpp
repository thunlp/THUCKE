#include <set>
#include <cmath>
#include <ctime>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "thucke/keyword.h"
#include "thucke/articleSpliter.h"
#include "thulac/segment_wrapper.h"

const double eps = 1e-12;

class input_word {
public:
    int id; //该词在bookwords中的id，即该词在模型中的编码
    std::string str; //词字符串
    std::string tag_str; //词性的字符串表示
    int length; //词的长度char的个数，一个汉字长度是2
    int tag; //词性，用一个int表示
    int tf; //该词在输入中出现的总次数除以段落数
    int df; //该词的df，实际是这个词在几个段落中出现
    int lastAppear; //该词上次出现的段落编号，用以统计生成df
    double tfidf; //根据tf,模型中lexicon提供的df计算得到的tfidf
    double tfidf1; //根据词性修正（仅限在原文中出现的词）得到的tfidf
    double score; //=0.4*trans_word.score2
    double score1; //score + tfidf1/max(tfidf, tfidf1)
    int is_trans; //用以标记区分原文中的词和翻译得到的词，0为原文中的词，1为翻译得到的词（主要用于中间分析，结果并没有用到）

    int printself() {
        printf("%d %s %s %d %d %f %f %f %f %d\n", id, str.c_str(), tag_str.c_str(), tf, df, tfidf, score, tfidf1, score1, is_trans);
    }
};

class trans_word {
public:
    std::string str;
    double score1; //score1 = trans_pro * input_word_attr.tfidf
    double score2; //score2 = para2 * score1 + (1-para2)*tfidf/totalTfidf

    int printself() {
        printf("%s %f %f\n", str.c_str(), (float)score1, (float)score2);
    }
};

int keyword::tag2Int(std::string tag) {
    std::map<std::string, int> tag2int;
    tag2int["ns"] = tag_ns;
    tag2int["np"] = tag_np;
    tag2int["nz"] = tag_nz;
    tag2int["ni"] = tag_ni;
    tag2int["id"] = tag_id;
    tag2int["j"] = tag_j;
    tag2int["n"] = tag_n;
    tag2int["i"] = tag_i;
    tag2int["v"] = tag_v;
    tag2int["x"] = tag_x;

    if (tag2int.find(tag) != tag2int.end()) {
        return tag2int[tag];
    }
    else {
        return 0;
    }
}

int alter(int x) {
    if (x <= 0) return 1;
    return x;
}

std::string keyword::getKeyword(const std::string &input, int wordLimit) {
    double timer = 0, TT = 0;
    TT -= clock();

    trie input_word_trie; //输入文本存储到trie树中，查询速度快
    std::vector<input_word> input_word_vec; //输入文本中词语组成的数组，下标是input_word_trie树中的id
    std::map<int, trans_word> trans_word_map; //翻译得到的词所组成的map
    double para_trans_for_back = 0.5, para_tfidf_trans = 0.5;
    articleSpliter splited_input(input, 10000, articleSpliter::strategyLong);

    while (splited_input.hasMoreSplit()) {
        std::pair<std::string, int> sentence = splited_input.nextSplit(); //将输入的文本进行分割，一般按照标点符号分割，key是分成的字符串，value是这个字符串所在的段落
        timer -= clock();
        std::istringstream wordsStream(CThulacSegment_segment(sentence.first)); //使用分词和词性标注处理分割的字符串
        timer += clock();
        std::string wordAndTag; //“词_词性”的格式
        while (wordsStream >> wordAndTag) { //取出句子中的每一个词
            if (wordAndTag.size() > 100) {
                std::cout << wordAndTag << std::endl;
                continue;
            }

            //分别取出分词中的词语和词性
            std::string word, tag;
            int pos = wordAndTag.find('_');
            if (pos == std::string::npos) continue;
            word = wordAndTag.substr(0, pos);
            tag = wordAndTag.substr(pos + 1);

            if (stopwords.find(word)) continue; //去掉停用词
            int trieId = input_word_trie.insert(word); //把词存到trie树中，如果已经在trie树中存在则返回trieId

            if (trieId >= input_word_vec.size()) {
                input_word_vec.resize(trieId + 1);
                input_word_vec[trieId] = input_word();
                input_word_vec[trieId].str = word;
                input_word_vec[trieId].tag_str = tag;
                input_word_vec[trieId].length = word.size();
            }
            if (sentence.second != input_word_vec[trieId].lastAppear) {
                input_word_vec[trieId].lastAppear = sentence.second;
                input_word_vec[trieId].df ++; //df++
            }
            input_word_vec[trieId].tf ++;
            input_word_vec[trieId].tag |= tag2Int(tag); //tag赋值
        }
    }

//    TT += clock();
/*
    //print input_word_vec
    int tmp_total = input_word_vec.size() - 1;
    for (int i = 0; i < tmp_total; i ++) {
        input_word_vec[i].printself();
    }
    return "";
*/

    int totalWords = input_word_vec.size() - 1;
    double totalScore1 = 0, totalTfidf = 0;
    for (int i = 1; i <= totalWords; i++) { //遍历input_word_vec中所有的word
        int tmpTrieId = bookwords.find(input_word_vec[i].str); //当前word在bookwords这个trie树中的id，输入文本的词在bookwords中存在，给对应的词计算翻译概率
        input_word &tmp_word = input_word_vec[i];
        //score1存在的条件是，翻译原词在bookwords中，并且正反翻译概率都存在,否则score1为0;tfidf存在的条件是翻译原词在bookwords中，否则tfidf为0
        if(tmpTrieId){
            tmp_word.id = bookwordsTrieId2Id[tmpTrieId];
            double tf = (double)tmp_word.tf / tmp_word.df; //求该词的tf
            double idf = 0.0;
            int lexiconTrieId = lexicon.find(tmp_word.str);
            if (lexiconTrieId)
                idf = std::log((double)lexiconNumDocs / (double)lexiconTrieId2Df[lexiconTrieId]); //求该词的idf
            else
                idf = std::log((double)lexiconNumDocs / 800.0);
            double tfidf = tf * idf;
            tmp_word.tfidf = tfidf;
            std::map< std::pair<int, int>, double>::iterator tmp = probabilityTable.lower_bound(std::make_pair(tmp_word.id, 0));
            int tmpid = tmp_word.id;
            if (tmp != probabilityTable.end() && tmp->first.first == tmpid) {
                totalTfidf += tfidf;
                std::map< std::pair<int, int>, double>::iterator start = tmp, end, ii;
                end = probabilityTable.lower_bound(std::make_pair(tmpid + 1, 0));
                for (ii = start; ii != end; ii++) {
                    int trans_word_id = ii->first.second;
                    if (inverseTable.find(std::make_pair(tmpid, trans_word_id)) != inverseTable.end()) {
                        double pro = 1.0 / (para_trans_for_back / ii->second +
                                            (1.0 - para_trans_for_back) / inverseTable.find(std::make_pair(tmpid, trans_word_id))->second);
                        //double score1 = tfidf * pro;
                        if (trans_word_map.find(trans_word_id) != trans_word_map.end()){
                            trans_word_map[trans_word_id].score1 += tfidf * pro;
                        }
                        else {
                            trans_word_map[trans_word_id].score1 = tfidf * pro;
                        }
                        totalScore1 += tfidf * pro;
                    }
                }
            }
        }
    }

//    TT += clock();
/*
    //print input_word_vec
    int tmp_total = input_word_vec.size() - 1;
    for (int i = 0; i < tmp_total; i ++) {
        input_word_vec[i].printself();
    }

    std::map<int, trans_word>::iterator it;
    for(it=trans_word_map.begin(); it!=trans_word_map.end(); ++it) {
        printf("%d ", it->first);
        it->second.printself();
    }
    return "test";
*/

    double maxtrans_word_score2 = 0;
    if (totalScore1 < eps) totalScore1 = eps;
    if (totalTfidf < eps) totalTfidf = eps;
    for (std::map<int, trans_word>::iterator ii = trans_word_map.begin(); ii != trans_word_map.end(); ii++) {
        double score2 = para_tfidf_trans * ii->second.score1 / totalScore1;
        int trie_id = input_word_trie.find(booktagsId2Str[ii->first]); //看翻译得到的词在原文中是否存在
        if (!trie_id || !bookwords.find(input_word_vec[trie_id].str)) continue; //如果该翻译得到的词既没有在原文中 或者 没有在bookwords中，那么score2为0
        if (bookwords.find(booktagsId2Str[ii->first]))
            score2 += (1.0 - para_tfidf_trans) * input_word_vec[trie_id].tfidf / totalTfidf;
        trans_word_map[ii->first].score2 = score2;
        trans_word_map[ii->first].str = input_word_vec[trie_id].str;
        maxtrans_word_score2 = std::max(maxtrans_word_score2, score2);
    }

/*
    //print 翻译得到的词和score1, score2
    std::map<int, trans_word>::iterator it;
    for(it=trans_word_map.begin(); it!=trans_word_map.end(); ++it) {
        printf("%d ", it->first);
        it->second.printself();
    }
    return "test";
*/

    int allTotalWords = totalWords;
    if (maxtrans_word_score2 < eps) maxtrans_word_score2 = eps;
    for (std::map<int, trans_word>::iterator ii = trans_word_map.begin(); ii != trans_word_map.end(); ii++) {
        int trie_id = input_word_trie.find(booktagsId2Str[ii->first]);
        if (trie_id) {
            input_word_vec[trie_id].score = ii->second.score2 * 0.4 / maxtrans_word_score2;
        }
        else {
            ++allTotalWords;
            input_word_trie.insert(booktagsId2Str[ii->first]);
            input_word new_input_word = input_word();
            new_input_word.str = booktagsId2Str[ii->first];
            new_input_word.tfidf = 0;
            new_input_word.is_trans = 1;
            input_word_vec.resize(allTotalWords + 1);
            input_word_vec[allTotalWords] = new_input_word;
        }
    }

    double maxTfidf = 0.0;
    for (int i = 1; i <= totalWords; i++) {
        input_word &temp_input_word = input_word_vec[i];
        double tf = temp_input_word.tf, df = temp_input_word.df, score = temp_input_word.score;
        int tag = temp_input_word.tag;
        bool isNoun = false, isNpsz = false, isV = false, isKeep = false, isI = false, isX = false;

        if(score > 1e-9) {
            double tfidf1 = tf * log(df + 1.0) * ((double)temp_input_word.length) / log(2.0);
            temp_input_word.tfidf1 = tfidf1;
            maxTfidf = std::max(maxTfidf, tfidf1);
            continue;
        }

        if (tag & tag_npsz)
            isNpsz = isKeep = true;
        if (tag & tag_n)
            isKeep = isNoun = true;
        if (tag & tag_i)
            isI = isKeep = true;
        if (tag & tag_v)
            isV = isKeep = true;
        if (tag & tag_x)
            isX = isKeep = true;
        if (!isKeep) continue;
        if (isNoun) {
            if(wikiwords.find(temp_input_word.str)) {
                if(temp_input_word.length > 4) {
                    double tfidf1 = tf * log(df + 1.0) * ((double)temp_input_word.length) / log(2.0);
                    temp_input_word.tfidf1 = tfidf1;
                    maxTfidf = std::max(maxTfidf, tfidf1);
                }
                else if((temp_input_word.length == 4) && (df >= 2)){
                    double tfidf1 = 0.9 * tf * log(df + 1.0) * ((double)temp_input_word.length) / log(2.0);
                    temp_input_word.tfidf1 = tfidf1;
                    maxTfidf = std::max(maxTfidf, tfidf1);
                }
            }
        }
        else if (isNpsz) {
            if (wikiwords.find(temp_input_word.str) || (df >= 2)) {
                double tfidf1 = tf * log(df + 1.0) * 3.0 / log(2.0);
                temp_input_word.tfidf1 = tfidf1;
                maxTfidf = std::max(maxTfidf, tfidf1);
            }
        }
        else if (isI | isV) {
            if (wikiwords.find(temp_input_word.str)) {
                double tfidf1 = tf * log(df + 1.0) * 4.0 / log(2.0);
                temp_input_word.tfidf1 = tfidf1;
                maxTfidf = std::max(maxTfidf, score);
            }
        }
        else if (isX) {
            double tfidf1 = tf * log(df + 1.0) * 4.0 / log(2.0);
            temp_input_word.tfidf1 = tfidf1;
            maxTfidf = std::max(maxTfidf, score);
        }
    }

/*
    //print input_word_vec
    int tmp_total = input_word_vec.size() - 1;
    for (int i = 0; i < tmp_total; i ++) {
        input_word_vec[i].printself();
    }
    return "";
*/

    std::vector<std::pair<double, std::string>> last_score;
    last_score.resize(allTotalWords + 1);
    if (maxTfidf < eps) maxTfidf = eps;
    for (int i = 1; i <= allTotalWords; i++) {
        input_word_vec[i].tfidf1 /= maxTfidf;
        input_word_vec[i].score1 = input_word_vec[i].tfidf1 + input_word_vec[i].score;
        last_score[i] = std::make_pair(input_word_vec[i].score1, input_word_vec[i].str);
    }

    std::sort(last_score.begin(), last_score.end());

    double maxn;
    if (last_score.size()) {
        maxn = last_score.rbegin()->first;
        if (maxn < eps)
            last_score.rbegin()->first = maxn = eps;
    }

    std::string s;
    std::ostringstream stm;
    stm << "{\"result\":[";
    for (std::vector< std::pair<double, std::string> >::reverse_iterator ii = last_score.rbegin(); ii != last_score.rend() - 1 && wordLimit; ii++, wordLimit--)
        stm << "{\"keyword\":\"" << ii->second << "\",\"score\":"
            << alter(ii->first / maxn * 1000) << "},";

    s = stm.str();
    if (s.size()) s.resize(s.size() - 1);

    TT += clock();
    std::ostringstream stm1;
    stm1 << s << "]"
         << ",\"timeuse\":" << TT / CLOCKS_PER_SEC
         << ",\"info\":"    << "\"success\""
         << ",\"status\":"  << 0
         << "}";

    return stm1.str();
}

#ifdef TESTER

#include <cstdio>

int main() {
    std::string line, input;

    while ( std::getline( std::cin, line ) ) {
        input += line;
    }
    printf( "your input text is : \n     %s\n", input.c_str() );

    keyword mykeyword = keyword();
    mykeyword.init( "../res/thucke_models/", "../res/thulac_models/" );

    std::string result = mykeyword.getKeyword( input.c_str(), 10000 );
    printf( "your result is : \n    %s\n", result.c_str() );

    return 0;
}

#endif
