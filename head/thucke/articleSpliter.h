#ifndef __articleSpliter_h__
#define __articleSpliter_h__

#include <string>

class articleSpliter {

public:
    static const int strategyLong, strategyShort;
    
    articleSpliter(const std::string& input,
                   int singleSentenceLimit = 10240,
                   int strategyID = strategyLong);
    //singleSentenceLimit is recommended to be at least 1024
    
    std::pair<std::string, int> nextSplit();
    //sentence with only chinese characters || paragraph number
    
    bool hasMoreSplit();
    
private:
    typedef int (articleSpliter::*strategyPtr)(const std::string&, const std::string&);
    
    static const int strategyStop, strategyContinue, strategyAdd, strategyIgnore, strategyNewline;
    static const int hanCharacter, unknowCharacter, newlineCharacter;
    
    int paragraphNum;
    std::string article;
    int position, limit;
    strategyPtr strategy;
    
    
    articleSpliter();
    articleSpliter(const articleSpliter&);
    int strategyFuncLong(const std::string&, const std::string&);
    
    int strategyFuncShort(const std::string&, const std::string&);
    
    std::string nextCharacter();
    
    static int detectCharacter(const std::string&);
};

#endif
