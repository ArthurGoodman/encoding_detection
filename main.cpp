#include <cmath>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>

namespace {

constexpr int c_num_symbols = 1 << 8;
constexpr int c_num_pairs = 1 << 16;

std::string readFile(const std::string &file_name) {
    std::ifstream file(file_name, std::ios::binary);

    if (!file)
        throw std::runtime_error("unable to open file '" + file_name + "'");

    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

void countSymbolsAndPairs(const std::string &str, int *symbol_counts, int *pair_counts) {
    for (char c : str)
        symbol_counts[static_cast<int>(static_cast<unsigned char>(c))]++;

    for (size_t i = 1; i < str.size(); i++) {
        unsigned char ca = static_cast<unsigned char>(str[i - 1]);
        unsigned char cb = static_cast<unsigned char>(str[i]);

        pair_counts[(ca << 8) + cb]++;
    }
}

double g_utf8_symbol_prob[c_num_symbols] = {}, g_win1251_symbol_prob[c_num_symbols] = {};
double g_utf8_pair_prob[c_num_pairs] = {}, g_win1251_pair_prob[c_num_pairs] = {};

void learn() {
    std::string utf8_text, win1251_text;

    int utf8_symbol_counts[c_num_symbols] = {}, win1251_symbol_counts[c_num_symbols] = {};
    int utf8_pair_counts[c_num_pairs] = {}, win1251_pair_counts[c_num_pairs] = {};

    utf8_text = readFile("./data/war-and-peace-utf-8.txt");
    win1251_text = readFile("./data/war-and-peace-windows-1251.txt");

    countSymbolsAndPairs(utf8_text, utf8_symbol_counts, utf8_pair_counts);
    countSymbolsAndPairs(win1251_text, win1251_symbol_counts, win1251_pair_counts);

    for (size_t i = 0; i < c_num_symbols; i++) {
        g_utf8_symbol_prob[i] = double(utf8_symbol_counts[i]) / double(utf8_text.size());
        g_win1251_symbol_prob[i] = double(win1251_symbol_counts[i]) / double(win1251_text.size());
    }

    for (size_t i = 0; i < c_num_pairs; i++) {
        g_utf8_pair_prob[i] = double(utf8_pair_counts[i]) / double(utf8_text.size() - 1);
        g_win1251_pair_prob[i] = double(win1251_pair_counts[i]) / double(win1251_text.size() - 1);
    }
}

constexpr double c_eps = 1e-12;

enum EEncoding {
    UTF_8,
    WINDOWS_1251,
};

EEncoding detect(const std::string &text) {
    double p_utf8 = 0;
    double p_win1251 = 0;

    if (g_utf8_symbol_prob[static_cast<int>(static_cast<unsigned char>(text[0]))] > c_eps)
        p_utf8 = -log(g_utf8_symbol_prob[static_cast<int>(static_cast<unsigned char>(text[0]))]);

    if (g_win1251_symbol_prob[static_cast<int>(static_cast<unsigned char>(text[0]))] > c_eps)
        p_win1251 =
            -log(g_win1251_symbol_prob[static_cast<int>(static_cast<unsigned char>(text[0]))]);

    for (size_t i = 1; i < text.size(); i++) {
        unsigned char ca = static_cast<unsigned char>(text[i - 1]);
        unsigned char cb = static_cast<unsigned char>(text[i]);

        size_t i_symbol = cb;
        size_t i_pair = static_cast<size_t>((ca << 8) + cb);

        if (g_utf8_symbol_prob[i_symbol] > c_eps && g_utf8_pair_prob[i_pair] > c_eps)
            p_utf8 -= log(g_utf8_symbol_prob[i_symbol]) + log(g_utf8_pair_prob[i_pair]);

        if (g_win1251_symbol_prob[i_symbol] > c_eps && g_win1251_pair_prob[i_pair] > c_eps)
            p_win1251 -= log(g_win1251_symbol_prob[i_symbol]) + log(g_win1251_pair_prob[i_pair]);
    }

    // std::cout << "p_utf8 = " << p_utf8 << std::endl;
    // std::cout << "p_win1251 = " << p_win1251 << std::endl;

    return p_utf8 > p_win1251 ? EEncoding::UTF_8 : EEncoding::WINDOWS_1251;
}

void run(const std::string &file_name) {
    std::cout << (detect(readFile(file_name)) == EEncoding::UTF_8 ? "UTF-8" : "Windows 1251")
              << std::endl;
}

} // namespace

int main(int argc, char *argv[]) {
    learn();

    if (argc > 1)
        run(argv[1]);

    return 0;
}
