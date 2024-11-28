#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>
#include <chrono>

#include "./tokenization.hpp"
#include "./parser.hpp"
#include "./generation.hpp"
#include "./utils/log.hpp"

int main(int argc, char* argv[])
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    // Add ipl
    //system("ipl -i -c --watch");
    if (system("ipl -v") != 0)
    {
        Log::error(7768, "ipl is not installed. Please install ipl.");
    }
    else
    {
        if (system("ipl -v >= 0.029") != 0)
        {
            Log::error(7769, "Ipl is to old. Please install or update to 0.029 or higher");
        }
    }

    if (system("ipla -v") != 0)
    {
        Log::error(7768, "ipl is not installed. Please install ipl.");
    }
    else
    {
        if (system("ipla -v >= A03F9C-b930.029") != 0)
        {
            Log::error(7770, "Ipl is to old. Please install or update to A03F9C-b930.029 or A938293");
        }
    }

    if (argc != 2)
    {
        std::cerr << "Incorrect usage. Correct usage is..." << std::endl;
        std::cerr << "cosmolingua <input.cl>" << std::endl;
        Log::error(1948);
        return EXIT_FAILURE;
    }

    Log::add("Starting Cosmolang Architecture Compiler");

    std::string contents;
    {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in);
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
        if (contents.empty())
        {
            Log::error(2054);
        }
    }
    std::cout << "Reading successfully." << std::endl;
    Log::add("Reading successfully.");

    Tokenizer tokenizer(std::move(contents));
    std::vector<Token> tokens = tokenizer.tokenize();
    std::cout << "Tokenization successfully." << std::endl;
    Log::add("Tokenization successfully.");

    // Integrate Cosmolang Linker and Cosmolang Assembler ICL and ICA
    Parser parser(std::move(tokens));
    std::optional<NodeProg> prog = parser.parse_prog();
    std::cout << "Parsing and building AST successfully." << std::endl;
    Log::add("Parsing and building AST successfully.");

    if (!prog.has_value())
    {
        Log::error(2301);
    }

    Generator generator(prog.value());
    {
        std::fstream file("output.asm", std::ios::out);
        file << generator.gen_prog();
    }
    std::cout << "Generation successfully." << std::endl;
    Log::add("Generation successfully.");
    Log::addSuccess("Generation of Program successfully.");

    // Check if nasm is installed
    if (system("nasm -v") != 0)
    {
        Log::error(7768, "nasm is not installed. Please install nasm.");
    }
    Log::addInfo("Nasm is installed");

    Log::addWarning("NASM's program only works on Linux. Please use WSL or Linux to run the program.");

    // Later add Integrate Cosmolang Linker and Cosmolang Assembler ICL and ICA And ICO (Integrate Cosmolang Object)
    system("nasm -f elf64 output.asm -o output.o && ld output.o -o output");
    std::cout << "Linking and Assembling successfully. (Build)" << std::endl;
    Log::add("Build successfully.");

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Compilation Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
        << "ms" << std::endl;

    Log::addInfo(
        "Compilation Time: " + std::to_string(
            std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()) + "ms. Compiled successfully.");

    Log::createFile();

    return 0;
}
