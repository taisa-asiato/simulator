#include "define.h"
using namespace std;

void printBlackUser( user_list_t * pointer_to_userlist )
{
	user_list_t * tmp = pointer_to_userlist;
	int i = 0;
	while ( tmp != NULL )
	{
		if ( tmp->isblackuser == 1 )
		{
			fprintf( stdout, "[NO%03d] -- \n", i );
			cout << " userp:" << tmp->userip << " flow numbwe:" << tmp->flow_number << endl;
			printSentFlow( tmp );
		}
		tmp = tmp->next;
		i++;
	}
}

void printUserList( user_list_t * pointer_to_userlist )
{
	user_list_t * tmp = pointer_to_userlist;
	int i = 0;

	while ( tmp != NULL )
	{
		if ( tmp->isblackuser == 1 )
		{
			fprintf( stdout, "[NO%03d] -- ", i );
			cout << "userip:" << tmp->userip << " flow_number:" << tmp->flow_number << " registered_time:" << tmp->registered_time << endl;
		} 
		else 
		{
			fprintf( stdout, "[NO%03d] -- ", i );
			cout << "userip:" << tmp->userip << " flow_number:" << tmp->flow_number << " registered_time:" << tmp->registered_time << endl;
		}
		printSentFlow( tmp );
		tmp = tmp->next;
		i++;
	}
}

