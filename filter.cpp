#include "define.h"
using namespace std;

void printBlackList()
{
	user_list_t * tmp = userlist;
	int i = 0;

	while ( tmp != NULL )
	{
		fprintf( stdout, "[NO%03d] -- userip:%s flow_number:%d\n", 
				i, 
				tmp->userip.c_str(), 
				tmp->flow_number );
		printSentFlow( tmp );
		tmp = tmp->next;
		i++;
	}
}

void printBlackListReverse()
{
	user_list_t * tmp = userlist_end;
	int i = 99;

	while ( tmp != NULL )
	{
		fprintf( stdout, "[NO%03d] -- userip:%s flow_number:%d 1packetflow:%d\n", 
				i, 
				tmp->userip.c_str(), 
				tmp->flow_number, 
				tmp->onepacket_number );
		printSentFlow( tmp );
		tmp = tmp->prev;
		i--;
	}
}
void printSentFlow( user_list_t * user_node )
{
	sent_flow_t * tmp_sent_node;
	int i = 1;
	tmp_sent_node = user_node->blacksentflow;
	while ( tmp_sent_node != NULL )
	{
		fprintf( stdout, "	|	[NO%03d] -- %s %s %d %d, %d\n", 
				i, 
				tmp_sent_node->flowid.dstip.c_str(), 
				tmp_sent_node->flowid.protcol.c_str(), 
				tmp_sent_node->flowid.dstport, 
				tmp_sent_node->flowid.srcport, 
				tmp_sent_node->count  );
		tmp_sent_node = tmp_sent_node->next;
		i++;
	}
}

