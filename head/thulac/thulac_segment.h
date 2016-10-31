#include "timeword.h"
#include "verbword.h"
#include "negword.h"
using namespace thulac;

class CThulacSegment{

	Character separator;
	bool useT2S;
	bool seg_only;
	bool useFilter;
	std::string prefix;

	/*
	permm::Model* cws_model;
	DAT* cws_dat;
	char** cws_label_info;
	int** cws_pocs_to_tags;
	*/

	permm::Model* tagging_model;
	DAT* tagging_dat;
	char** tagging_label_info;
	int** tagging_pocs_to_tags;

	Preprocesser* preprocesser;
	Postprocesser* ns_dict;
	Postprocesser* user_dict;
	Postprocesser* idiom_dict;
	Punctuation* punctuation;
	NegWord* negword;
	TimeWord* timeword;
	VerbWord* verbword;
	Filter* filter;

public:
	CThulacSegment():
		separator ('_'),
		useT2S (false),
		seg_only (false),
		useFilter (false),
		/*
		cws_model (NULL),
		cws_dat (NULL),
		cws_label_info (NULL),
		cws_pocs_to_tags (NULL),
		*/
		tagging_model (NULL),
		tagging_dat (NULL),
		tagging_label_info (NULL),
		tagging_pocs_to_tags (NULL),
		preprocesser (NULL),
		ns_dict (NULL),
		idiom_dict (NULL),
		timeword (NULL),
		negword (NULL),
		verbword (NULL),
		punctuation (NULL),
		filter(NULL){}

	~CThulacSegment();

	bool Init(std::string inPath);

	void setT2S(bool needT2S);

	void setDelimiter(char ch);

	void setOnlySeg(bool onlySeg);

	void setUseFilter(bool usefilter);

	bool addUserDict(std::string inPath, std::string inTag);

	std::string segment(std::string inText);

	void get_pocs_to_tags(const char*);
};
