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

void printBlackUser()
{
	black_list_t * tmp = blackuser;
	int i = 0;
	while ( tmp != NULL )
	{
		if ( tmp->isblackuser == 1 )
		{
			fprintf( stdout, "[NO%03d] -- userip:\x1b[33m%s\x1b[39m flow_number:%d\n", i, tmp->userip, tmp->flow_number );
			printSentFlow( tmp );
		}
		tmp = tmp->next;
		i++;
	}
}

void printBlackList()
{
	black_list_t * tmp = blackuser;
	int i = 0;

	while ( tmp != NULL )
	{
		if ( tmp->isblackuser == 1 )
		{
			fprintf( stdout, "[NO%03d] -- userip:\x1b[33m%s\x1b[39m flow_number:%d\n", i, tmp->userip, tmp->flow_number );
		} 
		else 
		{
			fprintf( stdout, "[NO%03d] -- userip:%s flow_number:%d\n", i, tmp->userip, tmp->flow_number );
		}
		printSentFlow( tmp );
		tmp = tmp->next;
		i++;
	}
}

void printBlackListReverse()
{
	black_list_t * tmp = blackuser_end;
	int i = 99;

	while ( tmp != NULL )
	{
		fprintf( stdout, "[NO%03d] -- userip:%s flow_number:%d 1packetflow:%d\n", i, tmp->userip, tmp->flow_number, tmp->onepacket_number );
		printSentFlow( tmp );
		tmp = tmp->prev;
		i--;
	}
}
void printSentFlow( black_list_t * user_node )
{
	sent_flow_t * tmp_sent_node;
	int i = 1;
	tmp_sent_node = user_node->blacksentflow;
	while ( tmp_sent_node != NULL )
	{
		fprintf( stdout, "	|	[NO%03d] -- %s %s %d %d, %d\n", i, tmp_sent_node->flowid.dstip, tmp_sent_node->flowid.protcol, tmp_sent_node->flowid.srcport, tmp_sent_node->flowid.dstport, tmp_sent_node->count  );
		tmp_sent_node = tmp_sent_node->next;
		i++;
	}
}

void printRegisteredBlackList()
{
	black_list_t * tmp = blackuser;
	int i = 0;

	while( tmp != NULL )
	{
		if ( tmp->flow_number == 0 )
		{
			break;
		}
		else
		{
			fprintf( stdout, "[NO%03d] -- userip:%s flow_number:%d 1packetflow:%d\n",i, tmp->userip, tmp->flow_number, tmp->onepacket_number );
			printSentFlow( tmp );
		}
		i++;
		tmp = tmp->next;
	}
}

void mallocFailed()
{
	fprintf( stdout, "malloc was failed\n" );
}

////////////////////////////////////////////////////////////
/* BlackListの作成を行う関数 TODO:汚い, 書き直した方が良い*/
////////////////////////////////////////////////////////////
int makeBlackList()
{
	int i = 0;
	black_list_t * tmp;
	black_list_t * tmp1;

	blackuser = malloc( sizeof( black_list_t ) );
	if ( blackuser == NULL )
	{
		mallocFailed();
	}
	initializeBlackUserList( blackuser );
	makeFlowList( blackuser );
	tmp = blackuser;
	tmp->prev = NULL;

	// 先頭ノードのメモリ確保を既にしているため
	for ( i = 0 ; i < BLACKUSER_MAX - 1 ; i = i + 1 )
	{
		tmp->next = malloc( sizeof( black_list_t ) );
		if ( tmp->next == NULL )
		{
			mallocFailed();
		}
		initializeBlackUserList( tmp->next );
		makeFlowList( tmp->next );
		tmp->next->prev = tmp;
		tmp = tmp->next;
	}

	blackuser_end = tmp;
	blackuser_end->next = NULL;

	return 1;
}

