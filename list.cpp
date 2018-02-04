#include "define.h"
using namespace std;

/* pointer が指すtuple_t構造体に xの各フィールドの値を代入する */
void listSubstitute( node_t * pointer, tuple_t x )
{
	pointer->entry.srcip = x.srcip;
	pointer->entry.dstip = x.dstip;
	pointer->entry.protcol = x.protcol;
	pointer->entry.srcport = x.srcport;
	pointer->entry.dstport = x.dstport;
	pointer->entry.reach_time = x.reach_time;
}

/* indexキャッシュの内容を出力する */
void printValueIndex( int index )
{
	node_t *pointer;
	int way = 0;

	fprintf( stdout, "=======================================================================================================================\n" );
	way = 0;
	fprintf( stdout, "index%d\n", index );
	pointer = p[index];
	while( pointer != head[index] )
	{
		fprintf( stdout, "way%03d : ", way );
		cout << pointer->entry.srcip << " " << pointer->entry.dstip << 
			" " << pointer->entry.protcol << 
			" " << pointer->entry.srcport << " " << pointer->entry.dstport << endl;
		pointer = pointer->prev; 
		way = way + 1;
	}
	fprintf( stdout, "=======================================================================================================================\n" );
}

/* キャッシュの内容を出力する, index別に出力した方が良いかもしれない */
void printValue()
{
	node_t *pointer;
	int index = 0;
	int way = 0;

	for ( index = 0 ; index < INDEX_MAX ; index = index + 1 )
	{
		printValueIndex( index );
	}
}


/* inputTupleと, listのnodeのタプル情報が一致するかどうか */
int isEqual( tuple_t inputTuple, node_t * node )
{
	//	fprintf( stdout, "%s %s %s %d %d\n", node->entry.srcip, node->entry.dstip, node->entry.protcol, node->entry.srcport, node->entry.dstport );

	if (
			inputTuple.srcip == node->entry.srcip &&
			inputTuple.dstip == node->entry.dstip &&
			inputTuple.protcol == node->entry.protcol &&
			inputTuple.srcport == node->entry.srcport  &&
			inputTuple.dstport == node->entry.dstport 
	   ) 

	{
		return EQUAL;
	}
	else 
	{
		return NOTEQUAL;
	}
}

/* inputTupleが, リストのどれかに登録されているかどうか */
node_t * isRegistered( tuple_t inputTuple, int index )
{
	node_t *tmp;
	tmp = p[index];

	while( tmp != head[index] )
	{
		if ( isEqual( inputTuple, tmp ) == 1 )
		{
			hitOrMiss( inputTuple, 1 );
			//cout << "HIT  " << inputTuple.srcip << " " << inputTuple.srcport << " " << inputTuple.dstip << " " << inputTuple.dstport << " " << inputTuple.protcol << endl;
			//cout << " >>>>>>>>>> HIT " << endl;
			return tmp;
		}
		else
		{
			tmp = tmp->prev;
		}
	}
	hitOrMiss( inputTuple, 0 );
	//cout << "MISS "  << inputTuple.srcip << " " << inputTuple.srcport << " " << inputTuple.dstip << " " << inputTuple.dstport << " " << inputTuple.protcol << endl;
	return NULL;
}

/////////////////////////////////////
/* 単位時間あたりのhit率を測定する */
/////////////////////////////////////
void hitRatePerSec( double reach_time )
{
	if ( arrival_time < reach_time )
	{
		// 1秒辺りのヒット率を求める処理, 別に関数を作成した方が良いかも
		hitrate_per_sec.push_back( (double)hit_per_sec / ( (double)hit_per_sec + (double)miss_per_sec ) );
		
		// 1秒あたりのtcamアクセス率
		tcamrate_per_sec.push_back( ( double )( miss_per_sec - tnaccess_per_sec ) / 
						( double )( hit_per_sec + miss_per_sec ));
		tcam_per_sec.push_back( ( int )( miss_per_sec - tnaccess_per_sec ) );
		ntcamrate_per_sec.push_back( ( double )( miss_per_sec - NIDS_RATE * tnaccess_per_sec ) / 
						( double )( hit_per_sec + miss_per_sec ));
		ntcam_per_sec.push_back( ( int )( miss_per_sec - NIDS_RATE * tnaccess_per_sec ) );
		nids_senddatasize.push_back( 1.0 * skipdatasize );

		flownum_per_sec.push_back( flownum.size() );
		/*fprintf( stdout, "%03d,  %7d,  %7d, %f, %7d, %f\n", 
				(int)arrival_time - 1,
				hit_per_sec,
				miss_per_sec,
				(double)hit_per_sec / ( (double)hit_per_sec + (double)miss_per_sec ),
				tnaccess_per_sec,
				( double )( miss_per_sec - tnaccess_per_sec ) / ( double )( hit_per_sec + miss_per_sec )
		);*/
		skipdatasize = 0;
		tnaccess_per_sec = 0;
		hit_per_sec = 0;
		miss_per_sec = 0;
		flownum.clear();
		arrival_time = arrival_time + 1;
	}
}

