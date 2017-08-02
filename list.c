#include "define.h"

/* pointer が指すtuple_t構造体に xの各フィールドの値を代入する */
void listSubstitute( node_t * pointer, tuple_t x )
{
	strcpy( pointer->entry.srcip, x.srcip );
	strcpy( pointer->entry.dstip, x.dstip );
	strcpy( pointer->entry.protcol, x.protcol );
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
		fprintf( stdout, "way%03d : %s %s %s %d %d\n", way, pointer->entry.srcip, pointer->entry.dstip, pointer->entry.protcol, pointer->entry.srcport, pointer->entry.dstport );
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
			( strcmp( inputTuple.srcip, node->entry.srcip ) == 0 ) &&
			( strcmp( inputTuple.dstip, node->entry.dstip ) == 0 ) &&
			( strcmp( inputTuple.protcol, node->entry.protcol) == 0 ) &&
			( inputTuple.srcport == node->entry.srcport ) &&
			( inputTuple.dstport == node->entry.dstport ) 
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
			return tmp;
		}
		else
		{
			tmp = tmp->prev;
		}
	}
	hitOrMiss( inputTuple, 0 );
	return NULL;
}

void hitOrMiss( tuple_t tuple, int isHit )
{
	if ( time < tuple.reach_time )
	{
		// 1秒辺りのヒット率を求める処理, 別に関数を作成した方が良いかも
		hitrate_per_sec[(int)time - 1] = (double)hit_per_sec / ( (double)hit_per_sec + (double)miss_per_sec );
		hit_per_sec = 0;
		miss_per_sec = 0;
		time = time + 1;

	}

	if ( isHit == 1 )
	{
		hit_per_sec = hit_per_sec + 1;
		hitflag = hitflag + 1;
	}
	else if ( isHit == 0 )
	{
		miss_per_sec = miss_per_sec + 1;
		miss = miss + 1;
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
////////////////////////////
void listOperationWithList( tuple_t x, int index, char * operation, char * debug )
{
	node_t * tmp;
	user_list_t * tmp_user_node;

//	fprintf( stdout, "===Before===\n" );
//	printValueIndex( index );
	if ( ( tmp = isRegistered( x, index ) ) )
	{	// キャッシュにフローが登録されている場合
		switchPolisy( x, index, operation, tmp );	
	}
	else
	{	// キャッシュにフローが登録されていない場合( キャッシュミスした時 )
		tmp_user_node = isUserRegistered( x ); 
		if ( tmp_user_node != NULL )
		{	// BlackListにuserが登録されている場合
			if ( tmp_user_node->isblackuser == 0 )
			{	// userがblackuserでない場合
				switchPolisy( x, index, operation, tmp );
			}
			else
			{	// userがblackuserである場合
				if ( debug )
				{
					fprintf( stdout, "%s %s %s %d %d\n", x.srcip, x.dstip, x.protcol, x.srcport, x.dstport );
					//fprintf( stdout, "\x1b[41m blackuser, skip regist to cache \x1b[49m\n" );
				}
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
	if ( !tmp && debug )
	{
		fprintf( stdout, "%s %s %s %d %d\n", x.srcip, x.dstip, x.protcol, x.srcport, x.dstport );
	}

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

		head[index_number] = malloc( sizeof( node_t ) );
		head[index_number]->next = NULL;
		head[index_number]->prev = NULL;

		/* headポインタ,  */
		strcpy( head[index_number]->entry.srcip, "0" );
		strcpy( head[index_number]->entry.dstip, "0" );
		head[index_number]->entry.srcport = 0;
		head[index_number]->entry.dstport = 0;
		strcpy( head[index_number]->entry.protcol, "0" );
		//値は代入しておくべき？
		//初めは最後のノードを指すポインタも先頭ノードを指しておく
		p[index_number] = head[index_number];

		strcpy( init_tuple.srcip, "0" );
		strcpy( init_tuple.dstip, "0" );
		strcpy( init_tuple.protcol, "0" );
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
	node_t *newnode;

	newnode = malloc( sizeof( node_t ) );
	p[number]->next = newnode;

	listSubstitute( newnode, x );

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
	strcpy( tmp.dstip, "0" );
	strcpy( tmp.srcip, "0" );
	strcpy( tmp.protcol, "0" );
	tmp.dstport = 0;
	tmp.srcport = 0;
	tmp.reach_time = 0.0;
}
