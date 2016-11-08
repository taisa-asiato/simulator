#include "define.h"

void flowStatic()
{
	int number, total_number = 0;
	node_t * pointer;
	node_t * empty;

	for ( number = 0 ; number < ENTRY_MAX / WAY_MAX ; number = number + 1 )
	{
		pointer = head_static[number]->next;
		fprintf( stdout, "==========INDEX%03d==========\n", number );
		while( pointer != NULL )
		{
			total_number = listSearchStatic( pointer, number );
			printValueStatic( pointer, total_number );
			pointer = pointer->next;
		}

	}
}

/* 出力する内容は, フローIDとフレーム数 */
void printValueStatic( node_t * pointer, int number )
{
	fprintf( stdout, "ID:%s %s %s %d %d | frame_number %d\n", pointer->entry.srcip, pointer->entry.dstip, pointer->entry.protcol, pointer->entry.srcport, pointer->entry.dstport, number );
}

/* リストの要素を全て出力する */
void printValueStaticAll()
{
	int number = 0;
	node_t * pointer;
	for ( number = 0 ; number < ENTRY_MAX / WAY_MAX ; number = number + 1 )
	{
		pointer = head_static[number]->next;
		fprintf( stdout, "==========INDEX%03d==========\n", number );
		while( pointer != NULL )
		{
			fprintf( stdout, "%s %s %s %d %d\n", pointer->entry.srcip, pointer->entry.dstip, pointer->entry.protcol, pointer->entry.srcport, pointer->entry.dstport );
			pointer = pointer->next;
		}

	}

}

/* 入力のdelete_pointer, numberをもとにリストに登録されているノードを削除する */
node_t *  listDeleteStatic( node_t * delete_pointer, int number )
{
	node_t * pointer;
	if ( delete_pointer->next == NULL )
	{	//消すノードがリストの一番最後の場合
		p_static[number] = delete_pointer->prev; 
		p_static[number]->next = NULL;
		/* 
		 * p_static[number]を使用しない方法も考えたが, 
		 * p_staticでリストの一番最後を常に指す方が良い気がしたので
		 * この書き方をした
		 * 
		 * */
	}
	else if ( delete_pointer != NULL )
	{
		delete_pointer->prev->next = delete_pointer->next;
		delete_pointer->next->prev = delete_pointer->prev;
	}
	pointer = delete_pointer->prev;
	free( delete_pointer );

	return pointer;
}

/* listの初期化, headはダミーノードとした */
/* この関数は統計情報を取るためのリストの初期化を行う関数 */
void listInitStatic()
{
	int index_number = 0;
	int way_number = 0;
	tapple_t init_tapple;

	fprintf( stdout, "init for static  finished\n" );
	for ( index_number = 0 ; index_number < INDEX_MAX ; index_number = index_number + 1 )
	{

		head_static[index_number] = malloc( sizeof( node_t ) );
		head_static[index_number]->next = NULL;
		head_static[index_number]->prev = NULL;

		/* head_staticポインタ,  */
		strcpy( head_static[index_number]->entry.srcip, "0" );
		strcpy( head_static[index_number]->entry.dstip, "0" );
		head_static[index_number]->entry.srcport = 0;
		head_static[index_number]->entry.dstport = 0;
		strcpy( head_static[index_number]->entry.protcol, "0");
		//値は代入しておくべき？
		//初めは最後のノードを指すポインタも先頭ノードを指しておく
		p_static[index_number] = head_static[index_number];

		strcpy( init_tapple.srcip, "0" );
		strcpy( init_tapple.dstip, "0" );
		strcpy( init_tapple.protcol, "0" );
		init_tapple.srcport = 0;
		init_tapple.dstport = 0;
	}
}

/* listに新しく要素を作成する時に使う, listMake, listAddとかの方が良かったかも */
/* こ関数は統計情報を取るためのリストに要素を追加する関数 */
void listInsertStatic( tapple_t x, int number )
{
//	fprintf( stdout, "insert started\n" );
	node_t *newnode;

	newnode = malloc( sizeof( node_t ) );
	p_static[number]->next = newnode;

	listSubstitute( newnode, x );

	newnode->next = NULL;
	newnode->prev = p_static[number];
	p_static[number] = newnode;
}

/* 入力xと等しいエントリを持つノードを探す */
/* 探すリストは, 入力numberの値によって決定する */
int listSearchStatic( node_t * search_pointer, int number )
{
	int total_number = 1;
	node_t * pointer;
	node_t * tmp_pointer;

	pointer = search_pointer->next;
	while( pointer != NULL )
	{
		if ( isEqual( search_pointer->entry, pointer ) == EQUAL )
		{
			total_number = total_number + 1;
			pointer = listDeleteStatic( pointer, number );
		}
		pointer = pointer->next;
	}

	return total_number;
}
