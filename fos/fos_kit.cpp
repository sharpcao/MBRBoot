
#include <iostream>
#include <fstream>
#include <memory>
#include "fos_kit.h"



using std::ifstream;
using std::fstream;


uint Meta::get_free_file_idx() const
{
	FMeta file_meta = get_meta(Meta::C_FILES);
	ifstream mfile(_meta_file, std::ios::binary);
	mfile.seekg(file_meta.start, std::ios::beg);
	
	for(uint i = 0; i < file_meta.length / sizeof(FFile); ++i)
	{
		FFile file_record;
		mfile.read((char*)&file_record, sizeof(file_record));
		if((file_record.attr == FFile::Attr::empty) || (file_record.attr == FFile::Attr::del))
		{
			return i;
		}
	}
	throw std::runtime_error(string("No aveilable free file record"));

}

void Meta::update_file_record(uint file_idx, const FFile& file_record)
{
	FMeta file_meta = get_meta(Meta::C_FILES);
	fstream mfile(_meta_file, std::ios::binary|std::ios::in|std::ios::out);
	mfile.seekg(file_meta.start + file_idx * sizeof(FFile), std::ios::beg);
	mfile.write((char*)& file_record, sizeof(file_record));
}

uint Meta::get_file_idx(const string& filename) const
{
	FMeta file_meta = get_meta(Meta::C_FILES);
	ifstream mfile(_meta_file, std::ios::binary);
	mfile.seekg(file_meta.start, std::ios::beg);
	
	for (uint i = 0; i < file_meta.length / sizeof(FFile);++i)
	{
		FFile file_record;
		mfile.read((char*)&file_record, sizeof(file_record));
		if(file_record.attr != FFile::Attr::empty) {
			if (filename ==string(file_record.name)) return i;
		}else{
			break;
		}
	}
	throw std::runtime_error(string("Can't find file:") + filename);

}

void Meta::del_file_record(uint file_idx)
{
	FMeta file_meta = get_meta(Meta::C_FILES);
	fstream mfile(_meta_file, std::ios::binary | std::ios::in| std::ios::out);
	FFile del_record = get_file(file_idx);
	del_record.attr = FFile::Attr::del;
	del_record.name[0] = 0x23; // 0x23 == #
	del_record.start_sect = 0xFFFFFFFF;
	del_record.file_size = 0;
	mfile.seekg(file_meta.start + file_idx * sizeof(FFile), std::ios::beg);
	mfile.write((char*)&del_record, sizeof(del_record));
}

void Meta::del_file_map(uint file_idx)
{
	FFile file_record = get_file(file_idx);
	FMeta map_meta = get_meta(Meta::C_FMAP);
	fstream mfile(_meta_file,std::ios::binary|std::ios::in|std::ios::out);
	ulong s = file_record.start_sect;
	ulong s_next=0, s_empty = 0;
	do {
		mfile.seekg(map_meta.start + s * sizeof(ulong), std::ios::beg);
		mfile.read((char*)&s_next,sizeof(s_next));
		mfile.seekg(map_meta.start + s * sizeof(ulong), std::ios::beg);
		mfile.write((char*)&s_empty,sizeof(s_empty));
		s = s_next;
	
	} while(s!= 0xFFFFFFFF);

}
void Meta::list_file_map(const string& filename) const
{
	ifstream afile(_meta_file,std::ios::binary);
	FMeta map_meta = get_meta(Meta::C_FMAP);
	FFile file_record = get_file(filename);
	ulong offset = file_record.start_sect;
	
	while(1){
		std::cout <<std::hex  << "[" << map_meta.start + offset * sizeof(ulong) << "]";
		afile.seekg(map_meta.start + offset * sizeof(ulong), std::ios::beg);
		afile.read((char*)&offset, sizeof(ulong));
		std::cout <<"=" << offset << "\n";
		if(offset == 0xFFFFFFFF) break;
	}
	std::cout << std::endl;

}

