/*
 *TODO
 *index$B$rA}$d$9(B ( index$B@8@.$N4X?t(B, 5tapple$B$N$I$N$$$A$r%$%s%G%C%/%9$K$9$k$+$N7hDj(B )
 * aaaaa 
 * */
/* header file */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define ENTRY_MAX 20
#define WAY_MAX 4 //way$B?t$N:GBgCM(B
//#define REGISTERED 1;
//#define NOTREGISTERED -1;
#define EQUAL 1;
#define NOTEQUAL -1;

/* 5$B%?%W%k$N>pJs$r;}$D9=B$BN(B */
typedef struct _tapple
{
	char srcip[17];
	char dstip[17];
	int srcport;
	int dstport;
	char protcol[4];
} tapple_t;

/* $BAPJ}8~%j%9%H$N9=B$BN(B */
typedef struct _node
{
	struct _node * next;
	tapple_t entry;
	struct _node * prev;
} node_t;

/* $B%W%m%H%?%$%W@k8@(B */
void listInit();
void listInsert( tapple_t x );
tapple_t stringSplit( char * tapple_string );
void listOperation( tapple_t x );
node_t *  isRegistered( tapple_t inputTapple );
int isEqual( tapple_t inputTapple, node_t * node );
void listDeleteFirst();
void printValue();
void listSubstitute( node_t * pointer, tapple_t x );
char *binaryConvert( tapple_t x, char * bin_tapple );

/* $B%0%m!<%P%kJQ?t(B */
FILE *inputfile; //$BF~NO%U%!%$%k$r;X$9%U%!%$%k%]%$%s%?(B
int entry_size = 0; //$B8=:_$N%(%s%H%j?t$r;X$9(B
int INDEX_MAX = ENTRY_MAX / WAY_MAX;
node_t *head;//[INDEX_MAX]; //$B:G=i$N%(%s%H%j$r;X$9%]%$%s%?(B
node_t *p;//[INDEX_MAX]; //$B8=:_$N%(%s%H%j$r;X$9%]%$%s%?(B

void listSubstitute( node_t * pointer, tapple_t x )
{
	strcpy( pointer->entry.srcip, x.srcip );
	strcpy( pointer->entry.dstip, x.dstip );
	strcpy( pointer->entry.protcol, x.protcol );
	pointer->entry.srcport = x.srcport;
	pointer->entry.dstport = x.dstport;
}

void printValue()
{
	node_t *pointer;
	pointer = p;

	fprintf( stdout, "=======================================================================================================================\n" );
	while( pointer != head )
	{
		fprintf( stdout, "%s %s %s %d %d\n", pointer->entry.srcip, pointer->entry.dstip, pointer->entry.protcol, pointer->entry.srcport, pointer->entry.dstport );
		pointer = pointer->prev; 
	}
	fprintf( stdout, "=======================================================================================================================\n" );
}


/* inputTapple$B$H(B, list$B$N(Bnode$B$N%?%W%k>pJs$,0lCW$9$k$+$I$&$+(B */
int isEqual( tapple_t inputTapple, node_t * node )
{
	if (
			( strcmp( inputTapple.srcip, node->entry.srcip ) == 0 ) &&
			( strcmp( inputTapple.dstip, node->entry.dstip ) == 0 ) &&
			( strcmp( inputTapple.protcol, node->entry.protcol) == 0 ) &&
			( inputTapple.srcport == node->entry.srcport ) &&
			( inputTapple.dstport == node->entry.dstport ) 
	  )
	{
		fprintf( stdout, "matched\n" );
		return EQUAL;
	}
	else 
	{
		return NOTEQUAL;
	}
}

/* inputTapple$B$,(B, $B%j%9%H$N$I$l$+$KEPO?$5$l$F$$$k$+$I$&$+(B */
node_t * isRegistered( tapple_t inputTapple )
{
	node_t *tmp;
	tmp = p;
	while( tmp != head )
	{
		if ( isEqual( inputTapple, tmp ) == 1 )
		{
			return tmp;
		}
		else
		{
			tmp = tmp->prev;
		}
	}

	return NULL;
}

