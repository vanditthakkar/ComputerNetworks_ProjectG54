#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h> 
#define NO_OF_CARDS 52
#define NO_PLAYERS 4

int cards[4];
int score [4];
char roundSuite ; 
int isHeartBroken = 0;
//card
struct card
{
	int no;
	char suit;
	int id_no;
};

typedef struct card card;

card* get_card(int no, char suit)
{
	card* c = (card*) malloc(sizeof(card));
	c -> no = no;
	c->suit = suit;
	return c;
}

//player
struct player
{
	char* name;
	card** hand;	
};

typedef struct player player;

player* get_player(char* name)
{
	player* p = (player*) malloc(sizeof(player));
	p->name = name;
	p->hand = (card**)malloc(13 * sizeof(card*));
	return p;
}

// swapping cards for randomization
void swap(card *a, card *b){
	char t1=a->suit;
	int t2=a->no;
	int t3 = a->id_no;
	a->suit=b->suit;
	a->no=b->no;
	a->id_no = b->id_no;
	b->suit=t1;
	b->no=t2;
	b->id_no = t3;
}

//printing the deck
void print_deck(card** c,int n)
{
	int i;
	for(i = 0;i<n;i++)
	{
		printf("%c %d %d\n",c[i]->suit,c[i]->no,c[i]->id_no);
	}
}

void print_shuffled_card(card** c, int n)
{
	int i;
	for(i=0;i<NO_OF_CARDS;i++)
	{
		if(c[i]->id_no==n)
		printf("%d\t%c\t%d \n",c[i]->id_no,c[i]->suit,c[i]->no);
	}
	
}

//generate the deck and shuffle randomly
card** shuffleArray(){
	int i;

	card** deck = (card**) malloc(sizeof(card*)*NO_OF_CARDS);
	for(i=0;i< NO_OF_CARDS ;i++)
	{

		deck[i] = (card*) malloc(sizeof(card));
		deck[i] -> no = (i%13)+1;
		deck[i] -> id_no = i + 1;

		if(i<NO_OF_CARDS/4){
			deck[i] -> suit = 'h';
		}
		else if(i<NO_OF_CARDS/2){
			deck[i] ->suit ='s';
		}
		else if(i< (3* NO_OF_CARDS) /4){
			deck[i] -> suit = 'd';
		}
		else {
			deck[i] -> suit = 'c';
		}

	}
	srand(time(NULL));
	for(i=NO_OF_CARDS-1;i>=0;i--){
		int k = rand()%52;
		swap(deck[i],deck[k]);
	}

	return deck;
}

player **p;

//set up players and deal
void set_game(card** c)
{
    p=(player **)malloc(4*sizeof(player *));
	p[0]= get_player("A");
	int i,pos=0;
	for(i = 0;i<13;i++)
		p[0]->hand[i] = c[pos++];

	p[1] = get_player("B");
	for(i = 0;i<13;i++)
		p[1]->hand[i] = c[pos++];

	p[2] = get_player("C");
	for(i = 0;i<13;i++)
		p[2]->hand[i] = c[pos++];

	p[3] = get_player("D");
	for(i = 0;i<13;i++)
		p[3]->hand[i] = c[pos++];

	printf("player 1\n");
	print_deck(p[0]->hand,13);
	printf("player 2\n");
	print_deck(p[1]->hand,13);
	printf("player 3\n");
	print_deck(p[2]->hand,13);
	printf("player 4\n");
	print_deck(p[3]->hand,13);
}


int evaluateCards(card** c_deck){

    int i,max = 0, win=0;
    for(i=0; i<4; i++){
        if(c_deck[cards[i]-1] -> suit == roundSuite ){
            if(cards[i]>max){
                max = cards[i];
                win =i;
            }
        }
    }

    return win;
}




