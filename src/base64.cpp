#include "smartthings4cpp/base64.h"

namespace smartthings4cpp {
namespace base64 {

namespace {
    constexpr char ALPHABET[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    // Map a base64 alphabet character to its 6-bit value, or -1 if it is not
    // part of the alphabet.
    int decodeChar(unsigned char c) {
        if (c >= 'A' && c <= 'Z') return c - 'A';
        if (c >= 'a' && c <= 'z') return c - 'a' + 26;
        if (c >= '0' && c <= '9') return c - '0' + 52;
        if (c == '+') return 62;
        if (c == '/') return 63;
        return -1;
    }

    bool isAsciiSpace(unsigned char c) {
        return c == ' ' || c == '\t' || c == '\r' || c == '\n';
    }
}

std::string encode(const std::string& input) {
    std::string output;
    output.reserve(((input.size() + 2) / 3) * 4);

    size_t i = 0;
    while (i + 3 <= input.size()) {
        unsigned char b0 = static_cast<unsigned char>(input[i]);
        unsigned char b1 = static_cast<unsigned char>(input[i + 1]);
        unsigned char b2 = static_cast<unsigned char>(input[i + 2]);

        output += ALPHABET[b0 >> 2];
        output += ALPHABET[((b0 & 0x03) << 4) | (b1 >> 4)];
        output += ALPHABET[((b1 & 0x0F) << 2) | (b2 >> 6)];
        output += ALPHABET[b2 & 0x3F];

        i += 3;
    }

    size_t remaining = input.size() - i;
    if (remaining == 1) {
        unsigned char b0 = static_cast<unsigned char>(input[i]);
        output += ALPHABET[b0 >> 2];
        output += ALPHABET[(b0 & 0x03) << 4];
        output += "==";
    } else if (remaining == 2) {
        unsigned char b0 = static_cast<unsigned char>(input[i]);
        unsigned char b1 = static_cast<unsigned char>(input[i + 1]);
        output += ALPHABET[b0 >> 2];
        output += ALPHABET[((b0 & 0x03) << 4) | (b1 >> 4)];
        output += ALPHABET[(b1 & 0x0F) << 2];
        output += "=";
    }

    return output;
}

std::optional<std::string> decode(const std::string& input) {
    std::string output;
    output.reserve((input.size() / 4) * 3);

    int accumulator = 0;   // holds up to 24 bits of decoded input
    int bits = 0;          // number of valid bits currently in the accumulator
    bool padding = false;  // once '=' is seen, only more '=' / whitespace may follow

    for (unsigned char c : input) {
        if (isAsciiSpace(c)) {
            continue;
        }
        if (c == '=') {
            padding = true;
            continue;
        }
        int value = decodeChar(c);
        if (value < 0 || padding) {
            // A non-alphabet character, or real data after padding: invalid.
            return std::nullopt;
        }
        accumulator = (accumulator << 6) | value;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            output += static_cast<char>((accumulator >> bits) & 0xFF);
        }
    }

    // Any bits left over must be the <8 bits that base64 pads with zeros; if
    // they are non-zero the input was malformed.
    if (bits > 0 && (accumulator & ((1 << bits) - 1)) != 0) {
        return std::nullopt;
    }
    return output;
}

} // namespace base64
} // namespace smartthings4cpp
