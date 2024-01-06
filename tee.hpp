#ifndef TEE_HPP
#define TEE_HPP
#include <ostream>
#include <streambuf>
class teebuf : public std::streambuf {
public:
  // Construct a streambuf which tees output to both input
  // streambufs.
  teebuf(std::streambuf *sb1, std::streambuf *sb2);

private:
  virtual int overflow(int c);

  virtual int sync();

private:
  std::streambuf *sb1;
  std::streambuf *sb2;
};

class teestream : public std::ostream {
public:
  teestream(std::ostream &o1, std::ostream &o2);

private:
  teebuf tbuf;
};

#endif
