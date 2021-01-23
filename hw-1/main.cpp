#include <bits/stdc++.h>
using namespace std;

template<const int prec = 10000>
struct Decimal {
    vector<uint32_t> bits;

    Decimal(int val) {
        assert(val == 0 || val == 1);
        bits.resize(prec);
        if (val == 1) {
            fill(bits.begin(), bits.end(), -1);
        }
    }

    Decimal(const Decimal& other) = default;

    Decimal & operator += (const Decimal& other) {
        uint64_t carry = 0;
        for (int i = 0; i < prec; ++i) {
            uint64_t val = carry + bits[i] + other.bits[i];
            bits[i] = val;
            carry = val >> 32;
        }
        return *this;
    }

    Decimal & operator -= (const Decimal& other) {
        uint64_t carry = 0;
        for (int i = 0; i < prec; ++i) {
            uint64_t val = bits[i] - other.bits[i] - carry;
            carry = (carry + other.bits[i] > bits[i]);
            bits[i] = val;
        }
        return *this;
    }

    Decimal & operator *= (uint32_t other) {
        uint64_t carry = 0;
        for (int i = 0; i < prec; ++i) {
            uint64_t val = carry + bits[i] * 1ull * other;
            bits[i] = val;
            carry = val >> 32;
        }
        return *this;
    }

    Decimal & operator /= (uint32_t other) {
        uint64_t carry = 0;
        for (int i = prec - 1; i >= 0; --i) {
            carry = (carry << 32) + bits[i];
            bits[i] = carry / other;
            carry %= other;
        }
        return *this;
    }

    friend Decimal operator + (Decimal d, const Decimal& other) {
        return d += other;
    }


    friend Decimal operator - (Decimal d, const Decimal& other) {
        return d -= other;
    }

    friend Decimal operator * (Decimal d, uint32_t other) {
        return d *= other;
    }

    friend Decimal operator / (Decimal d, uint32_t other) {
        return d /= other;
    }

    friend bool operator < (const Decimal & a, const Decimal & b) {
        for (int i = a.bits.size() - 1; i >= 0; --i) {
            if (a.bits[i] > b.bits[i]) {
                return false;
            } else if (a.bits[i] < b.bits[i]) {
                return true;
            }
        }
        return false;
    }

    friend bool operator >= (const Decimal & a, const Decimal & b) {
        for (int i = a.bits.size() - 1; i >= 0; --i) {
            if (a.bits[i] < b.bits[i]) {
                return false;
            } else if (a.bits[i] > b.bits[i]) {
                return true;
            }
        }
        return true;
    }

    void print() {
        for (int i = 0; i < prec; ++i) {
            cout << bits[i] << "\n";
        }
    }

    int getBit(int x, int i) {
       return (x >> i) & 1;
    }
    
    void printBin() {
        for (int i = prec - 1; i >= 0; --i) {
            for (int j = 31; j >= 0; --j) {
                cout << getBit(bits[i], j);
            }
        }
    }

    void printNorm() {
        double res = 0;
        double pw = 0.5;
        for (int i = prec - 1; i >= 0; --i) {
            for (int j = 31; j >= 0; --j) {
                if (getBit(bits[i], j)) {
                    res += pw;
                }
                pw /= 2;
            }
        }
        cout << res;
    }
};


struct ArithmeticEncoding {
    struct Segment {
        int left;
        int right;
        char c;
    };
    int length;
    string s;
    vector<char> letters;
    vector<int> probabilities;
    map<char, int> occurences;
    map<char, int> index;
    vector<Segment> segment;

    ArithmeticEncoding(string s) : s(s) {
        length = s.length();
        for (const auto& c : s) {
            letters.emplace_back(c);
            occurences[c]++;
        }
        sort(letters.begin(), letters.end());
        letters.erase(unique(letters.begin(), letters.end()), letters.end());
        int l = 0;
        int id = 0;
        for (const auto& c : letters) {
            index[c] = id++;
            probabilities.emplace_back(occurences[c]);
            segment.emplace_back(Segment{l, l + probabilities.back(), c});
            l += probabilities.back();
        }
    }

