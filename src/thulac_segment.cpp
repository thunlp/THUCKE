#include <list>
#include <sstream>
#include <iostream>
#include "thulac/thulac_base.h"
#include "thulac/preprocess.h"
#include "thulac/postprocess.h"
#include "thulac/punctuation.h"
#include "thulac/filter.h"
#include "thulac/cb_tagging_decoder.h"
#include "thulac/thulac_segment.h"
#include "thulac/timeword.h"
#include "thulac/verbword.h"
#include "thulac/negword.h"
#include "thulac/punctuation.h"
#include "thulac/filter.h"

void CThulacSegment::get_pocs_to_tags(const char* label_file="label.txt")
{
    std::list<int> poc_tags[16];
    char* str=new char[16];
    FILE *fp;
    fp = fopen(label_file, "r");
    int ind=0;

    while( fscanf(fp, "%s", str)==1){
        tagging_label_info[ind]=str;
        int seg_ind=str[0]-'0';
        for(int j=0;j<16;j++){
            if((1<<seg_ind)&(j)){
                poc_tags[j].push_back(ind);
            }
        }
        str=new char[16];
        ind++;
    }

    delete[]str;
    fclose(fp);

    /*pocs_to_tags*///pocs_to_tags是用于什么的呢？
    //tagging_pocs_to_tags =new int*[16];
    for(int j=1;j<16;j++){
        tagging_pocs_to_tags[j]=new int[(int)poc_tags[j].size()+1];
        int k=0;
        for(std::list<int>::iterator plist = poc_tags[j].begin();
                plist != poc_tags[j].end(); plist++){
            tagging_pocs_to_tags[j][k++]=*plist;
        };
        tagging_pocs_to_tags[j][k]=-1;
    }
}


bool CThulacSegment::Init(std::string inPath)
{

    prefix = inPath;

    if (prefix.length() == 0) return false;
    if (*prefix.rbegin() != '/')
    {
        prefix += "/";
    }
    preprocesser = new Preprocesser();
    ns_dict = new Postprocesser((prefix+"ns.dat").c_str(), "ns", false);
    idiom_dict = new Postprocesser((prefix+"idiom.dat").c_str(), "i", false);

    punctuation = new Punctuation((prefix+"singlepun.dat").c_str());

    negword = new NegWord((prefix+"neg.dat").c_str());
    timeword = new TimeWord();
    verbword = new VerbWord((prefix+"vM.dat").c_str(), (prefix+"vD.dat").c_str());

    filter = NULL;
     if(useFilter){
        filter = new Filter((prefix+"xu.dat").c_str(), (prefix+"time.dat").c_str());
    }

    tagging_model = new permm::Model((prefix + "model_c_model.bin").c_str());
    tagging_dat = new DAT((prefix + "model_c_dat.bin").c_str());
    tagging_pocs_to_tags = new int*[16];
    tagging_label_info=new char*[tagging_model->l_size];
    get_pocs_to_tags((prefix + "model_c_label.txt").c_str());
    return true;
}

void CThulacSegment::setT2S(bool needT2S)
{
    useT2S = needT2S;
}

void CThulacSegment::setDelimiter(char ch)
{
    separator = (int)ch;
}

void CThulacSegment::setOnlySeg(bool onlySeg)
{
    seg_only = onlySeg;
}

void CThulacSegment::setUseFilter(bool usefilter)
{
    useFilter = usefilter;
}

bool CThulacSegment::addUserDict(std::string inPath, std::string inTag)
{

    user_dict = new Postprocesser(inPath.c_str(), inTag, true);

    return true;
}

std::string CThulacSegment::segment(std::string inText)
{
    TaggingDecoder* tagging_decoder=new TaggingDecoder();

    tagging_decoder->init(tagging_model, tagging_dat, tagging_pocs_to_tags, tagging_label_info);

    tagging_decoder->set_label_trans();

    if (seg_only)
    {
        tagging_decoder->threshold = 0;
    }
    else
    {
        tagging_decoder->threshold = 15000;
    }
    POCGraph poc_cands;
    thulac::RawSentence raw;
    thulac::RawSentence oriRaw;
    thulac::TaggedSentence tagged;

    size_t inlen = inText.length();

    thulac::get_raw(oriRaw, inText.c_str(), inlen);

    std::ostringstream ost;
    ost.str("");
    preprocesser->clean(oriRaw, raw, poc_cands);


    if(raw.size()){
        tagging_decoder->segment(raw,poc_cands,tagged);
        //后处理
        ns_dict->adjust(tagged);
        idiom_dict->adjust(tagged);

        if(user_dict){
            user_dict->adjust(tagged);
        }

        punctuation->adjust(tagged);

        timeword->adjustDouble(tagged);
        negword->adjust(tagged);
        verbword->adjust(tagged);


        if(useFilter){
            filter->adjust(tagged);
        }

        //  输出
        ost <<tagged;//输出

    }

	poc_cands.clear();
	raw.clear();
	oriRaw.clear();
	tagged.clear();
	delete tagging_decoder;

    return ost.str();
}

CThulacSegment::~CThulacSegment(){
	delete preprocesser;
	delete ns_dict;
	delete idiom_dict;
	if(user_dict != NULL){
		delete user_dict;
	}

	delete negword;
	delete timeword;
	delete verbword;
	delete punctuation;
	if(filter != NULL){
		delete filter;
	}

	if(tagging_model != NULL){
		for(int i = 0; i < tagging_model->l_size; i ++){
			if(tagging_label_info) delete[] (tagging_label_info[i]);
		}
	}
	delete[] tagging_label_info;

	if(tagging_pocs_to_tags){
		for(int i = 1; i < 16; i ++){
			delete[] tagging_pocs_to_tags[i];
		}
	}
	delete[] tagging_pocs_to_tags;

	delete tagging_dat;
	if(tagging_model != NULL) delete tagging_model;
}

static CThulacSegment segment;

bool CThulacSegment_Init(std::string inPath)
{
    return segment.Init(inPath);
}

std::string CThulacSegment_segment(std::string inText)
{
    return segment.segment(inText);
}

#ifdef TESTER

int main(){
	CThulacSegment_Init("../res/thulac_models/");
	std::cout<<CThulacSegment_segment("我爱北京天安门")<<std::endl;
	return 0;
}

#endif