void printRegisteredBlackList()
{
	user_list_t * tmp = userlist;
	int i = 0;

	while( tmp != NULL )
	{
		if ( tmp->flow_number == 0 )
		{
			break;
		}
		else
		{
			fprintf( stdout, "[NO%03d] -- userip:%s flow_number:%d 1packetflow:%d\n",
					i, 
					tmp->userip.c_str(), 
					tmp->flow_number, 
					tmp->onepacket_number );
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

// $B%V%i%C%/%j%9%H#1CJ$a(B, user$B$N$_$rEPO?$9$k(B
int makeUserList()
{
	int i = 0;
	user_list_t * tmp;
	user_list_t * tmp1;

	userlist = ( user_list_t * )malloc( sizeof( user_list_t ) );
	if ( userlist == NULL )
	{
		mallocFailed();
	}
	initializeUserList( userlist );
	tmp = userlist;
	tmp->prev = NULL;

	for ( i = 0 ; i < USER_MAX - 1 ; i = i + 1 )
	{
		tmp->next = ( user_list_t * )malloc( sizeof( user_list_t ) );
		if ( tmp->next == NULL )
		{
			mallocFailed();
		}
		initializeUserList( tmp->next );
		tmp->next->prev = tmp;
		tmp = tmp->next;
	}

	userlist_end = tmp;
	userlist_end->next = NULL;

	return 1;
}

////////////////////////////////////////////////
/* user$B%V%i%C%/%j%9%H$NMWAG$N=i4|2=$r9T$&4X?t(B */
////////////////////////////////////////////////
void initializeUserList( user_list_t * user_node  )
{
	user_node->userip = "0";
	user_node->flow_number = 0;
	// isblackuserが0のときはブラックuserではない
	user_node->isblackuser = 0;
	user_node->onepacket_number = 0;
	user_node->blacksentflow = ( sent_flow_t * )malloc( sizeof( sent_flow_t ) );
	if ( user_node->blacksentflow == NULL )
	{
		mallocFailed();
	}
	initializeFlowList( user_node->blacksentflow );
}

//////////////////////////////////////////////////////////////
/* $B%V%i%C%/%j%9%H$KEPO?$5$l$?(Buser$B$N%U%m!<$N=i4|2=$r9T$&4X?t(B */
//////////////////////////////////////////////////////////////
void initializeFlowList( sent_flow_t * flow_node )
{
	flow_node->count = 0;
	flow_node->next = NULL;
	flow_node->prev = NULL;
	flow_node->flowid.srcip = "0";
	flow_node->flowid.dstip = "0";
	flow_node->flowid.srcport = 0;
	flow_node->flowid.dstport = 0;
	flow_node->flowid.protcol = "0";
}

////////////////////////////////////////////////
/* userが生成したflowを, flow list に追加する関数 */
////////////////////////////////////////////////
sent_flow_t * addFlow( user_list_t * user_node )
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

	tmp = ( sent_flow_t * )malloc( sizeof( sent_flow_t ) );
	if ( tmp == NULL )
	{
		mallocFailed();
	}
	tmp->prev = last;
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
	user_list_t * tmp;
	sent_flow_t * tmp_node;

	tmp = userlist;
	while ( tmp != NULL )
	{
		tmp->blacksentflow = ( sent_flow_t * )malloc( sizeof( sent_flow_t ) );
		if ( tmp->blacksentflow == NULL )
		{
			mallocFailed();
		}
		tmp_node = tmp->blacksentflow;
		initializeFlowList( tmp_node );
		tmp_node->next = NULL;
		tmp = tmp->next;
	}

	return 1;
}

//////////////////////////////
/* ブラックリストの初期化を行う */
/////////////////////////////
void blackListInit()
{
	user_list_t * tmp;
	sent_flow_t * tmp_node;
	sent_flow_t * tmp1;
	int i = 0;

	tmp = userlist;
	while ( tmp != NULL )
	{
		if ( tmp->flow_number > 0 ) 
		{
			deleteFlow( tmp->blacksentflow );
			initializeUserList( tmp );
		}
		i++;
		tmp = tmp->next; 
	}
}

////////////////////////////////////////////////////////////////
/* $B%V%i%C%/%j%9%HFb$N8!:w(B, $BEPO?$d:o=|$NA`:n$r9T$&4X?t$NBgK\(B   */
/* return 0:$B%-%c%C%7%e$XEPO?(B, return 1:$B%-%c%C%7%e$KEPO?$7$J$$(B */
////////////////////////////////////////////////////////////////
int userListOperation( tuple_t tuple )
{
	user_list_t * tmp_black_node;
	sent_flow_t * tmp_sent_flow;

	//一定時間ごとにブラックリストの初期化を行う
	if ( userlist_init_time < tuple.reach_time )
	{
		user_number = 0;
		blackListInit();
		userlist_init_time = userlist_init_time + 0.01;
	}

	if ( ( tmp_black_node = isUserRegistered( tuple ) ) == NULL )
	{ 
		if ( user_number < 100 )
		{
			tmp_black_node = registUser( tuple );
			user_number = user_number + 1;
			substituteFlow( tmp_black_node->blacksentflow, tuple );
		}
		else 
		{	// blackuser_endに登録されているuserを消去し, 新しいuserを登録し直す
			// 最下位のuserが生成したフローのリストを削除
			deleteFlow( userlist_end->blacksentflow );
			// 最下位に登録されているuserのuser情報を初期化 ( 同時に上でfree()した生成フローのリストをmallocする )
			initializeUserList( userlist_end );
			// user情報及び生成フローを登録
			substituteUser( userlist_end, tuple );
		}
	}
	else 
	{
		swapUserNode( tmp_black_node );
		if ( ( tmp_sent_flow = isFlowRegistered( tmp_black_node, tuple ) ) != NULL )
		{	//flowが登録されている場合
			tmp_black_node->onepacket_number--;
			tmp_black_node->flow_number = 0;
			/*fprintf( stdout, "%s %s %s %d %d\n", tmp_sent_flow->flowid.dstip, tmp_sent_flow->flowid.srcip,
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
				for ( int i = 0 ; i < tmp_black_node->onepacket_number ; i++ )
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
				if (  tmp_black_node->flow_number > THRESHOLD )
				{
					tmp_black_node->isblackuser = 1;
				}
				//TODO: 最後のノードと最初のノードを入れ替える必要がある
			}
		}

	}

	return 0;
}


/* 引数で取ったflow_nodeをリストの一番最後に持っていく */
sent_flow_t * moveLastFlowNode( sent_flow_t * flow_node, user_list_t * user_node )
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

////////////////////////////////////////////////////////////////////////////////
/* $BF~NO$N%U%m!<$r@8@.$7$F$$$k(Buser$B$,(B, $B%V%i%C%/%j%9%H$KEPO?$5$l$F$$$k$+C5$94X?t(B */
////////////////////////////////////////////////////////////////////////////////
user_list_t * isUserRegistered( tuple_t tuple )
{
	int i = 0;
	user_list_t * tmp;
	tmp = userlist;

	while ( tmp != NULL )
	{
		if ( tmp->userip == tuple.srcip )
		{
			return tmp;
		}
		tmp = tmp->next;
	}

	return NULL;
}

user_list_t * registUser( tuple_t tuple )
{
	int i = 0, registerd = 0;
	user_list_t * tmp;

	tmp = userlist;

	while ( tmp != NULL )
	{
		if ( tmp->userip == "0" )
		{
			/* ブラックリストにuserを登録する */
			swapUserNode( tmp );
			i = substituteUser( tmp, tuple );
			break;
		}
		tmp = tmp->next;
	}

	return tmp;	
}

int substituteUser( user_list_t * tmp, tuple_t tuple )
{
		tmp->userip = tuple.srcip;
		tmp->flow_number = 1;
		tmp->onepacket_number = 1;
		tmp->isblackuser = 0;
		// おそらくバグの原因はこれか? -- 2回mallocしているせいでバグっていた様子
		//tmp->blacksentflow = malloc( sizeof( sent_flow_t ) );

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
	flow_node->flowid.srcip = tuple.srcip;
	flow_node->flowid.dstip = tuple.dstip;
	flow_node->flowid.srcport = tuple.srcport;
	flow_node->flowid.dstport = tuple.dstport;
	flow_node->flowid.protcol = tuple.protcol;
	return flow_node->count;
}

//////////////////////////////////////////////////////////////////////////
/* $BEPO?$5$l$?%U%m!<$N9=@.%Q%1%C%H?t$,ogCM$rD6$($?$+$I$&$+$rH=Dj$9$k4X?t(B */
//////////////////////////////////////////////////////////////////////////
int isFlowCountOverThreshold( user_list_t user )
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
sent_flow_t * isFlowRegistered( user_list_t * node, tuple_t tuple )
{
	sent_flow_t * tmp;
	tmp = node->blacksentflow;

	while ( tmp != NULL )
	{
		if ( 	tmp->flowid.dstip == tuple.dstip &&
			tmp->flowid.srcip == tuple.srcip && 
			tmp->flowid.protcol == tuple.protcol &&
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
int removeFlow( sent_flow_t * remove_node, user_list_t * user_node )
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

/* user_nodeとblackuserを入れ替える,　交換するアドレスを２つとも引数として取るべきだった */
void swapUserNode( user_list_t * user_node )
{
	user_list_t * user_next;
	user_list_t * user_prev;

	if ( user_node == userlist )
	{
		return;
	}
	else if ( user_node == userlist_end )
	{
		user_prev = user_node->prev;
		user_prev->next = NULL;
		userlist_end = user_prev;
	}
	else
	{
		user_next = user_node->next;
		user_prev = user_node->prev;	
		user_next->prev = user_prev;
		user_prev->next = user_next;
	}

	userlist->prev = user_node;
	user_node->next = userlist;
	user_node->prev = NULL;
	userlist = user_node;
}
