#include <include/server.h>
#include <include/client.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <include/common.h>
#include <include/editline.h>
#include <algorithm>

#include <signal.h>
#include <sys/prctl.h>

#include <include/addmethod.h>
#include <include/listmethod.h>
#include <include/lookupmethod.h>
#include <include/getmethod.h>

#include <include/optionparser.h>

using namespace std;


struct Arg: public option::Arg
{
  static void printError(const char* msg1, const option::Option& opt, const char* msg2)
  {
    fprintf(stderr, "%s", msg1);
    fwrite(opt.name, opt.namelen, 1, stderr);
    fprintf(stderr, "%s", msg2);
  }
  static option::ArgStatus NonEmpty(const option::Option& option, bool msg)
  {
    if (option.arg != 0 && option.arg[0] != 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires a non-empty argument\n");
    return option::ARG_ILLEGAL;
  }
};

enum  optionIndex { SERVER, CLIENT, PATH, ADDRESS, HELP, UNKNOWN };
const option::Descriptor usage[] =
{
 {UNKNOWN, 0, "", "",option::Arg::None, "USAGE: example [options]\n\n"
                                        "Options:" },
 {HELP, 0,"h", "help",option::Arg::None, "  --help  \tPrint usage and exit." },
 {SERVER, 0,"s","server",option::Arg::None, "  --server, -s  \tServer Mode" },
 {CLIENT, 0,"c","client",option::Arg::None, "  --client, -c  \tClient Mode" },
 {ADDRESS, 0,"a","addr",Arg::NonEmpty, "  --addr, -a  \tAddress of server. Used with client mode" },
 {PATH, 0,"p","path",Arg::NonEmpty, "  --path, -p  \tDirectory to store rfc files. Used with client mode" },
 {0,0,0,0,0,0}
};

static string read_string(string prompt, bool lower=true){
  char * raw = NULL;
  raw = readline(prompt.c_str());
  string input = string(raw);
  free(raw);
  if(lower){
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
  }
  return trim(input);
}

Client::ClientCommands user_interface (std::vector<std::string> & opts) {
  std::string input;
  bool loop = true;
  do {
    cout<<"\n";
    cout<<"=======================Commands=====================\n";
    cout<<"~~  list | lookup | add | get | local | exit/q/x  ~~\n";
    cout<<"=======================:::::::::====================\n";

    input = read_string("Enter >");
    unsigned long l = input.find_first_of(" ");
    if(l < input.length()){
      input = input.substr(0,l);
    }
    if(input == "list"){
      opts.clear();
      return Client::LIST_REMOTE;
    }
    if(input == "lookup"){
      opts.clear();
      opts.push_back(read_string("Enter RFC >"));
      opts.push_back(read_string("Enter Title(optional) >", false));
      return Client::LOOKUP;
    }
    if(input == "get"){
      opts.clear();
      opts.push_back(read_string("Enter RFC >"));
      return Client::GET;
    }
    if(input == "add"){
      opts.clear();
      opts.push_back(read_string("Enter RFC >"));
      opts.push_back(read_string("Enter Title >", false));
      opts.push_back(read_string("Enter Path >", false));
      return Client::ADD;
    }
    if(input == "local"){
      opts.clear();
      return Client::LIST_LOCAL;
    }
    if(input == "exit" || input == "x" || input == "q"){
      loop = false;
    }
  }while(loop);
  return Client::EXIT;
}

int main(int argc, char ** argv) {

  // signal kernel not to keep any zombie proceses and reap them on exit
  signal(SIGCHLD, SIG_IGN);
  prctl(PR_SET_PDEATHSIG, SIGKILL);
  argc-=(argc>0); argv+=(argc>0); // skip program name argv[0] if present
  option::Stats  stats(usage, argc, argv);
  std::vector<option::Option> options(stats.options_max);
  std::vector<option::Option> buffer(stats.buffer_max);
  option::Parser parse(usage, argc, argv, &options[0], &buffer[0]);

  if (parse.error())
    return 1;

  if (options[HELP] || argc == 0) {
    option::printUsage(std::cout, usage);
    return 0;
  }

  if(options[CLIENT].count() > 0){
    char * h;
    if(options[ADDRESS].count()){
      server_address(options[ADDRESS].arg);
    }
    string dir;
    if(options[PATH].count()){
      dir = options[PATH].arg;
    }
    else{
      h = getenv("HOME");
      if(h == NULL){
        dir = "./p2p_rfc/";
      }
      else{
        dir = string(h);
        dir += "/.p2p_rfc/";
      }
    }
    Client cl = Client(dir);
    cl.run(user_interface);
  }
  else if(options[SERVER].count() > 0){
    Server sv = Server();
    sv.run();
  }
  return 0;
}

