#include "define.h"

void flowStaticMain()
{
//	flowStaticForSingle();
	flowStaticForParallel();
}

/* $B2r@O$rC`<!=hM}$G9T$&(B */
void flowStaticForSingle()
{
	int number, total_number = 0, max_num;
	node_t * pointer;
	node_t * tmp;

	max_num = ENTRY_MAX / WAY_MAX;
//	max_num = 3;
//	intervalFlowCounter();	

	for ( number = 12 ; number < 13 ; number = number + 1 )
	{
		pointer = head_static[number]->next;
		fprintf( stdout, "==========INDEX%03d==========\n", number );
		while( pointer != NULL )
		{
			listSearchStatic( pointer->entry, number );/* pointer$B$O%j%9%H$N@hF,MWAG(B*/
			printValueStatic( pointer, total_number );
//			printTimeRelative( pointer );

//			pointer = freeListStatic( pointer ); //$B%a%b%jMFNL$,B-$j$J$$$H$-$N6lFy$N:v(B
			pointer = pointer->next;
		}

	}
}

/* $B2r@O$rJBNs$K9T$&(B */
void flowStaticForParallel()
{
	int number, total_number = 0, max_num;
	node_t * pointer[256];
	node_t * tmp;

	max_num = ENTRY_MAX / WAY_MAX;

	#pragma omp parallel for
	for ( number = 0 ; number < max_num ; number = number + 1 )
	{
		pointer[number] = head_static[number]->next;
		while( pointer[number] != NULL )
		{
	//		fprintf( stdout, "%s %s %s %d %d first:%f\n", pointer->entry.srcip, pointer->entry.dstip,
	//				pointer->entry.protcol, pointer->entry.srcport, pointer->entry.dstport, pointer->entry.reach_time );
			listSearchStatic( pointer[number]->entry, number );/* pointer$B$O%j%9%H$N@hF,MWAG(B*/
//			printValueStatic( pointer[number], total_number );
//			printTimeRelative( pointer );
//			pointer = freeListStatic( pointer ); //$B%a%b%jMFNL$,B-$j$J$$$H$-$N6lFy$N:v(B
			pointer[number] = pointer[number]->next;
		}
		fprintf( stdout, "==========INDEX%03d========== completed\n", number );

	}

}

/* $BE}7W>pJs$rJ];}$9$k%j%9%H$NMWAG$r:o=|$9$k4X?t(B */
/* $B<g$K%a%b%jMFNL$,>/$J$$$H$-$N$3$H$r9M$($?$b$N(B */
node_t * freeListStatitc( node_t * pointer )
{
	//pointer$B$O8!:::Q$_$NE}7W>pJs%j%9%H$NMWAG(B
	node_t * tmp;
	tmp = pointer->next;
	free( pointer ); 

	//$BJV$jCM$O(B, $BE}7W>pJs%j%9%H$N$&$A(B, $B0lHV:G6a8!::$,=*$o$C$?$b$N$N<!$NMWAG(B
	return tmp;
}

/* $B=PNO$9$kFbMF$O(B, $B%U%m!<(BID$B$H%U%l!<%`?t(B */
void printValueStatic( node_t * pointer, int number )
{
	fprintf( stdout, "%f, %s, %s, %s, %d, %d, %d, %f\n",pointer->entry.reach_time, pointer->entry.srcip, pointer->entry.dstip, 
			pointer->entry.protcol, pointer->entry.srcport, pointer->entry.dstport, pointer->flow_interval, pointer->diff_of_time );
}

/* $B%j%9%H$NMWAG$r(Bindex$BKh$KA4$F=PNO$9$k(B */
void printValueStaticAll()
{
	int number = 0;
	node_t * pointer;
	pointer = analyze->next;

	while( pointer != NULL )
	{
//		if ( pointer->crcnum == 0 )
//		{
		fprintf( stdout, "%f, %s, %s, %s, %d, %d, %d, %f, %d\n",pointer->entry.reach_time, pointer->entry.srcip, pointer->entry.dstip, 
				pointer->entry.protcol, pointer->entry.srcport, pointer->entry.dstport, pointer->crc_flow_interval, pointer->diff_of_time, pointer->crcnum );
//		}
		pointer = pointer->next;
	}
}

//////////////////////////////////////////////////
/* $B;~4V4V3V$rJ];}$9$k%j%9%H$NMWAG$rA4$F=PNO$9$k(B */
//////////////////////////////////////////////////
void printTimeRelative( node_t * pointer )
{
	time_interval_t * tmp_time;
	tmp_time = pointer->time_relative;
	while( tmp_time != NULL )
	{
		fprintf( stdout, "%f -> ", tmp_time->time_interval );
		tmp_time = tmp_time->next;
	}

	fprintf( stdout, "\n" );
}

