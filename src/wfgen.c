/*
This is Source File OF server
*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <wfgen.h>
#define port 25255
extern unsigned int rmsg[512];           
extern unsigned int txmsg[512];
extern struct sockaddr_in target;
int can_quit=1;
int sockflags = MSG_DONTWAIT;
int tos_vo,tos_vi,tos_be,tos_bk;
struct client stations[NSTA];
struct sockaddr_in from;
int from_len=sizeof(from);
int sock;
extern int brsock;
int dscp;

struct apts_msg apts_msgs[] ={
	{0, -1},
	{"B.D", B_D},	
	{"B.H", B_H},
        {"B.B", B_B},
	{"B.M", B_M},	
	{"M.D", M_D},	
	{"B.Z", B_Z},	
	{"M.Y", M_Y},	
	{"L.1", L_1},	
	{"A.Y", A_Y},	
	{"B.W", B_W},	
	{"A.J", A_J},	
	{"M.V", M_V},	
	{"M.U", M_U},	
        {"A.U", A_U},	
	{"M.L", M_L},	
	{"B.K", B_K},	
	{"M.B", M_B},	
	{"M.K", M_K},	
	{"M.W", M_W},
	{"APTS TX         ", APTS_DEFAULT },
	{"APTS Hello      ", APTS_HELLO },
	{"APTS Broadcast  ", APTS_BCST },
	{"APTS Confirm    ", APTS_CONFIRM},
	{"APTS STOP       ", APTS_STOP},
	{"APTS CK BE      ", APTS_CK_BE },
	{"APTS CK BK      ", APTS_CK_BK },
	{"APTS CK VI      ", APTS_CK_VI },
	{"APTS CK VO      ", APTS_CK_VO },
	{"APTS RESET      ", APTS_RESET },
	{"APTS RESET RESP ", APTS_RESET_RESP },
	{"APTS RESET STOP ", APTS_RESET_STOP },
	{0, 0 }		// APTS_LAST
};





void create_apts_msg(int msg, unsigned int txbuf[],int id)
{
	struct apts_msg *t;

	t = &apts_msgs[msg];
	txbuf[ 1] = 0;
	txbuf[ 2] = 0;
	txbuf[ 3] = 0;
	txbuf[ 4] = 0;
	txbuf[ 5] = 0; 
	txbuf[ 6] = 0; 
	txbuf[ 7] = 0; 
	txbuf[ 8] = 0; 
	txbuf[ 9] = id;
	txbuf[ 10] = t->cmd; 
	strcpy((char *)&txbuf[11], t->name);
	
}
void print_hex_string(char* buf, int len)
{
    int i;

    if (len==0) { printf("<empty string>"); return; }

    for (i = 0; i < len; i++) {
        printf("%02x ", *((unsigned char *)buf + i));
	if ((i&0xf)==15) printf("\n   ");
    }
    if ((i&0xf))
	printf("\n");
}

void mpx(char *m, void *buf_v, int len)
{
    char *buf = buf_v;
    printf("%s   MSG: %s\n   ", m, &buf[44] );
    print_hex_string(buf, len);
}
struct apts_msg * apts_lookup(char *s)
{
	struct apts_msg *t;

	for (t=&apts_msgs[1]; s && t->cmd; t++) 
	{
		if (t->name && strcmp(t->name, s)==0) 
		{
			return(t);
		}
		//if ( traceflag) printf("APTS Test(%s)\n", t->name);
	}
	fprintf(stderr, "APTS Test(%s) unknown\n", s);
	fprintf(stderr, "available tests are:\n");
	for (t=&apts_msgs[1]; t->cmd; t++) 
	{
		if (t->cmd == APTS_DEFAULT)
			break;

		if (t->name) 
		{
			fprintf(stderr, "\t%s\n", t->name);
		}
	}
	exit(-1);
}
int expectedmsgrcd(unsigned int *rmsg,unsigned long type,int tos)
{
  char type_ok=1;
  char tos_ok=1;
  int r;
  char dsc;
  type_ok=((rmsg[10] == type)? 1:0);

  r = (type_ok&&tos_ok);

  return r;
  
}
int assign_sta_id(unsigned int addr)
{
    printf("Assign station id for %d\n", addr);
	int id;
	for(id=0; id<NSTA; id++) 
	{
		if (stations[id].s_addr == 0) 
		{
			stations[id].s_addr = addr;
			break;
		}
	}
    if(id == NSTA)
      id = -1;
    if(id > 0)
      can_quit=0;
	return(id);
}
int get_sta_id(unsigned int addr)
{
    printf("Assign station id for %d\n", addr);
	int id;
	for(id=0; id<NSTA; id++) 
	{
		if (stations[id].s_addr == addr) 
		{
			break;
		}
	}
    if(id == NSTA)
      id = -1;
	return(id);
}

int socket_create()
{
	int one=1; 
	sock=socket(AF_INET, SOCK_DGRAM, 0);
	from.sin_family = AF_INET;
	from.sin_port = htons(port);
	from.sin_addr.s_addr = htonl(INADDR_ANY);
	int r = bind(sock, (struct sockaddr *)&from, sizeof(from));
	if (r<0) 
	{
		perror("bind call failed");
		exit(-1);
	}
    //brsock =socket(AF_INET, SOCK_DGRAM, 0);
    //target.sin_family = AF_INET;
	//target.sin_port =htons( port);
	//memcpy((caddr_t)dst, (caddr_t)&target, sizeof(target));
	//r = setsockopt(brsock, SOL_SOCKET, SO_BROADCAST, &one, sizeof(one));
	if ( r<0 ) 
	{ 
		perror("multicast mode socket setup 1"); 
	}

	return 0;



}

int set_dscp(int new_dscp) 
{
	int r;
	//if (new_dscp == ldscp)
	//	return;

	if ((r=setsockopt(sock, SOL_IP, IP_TOS, &new_dscp, sizeof(dscp)))<0) 
	{
		perror("can't set dscp/tos field");
		exit(-1);
	}
	//dscp = ldscp = new_dscp;
	//rmsg[1] = dscp;
	return(new_dscp);
}


int WfaConRcvHello(struct client *sta,unsigned int *rmsg,int length)
{
	int r;
printf("in call back fucitopn\n");
mpx("CMD send\n", rmsg, 64);
  	if(!expectedmsgrcd(rmsg,APTS_HELLO,tos_be))
  	{
    	printf("\r\nHello not recd or BAD TOS\n");
  	}
  	else
  	{
    	create_apts_msg(sta->cmd, txmsg,sta->myid);
		r = sendto(sock, txmsg, 190, sockflags, (struct sockaddr *)&from, sizeof(from));
    	sta->state++;
		
  	}	

}
int WfaConRcvConf(struct client *sta,unsigned int *rmsg,int length)
{
 	int r;
  	if(!expectedmsgrcd(rmsg,APTS_CONFIRM,tos_be))
  	{
    	printf("\r\nConfirm not recd or BAD TOS\n");
  	}
  	else
  	{
    
    	sta->state++;
	
  	}
}
int WfaConRcvVOSnd(struct client *sta,unsigned int *rmsg,int length)
{
	int r;
  	if(!expectedmsgrcd(rmsg,APTS_DEFAULT,tos_vo))
  	{
    	printf("\r\nexpected message not recd or BAD TOS\n");
  	}
  	else
  	{
    	rmsg[0] = ++(sta->msgno);
		set_dscp(tos_vo);
		r = sendto(sock, rmsg, 190, sockflags, (struct sockaddr *)&from, sizeof(from));
    	sta->state++;
		printf("sent VO");
  	}
}
int WfaConRcvVOE(struct client *sta,unsigned int *rmsg,int length)
{
	int r;
  	if(!expectedmsgrcd(rmsg,APTS_DEFAULT,tos_vo))
  	{
    	printf("\r\nexpected message not recd or BAD TOS 0x%x\n", tos_vo);
  	}
  	else
  	{
		create_apts_msg(APTS_STOP, txmsg,sta->myid);
    	txmsg[0] = ++(sta->msgno);
    	set_dscp(tos_be);
		r = sendto(sock, txmsg, 190, sockflags, (struct sockaddr *)&from, sizeof(from));
    	sta->state++;
	}
}
int WfaConWaitStop(struct client *sta,unsigned int *rmsg,int length)
{
 	int r;
  	if(!expectedmsgrcd(rmsg,APTS_STOP,tos_be))
  	{
    	printf("\r\nSTOP not recd or BAD TOS\n");
  	}
  	else
  	{
    	if(can_quit)
       		exit(0);
    	else
      		can_quit=1;
  	}
}

int WfaConRcvVOSnd2VO(struct client *sta,unsigned int *rmsg,int length)
{
	int r;
  	if(!expectedmsgrcd(rmsg,APTS_DEFAULT,tos_vo))
  	{
    	printf("\r\nexpected message not recd or BAD TOS\n");
  	}
  	else
  	{
    	rmsg[0] = ++(sta->msgno);
		set_dscp(tos_vo);
		r = sendto(sock, rmsg, 190, sockflags, (struct sockaddr *)&from, sizeof(from));
		rmsg[0] = ++(sta->msgno);
		r = sendto(sock, rmsg, 190, sockflags, (struct sockaddr *)&from, sizeof(from));
    	sta->state++;
		printf("sent VO");
  	}
}



int WfaConRcvVO(struct client *sta,unsigned int *rmsg,int length)
{
	int r;
  	if(!expectedmsgrcd(rmsg,APTS_DEFAULT,tos_vo))
  	{
    	printf("\r\nexpected message not recd or BAD TOS 0x%x\n", tos_vo);
  	}
  	else
  	{
		
    	sta->state++;
	}
}
int WfaConRcvVI(struct client *sta,unsigned int *rmsg,int length)
{
	int r;
  	if(!expectedmsgrcd(rmsg,APTS_DEFAULT,tos_vi))
  	{
    	printf("\r\nexpected message not recd or BAD TOS 0x%x\n", tos_vo);
  	}
  	else
  	{
		
    	sta->state++;
	}
}

int WfaConRcvBE(struct client *sta,unsigned int *rmsg,int length)
{
	int r;
  	if(!expectedmsgrcd(rmsg,APTS_DEFAULT,tos_be))
  	{
    	printf("\r\nexpected message not recd or BAD TOS 0x%x\n", tos_vo);
  	}
  	else
  	{
		
    	sta->state++;
	}
}
int WfaConRcvBKE(struct client *sta,unsigned int *rmsg,int length)
{
	int r;
  	if(!expectedmsgrcd(rmsg,APTS_DEFAULT,tos_bk))
  	{
    	printf("\r\nexpected message not recd or BAD TOS 0x%x\n", tos_vo);
  	}
  	else
  	{
		create_apts_msg(APTS_STOP, txmsg,sta->myid);
    	
    	set_dscp(tos_be);
		r = sendto(sock, txmsg, 190, sockflags, (struct sockaddr *)&from, sizeof(from));
    	sta->state++;

    }
}
int WfaConRcvVIE(struct client *sta,unsigned int *rmsg,int length)
{
	int r;
  	if(!expectedmsgrcd(rmsg,APTS_DEFAULT,tos_vi))
  	{
    	printf("\r\nexpected message not recd or BAD TOS 0x%x\n", tos_vo);
  	}
  	else
  	{
		create_apts_msg(APTS_STOP, txmsg,sta->myid);
    	
    	set_dscp(tos_be);
		r = sendto(sock, txmsg, 190, sockflags, (struct sockaddr *)&from, sizeof(from));
    	sta->state++;

    }
}
int WfaConRcvVISndBE(struct client *sta,unsigned int *rmsg,int length)
{
	int r;
  	if(!expectedmsgrcd(rmsg,APTS_DEFAULT,tos_vi))
  	{
    	printf("\r\nexpected message not recd or BAD TOS\n");
  	}
  	else
  	{
    	rmsg[0] = ++(sta->msgno);
		set_dscp(tos_be);
		r = sendto(sock, rmsg, 190, sockflags, (struct sockaddr *)&from, sizeof(from));
		
    	sta->state++;
		printf("sent VO");
  	}
}
int WfaConRcvVISndBK(struct client *sta,unsigned int *rmsg,int length)
{
	int r;
  	if(!expectedmsgrcd(rmsg,APTS_DEFAULT,tos_vi))
  	{
    	printf("\r\nexpected message not recd or BAD TOS\n");
  	}
  	else
  	{
    	rmsg[0] = ++(sta->msgno);
		set_dscp(tos_bk);
		r = sendto(sock, rmsg, 190, sockflags, (struct sockaddr *)&from, sizeof(from));
		
    	sta->state++;
		printf("sent VO");
  	}
}
int WfaConRcvVISnd(struct client *sta,unsigned int *rmsg,int length)
{
	int r;
  	if(!expectedmsgrcd(rmsg,APTS_DEFAULT,tos_vi))
  	{
    	printf("\r\nexpected message not recd or BAD TOS\n");
  	}
  	else
  	{
    	rmsg[0] = ++(sta->msgno);
		set_dscp(tos_vi);
		r = sendto(sock, rmsg, 190, sockflags, (struct sockaddr *)&from, sizeof(from));
		
    	sta->state++;
		printf("sent VO");
  	}
}
int WfaConRcvVISndVOE(struct client *sta,unsigned int *rmsg,int length)
{
	int r;
  	if(!expectedmsgrcd(rmsg,APTS_DEFAULT,tos_vi))
  	{
    	printf("\r\nexpected message not recd or BAD TOS\n");
  	}
  	else
  	{
    	rmsg[0] = ++(sta->msgno);
		set_dscp(tos_vo);
		r = sendto(sock, rmsg, 190, sockflags, (struct sockaddr *)&from, sizeof(from));
		printf("sent VO\n");
		create_apts_msg(APTS_STOP, txmsg,sta->myid);
    	set_dscp(tos_be);
		r = sendto(sock, txmsg, 190, sockflags, (struct sockaddr *)&from, sizeof(from));
		printf("sent Stop\n");
		sta->state++;
    	
		
    	
  	}
}
