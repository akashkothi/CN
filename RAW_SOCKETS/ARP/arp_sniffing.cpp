#include "../cn.h"

/* ARP Header, (assuming Ethernet+IPv4) */

#define ARP_REQUEST 1  /* ARP Request */
#define ARP_REPLY 2 /*ARP Reply */

typedef struct arp_header {

    u_int16_t htype;        /* Hardware Type           */
    u_int16_t ptype;        /* Protocol Type           */
    u_char hlen;            /* Hardware Address Length */        
    u_char plen;            /* Protocol Address Length */
    u_int16_t oper;         /* Operation Code          */
    u_char sha[6];          /* Sender hardware address */
    u_char spa[4];          /* Sender IP address       */
    u_char tha[6];          /* Target hardware address */
    u_char tpa[4];          /*Target IP address        */

} arphdr_t;


#define MAXBYTES2CAPTURE 2048

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

    arphdr_t *arpheader = NULL; /* Pointer to ARP header */

    memset(errbuf, 0, PCAP_ERRBUF_SIZE);

    if(argc != 2) {
        printf("USAGE : arpsniffer <interface>\n");
        exit(EXIT_FAILURE);
    }

    /* Open network device for packet capture */

    char *interface = strdup(argv[1]);

    if((descr = pcap_open_live(interface, MAXBYTES2CAPTURE, 0, 512, errbuf)) == NULL){
        fprintf(stderr, "pcap_open_live(): %s \n", errbuf);
        exit(EXIT_FAILURE);
    }

    /* Look up info from the capture device */

    if( pcap_lookupnet(interface , &netaddr, &mask, errbuf) < 0) { 
        fprintf(stderr, "ERROR: pcap_lookupnet(): %s\n", errbuf );
        exit(EXIT_FAILURE);
    }

    /* Compiles the filter expresion into a BPF filter program */

    if(pcap_compile(descr, &filter, "arp", 1, mask) < 0){
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

        arpheader = (struct arp_header *)(packet + 14); /* point to the arp header */ 

        printf("\n \nReceived Packet Size : %dbytes\n",pkthdr.len);
        
        printf("Hardware Type : %s\n",(ntohs(arpheader->htype) == 1) ? "Ethernet" : "Unknown");

        printf("Protocol type : %s\n",(ntohs(arpheader->ptype) == 0x0800) ? "IPv4" : "Unknown");

        printf("Operation : %s\n", (ntohs(arpheader->oper) == ARP_REQUEST) ? "ARP Request" : "ARP Reply");

        /* if is Ethernet and IPv4, print packet contents */

        if(ntohs(arpheader->htype) == 1 && ntohs(arpheader->ptype) == 0x0800) {
            
            printf("Sender MAC : ");
            for(int i = 0; i < 5; i++)
                printf("%02X:",arpheader->sha[i]);
            printf("%02X",arpheader->sha[5]);

            printf("\nSender IP : ");
            for(int i = 0; i < 3; i++)
                printf("%d.",arpheader->spa[i]);
            printf("%d",arpheader->spa[3]);
            
            printf("\nTarget MAC : ");
            for(int i = 0; i < 5; i++)
                printf("%02X:",arpheader->tha[i]);
            printf("%02X",arpheader->tha[5]);
            
            printf("\nTarget IP : ");
            for(int i = 0; i < 3; i++)
                printf("%d.",arpheader->tpa[i]);
            printf("%d\n",arpheader->tpa[3]);
        }

    }

    return 0;

}