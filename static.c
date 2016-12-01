#include "define.h"

void flowStatic()
{
	int number, total_number = 0, max_num;
	node_t * pointer;
	node_t * tmp;

	max_num = ENTRY_MAX / WAY_MAX;
//	max_num = 3;
//	intervalFlowCounter();	

	for ( number = 0 ; number < max_num ; number = number + 1 )
	{
		pointer = head_static[number]->next;
		fprintf( stdout, "==========INDEX%03d==========\n", number );
		while( pointer != NULL )
		{
	//		fprintf( stdout, "%s %s %s %d %d first:%f\n", pointer->entry.srcip, pointer->entry.dstip,
	//				pointer->entry.protcol, pointer->entry.srcport, pointer->entry.dstport, pointer->entry.reach_time );
			total_number = listSearchStatic( pointer, number );/* pointer$B$O%j%9%H$N@hF,MWAG(B*/
			printValueStatic( pointer, total_number );
//			printTimeRelative( pointer );

//			pointer = freeListStatic( pointer ); //$B%a%b%jMFNL$,B-$j$J$$$H$-$N6lFy$N:v(B
			pointer = pointer->next;
		}

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
	for ( number = 0 ; number < ENTRY_MAX / WAY_MAX ; number = number + 1 )
	{
		pointer = head_static[number]->next;
		fprintf( stdout, "==========INDEX%03d==========\n", number );
		while( pointer != NULL )
		{
			fprintf( stdout, "%s %s %s %d %d\n", pointer->entry.srcip, pointer->entry.dstip, 
					pointer->entry.protcol, pointer->entry.srcport, pointer->entry.dstport );
			pointer = pointer->next;
		}

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

/* $B%U%m!<(BID$B$H%Q%1%C%H?t(B, $B%Q%1%C%H4V3V$rJ];}$9$k%j%9%H$rJ];}$9$k%j%9%H$N=i4|2=$r9T$&(B */
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
		head_static[index_number]->search_flag = 0;
//		TimeListInit( head_static[index_number] );
		//$BCM$OBeF~$7$F$*$/$Y$-!)(B
		//$B=i$a$O:G8e$N%N!<%I$r;X$9%]%$%s%?$b@hF,%N!<%I$r;X$7$F$*$/(B
		
		p_static[index_number] = head_static[index_number];
		/*
		strcpy( init_tapple.srcip, "0" );
		strcpy( init_tapple.dstip, "0" );
		strcpy( init_tapple.protcol, "0" );
		init_tapple.srcport = 0;
		init_tapple.dstport = 0;
		*/
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
	listStaticSubstitute( newnode );
	newnode->search_flag = 0;
//	TimeListInit( newnode );

	newnode->next = NULL;
	newnode->prev = p_static[number];
	p_static[number] = newnode;
}

void listStaticSubstitute( node_t * node )
{
	node->flow_interval = 0;
	node->diff_of_time = -1;
}

//////////////////////////////////////////////////
/* $BF~NO(Bx$B$HEy$7$$%(%s%H%j$r;}$D%N!<%I$rC5$9(B      */
/* $BC5$9%j%9%H$O(B, $BF~NO(Bnumber$B$NCM$K$h$C$F7hDj$9$k(B */
/* search_pointer$B$O%j%9%H$N@hF,MWAG$r;X$9(B       */
//////////////////////////////////////////////////
int listSearchStatic( node_t * search_pointer, int number )
{

	if ( search_pointer->search_flag == 1 )
	{	//search_flag$B$,(B1$B$N>l9g$O8!:w:Q$_$G$"$k$3$H$r<($9(B
		//$B8!:w:Q$_$N$b$N$O:F8!:wITMW$J$N$G(B, $B=hM}$r%9%-%C%W$9$k$3$H$,$G$-$k(B
		return 0;
	}

	int total_number = 1;
	int flow_interval_number = 0;
	double tmp_time;
	double diff;
	node_t * pointer;
	node_t * tmp_pointer;

	another_node_t * another_tmp_list = malloc( sizeof( another_node_t ) ); 
	//$B%U%m!<4V$NMWAG$N2r@O$r9T$&$?$a$N2>$N%j%9%H$N@hF,MWAG$r;X$9%j%9%H(B
	anotherListInit( another_tmp_list );

	tmp_time = search_pointer->entry.reach_time;
	pointer = search_pointer->next;//$BC5$9MWAG$N<!$NMWAG$+$iC5$9(B
//	fprintf( stdout, "%f, %s, %s, %s, %d, %d, %d, %f\n",search_pointer->entry.reach_time, search_pointer->entry.srcip, search_pointer->entry.dstip, 
//			search_pointer->entry.protcol, search_pointer->entry.srcport, search_pointer->entry.dstport, search_pointer->flow_interval, search_pointer->diff_of_time );

	while( pointer != NULL )
	{
	//	printAnotherList( another_tmp_list );
		if ( isEqual( search_pointer->entry, pointer ) == EQUAL )
		{
			total_number = total_number + 1;//$BF10l%U%m!<$NAm?t$r7W;;$9$k(B

			pointer->flow_interval = flow_interval_number;//$BF10l%U%m!<4V$N%U%m!<$N<oN`$rBeF~(B
			flow_interval_number = 0;//$BF10l%U%m!<4V$N%U%m!<$N<oN`$N=i4|2=(B

			//diff$B$K$O0l$DA0$N%U%m!<$H8=:_$N%U%m!<$N;~4V4V3V$rBeF~(B
			//tmp_time$B$K$O0l$DA0$N%U%m!<$N;~4V$,F~$C$F$$$k(B
			diff = pointer->entry.reach_time - tmp_time;
			pointer->diff_of_time = diff;
			
			//TimeListInsert( search_pointer, diff );//$B;~4V4V3V$r;~4V4V3V%j%9%H$KDI2C(B
			tmp_time = pointer->entry.reach_time;
			//pointer = listDeleteStatic( pointer, number );

			pointer->search_flag = 1; //$BC5:w$,=*N;$7$F$$$k$3$H$r<($9(B
		}
		else
		{	//search_pointer$B$H$O0c$&(B5$B%?%W%k$r;}$D%U%m!<$G$"$k>l9g(B
			if ( isRegisteredStaticList( pointer->entry, another_tmp_list ) == NULL )
			{
				anotherListInsert( pointer->entry, another_tmp_list );
				flow_interval_number = flow_interval_number + 1;
			}
		}
		pointer = pointer->next;
	}
	
	if ( search_pointer->diff_of_time == -1 )
	{
		//$BB>$KF10l%U%m!<$,8+$D$+$i$J$$$?$a(B, $B;~4V:9$K(B-1$B$,F~$C$?$^$^$N%U%m!<$K4X$7$F$O(B, $B;~4V:9$r(B0$B$H$9$k(B
		//$B$^$?(B, $BF10l%U%m!<4V$N%U%m!<$N<oN`$b(B0$B$H$9$k(B
		search_pointer->diff_of_time = 0;
		search_pointer->flow_interval = 0;
	}

//	printAnotherList( another_tmp_list );
	deleteAnotherList( another_tmp_list );
//	printAnotherList( another_tmp_list );

	//$B8!:w%-!<$NMWAG$N8!:w$,=*N;(B
	search_pointer->search_flag = 1;
	return total_number;
}

//////////////////////////////////////////////////////
/* $B%Q%1%C%H$NE~Ce4V3V$rJ];}$9$k%j%9%H$N=i4|2=$r9T$&(B */
//////////////////////////////////////////////////////
void TimeListInit( node_t * pointer )
{
	pointer->time_relative = malloc( sizeof( time_interval_t ) );
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
	time_newnode = malloc( sizeof( time_interval_t ) );
	
	tmp_time_node = pointer->time_relative; //$B;O$a$N%N!<%I$rBeF~(B
	while( tmp_time_node->next != NULL )
	{
		tmp_time_node = tmp_time_node->next; //$B%j%9%H$N:G=*C<$^$G9T$/(B
	}

	tmp_time_node->next = time_newnode;
	time_newnode->time_interval = interval;
	time_newnode->next = NULL;
}