/* list$B$N%(%s%H%j$NA`:n(B, $B%-%c%C%7%e$N%]%j%7!<$K$h$C$FFbMF$,JQ2=(B, $B2<$O(BLRU$B%]%j%7!<(B */
void listOperation( tapple_t x )
{
	//	fprintf( stdout, "listOperation started\n");
	node_t * tmp;
	if ( ( tmp = isRegistered( x ) ) != NULL )
	{
		/* $B@hF,%N!<%I$K$"$k>l9g$K$OM%@hEY$,0lHV9b$$;v$r<($7$F$$$k$N$G2?$b$;$:$K=*N;(B */
		if ( tmp == p )
		{
			;
		}
		else if ( tmp != p )
		{
			if( tmp == head->next )
			{
				/* $B:G=*%N!<%I$K$"$k>l9g(B */
				head->next = tmp->next;
				tmp->next->prev = head;
			}
			else if ( tmp != head->next )
			{
				/* $B:G=*%N!<%I$G$J$$>l9g(B */
				tmp->prev->next = tmp->next;
				tmp->next->prev = tmp->prev;
			}

			p->next = tmp;
			tmp->next = NULL;
			tmp->prev = p;
			p = tmp;
			listSubstitute( tmp, x );

		}
	}
	else
	{
		listDeleteFirst();
		listInsert( x );
	}
}

/* list$B$N=i4|2=(B, head$B$O%@%_!<%N!<%I$H$7$?(B */
void listInit()
{
	fprintf( stdout, "init finished\n" );
	head = malloc( sizeof( node_t ) );
	head->next = NULL;
	head->prev = NULL;

	/* head$B%]%$%s%?(B,  */
	strcpy( head->entry.srcip, "0" );
	strcpy( head->entry.dstip, "0" );
	head->entry.srcport = 0;
	head->entry.dstport = 0;
	strcpy( head->entry.protcol, "0");
	//$BCM$OBeF~$7$F$*$/$Y$-!)(B
	p = head;

	tapple_t init_tapple;
	int node_number = 0;

	strcpy( init_tapple.srcip, "0" );
	strcpy( init_tapple.dstip, "0" );
	strcpy( init_tapple.protcol, "0" );
	init_tapple.srcport = 0;
	init_tapple.dstport = 0;
	
	for ( node_number = 0 ; node_number < WAY_MAX ; node_number = node_number + 1 )
	{
		listInsert( init_tapple );	
	}
}

/* list$B$K?7$7$/MWAG$r:n@.$9$k;~$K;H$&(B, listMake, listAdd$B$H$+$NJ}$,NI$+$C$?$+$b(B */
void listInsert( tapple_t x )
{
//	fprintf( stdout, "insert started\n" );
	node_t *newnode;

	newnode = malloc( sizeof( node_t ) );
	p->next = newnode;

	listSubstitute( newnode, x );

	newnode->next = NULL;
	newnode->prev = p;
	p = newnode;
}

/* list$B$N:G=i$NMWAG$r:o=|$9$k(B, $BM%@hEY$,0lHVDc$$%(%s%H%j$r%-%c%C%7%e$+$iDI$$=P$9A`:n$KEy$7$$(B */
void listDeleteFirst()
{
	node_t *pointer;
	pointer = head->next;
	head->next = pointer->next;
	head->next->prev = head;
	free( pointer );
}


/* $B%U%!%$%k$+$iFI$_<h$C$?(B1$B9T$r6uGr$GJ,3d$79=B$BN$N3F%U%#!<%k%I$KBeF~(B */
tapple_t stringSplit( char *tapple_string )
{
	tapple_t tapple;
	char *cp;
	struct in_addr inp;
	
	cp = strtok( tapple_string, " " );
	memcpy( tapple.srcip, cp, 16 );

	cp = strtok( NULL, " " );
	memcpy( tapple.dstip, cp, 16 );

	cp = strtok( NULL, " " );
	memcpy( tapple.protcol, cp, 3 );

	cp = strtok( NULL, " " );
	tapple.srcport = atoi( cp );

	cp = strtok( NULL, " " );
	tapple.dstport = atoi( cp );

	return tapple;
}

