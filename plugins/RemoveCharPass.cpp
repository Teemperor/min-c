
#include <string>
#include <fstream>
#include <streambuf>


extern "C" {
    int check(const char* path) {
        return 1;
    }

    int transform(const char* path, unsigned long random) {
        std::string str;

        {
            std::ifstream t(path);

            t.seekg(0, std::ios::end);
            str.reserve(t.tellg());
            t.seekg(0, std::ios::beg);

            str.assign((std::istreambuf_iterator<char>(t)),
                        std::istreambuf_iterator<char>());
        }
        if (str.empty())
            return 0;
        {
            std::ofstream t(path);
            std::size_t charToSkip = random % str.size();
            for (size_t i = 0; i < str.size(); ++i) {
                if (i != charToSkip) {
                    t << str[i];
                }
            }
        }
        return 1;
    }
}
