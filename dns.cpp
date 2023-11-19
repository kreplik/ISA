#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include <netdb.h>
#include <iostream>
#include "dns.hpp"

using namespace std;

int main( int argc , char *argv[])
{
	// init Params structure
	Params p;
	Params *params = &p;

	// get command line arguments
	getArgs(argc,argv,params);

	// perform DNS query
	query(params);

	return 0;
}

// get command line arguments
// set arguments to Params structure
void getArgs(int argc, char *argv[], Params *params)
{
	
	for(int i = 1; i < argc; i++)
	{
		const char* argument = argv[i]; // represents single argument
		bool cont = true; // continue arg parsing
		
		// set port number to Params structure
		if(strcmp(argument,"-h") == 0)
		{
			printf("dns [-r] [-x] [-6] -s server [-p port] adresa\n");
			exit(0);
		}

		if(strcmp(argument,"-p") == 0)
		{
			int index = i;
			if((params->port = atoi(argv[++index])) == 0)
			{
				fprintf(stderr,"Port is not a number\n");
				exit(1);
			}

			i++;
			cont = false;
		}

		// allow reversed question
		if(strcmp(argument,"-x") == 0)
		{
			// set Params items
			params->reversed = true;
			params->Q_type = T_PTR;
			cont = false;
		}

		// allow AAAA question
		if(strcmp(argument,"-6") == 0)
		{
			params->Q_type = T_AAAA;
			cont = false;
		}

		// allow recursion
		if(strcmp(argument,"-r") == 0)
		{
			params->recursion = true;
			cont = false;
		}

		// get domain name
		if(strcmp(argument,"-s") == 0)
		{
			int index = i;
			// get domain name after -s option
			params->dns_server = argv[++index];
			
			i++;
			cont = false;
			
		}
		// other option represents address
		else if(cont){
			strcpy(params->hostname,argv[i]);
		}
	}
	return;
}



