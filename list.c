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

/* キャッシュの内容を出力する, index別に出力した方が良いかもしれない */
void printValue()
{
	node_t *pointer;
	int index = 0;
	int way = 0;

	fprintf( stdout, "=======================================================================================================================\n" );
	for ( index = 0 ; index < INDEX_MAX ; index = index + 1 )
	{
		way = 0;
		fprintf( stdout, "index%d\n", index );
		pointer = p[index];
		while( pointer != head[index] )
		{
			fprintf( stdout, "way%d : %s %s %s %d %d\n", way, pointer->entry.srcip, pointer->entry.dstip, pointer->entry.protcol, pointer->entry.srcport, pointer->entry.dstport );
			pointer = pointer->prev; 
			way = way + 1;
		}
	}
	fprintf( stdout, "=======================================================================================================================\n" );
}


/* inputTupleと, listのnodeのタプル情報が一致するかどうか */
int isEqual( tuple_t inputTuple, node_t * node )
{
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

// TODO:同じ文章が何回も出てくるので, 一部書き直す必要がある
/* listのエントリの操作, キャッシュのポリシーによって内容が変化, 下はLRUポリシー */
void listOperation( tuple_t x, int index, char * operation )
{
	node_t * tmp;
	black_list_t * tmp_black_node;
	if ( isRegistered( x, index ) )
	{	// キャッシュにフローが登録されている場合
		if ( strcmp( operation, "lru" ) == 0 )
		{
			lruPolicy( x, index );
		}
		else if ( strcmp( operation, "sp" ) == 0 )
		{
			spPolicy( x, index );
		}
	}
	else
	{	// キャッシュにフローが登録されていない場合

		blackListOperation( x );	// BlackListにフローの内容を登録・更新する

		// 上の処理で更新されたBlackListの情報を元に, 登録されたuserがblackuserかどうか確認する
		tmp_black_node = isUserRegistered( x ); 
		if ( tmp_black_node->isblackuser == 0 )
		{	// blackuserでない場合にはキャッシュに登録する
			if ( strcmp( operation, "lru" ) == 0 )
			{
				lruPolicy( x, index );
			}
			else if ( strcmp( operation, "sp" ) == 0 )
			{
				spPolicy( x, index );
			}
		}
	}
}

/* listの初期化, headはダミーノードとした */
void listInit()
{
	int index_number = 0;
	int way_number = 0;
	tuple_t init_tuple;

	fprintf( stdout, "init finished\n" );
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
		strcpy( head[index_number]->entry.protcol, "0");
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
	strcpy( tmp.dstip, "0\n" );
	strcpy( tmp.srcip, "0\n" );
	strcpy( tmp.protcol, "0\n" );
	tmp.dstport = 0;
	tmp.srcport = 0;
	tmp.reach_time = 0.0;
}
