dst_dir=.
src_dir=src
head_dir=head

base=g++ -O3 -std=c++0x -march=native -I $(head_dir) -lpthread -v

all: thucke 

thucke: $(src_dir)/*.cpp $(head_dir)/*/*.h
	  $(base) $(src_dir)/thucke.cpp $(src_dir)/keywordExtract.cpp $(src_dir)/articleSpliter.cpp $(src_dir)/keywordLoad.cpp $(src_dir)/thulac_segment.cpp $(src_dir)/trie.cpp -o ./thucke

clean:
	rm ./thucke

pack:
	tar -zcvf THUCKE_lite_c++_v1.tar.gz Makefile head res README.md src
