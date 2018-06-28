#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include <string>
#include <vector>
#include <map>
#include <inttypes.h>

class instructions
{
    private:
    bool verbose;
    //Count use for add sub neg and eq
    int count;
    std::string current_line;

    char* vm_file_name;
    char* asm_file_name;

    //File content without whitespace
    std::vector<std::string> file_content;
    std::vector<std::string> machinecode;

    void push_paser();
    void pop_paser();
    void operations();
    void go_to();
    void label();

    public:
    instructions(char* file,bool debug);
    //~instructions();
    void decode();
    void remove_comment_whitespace();
    void writefile();
};

#endif