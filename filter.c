// user$B$r%V%i%C%/%j%9%H$XEPO?$r9T$&$+$I$&$+$r9T$&4X?t72(B
//
// (1) $B%V%i%C%/%j%9%H$r8!:w(B
// 	$B%V%i%C%/%j%9%H$O!"Aw?.85(BIP$B%"%I%l%9$H!"$=$N(Buser$B$,Aw?.$7$?(B
// 	$B%U%m!<$H$=$N%U%m!<$r9=@.$9$k%Q%1%C%H?t$+$i$J$k(B.
// (2) $BAw?.85(BIP$B$,%V%i%C%/%j%9%H$KEPO?$5$l$F$$$k>l9g$K$O(B, 
// 	(2-1) user$B$,Aw?.$7$?%U%m!<$,(Bmice$B$+$I$&$+$NH=Dj$r9T$&(B
// 		$BH=Dj$r$I$N$h$&$K9T$&$+$,7h$^$C$F$$$J$$(B
//	(2-1-1) user$B$,Aw?.$7$?%U%m!<$,(Bmice$B$G$"$k>l9g$K$O(B, $B%-%c%C%7%e$KEPO?$7$J$$(B
//		$B$^$?(B, $B%V%i%C%/%j%9%H$r%"%C%W%G!<%H$9$k(B( $B2?$r9T$&(B? )
//	(2-1-2) user$B$,Aw?.$7$?%U%m!<$,(Bmice$B$G$J$$>l9g$K$O(B, $B%-%c%C%7%e$KEPO?$9$k(B
//		$B$3$N$H$-$K$b(B, $B%V%i%C%/%j%9%H$r%"%C%W%G!<%H$9$k(B
// (3) $BAw?.85(BIP$B$,%V%i%C%/%j%9%H$KEPO?$5$l$F$$$J$$>l9g$K$O(B, $B?7$?$KEPO?$9$k(B
// 	$BEPO?$O(BLRU$B$rMQ$$$k(B?, $B$I$N$h$&$J$b$N$,NI$$$N$+ITL@(B
//
#include "define.h"

void printBlackList()
{
	black_list_t * tmp = blackuser;
	while ( tmp != NULL )
	{
		fprintf( stdout, "userip:%s flow_number:%d\n", tmp->userip, tmp->flow_number );
		printSentFlow( tmp );
		tmp = tmp->next;
	}
}

void printSentFlow( black_list_t * user_node )
{
	sent_flow_t * tmp_sent_node;
	tmp_sent_node = user_node->blacksentflow;
	while ( tmp_sent_node != NULL )
	{
		fprintf( stdout, "		%s %s %d %d, %d\n", tmp_sent_node->flowid.dstip, tmp_sent_node->flowid.protcol, tmp_sent_node->flowid.dstport, tmp_sent_node->flowid.srcport, tmp_sent_node->count  );
		tmp_sent_node = tmp_sent_node->next;
	}
}

// $B%V%i%C%/%j%9%H#1CJ$a(B, user$B$N$_$rEPO?$9$k(B
int makeBlackList()
{
	int i = 0;
	black_list_t * tmp;
	black_list_t * tmp1;

	blackuser = malloc( sizeof( black_list_t ) );
	initializeBlackUserList( blackuser );

	tmp = blackuser;
	tmp->prev = NULL;

	for ( i = 0 ; i < BLACKUSER_MAX - 1 ; i = i + 1 )
	{
		// user$B$N(BIP$B%"%I%l%9$r=i4|2=(B
		// strcpy( tmp->userip, "0" );
		tmp->next = malloc( sizeof( black_list_t ) );
		initializeBlackUserList( tmp->next );
		tmp1 = tmp;
		tmp = tmp->next;
		tmp->prev = tmp1;
	}

	// $B:G8e$N%N!<%I$O=i4|2=$5$l$J$$$?$a(B
	blackuser_end = tmp;
	tmp->next = NULL;

	makeFlowList();
	return 1;
}

////////////////////////////////////////////////
/* user$B%V%i%C%/%j%9%H$NMWAG$N=i4|2=$r9T$&4X?t(B */
////////////////////////////////////////////////
void initializeBlackUserList( black_list_t * user_node  )
{
		strcpy( user_node->userip, "0" );
		user_node->flow_number = 0;
		user_node->blacksentflow = malloc( sizeof( sent_flow_t ) );
		initializeFlowList( user_node->blacksentflow );
}

//////////////////////////////////////////////////////////////
/* $B%V%i%C%/%j%9%H$KEPO?$5$l$?(Buser$B$N%U%m!<$N=i4|2=$r9T$&4X?t(B */
//////////////////////////////////////////////////////////////
void initializeFlowList( sent_flow_t * flow_node )
{
	flow_node->count = 0;
	strcpy( flow_node->flowid.srcip, "0" );
	strcpy( flow_node->flowid.dstip, "0" );
	flow_node->flowid.srcport = 0;
	flow_node->flowid.dstport = 0;
	strcpy( flow_node->flowid.protcol, "0" );
}

