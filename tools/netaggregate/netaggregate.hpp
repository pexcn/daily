#ifndef NETAGGREGATE_H
#define NETAGGREGATE_H

#include <iostream>
#include <vector>
#include <algorithm>

void vstrjoin(const std::vector<std::string>& v, char c, std::string& s) {
    s.clear();
    for (auto p = v.begin(); p != v.end(); ++p) {
        s += *p;
        if (p != v.end() - 1)
            s += c;
    }
}

std::string inttoipv6(unsigned __int128 a) {
    unsigned int a1 = (a >> 112);
    unsigned int a2 = (a << 16 >> 112);
    unsigned int a3 = (a << 32 >> 112);
    unsigned int a4 = (a << 48 >> 112);
    unsigned int a5 = (a << 64 >> 112);
    unsigned int a6 = (a << 80 >> 112);
    unsigned int a7 = (a << 96 >> 112);
    unsigned int a8 = (a << 112 >> 112);
    char s[41];
    sprintf(s,"%x:%x:%x:%x:%x:%x:%x:%x",a1,a2,a3,a4,a5,a6,a7,a8);
    std::string str(s);
    return str;
}

std::string inttoipv4(uint32_t a) {
    unsigned int a1 = (a >> 24);
    unsigned int a2 = (a << 8 >> 24);
    unsigned int a3 = (a << 16 >> 24);
    unsigned int a4 = (a << 24 >> 24);
    char s[17];
    sprintf(s,"%u.%u.%u.%u",a1,a2,a3,a4);
    std::string str(s);
    return str;
}

std::string compressipv6(std::string s) {
    size_t pos;
    std::string result;
    std::vector<std::string> patterns {":0:0:0:0:0:0:0", ":0:0:0:0:0:0", ":0:0:0:0:0", ":0:0:0:0", ":0:0:0", ":0:0"};
    for (auto i = patterns.begin(); i != patterns.end(); i++) {
        //std::cout << "pattern is " << *i << std::endl;
        pos = s.find(*i);
        //std::cout << "pos is " << pos << std::endl;
        if (pos != std::string::npos) {
            //std::cout << "found " << *i << std::endl;
            auto count = i->length();
            if (pos != 0) {
                result = s.substr(0,pos) + ":" + s.substr(pos+count, std::string::npos);
            } else {
                result =  ":" + s.substr(count, std::string::npos);
            }
            if ( pos + count == s.length() ) {
                result += ":";
            }
            break;
        }
    }
    if (result.length() == 0 ) {
        result = s;
    }
    return result;
}


/*
std::ostream& operator<<(std::ostream& os, const unsigned __int128 i) noexcept
{
    std::ostream::sentry s(os);
    if (s) {
        unsigned __int128 tmp = i < 0 ? -i : i;
        char buffer[128];
        char *d = std::end(buffer);
        do {
            --d;
            *d = "0123456789"[tmp % 10];
            tmp /= 10;
        } while (tmp != 0);
        if (i < 0) {
            --d;
            *d = '-';
        }
        int len = std::end(buffer) - d;
        if (os.rdbuf()->sputn(d, len) != len) {
            os.setstate(std::ios_base::badbit);
        }
    }
    return os;
}
*/

class IPv6Net {
public:
    IPv6Net(const std::string & = std::string("::"));
    IPv6Net(unsigned __int128, int);
    std::string net() const;
    std::string mask() const;
    std::string laddr();
    unsigned __int128 intfaddr();
    unsigned __int128 intladdr();
    unsigned int intmask();
    bool operator < (const IPv6Net& net) const;


private:
    unsigned __int128 address;
    unsigned __int128 first_address;
    unsigned __int128 last_address;
    unsigned int networklen;
    std::string expanded_address_part;
};



bool IPv6Net::operator<(const IPv6Net& net) const
{
    return (first_address < net.first_address);
}


std::string IPv6Net::net() const
{
    return compressipv6(inttoipv6(first_address));
    //return inttoipv6(first_address);
}

std::string IPv6Net::laddr()
{
    return compressipv6(inttoipv6(last_address));
    //return inttoipv6(last_address);
}

std::string IPv6Net::mask() const
{
    return std::to_string(IPv6Net::networklen);
}

unsigned __int128 IPv6Net::intfaddr()
{
    return first_address;
}

unsigned __int128 IPv6Net::intladdr()
{
    return last_address;
}

unsigned int IPv6Net::intmask()
{
    return networklen;
}