void simulate_one_round(int sock[], int first, card **c_deck)
{
    int current = first%10;
    char send_buf[4];
    char buffer[3];
    int count = 0;
    int gameStarted =0;
    int itr;
    for(itr=0; itr<4; itr++){
        cards[itr] =0;
    }


    while(count!=4)
    {
        memset(send_buf, 0, 4);
        memset(buffer, 0 , 3);
        sprintf(send_buf,"%d",90+current);

        if ( send ( sock[0], send_buf, strlen(send_buf), 0) != strlen(send_buf) )
                printf("[ERROR] Unable to send the data.\n");
        if ( send ( sock[1], send_buf, strlen(send_buf), 0) != strlen(send_buf) )
            printf("[ERROR] Unable to send the data.\n");
        if ( send ( sock[2], send_buf, strlen(send_buf), 0) != strlen(send_buf) )
            printf("[ERROR] Unable to send the data.\n");
        if ( send ( sock[3], send_buf, strlen(send_buf), 0) != strlen(send_buf) )
            printf("[ERROR] Unable to send the data.\n");
        
        if ( recv ( sock[current], buffer, 2, 0) < 0 )
        {
            printf("[ERROR] Connection closed prematurely.\n");
        }
        
        else
        {
            cards[current]=atoi(buffer)-10;
            memset(send_buf, 0, 4);
            sprintf(send_buf,"%d",cards[current]+10);

            if ( send ( sock[0], send_buf, strlen(send_buf), 0) != strlen(send_buf) )
                    printf("[ERROR] Unable to send the data.\n");
            if ( send ( sock[1], send_buf, strlen(send_buf), 0) != strlen(send_buf) )
                printf("[ERROR] Unable to send the data.\n");
            if ( send ( sock[2], send_buf, strlen(send_buf), 0) != strlen(send_buf) )
                printf("[ERROR] Unable to send the data.\n");
            if ( send ( sock[3], send_buf, strlen(send_buf), 0) != strlen(send_buf) )
                printf("[ERROR] Unable to send the data.\n");

            // printf("%d\n",id);
            // print_card(card_deck,id-1);
            printf("Card played: %d\n",cards[current]);
            if(!gameStarted){
                roundSuite = c_deck[cards[current]-1] -> suit ; 
                gameStarted =1;
            }

            current=(current+1)%4;
            count++;


        }
     
    }
    return ;
}











int main(int argc, char *argv[])
{
    if(argc!=5)
    {
        printf("Wrong number of arguments.");
        exit(1);
    }   
    
    card **c_deck = shuffleArray();
    print_deck(c_deck,NO_OF_CARDS);
    set_game(c_deck);
    
    int first;
    int serverSocket[NO_PLAYERS], newSocket[NO_PLAYERS];
    char *recvBuffer, *buffer;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddress;
    socklen_t addr_size;
    int recvBytes = 0, recvNumber = 0, portnum;

    recvBuffer = (char *)malloc(sizeof(char)*10);
    buffer = (char *)malloc(sizeof(char)*10);
    
    int send_buf_len;
    char send_buf[10];
        
    int i,j;
    for(i=0;i<NO_PLAYERS;i++)
    {
        portnum=atoi(argv[i+1]);
        
        /*  Create the network socket.   */
        serverSocket[i] = socket(PF_INET, SOCK_STREAM, 0);
        // Configure settings of the server address
        /* Address family = Internet */
        serverAddr.sin_family = AF_INET;
        /* Set port number */
        serverAddr.sin_port = htons(portnum);
        /* Set IP address to localhost */
        serverAddr.sin_addr.s_addr = inet_addr ( "127.0.0.1" );
        /* Set all bits of the padding field to 0 */
        memset ( serverAddr.sin_zero, '\0', sizeof (serverAddr.sin_zero) );

        memset(recvBuffer, 0, 10);
        memset(buffer, 0, 10);
        
        /*---- Bind the address struct to the socket ----*/
        if ( bind ( serverSocket[i], ( struct sockaddr *) &serverAddr, sizeof ( serverAddr ) )  < 0)
            printf("[ERROR] Socket binding Failed. \n");

        /* Listen on the socket, with 5 max connection requests queued */
        if ( listen ( serverSocket[i], 5 ) ==0 )
            printf ( "Server Socket Initiated. Listening to its clients : \n" );
        else
            printf("[ERROR] More than limit.\n");

        /* Accept call creates a new socket for the incoming connection */
        newSocket[i] = accept ( serverSocket[i], (struct sockaddr *) &clientAddress, &addr_size);
        
        
        for(j = 0;j<13;j++)
        {

            sprintf(send_buf,"%d",p[i]->hand[j]->id_no+10);
            // send_buf_len=strlen(send_buf);
            // if(send_buf_len==1)
            // send_buf[send_buf_len]='*';

            //  printf("[ERROR] Unable to send the data.\n");
            // printf("server side: %s\n",send_buf);
            print_shuffled_card(c_deck,p[i]->hand[j]->id_no);
            //printf("%d\n",p1->hand[i]->id_no);
            if ( send ( newSocket[i], send_buf, strlen(send_buf), 0) != strlen(send_buf) )
                printf("[ERROR] Unable to send the data.\n");
        
            if(p[i]->hand[j]->id_no==41)
                first=90+i;
        }
    }


    // before the game 
    for(i=0; i<4;i++){
        score[i] = 0;
    }
    
    for(i = 0;i<13;i++)
    {
        simulate_one_round(newSocket,first,c_deck);
        int winner = evaluateCards(c_deck);
        first = winner;
        printf("%d\n",winner);
        // int k;
        // for(k=0;k<4;k++){
        //     if(cards[k]== 25)
        //         score[winner] += 12;
        //     else{ 
        //         if(c_deck[cards[k]-1]->suit=='h')
        //             score[winner] ++;
        //     }
        // }

        // printf("Winner is player %d %d\n",winner+1,score[winner]);
        char *send_buf1;
        send_buf1=(char*)calloc(10,sizeof(char));
        // printf("Winner: %d\n",winner);
        sprintf(send_buf1,"%d",winner);
        printf("%s\n",send_buf1);
        if ( send ( newSocket[0], send_buf1, strlen(send_buf1), 0) != strlen(send_buf1) )
            printf("[ERROR] Unable to send the winner data.\n");

        if ( send ( newSocket[1], send_buf1, strlen(send_buf1), 0) != strlen(send_buf1) )
            printf("[ERROR] Unable to send the winner data.\n");

        if ( send ( newSocket[2], send_buf1, strlen(send_buf1), 0) != strlen(send_buf1) )
            printf("[ERROR] Unable to send the winner data.\n");

        if ( send ( newSocket[3], send_buf1, strlen(send_buf1), 0) != strlen(send_buf1) )
            printf("[ERROR] Unable to send the winner data.\n");
    }

    return 0;
}




