////////////////////////////////////////////////
/* userが生成したflowを, flow list に追加する関数 */
////////////////////////////////////////////////
sent_flow_t * addFlow( black_list_t * user_node )
{
	sent_flow_t * tmp;
	sent_flow_t * last;

	user_node->flow_number = user_node->flow_number + 1;
	tmp = user_node->blacksentflow;
	fprintf( stdout, "blacksetnflow address is %p\n", tmp );
	while ( tmp != NULL )
	{	// 最後のノードまで移動する
		last = tmp;
		tmp = tmp->next;
	}

	tmp = malloc( sizeof( sent_flow_t ) );
	last->next = tmp;
	tmp->next = NULL;
	return tmp;
}

///////////////////////////////////////////////
/* $B%V%i%C%/%j%9%H(B2$BCJL\(B, $B%U%m!<>pJs$rEPO?$9$k(B */
///////////////////////////////////////////////
int makeFlowList()
{
	int i = 0, j = 0;
	black_list_t * tmp;
	sent_flow_t * tmp_node;

	tmp = blackuser;
	while ( tmp != NULL )
	{
		tmp->blacksentflow = malloc( sizeof( sent_flow_t ) );
		tmp_node = tmp->blacksentflow;
		initializeFlowList( tmp_node );
		tmp_node->next = NULL;
		tmp = tmp->next;
	}

	return 1;
}

void blackListInit()
{
	black_list_t * tmp;
	int i = 0;
	tmp = blackuser;
	while ( tmp != NULL )
	{
		if ( tmp->flow_number > 0 ) 
		{
			deleteFlow( tmp->blacksentflow );
			initializeBlackUserList( tmp );
		}
		i++;
		tmp = tmp->next; 
	}
}

//void printBlackList()
//{
//	black_list_t * tmp_node;
//	int i = 0, j = 0;
//
//	tmp_node = blackuser;
//	while ( tmp_node != NULL )
//	{
//		fprintf( stdout, "NO:%d, userip:%s flownumber:%d\n", i, tmp_node->userip, tmp_node->flow_number );
//		//fprintf( stdout, "NO%d:%s %s %s %d %d ", i, tmp_node->flowid.srcip, tmp_node->flowid.dstip, tmp_node->flowid.protcol, tmp_node->flowid.srcport, tmp_node->flowid.dstport ); 
//		tmp_node = tmp_node->next;
//		i = i + 1;
//	}
//}

