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
			fprintf( stdout, "%s %s %s %d %d first:%f\n", pointer->entry.srcip, pointer->entry.dstip, pointer->entry.protcol, pointer->entry.srcport, pointer->entry.dstport, pointer->entry.reach_time );
			total_number = listSearchStatic( pointer, number );
			printValueStatic( pointer, total_number );
			printTimeRelative( pointer );
			pointer = pointer->next;
		}

	}
}

/* 出力する内容は, フローIDとフレーム数 */
void printValueStatic( node_t * pointer, int number )
{
	fprintf( stdout, "ID:%s %s %s %d %d | frame_number %d\n", pointer->entry.srcip, pointer->entry.dstip, pointer->entry.protcol, pointer->entry.srcport, pointer->entry.dstport, number );
}

/* リストの要素をindex毎に全て出力する */
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

//////////////////////////////////////////////////
/* 時間間隔を保持するリストの要素を全て出力する */
//////////////////////////////////////////////////
void printTimeRelative( node_t * pointer )
{
	time_interval_t * tmp_time;
	tmp_time = pointer->time_relative;
	while( tmp_time != NULL )
	{
		fprintf( stdout, "%f -> ", tmp_time->time_interval );
		tmp_time = tmp_time->next;
	}

	fprintf( stdout, "\n" );
}
/* 入力のdelete_pointerを消す */
/* numberによってどのポインタ配列なのかを指定する */
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

/* フローIDとパケット数, パケット間隔を保持するリストを保持するリストの初期化を行う */
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
		TimeListInit( head_static[index_number] );
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
	TimeListInit( newnode );

	newnode->next = NULL;
	newnode->prev = p_static[number];
	p_static[number] = newnode;
}

/* 入力xと等しいエントリを持つノードを探す */
/* 探すリストは, 入力numberの値によって決定する */
int listSearchStatic( node_t * search_pointer, int number )
{

	int total_number = 1;
	double tmp_time;
	double diff;
	node_t * pointer;
	node_t * tmp_pointer;

	tmp_time = search_pointer->entry.reach_time;
	fprintf( stdout, "first:%f\n", tmp_time );
	pointer = search_pointer->next;
	while( pointer != NULL )
	{
		if ( isEqual( search_pointer->entry, pointer ) == EQUAL )
		{
			total_number = total_number + 1;
			//diffには一つ前のフローと現在のフローの時間間隔を代入
			//tmp_timeには一つ前のフローの時間が入っている
			diff = pointer->entry.reach_time - tmp_time;
			TimeListInsert( search_pointer, diff );//時間間隔を時間間隔リストに追加
			tmp_time = pointer->entry.reach_time;
			pointer = listDeleteStatic( pointer, number );
		}
		pointer = pointer->next;
	}

	return total_number;
}

//////////////////////////////////////////////////////
/* パケットの到着間隔を保持するリストの初期化を行う */
//////////////////////////////////////////////////////
void TimeListInit( node_t * pointer )
{
	pointer->time_relative = malloc( sizeof( time_interval_t ) );
	pointer->time_relative->time_interval = 0;
	pointer->time_relative->next = NULL;
}

////////////////////////////////////////////////////////////////
/* パケットの到着間隔を保持するリストに, 新しい要素を追加する */
////////////////////////////////////////////////////////////////
void TimeListInsert( node_t * pointer, double interval )
{

	time_interval_t * time_newnode; //追加する新しいノード
	time_interval_t * tmp_time_node;
	time_newnode = malloc( sizeof( time_interval_t ) );
	
	tmp_time_node = pointer->time_relative; //始めのノードを代入
	while( tmp_time_node->next != NULL )
	{
		tmp_time_node = tmp_time_node->next; //リストの最終端まで行く
	}

	tmp_time_node->next = time_newnode;
	time_newnode->time_interval = interval;
	time_newnode->next = NULL;
}
