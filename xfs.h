#pragma once
#include <windows.h>
//#include "sddl.h"
#include "zlibengn.h"
#include "xfsconf.h"
#include <string>
#include <vector>

typedef struct FileInfoBlock_s
{
	char header[0x70];
	unsigned long position;
	unsigned long status;
	unsigned long unpackedSize;
	unsigned long packedSize;
} FileInfoBlock;

class xfs
{
public:
	xfs(void);
	~xfs(void);
	int unpack();
	int unpackInfo();
	int init(Bytef *, char*);

	int decompress(Bytef *,uLongf *, const Bytef *, uLong);

	Bytef *xfsInfo;
	long xfsInfoLen;
	char *OutputDir;
protected:
	Bytef *buff;
	Bytef *pPacketTail;
	Bytef *pPacketInfo;
	Bytef signBlockLen;
};

class Splitter {
  std::vector<std::string> _tokens;
public:
  typedef std::vector<std::string>::size_type size_type;
public:
  Splitter ( const std::string& src, const std::string& delim )
  {
    reset ( src, delim );
  }
  std::string& operator[] ( size_type i )
  {
    return _tokens.at ( i );
  }
  size_type size() const
  {
    return _tokens.size();
  }
  void reset ( const std::string& src, const std::string& delim )
  {
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end;
    for ( ; ; ) {
      end = src.find ( delim, start );
      tokens.push_back ( src.substr ( start, end - start ) );
      if ( end == std::string::npos )
        break;
      start = end + delim.size();
    }
    _tokens.swap ( tokens );
  }
};