////////////////////////////////////////////////////
/* $BF~NO$N(Bdelete_pointer$B$r>C$94X?t(B                 */
/* number$B$K$h$C$F$I$N%]%$%s%?G[Ns$J$N$+$r;XDj$9$k(B */
/* $BJV$jCM$H$7$F(B, $B>C$7$?MWAG$ND>A0MWAG$rJV$9(B       */
////////////////////////////////////////////////////
node_t * listDeleteStatic( node_t * delete_pointer, int number )
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
//	pointer = delete_pointer->prev;
	free( delete_pointer );

	return pointer;
}

/* $B%U%m!<(BID$B$H%Q%1%C%H?t(B, $B%Q%1%C%H4V3V$rJ];}$9$k%j%9%H$rJ];}$9$k%j%9%H$N=i4|2=$r9T$&(B */
void listInitStatic()
{
	int index_number = 0;
	int way_number = 0;
	tuple_t init_tuple;

	fprintf( stdout, "init for static  finished\n" );
	for ( index_number = 0 ; index_number < INDEX_MAX ; index_number = index_number + 1 )
	{

		head_static[index_number] = ( node_t * )malloc( sizeof( node_t ) );
		head_static[index_number]->next = NULL;
		head_static[index_number]->prev = NULL;

		/* head_static$B%]%$%s%?(B,  */
		strcpy( head_static[index_number]->entry.srcip, "0" );
		strcpy( head_static[index_number]->entry.dstip, "0" );
		head_static[index_number]->entry.srcport = 0;
		head_static[index_number]->entry.dstport = 0;
		strcpy( head_static[index_number]->entry.protcol, "0");
//		head_static[index_number]->search_flag = 0;
		//		TimeListInit( head_static[index_number] );
		//$BCM$OBeF~$7$F$*$/$Y$-!)(B
		//$B=i$a$O:G8e$N%N!<%I$r;X$9%]%$%s%?$b@hF,%N!<%I$r;X$7$F$*$/(B

		p_static[index_number] = head_static[index_number];
	}

	analyze = ( node_t * )malloc( sizeof( node_t ) );
	strcpy( analyze->entry.dstip, "0" );
	strcpy( analyze->entry.srcip, "0" );
	strcpy( analyze->entry.protcol, "0" );
	analyze->entry.srcport = 0;
	analyze->entry.dstport = 0;
	analyze->next = NULL;
	analyze->prev = NULL;
	analyze_end = analyze;

	static_search = ( node_t * )malloc( sizeof( node_t ) );
	strcpy( static_search->entry.dstip, "0" );
	strcpy( static_search->entry.srcip, "0" );
	strcpy( static_search->entry.protcol, "0" );
	static_search->entry.srcport = 0;
	static_search->entry.dstport = 0;
	static_search->next = NULL;
	static_search->prev = NULL;
	search_end = static_search;
}

/* list$B$K?7$7$/MWAG$r:n@.$9$k;~$K;H$&(B, listMake, listAdd$B$H$+$NJ}$,NI$+$C$?$+$b(B */
/* $B$34X?t$OE}7W>pJs$r<h$k$?$a$N%j%9%H$KMWAG$rDI2C$9$k4X?t(B 		       */
/* $BJV$jCM$N(B1$B$O(B, $B%j%9%H$NMWAG?t$r(B1$BA}$d$7$?;v$r<($9(B 			       */
int listInsertStatic( node_t * end, tuple_t x, int number )
{
	node_t *newnode;
	newnode = ( node_t * )malloc( sizeof( node_t ) );

	//5$B%?%W%k$N>pJs$rBeF~(B
	listSubstitute( newnode, x );
	newnode->flow_interval = 0;
	newnode->diff_of_time = 0;
	newnode->crcnum = number;
	newnode->crc_flow_interval = 0;

	if ( analyze_end == end )
	{
		//$B2r@OMQ$NCM$N=i4|2=(B
		analyze_end->next = newnode;

		//$B%]%$%s%?IU$1BX$((B
		newnode->next = NULL;
		newnode->prev = analyze_end;
		analyze_end = newnode;
	}
	else if ( search_end == end )
	{
		//$B2r@OMQ$NCM$N=i4|2=(B
		search_end->next = newnode;

		//$B%]%$%s%?IU$1BX$((B
		newnode->next = NULL;
		newnode->prev = search_end;
		search_end = newnode;
	}
	else if ( p_static[number] == end )
	{
		p_static[number]->next = newnode;
		newnode->next = NULL;
		newnode->prev = p_static[number];
		p_static[number] = newnode;
	}
	
//	fprintf( stdout, "listInsertStatic finished\n" );
	//$B%j%9%H$NMWAG$N8D?t$r(B1$B%+%&%s%H%"%C%W$9$k(B?
	return 1;
}

