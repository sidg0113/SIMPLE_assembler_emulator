#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <ciso646>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>


#include <ccomplex>
#include <cfenv>
#include <cinttypes>
#include <cstdbool>
#include <cstdint>
#include <ctgmath>
#include <cwchar>
#include <cwctype>

// C++
#include <algorithm>
#include <bitset>
#include <complex>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <typeinfo>
#include <utility>
#include <valarray>
#include <vector>
#include <fstream>
#include <iomanip>
using namespace std;

// removing spaces and useless lines
const string USELESS = " \n\r\t\f\v";
string right_del(const string& s);
string left_del(const string& s);
string del(const string& s);
bool prev_present(string s);
void pass1(string file_name, ofstream& log_file);
void pass2(string file, ofstream& outfile, ofstream& logfile, ofstream& objfile);
string insert_in_table(string s,int pnt,int cnt);
bool valid_label(string s);

map<string, string> mnem;
map < string,int > instructions_without_operand;
map < string,int > instructions_with_one_operand;
map < string,int > instructions_with_pcoffset;
map <string,int> special_cases;
struct labels{
    string name;
    int address;
    bool if_set;
    int set_pc;
    // bool if_data;
    // int set_data;
};
vector<labels> table;
int main(int argc, char** argv){
    // cout<<"!";
	mnem["ldc"] = string("00");
	mnem["adc"] = string("01");
	mnem["ldl"] = string("02");
	mnem["stl"] = string("03");
	mnem["ldnl"] = string("04");
	mnem["stnl"] = string("05");
	mnem["add"] = string("06");
	mnem["sub"] = string("07");
	mnem["shl"] = string("08");
	mnem["shr"] = string("09");
	mnem["adj"] = string("0a");
	mnem["a2sp"] = string("0b");
	mnem["sp2a"] = string("0c");
	mnem["call"] = string("0d");
	mnem["return"] = string("0e");
	mnem["brz"] = string("0f");
	mnem["brlz"] = string("10");
	mnem["br"] = string("11");
	mnem["HALT"] = string("12");
	mnem["data"] = string("13");
	mnem["SET"] = string("14");

    instructions_without_operand["add"]=6;
	instructions_without_operand["sub"]=7;
	instructions_without_operand["shl"]=8;
	instructions_without_operand["shr"]=9;
	instructions_without_operand["a2sp"]=11;
	instructions_without_operand["sp2a"]=12;
	instructions_without_operand["return"]=14;
	instructions_without_operand["HALT"]=18;

	instructions_with_one_operand["ldc"]=0;
	instructions_with_one_operand["adc"]=1;
	instructions_with_one_operand["ldl"]=2;
	instructions_with_one_operand["stl"]=3;
	instructions_with_one_operand["ldnl"]=4;
	instructions_with_one_operand["stnl"]=5;
	instructions_with_one_operand["adj"]=10;
	instructions_with_one_operand["call"]=13;
	instructions_with_one_operand["brz"]=15;
	instructions_with_one_operand["brlz"]=16;
	instructions_with_one_operand["br"]=17;
    instructions_with_one_operand["SET"]=19;
    instructions_with_one_operand["data"]=20;
	special_cases["data"]=19;
	special_cases["SET"]=20;

	instructions_with_pcoffset["call"]=13;
	instructions_with_pcoffset["brz"]=15;
	instructions_with_pcoffset["brlz"]=16;
	instructions_with_pcoffset["br"]=17;
    if(argc != 2) // if input file is not provided throw error
	{
		cout << "NO INPUT FILE SPECIFIED, CHECK INPUT : ./asm filename.asm";
	}

    string input_file=argv[1];


	string out_file = input_file.substr(0, input_file.find(".", 0)) + ".L";

	string log_file = input_file.substr(0, input_file.find(".", 0)) + ".log";

	string obj_file = input_file.substr(0, input_file.find(".", 0)) + ".o";

	
    ofstream outfile(out_file);
    ofstream logfile(log_file);
    ofstream objfile(obj_file,ios::out | ios::binary);
    pass1(input_file,logfile);
    // for(int i=0;i<table.size();i++){
    //     cout<<table[i].name<<endl;
    // }
    pass2(input_file,outfile,logfile,objfile);
    outfile.close();
    logfile.close();
    objfile.close();
}

