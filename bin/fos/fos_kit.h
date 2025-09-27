#ifndef FOSKIT_H
#define FOSKIT_H

#include <vector>
#include <string>
#include <map>

using std::vector;
using std::string;
using std::pair;

using ulong = unsigned long;
using uint = unsigned int;

struct FFile{

	enum Attr {
		empty = 0, del = 1, dir = 2, file = 3,
	};

	char name[16] = "";
	Attr attr = Attr::empty;
	ulong parent = 0;
	ulong start_sect = 0;
	ulong file_size = 0;
};




struct FMap{
	enum Status{
		empty = 0, file_end = 0xFFFFFFFF
	};
	Status next = Status::empty;
};

struct FMeta{
	char name[8] = "";
	ulong start = 0;
	ulong length = 0;
	FMeta () = default;
	FMeta(string meta_name, ulong meta_start, ulong meta_length) : start(meta_start) , length(meta_length)
	{
		uint name_size = (meta_name.size() <7) ? meta_name.size() : 7;
		for(uint i = 0; i<name_size; ++i) name[i] = meta_name[i];
	}
	
};

class Meta{
	vector<FMeta> _vec;
	string _meta_file;

	uint get_file_idx(const string& filename) const;
	uint get_free_file_idx() const;
	void del_file_record(uint file_idx);
	void update_file_record(uint file_idx, const FFile& file_record);
	void del_file_map(uint file_idx);
	
public:
	static const char *C_MBR, *C_META, *C_FILES, *C_FMAP, *C_FDATA;
	const FMeta get_meta(const string& key) const;

	Meta(string sfile);

	const vector<FMeta>& get() const { return _vec;}
	FFile get_file(uint file_idx) const;
	FFile get_file(const string& fn) const;
	void list_file() const;
	void list_file_map(const string& filename) const;
	void add_file(const string& filename);
	void del_file(const string& filename);
	void print_file(const FFile& file_record) const;
	void print_space() const;
	static void make_image(const string& filename, uint file_size = 1440);
	static void format(const string& filename);

	
};

const char* Meta::C_MBR="MBR0";
const char* Meta::C_META="META";
const char* Meta::C_FILES="FILE";
const char* Meta::C_FMAP="FMAP";
const char* Meta::C_FDATA="FDAT";




#endif