void hitOrMiss( tuple_t tuple, int isHit )
{

	HITORMISS = isHit;
/*	if ( arrival_time < tuple.reach_time )
	{
		// 1秒辺りのヒット率を求める処理, 別に関数を作成した方が良いかも
		hitrate_per_sec[(int)arrival_time - 1] = (double)hit_per_sec / 
			( (double)hit_per_sec + (double)miss_per_sec );
		
		// 1秒あたりのtcamアクセス率
		tcamrate_per_sec[(int)arrival_time-1] = ( double )( miss_per_sec - tnaccess_per_sec ) / 
							( double )( hit_per_sec + miss_per_sec );
		fprintf( stdout, "%03d,  %7d,  %7d, %f, %7d, %f\n", 
				(int)arrival_time - 1,
				hit_per_sec,
				miss_per_sec,
				hitrate_per_sec[(int)arrival_time-1],
				tnaccess_per_sec,
				tcamrate_per_sec[(int)arrival_time-1]
		       );
		tnaccess_per_sec = 0;
		hit_per_sec = 0;
		miss_per_sec = 0;
		arrival_time = arrival_time + 1;
	}
*/
	if ( isHit == 1 )
	{
		hit_per_sec = hit_per_sec + 1;
		hitflag = hitflag + 1;
		//fprintf( stdout, "Hit\n" );
	}
	else if ( isHit == 0 )
	{
		miss_per_sec = miss_per_sec + 1;
		miss = miss + 1;
		//fprintf( stdout, "Miss\n" );
	}
}

//////////////////////////////////////////////////////
/* キャッシュのエントリの操作を行う関数のメイン関数 */
//////////////////////////////////////////////////////
void listOperation( tuple_t x, int index, char * operation, char * blacklist, char * debug )
{
	if ( strcmp( blacklist, "ON" ) == 0 )
	{	// ONの時BlackListを使用する
		listOperationWithList( x, index, operation, debug );
	}
	else 
	{	// ONでないとき, BlackListを使用しない
		listOperationNoList( x, index, operation, debug );
	}
}

/////////////////////////////
/* BlackList有りで動作する */
/////////////////////////////
void listOperationWithList( tuple_t x, int index, char * operation, char * debug )
{	// TODO:umpを用いたuserlist方式がバグるので改善する
	node_t * tmp;
	user_list_t * tmp_user_node;
	int this_time;
	// std::list< ump_user_t >::iterator itr_node;

	string search_flow = x.srcip + " " + x.dstip + " " + 
		x.protcol + " " + to_string( x.srcport ) + " " + to_string ( x.dstport );

	//	fprintf( stdout, "===Before===\n" );
	//	printValueIndex( index );
	if ( ( tmp = isRegistered( x, index ) ) )
	{	// キャッシュにフローが登録されている場合
		switchPolisy( x, index, operation, tmp );	
	}
	else
	{	// キャッシュにフローが登録されていない場合( キャッシュミスした時 )
		tmp_user_node = isUserRegistered( x ); 
		// itr_node = ump_isUserRegistered( x );
		// auto itr_node = ump_userlist.find( x.srcip );
		if ( tmp_user_node != NULL /* itr_node != ump_userlist.end() */ )
		{	// UserListにuserが登録されている場合
			//tmp_user_node = ump_userlist[x.srcip];
			if ( tmp_user_node->isblackuser == 0 /* itr_node->second->isblackuser == 0 */ )
			{	// userがblackuserでない場合
				switchPolisy( x, index, operation, tmp );
			}
			else if ( tmp_user_node->isblackuser == 1 /* itr_node->second->isblackuser == 1 */ )
			{	// userがblackuserである場合
				//if ( strcmp( debug, "DEBUG" ) == 0 )
				//{
				//	fprintf( stdout, "%s %s %s %d %d\n", x.srcip, x.dstip, x.protcol, x.srcport, x.dstport );
					//fprintf( stdout, "\x1b[41m blackuser, skip regist to cache \x1b[49m\n" );
				//}
				skipflow++;
				skip_1p++;
				// miss_per_sec--;
				// miss--;
				this_time = x.reach_time;
				int s = user_interval[(int)this_time][x.srcip].size();
				if ( ump_tuple[search_flow] == 1 && s >= average_flownum )
				{
					tnaccess_per_sec++;
					skipdatasize = skipdatasize + x.datasize;
					hit_1p++;
					onepflow++;
				}

			//	if ( ump_tuple[search_flow] > 10 )
			//	{
			//		cout << "long flow generate blackuser " << tmp_user_node->userip << endl;
			//		cout << search_flow << endl;
			//	}
			}
			else if ( tmp_user_node->isblackuser == 2 /* itr_node->second->isblackuser == 2 */ )
			{
				// switchPolisy( x, index, operation, tmp );
				;
			}
			// もしもuserがisblackuserならば, キャッシュに対する処理を行わない
		}
		else 
		{	// BlackListにuserが登録されていない場合
			switchPolisy( x, index, operation, tmp );
			//			fprintf( stdout, "user was not registered as blackuser\n");
		}

		// キャッシュを見た後にBlackListの更新を行う
		userListOperation( x );
		//ump_UserListOperation( x );
	}

	//	fprintf( stdout, "===After===\n" );
	//	printValueIndex( index );
}