/* $BBeF~4X?t$G$O$J$$$N$G(B, $BL>A0$rJQ99$9$kI,MW$,$"$k(B */
void listStaticSubstitute( node_t * node )
{
	node->flow_interval = 0;
//	newnode->crcnum = number;
	node->crc_flow_interval = 0;
	node->diff_of_time = 0;
}

//////////////////////////////////////////////////
/* $BF~NO(Bx$B$HEy$7$$%(%s%H%j$r;}$D%N!<%I$rC5$9(B      */
/* $BC5$9%j%9%H$O(B, $BF~NO(Bnumber$B$NCM$K$h$C$F7hDj$9$k(B */
/* search_pointer$B$O%j%9%H$N@hF,MWAG$r;X$9(B       */
//////////////////////////////////////////////////
void listSearchStatic( tuple_t search_tuple, int number )
{
	node_t * pointer;
	int tmp_crc = 0;
	int tmp_interval = 0;
	int registered = 0;
	double tmp_diff_time;
	double tmp_crc_time;

	if ( p_static[number] == head_static[number] )
	{	//
		listInsertStatic( p_static[number], search_tuple, number );
	}

	pointer = p_static[number];
	while ( pointer != head_static[number] )
	{
		if ( isEqual( search_tuple,  pointer ) == EQUAL )
		{
			//$B8!::MQ%j%9%H$KEPO?$5$l$F$$$k;~(B
			registered = 1;
	//		fprintf( stdout, "registered\n" );
			tmp_diff_time = search_tuple.reach_time - pointer->entry.reach_time;
//			tmp_crc_time = search_tuple.reach_time - tmp_crc_time;;

			//pointer$B$NMWAG$N:o=|(B
			if ( pointer == p_static[number] )
			{	//pointer$B$,%j%9%H$N:G8e$NMWAG$N(BID$B$HF1$8>l9g$O(B, $B:o=|(B->$BDI2C$G$O$J$/(B, $BCM$NF~$lBX$($G=*N;(B
				p_static[number]->entry.reach_time = search_tuple.reach_time;
		
			}
			else 
			{
				//$B%j%9%H$N:G8e$NMWAG$K$3$N%(%s%H%j$rDI2C(B
				listDeleteStatic( pointer, number );
				listInsertStatic( p_static[number], search_tuple, number );
			}
			break;
		}
		else 
		{
			//$BEPO?$5$l$F$$$J$$;~(B
//			fprintf( stdout, "not registered\n" );
			tmp_interval = tmp_interval + 1;
			if ( number == pointer->crcnum )
			{
				tmp_crc = tmp_crc + 1;
				tmp_crc_time = pointer->entry.reach_time;
			}
		}
		pointer = pointer->prev;
	}

	if ( registered == 0 )
	{
		listInsertStatic( p_static[number], search_tuple, number );
		analyze_end->flow_interval = 0;
		analyze_end->crc_flow_interval = 0;
		analyze_end->diff_of_time = 0;
	}
	else if ( registered == 1 )
	{
		analyze_end->flow_interval = tmp_interval;
		analyze_end->diff_of_time = tmp_diff_time;
		analyze_end->crc_flow_interval = tmp_crc;
		analyze_end->diff_of_crc_time = tmp_crc_time;
	}

//	fprintf( stdout, "listSearchStatic finished\n" );
}

//////////////////////////////////////////////////////
/* $B%Q%1%C%H$NE~Ce4V3V$rJ];}$9$k%j%9%H$N=i4|2=$r9T$&(B */
//////////////////////////////////////////////////////
void TimeListInit( node_t * pointer )
{
	pointer->time_relative = ( time_interval_t * )malloc( sizeof( time_interval_t ) );
	pointer->time_relative->time_interval = 0;
	pointer->time_relative->next = NULL;
}

////////////////////////////////////////////////////////////////
/* $B%Q%1%C%H$NE~Ce4V3V$rJ];}$9$k%j%9%H$K(B, $B?7$7$$MWAG$rDI2C$9$k(B */
////////////////////////////////////////////////////////////////
void TimeListInsert( node_t * pointer, double interval )
{

	time_interval_t * time_newnode; //$BDI2C$9$k?7$7$$%N!<%I(B
	time_interval_t * tmp_time_node;
	time_newnode = ( time_interval_t * )malloc( sizeof( time_interval_t ) );
	
	tmp_time_node = pointer->time_relative; //$B;O$a$N%N!<%I$rBeF~(B
	while( tmp_time_node->next != NULL )
	{
		tmp_time_node = tmp_time_node->next; //$B%j%9%H$N:G=*C<$^$G9T$/(B
	}

	tmp_time_node->next = time_newnode;
	time_newnode->time_interval = interval;
	time_newnode->next = NULL;
}
