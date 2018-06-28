#include <iostream>
#include <fstream>
#include <iomanip>
#include <string.h>
#include <stdlib.h>
#include <bitset>
#include <algorithm>

#include "instruction.h"

using namespace std;

instructions::instructions(char* file,bool debug)
{
    verbose = debug;
    count = 1;
    vm_file_name = new char;
    asm_file_name = new char;
    strcpy (vm_file_name,file);

    char keys[] = ".";
    int i = strcspn (file,keys);
    strncpy ( asm_file_name, file, i );
    strcat(asm_file_name,".asm");
}

void instructions::remove_comment_whitespace()
{
    ifstream vmfile (vm_file_name);
    if (!vmfile.is_open())
        perror("ERROR WHILE OPENING VM FILE");

    while(getline(vmfile, current_line)) 
    {
        for(unsigned int i=0;i<current_line.length();i++)
        {              
            //REMOVE COMMENT
            if (current_line[i]=='/')
                current_line.erase (i,current_line.length());
        }
        //REMOVE WHITESPACE
        current_line.erase( remove( current_line.begin(), current_line.end(), ' ' ), current_line.end() );

        //IF NOT EMPTY LINE, PUSH IT TO VECTOR
        if ( !current_line.empty() && current_line[0]!='	')
        {
            file_content.push_back(current_line);
        }
    }

    if(verbose)
    {
        unsigned int index = 0;
        cout<<"----------BOF----------"<<endl;
        for (vector<string>::const_iterator i = file_content.begin(); i != file_content.end(); ++i)
        {
            cout <<"VM LINE["<<index<<"]: "<< *i << endl;
            index++;
        }
        cout<<"----------EOF----------"<<endl<<endl;
    }
    vmfile.close();
}

void instructions::push_paser()
{
    size_t found_constant = current_line.find("constant");
    if (found_constant!=string::npos)
    {
        string write_line;
        current_line.erase (0,12);
        
        write_line.append("@");
        write_line.append(current_line);
        machinecode.push_back(write_line);
        machinecode.push_back("D=A");
    }

    size_t local_constant = current_line.find("local");
    if (local_constant!=string::npos)
    {
        string write_line;
        current_line.erase (0,9);
        
        machinecode.push_back("@LCL");
        machinecode.push_back("D=M");
        write_line.append("@");
        write_line.append(current_line);
        machinecode.push_back(write_line);
        machinecode.push_back("A=D+A");
        machinecode.push_back("D=M");
    }

    size_t argument_constant = current_line.find("argument");
    if (argument_constant!=string::npos)
    {
        string write_line;
        current_line.erase (0,12);
        
        machinecode.push_back("@ARG");
        machinecode.push_back("D=M");
        write_line.append("@");
        write_line.append(current_line);
        machinecode.push_back(write_line);
        machinecode.push_back("A=D+A");
        machinecode.push_back("D=M");
    }

    size_t this_constant = current_line.find("this");
    if (this_constant!=string::npos)
    {
        string write_line;
        current_line.erase (0,8);
        
        machinecode.push_back("@THIS");
        machinecode.push_back("D=M");
        write_line.append("@");
        write_line.append(current_line);
        machinecode.push_back(write_line);
        machinecode.push_back("A=D+A");
        machinecode.push_back("D=M");
    }

    size_t that_constant = current_line.find("that");
    if (that_constant!=string::npos)
    {
        string write_line;
        current_line.erase (0,8);
        
        machinecode.push_back("@THAT");
        machinecode.push_back("D=M");
        write_line.append("@");
        write_line.append(current_line);
        machinecode.push_back(write_line);
        machinecode.push_back("A=D+A");
        machinecode.push_back("D=M");
    }

    size_t pointer_constant = current_line.find("pointer");
    if (pointer_constant!=string::npos)
    {
        string write_line;
        current_line.erase (0,11);
        
        if(current_line=="0")
            machinecode.push_back("@THIS");
        else
            machinecode.push_back("@THAT");
        machinecode.push_back("D=M");
    }

    size_t static_constant = current_line.find("static");
    if (static_constant!=string::npos)
    {
        string write_line;
        current_line.erase (0,10);
        
        write_line.append("@");
        write_line.append(vm_file_name);
        write_line.append(".");
        write_line.append(current_line);
        machinecode.push_back(write_line);
        machinecode.push_back("D=M");
    }

    size_t temp_constant = current_line.find("temp");
    if (temp_constant!=string::npos)
    {
        string write_line;
        current_line.erase (0,8);
        
        machinecode.push_back("@R5");
        machinecode.push_back("D=A");
        write_line.append("@");
        write_line.append(current_line);
        machinecode.push_back(write_line);
        machinecode.push_back("A=D+A");
        machinecode.push_back("D=M");
    }

    machinecode.push_back("@SP");
    machinecode.push_back("A=M");
    machinecode.push_back("M=D");
    machinecode.push_back("@SP");
    machinecode.push_back("M=M+1");
    if(verbose)
    {
        cout<<"Current Line: "<<current_line<<endl;
    }
}

