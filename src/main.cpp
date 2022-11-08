#include <iostream>
#include <fstream>
#include <stack>

#define BF_SRC_PATH "hello.bf"
#define ASM_OUT_PATH "hello.asm"

void codegen_prelude(std::ofstream &out_s) {
    // Assembly output information
    out_s << "format ELF64 executable 3\n";
    // .bss and .data segment
    out_s << "segment readable writable\n";
    out_s << "\tmsg rb 1\n";
    out_s << "\tbuff rb 1000\n";
    // .code segment
    out_s << "segment readable executable\n";
    // Specify entry point label
    out_s << "entry main\n";
    // Helper procedures
    out_s << "put_char:\n";
    out_s << "\tmov al, byte[buff+rbx]\n";
    out_s << "\tmov byte[msg], al\n";
    out_s << "\tmov rax, 1\n";
    out_s << "\tmov rdi, 1\n";
    out_s << "\tmov rsi, msg\n";
    out_s << "\tmov rdx, 1\n";
    out_s << "\tsyscall\n";
    out_s << "\tret\n";
    // Start of main procedure
    out_s << "main:\n";
    // rbx register holds data pointer
    out_s << "\tmov rbx, 0\n";
}

void codegen_epilogue(std::ofstream &out_s) {
    // Wrap up main procedure
    out_s << "\tmov rax, 60\n";
    out_s << "\tmov rdi, 0\n"; // Exit code 0
    out_s << "\tsyscall\n";
}

void parse_line(std::ofstream &out_s, std::string &line) {
    unsigned int addr_count = 0;
    std::stack<unsigned int> loop_stack;   
    for (char c: line) {
        switch (c) {
            case '+':
                out_s << "\tadd byte[buff+rbx], 1\n";
                break;
            case '-':
                out_s << "\tsub byte[buff+rbx], 1\n";
                break;
            case '>':
                out_s << "\tadd rbx, 1\n";
                break;
            case '<':
                out_s << "\tsub rbx, 1\n";
                break;
            case '.':
                out_s << "\tcall put_char\n";
                break;
            case '[':
                out_s << "\tcmp byte[buff+rbx], 0\n";
                out_s << "\tje addr_" << addr_count+1 << "\n";
                out_s << "addr_" << addr_count+2 << ":\n";
                loop_stack.push(addr_count+2);
                addr_count += 2;
                break;
            case ']':
            {
                unsigned int return_addr = loop_stack.top();
                out_s << "\tcmp byte[buff+rbx], 0\n";
                out_s << "\tjne addr_" << return_addr << "\n";
                out_s << "addr_" << return_addr-1 << ":\n";
                loop_stack.pop();
                break;
            }
            default:
                break;
        }
    }
}

int main() {
    // Initialise file handlers
    std::ifstream bf_src_fhandler;
    std::ofstream asm_out_fhandler;
    bf_src_fhandler.open(BF_SRC_PATH);
    asm_out_fhandler.open(ASM_OUT_PATH);

    codegen_prelude(asm_out_fhandler);   
    // Parse BF source
    std::string line;
    while (getline(bf_src_fhandler, line)) {
        parse_line(asm_out_fhandler, line);
    }
    codegen_epilogue(asm_out_fhandler);
    // Close file handlers
    bf_src_fhandler.close();
    asm_out_fhandler.close();
    return 0;
}