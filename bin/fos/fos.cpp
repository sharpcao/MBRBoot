#include <iostream>
#include "fos_kit.h"


void print_help()
{
std::cout <<"fos 	[file_image] <command> \n" 
	  <<"	command:\n"
	  <<"	<makeimage> [image_size (kb)]\n"
	  <<"	<format | printspace>\n"
	  <<"	<listmeta | listfile>\n"
	  <<"	<listfilemap | printfile | addfile | delfile> [file_name]"
	  << std::endl; 
}

int main(int argc, char* argv[])
{
	
	try{
		
		if (argc == 3 || argc ==4)	
		{
			string s_file(argv[1]);
			string s_cmd(argv[2]);

			if(s_cmd == "makeimage" && argc == 4 ){
		
				uint file_kb = std::stoi(argv[3]);
				Meta::make_image(s_file,file_kb);
				return 0;
			
			}
			Meta meta(s_file);
			
			if( s_cmd == "listmeta"){
				
				for(const auto& i:meta.get()){
					std::cout <<std::hex << i.name <<": 0x" << i.start << ", 0x"  << i.length << std::endl;
				}
			}else if(s_cmd == "printspace" && argc == 3 ){
				meta.print_space();

			}else if(s_cmd == "listfile" && argc == 3){
				meta.list_file();
			}else if (s_cmd == "addfile" && argc == 4){
				string afile = argv[3];
				meta.add_file(afile);

			}else if(s_cmd == "printfile" && argc == 4){
				string afile = argv[3];
				FFile rec = meta.get_file(afile);
				meta.print_file(rec);
			}else if (s_cmd == "delfile" && argc == 4){
				string afile = argv[3];
				meta.del_file(afile);
			}else if (s_cmd =="listfilemap" && argc == 4){
				meta.list_file_map(string(argv[3]));
			}else if (s_cmd == "format"){
				meta.format(s_file);

			}else{
				print_help();
			}
		}else{
			print_help();
		}
	}catch(const std::runtime_error& e){
		std::cerr << e.what() << std::endl;
	}

}