    Decimal<> encode() {
        Decimal<> left(0);
        Decimal<> right(1);
        for (const auto& c : s) {
            Decimal<> updLeft = left + (right - left) / length * segment[index[c]].left;
            Decimal<> updRight = left + (right - left) / length * segment[index[c]].right;
            left = updLeft;
            right = updRight;
        }
        return (left + right) / 2;
    }
};

struct ArithmeticDecoding {
    struct Segment {
        int left;
        int right;
        char c;
    };
    int length;
    vector<char> letters;
    vector<int> probabilities;
    vector<Segment> segment;
    Decimal<> val;

    ArithmeticDecoding(int length, vector<char>& letters, vector<int>& probabilities, Decimal<> val) : 
      length(length),
      letters(letters),
      probabilities(probabilities),
      val(val) {
        int n = letters.size();
        int l = 0;
        for (int i = 0; i < n; ++i) {
            segment.emplace_back(Segment{l, l + probabilities[i], letters[i]});
            l += probabilities[i];
        }
    }

    string decode() {
        string s;
        Decimal<> denom(1);
        denom /= length;
        for (int i = 0; i < length; ++i) {
            for (int j = 0; j < letters.size(); ++j) {
                if (val >= denom * segment[j].left && val < denom * segment[j].right) {
                    s += segment[j].c;
                    val = (val - denom * segment[j].left) / (segment[j].right - segment[j].left) * length;
                    break;
                }
            }
        } 
        return s;
    }
};

string encodeInt(uint32_t x) {
    string res;
    res.resize(sizeof(x));
    memcpy(res.data(), &x, sizeof(x));
    return res;
}

int decodeInt(uint32_t* x, unsigned char* byte) {
    memcpy(x, byte, sizeof(uint32_t));
    return sizeof(uint32_t);
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        return 1;
    }
    string type, finS, foutS, encode;
    type = argv[1];
    finS = argv[2];
    foutS = argv[3];
    encode = argv[4];
    ifstream fin(finS);
    ofstream fout(foutS);
    if (type == "c") {
        string text, line;
        while (getline(fin, line)) {
            text += line + "\n";
        }
        ArithmeticEncoding encoder = ArithmeticEncoding(text);
        Decimal<> vv = encoder.encode();
        string len = encodeInt(text.length());
        string cnt = encodeInt(encoder.letters.size());
        string curr;
        for (int i = 0; i < encoder.letters.size(); ++i) {
            curr += encodeInt(encoder.probabilities[i]);
            curr += encoder.letters[i];
        }
        string s;
        for (const auto& bit : vv.bits) {
            s += encodeInt(bit);
        }
        fout << len + cnt + curr + s;
    } else if (type == "d") {
        int length;
        int cnt;
        vector<char> letters;
        vector<int> probabilities;
        Decimal<> vv(0);
        int c;
        string data;
        while ((c = fin.get()) != EOF) {
            data.push_back(c);
        }
        unsigned char* bytes = reinterpret_cast<unsigned char*>(data.data());
        memcpy(&length, bytes, sizeof(length));
        bytes += sizeof(length);
        memcpy(&cnt, bytes, sizeof(cnt));
        bytes += sizeof(cnt);
        for (int i = 0; i < cnt; ++i) {
            char c;
            int prob;
            memcpy(&prob, bytes, sizeof(prob));
            bytes += sizeof(prob);
            probabilities.emplace_back(prob);
            memcpy(&c, bytes, sizeof(c));
            bytes += sizeof(c);
            letters.emplace_back(c);
        }
        for (auto& bit : vv.bits) {
            uint32_t val;
            bytes += decodeInt(&val, bytes);
            bit = val;
        }
        ArithmeticDecoding decoder = ArithmeticDecoding(length, letters, probabilities, vv);
        fout << decoder.decode();
    } else {
        exit(0);
    }
    fin.close();
    fout.close();
    return 0;
}