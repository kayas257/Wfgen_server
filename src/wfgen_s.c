#include <stdio.h>
#include <wfgen.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

extern int sock,brsock;
extern struct sockaddr_in from;
struct sockaddr_in target;
extern from_len;
extern int tos_vo,tos_vi,tos_be,tos_bk;
char *procname; 
unsigned int rmsg[512];           
unsigned int txmsg[512];
extern struct client stations[NSTA];

int WfaConRcvHello(struct client *,unsigned int *,int );
int WfaConRcvConf(struct  client *,unsigned int *,int );
int WfaConRcvVOSnd(struct  client *,unsigned int *,int );
int WfaConRcvVOE(struct client *,unsigned int *,int );
int WfaConWaitStop(struct client *,unsigned int *,int );
int WfaConRcvVOSnd2VO(struct  client *,unsigned int *,int );
int WfaConRcvVO(struct  client *,unsigned int *,int );
int WfaConRcvVI(struct  client *,unsigned int *,int );
int WfaConRcvBE(struct  client *,unsigned int *,int );
int WfaConRcvBKE(struct  client *,unsigned int *,int );
int WfaConRcvVIE(struct  client *,unsigned int *,int );
int WfaConRcvVISndBE(struct  client *,unsigned int *,int );
int WfaConRcvVISndBK(struct  client *,unsigned int *,int );
int WfaConRcvVISnd(struct  client *,unsigned int *,int );
int WfaConRcvVISndVOE(struct  client *,unsigned int *,int );
consoleProcStatetbl_t consoleProcStatetbl[LAST_TEST+1][10] = {
{0,0},

{WfaConRcvHello,WfaConRcvConf,WfaConRcvVOSnd      ,WfaConRcvVOE              ,WfaConWaitStop      ,WfaConWaitStop      ,0,0,0,0},
{WfaConRcvHello,WfaConRcvConf,WfaConRcvVOSnd2VO   ,WfaConRcvVOE              ,WfaConWaitStop      ,WfaConWaitStop      ,0,0,0,0},
{WfaConRcvHello,WfaConRcvConf,WfaConRcvVO         ,WfaConRcvVI               ,WfaConRcvBE         ,WfaConRcvBKE        ,WfaConWaitStop,0,0,0},
{WfaConRcvHello,WfaConRcvConf,WfaConRcvVIE        ,WfaConWaitStop            ,0,0,0,0,0,0},
{WfaConRcvHello,WfaConRcvConf,WfaConRcvVISndBE    ,WfaConRcvVISndBK          ,WfaConRcvVISnd      ,WfaConRcvVISndVOE    ,WfaConWaitStop      ,0,0,0}
//{WfaConRcvHello,WfaConRcvConfSndVI                ,WfaConRcvVOSndBcastE      ,WfaConWaitStop      ,0,0,0,0}
};


//Main Code
int main(int argc, char **argv)

{
int nsta;
tos_vo = 0xD0;
tos_vi = 0xA0;
tos_be = 0x00;
tos_bk = 0x20;
char IP[INET_ADDRSTRLEN],string[128];	
struct apts_msg *testcase;
struct client *sta;
consoleProcStatetbl_t func;
int id;
if(argc<2)
{
	printf("USAGE:\n");
	exit(0);
}
procname = argv[1];
socket_create();
testcase = (struct apts_msg *) apts_lookup(procname);
printf("got tests case: %d\n",testcase->cmd);
int nBytes;

while(1)
{
printf("waiting For Sta\n");

nBytes = recvfrom(sock,rmsg,sizeof(rmsg),0,(struct sockaddr *)&from, &from_len);

if(nBytes<0)

{
printf("error");
}
printf("\r\n cmd is %d\n",rmsg[10]);
if (rmsg[10]==APTS_HELLO) 
{
	if((id = get_sta_id(from.sin_addr.s_addr))>=0)
    {
    }
    else if((id = assign_sta_id(from.sin_addr.s_addr))<0)
    {
		printf( "Can not assign id,sta list full");
		continue;
    }
    nsta++;
    sta = &stations[id];
    bzero(sta->ipaddress, 20);
	inet_ntop(AF_INET, &(from.sin_addr), IP, INET_ADDRSTRLEN);    
    printf("ip is %s\n",IP);
    strcpy( &(sta->ipaddress[0]), IP);
    sta->cmd =  testcase->cmd;
    sta->cookie = 0;
    sta->nsent = 0;
    sta->nerr = 0;
    sta->msgno = 0;
    sta->myid = id;
    sta->alreadyCleared = 0;
    printf("new_station: size(%d) id=%02d IP address:%s\n", nBytes, id, sta->ipaddress);
    printf( "New STA = %d\n", nsta );
    sta->state = 0;
    sta->statefunc = consoleProcStatetbl[sta->cmd];
}
else
{
    if((id = get_sta_id(from.sin_addr.s_addr))<0)
    {
        printf("\r\n Unexpected message %d rcd\n",rmsg[10]);
        continue;
    }
        sta = &stations[id];

}
if(rmsg[10] == APTS_RESET)
{
	//reset_recd = 1;
    //WfaConResetAll();
    continue;
}
if(rmsg[10] == APTS_RESET_STOP)
{
	printf("\r\n Recd Kill from Sta\n");
	exit(0);
}

func = (sta->statefunc)[sta->state];
if(!sta->alreadyCleared) 
func.statefunc(sta,rmsg,nBytes);

	}





}