////////////////////////////////////////////////
/* user$B%V%i%C%/%j%9%H$NMWAG$N=i4|2=$r9T$&4X?t(B */
////////////////////////////////////////////////
void initializeBlackUserList( black_list_t * user_node )
{
	strcpy( user_node->userip, "0" );
	user_node->flow_number = 0;
	// isblackuserが0のときはブラックuserではない
	user_node->isblackuser = 0;
	user_node->onepacket_number = 0;
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

//////////////////////////////////
/* flowlist全てを初期化する関数 */
//////////////////////////////////
void initializeAllFlowList( sent_flow_t * flow_node )
{
	sent_flow_t * tmp = flow_node;
	while( tmp != NULL )
	{
		initializeFlowList( tmp );
		tmp = tmp->next;
	}
}

////////////////////////////////////////////////
/* userが生成したflowを, flow list に追加する関数 */
////////////////////////////////////////////////
sent_flow_t * addFlow( black_list_t * user_node )
{
	sent_flow_t * tmp;
	sent_flow_t * last;

	user_node->flow_number = user_node->flow_number + 1;
	user_node->onepacket_number = user_node->onepacket_number + 1;
	tmp = user_node->blacksentflow;

	while ( tmp != NULL )
	{	// 最後のノードまで移動する
		last = tmp;
		tmp = tmp->next;
	}

	tmp = malloc( sizeof( sent_flow_t ) );
	if ( tmp == NULL )
	{
		mallocFailed();
	}
	tmp->prev = last;
	last->next = tmp;
	tmp->next = NULL;
	return tmp;
}

////////////////////////////////////////////////////////
/* FLOW_MAX で指定された数だけFlow listを作成する関数 */
////////////////////////////////////////////////////////
int makeFlowList( black_list_t * user_node )
{
	int i = 0, j = 0;
	sent_flow_t * tmp;
	user_node->blacksentflow = malloc( sizeof( sent_flow_t ) );
	tmp = user_node->blacksentflow;
	tmp->prev = NULL;
	initializeFlowList( tmp );

	for ( i = 0 ; i < FLOW_MAX - 1; i++ )
	{	// FLOWLIST_MAXで指定した数だけflowの登録を行う事ができる
		// 既に1つsent_flow_tのノードができているため, FLOW_MAX-1回だけsent_flow_tのノードを作成する
		tmp->next = malloc( sizeof( sent_flow_t ) );
		initializeFlowList( tmp->next );
		tmp->next->prev = tmp;
		tmp = tmp->next;
	}
	tmp->next = NULL;

	return 1;
}

//////////////////////////////
/* ブラックリストの初期化を行う */
/////////////////////////////
void blackListInit()
{
	black_list_t * tmp_user;
	sent_flow_t * tmp_flow;
	sent_flow_t * tmp1;
	int i = 0;

	tmp_user = blackuser;
	while ( tmp_user != NULL )
	{
		tmp_flow = tmp_user->blacksentflow;
		initializeAllFlowList( tmp_flow );
		initializeBlackUserList( tmp_user );
		i++;
		tmp_user = tmp_user->next; 
	}
}

////////////////////////////////////////////////////////////////
/* $B%V%i%C%/%j%9%HFb$N8!:w(B, $BEPO?$d:o=|$NA`:n$r9T$&4X?t$NBgK\(B   */
/* return 0:$B%-%c%C%7%e$XEPO?(B, return 1:$B%-%c%C%7%e$KEPO?$7$J$$(B */
////////////////////////////////////////////////////////////////
//TODO: if 文ないのブロックが大きいので分割すべき
int blackListOperation( tuple_t tuple )
{
	black_list_t * tmp_black_node;
	sent_flow_t * tmp_sent_flow;
	tuple_t tmp_tuple;
	int i = 0;
	//一定時間ごとにブラックリストの初期化を行う
	if ( blacklist_init_time < tuple.reach_time )
	{
		user_number = 0;
		blackListInit();
		blacklist_init_time = blacklist_init_time + BLACKLIST_INIT_INTERVAL;
	}

	if ( ( tmp_black_node = isUserRegistered( tuple ) ) == NULL )
	{ 	// userip がBlackListに登録されていない場合
		if ( user_number < BLACKUSER_MAX )
		{	// BlackListに登録されているuseripの数が99以下の場合
			tmp_black_node = registUser( tuple );
			user_number = user_number + 1;
			substituteFlow( tmp_black_node->blacksentflow, tuple );
		}
		else 
		{	// blackuser_endに登録されているuserを消去し, 新しいuserを登録し直す
			// 最下位のuserが生成したフローのリストを初期化
			tmp_black_node = blackuser_end;
			initializeAllFlowList( tmp_black_node->blacksentflow );
			// 最下位に登録されているuserのuser情報を初期化 
			initializeBlackUserList( tmp_black_node );
			// user情報及び生成フローを登録
			substituteUser( tmp_black_node, tuple );
			newUserForMaxList();
		}
	}
	else 
	{	//userip がBlackListに登録されている場合
		swapBlackNode( tmp_black_node );
		if ( tmp_sent_flow = isFlowRegistered( tmp_black_node, tuple ) )
		{	//flowが登録されている場合
			tmp_black_node->onepacket_number--;
			// 連続してmissした場合のカウンタの値を0にする
			tmp_black_node->flow_number = 0;
			/*fprintf( stdout, "%s %s %s %d %d\n", tmp_set_flow->flowid.dstip, tmp_sent_flow->flowid.srcip,
			  tmp_sent_flow->flowid.protcol, tmp_sent_flow->flowid.dstport, tmp_sent_flow->flowid.srcport);*/
			initializeFlowList( tmp_sent_flow );
			moveLastFlowNode( tmp_sent_flow, tmp_black_node );
			/*fprintf( stdout, "%s %s %s %d %d\n", tmp_sent_flow->flowid.dstip, tmp_sent_flow->flowid.srcip,
			  tmp_sent_flow->flowid.protcol, tmp_sent_flow->flowid.dstport, tmp_sent_flow->flowid.srcport);*/
		}
		else
		{	// flowが登録されていない場合
			if ( tmp_black_node->onepacket_number < FLOW_MAX )
			{	//flowが登録されておらず, 更にフローリストに空きがある場合
				tmp_sent_flow = tmp_black_node->blacksentflow;
				for ( i = 0 ; i < tmp_black_node->onepacket_number ; i++ )
					tmp_sent_flow = tmp_sent_flow->next;
				// フローリストの登録していない場所まで移動する
				substituteFlow( tmp_sent_flow, tuple );
				tmp_black_node->onepacket_number++;
				tmp_black_node->flow_number++;
			}
			else
			{	//flowlistに空きが無い場合
				// flowリストの先頭ノードをリストの最後に移動
				tmp_sent_flow = moveLastFlowNode( tmp_black_node->blacksentflow, tmp_black_node );
				// リストの最後のノードに5タプルの値を代入
				substituteFlow( tmp_sent_flow, tuple );
				tmp_black_node->flow_number++;
			}
		}

	}


	//  flowの数がしきい値を超えた場合にはblackuserとする
	if (  tmp_black_node->onepacket_number >= THRESHOLD )
	{	
		fprintf( stdout, "user is registered as blackuser\n" );
		tmp_black_node->isblackuser = 1;
	}

	return 0;
}

/* 引数で取ったflow_nodeをリストの一番最後に持っていく */
sent_flow_t * moveLastFlowNode( sent_flow_t * flow_node, black_list_t * user_node )
{
	sent_flow_t * tmp;
	tmp = flow_node;

	while ( tmp->next != NULL )
	{
		tmp = tmp->next;
	}


	if ( user_node->blacksentflow == flow_node )
	{	// flow_nodeがリストの先頭ノードであった場合
		tmp->next = flow_node;
		flow_node->prev = tmp;
		user_node->blacksentflow = flow_node->next;
		user_node->blacksentflow->prev = NULL;
		flow_node->next = NULL;
	}
	else if ( tmp != flow_node )
	{	// flow_nodeがリストの最後のノードではない場合 
		flow_node->next->prev = flow_node->prev;
		flow_node->prev->next = flow_node->next;
		tmp->next = flow_node;
		flow_node->next = NULL;
		flow_node->prev = tmp;
	}
	else if ( tmp == flow_node )
	{	//flow_nodeが最後のノードであった場合
		;
	}
	return flow_node;
}

///////////////////////////////////////////////////////////////////////////////
/* blacklistに最大までuserが登録されている場合に新しく登録するuserが来た場合 */
///////////////////////////////////////////////////////////////////////////////
void newUserForMaxList()
{
	black_list_t * tmp, tmp1;
	tmp = blackuser_end->prev;
	tmp->next = NULL;
	blackuser_end->prev = NULL;
	blackuser_end->next = blackuser;
	blackuser->prev = blackuser_end;
	blackuser = blackuser_end;
	blackuser_end = tmp;
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
			/* ブラックリストにuserを登録する */
			swapBlackNode( tmp );
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
	tmp->onepacket_number = 1;
	tmp->isblackuser = 0;

	if ( substituteFlow( tmp->blacksentflow , tuple ) != 0 )
	{
		return -1;
	} 
	return 0;
}

int substituteFlow( sent_flow_t * flow_node, tuple_t tuple  )
{
	// 代入するときは1をいれるのみ(1packetの意)
	flow_node->count = 1;
	strcpy( flow_node->flowid.srcip, tuple.srcip );
	strcpy( flow_node->flowid.dstip, tuple.dstip );
	flow_node->flowid.srcport = tuple.srcport;
	flow_node->flowid.dstport = tuple.dstport;
	strcpy( flow_node->flowid.protcol, tuple.protcol );
	return flow_node->count;
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
sent_flow_t * isFlowRegistered( black_list_t * node, tuple_t tuple )
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
			return tmp;
		}
		tmp = tmp->next;
	}
			
	return NULL;
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
		tmp = deleteLastFlowNode( tmp );
	}

	return i;
}

sent_flow_t * deleteLastFlowNode( sent_flow_t * flow_node )
{
	sent_flow_t * tmp;

	tmp = flow_node->next;
	free( flow_node );
	flow_node = NULL;

	return tmp;
}

///////////////////////////////////////////////////////////////////////////////////////////
/* user_nodeとblackuserを入れ替える,　交換するアドレスを２つとも引数として取るべきだった */
///////////////////////////////////////////////////////////////////////////////////////////
void swapBlackNode( black_list_t * user_node )
{
	black_list_t * user_next;
	black_list_t * user_prev;

	if ( user_node == blackuser )
	{
		return;
	}
	else if ( user_node == blackuser_end )
	{
		user_prev = user_node->prev;
		user_prev->next = NULL;
		blackuser_end = user_prev;
	}
	else
	{
		user_next = user_node->next;
		user_prev = user_node->prev;	
		user_next->prev = user_prev;
		user_prev->next = user_next;
	}

	blackuser->prev = user_node;
	user_node->next = blackuser;
	user_node->prev = NULL;
	blackuser = user_node;
}
