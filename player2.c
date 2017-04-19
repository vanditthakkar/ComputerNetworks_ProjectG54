#include <stdio.h>
#include <math.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#define NO_OF_CARDS 52
#define player_id 1

int played[13];
int playedCards=0;
int isHeartBroken=0; 
int emptied[13];
char gameSuite ;
int currentGameCards[4];
//card
struct card
{
	int no;
	char suit;
	int id_no;
};

typedef struct card card;

char player_name[16];

card* get_card(int no, char suit)
{
	card* c = (card*) malloc(sizeof(card));
	c -> no = no;
	c->suit = suit;
	return c;
}

struct deck
{
	card cards[13];
};

struct deck *my_deck;
card** card_deck;
int count = 0;


int validateEntry( int * currentGameCards,int * player_first ){ 

    int itrGameSuite;
    int ifAccepted =0, notExisting = 0, ifSuitePresent=0,  isheartPlayed = 0;

    // for(itrGameSuite=0;itrGameSuite<4; itrGameSuite++){
    //  if(currentGameCards[itrGameSuite]!=-1){
    //      break;
    //  }
    // }

    // if(itrGameSuite!=4){
    //  gameSuite =   card_deck[currentGameCards[(*player_first)] -1 ] -> suit;
    // }


    // in loop of the round if curr_chance
    int cardNo, suit, idNo ;
    printf("Play card in the game\n");

    while(!ifAccepted){
        scanf("%d",&idNo);
        ifAccepted = 0 , notExisting = 0, ifSuitePresent=0, isheartPlayed = 0;
        if(card_deck[idNo-1]->suit =='h'){
            isheartPlayed =1;
        }
        for(int j=0; j<13; j++){

            //printf("%d . ",my_deck->cards[j].id_no);
            if(my_deck->cards[j].id_no == idNo){

                // checking if already emptied
                if(emptied[j]==1) { notExisting=1;  continue;}

                // check that even if existing card , if it's valid

                if(gameSuite>1 && gameSuite != card_deck[idNo-1]->suit) {

                    // check if gameSuite Present

                    for(int k=0; k<13; k++){
                            // check if player has the card
                            if(!emptied[k]){
                                // check if player has the suite of the game suite. IF yes, break it
                                if( gameSuite == my_deck->cards[k].suit ){
                                    ifSuitePresent = 1;
                                    break;
                                }
                            }
                    }

                    if(ifSuitePresent==1){
                        continue;
                    }

                }

                if(!isHeartBroken && isheartPlayed){
                    
                    if(*player_first == player_id)
                        continue;
                    else
                        isHeartBroken =1;
                }


                // empty and accept now
                emptied[j] = 1;
                ifAccepted = 1;
                playedCards++;
                break;
            }


        }

        //printf("%d notExisting\n",notExisting );

        if(!notExisting && !ifAccepted) notExisting = 1;

        //printf("%d notExisting\n",notExisting );


        if(!ifAccepted){
            // this can happen in these conditions
            
            if(ifSuitePresent)
                printf("Hey! the suite %c is present in your deck, choose from that suite only\n",gameSuite);
            else if(!isHeartBroken && isheartPlayed && *player_first == player_id)
                printf("Heart is not broken yet. Play card from some other suite \n");
            else if(notExisting)
                printf("Enter valid ID, You don't have this card Or You have already chosen it\n");

        } 



    }

    // idNo is my final card 

    // if(gameSuite==1){
    //  (*player_first) = player_id;
    // }

    return  idNo;

}


void make_deck()
{
	int i;

	card_deck = (card**) malloc(sizeof(card*)*NO_OF_CARDS);
	for(i=0;i< NO_OF_CARDS ;i++)
	{

		card_deck[i] = (card*) malloc(sizeof(card));
		card_deck[i] -> no = (i%13)+1;
		card_deck[i] -> id_no = i + 1;

		if(i<NO_OF_CARDS/4){
			card_deck[i] -> suit = 'h';
		}
		else if(i<NO_OF_CARDS/2){
			card_deck[i] ->suit ='s';
		}
		else if(i< (3* NO_OF_CARDS) /4){
			card_deck[i] -> suit = 'd';
		}
		else {
			card_deck[i] -> suit = 'c';
		}

	}
}

void print_card(card** c, int n)
{
	
	printf("%d\t%c\t%d\n",c[n]->id_no,c[n]->suit,c[n]->no);
	my_deck->cards[count].id_no = c[n]->id_no;
	my_deck->cards[count].no = c[n]->no;
	my_deck->cards[count].suit = c[n]->suit;
	count++;
}


