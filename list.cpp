#include "define.h"

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

void listNodeInit( node_t * pointer )
{
	pointer->entry.srcip = "0";
	pointer->entry.dstip = "0";
	pointer->entry.srcport = 0;
	pointer->entry.dstport = 0;
	pointer->entry.protcol = "0";
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
		cout << "index" << index << endl;
		pointer = p[index];
		while( pointer != head[index] )
		{
			cout << "way:" << way << " " << pointer->entry.srcip << " "
				<< pointer->entry.dstip << " " << pointer->entry.protcol << " "
				<< pointer->entry.srcport << " " << pointer->entry.dstport << endl;
			pointer = pointer->prev; 
			way = way + 1;
		}
	}
	fprintf( stdout, "=======================================================================================================================\n" );
}


/* inputTappleと, listのnodeのタプル情報が一致するかどうか */
int isEqual( tuple_t inputTapple, node_t * node )
{
	if (
			inputTapple.srcip == node->entry.srcip && inputTapple.dstip == node->entry.dstip &&
			inputTapple.protcol == node->entry.protcol && inputTapple.srcport == node->entry.srcport &&
			inputTapple.dstport == node->entry.dstport 
	  )
	{
		return EQUAL;
	}
	else 
	{
		return NOTEQUAL;
	}
}

/* inputTappleが, リストのどれかに登録されているかどうか */
node_t * isRegistered( tuple_t inputTapple, int index )
{
	node_t *tmp;
	tmp = p[index];

	while( tmp != head[index] )
	{
		if ( isEqual( inputTapple, tmp ) == 1 )
		{
			hitOrMiss( inputTapple, 1 );
			return tmp;
		}
		else
		{
			tmp = tmp->prev;
		}
	}

	hitOrMiss( inputTapple, 0 );
	return NULL;
}

void hitOrMiss( tuple_t tuple, int isHit )
{
	if ( arrival_time < tuple.reach_time )
	{
		// 1秒辺りのヒット率を求める処理, 別に関数を作成した方が良いかも
		hitrate_per_sec[(int)arrival_time - 1] = (double)hit_per_sec / ( (double)hit_per_sec + (double)miss_per_sec );
		hit_per_sec = 0;
		miss_per_sec = 0;
		arrival_time = arrival_time + 1;

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

/* listのエントリの操作, キャッシュのポリシーによって内容が変化, 下はLRUポリシー */
void listOperation( tuple_t x, int index, char * operation, char * blacklist )
{
	node_t * tmp;
	if ( strcmp( operation, "lru" ) == 0 )
	{
		lruPolicy( x, index );
	}
	else if ( strcmp( operation, "sp" ) == 0 )
	{
		spPolicy( x, index );
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
		head[index_number] = ( node_t *)malloc( sizeof( node_t ) );
		head[index_number]->next = NULL;
		head[index_number]->prev = NULL;

		/* headポインタ,  */
		listNodeInit( head[index_number] );
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

	newnode = ( node_t * )malloc( sizeof( node_t ) );
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
