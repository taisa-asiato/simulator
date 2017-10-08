#include "define.h"

void printTuple( tuple_t tuple )
{
	fprintf( stdout, "%s %s %s %d %d\n", 	tuple.srcip, tuple.dstip, tuple.protcol, 
						tuple.srcport, tuple.dstport );
}

void printOnly1pFlow()
{
	node_t * tmp = h_flowlist;
	while ( tmp != NULL )
	{
		if ( tmp->packet_count == 1 )
		{
			printTuple( tmp->entry );
		}
		tmp = tmp->next;
	}
}

void flowListInit()
{
	h_flowlist = malloc( sizeof( node_t ) );
	h_flowlist->packet_count = 0;
	h_flowlist->next = NULL;
	h_flowlist->prev = NULL;
}

int flowListUpdate( tuple_t tuple )
{
	node_t * tmp1;
	tmp1 = h_flowlist;

	while( tmp1->next != NULL )
	{
		if ( isEqual( tuple, tmp1->next ) == EQUAL )
		{
			tmp1->packet_count++;
			return 0;
		}
		tmp1 = tmp1->next;
	}

	tmp1->next = malloc( sizeof( node_t ) );
	tmp1->prev = tmp1;
	tmp1->packet_count = 1;
	listSubstitute( tmp1->next, tuple );

	return 1;
}
