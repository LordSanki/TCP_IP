#ifndef __P2P_CLIENT_H__
#define __P2P_CLIENT_H__
#include <string>
#include <vector>
#include <algorithm>
#include <include/addmethod.h>
#include <include/listmethod.h>
#include <include/lookupmethod.h>
#include <include/getmethod.h>
#include <include/ClientSocket.h>
#include <include/ServerSocket.h>
class Client {
  public:
    enum ClientCommands{
      LIST_REMOTE,
      LIST_LOCAL,
      LOOKUP,
      ADD,
      GET,
      EXIT
    };
    typedef ClientCommands(*ClientCallback)(std::vector< std::string > & opts);
    struct RFC_Info{
      std::string title;
      std::string rfc;
      std::string filename;
      // construct from internal filename
      RFC_Info(std::string f){
        filename = f;
        unsigned long loc1 = f.find_last_of("/");
        if(loc1 == std::string::npos) loc1 = 0;
        else loc1++;
        std::string f2 = f.substr(loc1, std::string::npos);
        unsigned long loc2 = f2.find_first_of("#");
        unsigned long loc3 = f2.find_last_of(".");
        rfc = f2.substr(0,loc2);
        loc2++;
        title = f2.substr(loc2, loc3-loc2);
        std::replace(title.begin(), title.end(), '_', ' ');
      }
      // construct from rfc num title and user specified filename
      RFC_Info(std::string r, std::string t, std::string f){
        rfc = r; title = t;
        //unsigned long loc2 = f.find_last_of(".");
        std::string f3 = ".txt";
        //if(loc2 != std::string::npos)
        //  f3 = f.substr(loc2, std::string::npos);
        std::replace(t.begin(), t.end(), ' ', '_');
        filename = rfc + std::string("#") + t + f3;
      }

    };
  protected:
    std::vector<RFC_Info> m_local_rfc;
    std::string m_rfc_dir;

  public:
    Client(std::string path);
    void run(ClientCallback func);

  private:
    std::string readDataFromFile(std::string rfc);
    void getResponse(GetMethod &gm);
    void scanForFiles();
    void checkRFCDir();
    void advertizeLocalRFC(ClientSocket &conn);
    void saveDataToFile(std::string data, std::string rfc);
    void addRequest(ClientSocket &conn, RFC_Info r);
    LookupMethod lookupRequest(ClientSocket &conn, std::vector<std::string> &opts);
    void listRequest(ClientSocket &conn);
    void getRequest(ClientSocket &conn, std::vector<std::string> &opts);
    void run_upload_server(ServerSocket &server);
    bool isFilePresent(std::string filename);
    RFC_Info copyFile(std::vector <std::string> &opts);
    void listLocal();
};
#endif //__P2P_CLIENT_H_