void instructions::pop_paser()
{
    size_t found_static = current_line.find("static");
    if (found_static!=string::npos)
    {
        string write_line;
        current_line.erase (0,9);
        
        write_line.append("@");
        write_line.append(vm_file_name);
        write_line.append(".");
        write_line.append(current_line);
        machinecode.push_back(write_line);
        machinecode.push_back("D=A");
        if(verbose)
        {
            cout<<"Current Line: "<<current_line<<endl;
        }
    }

    size_t local_static = current_line.find("local");
    if (local_static!=string::npos)
    {
        string write_line;
        current_line.erase (0,8);

        machinecode.push_back("@LCL");
        machinecode.push_back("D=M");
        write_line.append("@");
        write_line.append(current_line);
        machinecode.push_back(write_line);
        machinecode.push_back("D=D+A");
        if(verbose)
        {
            cout<<"Current Line: "<<current_line<<endl;
        }
    }

    size_t argument_static = current_line.find("argument");
    if (argument_static!=string::npos)
    {
        string write_line;
        current_line.erase (0,11);

        machinecode.push_back("@ARG");
        machinecode.push_back("D=M");
        write_line.append("@");
        write_line.append(current_line);
        machinecode.push_back(write_line);
        machinecode.push_back("D=D+A");
        if(verbose)
        {
            cout<<"Current Line: "<<current_line<<endl;
        }
    }

    size_t this_static = current_line.find("this");
    if (this_static!=string::npos)
    {
        string write_line;
        current_line.erase (0,7);

        machinecode.push_back("@THIS");
        machinecode.push_back("D=M");
        write_line.append("@");
        write_line.append(current_line);
        machinecode.push_back(write_line);
        machinecode.push_back("D=D+A");
        if(verbose)
        {
            cout<<"Current Line: "<<current_line<<endl;
        }
    }

    size_t that_static = current_line.find("that");
    if (that_static!=string::npos)
    {
        string write_line;
        current_line.erase (0,7);

        machinecode.push_back("@THAT");
        machinecode.push_back("D=M");
        write_line.append("@");
        write_line.append(current_line);
        machinecode.push_back(write_line);
        machinecode.push_back("D=D+A");
        if(verbose)
        {
            cout<<"Current Line: "<<current_line<<endl;
        }
    }

    size_t pointer_static = current_line.find("pointer");
    if (pointer_static!=string::npos)
    {
        string write_line;
        current_line.erase (0,10);

        if(current_line=="0")
            machinecode.push_back("@THIS");
        else
            machinecode.push_back("@THAT");
        
        machinecode.push_back("D=A");
        if(verbose)
        {
            cout<<"Current Line: "<<current_line<<endl;
        }
    }

    size_t temp_static = current_line.find("temp");
    if (temp_static!=string::npos)
    {
        string write_line;
        current_line.erase (0,7);

        machinecode.push_back("@R5");
        machinecode.push_back("D=A");
        write_line.append("@");
        write_line.append(current_line);
        machinecode.push_back(write_line);
        machinecode.push_back("D=D+A");
        if(verbose)
        {
            cout<<"Current Line: "<<current_line<<endl;
        }
    }

    machinecode.push_back("@R13");
    machinecode.push_back("M=D");
    machinecode.push_back("@SP");
    machinecode.push_back("AM=M-1");
    machinecode.push_back("D=M");
    machinecode.push_back("@R13");
    machinecode.push_back("A=M");
    machinecode.push_back("M=D");
}

