#ifndef DNS_H
#define DNS_H
#include <stdio.h>
#include <stdlib.h>

#define T_A 1 //Ipv4 address
#define IN 1 // Internet
#define T_NS 2 // NS
#define T_AAAA 28 // Ipv6
#define T_CNAME 5 // canonical name
#define T_SOA 6 // SOA
#define T_PTR 12

//Constant sized fields of the resource record structure
#pragma pack(push, 1)
struct R_DATA
{
	unsigned short type;
	unsigned short _class;
	unsigned int ttl;
	unsigned short data_len;
};
#pragma pack(pop)

struct in_addr ipv4_addr;
struct in6_addr ipv6_addr;
char add[100]; // reversed address
char expandedIPv6[60];

//Pointers to resource record contents
struct RES_RECORD
{
	unsigned char *name;
	struct R_DATA *resource;
	unsigned char *rdata;
};


//DNS header structure
struct DNS_HEADER
{
	unsigned short id; // identification number

	unsigned char rd :1; // recursion desired
	unsigned char tc :1; // truncated message
	unsigned char aa :1; // authoritive answer
	unsigned char opcode :4; // purpose of message
	unsigned char qr :1; // query/response flag

	unsigned char rcode :4; // response code
	unsigned char cd :1; // checking disabled
	unsigned char ad :1; // authenticated data
	unsigned char z :1; // its z! reserved
	unsigned char ra :1; // recursion available

	unsigned short q_count; // number of question entries
	unsigned short ans_count; // number of answer entries
	unsigned short auth_count; // number of authority entries
	unsigned short add_count; // number of resource entries
};

struct Params {
	bool recursion = false;
	int Q_type = T_A;
	unsigned int port = 53;
	bool reversed = false;
	char hostname[100];
	char* dns_server;
};

//Constant sized fields of query structure
struct QUESTION
{
	unsigned short qtype;
	unsigned short qclass;
};

//Structure of a Query
typedef struct
{
	unsigned char *name;
	struct QUESTION *ques;
} QUERY;

void query(Params *params);
void ChangetoDnsNameFormat (unsigned char* dns,char* host);
unsigned char* ReadName (unsigned char* reader,unsigned char* buffer,int* count);

void getArgs(int argc, char *argv[], Params *params);
char* reversedFormat(char* host);
void expandIPv6(const char* compressedIPv6, char* expandedIPv6, size_t expandedIPv6Size);

void printAnswers(RES_RECORD answers[],Params *params, int i,sockaddr_in a,in_addr,in6_addr);
void printQuesions(Params *params);
void printInfo(DNS_HEADER *dns);

#endif