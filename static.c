#include "define.h"

void flowStatic()
{
	int number, total_number = 0;
	node_t * pointer;
	node_t * empty;

	for ( number = 0 ; number < ENTRY_MAX / WAY_MAX ; number = number + 1 )
	{
		pointer = head_static[number]->next;
		fprintf( stdout, "==========INDEX%03d==========\n", number );
		while( pointer != NULL )
		{
			total_number = listSearchStatic( pointer, number );
			printValueStatic( pointer, total_number );
			pointer = pointer->next;
		}

	}
}

/* $B=PNO$9$kFbMF$O(B, $B%U%m!<(BID$B$H%U%l!<%`?t(B */
void printValueStatic( node_t * pointer, int number )
{
	fprintf( stdout, "ID:%s %s %s %d %d | frame_number %d\n", pointer->entry.srcip, pointer->entry.dstip, pointer->entry.protcol, pointer->entry.srcport, pointer->entry.dstport, number );
}

/* $B%j%9%H$NMWAG$rA4$F=PNO$9$k(B */
void printValueStaticAll()
{
	int number = 0;
	node_t * pointer;
	for ( number = 0 ; number < ENTRY_MAX / WAY_MAX ; number = number + 1 )
	{
		pointer = head_static[number]->next;
		fprintf( stdout, "==========INDEX%03d==========\n", number );
		while( pointer != NULL )
		{
			fprintf( stdout, "%s %s %s %d %d\n", pointer->entry.srcip, pointer->entry.dstip, pointer->entry.protcol, pointer->entry.srcport, pointer->entry.dstport );
			pointer = pointer->next;
		}

	}

}

/* $BF~NO$N(Bdelete_pointer, number$B$r$b$H$K%j%9%H$KEPO?$5$l$F$$$k%N!<%I$r:o=|$9$k(B */
node_t *  listDeleteStatic( node_t * delete_pointer, int number )
{
	node_t * pointer;
	if ( delete_pointer->next == NULL )
	{	//$B>C$9%N!<%I$,%j%9%H$N0lHV:G8e$N>l9g(B
		p_static[number] = delete_pointer->prev; 
		p_static[number]->next = NULL;
		/* 
		 * p_static[number]$B$r;HMQ$7$J$$J}K!$b9M$($?$,(B, 
		 * p_static$B$G%j%9%H$N0lHV:G8e$r>o$K;X$9J}$,NI$$5$$,$7$?$N$G(B
		 * $B$3$N=q$-J}$r$7$?(B
		 * 
		 * */
	}
	else if ( delete_pointer != NULL )
	{
		delete_pointer->prev->next = delete_pointer->next;
		delete_pointer->next->prev = delete_pointer->prev;
	}
	pointer = delete_pointer->prev;
	free( delete_pointer );

	return pointer;
}

/* list$B$N=i4|2=(B, head$B$O%@%_!<%N!<%I$H$7$?(B */
/* $B$3$N4X?t$OE}7W>pJs$r<h$k$?$a$N%j%9%H$N=i4|2=$r9T$&4X?t(B */
void listInitStatic()
{
	int index_number = 0;
	int way_number = 0;
	tapple_t init_tapple;

	fprintf( stdout, "init for static  finished\n" );
	for ( index_number = 0 ; index_number < INDEX_MAX ; index_number = index_number + 1 )
	{

		head_static[index_number] = malloc( sizeof( node_t ) );
		head_static[index_number]->next = NULL;
		head_static[index_number]->prev = NULL;

		/* head_static$B%]%$%s%?(B,  */
		strcpy( head_static[index_number]->entry.srcip, "0" );
		strcpy( head_static[index_number]->entry.dstip, "0" );
		head_static[index_number]->entry.srcport = 0;
		head_static[index_number]->entry.dstport = 0;
		strcpy( head_static[index_number]->entry.protcol, "0");
		//$BCM$OBeF~$7$F$*$/$Y$-!)(B
		//$B=i$a$O:G8e$N%N!<%I$r;X$9%]%$%s%?$b@hF,%N!<%I$r;X$7$F$*$/(B
		p_static[index_number] = head_static[index_number];

		strcpy( init_tapple.srcip, "0" );
		strcpy( init_tapple.dstip, "0" );
		strcpy( init_tapple.protcol, "0" );
		init_tapple.srcport = 0;
		init_tapple.dstport = 0;
	}
}

/* list$B$K?7$7$/MWAG$r:n@.$9$k;~$K;H$&(B, listMake, listAdd$B$H$+$NJ}$,NI$+$C$?$+$b(B */
/* $B$34X?t$OE}7W>pJs$r<h$k$?$a$N%j%9%H$KMWAG$rDI2C$9$k4X?t(B */
void listInsertStatic( tapple_t x, int number )
{
//	fprintf( stdout, "insert started\n" );
	node_t *newnode;

	newnode = malloc( sizeof( node_t ) );
	p_static[number]->next = newnode;

	listSubstitute( newnode, x );

	newnode->next = NULL;
	newnode->prev = p_static[number];
	p_static[number] = newnode;
}

/* $BF~NO(Bx$B$HEy$7$$%(%s%H%j$r;}$D%N!<%I$rC5$9(B */
/* $BC5$9%j%9%H$O(B, $BF~NO(Bnumber$B$NCM$K$h$C$F7hDj$9$k(B */
int listSearchStatic( node_t * search_pointer, int number )
{
	int total_number = 1;
	node_t * pointer;
	node_t * tmp_pointer;

	pointer = search_pointer->next;
	while( pointer != NULL )
	{
		if ( isEqual( search_pointer->entry, pointer ) == EQUAL )
		{
			total_number = total_number + 1;
			pointer = listDeleteStatic( pointer, number );
		}
		pointer = pointer->next;
	}

	return total_number;
}
