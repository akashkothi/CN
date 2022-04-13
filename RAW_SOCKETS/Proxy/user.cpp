#include "../cn.h"

#define USER_IP "127.0.0.2"

void* receive(void *arg) {

    int i = 0;

    bpf_u_int32 netaddr = 0, mask = 0;  /*To Store network 
                            address and netmask */

    struct bpf_program filter;    /* Place to store the
                        BPF filter program */

    char errbuf[PCAP_ERRBUF_SIZE]; /* Error buffer */

    pcap_t *descr  = NULL;  /* Network interface handler */

    struct pcap_pkthdr pkthdr; /*Packet information
                        (timestamp , size...)*/
    
    const unsigned char *packet = NULL; /* Received raw data */

    struct tcphdr *tcpheader = NULL; /* Pointer to ARP header */

    memset(errbuf, 0, PCAP_ERRBUF_SIZE);

    /* Open network device for packet capture */

    char *interface = (char *)arg;

    if((descr = pcap_open_live(interface, MAXBYTES2CAPTURE, 0, 512, errbuf)) == NULL) {
        fprintf(stderr, "pcap_open_live(): %s \n", errbuf);
        exit(EXIT_FAILURE);
    }

    /* Look up info from the capture device */

    if(pcap_lookupnet(interface , &netaddr, &mask, errbuf) < 0) { 
        fprintf(stderr, "ERROR: pcap_lookupnet(): %s\n", errbuf );
        exit(EXIT_FAILURE);
    }

    /* Compiles the filter expresion into a BPF filter program */

    if(pcap_compile(descr, &filter, "dst host 127.0.0.2", 1, mask) < 0) {
        fprintf(stderr, "Error in pcap_compile(): %s\n", pcap_geterr(descr) );
        exit(EXIT_FAILURE);
    }

    /* Load the filter program into the packet capture device */

    if(pcap_setfilter(descr,&filter) < 0) {
        fprintf(stderr, "Error in pcap_setfilter(): %s\n", pcap_geterr(descr));
        exit(EXIT_FAILURE);
    }

    while(1) {
        
        if((packet = pcap_next(descr,&pkthdr)) == NULL) {     /* Get one packet */
            fprintf(stderr, "Error in pcap_next(): %s\n", errbuf);
            exit(EXIT_FAILURE);
        }

        struct iphdr *ip = (struct iphdr*)(packet + 14);

        struct tcphdr *tcp = (struct tcphdr*)(packet + 14 + ip->ihl*4);

        print_ip_header(ip);

    }

}

int main(int argc, char*argv[]) {

    if(argc != 2) {
        printf("USAGE : tcp_sniffing.exe <interface> \n");
        exit(EXIT_FAILURE);
    }

    char* interface = strdup(argv[1]);

    pthread_t thread;
    int rsfd, opt = 1; 
    struct iphdr *ip_header;
    char buff[BUFFSIZE] = {'\0'};
	struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    ip_header = (struct iphdr*)buff;

    ip_header->version = 4;
    ip_header->ihl = 5;
	ip_header->tos = 0; 
	ip_header->tot_len = BUFFSIZE;
	ip_header->id = htons(218);
	ip_header->ttl = 100; 
	ip_header->protocol = IPPROTO_TCP; 
	ip_header->saddr = inet_addr("127.0.0.2");
	ip_header->daddr = inet_addr(LOCAL_HOST);

    strcpy(buff + 20,"Hello");

    pthread_create(&thread,NULL,receive,interface);

    if((rsfd = socket(AF_INET,SOCK_RAW,IPPROTO_TCP)) < 0)
	    error("socket error");
    
    if(setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(int)) < 0)
		error("setsockopt error");

    init_socket_address(&client_addr,LOCAL_HOST);	

    while(1) {

        if(sendto(rsfd,buff,BUFFSIZE,0,(struct sockaddr*)&client_addr,len) < 0)
            error("sendto error");

        sleep(2);
    }

}