vector<string> insert_in_table(string s,int* pointer,int cnt){
    int pnt=*pointer;
    vector<string> errors;
    // cout<<s<<endl;
    if(s.find(':')!=(string::npos)){
        int col=s.find(':',0);

        if(prev_present(s.substr(0,col))){
            errors.push_back("ERROR: Label aldredy declared. line= " + to_string(cnt));
        }

        if(!valid_label(s.substr(0,col))){
            errors.push_back("WARNING: Incorrect label formating. line = " + to_string(cnt));
        }

        if(col!=s.length()-1){
            string temp=s.substr(col+1,s.length());
            temp=del(temp);
            insert_in_table(temp,&pnt,cnt);

            int space = temp.find(" ",0);

            string operation= temp.substr(0,space);
            string value= temp.substr(space+1,temp.length());

            operation=del(operation);
            value=del(value);
            // cout<<temp<<"!!!"<<endl;
            if(operation=="SET"){
                labels tempp;
                tempp.name=s.substr(0,col);
                tempp.address=pnt;
                tempp.if_set=1;
                tempp.set_pc=stoi(value);
                table.push_back(tempp);
            }
            else if(operation=="data"){
                labels tempp;
                tempp.name=s.substr(0,col);
                tempp.address=pnt;
                tempp.if_set=0;
                // tempp.if_data=1;
                // tempp.set_data=stoi(value);
                table.push_back(tempp);
            }
            else{
                labels tempp;
                tempp.name=s.substr(0,col);
                tempp.address=pnt;
                tempp.if_set=0;
                table.push_back(tempp);

            }
        }
        else{
            *pointer=*pointer-1;
            labels tempp;
            tempp.name=s.substr(0,col);
            tempp.address=pnt;
            tempp.if_set=0;
            table.push_back(tempp);
        }
    }
    return errors;

}

