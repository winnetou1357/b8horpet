#include <iostream>
#include <ios>
#include <string>

#include "MD5.hxx"


// Test application for MD5 generator
// works like `md5sum` from GNU coreutils


// write the md5 digest in the usual format, each byte as 2 hexadecimal digits
std::ostream& operator<< (std::ostream& os, const rps::crypto::md5::digest& md5digest)
{
  for (const rps::crypto::md5::byte& digit : md5digest)
  {
    if (digit < 0x10) {
      os << 0;
    }
    os << std::hex << int(digit);
  }
  return os;
}


// should be upgraded to use md5::generator instead, thus avoid reading the whole (probably large) input into memory
int main()
{
  // get the full content of standard input unformatted
  std::string input;
  int ch;
  while ((ch = std::cin.get()) != decltype(std::cin)::traits_type::eof())
    input.push_back(char(ch));

  // generate the md5 digest
  rps::crypto::md5::digest md5digest = rps::crypto::md5::generate(reinterpret_cast<const rps::crypto::md5::byte *>(input.c_str()), input.size());

  std::cout << md5digest << std::endl;

  return 0;
}