void Meta::print_file(const FFile& file_record) const
{
	ifstream afile(_meta_file,std::ios::binary);
	FMeta map_meta = get_meta(Meta::C_FMAP);
	FMeta data_meta = get_meta(Meta::C_FDATA);
	vector<ulong> sectors;
	ulong offset = file_record.start_sect;

	sectors.push_back(offset);
	while(1){

		afile.seekg(map_meta.start + offset * sizeof(ulong), std::ios::beg);
		
		afile.read((char*)&offset, sizeof(ulong));

		if(offset == 0xFFFFFFFF){
			break;
		}else{
			sectors.push_back(offset);
			
		}
	}
	for(const auto i: sectors){
		char buf[1024];
		afile.seekg(data_meta.start + i * 1024, std::ios::beg);
		afile.read((char*)buf, 1024);
		std::cout << buf;

	}
	std::cout << std::endl;

}
void Meta::add_file(const string& filename)
{

	FMeta files_meta = get_meta(Meta::C_FILES);
	FMeta map_meta = get_meta(Meta::C_FMAP);
	FMeta data_meta = get_meta(Meta::C_FDATA);
	
	ifstream afile(filename,std::ios::binary|std::ios::ate);
	if(!afile) throw std::runtime_error(string("Can't open file ") + filename); 

	uint file_size = afile.tellg();
	afile.seekg(0, std::ios::beg);
	char buf[1024];

	std::cout << "[Read] " << filename << " " << file_size << std::endl; 

	fstream mfile(_meta_file,std::ios::binary|std::ios::in|std::ios::out);
	mfile.seekg(map_meta.start, std::ios::beg);

	std::unique_ptr<ulong[]> map_data = std::make_unique<ulong[]>(map_meta.length / sizeof(ulong));

	mfile.read((char*)map_data.get(), map_meta.length); 


	//update FDATA 
	uint cur = 0, pre = 0, first = 0;
	auto find_free_map = [&map_data, &map_meta](uint idx = 0)-> uint {
		while(map_data[idx])
		{
			
			if(++idx >= map_meta.length / sizeof(ulong)) throw std::runtime_error("No space available!");
		}
		return idx;
		
	};

	cur = first = find_free_map();

	for(uint remain_size = file_size; remain_size > 0; )
	{
		for(uint j = 0; j < 1024; ++j) buf[j] = 0;

		cur = find_free_map(cur);

		uint read_size = (remain_size>1024)? 1024 : remain_size;
		
		afile.read((char*)buf,  read_size);
		
		mfile.seekg(data_meta.start + cur * 1024, std::ios::beg);
		mfile.write(buf, 1024);
		std::cout << "Write chunk " << cur << " " << read_size << " byte" << std::endl;

		if(pre != cur){
			map_data[pre] = cur;		
			pre = cur;
		}
		map_data[cur] = 0xFFFFFFFF;

		remain_size = (remain_size>1024)? remain_size - 1024 : 0;

	}


	//update FMAP
	mfile.seekg(map_meta.start, std::ios::beg);
	mfile.write((char*)map_data.get(), map_meta.length);
	mfile.close();
	

	//update FILES Record

	FFile file_record;

	string basename = filename;
	size_t pos = filename.find_last_of("/\\");
	if(pos != string::npos) basename = filename.substr(pos+1);

	uint name_size = (basename.size() > 15)? 15 : basename.size();
	for(uint j = 0 ; j < name_size; ++j) file_record.name[j] = basename[j];

	file_record.attr = FFile::Attr::file;
	file_record.parent = 0;
	file_record.start_sect = first;
	file_record.file_size = file_size;
	uint idx = get_free_file_idx();
	update_file_record(idx, file_record);
	
}


void Meta::del_file(const string& filename)
{
	auto file_idx = get_file_idx(filename);
	del_file_map(file_idx);
	del_file_record(file_idx);

}

const FMeta Meta::get_meta(const string& key) const
{
	for(const auto& i : _vec)
	{
		if( string(i.name) == key) return i;
	}
	return FMeta();
}

FFile Meta::get_file(uint file_idx) const
{
	const FMeta files_meta = get_meta(C_FILES);
	ifstream mfile(_meta_file);
	
	FFile result;
	mfile.seekg(files_meta.start + file_idx * sizeof(FFile), std::ios::beg);
	mfile.read((char*)& result, sizeof(FFile));
	return result;

}

FFile Meta::get_file(const string& fn) const
{
	const FMeta files_meta = get_meta(C_FILES);
	//const FMeta meta = get_meta(C_META);

	ifstream mfile(_meta_file);

	for(uint i = 0; i < files_meta.length / sizeof(FFile); ++i )
	{
		FFile tmp;
		mfile.seekg(files_meta.start + i * sizeof(FFile), std::ios::beg);
		mfile.read((char*)&tmp, sizeof(FFile));
	
		if (string(tmp.name) == fn) {
			return tmp;
		}else if(tmp.attr == 0){
			break;
		}
	}
	throw std::runtime_error(string("Can't find file : ") + fn);
}


void Meta::list_file() const
{
	FMeta file_meta = get_meta(Meta::C_FILES);
			
	uint n_max = file_meta.length / sizeof(FFile);
	for(uint i = 0; i < n_max; ++i)
	{
		FFile record = get_file(i);
		string name = record.name;
		if (name.empty()) break;
		std::cout << i <<": " << record.name << " : start=0x" << std::hex << record.start_sect 
					<< " size=" << std::dec << record.file_size << " attr=" << record.attr << std::endl;
	}
}

Meta::Meta(string sfile)
{
	_meta_file = sfile;
	ifstream afile(_meta_file, std::ios::binary);
	if(!afile) {
		throw std::runtime_error(string("can't open file ") + sfile);
	};

	afile.seekg(512,std::ios::beg);
	FMeta m_record;
	for(int i = 0; i < 512 / sizeof(FMeta);++i)
	{
		afile.read((char*)&m_record,sizeof(m_record));
		std::string s = m_record.name;
		if(!s.empty()){
			_vec.push_back(m_record);
			
		}else{
			break;
		}
	}

}

void Meta::make_image(const string& filename) 
{
	ofstream afile(filename, std::ios::binary );
	if(!afile) throw std::runtime_error(string("Can't create file ") + filename);

	char buf[512];
	auto clear_buf = [&buf] () {
		for(uint i = 0; i<512; ++i) buf[i] = 0;
	};
	
	clear_buf();
	afile.write(buf, 512);
	afile.write((char*)DISK_META, sizeof(DISK_META));
	afile.write(buf, 512 - sizeof(DISK_META));

	for(uint i = 0; i < 1440 * 2 - 2 ; ++i){
		afile.write(buf, 512);
	}



}



