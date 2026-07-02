#include "smartthings4cpp/base64.h"

namespace smartthings4cpp {
namespace base64 {

namespace {
    constexpr char ALPHABET[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
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

} // namespace base64
} // namespace smartthings4cpp
