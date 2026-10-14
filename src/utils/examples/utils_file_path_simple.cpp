// Demonstrates the small file and path helpers provided by the UTILS module.
//
// The example writes a temporary file, appends another line, reads the file
// back, and then removes it so running the example leaves no test artifact.

#include <cstdio>
#include <iostream>
#include <string>

#include "utils/file_utils.h"
#include "utils/path_utils.h"

int main()
{
    using bitgraph::utils::File;
    using bitgraph::utils::append_slash;
    using bitgraph::utils::remove_path;

    const std::string filename = "utils_file_path_simple.txt";

    // Start from a clean state in case a previous run was interrupted.
    std::remove(filename.c_str());

    // WRITE creates the file and stores its first line.
    {
        File file(filename, File::Mode::WRITE);
        file.stream() << "created with WRITE mode\n";
    }

    // APPEND preserves the first line and adds a second one.
    {
        File file(filename, File::Mode::APPEND);
        file.stream() << "extended with APPEND mode\n";
    }

    // READ lets us consume the complete file through the wrapped stream.
    std::cout << "file contents:\n";
    {
        File file(filename, File::Mode::READ);
        std::string line;
        while (std::getline(file.stream(), line))
        {
            std::cout << "  " << line << '\n';
        }
    }

    // Path helpers operate on strings and do not access the filesystem.
    const std::string path = "output/examples/graph.txt";
    std::string directory = "output/examples";
    append_slash(directory);

    std::cout << "directory with separator: " << directory << '\n';
    std::cout << "filename without its path: " << remove_path(path) << '\n';

    // Keep the example self-contained and repeatable.
    std::remove(filename.c_str());
    return 0;
}
