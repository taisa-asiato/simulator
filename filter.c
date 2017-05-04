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
		fprintf( stdout, "No:%d\n", i );
	}

	// $B:G8e$N%N!<%I$O=i4|2=$5$l$J$$$?$a(B
	blackuser_end = tmp;
	tmp->next = NULL;

	tmp = blackuser;
	i = 0;
	while ( tmp != NULL )
	{
		fprintf( stdout, "%d\n", i );
		tmp = tmp->next;
		i = i + 1;
	}
	return 1;
}

////////////////////////////////////////////////
/* user$B%V%i%C%/%j%9%H$NMWAG$N=i4|2=$r9T$&4X?t(B */
////////////////////////////////////////////////
void initializeBlackUserList( black_list_t * user_node )
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

///////////////////////////////////////////////
/* $B%V%i%C%/%j%9%H(B2$BCJL\(B, $B%U%m!<>pJs$rEPO?$9$k(B */
///////////////////////////////////////////////
int makeFlowList()
{
	int i = 0, j = 0;
	
	sent_flow_t * tmp_node;
	for ( i = 0 ; i < 100 ; i++ )
	{
		blackuser[i].blacksentflow = malloc( sizeof( sent_flow_t ) );
		tmp_node = blackuser[i].blacksentflow;
		initializeFlowList( tmp_node );
		tmp_node->next = malloc( sizeof( sent_flow_t ) );
		tmp_node = tmp_node->next;
	}
		// $B:G8e$N%N!<%I$O=i4|2=$5$l$J$$$?$a(B
		initializeFlowList( tmp_node );
		tmp_node->next = NULL;
		j = j + 1;

	return 1;
}

void blackListInit()
{
	int i = 0, j = 0;
	int retval = 0;

	fprintf( stdout, "makeBlackList started\n" );
	retval = makeBlackList();
	fprintf( stdout, "makeBlackList finished\n" );
	//retval = makeFlowList();
	// $B%V%i%C%/%j%9%H$NFbMF$r=PNO$9$k(B
	// $B3NG'MQ(B
	// printBlackList();
	fprintf( stdout, "blackListInit was finished\n" );
}

void printBlackList()
{
	black_list_t * tmp_node;
	int i = 0, j = 0;

	tmp_node = blackuser;
	while ( tmp_node != NULL )
	{
		fprintf( stdout, "NO:%d, userip:%s flownumber:%d\n", i, tmp_node->userip, tmp_node->flow_number );
		//fprintf( stdout, "NO%d:%s %s %s %d %d ", i, tmp_node->flowid.srcip, tmp_node->flowid.dstip, tmp_node->flowid.protcol, tmp_node->flowid.srcport, tmp_node->flowid.dstport ); 
		tmp_node = tmp_node->next;
		i = i + 1;
	}
}

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
int isUserRegistered( tuple_t tuple )
{
	int i = 0;
	for ( i = 0 ; i < 100 ; i++ )
	{
		if ( strcmp( blackuser[i].userip, tuple.srcip ) == 0 )
		{
			return i;
		}
	}

	fprintf( stdout, "user was not registerd\n" );
	return -1;
}

//void addUser( tuple_t tuple )
//{
//	int i = 0;
//
//	for ( ; i < 100 ; i++ )
//	{
//		if ( strcmp( tmp->srcip, "0" ) == 0 )
//		{	// userがブラックリストに登録されている場合
//			// userが送信したフロー数の確認
//			strcpy( tmp->srcip, tuple.srcip );
//		}
//	}	
//}

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
		if ( 	strcpy( tmp->flowid.dstip, tuple.dstip ) == 0 &&
			strcpy( tmp->flowid.srcip, tuple.srcip ) == 0 && 
			strcpy( tmp->flowid.protcol, tuple.protcol ) == 0 &&
			tmp->flowid.srcport == tuple.srcport && 
			tmp->flowid.dstport == tuple.dstport  
		   )
		{
			// $B%U%m!<$,EPO?$5$l$F$$$k>l9g(B
			if ( tmp->count >= THRESHOLD )
			{
				// $B%U%m!<$,(BTHRESHOLD$B8D0J>e$N%Q%1%C%H$G9=@.$5$l$F$$$k(B
				initializeFlowList( tmp );
				//removeFlow( tmp, node );
				return 0;
			}
			else if ( tmp->count < THRESHOLD )
			{
				// $B%U%m!<$,(BTHRESHOLD$B8DL$K~$N%Q%1%C%H$G$"$k(B
				tmp->count = tmp->count + 1;
				return 0;
			}
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