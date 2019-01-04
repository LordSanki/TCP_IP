#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <include/Common.h>
#include <include/Socket.h>

using namespace std;

// timeout in milli seconds
static ulong S_Timeout = 100;
static ulong S_npackets = 0;
static ulong S_data = 0;

void rdt_send();

int main(int argc, char** argv)
{
  argc-=(argc>0); argv+=(argc>0); // skip program name argv[0] if present

  int times = 1;
  
  if( argc < 5 ){
    cout << "Usage:\n SimpleFTP_Client <server address>";
    cout<<" <server port> <filename> <WinSize> <MSS>\n";
    cout<<"optional arguments -n <iterations>  -t <timeout>\n";
    return -1;
  }

  if(argc > 5){
    for(int i=6; i<argc; i++){
      if(string(argv[i-1]) == "-n" && argc >6){
        times = atoi(argv[i]);
        i++;
        if(times < 1) times = 1;
      }
      if(string(argv[i-1]) == "-t" && argc >6){
        S_Timeout = atoi(argv[i]);
        i++;
        if(S_Timeout < 1) S_Timeout = 10;
      }
    }
  }
  ServerAddress(  argv[0]);
  ServerPort(     atoi(argv[1]));
  CheckFileRead(  argv[2]);
  WindowSize(     atoi(argv[3]));
  MSS(            atoi(argv[4]));

  cout << "Starting FTP Client with\n"
    <<"Server     : "<<ServerAddress()<<"\n"
    <<"Port       : "<<ServerPort() <<"\n"
    <<"InputFile  : "<<GetFileName()<<"\n"
    <<"WindowSize : "<<WindowSize()<<"\n"
    <<"MSS        : "<<MSS()<<"\n"
    <<"Iterations : "<<times<<"\n"
    <<"Timeout    : "<<S_Timeout<<"\n";

  Timestamp_t t0 = timestamp();
  ulong npack = 0;
  ulong ndata = 0;
  for(int i=0; i<times; i++){
    Timestamp_t t1 = timestamp();
    S_npackets = 0;
    S_data = 0;
    rdt_send();
    npack += S_npackets;
    ndata += S_data;
    ulong diff = diff_time(t1, timestamp());
    cout <<"\n===============================\n"
      <<"IterTime  : "<<diff/1000.0<<" secs\n"
      <<"Tx Rate   : "<<((S_data*MSS())/(float)diff)/1.024<<" KB/s\n"
      <<"Pack Rate : "<<(S_npackets/(float)diff)*1000<<" Packets/s\n"
      <<"PTx Rate  : "<<((S_npackets*MSS())/(float)diff)/1.024<<" KB/s\n"
      <<"\n===============================\n";
  }
  ulong diff = diff_time(t0, timestamp());
  cout <<"\n===============================\n"
    <<"AvgTime   : "<<diff/(1000.0*times)<<" secs\n"
    <<"Tx Rate   : "<<((ndata*MSS())/(float)diff)/1.024<<" KB/s\n"
    <<"Pack Rate : "<<(npack/(float)diff)*1000<<" Packets/s\n"
    <<"PTx Rate  : "<<((npack*MSS())/(float)diff)/1.024<<" KB/s\n"
    <<"\n===============================\n";

  return 0;
}


void rdt_send() {

  const uint C_mss = MSS();
  const uint C_win_size = WindowSize();

  bool loop = true;
  bool read_file = true;
  uint seqno = 0;
  uint win_pos = 0;

  // creating objects
  ClientSocket sock  = ClientSocket(ServerAddress(), ServerPort());
  FILE    *file    = fopen(GetFileName().c_str(), "rb");
  Window  *window   = new Window();

  do{

    /******** look for timeouts ***********************************************/
    if(!window->empty() && 
        diff_time(window->front().timer, timestamp()) > S_Timeout){
      cout<<"Timeout, sequence number = "<<window->front().seg->head.seqno<<endl;
      for(WinIter it = window->begin(); it != window->end(); it++){
        //cout<<"Timeout, sequence number = "<<(it->seg)->head.seqno<<endl;
        S_npackets++;
        Packet pt;
        pt.buff = (void*)it->seg;
        pt.size = it->size;
        sock << pt;
        it->timer = timestamp();
      }
    }
    /**************************************************************************/

    /******** check for acks **************************************************/

    Packet pack;
    sock >> pack; // do a non blocking receive
    while(pack.buff){
      Ack *ack = (Ack*)pack.buff;// convert to Ack structure to get seq no
      // check if window has elements and seq no belons to window
      if(!window->empty() && ack->seqno >= window->front().seg->head.seqno){
        // find out number of packets to remove
        if(ack->seqno <= window->back().seg->head.seqno){
          uint num_elem = ack->seqno - window->front().seg->head.seqno;
          // remove the packets from window
          for(uint i=0; i<=num_elem; i++){
            free(window->front().seg);
            window->pop_front();
          }
        }
      }
      // free memory
      free(pack.buff);
      // try to get another ack
      sock >> pack;
    }
    /**************************************************************************/

    /******** check the window size *******************************************/
    if(window->size() >= C_win_size){
      //msleep(1);
      continue;
    }
    /**************************************************************************/
    uint bytes_read = 0;

    /******** Read file fill in segment and send ******************************/
    if(read_file){
      uchar   *buffer  = (uchar*)malloc(sizeof(SegmentHeader) + C_mss);
      Segment *segment = (Segment*)buffer;

      // reading max mss bytes from file
      bytes_read = fread(&(segment->data), sizeof(uchar), C_mss, file);

      /******** Fill in the Header section inc seqno **************************/
      segment->head.seqno      = seqno++;
      segment->head.identifier = DATA_PACK_IDENT;
      segment->head.checksum   = 0U;
      segment->head.checksum   = ComputeChecksum((uchar*)segment, sizeof(SegmentHeader) + bytes_read);
      /******** push segment into window **************************************/
      WindowElement wine;
      wine.seg = segment;
      wine.timer = timestamp();
      wine.size = bytes_read+sizeof(SegmentHeader);
      window->push_back(wine);

      /******** send the packet ***********************************************/
      Packet p = Packet(segment, bytes_read+sizeof(SegmentHeader));
      sock << p;
      S_npackets++;
      S_data++;
      /******** close file if less than mss bytes read ************************/
      if(bytes_read < C_mss){
        read_file = false;
        fclose(file);
      }
    }
    else{
      if(window->empty()){
        loop = false;
      }
    }
  }while(loop);
  delete window;
  cout<<"\nFile transmission completed\n";
}