/////////////////////////////
/* BlackListなしで動作する */
/////////////////////////////
void listOperationNoList( tuple_t x, int index, char * operation, char * debug )
{
	node_t * tmp = isRegistered( x, index );
	//	if ( !tmp && debug )
	//	{
	//		fprintf( stdout, "%s %s %s %d %d\n", x.srcip, x.dstip, x.protcol, x.srcport, x.dstport );
	//	}
	switchPolisy( x, index, operation, tmp );
}

//////////////////////////////////////////////////
/* キャッシュメモリの置換アルゴリズムの切り替え */
//////////////////////////////////////////////////
void switchPolisy( tuple_t x, int index, char * operation, node_t * tmp )
{
	if ( strcmp( operation, "lru" ) == 0 )
	{
		lruPolicy( x, index, tmp );
	}
	else if ( strcmp( operation, "sp" ) == 0 )
	{
		spPolicy( x, index, tmp );
	}
	else if ( strcmp( operation, "opt" ) == 0 )
	{
		OPT( x, index, tmp );
	}
	else if ( strcmp( operation, "nocache") == 0 )
	{	// nocacheの場合, キャッシュが無い場合を想定するので
		// オペレーションは行わない
		;
	}
}

////////////////////////////////////////////
/* listの初期化, headはダミーノードとした */
////////////////////////////////////////////
void listInit()
{
	int index_number = 0;
	int way_number = 0;
	tuple_t init_tuple;

	for ( index_number = 0 ; index_number < INDEX_MAX ; index_number = index_number + 1 )
	{

		head[index_number] = new node_t;
		head[index_number]->next = NULL;
		head[index_number]->prev = NULL;

		/* headポインタ,  */
		head[index_number]->entry.srcport = 0;
		head[index_number]->entry.dstport = 0;
		head[index_number]->entry.srcip = "0";
		head[index_number]->entry.dstip = "0";
		head[index_number]->entry.protcol = "0";
		//値は代入しておくべき？
		//初めは最後のノードを指すポインタも先頭ノードを指しておく
		p[index_number] = head[index_number];

		init_tuple.srcip = "0";
		init_tuple.dstip = "0";
		init_tuple.protcol = "0";
		init_tuple.srcport = 0;
		init_tuple.dstport = 0;

		for ( way_number = 0 ; way_number < WAY_MAX ; way_number = way_number + 1 )
		{
			listInsert( init_tuple, index_number );
		}
	}
}


/* listに新しく要素を作成する時に使う, listMake, listAddとかの方が良かったかも */
void listInsert( tuple_t x, int number )
{
	//	fprintf( stdout, "insert started\n" );
	node_t * newnode;

	newnode = new node_t;
	listSubstitute( newnode, x );

	p[number]->next = newnode;
	newnode->next = NULL;
	newnode->prev = p[number];
	p[number] = newnode;
}


/* listの最初の要素を削除する, 優先度が一番低いエントリをキャッシュから追い出す操作に等しい */
void listDeleteFirst( int number )
{
	node_t * pointer;
	pointer = head[number]->next;
	head[number]->next = pointer->next;
	head[number]->next->prev = head[number];
	free( pointer );
}

