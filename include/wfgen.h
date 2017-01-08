#ifndef __WFGEN_H__
#define __WFGEN_H__

#define B_D		1
#define	B_H	        2	
#define B_B             3
#define B_M             4
#define M_D             5
#define B_Z             6
#define M_Y             7
#define	L_1		8
#define	A_Y		9		// active mode version of M_Y
#define	B_W		10		// 
#define	A_J		11		// Active test of sending 4 down
#define M_V             12
#define M_U             13
#define A_U             14
#define M_L             15
#define B_K             16
#define M_B             17
#define M_K             18
#define M_W             19
#define LAST_TEST       M_W
#define	APTS_DEFAULT	(LAST_TEST + 0x01)		// message codes
#define	APTS_HELLO	(APTS_DEFAULT+ 0x01)
#define	APTS_BCST	(APTS_HELLO+ 0x01)
#define	APTS_CONFIRM	(APTS_BCST+ 0x01)
#define	APTS_STOP	(APTS_CONFIRM+ 0x01)
#define APTS_CK_BE      (APTS_STOP+ 0x01)
#define APTS_CK_BK      (APTS_CK_BE+ 0x01)
#define APTS_CK_VI      (APTS_CK_BK+ 0x01)
#define APTS_CK_VO      (APTS_CK_VI+ 0x01)
#define APTS_RESET      (APTS_CK_VO+ 0x01)
#define APTS_RESET_RESP (APTS_RESET+ 0x01)
#define APTS_RESET_STOP (APTS_RESET_RESP+ 0x01)
#define APTS_LAST       99
#define	NSTA	20
struct apts_msg {			// 
	char *name;			// name of test
	int cmd;			// msg num
};





struct client;
typedef int (*ConsoleStateFunctionPtr)(struct client*, unsigned int *, int ); 
typedef struct console_state_table
{
ConsoleStateFunctionPtr statefunc;
} consoleProcStatetbl_t;



struct client
{
int cookie;			    
unsigned long s_addr;	   
int nsent, nrecv, nend, nerr,msgno;  
int alreadyCleared;
char ipaddress[20];
int state;
int cmd;
int myid;
consoleProcStatetbl_t  *statefunc;
};




#endif