void printUserListReverse()
{
	user_list_t * tmp = userlist_end;
	int i = 99;

	while ( tmp != NULL )
	{
		fprintf( stdout, "[NO%03d] -- \n", i );
		cout << " userip:" << tmp->userip << " flow_number:" << tmp->flow_number << endl;
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
		fprintf( stdout, "	|	[NO%03d] -- ", i );
		cout <<  tmp_sent_node->flowid.dstip << " " << tmp_sent_node->flowid.protcol << " " << tmp_sent_node->flowid.srcport << " " << tmp_sent_node->flowid.dstport << " " << tmp_sent_node->count << endl;
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
		if ( tmp->userip == "0" )
		{
			break;
		}
		else
		{
			fprintf( stdout, "[NO%03d] -- ", i );
			cout << " userip:" << tmp->userip << " flow_number:" << tmp->flow_number 
			<< " 1packetflow:" << tmp->onepacket_number << " registered_time:" << tmp->registered_time << endl;
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
int makeUserList()
{
	int i = 0;
	user_list_t * tmp;
	user_list_t * tmp1;

	userlist = new user_list_t;
	if ( userlist == NULL )
	{
		mallocFailed();
	}
	initializeUserList( userlist );
	makeFlowList( userlist );
	tmp = userlist;
	tmp->prev = NULL;

	// 先頭ノードのメモリ確保を既にしているため
	for ( i = 0 ; i < USER_MAX - 1 ; i = i + 1 )
	{
		tmp->next = new user_list_t;
		if ( tmp->next == NULL )
		{
			mallocFailed();
		}
		initializeUserList( tmp->next );
		makeFlowList( tmp->next );
		tmp->next->prev = tmp;
		tmp = tmp->next;
	}

	userlist_end = tmp;
	userlist_end->next = NULL;

	return 1;
}

////////////////////////////////////////////////
/* BlackListを初期化する関数                  */
////////////////////////////////////////////////
void initializeUserList( user_list_t * user_node )
{
	user_node->userip = "0";
	user_node->flow_number = 0;
	// isuserlistが0のときはブラックuserではない
	user_node->isblackuser = 0;
	user_node->onepacket_number = 0;
	user_node->registered_time = 0.0;
}

//////////////////////////////////////////////////////////////
/* Flow listを初期化する関数                                */
//////////////////////////////////////////////////////////////
void initializeFlowList( sent_flow_t * flow_node )
{
	flow_node->count = 0;
	flow_node->flowid.srcip = "0";
	flow_node->flowid.dstip = "0";
	flow_node->flowid.srcport = 0;
	flow_node->flowid.dstport = 0;
	flow_node->flowid.protcol = "0";
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

	tmp = new sent_flow_t;
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
int makeFlowList( user_list_t * user_node )
{
	int i = 0, j = 0;
	sent_flow_t * tmp;
	user_node->blacksentflow = new sent_flow_t;
	tmp = user_node->blacksentflow;
	tmp->prev = NULL;
	initializeFlowList( tmp );

	for ( i = 0 ; i < FLOW_MAX - 1; i++ )
	{	// FLOWLIST_MAXで指定した数だけflowの登録を行う事ができる
		// 既に1つsent_flow_tのノードができているため, FLOW_MAX-1回だけsent_flow_tのノードを作成する
		tmp->next = new sent_flow_t;
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
void userListInit()
{
	user_list_t * tmp_user;
	sent_flow_t * tmp_flow;
	sent_flow_t * tmp1;
	int i = 0;

	tmp_user = userlist;
	while ( tmp_user != NULL )
	{
		tmp_flow = tmp_user->blacksentflow;
		initializeAllFlowList( tmp_flow );
		initializeUserList( tmp_user );
		i++;
		tmp_user = tmp_user->next; 
	}
}

//////////////////////////////////////////
/* userlistの一つのノードの初期化を行う */
//////////////////////////////////////////
void userListNodeInit( user_list_t * user_node )
{
	initializeAllFlowList( user_node->blacksentflow );
	initializeUserList( user_node );
}

/////////////////////////////////////////////////////
/* 引数でとったuser_nodeをリストの最後に持っていく */
/////////////////////////////////////////////////////
void moveLastUserNode( user_list_t * user_node )
{
	user_list_t * tmp = user_node;

	if ( tmp == userlist_end )
	{	// 引数のuser_nodeが最終ノードである場合
		;
	}
	else if ( tmp == userlist )
	{	// 引数のuser_nodeが先頭のノードである場合
		userlist = userlist->next;
		userlist->prev = NULL;
		tmp->prev = userlist_end;
		userlist_end->next = tmp;
		tmp->next = NULL;
		userlist_end = tmp;
	}
	else 
	{	// 中間ノードである場合
		tmp->prev->next = tmp->next;
		tmp->next->prev = tmp->prev;
		userlist_end->next = tmp;
		tmp->next = NULL;
		tmp->prev = userlist_end;
		userlist_end = tmp;
	}
}

////////////////////////////////////////
/* 一定時間毎にUserListの初期化を行う */
/* 空きノードの優先度下げも行う       */
////////////////////////////////////////
void userListIntervalInit()
{
	user_list_t * user_node = userlist;
	user_list_t * tmp;
	int isinit = 0;

	while ( user_node != NULL )
	{	// UserListの全ノードに対して
		if ( ( user_node->userip != "0" ) && ( user_node->isblackuser == 0 ) )
		{
			// 探索中のノードがblackuserでなく, またuserが登録されている場合
			userListNodeInit( user_node );
			isinit = 1;
			// user_nodeを初期化する場合にはtmpにuser_nodeの次のノードのアドレスを入れておく
			tmp = user_node->next;
			moveLastUserNode( user_node );
			user_number--;
		}

		if ( isinit == 1 )
		{	// 初期化した場合にはtmpのアドレスをuser_nodeに代入
			user_node = tmp;
		}
		else if ( isinit == 0 )
		{
			user_node = user_node->next;
		}

		isinit = 0;
	}
	userlist_init_time = userlist_init_time + USERLIST_INIT_INTERVAL;
}

/////////////////////////////////////////////
/* blackuser関係なくUserListの初期化を行う */
/////////////////////////////////////////////
void userListIntervalInitAll()
{
	user_list_t * tmp = userlist;
	int isinit = 0, i = 0;

	user_number = 0;
	while ( tmp != NULL )
	{	// UserListの全ノードに対して
		userListNodeInit( tmp );
		tmp = tmp->next;
		i++;
	}
	// cout << "Update userlist_init_time" << endl;
	userlist_init_time = userlist_init_time + USERLIST_INIT_INTERVAL;
}

///////////////////////////////////////////
/* 一定時間ごとにblackuserの初期化を行う */
///////////////////////////////////////////
void blackuserIntervalInit( double now_time )
{
	user_list_t * user_node = userlist;
	double diff_time;
	while ( user_node != NULL )
	{
		if ( ( user_node->isblackuser == 1 ) && ( now_time - user_node->registered_time > BLACKUSER_INIT_INTERVAL ) )
		{	// blackuserであり, 登録時刻から一定時間経過している場合
			// blackuserを初期化する
			// fprintf( stdout, "blackuser Init\n" );
			user_node->isblackuser = 0;
			user_node->flow_number = 0;
			user_node->onepacket_number = 0;
			user_node->registered_time = 0.0;
			initializeAllFlowList( user_node->blacksentflow );
		}
		user_node = user_node->next;
	}
}

//////////////////////////////////////////////////////////////
/* UserListがどれだけ精度高くblackuserを識別できたか計算する*/
//////////////////////////////////////////////////////////////
void identifyRateCounter()
{
	if ( skip_1p == 0 ) 
	{
		identify_rate.push_back( 0 );
	}
	else 
	{
		identify_rate.push_back( 1.0 * hit_1p / skip_1p );
	}
	hit_1p = 0;
	skip_1p = 0;
}

////////////////////////////////////////////////////////////////
/* BlackListの様々な処理を行う関数                            */
////////////////////////////////////////////////////////////////
//TODO: if 文ないのブロックが大きいので分割すべき(　個別の関数に分割すべき )
int userListOperation( tuple_t tuple )
{
	user_list_t * tmp_black_node;
	user_list_t * user_node, tmp_node;
	sent_flow_t * tmp_sent_flow;
	tuple_t tmp_tuple;
	int i = 0, count = 0, tmp_count = 1;

	//一定時間ごとにブラックリストの初期化を行う
	if ( userlist_init_time < tuple.reach_time )
	{	// countにはblackuser数が入る
		// UserListの初期化及び初期化ノードの優先度の変更を行う
		// fprintf( stdout, "UserList Init\n" );
		userListIntervalInit();
		//userListIntervalInitAll();
	}
	
	// blackuserの初期化を行う
//	blackuserIntervalInit( tuple.reach_time );


//	cout << "find out user spec" << endl;
	if ( ( tmp_black_node = isUserRegistered( tuple ) ) == NULL )
	{ 	// userip がUserListに登録されていない場合
		//fprintf( stdout, "user was not registered at UserList\n" );
		if ( user_number < USER_MAX )
		{	// UserListに登録されているuseripの数が99以下の場合
			//fprintf( stdout, "user number wis under USER_MAX\n" );
			tmp_black_node = registUser( tuple );
			//fprintf( stdout, "regist user at UserList\n" );
			user_number = user_number + 1;
			//fprintf( stdout, "%p\n", tmp_black_node );
			substituteFlow( tmp_black_node->blacksentflow, tuple );
			//fprintf( stdout, "substitute flow into user's flow list\n" );
		}
		else 
		{	// UserListがいっぱいの場合
			// userlist_endに登録されているuserを消去し, 新しいuserを登録し直す
			// 最下位のuserが生成したフローのリストを初期化
			tmp_black_node = userlist_end;
			initializeAllFlowList( tmp_black_node->blacksentflow );
			// 最下位に登録されているuserのuser情報を初期化 
			initializeUserList( tmp_black_node );
			// user情報及び生成フローを登録
			substituteUser( tmp_black_node, tuple );
			newUserForMaxList();
		}
	}
	else 
	{	// userip がUserListに登録されている場合
		// useripが登録されているエントリを優先度が最も高いところにおく
		//fprintf( stdout, "user was registered at UserList\n" );
		swapUserNode( tmp_black_node );
		if ( ( tmp_sent_flow = isFlowRegistered( tmp_black_node, tuple ) ) )
		{	//flowが登録されている場合
			tmp_sent_flow->count = tmp_sent_flow->count + 1;
			if ( tmp_sent_flow->count == 2 )
			{
				tmp_black_node->onepacket_number--;
			}
			// 連続してmissした場合のカウンタの値を0にする
			// tmp_black_node->flow_number = 0;
			//	fprintf( stdout, "%s %s %s %d %d\n", tmp_sent_flow->flowid.dstip, tmp_sent_flow->flowid.srcip,
			//	  tmp_sent_flow->flowid.protcol, tmp_sent_flow->flowid.dstport, tmp_sent_flow->flowid.srcport);
			// initializeFlowList( tmp_sent_flow );
			// moveLastFlowNode( tmp_sent_flow, tmp_black_node );
			/*fprintf( stdout, "%s %s %s %d %d\n", tmp_sent_flow->flowid.dstip, tmp_sent_flow->flowid.srcip,
			  tmp_sent_flow->flowid.protcol, tmp_sent_flow->flowid.dstport, tmp_sent_flow->flowid.srcport);*/
		}
		else
		{	// flowが登録されていない場合
			tmp_count = isSimilarFlow( tmp_black_node, tuple );
			if ( tmp_black_node->flow_number < FLOW_MAX )
			{	//flowが登録されておらず, 更にフローリストに空きがある場合
				tmp_sent_flow = tmp_black_node->blacksentflow;
				for ( i = 0 ; i < tmp_black_node->flow_number ; i++ )
					tmp_sent_flow = tmp_sent_flow->next;
				// フローリストの登録していない場所まで移動する
				substituteFlow( tmp_sent_flow, tuple );
				//tmp_black_node->onepacket_number++;
				tmp_black_node->onepacket_number += tmp_count; // use when isSimilarFlow function used
				tmp_black_node->flow_number++;
			}
			else
			{	// flowlistに空きが無い場合
				// flowリストの先頭ノードをリストの最後に移動
				tmp_sent_flow = moveLastFlowNode( tmp_black_node->blacksentflow, tmp_black_node );
				// リストの最後のノードに5タプルの値を代入
				substituteFlow( tmp_sent_flow, tuple );
				tmp_black_node->flow_number++;
				//tmp_black_node->onepacket_number++;
				tmp_black_node->onepacket_number += tmp_count; //use when isSimilarFlow fucntion used
			}
		}

	}


	//  flowの数がしきい値を超えた場合にはuserlistとする
	if ( ( tmp_black_node->onepacket_number >= THRESHOLD ) && ( tmp_black_node->isblackuser == 0 ) )
	{	
		//		fprintf( stdout, "user is registered as blackuser\n" );
		tmp_black_node->isblackuser = 1;
		tmp_black_node->registered_time = tuple.reach_time;
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

///////////////////////////////////////////////////////////////////////////////
/* blacklistに最大までuserが登録されている場合に新しく登録するuserが来た場合 */
///////////////////////////////////////////////////////////////////////////////
void newUserForMaxList()
{
	user_list_t * tmp, tmp1;
	tmp = userlist_end->prev;
	tmp->next = NULL;
	userlist_end->prev = NULL;
	userlist_end->next = userlist;
	userlist->prev = userlist_end;
	userlist = userlist_end;
	userlist_end = tmp;
}

////////////////////////////////////////////////////////////////////////////////
/* $BF~NO$N%U%m!<$r@8@.$7$F$$$k(Buser$B$,(B, $B%V%i%C%/%j%9%H$KEPO?$5$l$F$$$k$+C5$94X?t(B */
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

//////////////////////////////
/* UserListにuserを登録する */
//////////////////////////////
user_list_t * registUser( tuple_t tuple )
{
	int i = 0, registerd = 0;
	user_list_t * tmp;

	tmp = userlist;

	while ( tmp != NULL )
	{
		//fprintf( stdout, "NO%03duserip:%s\n", i, tmp->userip );
		if ( tmp->userip == "0" )
		{
			/* ブラックリストにuserを登録する */
			swapUserNode( tmp );
			i = substituteUser( tmp, tuple );
			break;
		}
		tmp = tmp->next;
		i = i + 1;
	}

	return tmp;	
}

int substituteUser( user_list_t * tmp, tuple_t tuple )
{
	tmp->userip = tuple.srcip;
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
	flow_node->flowid.srcip = tuple.srcip;
	flow_node->flowid.dstip = tuple.dstip;
	flow_node->flowid.srcport = tuple.srcport;
	flow_node->flowid.dstport = tuple.dstport;
	flow_node->flowid.protcol = tuple.protcol;
	return flow_node->count;
}

//////////////////////////////////////////////////////////////////////////
/* $BEPO?$5$l$?%U%m!<$N9=@.%Q%1%C%H?t$,ogCM$rD6$($?$+$I$&$+$rH=Dj$9$k4X?t(B */
//////////////////////////////////////////////////////////////////////////
int isFlowCountOverThreshold( user_list_t user )
{
	// $B%U%m!<$,EPO?$5$l$F$$$k>l9g(B
	if ( user.flow_number >= THRESHOLD )
	{
		// $B%U%m!<$,(BTHRESHOLD$B8D0J>e$N%Q%1%C%H$G9=@.$5$l$F$$$k(B
		initializeFlowList( user.blacksentflow );
		return 0;
	}
	else if ( user.flow_number < THRESHOLD )
	{
		// $B%U%m!<$,(BTHRESHOLD$B8DL$K~$N%Q%1%C%H$G$"$k(B
		user.flow_number++;
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////
/* $B%V%i%C%/%j%9%H$KEPO?$5$l$?(Buser$B$,@8@.$7$F$$$k%U%m!<$H$NHf3S$r9T$&(B */
//////////////////////////////////////////////////////////////////////
sent_flow_t * isFlowRegistered( user_list_t * node, tuple_t tuple )
{
	sent_flow_t * tmp;
	tmp = node->blacksentflow;

	while ( tmp != NULL )
	{
		if ( 	
			tmp->flowid.dstip == tuple.dstip &&
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
/* $B%V%i%C%/%j%9%H$KEPO?$5$l$?%U%m!<$r:o=|$9$k4X?t(B */
////////////////////////////////////////////////////
int removeFlow( sent_flow_t * remove_node, user_list_t * user_node )
{
	// $BEPO?$5$l$F$$$k%U%m!<?t$r8:>/$5$;$k(B
	user_node->flow_number = user_node->flow_number - 1;
	if ( user_node->flow_number == 0 )
	{
		// $B$3$N$H$-(B, user$B$,@8@.$7$?%U%m!<$,:o=|$5$l$k(B
		// user$B$r%V%i%C%/%j%9%H$+$i:o=|$9$kI,MW$,$"$k(B
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

//////////////////////////////////////////////////////
/* 引数で与えられたuser_nodeとuser_listを入れ替える */
/* TODO : 関数名を変更する必要がある                */
//////////////////////////////////////////////////////
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
