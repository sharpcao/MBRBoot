/* 
    useage: osfont <font file> <options>
                -b <bin file>       : output bin file
                -c:xxx <cpp file>  : output C++ file with variable name xxx
                -d <index>          : display index char

    Author:Sharp.Cao
    Date:2025/1/31

*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;
using uint = unsigned int;
using uchar = unsigned char;
enum filetype {
    bin,
    cpp
};

void error_exit(uint code, const string& err_msg);
uchar line_to_code(const string& s);
void display_char(const string& font_file, unsigned long idx);
void make_file(const string& font_file, const string& out_file, filetype tp, const string& data_name = "font_data");
void print_usage();

int main(int argc, char* argv[])
{

    if (argc == 4 ) {       
        string font_file = argv[1];
        string options = argv[2];
        if (options == "-b"){
            string out_file = argv[3];
            make_file(font_file,out_file, filetype::bin);
        }else if (options.size() >3 && options.substr(0,3) =="-c:"){
            string out_file = argv[3];      
            make_file(font_file,out_file, filetype::cpp, options.substr(3,options.size()-3));

        }else if(options =="-d"){
            unsigned long idx = stoul(argv[3]);
            display_char(font_file, idx);           
        }else{
            print_usage();
            exit(1);
        }
            
    }else{
        
        print_usage();
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

string char2hex(uint c)
{
    stringstream ss;
    ss << "0x" << std::hex << std::setfill('0') << std::setw(2) <<  c;
    return ss.str();
}

void make_file(const string& font_file, const string& out_file, filetype tp, const string& data_name)
{

    ifstream i_file(font_file);
    if (!i_file.is_open()) {error_exit(2,"Cann't open font file:" + font_file);}

    ofstream o_file;
    if (tp == filetype::bin)
    {
        o_file.open(out_file,ios::out|ios::binary);
        if (!o_file.is_open()) {error_exit(2,"Cann't open output file:" + out_file);}

    }else if (tp==filetype::cpp)
    {
        o_file.open(out_file);
        if (!o_file.is_open()) {error_exit(2,"Cann't open output file:" + out_file);}
        o_file << "unsigned char " << data_name << "[] = {" << endl;
    }else
    {
        error_exit(2,"Not support this type");
    }

    

    uint line_no = 1;
    string line;
    while(getline(i_file,line))
    {
        if (line.substr(0,4)=="char"){           
            for(int i = 0; i<16; ++i)
            {
                getline(i_file,line);
                if (line.size() == 8){
                    uchar c = line_to_code(line);
                    if (tp==filetype::bin){
                       o_file.put(c); 
                    }else{
                        o_file << char2hex((uint)c) <<"," ;
                   }
                    
                }else{
                    error_exit(2, string("Font file line error :") + to_string(line_no));
                }
            }
            if(tp==filetype::cpp) {o_file << "\n" ;}
        }
    }

    if (tp==filetype::cpp){ o_file << "};" <<endl; }

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

void print_usage()
{
    cout << "useage: osfont <font file> <options>\n" 
         << "\t-b <bin file>  : output bin file\n "
         << "\t-c:xxx <cpp file>  : output cpp hex file,xxx is the data name\n"
         << "\t-d <xth>       : display xth char, x is idx number:0,1,2... \n"
         << endl;
}