IPv6Net::IPv6Net(unsigned __int128 a, int m)
{
    address = a;
    networklen = m;
    if (networklen == 128) {
        first_address = address;
        last_address = address;
    } else {
        first_address = address>>(128-networklen)<<(128-networklen);
        last_address = address | (((unsigned __int128)1<<(128-networklen))-1);
    }
}

IPv6Net::IPv6Net(const std::string & s)
{
    std::string address_part;
    auto n = s.find("/");
    if ( n != std::string::npos ) {
        address_part = s.substr(0,n);
        networklen = stoi(s.substr(n+1));
    } else {
        address_part = std::string(s);
        networklen = 128;
    }

    //expand ::
    for (int i=0, scc = 0; i<address_part.length();) {
        if (address_part[i] != ':') {
            expanded_address_part += address_part[i];
            i++;
        } else if (address_part[i] == ':' and address_part[i+1] == ':') {
            auto repeat = 6 - scc;
            for(int j = 0; j < repeat; j++) {
                expanded_address_part += ":0";
            }
            expanded_address_part += ":";
            i +=2;
        } else if (address_part[i] == ':' and address_part[i+1] != ':') {
            expanded_address_part += ":";
            scc++;
            i++;
        }
    }
    if (expanded_address_part[expanded_address_part.length()-1] == ':') {
        expanded_address_part += "0";
    }
    if (expanded_address_part[0] == ':') {
        expanded_address_part = "0"+expanded_address_part;
    }

    //expand leading zeroes and convert to __int128
    std::size_t current, previous = 0;
    std::vector<std::string> cont;
    current = expanded_address_part.find(":");
    while (current != std::string::npos) {
        cont.push_back(expanded_address_part.substr(previous, current - previous));
        previous = current + 1;
        current = expanded_address_part.find(":", previous);
    }
    cont.push_back(expanded_address_part.substr(previous, current - previous));
    unsigned __int128 num = 0;
    unsigned int bitsnum = 112;
    address = 0;
    for (auto i = cont.begin();  i != cont.end(); ++i) {
        if ( i->length() < 4 ) {
            auto repeat = 4 - i->length();
            for(int j = 0; j < repeat; j++) {
                *i = "0" + *i;
            }
        }
        num = std::stoi(*i, 0, 16);
        address = address + (num << bitsnum);
        bitsnum -= 16;
    }
    vstrjoin(cont, ':', expanded_address_part);
    if (networklen == 128) {
        first_address = address;
        last_address = address;
    } else {
        first_address = address>>(128-networklen)<<(128-networklen);
        last_address = address | (((unsigned __int128)1<<(128-networklen))-1);
    }
}

template <class MyNet>
std::vector<MyNet> MergeNets(std::vector<MyNet> orig) {
    std::vector<MyNet> mnets {};
    bool to_merge = true;
    while (to_merge) {
        //std::cout << "merge iteration " << std::endl;
        mnets.clear();
        to_merge = false;
        size_t rl = orig.size();
        size_t r = 0;
        for ( auto i = orig.begin(); r < rl; ) {
            if ( (i+1) == orig.end() ) {
                //std::cout << "last element " << std::endl;
                mnets.push_back(*i);
                break;
            }
            if ( i->intmask() == (i+1)->intmask()) {
                //std::cout << "merging " << i->net() << " and " << (i+1)->net()  << std::endl;
                auto mnet = MyNet(i->intfaddr(), i->intmask() - 1);
                if ( mnet.intfaddr() == i->intfaddr() && mnet.intladdr() == (i+1)->intladdr() ) {
                    mnets.push_back(mnet);
                    to_merge=true;
                    i += 2;
                    r += 2;
                } else {
                    mnets.push_back(*i);
                    i ++;
                    r ++;
                }
            } else {
                //std::cout << "not merging" << std::endl;
                auto mnet = MyNet(i->intfaddr(), i->intmask());
                mnets.push_back(mnet);
                i++;
                r++;
            }
        }
        orig = mnets;
    }
    return mnets;
}