// int main(int argc, char* argv[])
// {
// 	if(argc != 5)
// 		return;

// 	card **c = shuffleArray();
// 	print_deck(c,52);	
// 	set_game(c);

// 	int serverSocket = 0, newSocket = 0;
//     char *recvBuffer, *buffer;
//     struct sockaddr_in serverAddr;
//     struct sockaddr_in clientAddress;
//     socklen_t addr_size;
//     int recvBytes = 0, recvNumber = 0;

//     int portnum=atoi(argv[1]);

//     recvBuffer = (char *)malloc(sizeof(char)*10);
//     buffer = (char *)malloc(sizeof(char)*10);

//     /*  Create the network socket.   */
//     serverSocket = socket(PF_INET, SOCK_STREAM, 0);
//     // Configure settings of the server address
//     /* Address family = Internet */
//     serverAddr.sin_family = AF_INET;
//     /* Set port number */
//     serverAddr.sin_port = htons(portnum);
//     /* Set IP address to localhost */
//     serverAddr.sin_addr.s_addr = inet_addr ( "127.0.0.1" );
//     /* Set all bits of the padding field to 0 */
//     memset ( serverAddr.sin_zero, '\0', sizeof (serverAddr.sin_zero) );

//     memset(recvBuffer, 0, 10);
//     memset(buffer, 0, 10);

//     /*---- Bind the address struct to the socket ----*/
//     if ( bind ( serverSocket, ( struct sockaddr *) &serverAddr, sizeof ( serverAddr ) )  < 0)
//         printf("[ERROR] Socket binding Failed. \n");

//     /* Listen on the socket, with 5 max connection requests queued */
//     if ( listen ( serverSocket, 5 ) ==0 )
//         printf ( "Server Socket Initiated. Listening to its clients : \n" );
//     else
//         printf("[ERROR] More than limit.\n");

//     /* Accept call creates a new socket for the incoming connection */
//     newSocket = accept ( serverSocket, (struct sockaddr *) &clientAddress, &addr_size);

//     int i,send_buf_len,first;
//     char send_buf[10];
//     for(i = 0;i<13;i++)
//     {

//     	sprintf(send_buf,"%d",p1->hand[i]->id_no+10);
//     	// send_buf_len=strlen(send_buf);
//     	// if(send_buf_len==1)
//     	// send_buf[send_buf_len]='*';

// 		
// 		// 	printf("[ERROR] Unable to send the data.\n");
// 		// printf("server side: %s\n",send_buf);
//     	print_shuffled_card(c,p1->hand[i]->id_no);
//     	//printf("%d\n",p1->hand[i]->id_no);
// 		if ( send ( newSocket, send_buf, strlen(send_buf), 0) != strlen(send_buf) )
// 			printf("[ERROR] Unable to send the data.\n");
        
