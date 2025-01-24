// @@
// @ Author       : Eacher
// @ Date         : 2025-01-13 13:44:43
// @ LastEditTime : 2025-01-23 16:50:32
// @ LastEditors  : Eacher
// @ --------------------------------------------------------------------------------<
// @ Description  : 
// @ --------------------------------------------------------------------------------<
// @ FilePath     : /20yyq/test/c++/mllpv2/include/mllpv2.h
// @@
#ifndef __EACHER_MLLPV2_H
#define __EACHER_MLLPV2_H	1

#include <vector>

namespace mllpv2
{
    typedef signed char int8_t;
    typedef unsigned char uint8_t;
    typedef unsigned short int uint16_t;
    typedef unsigned long int uint64_t;
    
    typedef signed long int int64_t;
    // typedef unsigned long long int uint128_t;
    typedef struct
    {
        // 接受一个数组指针，数组长度；返回写入的长度，返回负值为错误
        int (*Read)(unsigned char*, unsigned long int);
        // int (*Read)(const std::vector<unsigned char>&);
        // #include <functional>
        // std::function<int (unsigned char *, unsigned long)> *Read;
        int (*Close)();
    } ReadCloser;

    class Config
    {
    protected:
        std::vector<unsigned char> head, tail, br;
    public:
        const uint8_t lenBytes;
        Config(uint8_t hl, uint8_t tl, uint8_t brl, uint8_t len) noexcept(false)
            : head(hl), tail(tl), br(brl), lenBytes(len)
        { if (head.size() < 1 || tail.size() < 1) throw "need head && tail"; }
        void ConfigSet(unsigned char const* = nullptr, unsigned char const* = nullptr, unsigned char const* = nullptr);
    };
    
    enum SCAN_STATE : uint8_t { OK, NEXT, JUMP };
    class Mllp;
    class scan : protected Config
    {
        std::vector<unsigned char> &buf;
    public:
        uint64_t dl, hidx, tidx, i;
        int64_t n;
        SCAN_STATE (scan::*run)();

        scan(std::vector<unsigned char>& m, uint8_t hl, uint8_t tl, uint8_t brl, uint8_t len) noexcept(false)
            : buf(m), Config(hl, tl, brl, len), dl(0), i(0), n(0)
        { }
        SCAN_STATE Head();
        SCAN_STATE Datalen();
        SCAN_STATE V1Tail();
        SCAN_STATE Tail();
        SCAN_STATE BR();
    };

    class Mllp : protected scan
    {
        ReadCloser io;
        std::vector<unsigned char> buffer;
    public:
        Mllp(ReadCloser rc, uint8_t hl, uint8_t tl, uint8_t brl, uint8_t len) noexcept(false)
            : scan(buffer, hl, tl, brl, len), buffer(4096), io(rc)
        { }
        using scan::ConfigSet;
        void Read(std::vector<unsigned char>&) noexcept(false);
        bool Splice(std::vector<unsigned char>&);
    };
}

#endif // mllpv2