void pass1(string file_name, ofstream& log_file){
    string line;
    int counter=1; // line counter
    int pnt=0; // location in file

    ifstream input_file(file_name);

    while(getline(input_file,line)){
        string temp;
        temp=line;
        temp = line.substr(0, line.find(";", 0));
        temp = del(temp);

        // Skip empty lines
        if(temp == "")
        {
            counter++;
            continue;
        }
        // string temp2="";

        // int len=temp.length();
        // int end=len;
        // for(int i=0;i<len;i++){
        //     if(temp[i]==';'){
        //         end=len+1;
        //         break;
        //     }
        //     temp2+=temp[i];
        // }
        // temp=del(temp);// delete extra spaces
        // temp=temp2;
        // if(temp.length()==0){
        //     counter++;
        //     continue;
        // }
        // cout<<temp<<endl;
        vector<string> err = insert_in_table(temp,&pnt,counter++);
        for(int i=0;i<err.size();i++){
            log_file << err[i];
            log_file << "\n";
        }
        pnt++;
    }

    input_file.close();
}
string convert_to_hex(int x){

    stringstream s;
    s << hex << x;
    string res=s.str();
    int z=0;
    z=8-res.length();
    for(int i=0;i<z;i++){
        res='0'+res;
    }
    return res;
}
bool chk_number(string s){
    if(s[0]=='-' || s[0]=='+'){
        s=s.substr(1,s.length());
    }
    if(s.find('0x')==1){
        for(int i=2;i<s.length();i++){
            if(((s[i]-'0'>=0) && (s[i]-'0'<=9))|| s[i]=='a' || s[i]=='b' || s[i]=='c'|| s[i]=='d'|| s[i]=='e'|| s[i]=='f'){
                // cout<<s[i]<<endl;
                continue;
            }
            else{
        // cout<<"!"<<endl;
                return 0;
            }
        }
        return 1;
    }
    if(s[0]=='0'){
    for(int i=1;i<s.length();i++){
        if(s[i]-'0'>=0 && s[i]-'0'<=7){
            continue;
        }
        else{
    // cout<<"!!!"<<endl;
            return 0;
        }
    }
    return 1;
    }
    for(int i=0;i<s.length();i++){//decimal
        if((s[i]-'0'>=0) && (s[i]-'0'<=9)){
            // cout<<s[i];
            continue;
        }
        else{
    // cout<<"!!"<<endl;
            return 0;
        }
    }
    return 1;
    // cout<<"!!!!"<<endl;
}
vector<string> write_listing_file(string line, int* location_ptr, int line_count){ // 0->machine_code 1->encoded 2->errors
    int loc= *location_ptr;


    string encoded;
    string errors;
    string machine_code;
    string temp_encoded;

    encoded+=convert_to_hex(loc)+" ";
    vector<string> return_value;
    if(line.find(":")!=string::npos){
    int col = line.find(':',0);
    if(col!=line.length()-1){
        string subs = line.substr(col + 1, line.length()-col-1);
        subs = del(subs);
        // cout<<"line count"<<line_count<<endl;
        vector<string> tempp=write_listing_file(subs,&loc,line_count);
        string temp_encoded=tempp[1];
        temp_encoded=temp_encoded.substr(9,9);
        encoded+=temp_encoded;
        errors=tempp[2];
        machine_code=tempp[0];
    }
    else{
        encoded+="         ";
        *location_ptr=*location_ptr-1;
    }
    encoded+=line+"\n";
    }
    else{
        int space=line.find(" ",0);
        string operation = line.substr(0,space);
        string operand = line.substr(space+1,line.length());
        string hexa;
        operation=del(operation);
        operand=del(operand);
        // cout<<operand<<" "<<operation<<endl;
        if(mnem.find(operation)==mnem.end()){
            errors += "ERROR: Mneominic not defined" + to_string(line_count) + "\n";
        }
        else if(instructions_without_operand.find(operation)!=instructions_without_operand.end()){
            encoded+= "000000"+mnem[operation]+" ";
            machine_code+= "000000"+mnem[operation]+" ";
            if(operation.length()!=line.length()){
                errors+="ERROR: No operand expected at line" + to_string(line_count) + "\n";
            }
        }
        else if(chk_number(operand)){
            // cout<<"operand="<<operand<<" "<<operation<<endl;
            // cout<<operand.length()<<" "<<operand<<endl;
            if(operand[0]=='0' && operand.length()>=2 && operand[1]!='x'){
                int z=0;
                int cnt_8=1;
                for(int i=operand.length()-1;i>=0;i--){
                    z=z+(operand[i]-'0')*cnt_8;
                    cnt_8*=8;
                }
                hexa=convert_to_hex(z);
            }
            else if(operand.length()>2 && operand[0]=='0' && operand[1]=='x'){
                hexa=operand.substr(2);
                int cc=6;
                string tt="";
                for(int i=0;i<6-hexa.length();i++){
                    tt+='0';
                }
                tt+=hexa;
                hexa=tt;
            }
            else{
                int number=0;
                int mult=1;
                for(int i=operand.length()-1;i>=1;i--){
                    number+=(operand[i]-'0')*mult;
                    mult*=10;
                }
                if(operand[0]=='+'){
                hexa=convert_to_hex(number);
                }
                else if(operand[0]=='-'){
                    hexa=convert_to_hex(-number);
                }
                else{
                    hexa=convert_to_hex(number+=((operand[0]-'0')*mult));
                }
            }
            if(mnem[operation]=="13"){

            encoded += "00" + hexa.substr(hexa.length() - 6, hexa.length()) + " "; 
        	machine_code +=" 00"+ hexa.substr(hexa.length() - 6, hexa.length()) + " "; 
            }
            else{
            encoded += hexa.substr(hexa.length() - 6, hexa.length()) + mnem[operation] + " "; 
        	machine_code += hexa.substr(hexa.length() - 6, hexa.length()) + mnem[operation] + " "; 
            }
        }
        else{
            int exist=0;
            // cout<<table.size()<<endl;
            for(int i=0;i<table.size();i++){
                // cout<<operation<<" "<<table[i].name<<endl;
                if(table[i].name.compare(operand)==0){
                exist=1;
                    if(table[i].if_set){
                        hexa=convert_to_hex(table[i].set_pc);
                    }
                    // else if(table[i].if_data){
                    //     hexa=convert_to_hex(table[i].set_data);
                    // }
                    else if(instructions_with_pcoffset[operation]){
                        hexa=convert_to_hex(table[i].address-loc-1);
                    }
                    else{
                        hexa=convert_to_hex(table[i].address);
                    }

                    // if(mnem[operation] == "13"){
                    //     cout<<table[i].name<<endl;
                    //     encoded+=hexa.substr(hexa.length()-6,hexa.length()) + " ";
                    //     machine_code+=hexa.substr(hexa.length()-6,hexa.length()) + " ";
                    // }
                    // cout<<mnem[operation]<<endl;
                    encoded+=hexa.substr(hexa.length()-6,hexa.length()) + (mnem[operation]) + " ";
                    machine_code+=hexa.substr(hexa.length()-6,hexa.length()) + (mnem[operation]) + " ";
                    break;
                }

            }
            if(operation.length()==line.length()){
                errors+="ERROR: Expected an operand at line!" + to_string(line_count) + "\n";

            }
            else if(!exist){
                errors+="ERROR: Unkown symbol at line" + to_string(line_count) + "\n";
            }
        }
        encoded+=line + "\n";
    }
    return_value.push_back(machine_code);
    return_value.push_back(encoded);
    return_value.push_back(errors);
    return return_value;
}


