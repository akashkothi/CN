#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <poll.h>
#include <ctype.h>
#include <signal.h>
#include <inttypes.h>
#include <ifaddrs.h>
#include <getopt.h>

#include <bits/stdc++.h>

#include <net/if.h>
#include <netpacket/packet.h>
// #include <linux/if_packet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <linux/if_ether.h>
#include <linux/kcmp.h>
// #include <linux/if_arp.h>

#include <asm/byteorder.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/un.h>


#include <pcap.h>

#define BUFFSIZE 1024
#define PACKETSIZE 65536
#define RWX 0666
#define R O_RDONLY
#define W O_WRONLY
#define RW O_RDWR
#define PORT 8080
#define BACKLOG 10
#define LOCAL_HOST "127.0.0.1"
#define ADDRESS "mysocket"
#define MAXBYTES2CAPTURE 2048

int ports[3] = {8081,8082,8083};
const char* path[3] = {"mysocket1","mysocket2","mysocket3"};

struct mymesg {
    long type;
    char text[BUFFSIZE];
} msg;

using namespace std;

void error(const char* err) {
    perror(err);
    exit(EXIT_FAILURE);
}

void capitalize(char *buff) {
    int n = strlen(buff);
    for(int i = 0; i < n; i++)
        buff[i] = toupper(buff[i]);
}

void de_capitalize(char *buff) {
    int n = strlen(buff);
    for(int i = 0; i < n; i++)
        buff[i] = tolower(buff[i]);
}

void init_socket_address(struct sockaddr_in* socket_addr, const char* ip_addr) {
    socket_addr->sin_family = AF_INET;
    if(inet_pton(AF_INET,ip_addr,&socket_addr->sin_addr) < 1)
        error("inet_pton error");
    // printf("\nInitialized socket address ...\n");
}

void init_socket_address(struct sockaddr_in* socket_addr, const char* ip_addr, int port) {
    socket_addr->sin_family = AF_INET;
    socket_addr->sin_port = htons(port);
    if(inet_pton(AF_INET,ip_addr,&socket_addr->sin_addr) < 1)
        error("inet_pton error");
    // printf("\nInitialized socket address ...\n");
}

pid_t getpid_by_name(string name) {
    
    int fd;
    pid_t pid;
    char buff[BUFFSIZE] = {'\0'};
    
    name = "pidof " + name;
    fd = fileno(popen(name.c_str(),"r"));
    read(fd,buff,BUFFSIZE);
    pid = atoi(buff);
    
    return pid;
}

int send_fd(int socket, int fd_to_send) {
    
    struct msghdr socket_message;
    struct iovec io_vector[1];
    struct cmsghdr *control_message = NULL;
    char message_buffer[1];
    
    
    /* storage space needed for an ancillary element with a paylod of
    length is CMSG_SPACE(sizeof(length)) */
    char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
    int available_ancillary_element_buffer_space;
    
    
    /* at least one vector of one byte must be sent */
    message_buffer[0] = 'F';
    io_vector[0].iov_base = message_buffer;
    io_vector[0].iov_len = 1;
    
    
    /* initialize socket message */
    memset(&socket_message, 0, sizeof(struct msghdr));
    socket_message.msg_iov = io_vector;
    socket_message.msg_iovlen = 1;
    
    
    /* provide space for the ancillary data */
    available_ancillary_element_buffer_space = CMSG_SPACE(sizeof(int));
    memset(ancillary_element_buffer, 0, available_ancillary_element_buffer_space);
    socket_message.msg_control = ancillary_element_buffer;
    socket_message.msg_controllen = available_ancillary_element_buffer_space;


    /* initialize a single ancillary data element for fd passing */
    control_message = CMSG_FIRSTHDR(&socket_message);
    control_message->cmsg_level = SOL_SOCKET;
    control_message->cmsg_type = SCM_RIGHTS;
    control_message->cmsg_len = CMSG_LEN(sizeof(fd_to_send));
    *((int *) CMSG_DATA(control_message)) = fd_to_send;
    
    return sendmsg(socket, &socket_message, 0);
}

