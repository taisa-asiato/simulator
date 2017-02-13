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


/* inputTappleと, listのnodeのタプル情報が一致するかどうか */
int isEqual( tuple_t inputTapple, node_t * node )
{
	if (
			( strcmp( inputTapple.srcip, node->entry.srcip ) == 0 ) &&
			( strcmp( inputTapple.dstip, node->entry.dstip ) == 0 ) &&
			( strcmp( inputTapple.protcol, node->entry.protcol) == 0 ) &&
			( inputTapple.srcport == node->entry.srcport ) &&
			( inputTapple.dstport == node->entry.dstport ) 
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

	if ( time < inputTapple.reach_time )
	{	
		// 1秒辺りのヒット率を求める処理, 別に関数を作成した方が良いかも
		hitrate_per_sec[(int)time - 1] = (double)hit_per_sec / ( (double)hit_per_sec + (double)miss_per_sec );
		hit_per_sec = 0;
		miss_per_sec = 0;
		time = time + 1;
	}

	while( tmp != head[index] )
	{
		if ( isEqual( inputTapple, tmp ) == 1 )
		{
			hit_per_sec = hit_per_sec + 1;
			hitflag = hitflag + 1;
			return tmp;
		}
		else
		{
			tmp = tmp->prev;
		}
	}

	miss_per_sec = miss_per_sec + 1;
	miss = miss + 1;
	return NULL;
}

/* listのエントリの操作, キャッシュのポリシーによって内容が変化, 下はLRUポリシー */
void listOperation( tuple_t x, int index, char * operation )
{
	node_t * tmp;
	if ( strcmp( operation, "lru" ) == 0 )
	{
		lruPolicy( x, index );
	}
	else if ( strcmp( operation, "sp") == 0 )
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
