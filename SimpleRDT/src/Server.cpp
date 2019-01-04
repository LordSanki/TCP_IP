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
    cout<<" <filename> <packet loss probabilty>\n";
    return -1;
  }

  ServerPort(     atoi(argv[0]));
  CheckFileWrite(  argv[1]);
  LossProb(       atof(argv[2]));

  cout << "Starting Server with\n"
    <<"Port     : "<<ServerPort()<<"\n"
    <<"OutFile  : "<<GetFileName()<<"\n"
    <<"LossProb : "<<LossProb()<<"\n";
  rdt_recv();

  return 0;
}

void rdt_recv() {

  const float C_loss_prob = LossProb();

  bool loop = true;
  uint seqno = 0;
  uint mss = 0;
  // creating objects
  ServerSocket sock  = ServerSocket(ServerPort());
  FILE    *file    = fopen(GetFileName().c_str(), "wb+");

  srand(time(0));
  do{

    Packet p;
    sock >> p; // wait for packet

    if(p.buff){ // check if packet has data

      Segment *seg = (Segment*)p.buff;
      // randomly loose packets
      // rand()/RAND_MAX should give range of (0,1)
      if( rand()/(float)RAND_MAX > C_loss_prob){
        // check the sequence number and identifier
        if(seg->head.seqno == seqno 
            && seg->head.identifier == DATA_PACK_IDENT){
          // find size of data segment
          uint size = p.size-sizeof(SegmentHeader);
          // verify the checksum of data segment
          if((ushort)0U == ComputeChecksum((uchar*)seg, p.size) ){
            if(mss == 0) mss = p.size;
            // write the data to file
            if(size){
              fwrite(&(seg->data), sizeof(uchar), size, file);
              fflush(file);
            }

            // send ack
            Ack ack;
            ack.seqno = seqno;
            ack.zero = (ushort)0U;
            ack.identifier = ACK_PACK_IDENT;
            Packet pack = Packet(&ack, sizeof(Ack));
            sock << pack;

            // increment seqno
            seqno++;

            // file transfer complete close file and inform user
            if(p.size < mss){
              fclose(file);
              cout << "\n=============================\nFile transfer complete\n";
              seqno = 0;
              file = fopen(GetFileName().c_str(), "wb+");
              cout.flush();
            }
          }
          else{
            //cout <<"Incorrect Checksum SeqNo ="<<seg->head.seqno<<endl;
          }
        }
        else{
          //cerr<<"Invalid SeqNo E"<<seqno<<" G "<<seg->head.seqno<<endl;
          Ack ack;
          ack.seqno = seqno-1;
          ack.zero = (ushort)0U;
          ack.identifier = ACK_PACK_IDENT;
          Packet pack = Packet(&ack, sizeof(Ack));
          sock << pack;
        }
      }
      else{
        cout << "Packet Loss, seuqence number = " << seqno << endl;
      }
      free(p.buff);
    }
  }while(loop);
}

