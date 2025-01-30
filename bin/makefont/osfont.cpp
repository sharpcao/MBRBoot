#include <iostream>
#include <fstream>
#include <string>
using namespace std;
using uint = unsigned int;
using uchar = unsigned char;

void error_exit(uint code, const string& err_msg);
uchar line_to_code(const string& s);
void display_char(const string& font_file, unsigned long idx);
void make_font(const string& font_file, const string& out_file);

int main(int argc, char* argv[])
{
	
	string font_file, out_file;
	if (argc >=2 &&  argc <=3) {
		font_file = argv[1];
		if(argc == 3) {
			string s = argv[2];
			if (s.substr(0,2)=="-d"){
				unsigned long idx = stoul(s.substr(2,s.size()-2));
				//cout << idx << endl;
				display_char(font_file, idx);
			}else{
				make_font(font_file, s);
			}

		}else{
			out_file = font_file + ".bin";
			make_font(font_file, out_file);
		}
			
	}else{
		
		cout << "usage:" <<"osfont <font file> [<out file>|-dx]" <<endl;
		cout << "* -dx: display xth char, x is idx number:0,1,2..." <<endl; 
		exit(1);
	}
	
	return 0;

}

void display_char(const string& font_file, unsigned long idx)
{
	ifstream i_file(font_file, ios::binary);
	if (!i_file.is_open()) {error_exit(2,"Cann't open font file:" + font_file);}

	i_file.seekg(idx*16,ios::beg);
	for(uint i = 0; i<16; ++i)
	{		
		unsigned char c = i_file.get();
		for(unsigned char b = 0b10000000; b>0 ; b >>=1 )
		{
			cout << ((c & b)? "*":".");
		}
		cout << endl;
	}
	i_file.close();

}

void make_font(const string& font_file, const string& out_file)
{

	ifstream i_file(font_file);
	if (!i_file.is_open()) {error_exit(2,"Cann't open font file:" + font_file);}

	ofstream o_file(out_file,ios::out|ios::binary);
	if (!o_file.is_open()) {error_exit(2,"Cann't open output file:" + out_file);}

	uint line_no = 1;
	string line;
	while(getline(i_file,line))
	{
		if (line.substr(0,4)=="char"){
			for(int i = 0; i<16; ++i)
			{
				getline(i_file,line);
				if (line.size() == 8){
					o_file.put(line_to_code(line));
				}else{
					error_exit(2, string("Font file line error :") + to_string(line_no));
				}
			}
		}
	}
	cout << "Done! file:" << out_file << endl;

	i_file.close();
	o_file.close();

}

uchar line_to_code(const string& s)
{
	uchar c = 0;
	for(int i=0; i<8; ++i)
	{
		c *= 2;
		if(s[i] == '*')
		{
			c += 1;
		}
	}
	return c;
}


void error_exit(uint code, const string& err_msg)
{
	cerr << err_msg << endl;
	exit(code);
}