template <class MyNet>
std::vector<MyNet> CollapsNets(std::vector<MyNet> orig) {
    std::vector<MyNet> mnets {};
    bool to_collaps = true;
    while (to_collaps) {
        //std::cout << "collapsing iteration " << std::endl;
        mnets.clear();
        to_collaps = false;
        //std::cout << "befor for " << std::endl;
        //for ( auto i=orig.begin(); i !=orig.end(); i++) {
        //    std::cout << i->net() << "/" << i->intmask() << std::endl;
        //}
        //std::cout << "orig lenght is " << orig.size() << std::endl;
        size_t rl = orig.size();
        size_t r = 0;
        for ( auto i = orig.begin(); r < rl ;  ) {
            //std::cout << "working on " << i->net() << "/" << i->mask() << " and " << (i+1)->net() << "/" << (i+1)->mask() << std::endl;
            if ( (i+1) == orig.end() ) {
                //std::cout << "last element " << std::endl;
                auto mnet = MyNet(i->intfaddr(), i->intmask());
                //std::cout << "added " << mnet.net() << "/" << mnet.mask() << std::endl;
                mnets.push_back(mnet);
                break;
            }
            if (i->intfaddr() <= (i+1)->intfaddr() && i->intladdr() >= (i+1)->intladdr() ) {
                //std::cout << "lr collapsing " << i->net() << " and " << (i+1)->net()  << std::endl;
                auto mnet = MyNet(i->intfaddr(), i->intmask());
                //std::cout << "added " << mnet.net() << "/" << mnet.mask() << std::endl;
                mnets.push_back(mnet);
                to_collaps=true;
                i += 2;
                r += 2;
            } else if ( (i+1)->intfaddr() <= i->intfaddr() && (i+1)->intladdr() >= i->intladdr() ) {
                //std::cout << "rl collapsing " << (i+1)->net() << " and " << i->net() << std::endl;
                auto mnet = MyNet((i+1)->intfaddr(), (i+1)->intmask());
                //std::cout << "added " << mnet.net() << "/" << mnet.mask() << std::endl;
                mnets.push_back(mnet);
                to_collaps=true;
                i += 2;
                r += 2;
            } else {
                //std::cout << "not collapsing " << i->net() << " and " << (i+1)->net()  << std::endl;
                auto mnet = MyNet(i->intfaddr(), i->intmask());
                //std::cout << "added " << mnet.net() << "/" << mnet.mask() << std::endl;
                mnets.push_back(mnet);
                i++;
                r++;
            }
        }
        orig = mnets;
        //std::cout << "after iteration " << std::endl;
        //for ( auto i=orig.begin(); i !=orig.end(); i++) {
        //    std::cout << i->net() << "/" << i->intmask() << std::endl;
        //}
    }
    return mnets;
}

class IPv4Net {
public:
    IPv4Net(const std::string & = std::string("0.0.0.0"));
    IPv4Net(uint32_t, unsigned int);
    std::string net() const;
    std::string mask() const;
    std::string laddr();
    uint32_t intfaddr();
    uint32_t intladdr();
    unsigned int intmask();
    bool operator < (const IPv4Net& net) const;


private:
    uint32_t address;
    uint32_t first_address;
    uint32_t last_address;
    unsigned int networklen;
};



IPv4Net::IPv4Net(const std::string& s)
{
        std::string address_part;
    auto n = s.find("/");
    if ( n != std::string::npos ) {
        address_part = s.substr(0,n);
        networklen = std::stoi(s.substr(n+1));
    } else {
        address_part = std::string(s);
        networklen = 32;
    }
    

    std::size_t current, previous = 0;
    std::vector<std::string> cont;
    current = address_part.find(".");
    while (current != std::string::npos) {
        cont.push_back(address_part.substr(previous, current - previous));
        previous = current + 1;
        current = address_part.find(".", previous);
    }
    cont.push_back(address_part.substr(previous, current - previous));
    uint32_t num = 0;
    unsigned int bitsnum = 24;
    address = 0;
    for (auto i = cont.begin();  i != cont.end(); ++i) {
           num = std::stoi(*i);
        address = address + (num << bitsnum);
        bitsnum -= 8;
    }
    if (networklen == 32) {
        first_address = address;
        last_address = address;
    } else {
        first_address = address>>(32-networklen)<<(32-networklen);
        last_address = address | (((uint32_t)1<<(32-networklen))-1);
    }
}

IPv4Net::IPv4Net(uint32_t a, unsigned int m)
{
    address = a;
    networklen = m;
    if (networklen == 32) {
        first_address = address;
        last_address = address;
    } else {
        first_address = address>>(32-networklen)<<(32-networklen);
        last_address = address | (((uint32_t)1<<(32-networklen))-1);
    }
}

std::string IPv4Net::net() const
{
    return inttoipv4(first_address);
}

std::string IPv4Net::mask() const
{
    return std::to_string(IPv4Net::networklen);
}

bool IPv4Net::operator<(const IPv4Net& net) const
{
    return (first_address < net.first_address);
}

uint32_t IPv4Net::intfaddr()
{
    return first_address;
}

uint32_t IPv4Net::intladdr()
{
    return last_address;
}

unsigned int IPv4Net::intmask()
{
    return networklen;
}

#endif //NETAGGREGATE_H
