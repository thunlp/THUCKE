#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <getopt.h>
#include <sys/types.h>
#include <dirent.h>
#include "thucke/keyword.h"

int is_dir_exist(const char *);
std::string join_path(const char *, const char *);

int main( int argc, char **argv ) {
    int cnt = 10;
    char * keywords_cnt;
    char * input_path = "stdin";
    char * model_path = "../res/models/";

    struct option longopts[] = {
        { "cnt",        required_argument,    NULL,    'n'},
        { "input_path", required_argument,    NULL,    'i'},
        { "model",      required_argument,    NULL,    'm'},
        { 0,            0,                    0,        0 },
    };

    int c, ec;
    while( (c = getopt_long(argc, argv, ":n:i:m:", longopts, NULL)) != -1 ) {
        switch (c){
            case 'n':
                keywords_cnt = optarg;
                cnt = std::atoi(keywords_cnt);
                break;
            case 'i':
                input_path = optarg;
                break;
            case 'm':
                model_path = optarg;
                break;
            case '?':
                ec = (char)optopt;
                printf("无效的选项字符 \' %c \'!\n", ec);
                break;
            case ':':
                printf("缺少选项参数！\n");
                break;
        }
    }

    //read input text(from file or stdin)
    std::string line, input_text;
    if(0 == std::strcmp(input_path, "stdin")) {
        while(std::getline(std::cin, line)) {
            input_text += line;
        }
    }
    else {
         std::ifstream input_file(input_path);
         if(! input_file) {
             printf("invalid input_path\n");
         }
         while(std::getline(input_file, line)) {
             input_text += line;
         }
    }

    //start to extract keywords
    keyword mykeyword = keyword();
    std::string model_path_thulac, model_path_thucke;

    try {
        model_path_thulac = join_path(model_path, "thulac_models");
    }
    catch (std::string){
        printf("invalid model_path: %s\n", model_path);
    }
    try {
        model_path_thucke = join_path(model_path, "thucke_models");
    }
    catch (std::string){
        printf("invalid model_path: %s\n", model_path);
    }

    mykeyword.init(model_path_thulac, model_path_thucke);

    std::string result = mykeyword.getKeyword(input_text, cnt);
    printf("%s", result.c_str());

    return 0;
}

int is_dir_exist(const char * dir_path) {
    if (dir_path == NULL) {
        return -1;
    }
    if (opendir(dir_path) == NULL) {
        return -1;
    }
    return 0;
}

std::string join_path(const char * pre, const char * suf) {
    std::string prefix = pre;
    std::string suffix = suf;
    if (0 != is_dir_exist(prefix.c_str())) {
        throw prefix;
    }
    if (prefix.substr(prefix.length() - 1, 1).compare("/") != 0) {
        prefix = prefix + "/";
    }
    return prefix + suffix + "/";
}
