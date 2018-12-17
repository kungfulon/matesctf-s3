#include <bits/stdc++.h>

using namespace std;

unsigned char magic[42] =
{
    0x08, 0x8B, 0xDE, 0xBF, 0x9D, 0xA9, 0x04, 0x12, 0x8B, 0x12, 
    0x12, 0x9A, 0x62, 0xA9, 0x51, 0x47, 0xEA, 0xEA, 0x30, 0x8B, 
    0x26, 0x02, 0x4B, 0x77, 0x32, 0x74, 0x41, 0x7D, 0x4A, 0x7C, 
    0x4F, 0x0B, 0x4F, 0x7B, 0x05, 0x6C, 0x1B, 0x7D, 0x0B, 0x63, 
    0x14, 0x70
};

char out[42];

int main() {
    reverse(magic, magic + 42);
    out[0] = magic[0];
    
    for (int i = 1; i < 21; ++i) {
        out[i] = magic[i] ^ magic[i - 1];
    }

    for (int x = 0; x < 256; ++x) {
        if ((x ^((x * 16) & 0xff)) == magic[21]) {
            out[21] = x;
            break;
        } 
    }

    for (int i = 22; i < 42; ++i) {
        for (int x = 0; x < 256; ++x) {
            int c = x;
            c ^= magic[i - 1] >> 4;
            c ^= (16 * c) & 0xff;

            if (c == magic[i]) {
                out[i] = x;
                break;
            }
        }
    }

    for (int i = 0; i < 42; ++i) {
        out[i] -= 3;
    }

    cout << out;
}
