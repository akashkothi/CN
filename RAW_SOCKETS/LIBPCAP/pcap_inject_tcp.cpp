#include "../cn.h"


int main(int argc, const char* argv[]) {

    if(argc != 3) {
        printf("USAGE : pcap_inject_tcp.exe <interface> <destination ip>\n");
        exit(EXIT_FAILURE);
    }

    char *interface, *victim_ip;
    unsigned char  victim_mac[6], src_mac[6];

    interface = strdup(argv[1]);
    victim_ip = strdup(argv[2]);

    get_mac(interface, src_mac);
    printf("Source MAC : ");
    print_mac(src_mac);

    get_target_mac(interface, victim_ip, victim_mac);
    printf("Victim MAC : ");
    print_mac(victim_mac);

    // while(1);

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


    /* Open network device for packet capture */

    if((descr = pcap_open_live(interface, MAXBYTES2CAPTURE, 0, 512, errbuf)) == NULL) {
        fprintf(stderr, "pcap_open_live(): %s \n", errbuf);
        exit(EXIT_FAILURE);
    }

    /* Look up info from the capture device */

    if(pcap_lookupnet(interface , &netaddr, &mask, errbuf) < 0) { 
        fprintf(stderr, "ERROR: pcap_lookupnet(): %s\n", errbuf);
        exit(EXIT_FAILURE);
    }

    /* Compiles the filter expresion into a BPF filter program */

    if(pcap_compile(descr, &filter, "tcp port 8080", 1, mask) < 0){
        fprintf(stderr, "Error in pcap_compile(): %s\n", pcap_geterr(descr) );
        exit(EXIT_FAILURE);
    }

    /* Load the filter program into the packet capture device */

    if(pcap_setfilter(descr,&filter) < 0) {
        fprintf(stderr, "Error in pcap_setfilter(): %s\n", pcap_geterr(descr));
        exit(EXIT_FAILURE);
    }

    while(1) {

        u_char packet[BUFFSIZE];

        struct ether_header *ether_hdr = (struct ether_header*)(packet);
        struct iphdr *ip_header = (struct iphdr*)(packet + sizeof(struct ether_header));
        struct tcphdr *tcp_header = (struct tcphdr*)(packet + sizeof(struct ether_header) + sizeof(iphdr));
        char* data = (char *)(packet + sizeof(struct ether_header) + sizeof(iphdr)+ sizeof(struct tcphdr));

        strcpy(data,"Hello!! I am Akash ...");

        set_ether_header(ether_hdr,ETH_P_IP,src_mac,victim_mac);
        set_ip_hdr(ip_header,IPPROTO_TCP,"172.30.141.72",victim_ip);
        set_tcp_header(tcp_header,8080,"172.30.141.72",9090,victim_ip,data);

        if(pcap_inject(descr, packet, sizeof(packet)) < 0)
            error("pcap_inject error");

        cout<<"packet sent ..."<<endl;

        sleep(2);

    }
  
    return 0;
}