int* simulate_one_round(int socket)
{
    char recv_chance_buffer[3];
    char send_buffer[4]; // TODO : vandit
    int current_chance;
    int id_no;
    int played_card;
    int player_first=-1;
    int i;
    gameSuite = 1;
    for(i=0; i<4; i++){
            currentGameCards[i]= -1;
    }

    
    for(i = 0;i<4;i++)
    {
        memset(recv_chance_buffer, 0 , 3);
        if ( recv ( socket, recv_chance_buffer, 2, 0) < 0 )
        {
            printf("[ERROR] Connection closed prematurely.\n");
        }

        else
        {
            memset(send_buffer, 0 , 4);
            current_chance = atoi(recv_chance_buffer)%10;

            if(current_chance == player_id)
            {
                // printf("Enter id no\n");
                // scanf("%d",&id_no);
                int id_no = validateEntry(currentGameCards, &player_first);
                // id_no += 10;
                sprintf(send_buffer,"%d",id_no+10);
                if(send(socket,send_buffer,strlen(send_buffer),0) < 0)
                {
                    printf("[ERROR] Unable to send the data.\n");
                }
            }

            memset(recv_chance_buffer, 0 , 3);
            if ( recv ( socket, recv_chance_buffer, 2, 0) < 0 )
            {
                printf("[ERROR] Connection closed prematurely.\n");
            }

            else
            {
                played_card=atoi(recv_chance_buffer)-10;
                printf("Card played: %d\n",played_card);
                currentGameCards [current_chance] = played_card;
            }

            // saaving fitrst player suite 

            if(i==0){
                gameSuite = card_deck[played_card-1]->suit; 
            }

        }
    }
}

int main(int argc, char* argv[])
{
	make_deck();
	int i;
	for(i = 0;i<13;i++)
		played[i] = 1;

	my_deck = (struct deck*) malloc(sizeof(struct deck));
	if(argc != 2)
		return 0;
	int portnum = atoi(argv[1]);

	// Important declaration
    int clientSocket;
    char *buffer;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    buffer = (char *)calloc(10,sizeof(char));

    /*  Create the network socket.   */
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);

    /*  1) PR_INET Internet domain
        2) SOCK_STREAM represents Stream socket
        3) 0 represents Default protocol (TCP in this case)
        */

    // Configure settings of the server address
    /* Address family = Internet */
    serverAddr.sin_family = AF_INET;

    /* Set port number */
    serverAddr.sin_port = htons(portnum);
    /*  In networking we prefer to use big-endian binary 
        sequencing.  Since in our PC we use small endian binary
        scheme, we use htons to convert little endian to big
        endian.  
        */

    /* Set IP address to localhost */
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* Set all bits of the padding field to 0 */
    memset(serverAddr.sin_zero, '\0', sizeof
            serverAddr.sin_zero);

    /* Connect the socket to the server using the address*/
    addr_size = sizeof serverAddr;
    if ( connect( clientSocket, (struct sockaddr *) &serverAddr, 
                addr_size) < 0 )
    {
        printf("[ERROR] Unable to connect to server.\n");
        close( clientSocket );
        exit(0);
    }

    // int i;
    int id=0,pos=0;
    for(i = 0;i<13;i++)
    {
    	// memset(buffer, 0, 10);
    	// buffer = (char *)calloc(2,sizeof(char));
	    if ( recv ( clientSocket, buffer, 2, 0) < 0 )
	    {
	        printf("[ERROR] Connection closed prematurely.\n");
	    }
	    else
	    {
	    	id=atoi(buffer)-10;
	    	// printf("%d\n",id);
	    	print_card(card_deck,id-1);
	    	
	    }
	}

	// // send three cards to left side player 
	// printf("Choose 3 cards to pass : ");
	// for(int i =0; i<3;i++)
	// 	scanf("%d",&);

	for(i=0;i<13;i++)
		emptied[i] = 0;

    int score_player[4];
     for(i = 0;i<4;i++)
        score_player[i] = 0;
    printf("Current Scoreboard\n");
    printf("p1\tp2\tp3\tp4\n");
    printf("%d\t%d\t%d\t%d\n",score_player[0],score_player[1],score_player[2],score_player[3]);



    // Simulate rounds
    for(i = 0;i<13;i++)
    {
        simulate_one_round(clientSocket);
        char recv_winner_buffer[2];
        memset(recv_winner_buffer,0,2);
        if ( recv (clientSocket, recv_winner_buffer, 2, 0) < 0 )
        {
            printf("[ERROR] Connection closed prematurely, winner data not received.\n");
        }
        int current_winner = atoi(recv_winner_buffer)%10;
        printf("winner of current round: player %d\n",current_winner+1 );
        //current_winner--;
        
        int k;
        for(k=0;k<4;k++){
            if(currentGameCards[k]== 25)
                score_player[current_winner] += 12;
            else{ 
                if(card_deck[currentGameCards[k]-1]->suit=='h')
                    score_player[current_winner] ++;
            }
        }

        printf("Current Scoreboard\n");
        printf("p1\tp2\tp3\tp4\n");
        printf("%d\t%d\t%d\t%d\n",score_player[0],score_player[1],score_player[2],score_player[3]);
    }

    int min=score_player[0];
    int min_index=0;
    for(i=1;i<4;i++)
    {
        if(score_player[i]<min)
        {
            min=score_player[i];
            min_index=i;
        }
    }
    printf("THE WINNER IS PLAYER NO. %d. CONGRATULATIONS!!",min_index);

    close( clientSocket );

    return 0;
}