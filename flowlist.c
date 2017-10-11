#include "define.h"

void printTuple( tuple_t tuple )
{
	fprintf( stdout, "%s %s %s %d %d\n", 	tuple.srcip, tuple.dstip, tuple.protcol, 
						tuple.srcport, tuple.dstport );
}

void printOnly1pFlow( node_t * flowary )
{
	int flow_num = 0;
	for ( flow_num = 0 ; flow_num < filerow ; flow_num++ )
	{
		printTuple( flowary[flow_num].entry );
	}
//	return flow_num;
}

void flowListInit()
{
	for ( int i = 0 ; i < INDEX_MAX ; i++ )
	{
		h_flowlist[i] = malloc( sizeof( node_t ) );
		h_flowlist[i]->packet_count = 0;
		h_flowlist[i]->next = NULL;
		h_flowlist[i]->prev = NULL;
	}
}

int flowListSearch( tuple_t tuple, int index )
{
	node_t * tmp;
	tmp = h_flowlist[index];

	while ( tmp != NULL )
	{
		if  ( isEqual( tuple, tmp ) == EQUAL ) 
		{
			if ( tmp->packet_count == 1 ) 
			{
				// 入力フローが1パケットのときには1を返す
				return 1;
			} 
			else if ( tmp->packet_count > 1 )
			{	// 入力フローが1パケットでないときには0を返す
				return 0;
			}
		}
		tmp = tmp->next;
	}

	// 入力フローが見つからない場合には-1を返す
	return -1;
}

int flowListUpdate( tuple_t tuple, int index )
{
	node_t * tmp1;
	node_t * tmp2;
	tmp2 = h_flowlist[index];
	tmp1 = h_flowlist[index]->next;

	while ( tmp1 != NULL )
	{
		if ( isEqual( tuple, tmp1 ) == EQUAL )
		{
			tmp1->packet_count++;
			return 0;
		}
		tmp2 = tmp1;
		tmp1 = tmp1->next;
	}

	tmp1 = malloc( sizeof( node_t ) );
	tmp1->next = NULL;
	tmp1->prev = tmp2;
	tmp2->next = tmp1;
	tmp1->packet_count = 1;
	listSubstitute( tmp1, tuple );

	return 1;
}

void arySubstitute( int index, ary_t * flowary, tuple_t tuple )
{
	strcpy( flowary[index].entry.dstip, tuple.dstip );
	strcpy( flowary[index].entry.srcip, tuple.srcip );
	strcpy( flowary[index].entry.protcol, tuple.protcol );
	flowary[index].entry.dstport = tuple.dstport;
	flowary[index].entry.srcport = tuple.srcport;
	flowary[index].packet_count = 1;
}

int flowAryUpdate( tuple_t tuple, ary_t * flowary, int index )
{
	int i = 0;
	for ( i = 0 ; i < index ; i++ )
	{
		if ( isExist( flowary[i], tuple ) == 0 )
		{
			flowary[i].packet_count++;
			return 0;
		}
	}

	arySubstitute( i, flowary, tuple );
	return 1;
}

int flowArySearch( ary_t * flowary, tuple_t tuple, int index )
{
	int i = 0;
	for ( i = 0 ; i < index ; i++ )
	{
		if ( isExist( flowary[i], tuple ) == 0 )
		{
			return 0;
		}
	}

	return 1;
}

int isExist( ary_t tmp_node, tuple_t tuple )
{
	if ( 	strcmp( tmp_node.entry.srcip, tuple.srcip ) == 0 &&
		strcmp( tmp_node.entry.dstip, tuple.dstip ) == 0 &&
		strcmp( tmp_node.entry.protcol, tuple.protcol ) == 0 &&
		tmp_node.entry.srcport == tmp_node.entry.srcport &&
		tmp_node.entry.dstport == tmp_node.entry.dstport)
	{
		return 0;
	}
	return 1;
}