// Perform a DNS query by sending a packet
void query(Params *params)
{
	unsigned char buf[65536]; // represents message
	unsigned char *qname,*reader;
	int i , j , stop;
	int s; // socket descriptor

	struct sockaddr_in a; // for printing answer's elements

	struct RES_RECORD answers[20],auth[20],addit[20]; //the replies from the DNS server
	struct sockaddr_in dest; // server's address

	struct DNS_HEADER *dns = NULL; // DNS query header
	struct QUESTION *qinfo = NULL; // Question structure

	// creating socket for UDP packet
	s = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP);

	// get server's address passed in command line
	struct hostent *host_info = gethostbyname(params->dns_server);
	if (host_info == NULL) {
        perror("gethostbyname failed");
    }
	struct in_addr **addr_list = reinterpret_cast<struct in_addr **>(host_info->h_addr_list);

	// set server's address
	dest.sin_family = AF_INET;
	dest.sin_port = htons(params->port);
	dest.sin_addr.s_addr = inet_addr(inet_ntoa(*addr_list[0])); //dns servers

	//Set the DNS structure to standard queries
	dns = (struct DNS_HEADER *)&buf;

	dns->id = (unsigned short) htons(getpid()); // id
	dns->qr = 0;
	dns->opcode = 0;
	dns->aa = 0; //Not Authoritative
	dns->tc = 0; //This message is not truncated
	// set recursion
	if(params->recursion)
	{
		dns->rd = 1; //Recursion Desired
	}
	else{
		dns->rd = 0;
	}
	dns->ra = 0; //Recursion avalibility
	dns->z = 0;
	dns->ad = 0;
	dns->cd = 0;
	dns->rcode = 0;
	dns->q_count = htons(1); // only 1 question
	dns->ans_count = 0;
	dns->auth_count = 0;
	dns->add_count = 0;

	//point to the query portion
	qname =(unsigned char*)&buf[sizeof(struct DNS_HEADER)];

	// If recursion desired
	if(params->reversed){

		// IPv4 reversed address	
		ChangetoDnsNameFormat(qname , reversedFormat(params->hostname));
	}
	else{
		ChangetoDnsNameFormat(qname , params->hostname);
	}

	// set question message
	qinfo =(struct QUESTION*)&buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1)];

	qinfo->qtype = htons(params->Q_type); // question type according to command line option
	qinfo->qclass = htons(IN); // Internet


	// send question to servers
	if( sendto(s,(char*)buf,sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION),0,(struct sockaddr*)&dest,sizeof(dest)) < 0)
	{
		fprintf(stderr,"sendto failed");
	}
	
	//Receive the answer from server
	i = sizeof dest;
	if(recvfrom (s,(char*)buf , 65536 , 0 , (struct sockaddr*)&dest , (socklen_t*)&i ) < 0)
	{
		fprintf(stderr,"recvfrom failed");
	}

	// check recieved DNS header
	dns = (struct DNS_HEADER*) buf;
	
	//Skip DNS Header and query part
	reader = &buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION)];
	
	// Print info about server's answer
	printf("Authoritative: ");
	printInfo(dns);

	// Questions
	printf("Question section (%d)\n",1);
	printQuesions(params);

	//Start reading answers
	stop=0;

	for(i=0;i<ntohs(dns->ans_count);i++)
	{
		answers[i].name=ReadName(reader,buf,&stop);
		reader = reader + stop;

		answers[i].resource = (struct R_DATA*)(reader);
		reader = reader + sizeof(struct R_DATA);

		if(ntohs(answers[i].resource->type) == 1) //if its an ipv4 address
		{
			answers[i].rdata = (unsigned char*)malloc(ntohs(answers[i].resource->data_len));

			for(j=0 ; j<ntohs(answers[i].resource->data_len) ; j++)
			{
				answers[i].rdata[j]=reader[j];
			}

			answers[i].rdata[ntohs(answers[i].resource->data_len)] = '\0';

			reader = reader + ntohs(answers[i].resource->data_len);
		}
		else
		{
			answers[i].rdata = ReadName(reader,buf,&stop);
			reader = reader + stop;
		}
	}

	//read authorities
	for(i=0;i<ntohs(dns->auth_count);i++)
	{
		auth[i].name=ReadName(reader,buf,&stop);
		reader+=stop;

		auth[i].resource=(struct R_DATA*)(reader);
		reader+=sizeof(struct R_DATA);

		auth[i].rdata=ReadName(reader,buf,&stop);
		reader+=stop;
	}
	
	//read additional
	for(i=0;i<ntohs(dns->add_count);i++)
	{
		addit[i].name=ReadName(reader,buf,&stop);
		reader+=stop;

		addit[i].resource=(struct R_DATA*)(reader);
		reader+=sizeof(struct R_DATA);

		if(ntohs(addit[i].resource->type)==1)
		{
			addit[i].rdata = (unsigned char*)malloc(ntohs(addit[i].resource->data_len));
			for(j=0;j<ntohs(addit[i].resource->data_len);j++)
			addit[i].rdata[j]=reader[j];

			addit[i].rdata[ntohs(addit[i].resource->data_len)]='\0';
			reader+=ntohs(addit[i].resource->data_len);
		}
		else
		{
			addit[i].rdata=ReadName(reader,buf,&stop);
			reader+=stop;
		}
	}

	// Prints answers section
	printf("Answer section (%d)\n" , ntohs(dns->ans_count) );
	for(i=0 ; i < ntohs(dns->ans_count) ; i++)
	{
		printAnswers(answers,params,i,a,ipv4_addr,ipv6_addr);
	}

	//Prints authority section
	printf("Authority section (%d)\n" , ntohs(dns->auth_count) );
	for( i=0 ; i < ntohs(dns->auth_count) ; i++)
	{
		
		printAnswers(auth,params,i,a,ipv4_addr,ipv6_addr);
	}

	//Prints additional section
	printf("Additional section (%d)\n" , ntohs(dns->add_count) );
	for(i=0; i < ntohs(dns->add_count) ; i++)
	{
		printAnswers(addit,params,i,a,ipv4_addr,ipv6_addr);
	}
	close(s); // close socket
	return;
}

/*
 * 
 * */
