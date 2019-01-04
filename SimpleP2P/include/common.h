#ifndef __COMMON_H__
#define __COMMON_H__

#define SERVER_PORT 7734
#define DB_SERVER_ADDRESS "localhost"
#if 1
#include <iostream>
#define DBG1(X) cerr << __LINE__ <<":"<<#X<<" :: "<<X<<endl;
#define DBG2(X) cerr << __LINE__ <<" :: "<<X<<endl;
#else
#define DBG1(X) 
#define DBG2(X) 
#endif
#include <vector>
#include <string>
std::string getLocalOS();

std::string getLocalHost();

void setLocalHost(std:: string h);

std::string getLocalVersion();

int server_port();

int db_server_port();
void db_server_port(int p);

std::string server_address();

void server_address(std::string s);

std::string db_server_address();

int uploadPortNo();
void uploadPortNo(int port);
void print_reply(std::string s);

void print_req(std::string s);

std::string trim(const std::string& str);

std::string combine_vector(const std::vector <std::string>& v, int start);

bool copyFileOnDisk(std::string s, std::string d);

std::string getLocalDateTime();

std::vector<std::string> hostname_to_ip(std::string hostname);

#endif //__COMMON_H__