//         if(p1->hand[i]->id_no==41)
//             first=90;
// 	}

// 	close ( newSocket );
//     close ( serverSocket );

    



//     int serverSocket1 = 0, newSocket1 = 0;
//     char *recvBuffer1, *buffer1;
//     struct sockaddr_in serverAddr1;
//     struct sockaddr_in clientAddress1;
//     socklen_t addr_size1;
//     int recvBytes1 = 0, recvNumber1 = 0;

//     int portnum1=atoi(argv[2]);

//     recvBuffer1 = (char *)malloc(sizeof(char)*10);
//     buffer1 = (char *)malloc(sizeof(char)*10);

//     /*  Create the network socket.   */
//     serverSocket1 = socket(PF_INET, SOCK_STREAM, 0);
//     // Configure settings of the server address
//     /* Address family = Internet */
//     serverAddr1.sin_family = AF_INET;
//     /* Set port number */
//     serverAddr1.sin_port = htons(portnum1);
//     /* Set IP address to localhost */
//     serverAddr1.sin_addr.s_addr = inet_addr ( "127.0.0.1" );
//     /* Set all bits of the padding field to 0 */
//     memset ( serverAddr1.sin_zero, '\0', sizeof (serverAddr1.sin_zero) );

//     memset(recvBuffer1, 0, 10);
//     memset(buffer1, 0, 10);

//     /*---- Bind the address struct to the socket ----*/
//     if ( bind ( serverSocket1, ( struct sockaddr *) &serverAddr1, sizeof ( serverAddr1 ) )  < 0)
//         printf("[ERROR] Socket binding Failed. \n");

//     /* Listen on the socket, with 5 max connection requests queued */
//     if ( listen ( serverSocket1, 5 ) ==0 )
//         printf ( "Server Socket Initiated. Listening to its clients : \n" );
//     else
//         printf("[ERROR] More than limit.\n");

//     /* Accept call creates a new socket for the incoming connection */
//     newSocket1 = accept ( serverSocket1, (struct sockaddr *) &clientAddress1, &addr_size1);

//     // int send_buf_len;
//     // char send_buf1[10];
//     for(i = 0;i<13;i++)
//     {

//     	sprintf(send_buf,"%d",p2->hand[i]->id_no+10);
//     	// send_buf_len=strlen(send_buf);
//     	// if(send_buf_len==1)
//     	// send_buf[send_buf_len]='*';

// 		
// 		// 	printf("[ERROR] Unable to send the data.\n");
// 		// printf("server side: %s\n",send_buf);
//     	print_shuffled_card(c,p2->hand[i]->id_no);
//     	//printf("%d\n",p1->hand[i]->id_no);
// 		if ( send ( newSocket1, send_buf, strlen(send_buf), 0) != strlen(send_buf) )
// 			printf("[ERROR] Unable to send the data.\n");

//         if(p2->hand[i]->id_no==41)
//             first=91;
// 	}

// 	// close ( newSocket1 );
//     close ( serverSocket1 );



//     int serverSocket2 = 0, newSocket2 = 0;
//     char *recvBuffer2, *buffer2;
//     struct sockaddr_in serverAddr2;
//     struct sockaddr_in clientAddress2;
//     socklen_t addr_size2;
//     int recvBytes2 = 0, recvNumber2 = 0;

//     int portnum2=atoi(argv[3]);

//     recvBuffer2 = (char *)malloc(sizeof(char)*10);
//     buffer2 = (char *)malloc(sizeof(char)*10);

//     /*  Create the network socket.   */
//     serverSocket2 = socket(PF_INET, SOCK_STREAM, 0);
//     // Configure settings of the server address
//     /* Address family = Internet */
//     serverAddr2.sin_family = AF_INET;
//     /* Set port number */
//     serverAddr2.sin_port = htons(portnum2);
//     /* Set IP address to localhost */
//     serverAddr2.sin_addr.s_addr = inet_addr ( "127.0.0.1" );
//     /* Set all bits of the padding field to 0 */
//     memset ( serverAddr2.sin_zero, '\0', sizeof (serverAddr2.sin_zero) );

//     memset(recvBuffer2, 0, 10);
//     memset(buffer2, 0, 10);