////////////////////////////////////////
/* tuple_t の各メンバ変数を初期化する */
////////////////////////////////////////
tuple_t initializeTuple()
{
	tuple_t tmp;
	tmp.dstip = "0";
	tmp.srcip = "0";
	tmp.protcol = "0";
	tmp.dstport = 0;
	tmp.srcport = 0;
	tmp.reach_time = 0.0;
	return tmp;
}

////////////////////////////////////////////////////
/* listの最低優先度ノードを, UserListを元に決める */
////////////////////////////////////////////////////
void listExchange( int index )
{
	std::array< node_t * , 4 > reg_entry;
	node_t * tmp_entry;
	node_t * target_node;
	user_list_t * tmp = userlist;
	sent_flow_t * target_flow;
	sent_flow_t * tmp_sent;
	int flag = 0, max = 0;

	reg_entry = regEntry( index );
	while ( tmp != NULL )
	{
		if ( tmp->userip != "0" )
		{
			tmp_entry = isUserHasEntry( tmp, reg_entry );
			if ( tmp_entry != NULL )
			{	// Cacheに登録されているフローを生成したuserのエントリがuserlistにある場合
				if ( tmp->isblackuser == 1 )
				{	// userがblackuserである場合
					// エントリを空け, エントリ順番を変更
					// initBlackUserEntry( tmp_entry, index );
					// flag = 1;
				}
				else 
				{	// userがblackuserでない場合
					tmp_sent = isFlowRegistered( tmp, tmp_entry->entry );
					if ( tmp_sent != NULL )
					{	// cacheに登録されているエントリがuserlistにも存在する
						if ( tmp_sent->count > max )
						{
							max = tmp_sent->count;
							target_node = tmp_entry;
						}
					}
				}
			}
		}
		tmp = tmp->next;
	}

	if ( flag == 1 )
		return;

	if ( max > 0 )
	{
		moveToMPState( target_node, index );		
	}
}

///////////////////////////////////////////////
/* Cacheに登録されているentryを配列に保存する */
///////////////////////////////////////////////
std::array< node_t * , 4 > regEntry( int index )
{
	std::array< node_t * , 4 > reg_entry;
	int i = 0;
	node_t * tmp = head[index]->next;
	while ( tmp != NULL )
	{
		reg_entry[i] = tmp;
		i++;
		tmp = tmp->next;
	}
	
	return reg_entry;
}

//////////////////////////////////
/* userがエントリを持っているか */
//////////////////////////////////
node_t * isUserHasEntry( user_list_t * tmp, std::array< node_t *, 4 > reg_entry )
{
	for ( int i = 0 ; i < 4 ; i++ )
	{
		if ( 	tmp->userip == reg_entry[i]->entry.srcip )
		{
			return reg_entry[i];
		}

	}
	return NULL;
}

/////////////////////////////////////////////////////////
/* 引数で得たノードをcacheの最優先エントリに持っていく */
/////////////////////////////////////////////////////////
void moveToMPState( node_t * tmp, int index )
{
	node_t * p_node;

	if ( tmp == p[index] )
	{	/* 優先度が一番高いエントリの場合には何もしない */
		;
	}
	else if ( tmp != p[index] )
	{
		p_node = tmp;
		tmp->prev->next = tmp->next;
		tmp->next->prev = tmp->prev;
		tmp->prev = p[index];
		p[index]->next = tmp;
		tmp->next = NULL;
		p[index] = tmp;
	}
}

////////////////////////////////////////////////////////////////////////////////////
/* blackuserの生成したフローの, cacheのエントリを削除し, 最低優先度位置に移動する */
////////////////////////////////////////////////////////////////////////////////////
void initBlackUserEntry( node_t * tmp_entry, int index )
{
	tuple_t tmp = initializeTuple();
	listSubstitute( tmp_entry, tmp );

	if ( tmp_entry == head[index]->next )
	{
		;
	}
	else
	{
		if ( tmp_entry == p[index] )
		{
			p[index] = tmp_entry->prev;
			p[index]->next = NULL;
		}
		else 
		{
			tmp_entry->next->prev = tmp_entry->prev;
			tmp_entry->prev->next = tmp_entry->next;
		}

		tmp_entry->next = head[index]->next;
		head[index]->next->prev = tmp_entry;
		head[index]->next = tmp_entry;
		tmp_entry->prev = head[index];
	}
}
