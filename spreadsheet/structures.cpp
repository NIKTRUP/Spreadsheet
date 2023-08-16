#include "common.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <charconv>

const int LETTERS = 26;
const int MAX_POSITION_LENGTH = 17;
const int MAX_POS_LETTER_COUNT = 3;

const Position Position::NONE = {-1, -1};

bool Position::operator==(const Position rhs) const {
    return row == rhs.row && col == rhs.col;
}

bool Position::operator<(const Position rhs) const {
    return row * MAX_COLS + col < rhs.row * MAX_COLS + rhs.col;
}

bool Position::IsValid() const {
    return row >= 0 && row < MAX_ROWS && col >= 0 && col < MAX_COLS;
}

std::string Position::ToString() const {
    if(!IsValid()){
        return std::string{};
    }

    std::string out;
    out.reserve(MAX_POSITION_LENGTH);
    for(int ch = col; ch >= 0; ch = ch / LETTERS - 1){
        out.push_back(static_cast<char>('A' + ch % LETTERS));
    }
    std::reverse(out.begin(), out.end());
    out += std::to_string(row + 1);

    return out;
}

Position Position::FromString(std::string_view str) {
    auto it = std::find_if_not(str.begin(), str.end(), [](const char c) {
        return std::isalpha(c) && std::isupper(c);
    });
    auto letters = str.substr(0, it - str.begin()),
        numbers = str.substr(it - str.begin());

    int row = 0, col = 0;
    if (letters.empty() || numbers.empty() ||
    letters.size() > MAX_POS_LETTER_COUNT ||
    std::find_if_not(numbers.begin(), numbers.end(), [](const char c){ return std::isdigit(c); }) != numbers.end() ||
    std::from_chars(numbers.data(), numbers.data() + numbers.size(), row).ec == std::errc::invalid_argument ||
    row > MAX_ROWS) {
        return Position::NONE;
    }

    for (const auto ch : letters) {
        col *= LETTERS;
        col += ch - 'A' + 1;
    }

    return {row - 1, col - 1};
}

bool Size::operator==(Size rhs) const {
    return cols == rhs.cols && rows == rhs.rows;
}