////////////////////////////////////////////////////////////////
/* $B%V%i%C%/%j%9%HFb$N8!:w(B, $BEPO?$d:o=|$NA`:n$r9T$&4X?t$NBgK\(B   */
/* return 0:$B%-%c%C%7%e$XEPO?(B, return 1:$B%-%c%C%7%e$KEPO?$7$J$$(B */
////////////////////////////////////////////////////////////////
int blackListOperation( tuple_t tuple )
{
	// search black list
	int i;
	int registerd;
	
	//registerd = filterSearch( tuple );
	if ( registerd == 0 )
	{
	//GblackList
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
/* $BF~NO$N%U%m!<$r@8@.$7$F$$$k(Buser$B$,(B, $B%V%i%C%/%j%9%H$KEPO?$5$l$F$$$k$+C5$94X?t(B */
////////////////////////////////////////////////////////////////////////////////
black_list_t * isUserRegistered( tuple_t tuple )
{
	int i = 0;
	black_list_t * tmp;
	tmp = blackuser;

	while ( tmp != NULL )
	{
		if ( strcmp( tmp->userip, tuple.srcip ) == 0 )
		{
			return tmp;
		}
		tmp = tmp->next;
	}

	return NULL;
}

black_list_t * registUser( tuple_t tuple )
{
	int i = 0, registerd = 0;
	black_list_t * tmp;

	tmp = blackuser;

	while ( tmp != NULL )
	{
		if ( strcmp( tmp->userip, "0" ) == 0 )
		{
			/* ブラックリストにuserを追加する */
			i = substituteUser( tmp, tuple );
			break;
		}
		tmp = tmp->next;
	}

	return tmp;	
}

int substituteUser( black_list_t * tmp, tuple_t tuple )
{
		strcpy( tmp->userip, tuple.srcip );
		tmp->flow_number = 1;
		tmp->blacksentflow = malloc( sizeof( sent_flow_t ) );
		if ( substituteFlow( tmp->blacksentflow , tuple ) != 0 )
		{
			return -1;
		} 
		return 0;
}

int substituteFlow( sent_flow_t * flow_node, tuple_t tuple  )
{
	flow_node->count = flow_node->count + 1;
	strcpy( flow_node->flowid.srcip, tuple.srcip );
	strcpy( flow_node->flowid.dstip, tuple.dstip );
	flow_node->flowid.srcport = tuple.srcport;
	flow_node->flowid.dstport = tuple.dstport;
	strcpy( flow_node->flowid.protcol, tuple.protcol );
	return flow_node->count;;
}

//////////////////////////////////////////////////////////////////////////
/* $BEPO?$5$l$?%U%m!<$N9=@.%Q%1%C%H?t$,ogCM$rD6$($?$+$I$&$+$rH=Dj$9$k4X?t(B */
//////////////////////////////////////////////////////////////////////////
int isFlowCountOverThreshold( black_list_t user )
{
	// $B%U%m!<$,EPO?$5$l$F$$$k>l9g(B
	if ( user.flow_number >= THRESHOLD )
	{
		// $B%U%m!<$,(BTHRESHOLD$B8D0J>e$N%Q%1%C%H$G9=@.$5$l$F$$$k(B
		initializeFlowList( user.blacksentflow );
		return 0;
	}
	else if ( user.flow_number < THRESHOLD )
	{
		// $B%U%m!<$,(BTHRESHOLD$B8DL$K~$N%Q%1%C%H$G$"$k(B
		user.flow_number++;
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////
/* $B%V%i%C%/%j%9%H$KEPO?$5$l$?(Buser$B$,@8@.$7$F$$$k%U%m!<$H$NHf3S$r9T$&(B */
//////////////////////////////////////////////////////////////////////
int isFlowRegistered( black_list_t * node, tuple_t tuple )
{
	sent_flow_t * tmp;
	tmp = node->blacksentflow;

	while ( tmp != NULL )
	{
		if ( 	strcmp( tmp->flowid.dstip, tuple.dstip ) == 0 &&
				strcmp( tmp->flowid.srcip, tuple.srcip ) == 0 && 
				strcmp( tmp->flowid.protcol, tuple.protcol ) == 0 &&
				tmp->flowid.srcport == tuple.srcport && 
				tmp->flowid.dstport == tuple.dstport  )
		{
			tmp->count = tmp->count + 1;
			return 0;
		}
		tmp = tmp->next;
	}
			
	return 1;
}
////////////////////////////////////////////////////
/* $B%V%i%C%/%j%9%H$KEPO?$5$l$?%U%m!<$r:o=|$9$k4X?t(B */
////////////////////////////////////////////////////
int removeFlow( sent_flow_t * remove_node, black_list_t * user_node )
{
	// $BEPO?$5$l$F$$$k%U%m!<?t$r8:>/$5$;$k(B
	user_node->flow_number = user_node->flow_number - 1;
	if ( user_node->flow_number == 0 )
	{
		// $B$3$N$H$-(B, user$B$,@8@.$7$?%U%m!<$,:o=|$5$l$k(B
		// user$B$r%V%i%C%/%j%9%H$+$i:o=|$9$kI,MW$,$"$k(B
		//removeUser( user_node );
	}
}
int deleteFlow( sent_flow_t * flow_node )
{
	sent_flow_t * tmp;
	sent_flow_t * tmp1;
	int i = 0;
	tmp = flow_node;

	while ( tmp != NULL )
	{
		fprintf( stdout, "%d\n", i );
		tmp1 = tmp->next;
		free( tmp );
		tmp = tmp1;
		i++;
	}

	return i;
}
//////////////////////////////////////////////////
/* $B%V%i%C%/%j%9%H$KEPO?$5$l$?(Buser$B$r:o=|$9$k4X?t(B */
//////////////////////////////////////////////////
//void removeUser( black_list_t * user_node )
//{
	//blackListSwap( user_node );
//}

//////////////////////////////////////////////
/* $B%V%i%C%/%j%9%H$N%(%s%H%j$rF~$lBX$($k4X?t(B */
//////////////////////////////////////////////
//void blackListSwap( black_list_t * user_node )
//{
//	black_list_t * tmp;
//	if ( user_node == blackuser )
//	{
//		// $B%V%i%C%/%j%9%H$N:G=i$NMWAG$@$C$?>l9g(B
//		tmp = blackuser;
//		blackuser = blackuser->next;
//		blackuser->prev = NULL;
//
//		tmp->prev = blackuser_end;
//		blackuser_end = tmp;
//		tmp->next = NULL;
//	}
//	else if ( user_node == blackuser_end )
//	{
//		// $B%V%i%C%/%j%9%H$N:G8e$NMWAG$N>l9g(B
//		// $B:G8e$NMWAG$O=i4|2=$r9T$&(B( )
//		initializeFlowList( user_node->blacksentflow );
//		initializeBlackUserList( user_node );
//	}
//	else
//	{
//		tmp = user_node;
////		user_node;
//	}
//}
//