void pass2(string file, ofstream& outfile, ofstream& logfile, ofstream& objfile){
    int loc=0;
    int counter=1;
    string line;

    ifstream input_file(file);

    while(getline(input_file,line)){
       string comm;
        comm = line.substr(0, line.find(";", 0));
        comm = del(comm);

        // Skip empty lines
        if(comm == "")
        {
            counter++;
            continue;
        }
        line=comm;
        vector<string> res=write_listing_file(line, &loc,counter++);
        outfile << res[1];
        // cout<<"encoded= "<<res[0]<<endl;
        // cout<<"machine_code="<<res[1]<<endl;
        // cout<<res[0]<<endl;
        if(res[0]!=""){
            uint32_t temp = stoul("0x" + del(res[0]), nullptr, 16);
        	objfile.write((char *)&temp, sizeof(temp));
        }
        logfile << res[2];
        loc++;
    }


}


bool prev_present(string s){ // checking if label is previously present in table or not
    for(int i=0;i<table.size();i++){
        if(s==table[i].name){
            return 1;
        }
    }
    return 0;
}

bool valid_label(string s){ // checking valid label
    if( !((s[0] >= 'a' && s[0] <= 'z') || (s[0] >= 'A' && s[0] <= 'Z') || (s[0] == '_')) ) return false;
    
    for(int i = 0; i < s.length(); i++) {
        if( !( isdigit(s[i]) || (s[0] >= 'a' && s[0] <= 'z') || (s[0] >= 'A' && s[0] <= 'Z') || (s[0] == '_') ) ) {
            return false;
        }
    }

    return true;
}

string left_del(const string& s) {
	size_t start = s.find_first_not_of(USELESS);
    if(start != string::npos) {
        return s.substr(start);
    }
	return "";
}

string right_del(const string& s) {
	size_t end = s.find_last_not_of(USELESS);
    if(end != string::npos) {
        return s.substr(0,end+1);
    }
	return "";
}

string del(const string& s) {
	return right_del(left_del(s));
}