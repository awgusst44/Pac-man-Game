#pragma once

// Ajutoare interne pentru operatorii de intrare (>>). Sunt funcții inline (header-only),
// deci nu aparțin niciunei biblioteci și pot fi folosite de toate.

#include <algorithm>
#include <cctype>
#include <initializer_list>
#include <istream>
#include <string>

#include "Enums.hpp"

namespace pacman {
namespace detail {

// Citește un cuvânt format doar din litere (după spații).
inline bool readWord(std::istream& in, std::string& word) {
    in >> std::ws;
    word.clear();
    while (in && std::isalpha(in.peek())) {
        word.push_back(static_cast<char>(in.get()));
    }
    return !word.empty();
}

// Așteaptă un anumit caracter (după spații); altfel setează failbit.
inline bool expectChar(std::istream& in, char expected) {
    in >> std::ws;
    if (in.peek() == std::char_traits<char>::to_int_type(expected)) {
        in.get();
        return true;
    }
    in.setstate(std::ios::failbit);
    return false;
}

// Așteaptă un cuvânt-cheie anume; altfel setează failbit.
inline bool expectKeyword(std::istream& in, const std::string& keyword) {
    std::string word;
    if (readWord(in, word) && word == keyword) {
        return true;
    }
    in.setstate(std::ios::failbit);
    return false;
}

// "nume=" (cuvânt-cheie urmat de '=').
inline bool field(std::istream& in, const std::string& name) {
    return expectKeyword(in, name) && expectChar(in, '=');
}

inline bool intField(std::istream& in, const std::string& name, int& value) {
    return field(in, name) && static_cast<bool>(in >> value);
}

// Citește un cuvânt și îl potrivește cu numele valorilor enum.
template <typename E>
std::istream& readEnum(std::istream& in, E& out, std::initializer_list<E> values) {
    std::string word;
    if (!readWord(in, word)) {
        in.setstate(std::ios::failbit);
        return in;
    }
    const auto found = std::find_if(values.begin(), values.end(),
                                    [&word](E value) { return toString(value) == word; });
    if (found == values.end()) {
        in.setstate(std::ios::failbit);
        return in;
    }
    out = *found;
    return in;
}

}  // namespace detail
}  // namespace pacman
