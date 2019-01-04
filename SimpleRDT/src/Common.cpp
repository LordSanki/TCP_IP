#include <include/Common.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;
static string S_serv_addr;
static string S_filename;
static int S_serv_port;
static uint S_mss;
static uint S_win_size;
static float S_loss_prob;

int ServerPort(){
  return S_serv_port;
}
void ServerPort(int p){
  S_serv_port = p;
}

string ServerAddress(){
  return S_serv_addr;
}
void ServerAddress(string s){
  S_serv_addr = s;
}

uint MSS(){
  return S_mss;
}
void MSS(int m){
  S_mss = (uint)m;
}

uint WindowSize(){
  return S_win_size;
}
void WindowSize(int w){
  S_win_size = (uint)w;
}

float LossProb(){
  return S_loss_prob;
}
void LossProb(float l){
  S_loss_prob = l;
}

void CheckFileRead(string s){
  S_filename = s;
  FILE * f= fopen(s.c_str(), "rb");
  if(f != NULL){
    fclose(f);
    return;
  }
  else{
    cerr<<"Unable to open file for reading\n";
    exit(0);
  }
}

void CheckFileWrite(string s){
  S_filename = s;
  FILE * f= fopen(s.c_str(), "wb+");
  if(f != NULL){
    fclose(f);
    return;
  }
  else{
    cerr<<"Unable to open file for writing\n";
    exit(0);
  }
}

string GetFileName(){
  return S_filename;
}

ushort ComputeChecksum(uchar * data, uint size){
  uint sum = 0U; //init sum
  ushort *ptr = (ushort*)data; //cast ptr to 16 bit type
  for(uint i=0; i<size/2; i++){ //loop to size/2 since 16 bits
    sum += ptr[i]; // add 16 bit to sum
    // shifting sum 16 bits left to get carry
    // then adding to lower 16 bits
    // this should do the carry swing without if else
    sum = (sum&0x0000FFFFU) + (sum>>16);
  }
  if(size%2 != 0){
    ushort ts = (ushort)data[size-1];
    sum += ts;
    sum = (sum&0x0000FFFFU) + (sum>>16);
  }
  ushort ret = (ushort)sum;
  return ~ret;
}

void msleep(uint msecs){
  usleep(msecs*1000);
}

ulong diff_time(const Timestamp_t & before, const Timestamp_t &after){
  ulong b = ((ulong)before.tv_sec*1000UL) + before.tv_usec/1000UL;
  ulong a = ((ulong)after.tv_sec*1000UL) + after.tv_usec/1000UL;
  return a-b;
}

Timestamp_t timestamp(){
  Timestamp_t t;
  gettimeofday(&t, NULL);
  return t;
}

string hostname_to_ip(string hostname)
{
  struct hostent *he;
  struct in_addr **addr_list;
  int i;
  if ( (he = gethostbyname( hostname.c_str() ) ) == NULL) 
  {
    return string();
  }

  addr_list = (struct in_addr **) he->h_addr_list;

  for(i = 0; addr_list[i] != NULL; i++) 
  {
    //Return the first one;
    return string( inet_ntoa(*addr_list[i]) );
  }

  return string();
}

void printTime(Timestamp_t t1, Timestamp_t t2, std::string msg){
  unsigned long t = diff_time(t1, t2);
  cout<<msg<<" "<<t<<endl;
}


