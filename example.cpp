// @@
// @ Author       : Eacher
// @ Date         : 2025-01-13 11:47:41
// @ LastEditTime : 2025-01-24 08:14:18
// @ LastEditors  : Eacher
// @ --------------------------------------------------------------------------------<
// @ Description  : 
// @ --------------------------------------------------------------------------------<
// @ FilePath     : /20yyq/test/c++/mllpv2/example.cpp
// @@
#include <iostream>
#include <string>
#include "../src/business.h"

#include "include/mllpv2.h"
#include "../time/include/time.h"

const std::string orgtp = "// std::size_t lll = args[1].size(); // mllpv2::Slice lic(lll); // lic.SetData(lll, (const unsigned char*)args[1].data()); // lll = args[2].size(); // lic = mllpv2::Slice(lll);// lic.SetData(lll, (const unsigned char*)args[2].data());";

size_t count = 10;
bool is;
int read(unsigned char*, unsigned long int);
mllpv2::ReadCloser r = { .Read = read };
mllpv2::Mllp *testm;


int read(unsigned char* buf, unsigned long int len)
{
    
    time_t rawtime;
    time(&rawtime);
    struct tm *when = localtime(&rawtime);
    char buf1[80];
    strftime(buf1, 80, etime::DateTime, when);
    std::string strtp = std::string(buf1) + "." + std::to_string(clock());
    unsigned long int sz = orgtp.size() + strtp.size();
    std::vector<unsigned char> tmp(sz);
    std::cout << "len\t" << len << "\tsize\t" << sz << "\ttmp.size\t" << tmp.size() << std::endl;
    strtp = orgtp + strtp;
    std::copy(strtp.begin(), strtp.end(), tmp.begin());
    testm->Splice(tmp);
    // if (count % 10 == 0 && !is)
    // {
    //     is = true, tmp.at(13) = 255;
    // }
    sz = tmp.capacity();
    if (sz > len) sz -= len;
    
    // unsigned short int ttt;
    // for (int i = 1; i > -1; i--) ttt += *(tmp.data() + 13 + i) << i*8;
    std::copy(tmp.begin(), tmp.begin()+sz, buf);
    return sz;
}

void mllp(int len) noexcept(false)
{
    try { testm = new mllpv2::Mllp(r, 13, 0, 4, 3); } catch(const char *e) { throw e; }
    
    std::cout << args[len-1] << std::to_string(len) << "len\t" << len << std::endl;
    testm->ConfigSet((const unsigned char*)"1234567891234", (const unsigned char*)"4321", (const unsigned char*)"5678");
    
    std::vector<unsigned char> tmp1(234);
    clock_t t = clock();
    std::cout << "start : " << t << std::endl;

    for (size_t i = 0; i < count; i++)
    {
        testm->Read(tmp1);
        tmp1.data()[tmp1.size()] = '\0';
        std::cout << "tmp1.data() : " << tmp1.data() << std::endl;
        is = false;
    }
    std::cout << "end : " << (clock() - t) << std::endl;
    if (len > 2)
    {
        // std::size_t lll = args[1].size();
        // mllpv2::Slice lic(lll);
        // lic.SetData(lll, (const unsigned char*)args[1].data());
        // std::cout << "111111111111111111 " << lic.Data() << "len\t" << lll << std::endl;
        // lll = args[2].size();
        // std::cout << "-------start----- " << std::endl;
        // lic = mllpv2::Slice(lll);
        // std::cout << "-------end----- " << std::endl;
        // lic.SetData(lll, (const unsigned char*)args[2].data());
        // std::cout << "222222222222222222 " << lic.Data() << "len\t" << lll << std::endl;
    }
    // MllpConfig_1 ttt(14, 5, 5, 4);
    // ttt.test();
}
