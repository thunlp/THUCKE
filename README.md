# THUCKE： 中文关键词抽取工具包


## 目录
* [项目介绍](#项目介绍)
* [编译安装](#编译安装)
* [使用方法](#使用方法)
* [与其它关键词抽取工具结果对比](#与其它关键词抽取工具结果对比)
* [THUCKE工程介绍](#THUCKE工程介绍)
* [THUCKE模型介绍](#THUCKE模型介绍)
* [开源协议](#开源协议)
* [相关论文](#相关论文)
* [作者](#作者)

## 项目介绍
  THUCKE（THU Chinese Keyphrase Extraction）由清华大学自然语言处理与社会人文计算实验室研制推出的中文关键词抽取工具包。采用WAM（Word Alignment Model）模型，引入翻译的方法，将关键词和正文作为翻译对，使用IBM-Model1进行训练，获得“关键词-正文词”翻译概率表，综合该翻译概率表和TFIDF方法，进行关键词抽取。
  训练语料采用网易新闻最近的新闻近40000条，并且我们将不断更新语料库，训练新的模型，在模型下载界面可以选择不同版本模型。

## 编译安装：
  在当前目录执行make，编译完成会在当前目录生成可执行文件thucke


## 使用方法
###1.下载模型文件（[下载地址](http://thucke.thunlp.org)）：
  将模型下载并解压到./res/，解压后目录结构应为：

    - res
      - thucke
      - thulac

###2.参数介绍：
|参数|解析| 
|:----------|:----------:|
| -i | 需要提取关键词的文本文件路径 |
| -n | 需要提取关键词的最大个数 |
| -m | 模型文件路径 |

###3.运行示例：

####3.1 准备抽取文本
  使用演示文本test1.txt或test2.txt，或者使用自己的文本替换该文件内容
####3.2.运行工具
  执行./thucke -i ./test1.txt -n 5 -m ./res
####3.3 输出结果为json格式


    {
      "result": [
        {
          "keyword": "财政",
          "score": 1000
        },
        {
          "keyword": "企业",
          "score": 875
        },
        {
          "keyword": "政策",
          "score": 625
        },
        {
          "keyword": "减税",
          "score": 541
        },
        {
          "keyword": "支出",
          "score": 374
        }
      ],
      "timeuse": 0.10199,
      "info": "success",
      "status": 0
    }

####3.4 结果内容介绍：
| key | value |
|:------------|:-------------:| 
| result | 返回结果:keyword(关键词)，score(对应评分，结果按评分排序，最高分1000)) |
| timeuse | 运行时间/s，仅为关键词抽取时间，不包括模型加载时间 |
| status | 返回状态码 |
| info | 状态码注释 |


## 与其它关键词抽取工具结果对比
我们与其它常见的关键词抽取工具（[jieba](https://github.com/fxsjy/jieba)）进行对比，采用了两篇新闻作为抽取文本，抽取的结果对比如下：

1.测试文本[test1.txt](https://github.com/thunlp/THUCKE/test1.txt)

|THUCKE|评分（最高1000）|jieba-TFIDF|评分（最高1）|jieba-TextRank|评分（最高1）
|:------------|:-------------|:-------------|:------------|:-------------|:-------------|
|耳机|1000|耳机|0.2455|耳机|1.0|
|辐射|521|蓝牙|0.2163|蓝牙|0.7364|
|人体|391|苹果|0.1688|苹果|0.7210|
|表示|376|AirPods|0.1488|表示|0.5163|
|使用|318|iPhone7|0.1339|使用|0.4901|
|iPhone7|318|辐射|0.1067|没有|0.4808|
|手机|304|Plus|0.1042|辐射|0.4750|
|AirPods|289|无线耳机|0.1042|传送|0.3215|
|电波|217|健康|0.0743|人体|0.3186|
|研究|202|传送|0.0722|问题|0.2980|

2.测试文本[test2.txt](https://github.com/thunlp/THUCKE/test2.txt)

|THUCKE|评分（最高1000）|jieba-TFIDF|评分（最高1）|jieba-TextRank|评分（最高1）
|:------------|:-------------|:-------------|:------------|:-------------|:-------------|
|财政|1000|减税|0.2445|经济|1.0|
|企业|875|财政政策|0.1313|减税|0.8912|
|政策|625|企业|0.1033|企业|0.8254|
|减税|541|税收收入|0.0968|增长|0.6250|
|支出|374|经济|0.0875|增加|0.5322|
|增长|291|数据|0.0780|扩大|0.4892|
|税收|250|降费|0.0731|税收|0.4434|
|政府|250|负担|0.07198|数据|0.4217|
|增加|250|税收|0.0698|财政|0.4134|
|扩大|208|税率|0.0693|财政政策|0.4092|


## THUCKE工程介绍

```
  - Makefile //makefile配置文件
  - README.md //文档
  - head //头文件
    - thulac //thulac分词和词性标注相关头文件
    - thucke //thucke关键词抽取相关头文件
        - articleSpliter.h //输入文本分割相关
        - keyword.h //关键词相关
        - trie.h //trie树相关
  - res //资源文件，主要是模型文件放置目录
  - src //源代码存放目录
      - thucke.cpp //主程序入口，如需进行任何形式的改造，从此文件开始
      - keywordLoad.cpp //模型加载
      - articleSpliter.cpp //输入文本切分预处理
      - thulac_segment.cpp //分词和词性标注
      - keywordExtract.cpp //关键词抽取核心算法实现 
      - trie.cpp //trie搜索树实现
  - test1.txt //测试文本1
  - test2.txt //测试文本2
```

## THUCKE模型介绍
1. THUCKE采用WAM方法，将新闻标题和新闻内容作为翻译对，生成“标题词语”-“内容词语”翻译概率矩阵，综合该词语翻译概率矩阵和tfidf，对给定文本进行关键词抽取，算法详细情况请查看“相关论文”部分；
2. 训练语料采用从网易新闻爬取的最近一段时间的新闻数据大概40000条，因此该关键词抽取模型对于新闻类文本效果较好；
3. 下载的模型除了thucke关键词抽取模型之外，还包含thulac分词词性标注模型，thulac同为thunlp推出的中文词法分析工具包，具有分词和词性标注功能；
4. THUCKE将一直不断更新训练数据，生成最新的模型，同时保留旧模型，可以根据需要进行下载。


## 开源协议
1. THUCKE面向国内外大学、研究所、企业以及个人用于研究目的免费开放源代码。
2. 如有机构或个人拟将THUCKE用于商业目的，请发邮件至thunlp@gmail.com洽谈技术许可协议。
3. 欢迎对该工具提出任何宝贵意见和建议，请发邮件至thunlp@gmail.com提出意见和建议。
4. 如果您在THUCKE基础上发表论文或取得科研成果，请您在发表论文和申报成果时声明“使用了清华大学THUCKE”，并按如下格式引用：

	* Zhiyuan Liu, Xinxiong Chen, Maosong Sun. A Simple Word Trigger Method for Social Tag Suggestion. The Conference on Empirical Methods in Natural Language Processing (EMNLP 2011).


## 相关论文
* Zhiyuan Liu, Xinxiong Chen, Maosong Sun. A Simple Word Trigger Method for Social Tag Suggestion. The Conference on Empirical Methods in Natural Language Processing (EMNLP 2011).\[[pdf](http://nlp.csai.tsinghua.edu.cn/~lzy/publications/emnlp2011.pdf)\]
* Zhiyuan Liu, Xinxiong Chen, Yabin Zheng, Maosong Sun. Automatic Keyphrase Extraction by Bridging Vocabulary Gap. The 15th Conference on Computational Natural Language Learning (CoNLL 2011).\[[pdf](http://nlp.csai.tsinghua.edu.cn/~lzy/publications/conll2011.pdf)\]


## 作者
* Zhipeng Guo（郭志芃，本科生）, Yunshan Ma（马云山，本科生）, Zhiyuan Liu（刘知远，助理教授）, Maosong Sun（孙茂松，教授）