char * binaryConvert( tapple_t x, char * bin_tapple )
{
	struct in_addr inp;
	int tcp, position, i, tmp, j;
	char eight_byte[8];
	unsigned long tmp_ip;

	if ( inet_aton( x.srcip, &inp ) == 1 )
	{
		position = 0;
		for ( i = 0 ; i < 4 ; i = i + 1 )
		{
			for ( j = 0 ; j < 8 ; j = j + 1 )
			{
				if( inp.s_addr % 2 == 1 )
				{
					eight_byte[j] = 49;
				}
				else if ( inp.s_addr % 2 == 0 )
				{
					eight_byte[j] = 48;
				}
				inp.s_addr = inp.s_addr / 2;
			}

			for ( j = 0 ; j < 8 ; j = j + 1 )
			{
				bin_tapple[j + 8*i + position] = eight_byte[7 - j];
			}
		}
	}

	if ( inet_aton( x.dstip, &inp ) == 1 )
	{
		position = 32;
		for ( i = 0 ; i < 4 ; i = i + 1 )
		{
			for ( j = 0 ; j < 8 ; j = j + 1 )
			{
				if( inp.s_addr % 2 == 1 )
				{
					eight_byte[j] = 49;
				}
				else if ( inp.s_addr % 2 == 0 )
				{
					eight_byte[j] = 48;
				}
				inp.s_addr = inp.s_addr / 2;
			}

			for ( j = 0 ; j < 8 ; j = j + 1 )
			{
				bin_tapple[j + 8*i + position] = eight_byte[7 - j];
			}
		}

	}

	if ( strcmp( x.protcol, "TCP" ) == 0 )
	{
		position = 71;
		tcp = 6;
		for ( i = 0 ; i < 8 ; i = i + 1 )
		{
			if( tcp % 2 == 1 )
			{
				bin_tapple[position] = 49;
			}
			else if ( tcp % 2 == 0 )
			{
				bin_tapple[position] = 48;
			}
			position = position - 1;
			tcp = tcp / 2;
		}
	}

	position = 87;
	tmp = x.srcport;
	for ( i = 0 ; i < 16 ; i = i + 1 )
	{
		if( tmp % 2 == 1 )
		{
			bin_tapple[position] = 49;
		}
		else if ( tmp % 2 == 0 )
		{
			bin_tapple[position] = 48;
		}
		position = position - 1;
		tmp = tmp / 2;
	}

	position = 103;
	tmp = x.dstport;
	for ( i = 0 ; i < 16 ; i = i + 1 )
	{
		if( tmp % 2 == 1 )
		{
			bin_tapple[position] = 49;
		}
		else if ( tmp % 2 == 0 )
		{
			bin_tapple[position] = 48;
		}
		position = position - 1;
		tmp = tmp / 2;
	}

	bin_tapple[104] = '\0';
	return bin_tapple;
}

int main( int argc, char *argv[] )
{
	char fivetapple[200];
	char bin_tapple[105];
	char *tmp;
	tapple_t tapple;
	int i = 1;

	listInit();
	if ( ( inputfile = fopen( argv[1], "r" ) ) == NULL )
	{
		fprintf( stdout, "file open error\n" );
		return 0;
	}

	while( fgets( fivetapple, 200, inputfile ) != NULL )
	{
		tapple = stringSplit( fivetapple );
		tmp = binaryConvert( tapple, bin_tapple );
		fprintf( stdout, "NO%d - %s %s %s %d %d\n", i, tapple.srcip, tapple.dstip, tapple.protcol, tapple.srcport, tapple.dstport );
		listOperation( tapple );
//		fprintf( stdout, "%s\n", tmp );
		printValue();
		i = i + 1;
	}

	fclose( inputfile );

	return 0;
	
}
