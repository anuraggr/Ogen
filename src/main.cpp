#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>

#include "./generation.hpp"

#ifdef _WIN32
    #define OS_WINDOWS
#elif __linux__
    #define OS_LINUX
#endif

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Incorrect usage. Correct usage is..." << std::endl;
        std::cerr << "ogen <input.hy>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string contents;
    {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in);
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }

    Tokenizer tokenizer(std::move(contents));
    std::vector<Token> tokens = tokenizer.tokenize();

    Parser parser(std::move(tokens));
    std::optional<NodeProg> prog = parser.parse_prog();

    if (!prog.has_value()) {
        std::cerr << "Invalid program" << std::endl;
        exit(EXIT_FAILURE);
    }

    Generator generator(prog.value());
    {
        std::fstream file("out.asm", std::ios::out);
        file << generator.gen_prog();
    }

    #ifdef OS_WINDOWS                       //Untestd. might not work on windows.
        system("nasm -fwin64 out.asm");
        system("link.exe out.obj /out:out.exe");
    #elif defined(OS_LINUX)
        system("nasm -felf64 out.asm");
        system("ld -o out out.o");
    #else
        std::cout << "Unsupported OS" << std::endl;
    #endif

    return EXIT_SUCCESS;
}