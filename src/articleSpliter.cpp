#include "thucke/articleSpliter.h"

const int articleSpliter::strategyLong = 1, articleSpliter::strategyShort = 2;
const int articleSpliter::strategyStop = 1, articleSpliter::strategyContinue = 0, articleSpliter::strategyAdd = 2, articleSpliter::strategyIgnore = 0, articleSpliter::strategyNewline = 4;
const int articleSpliter::hanCharacter = 0, articleSpliter::unknowCharacter = 1, articleSpliter::newlineCharacter = -1;

articleSpliter::articleSpliter(const std::string& input,
               int singleSentenceLimit,
               int strategyID) {

    article = input;
    position = 0;
    paragraphNum = 1;

    limit = singleSentenceLimit;
    switch (strategyID) {
        case strategyShort:
            strategy = &articleSpliter::strategyFuncShort;
            break;

        default:
            strategy = &articleSpliter::strategyFuncLong;
            break;
    }
}

std::pair<std::string, int> articleSpliter::nextSplit() {
    std::pair< std::string, int > sentence;
    sentence.second = paragraphNum;
    while (position < article.size()) {
        std::string character = nextCharacter();
        int task = (this->*strategy)(sentence.first, character);
        if (task & strategyAdd)
            sentence.first += character;
        if (task & strategyNewline)
            paragraphNum++;
        if (task & strategyStop)
            break;
    }
    return sentence;
}

bool articleSpliter::hasMoreSplit() {
    bool flag = 0;
    while (position != article.size()) {
        std::string character = nextCharacter();
        int characterType = detectCharacter(character);
        if (!flag && characterType == newlineCharacter) {
            flag = 1;
            paragraphNum++;
        }
        if (characterType == hanCharacter) {
            position -= character.size();
            break;
        }
    }
    return position != article.size();
}

int articleSpliter::strategyFuncLong(const std::string& sentence, const std::string& character) {
    int characterType = detectCharacter(character);
    switch (characterType) {
        case unknowCharacter:
            if (limit && sentence.size() + 500 > limit)
                return strategyIgnore | strategyStop;
            else
                return strategyIgnore | strategyContinue;
            break;
        case newlineCharacter:
            return strategyIgnore | (sentence.size() ? strategyStop : strategyContinue) | strategyNewline;
            break;
        default:
            if (limit && sentence.size() + 10 > limit)
                return strategyAdd | strategyStop;
            else
                return strategyAdd | strategyContinue;
            break;
    }
}

int articleSpliter::strategyFuncShort(const std::string& sentence, const std::string& character) {
    int characterType = detectCharacter(character);
    switch (characterType) {
        case unknowCharacter:
            return strategyIgnore | (sentence.size() ? strategyStop : strategyContinue);
            break;
        case newlineCharacter:
            return strategyIgnore | (sentence.size() ? strategyStop : strategyContinue) | strategyNewline;
            break;
        default:
            if (limit && sentence.size() + 10 > limit)
                return strategyAdd | strategyStop;
            else
                return strategyAdd | strategyContinue;
            break;
    }
}

std::string articleSpliter::nextCharacter() {
    std::string character;
    unsigned char ch = (unsigned char) article[position];
    if (ch < 128)
        return character += article[position++];
    for (unsigned char base = 128; (position != article.size()) && (ch & base); base >>= 1)
        character += article[position++];
    return character;
}

int articleSpliter::detectCharacter(const std::string &character) {

    unsigned char ch = (unsigned char)character[0];
    if (ch < 128) {
        if (ch == '\n')
            return newlineCharacter;
        else
        //    return unknowCharacter;
            return hanCharacter;
    }

    unsigned char base = 64;
    int unicode = 0, pp = 0;
    for (; (pp + 1 != character.size()) && (ch & base); base >>= 1)
        unicode = (unicode << 6) | ((unsigned char)character[++pp] & (unsigned char)63);

    unicode |= (ch & (base - 1)) << (pp * 6);
    if (19968 <= unicode && unicode <= 40907)
        return hanCharacter;
    else
        return unknowCharacter;
}


#ifdef TESTER

#include <cstdio>

char input[10000000];

int main() {
    fread(input, 1, 10000000, stdin);
    articleSpliter spliter(std::string(input), 1024, articleSpliter::strategyShort);
    while (spliter.hasMoreSplit()) {
        std::pair< std::string, int > now = spliter.nextSplit();
        printf("%s %d\n", now.first.c_str(), now.second);
    }
    return 0;
}

#endif
