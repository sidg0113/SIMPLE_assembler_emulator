// SIDDHANT GUPTA 2001CS69
//
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
int A, B, sp=0xFFFFFF, PC;
uint32_t memory[0x1000000];
int mnemonictype(uint32_t x)
{   
    return x%256;
}
int operandvalue(uint32_t x)
{
    x= x/256;
    if(x>=0x800000)
        x=x-0x1000000;
    return x;
}
void prompt() {
    cout << "Usage: ./emu.exe [option] file.o" << endl;
    cout << "Options:" << endl;
    cout << "\t-trace\tshow instruction trace" << endl;
    cout << "\t-before\tshow memory dump before execution" << endl;
    cout << "\t-after\tshow memory dump after execution" << endl;
    cout << "\t-isa\tdisplay ISA" << endl;
}

void ISA() {
    cout << "Opcode Mnemonic Operand\n";
    cout << "       data     value\n";
    cout << "0      ldc      value\n";
    cout << "1      adc      value\n";
    cout << "2      ldl      value\n";
    cout << "3      stl      value\n";
    cout << "4      ldnl     value\n";
    cout << "5      stnl     value\n";
    cout << "6      add\n";
    cout << "7      sub\n";
    cout << "8      shl\n";
    cout << "9      shr\n";
    cout << "10     adj      value\n";
    cout << "11     a2sp\n";
    cout << "12     sp2a\n";
    cout << "13     call     offset\n";
    cout << "14     return\n";
    cout << "15     brz      offset\n";
    cout << "16     brlz     offset\n";
    cout << "17     br       offset\n";
    cout << "18     HALT\n";
    cout << "       SET      value\n";
}
int main(int argc, char *argv[])
{
    if(argc < 3)
	{
		// Print prompt for usage
		prompt();
		return(0);
	}
    
	// Argument for input file
	string in_file = argv[2];

	// Argument for output file
	string trace_file = in_file.substr(0, in_file.find(".", 0)) + ".txt";

	// Input object file
	ifstream infile;

	// Output trace file
	ofstream trcfile; 

	// Open file
	infile.open(in_file, ios::out | ios::binary);
	trcfile.open(trace_file);
    int i=0;
    while (infile.read((char *)&memory[i],sizeof(memory[i])))
    {
        i++;
    }
    if(string(argv[1]) == "-isa"){
        ISA();
    }
    if(string(argv[1])=="-before"){
        string memfile = in_file.substr(0, in_file.find(".", 0)) + ".txt";
        ofstream mem_file;
        mem_file.open(memfile);
        for(int j=0;j<i;j++){
            mem_file << hex << memory[j]<<"\n";
        }
        mem_file.close();
    }
    while (1)
    {
        int type = mnemonictype(memory[PC]);
        int value = operandvalue(memory[PC]);
        if(string(argv[1]) == "-trace"){
            trcfile << hex <<"A= "<<A<<"\tB= "<<B<<"\tSP= "<<sp<<"\tPC= "<<PC<<"\n";
        }
        PC+=1;
        if (type == 0)
        {
            // ldc 0 value B=A, A=value
            B = A;
            A = value;
        }
        else if (type == 1)
        {
            // adc 1 value A=A+value
            A += value;
        }
        else if (type == 2)
        {
            // ldl 2 offset B=A,A=memory[sp+offset]
            B = A;
            A = memory[sp + value];
        }
        else if (type == 3)
        {
            // stl 3 offset memory[sp+offset]=A,A=B
            memory[sp + value] = A;
            A = B;
        }
        else if (type == 4)
        {
            // ldnl 4 offset A=memory[A+offset]
            A = memory[A + value];
        }
        else if (type == 5)
        {
            // stnl 5 offset memory[A+offset]=B
            memory[A + value] = B;
        }
        else if (type == 6)
        {
            // add 6 A=B+A
            A = B + A;
        }
        else if (type == 7)
        {
            // sub 7 A=B-A
            A = B - A;
        }
        else if (type == 8)
        {
            // shl 8 A=B<<A
            A = B << A;
        }
        else if (type == 9)
        {
            // shr 9 A=B>>A
            A = B >> A;
        }
        else if (type == 10)
        {
            // adj 10 value sp=sp+value
            sp = sp + value;
        }
        else if (type == 11)
        {
            // as2p 11 sp=A,A=B
            sp = A;
            A = B;
        }
        else if (type == 12)
        {
            // sp2a 12 B=A,A=sp
            B = A;
            A = sp;
        }
        else if (type == 13)
        {
            // call 13 offset B=A,A=PC,PC=PC+offset
            B = A;
            A = PC;
            PC = PC + value;
        }
        else if (type == 14)
        {
            // return 14 PC=A,A=B
            PC = A;
            A = B;
        }
        else if (type == 15)
        {
            // brz 15 offset if(A==0) PC=PC+offset
            if (A == 0)
                PC = PC + value;
        }
        else if (type == 16)
        {
            // brlz 16 offset if(A<0) PC=PC+offset
            if (A < 0)
                PC = PC + value;
        }
        else if (type == 17)
        {
            // br 17 offset PC=PC+offset
            PC = PC + value;
        }
        else if (type == 18)
        {
            // HALT 18
            // out << endl
            //      << "a " << A << "\tb " << B << "\tsp " << sp << "\tPC " << PC << endl;
            break;
        }

        // out << endl
        //      << "a " << A << "\tb " << B << "\tsp " << sp << "\tPC " << PC << endl;
    }
    if(string(argv[1])=="-after"){
        string memfile = in_file.substr(0, in_file.find(".", 0)) + ".txt";
        ofstream mem_file;
        mem_file.open(memfile);
        for(int j=0;j<i;j++){
            mem_file << hex << memory[j]<<"\n";
        }
        mem_file.close();
    }
    infile.close();
    trcfile.close();
}