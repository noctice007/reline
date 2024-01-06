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
  // This tee buffer has no buffer. So every character "overflows"
  // and can be put directly into the teed buffers.
  virtual int overflow(int c);

  // Sync both teed buffers.
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