//     /*---- Bind the address struct to the socket ----*/
//     if ( bind ( serverSocket2, ( struct sockaddr *) &serverAddr2, sizeof ( serverAddr2 ) )  < 0)
//         printf("[ERROR] Socket binding Failed. \n");

//     /* Listen on the socket, with 5 max connection requests queued */
//     if ( listen ( serverSocket2, 5 ) ==0 )
//         printf ( "Server Socket Initiated. Listening to its clients : \n" );
//     else
//         printf("[ERROR] More than limit.\n");

//     /* Accept call creates a new socket for the incoming connection */
//     newSocket2 = accept ( serverSocket2, (struct sockaddr *) &clientAddress2, &addr_size2);

//     // int send_buf_len;
//     // char send_buf1[10];
//     for(i = 0;i<13;i++)
//     {

//     	sprintf(send_buf,"%d",p3->hand[i]->id_no+10);
//     	// send_buf_len=strlen(send_buf);
//     	// if(send_buf_len==1)
//     	// send_buf[send_buf_len]='*';

// 		
// 		// 	printf("[ERROR] Unable to send the data.\n");
// 		// printf("server side: %s\n",send_buf);
//     	print_shuffled_card(c,p3->hand[i]->id_no);
//     	//printf("%d\n",p1->hand[i]->id_no);
// 		if ( send ( newSocket2, send_buf, strlen(send_buf), 0) != strlen(send_buf) )
// 			printf("[ERROR] Unable to send the data.\n");

//         if(p3->hand[i]->id_no==41)
//             first=92;
// 	}

// 	// close ( newSocket2 );
//     close ( serverSocket2 );



//     int serverSocket3 = 0, newSocket3 = 0;
//     char *recvBuffer3, *buffer3;
//     struct sockaddr_in serverAddr3;
//     struct sockaddr_in clientAddress3;
//     socklen_t addr_size3;
//     int recvBytes3 = 0, recvNumber3 = 0;

//     int portnum3=atoi(argv[4]);

//     recvBuffer3 = (char *)malloc(sizeof(char)*10);
//     buffer3 = (char *)malloc(sizeof(char)*10);

//     /*  Create the network socket.   */
//     serverSocket3 = socket(PF_INET, SOCK_STREAM, 0);
//     // Configure settings of the server address
//     /* Address family = Internet */
//     serverAddr3.sin_family = AF_INET;
//     /* Set port number */
//     serverAddr3.sin_port = htons(portnum3);
//     /* Set IP address to localhost */
//     serverAddr3.sin_addr.s_addr = inet_addr ( "127.0.0.1" );
//     /* Set all bits of the padding field to 0 */
//     memset ( serverAddr3.sin_zero, '\0', sizeof (serverAddr3.sin_zero) );

//     memset(recvBuffer3, 0, 10);
//     memset(buffer3, 0, 10);

//     /*---- Bind the address struct to the socket ----*/
//     if ( bind ( serverSocket3, ( struct sockaddr *) &serverAddr3, sizeof ( serverAddr3 ) )  < 0)
//         printf("[ERROR] Socket binding Failed. \n");

//     /* Listen on the socket, with 5 max connection requests queued */
//     if ( listen ( serverSocket3, 5 ) ==0 )
//         printf ( "Server Socket Initiated. Listening to its clients : \n" );
//     else
//         printf("[ERROR] More than limit.\n");

//     /* Accept call creates a new socket for the incoming connection */
//     newSocket3 = accept ( serverSocket3, (struct sockaddr *) &clientAddress3, &addr_size3);

//     // int send_buf_len;
//     // char send_buf1[10];
//     for(i = 0;i<13;i++)
//     {

//     	sprintf(send_buf,"%d",p4->hand[i]->id_no+10);
//     	// send_buf_len=strlen(send_buf);
//     	// if(send_buf_len==1)
//     	// send_buf[send_buf_len]='*';

// 		
// 		// 	printf("[ERROR] Unable to send the data.\n");
// 		// printf("server side: %s\n",send_buf);
//     	print_shuffled_card(c,p4->hand[i]->id_no);
//     	//printf("%d\n",p1->hand[i]->id_no);
// 		if ( send ( newSocket3, send_buf, strlen(send_buf), 0) != strlen(send_buf) )
// 			printf("[ERROR] Unable to send the data.\n");

//         if(p4->hand[i]->id_no==41)
//             first=93;
// 	}

// 	// close ( newSocket3 );
//     close ( serverSocket3 );

//     return 0;

// }