void instructions::operations()
{
    size_t found_add = current_line.find("add");
    if (found_add!=string::npos)
        {
           machinecode.push_back("@SP");
           machinecode.push_back("AM=M-1");
           machinecode.push_back("D=M");
           machinecode.push_back("A=A-1");
           machinecode.push_back("M=D+M");
        }

    size_t found_sub = current_line.find("sub");
    if (found_sub!=string::npos)
        {
           machinecode.push_back("@SP");
           machinecode.push_back("AM=M-1");
           machinecode.push_back("D=M");
           machinecode.push_back("A=A-1");
           machinecode.push_back("M=M-D");
        }

    size_t found_neg = current_line.find("neg");
    if (found_neg!=string::npos)
        {
           machinecode.push_back("@SP");
           machinecode.push_back("A=M-1");
           machinecode.push_back("M=-M");
        } 

    size_t found_eq = current_line.find("eq");
    if (found_eq!=string::npos)
        {
            machinecode.push_back("@SP");
            machinecode.push_back("AM=M-1");
            machinecode.push_back("D=M");
            machinecode.push_back("A=A-1");
            machinecode.push_back("D=M-D");
            
            string write_line1;
            write_line1.append("@EQ.1.");
            write_line1.append(to_string(count));
            machinecode.push_back(write_line1);

            machinecode.push_back("D;JEQ");
            machinecode.push_back("@SP");
            machinecode.push_back("A=M-1");
            machinecode.push_back("M=0");

            string write_line2;
            write_line2.append("@EQ.2.");
            write_line2.append(to_string(count));
            machinecode.push_back(write_line2);

            machinecode.push_back("0;JMP");

            string write_line3;
            write_line3.append("(EQ.1.");
            write_line3.append(to_string(count));
            write_line3.append(")");
            machinecode.push_back(write_line3);

            machinecode.push_back("@SP");
            machinecode.push_back("A=M-1");
            machinecode.push_back("M=-1");

            string write_line4;
            write_line4.append("(EQ.2.");
            write_line4.append(to_string(count));
            write_line4.append(")");
            machinecode.push_back(write_line4);

            count++;
        }

}

void instructions::go_to()
{
    size_t found_if = current_line.find("if");
    if (found_if!=string::npos)
    {
        string write_line;
        current_line.erase (0,7);
        machinecode.push_back("@SP");
        machinecode.push_back("AM=M-1");
        machinecode.push_back("D=M");

        write_line.append("@");
        write_line.append(current_line);
        machinecode.push_back(write_line);
        machinecode.push_back("D;JNE");
    }
    else
    {
        string write_line;
        current_line.erase (0,4);
        write_line.append("@");
        write_line.append(current_line);
        machinecode.push_back(write_line);
        machinecode.push_back("0;JMP");
    }
}

void instructions::label()
{
    string write_line;
    current_line.erase (0,5);
    write_line.append("(");
    write_line.append(current_line);
    write_line.append(")");
    machinecode.push_back(write_line);
}

void instructions::decode()
{
    for (vector<string>::const_iterator i = file_content.begin(); i != file_content.end(); ++i) 
    {
       current_line = *i;
       if(verbose)
        {
            cout<<"Decode Line: "<<current_line<<endl;
        }

        size_t found_push = current_line.find("push");
        if (found_push!=string::npos)
        {
            push_paser();
        }

        size_t found_pop = current_line.find("pop");
        if (found_pop!=string::npos)
        {
            pop_paser();
        }

        size_t found_add = current_line.find("add");
        size_t found_sub = current_line.find("sub");
        size_t found_neg = current_line.find("neg"); 
        size_t found_eq = current_line.find("eq");
        if (found_add!=string::npos||found_sub!=string::npos||found_neg!=string::npos||found_eq!=string::npos)
        {
            operations();
        }

        size_t found_goto = current_line.find("goto");
        if (found_goto!=string::npos)
        {
            go_to();
        }

        size_t found_label = current_line.find("label");
        if (found_label!=string::npos)
        {
            label();
        }

    }
}


void instructions::writefile()
{
    ofstream asmfile;
    asmfile.open (asm_file_name);
    if (!asmfile.is_open())
        perror("ERROR WHILE OPENING ASM FILE");

    //Read vector and append to file    
    for (vector<string>::const_iterator i = machinecode.begin(); i != machinecode.end(); ++i)
    {
            asmfile<<(*i)<<endl;
    }
    asmfile.close();
}

