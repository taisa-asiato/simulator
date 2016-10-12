#include "define.h"

/* pointer が指すtapple_t構造体に xの各フィールドの値を代入する */
void listSubstitute( node_t * pointer, tapple_t x )
{
	strcpy( pointer->entry.srcip, x.srcip );
	strcpy( pointer->entry.dstip, x.dstip );
	strcpy( pointer->entry.protcol, x.protcol );
	pointer->entry.srcport = x.srcport;
	pointer->entry.dstport = x.dstport;
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
int isEqual( tapple_t inputTapple, node_t * node )
{
	if (
			( strcmp( inputTapple.srcip, node->entry.srcip ) == 0 ) &&
			( strcmp( inputTapple.dstip, node->entry.dstip ) == 0 ) &&
			( strcmp( inputTapple.protcol, node->entry.protcol) == 0 ) &&
			( inputTapple.srcport == node->entry.srcport ) &&
			( inputTapple.dstport == node->entry.dstport ) 
	  )
	{
//		fprintf( stdout, "hit\n" );
		hitflag = 1;
		return EQUAL;
	}
	else 
	{
		return NOTEQUAL;
	}
}

/* inputTappleが, リストのどれかに登録されているかどうか */
node_t * isRegistered( tapple_t inputTapple, int index )
{
	node_t *tmp;
	tmp = p[index];
	while( tmp != head[index] )
	{
		if ( isEqual( inputTapple, tmp ) == 1 )
		{
			return tmp;
		}
		else
		{
			tmp = tmp->prev;
		}
	}

	return NULL;
}

/* listのエントリの操作, キャッシュのポリシーによって内容が変化, 下はLRUポリシー */
void listOperation( tapple_t x, int index )
{
	/* どこらへんからLRUなのかはっきりとするべき. 関数を作る時にめんどくなる */
	node_t * tmp;
//	lruPolicy( x, index );
	spPolicy( x, index );
}

/* listの初期化, headはダミーノードとした */
void listInit()
{
	int index_number = 0;
	tapple_t init_tapple;
	int node_number = 0;

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

		strcpy( init_tapple.srcip, "0" );
		strcpy( init_tapple.dstip, "0" );
		strcpy( init_tapple.protcol, "0" );
		init_tapple.srcport = 0;
		init_tapple.dstport = 0;

		for ( node_number = 0 ; node_number < WAY_MAX ; node_number = node_number + 1 )
		{
			listInsert( init_tapple, index_number );	
		}
	}
}

/* listに新しく要素を作成する時に使う, listMake, listAddとかの方が良かったかも */
void listInsert( tapple_t x, int number )
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
	node_t *pointer;
	pointer = head[number]->next;
	head[number]->next = pointer->next;
	head[number]->next->prev = head[number];
	free( pointer );
}
