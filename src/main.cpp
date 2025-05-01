#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<optional>
#include<vector>

#include "./generation.hpp"

using namespace std;

int main(int argc, char* argv[]){
    if(argc!=2){
        cerr<<"Incorrect usage"<<endl;
        cerr<<"Correct Usage: "<<argv[0]<<" <input_file>"<<endl;
        return EXIT_FAILURE;
    }

    string contents;
    {
        stringstream contents_stream;
        fstream input(argv[1],ios::in);
        if(!input){
            cerr<<"Error opening file "<<argv[1]<<endl;
            return EXIT_FAILURE;
        }
        contents_stream<<input.rdbuf();
        contents=contents_stream.str();
    }
    Tokenizer tokenizer(contents);
    vector<Token> tokens=tokenizer.tokenize();
    
    Parser parser(tokens);
    optional<NodeProg> prog = parser.parse_prog();
    if(!prog.has_value()){
        cerr<<"Error parsing file "<<argv[1]<<endl;
        exit(EXIT_FAILURE);
    }
    Generator generator(prog.value());
    {
        fstream file("./build/out.asm",ios::out);
        file<<generator.gen_prog();
    }

    system("nasm -f elf64 ./build/out.asm -o ./build/out.o");
    system("ld ./build/out.o -o ./build/out");
    
    return EXIT_SUCCESS;
}