int send_fd(int socket, int fd_to_send,const char *msg) {
    
    struct msghdr socket_message;
    struct iovec io_vector[1];
    struct cmsghdr *control_message = NULL;
    char message_buffer[BUFFSIZE] = {'\0'};
    
    
    /* storage space needed for an ancillary element with a paylod of
    length is CMSG_SPACE(sizeof(length)) */
    char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
    int available_ancillary_element_buffer_space;
    
    
    /* at least one vector of one byte must be sent */
    strcpy(message_buffer,msg);
    io_vector[0].iov_base = message_buffer;
    io_vector[0].iov_len = BUFFSIZE;
    
    
    /* initialize socket message */
    memset(&socket_message, 0, sizeof(struct msghdr));
    socket_message.msg_iov = io_vector;
    socket_message.msg_iovlen = 1;
    
    
    /* provide space for the ancillary data */
    available_ancillary_element_buffer_space = CMSG_SPACE(sizeof(int));
    memset(ancillary_element_buffer, 0, available_ancillary_element_buffer_space);
    socket_message.msg_control = ancillary_element_buffer;
    socket_message.msg_controllen = available_ancillary_element_buffer_space;


    /* initialize a single ancillary data element for fd passing */
    control_message = CMSG_FIRSTHDR(&socket_message);
    control_message->cmsg_level = SOL_SOCKET;
    control_message->cmsg_type = SCM_RIGHTS;
    control_message->cmsg_len = CMSG_LEN(sizeof(fd_to_send));
    *((int *) CMSG_DATA(control_message)) = fd_to_send;
    
    return sendmsg(socket, &socket_message, 0);
}

int recv_fd(int socket) {
    
    int sent_fd, available_ancillary_element_buffer_space;
    struct msghdr socket_message;
    struct iovec io_vector[1];
    struct cmsghdr *control_message = NULL;
    char message_buffer[1];
    char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
    
    /* start clean */
    memset(&socket_message, 0, sizeof(struct msghdr));
    memset(ancillary_element_buffer, 0, CMSG_SPACE(sizeof(int)));
    
    /* setup a place to fill in message contents */
    io_vector[0].iov_base = message_buffer;
    io_vector[0].iov_len = 1;
    socket_message.msg_iov = io_vector;
    socket_message.msg_iovlen = 1;
    
    /* provide space for the ancillary data */
    socket_message.msg_control = ancillary_element_buffer;
    socket_message.msg_controllen = CMSG_SPACE(sizeof(int));
    
    if(recvmsg(socket, &socket_message, MSG_CMSG_CLOEXEC) < 0) {
        return -1;
    }
    
    if(message_buffer[0] != 'F') {
        /* this did not originate from the above function */
        return -1;
    }

    if((socket_message.msg_flags & MSG_CTRUNC) == MSG_CTRUNC) {
        /* we did not provide enough space for the ancillary element array */
        return -1;
    }
    
    /* iterate ancillary elements */
    for(control_message = CMSG_FIRSTHDR(&socket_message); control_message != NULL; control_message = CMSG_NXTHDR(&socket_message, control_message)) {
        if((control_message->cmsg_level == SOL_SOCKET) &&(control_message->cmsg_type == SCM_RIGHTS)) {
            sent_fd = *((int *) CMSG_DATA(control_message));
            return sent_fd;
        }
    }

    return -1; 
}

int recv_fd(int socket, char* msg) {
    
    int sent_fd, available_ancillary_element_buffer_space;
    struct msghdr socket_message;
    struct iovec io_vector[1];
    struct cmsghdr *control_message = NULL;
    char message_buffer[BUFFSIZE] = {'\0'};
    char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
    
    /* start clean */
    memset(&socket_message, 0, sizeof(struct msghdr));
    memset(ancillary_element_buffer, 0, CMSG_SPACE(sizeof(int)));
    
    /* setup a place to fill in message contents */
    io_vector[0].iov_base = message_buffer;
    io_vector[0].iov_len = BUFFSIZE;
    socket_message.msg_iov = io_vector;
    socket_message.msg_iovlen = 1;
    
    /* provide space for the ancillary data */
    socket_message.msg_control = ancillary_element_buffer;
    socket_message.msg_controllen = CMSG_SPACE(sizeof(int));
    
    if(recvmsg(socket, &socket_message, MSG_CMSG_CLOEXEC) < 0) {
        return -1;
    }

    strcpy(msg,message_buffer);
    
    if((socket_message.msg_flags & MSG_CTRUNC) == MSG_CTRUNC) {
        /* we did not provide enough space for the ancillary element array */
        return -1;
    }
    
    /* iterate ancillary elements */
    for(control_message = CMSG_FIRSTHDR(&socket_message); control_message != NULL; control_message = CMSG_NXTHDR(&socket_message, control_message)) {
        if((control_message->cmsg_level == SOL_SOCKET) &&(control_message->cmsg_type == SCM_RIGHTS)) {
            sent_fd = *((int *) CMSG_DATA(control_message));
            return sent_fd;
        }
    }

    return -1; 
}

int uds_socket(int type, const char* name,struct sockaddr_un &userv_addr ) {

    int usfd;
    
    if((usfd = socket(AF_UNIX, type, 0)) < 0)
        error("uds socket error");
    
    bzero(&userv_addr,sizeof(userv_addr));
    userv_addr.sun_family = AF_UNIX;
    strcpy(userv_addr.sun_path, name);

    return usfd;
}