u_char* ReadName(unsigned char* reader,unsigned char* buffer,int* count)
{
	unsigned char *name;
	unsigned int p=0,jumped=0,offset;
	int i , j;

	*count = 1;
	name = (unsigned char*)malloc(256);

	name[0]='\0';

	//read the names in DNS format
	while(*reader!=0)
	{
		if(*reader>=192)
		{
			offset = (*reader)*256 + *(reader+1) - 49152; //49152 = 11000000 00000000 ;)
			reader = buffer + offset - 1;
			jumped = 1; //we have jumped to another location so counting wont go up!
		}
		else
		{
			name[p++]=*reader;
		}

		reader = reader+1;

		if(jumped==0)
		{
			*count = *count + 1; //if we havent jumped to another location then we can count up
		}
	}

	name[p]='\0'; //string complete
	if(jumped==1)
	{
		*count = *count + 1; //number of steps we actually moved forward in the packet
	}

	//now convert 3www6google3com0 to www.google.com
	for(i=0;i<(int)strlen((const char*)name);i++) 
	{
		p=name[i];
		for(j=0;j<(int)p;j++) 
		{
			name[i]=name[i+1];
			i=i+1;
		}
		name[i]='.';
	}
	name[i-1]='\0'; //remove the last dot
	return name;
}

// This will convert address to DNS format
void ChangetoDnsNameFormat(unsigned char* dns,char* host) 
{
	int lock = 0 , i;
	strcat((char*)host,".");
	
	for(i = 0 ; i < strlen((char*)host) ; i++) 
	{
		if(host[i]=='.') 
		{
			*dns++ = i-lock;
			for(;lock<i;lock++) 
			{
				*dns++=host[lock];
			}
			lock++; //or lock=i+1;
		}
	}
	*dns++='\0';
}

// get IPv4 reversed format
char* reversedFormat(char* host)
{
	char *gfg = host; // get host name

    const char s[4] = "."; // s represents delimeter
    char* tok; // hostname's token
	
	char* tmp[100]; // array for reversing tokens
   
    // get first token
    tok = strtok(gfg, s);
	strcpy(add,""); // copy string one into the result.
    
	int i = 0;
	int cnt = 0;
    while (tok != 0) {
		// hold tokenized hostname
		tmp[i] = tok;
		cnt++;
		i++;
        // go through other tokens
        tok = strtok(0, s);
    }
	// now store each tokens in reverse
	i = cnt - 1;
	while(i >= 0)
	{
		strcat(add,tmp[i]);
		// split tokens with dot '.'
		strcat(add,".");
		i--;
	}
	// append "in-addr.arpa" to the end of thee reversed hostname
	strcat(add,"in-addr.arpa");
	return add;
}

void expandIPv6(const char* compressedIPv6, char* expandedIPv6, size_t expandedIPv6Size) {
    char buffer[60];  // Buffer to hold the expanded IPv6 address
    int bufferIndex = 0;
	char tmpBuffer[60];
	char reversedBuffer[60];
	char address[1024];

	bool colon = false;

	int colonCnt = 0;
	int currentColon = 0;

	int segmentCnt = 0;

	for (size_t i = 0; i < strlen(compressedIPv6); ++i)
	{
		if(compressedIPv6[i] == ':')
		{
			colonCnt++;
		}
	}
	
    // Iterate through the compressed IPv6 address
  for (size_t i = 0; i < strlen(compressedIPv6); ++i) {
        // If the current character is a colon, expand the segment to 4 characters with leading zeros
        if (compressedIPv6[i] == ':') {
			i++;
			currentColon++;
			while(compressedIPv6[i] != ':' && i < strlen(compressedIPv6))
			{
				tmpBuffer[segmentCnt] = compressedIPv6[i];
				segmentCnt++;
				i++;
				colon = true;
			}
			
			if(compressedIPv6[i] == ':')
			{
				currentColon++;
			}
		}
		else{
			buffer[bufferIndex++] = compressedIPv6[i];
		}


		if(currentColon == colonCnt){
			for(int cnt = 0;cnt < 4*(8-colonCnt);cnt++)
			{
				buffer[bufferIndex++] = '0';
			}

		}

		if(segmentCnt < 4 && colon == true)
		{
			for(int cnt = 0; cnt < 4 - segmentCnt; cnt++)
			{
				buffer[bufferIndex++] = '0';	
			}
			for(int cnt = 0; cnt < segmentCnt; cnt++)
			{
				buffer[bufferIndex++] = tmpBuffer[cnt];
			}
			segmentCnt = 0;
			colon = false;
		}
		
    }

	buffer[bufferIndex] = '\0';

	int bufferY = bufferIndex -1;
	int y = 0;
	while(bufferY >= 0)
	{
		reversedBuffer[y] = buffer[bufferY];
		bufferY--;
		y++;
	}



	int i = 0;
	int bufferI = 0;

	while(i < (bufferIndex*2)-1)
	{
		address[i] = reversedBuffer[bufferI];
		i++;
		bufferI++;
		if(i != (bufferIndex*2)-2)
		{
		address[i] = '.';
		i++;
		} 
	}
	address[i] = '\0';
    // Copy the expanded IPv6 address to the provided output buffer
    snprintf(expandedIPv6, sizeof(address), "%s", address);
	strcat(expandedIPv6,"ip6.arpa");
}

