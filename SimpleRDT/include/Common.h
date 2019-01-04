#ifndef __COMMON_H__
#define __COMMON_H__

#include <sys/time.h>
#include <list>
#include <string>

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned short ushort;
typedef struct timeval Timestamp_t;

struct SegmentHeader {
  uint seqno;
  ushort checksum;
  ushort identifier;
};

struct Segment{
  SegmentHeader head;
  uchar data;
};

struct Ack{
  uint seqno;
  ushort zero;
  ushort identifier;
  uint nack;
};

struct Packet{
  void *buff;
  uint size;
  Packet(void* b, uint s){ buff = b, size = s; }
  Packet(){buff = NULL; size = 0; }
};

class WindowElement {
  public:
    Segment *seg;
    uint size;
    Timestamp_t timer;
    uint seqno;

    WindowElement(){
      seg = NULL;
      size = 0;
      seqno = 0xFFFFFFFFU;
    }
};

typedef std::list<WindowElement> Window;
typedef std::list<WindowElement>::iterator WinIter;

#define DATA_PACK_IDENT ((ushort)0x5555u)
#define ACK_PACK_IDENT ((ushort)0xAAAAu)

std::string ServerAddress();
void ServerAddress(std::string s);

int ServerPort();
void ServerPort(int p);

void CheckFileWrite(std::string s);
void CheckFileRead(std::string s);
std::string GetFileName();

uint MSS();
void MSS(int m);

uint WindowSize();
void WindowSize(int s);

float LossProb();
void LossProb(float l);

ushort ComputeChecksum(uchar *data, uint size);

void msleep(uint msecs);

ulong diff_time(const Timestamp_t & before, const Timestamp_t &after);

Timestamp_t timestamp();

std::string hostname_to_ip(std::string hostname);

void printTime(Timestamp_t t1, Timestamp_t t2, std::string msg);
#endif //__COMMON_H__