int uds_socket_bind(const char* name, struct sockaddr_un &addr) {

    unlink(name);

    int usfd = uds_socket(SOCK_STREAM, name, addr); 

    if(bind(usfd,(struct sockaddr*)&addr,sizeof(struct sockaddr_un)) < 0)
        error("uds bind error");
        
    if(listen(usfd,BACKLOG) < 0)
        error("uds listen error");
    
    return usfd;
}

int uds_socket(const char* name, struct sockaddr_un &addr) {

    int usfd;
    if((usfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        error("uds socket error");
    
    bzero(&addr,sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, name);

    return usfd;
}


void print_ip_header(struct iphdr* ip) {
	cout<<"\n\t--------------------- IP HEADER --------------------\n"<<endl;
	cout<<"\t\tIP version : "<<(unsigned int)ip->version<<endl;
	cout<<"\t\tIP header length : "<<((unsigned int)ip->ihl)*4<<endl;
	cout<<"\t\tType of service : "<<(unsigned int)ip->tos<<endl;
	cout<<"\t\tIP packet length : "<<ntohs(ip->tot_len)<<endl;
	cout<<"\t\tPacket ID : "<<ntohs(ip->id)<<endl;
	cout<<"\t\tTime to live : "<<(unsigned int)ip->ttl<<endl;
	cout<<"\t\tProtocol : "<<(unsigned int)ip->protocol<<endl;
	cout<<"\t\tCheckSum : "<<ip->check<<endl;
	cout<<"\t\tSource IP Address : "<<inet_ntoa(*(in_addr*)&ip->saddr)<<endl;
	cout<<"\t\tDestination IP Address : "<<inet_ntoa(*(in_addr*)&ip->daddr)<<endl<<endl;
    return;
}


struct tcphdr {
        __be16        source;
        __be16        dest;
        __be32        seq;
        __be32        ack_seq;
#if defined(__LITTLE_ENDIAN_BITFIELD)
        __u16        res1:4,
                doff:4,
                fin:1,
                syn:1,
                rst:1,
                psh:1,
                ack:1,
                urg:1,
                ece:1,
                cwr:1;
#elif defined(__BIG_ENDIAN_BITFIELD)
        __u16        doff:4,
                res1:4,
                cwr:1,
                ece:1,
                urg:1,
                ack:1,
                psh:1,
                rst:1,
                syn:1,
                fin:1;
#else
#error        "Adjust your <asm/byteorder.h> defines"
#endif        
        __be16        window;
        __sum16        check;
        __be16        urg_ptr;
};


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

void print_tcp_header(struct tcphdr *tcph) {
     
    cout<<"\n\t--------------------- TCP HEADER --------------------\n"<<endl;
         
    printf("\t\tSource Port      : %u\n",ntohs(tcph->source));
    printf("\t\tDestination Port : %u\n",ntohs(tcph->dest));
    printf("\t\tSequence Number    : %u\n",ntohl(tcph->seq));
    printf("\t\tAcknowledge Number : %u\n",ntohl(tcph->ack_seq));
    printf("\t\tHeader Length      : %d\n",(unsigned int)tcph->doff*4);
    printf("\t\tUrgent Flag          : %d\n",(unsigned int)tcph->urg);
    printf("\t\tAcknowledgement Flag : %d\n",(unsigned int)tcph->ack);
    printf("\t\tFinish Flag          : %d\n",(unsigned int)tcph->fin);
    printf("\t\tPush Flag            : %d\n",(unsigned int)tcph->psh);
    printf("\t\tReset Flag           : %d\n",(unsigned int)tcph->rst);
    printf("\t\tSynchronise Flag     : %d\n",(unsigned int)tcph->syn);
    printf("\t\tWindow         : %d\n",ntohs(tcph->window));
    printf("\t\tChecksum       : %d\n",ntohs(tcph->check));
    printf("\t\tUrgent Pointer : %d\n",tcph->urg_ptr);
    printf("\n");    
}

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

pid_t getpid_by_port(unsigned short port) {

    int fd, pid;
    char buff[BUFFSIZE] = {'\0'};
    string cmd = "netstat -nlp | grep " + to_string(port) + " | awk '{print $NF}' ";

    if((fd = fileno(popen(cmd.c_str(), "r"))) < 0)
        error("getpid_by_port : popen error");

    if(read(fd, buff,BUFFSIZE) < 0)
        error("getpid_by_port : read error");

    if(close(fd) < 0)
        error("getpid_by_port : close error");

    pid = atoi(strtok(buff, "/"));

    return (pid_t)pid;

}
