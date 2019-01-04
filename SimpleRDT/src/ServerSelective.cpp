#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <include/Common.h>
#include <include/Socket.h>

using namespace std;

void rdt_recv();


int main(int argc, char** argv)
{
  argc-=(argc>0); argv+=(argc>0); // skip program name argv[0] if present

  if( argc < 3 ){
    cout << "Usage:\n SimpleFTP_Server <listen port>";
    cout<<" <filename> <packet loss probabilty> <window>\n";
    return -1;
  }

  ServerPort(     atoi(argv[0]));
  CheckFileWrite(  argv[1]);
  LossProb(       atof(argv[2]));
  WindowSize(     atoi(argv[3]));

  cout << "Starting Server with\n"
    <<"Port     : "<<ServerPort()<<"\n"
    <<"OutFile  : "<<GetFileName()<<"\n"
    <<"LossProb : "<<LossProb()<<"\n"
    <<"window   : "<<WindowSize()<<"\n";
  rdt_recv();

  return 0;
}

void rdt_recv() {

  const float C_loss_prob = LossProb();
  const uint C_winsize = WindowSize();
  bool loop = true;
  uint seqno = 0;
  uint mss = 0;
  // creating objects
  ServerSocket sock  = ServerSocket(ServerPort());
  FILE    *file    = fopen(GetFileName().c_str(), "wb+");
  Window *window = new Window();
  srand(time(0));
  do{

    Packet p;
    sock >> p; // wait for packet

    if(p.buff){ // check if packet has data

      Segment *seg = (Segment*)p.buff;
      // randomly loose packets
      // rand()/RAND_MAX should give range of (0,1)
      if( rand()/(float)RAND_MAX > C_loss_prob){

        // check if seqnum will fit in the window and the identifier
        if(seg->head.seqno < (seqno + C_winsize)
            && seg->head.seqno >= seqno
            && seg->head.identifier == DATA_PACK_IDENT){

          // verify the checksum of data segment
          if((ushort)0U == ComputeChecksum((uchar*)seg, p.size) ){

            Ack ack;
            ack.seqno = seg->head.seqno;
            ack.zero = (ushort)0xFFFFU;
            ack.identifier = ACK_PACK_IDENT;

            // received packet is not the first in wondow
            // we need to push it into window
            if(seg->head.seqno > seqno) {

              uint diff = seg->head.seqno - seqno; // find diffenrence in seqno
              
              // the current list is large enough to store this packet
              if(window->size() > diff){ 
                WinIter it = window->begin();
                std::advance(it, diff); // going to posistion
                if(it->seg == NULL){ // checking if packet is filled
                  it->seg = seg; // consuming packet
                  it->size = p.size;
                  it->seqno = seg->head.seqno;
                }
              }
              else{ // list is too small
                while(window->size() <= diff){ // fill list untill size > diff
                  window->push_back(WindowElement());
                }
                window->back().seg = seg; // add segment to back of list
                window->back().size = p.size;
                window->back().seqno = seg->head.seqno;
              }

            } // current packet is not first in window

            if(seg->head.seqno == seqno) { //current packet is first in window
              if(window->empty()) window->push_back(WindowElement());
              window->front().seg = seg;
              window->front().size = p.size;
              window->front().seqno = seg->head.seqno;
              WinIter it = window->begin();
              while( it != window->end() && it->seg ){
                uint data_size = it->size - sizeof(SegmentHeader);
                if(data_size > 0){
                  fwrite(&((it->seg)->data), sizeof(uchar), data_size, file);
                  fflush(file);
                }
                seqno++;
                free(it->seg);
                window->pop_front();
                it = window->begin();
              }
            }//current packet is first in window
            
            // send ack for this packet
            ack.nack = seqno;
            Packet pack = Packet(&ack, sizeof(Ack));
            sock << pack;

            /*
            // file transfer complete close file and inform user
            if(p.size < mss){
              fclose(file);
              cout << "\n=============================\nFile transfer complete\n";
              seqno = 0;
              file = fopen(GetFileName().c_str(), "ab+");
              cout.flush();
            } // file transfer complete
            */
            p.buff = NULL;

          } // correct checksum
        } // seqno within window
        else{
            Ack ack;
            ack.seqno = seqno-1;
            ack.zero = (ushort)0xFFFFU;
            ack.identifier = ACK_PACK_IDENT;
            ack.nack = seqno;
            Packet pt = Packet(&ack, sizeof(Ack));
            sock<<pt;
        }
      } // random drop
      else{
        cout << "Packet Loss, seuqence number = " << seqno << endl;
      }

      if(p.buff) free(p.buff);

    } // valid packet ie p.buff != NULL
  }while(loop);
}

