#include "define.h"

void flowStatic()
{
	int number, total_number = 0, max_num;
	node_t * pointer;
	node_t * tmp;

	max_num = ENTRY_MAX / WAY_MAX;
//	max_num = 3;
//	intervalFlowCounter();	

	for ( number = 0 ; number < max_num ; number = number + 1 )
	{
		pointer = head_static[number]->next;
		fprintf( stdout, "==========INDEX%03d==========\n", number );
		while( pointer != NULL )
		{
	//		fprintf( stdout, "%s %s %s %d %d first:%f\n", pointer->entry.srcip, pointer->entry.dstip,
	//				pointer->entry.protcol, pointer->entry.srcport, pointer->entry.dstport, pointer->entry.reach_time );
			total_number = listSearchStatic( pointer, number );/* pointerはリストの先頭要素*/
			printValueStatic( pointer, total_number );
//			printTimeRelative( pointer );

//			pointer = freeListStatic( pointer ); //メモリ容量が足りないときの苦肉の策
			pointer = pointer->next;
		}

	}
}

/* 統計情報を保持するリストの要素を削除する関数 */
/* 主にメモリ容量が少ないときのことを考えたもの */
node_t * freeListStatitc( node_t * pointer )
{
	//pointerは検査済みの統計情報リストの要素
	node_t * tmp;
	tmp = pointer->next;
	free( pointer ); 

	//返り値は, 統計情報リストのうち, 一番最近検査が終わったものの次の要素
	return tmp;
}

/* 出力する内容は, フローIDとフレーム数 */
void printValueStatic( node_t * pointer, int number )
{
	fprintf( stdout, "%f, %s, %s, %s, %d, %d, %d, %f\n",pointer->entry.reach_time, pointer->entry.srcip, pointer->entry.dstip, 
			pointer->entry.protcol, pointer->entry.srcport, pointer->entry.dstport, pointer->flow_interval, pointer->diff_of_time );
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
			fprintf( stdout, "%s %s %s %d %d\n", pointer->entry.srcip, pointer->entry.dstip, 
					pointer->entry.protcol, pointer->entry.srcport, pointer->entry.dstport );
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

////////////////////////////////////////////////////
/* 入力のdelete_pointerを消す関数                 */
/* numberによってどのポインタ配列なのかを指定する */
/* 返り値として, 消した要素の直前要素を返す       */
////////////////////////////////////////////////////
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
		head_static[index_number]->search_flag = 0;
//		TimeListInit( head_static[index_number] );
		//値は代入しておくべき？
		//初めは最後のノードを指すポインタも先頭ノードを指しておく
		
		p_static[index_number] = head_static[index_number];
		/*
		strcpy( init_tapple.srcip, "0" );
		strcpy( init_tapple.dstip, "0" );
		strcpy( init_tapple.protcol, "0" );
		init_tapple.srcport = 0;
		init_tapple.dstport = 0;
		*/
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
	listStaticSubstitute( newnode );
	newnode->search_flag = 0;
//	TimeListInit( newnode );

	newnode->next = NULL;
	newnode->prev = p_static[number];
	p_static[number] = newnode;
}

void listStaticSubstitute( node_t * node )
{
	node->flow_interval = 0;
	node->diff_of_time = -1;
}

//////////////////////////////////////////////////
/* 入力xと等しいエントリを持つノードを探す      */
/* 探すリストは, 入力numberの値によって決定する */
/* search_pointerはリストの先頭要素を指す       */
//////////////////////////////////////////////////
int listSearchStatic( node_t * search_pointer, int number )
{

	if ( search_pointer->search_flag == 1 )
	{	//search_flagが1の場合は検索済みであることを示す
		//検索済みのものは再検索不要なので, 処理をスキップすることができる
		return 0;
	}

	int total_number = 1;
	int flow_interval_number = 0;
	double tmp_time;
	double diff;
	node_t * pointer;
	node_t * tmp_pointer;

	another_node_t * another_tmp_list = malloc( sizeof( another_node_t ) ); 
	//フロー間の要素の解析を行うための仮のリストの先頭要素を指すリスト
	anotherListInit( another_tmp_list );

	tmp_time = search_pointer->entry.reach_time;
	pointer = search_pointer->next;//探す要素の次の要素から探す
//	fprintf( stdout, "%f, %s, %s, %s, %d, %d, %d, %f\n",search_pointer->entry.reach_time, search_pointer->entry.srcip, search_pointer->entry.dstip, 
//			search_pointer->entry.protcol, search_pointer->entry.srcport, search_pointer->entry.dstport, search_pointer->flow_interval, search_pointer->diff_of_time );

	while( pointer != NULL )
	{
	//	printAnotherList( another_tmp_list );
		if ( isEqual( search_pointer->entry, pointer ) == EQUAL )
		{
			total_number = total_number + 1;//同一フローの総数を計算する

			pointer->flow_interval = flow_interval_number;//同一フロー間のフローの種類を代入
			flow_interval_number = 0;//同一フロー間のフローの種類の初期化

			//diffには一つ前のフローと現在のフローの時間間隔を代入
			//tmp_timeには一つ前のフローの時間が入っている
			diff = pointer->entry.reach_time - tmp_time;
			pointer->diff_of_time = diff;
			
			//TimeListInsert( search_pointer, diff );//時間間隔を時間間隔リストに追加
			tmp_time = pointer->entry.reach_time;
			//pointer = listDeleteStatic( pointer, number );

			pointer->search_flag = 1; //探索が終了していることを示す
		}
		else
		{	//search_pointerとは違う5タプルを持つフローである場合
			if ( isRegisteredStaticList( pointer->entry, another_tmp_list ) == NULL )
			{
				anotherListInsert( pointer->entry, another_tmp_list );
				flow_interval_number = flow_interval_number + 1;
			}
		}
		pointer = pointer->next;
	}
	
	if ( search_pointer->diff_of_time == -1 )
	{
		//他に同一フローが見つからないため, 時間差に-1が入ったままのフローに関しては, 時間差を0とする
		//また, 同一フロー間のフローの種類も0とする
		search_pointer->diff_of_time = 0;
		search_pointer->flow_interval = 0;
	}

//	printAnotherList( another_tmp_list );
	deleteAnotherList( another_tmp_list );
//	printAnotherList( another_tmp_list );

	//検索キーの要素の検索が終了
	search_pointer->search_flag = 1;
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
