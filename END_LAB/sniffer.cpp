#include "./cn.h"

int main(int argc, char* argv[]) {

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

    memset(errbuf, 0, PCAP_ERRBUF_SIZE);

    if(argc != 2) {
        printf("USAGE : sniffer.exe <interface> \n");
        exit(EXIT_FAILURE);
    }

    /* Open network device for packet capture */

    char *interface = strdup(argv[1]);

    if((descr = pcap_open_live(interface, MAXBYTES2CAPTURE, 0, 512, errbuf)) == NULL) {
        fprintf(stderr, "pcap_open_live(): %s \n", errbuf);
        exit(EXIT_FAILURE);
    }

    /* Look up info from the capture device */

    if( pcap_lookupnet(interface , &netaddr, &mask, errbuf) < 0) { 
        fprintf(stderr, "ERROR: pcap_lookupnet(): %s\n", errbuf );
        exit(EXIT_FAILURE);
    }

    /* Compiles the filter expresion into a BPF filter program */

    if(pcap_compile(descr, &filter, "ip", 1, mask) < 0){
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

        print_ip_header(ip);

        struct pseudo_tcphdr *ptcp_header = (struct pseudo_tcphdr *)(14 + packet + sizeof(struct iphdr)); /* point to the arp header */ 

        // sleep(2);

    }

    return 0;

}