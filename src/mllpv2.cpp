// @@
// @ Author       : Eacher
// @ Date         : 2025-01-13 11:49:47
// @ LastEditTime : 2025-01-23 16:30:12
// @ LastEditors  : Eacher
// @ --------------------------------------------------------------------------------<
// @ Description  : 
// @ --------------------------------------------------------------------------------<
// @ FilePath     : /20yyq/test/c++/mllpv2/src/mllpv2.cpp
// @@
#include "../include/mllpv2.h"

#include <iostream>
#include <string>
#include <algorithm> 

using namespace mllpv2;

template<typename T> signed long int bytesIndex(std::vector<T>& p, std::vector<T>& sub)
{
	if (p.size() < sub.size()) return -1;
	size_t maxLen = p.size() - sub.size();
	for (size_t i = 0; i <= maxLen; i++)
	{
		bool foud = true;
		for (size_t i1 = 0; i1 < sub.size(); i1++)
		{
			if (sub[i1] != p[i+i1])
			{
				foud = false;
				break;
			}
		}
		if (foud) return i;
	}
	return -1;
}

template<typename T> bool vectorMatch(std::vector<T>& a, std::vector<T>& b)
{
	size_t len = a.size() > b.size() ? b.size() : a.size();
	for (size_t i = 0; i < len; i++) if (a[i] != b[i]) return false;
	return true;
}

void Config::ConfigSet(unsigned char const*head, unsigned char const*tail, unsigned char const*br)
{
	if (head != nullptr) std::copy(head, head + this->head.size(), this->head.begin());
	if (tail != nullptr) std::copy(tail, tail + this->tail.size(), this->tail.begin());
	if (br != nullptr) std::copy(br, br + this->br.size(), this->br.begin());
}

void Mllp::Read(std::vector<unsigned char> &s) noexcept(false)
{
	run = &scan::Head;
	uint64_t length = buffer.capacity();
	unsigned char *ptr = buffer.data();
	n = io.Read(&(ptr[i]), length - i);
    while (n > -1)
    {
		n += i, i = 0;
		// std::cout << "n += i\t" << n << "\tn += i\t" << i << std::endl;
		SCAN_STATE res = (this->*run)();
		if (res == SCAN_STATE::OK) {
			i = tidx - tail.size() - hidx - lenBytes;
			s.reserve(i);
			s.resize(s.capacity());
			std::copy(buffer.begin() + hidx + lenBytes, buffer.begin() + hidx + lenBytes + i, s.begin());
			n -= tidx + br.size(), hidx = 0, tidx = 0, dl = 0, i = n;
			return;
		}
		else if (res == SCAN_STATE::JUMP) {
			continue;
		}
		if (n < 0 || uint64_t(n) >= length) n = 0, hidx = 0, tidx = 0, run = &scan::Head;
		i = n;
		n = io.Read(&(ptr[i]), length-i);
    }
	throw("EOF");
}

bool Mllp::Splice(std::vector<unsigned char> &s)
{
	unsigned int l = s.size();
	if (l < head.size() + lenBytes + tail.size() + br.size()) return false;
	s.reserve(head.size() + lenBytes + l + tail.size() + br.size());
	s.resize(s.capacity());
	std::vector<unsigned char>::iterator it = s.begin();
	std::copy_backward(it, it + l, it + head.size() + lenBytes + l);
	std::copy(head.begin(), head.end(), it);
	std::copy((unsigned char *)&l, (unsigned char *)&l + lenBytes, it + head.size());
	it += head.size() + lenBytes + l;
	std::copy(tail.begin(), tail.end(), it);
	it += tail.size();
	std::copy(br.begin(), br.end(), it);
	return true;
}

SCAN_STATE scan::Head()
{
	std::vector<unsigned char> tmp(n);
	std::copy(buf.begin(), buf.begin() + n, tmp.begin());
	signed long int i1 = bytesIndex(tmp, head);
	if (i1 < 0) return SCAN_STATE::NEXT;
	hidx = head.size(), run = &scan::Datalen;
	if (i1 > 0)
	{
		std::copy_backward(buf.begin() + i1, buf.begin() + n, buf.begin() + (n - i1));
		std::copy_backward(buf.begin() + n,  buf.end(), buf.end() - i1);
		n -= i1;
	}
	return SCAN_STATE::JUMP;
}