void printInfo(DNS_HEADER *dns)
{
	if(dns->aa == 1){
		printf("Yes,");
	}
	else{
		printf("No,");
	}

	printf("Recursion: ");
	if(dns->ra == 1){
		printf("Yes,");
	}
	else{
		printf("No,");
	}

	printf("Truncated: ");
	if(dns->tc == 1){
		printf("Yes,");
	}
	else{
		printf("No\n");
	}
}

void printQuesions(Params *params)
{
	if(params->reversed){
		printf(" %s, ",reversedFormat(params->hostname));
	}
	else{
		printf(" %s, ",params->hostname);
	}

	switch(params->Q_type)
	{
		case T_A:
			printf("A, ");
			break;
		case T_AAAA:
			printf("AAAA, ");
			break;
		case T_CNAME:
			printf("CNAME, ");
			break;
		case T_PTR:
			printf("PTR, ");
			break;
		default:
			break;
	}
	
	printf("IN\n");
}

void printAnswers(RES_RECORD answers[],Params *params, int i,sockaddr_in a,in_addr ipv4_addr,in6_addr ipv6_addr)
{
	printf(" %s., ",answers[i].name);

		switch(ntohs(answers[i].resource->type))
		{
			case T_A:
				printf("A, ");
				break;
			case T_AAAA:
				printf("AAAA, ");
				break;
			case T_CNAME:
				printf("CNAME, ");
				break;
			case T_PTR:
				printf("PTR, ");
				break;
			case T_NS:
				printf("NS, ");
				break;
			case T_SOA:
				printf("SOA, ");
				break;
			default:
				break;
		}
		printf("IN, ");

		if( ntohs(answers[i].resource->type) == T_A) //IPv4 address
		{
			printf("%d,",ntohl(answers[i].resource->ttl));
			
				long *p;
				p=(long*)answers[i].rdata;
				a.sin_addr.s_addr=(*p);
				printf("%s",inet_ntoa(a.sin_addr));
			
			printf("\n");
		}

		if( ntohs(answers[i].resource->type) == T_AAAA) //IPv6 address
		{
			printf("%d,",ntohl(answers[i].resource->ttl));
			
			struct in6_addr *ipv6_address = (struct in6_addr *)answers[i].rdata;

				char ipv6_str[INET6_ADDRSTRLEN];
				inet_ntop(AF_INET6, ipv6_address, ipv6_str, INET6_ADDRSTRLEN);
				printf("%s\n",ipv6_str);
			
		}

		if( ntohs(answers[i].resource->type) == T_CNAME)
		{
			printf("%d,",ntohl(answers[i].resource->ttl));
			
			long *p;
			p=(long*)answers[i].rdata;
			a.sin_addr.s_addr=(*p);
			printf("%s.",answers[i].rdata);
			
			printf("\n");
		}

		if( ntohs(answers[i].resource->type) == T_PTR) //IPv4 address
		{

			printf("%d,",ntohl(answers[i].resource->ttl));
			
			printf("%s.",answers[i].rdata);
			
			printf("\n");
		}

		if( ntohs(answers[i].resource->type) == T_NS) //IPv4 address
		{

			printf("%d,",ntohl(answers[i].resource->ttl));
			
			printf("%s.",answers[i].rdata);
			
			printf("\n");
		}

}