SCAN_STATE scan::Datalen()
{
	run = &scan::V1Tail;
	if (lenBytes > 0) {
		if (n < 0 || hidx + lenBytes > uint64_t(n)) return SCAN_STATE::NEXT;
		unsigned int u32byte = 0;
		for (i = 0; i < lenBytes; i++) *(((unsigned char *)&u32byte)+i) = *(buf.data() + hidx + i);
		dl = u32byte, i = 0;
		std::cout << "---------Datalen---------" << u32byte << "\tnnnnnnn\t" << n << "\tiiiiiiiiiiiii\t" << hidx << std::endl;
		if (dl == 0 || dl > uint64_t(n) - (hidx + lenBytes + tail.size()))
        {
			std::copy_backward(buf.begin() + hidx, buf.begin() + n, buf.begin() + (n - hidx));
			std::copy_backward(buf.begin() + n,  buf.end(), buf.end() - hidx);
			// std::cout << "-------begin---------" << (buf.data()) << std::endl;
			n -= hidx, dl = 0, hidx = 0, tidx = 0, run = &scan::Head;
			if (n < head.size()) return SCAN_STATE::NEXT;
			return SCAN_STATE::JUMP;
		}
		run = &scan::Tail;
	}
	return SCAN_STATE::JUMP;
}

// mllp 原始版本支持
SCAN_STATE scan::V1Tail()
{
	dl = n - hidx;
	std::vector<unsigned char> tmp(dl);
	std::copy(buf.begin() + hidx, buf.begin() + n, tmp.begin());
	signed long int i1 = bytesIndex(tmp, tail);
	if (i1 < 0) return SCAN_STATE::NEXT;
	tidx = hidx + i1 + tail.size(), dl = 0, run = &scan::BR;
	return SCAN_STATE::JUMP;
}

SCAN_STATE scan::Tail()
{
	if (n > hidx)
    {
		if (hidx + lenBytes + dl + tail.size() <= uint64_t(n))
		{
			std::vector<unsigned char> tmp(tail.size());
			std::copy(buf.begin() + hidx + lenBytes + dl, buf.begin() + hidx + lenBytes + dl + tail.size(), tmp.begin());
			std::cout << "---------Tail---------\t" << hidx << "\thead\t" << dl << std::endl;
			if (vectorMatch(tmp, tail))
			{
				tidx = hidx + lenBytes + dl + tail.size(), run = &scan::BR;
				return SCAN_STATE::JUMP;
			}
		}
		std::copy_backward(buf.begin() + hidx, buf.begin() + n, buf.begin() + (n - hidx));
		std::copy_backward(buf.begin() + n,  buf.end(), buf.end() - hidx);
		n -= hidx, dl = 0, hidx = 0, run = &scan::Head;
		if (n > head.size()) return SCAN_STATE::JUMP;
	}
	return SCAN_STATE::NEXT;
}

SCAN_STATE scan::BR()
{
	if (br.size() > 0)
    {
		if (n < 0 || tidx + br.size() > uint64_t(n)) return SCAN_STATE::NEXT;
		std::vector<unsigned char> tmp(br.size());
		std::copy(buf.begin() + tidx, buf.begin() + tidx + br.size(), tmp.begin());
		// std::cout << "---------BR---------\t" << tidx << "\thead\t" << tmp.data() << std::endl;
		if (!vectorMatch(tmp, br))
        {
			std::copy_backward(buf.begin() + hidx, buf.begin() + n, buf.begin() + (n - hidx));
			std::copy_backward(buf.begin() + n,  buf.end(), buf.end() - hidx);
			n -= hidx, dl = 0, hidx = 0, tidx = 0, run = &scan::Head;
			if (n < head.size()) return SCAN_STATE::NEXT;
			return SCAN_STATE::JUMP;
		}
	}
	return SCAN_STATE::OK;
}
