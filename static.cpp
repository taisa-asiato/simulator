#include "define.h"

void flowStaticMain()
{
//	flowStaticForSingle();
	flowStaticForParallel();
}

/* 解析を逐次処理で行う */
void flowStaticForSingle()
{
	int number, total_number = 0, max_num;
	node_t * pointer;
	node_t * tmp;

	max_num = ENTRY_MAX / WAY_MAX;
//	max_num = 3;
//	intervalFlowCounter();	

	for ( number = 12 ; number < 13 ; number = number + 1 )
	{
		pointer = head_static[number]->next;
		fprintf( stdout, "==========INDEX%03d==========\n", number );
		while( pointer != NULL )
		{
			listSearchStatic( pointer->entry, number );/* pointerはリストの先頭要素*/
			printValueStatic( pointer, total_number );
//			printTimeRelative( pointer );

//			pointer = freeListStatic( pointer ); //メモリ容量が足りないときの苦肉の策
			pointer = pointer->next;
		}

	}
}

/* 解析を並列に行う */
void flowStaticForParallel()
{
	int number, total_number = 0, max_num;
	node_t * pointer[256];
	node_t * tmp;

	max_num = ENTRY_MAX / WAY_MAX;

	#pragma omp parallel for
	for ( number = 0 ; number < max_num ; number = number + 1 )
	{
		pointer[number] = head_static[number]->next;
		while( pointer[number] != NULL )
		{
	//		fprintf( stdout, "%s %s %s %d %d first:%f\n", pointer->entry.srcip, pointer->entry.dstip,
	//				pointer->entry.protcol, pointer->entry.srcport, pointer->entry.dstport, pointer->entry.reach_time );
			listSearchStatic( pointer[number]->entry, number );/* pointerはリストの先頭要素*/
//			printValueStatic( pointer[number], total_number );
//			printTimeRelative( pointer );
//			pointer = freeListStatic( pointer ); //メモリ容量が足りないときの苦肉の策
			pointer[number] = pointer[number]->next;
		}
		fprintf( stdout, "==========INDEX%03d========== completed\n", number );

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
	pointer = analyze->next;

	while( pointer != NULL )
	{
//		if ( pointer->crcnum == 0 )
//		{
		fprintf( stdout, "%f, %s, %s, %s, %d, %d, %d, %f, %d\n",pointer->entry.reach_time, pointer->entry.srcip, pointer->entry.dstip, 
				pointer->entry.protcol, pointer->entry.srcport, pointer->entry.dstport, pointer->crc_flow_interval, pointer->diff_of_time, pointer->crcnum );
//		}
		pointer = pointer->next;
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
node_t * listDeleteStatic( node_t * delete_pointer, int number )
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
//	pointer = delete_pointer->prev;
	free( delete_pointer );

	return pointer;
}

/* フローIDとパケット数, パケット間隔を保持するリストを保持するリストの初期化を行う */
void listInitStatic()
{
	int index_number = 0;
	int way_number = 0;
	tuple_t init_tuple;

	fprintf( stdout, "init for static  finished\n" );
	for ( index_number = 0 ; index_number < INDEX_MAX ; index_number = index_number + 1 )
	{

		head_static[index_number] = ( node_t * )malloc( sizeof( node_t ) );
		head_static[index_number]->next = NULL;
		head_static[index_number]->prev = NULL;

		/* head_staticポインタ,  */
		strcpy( head_static[index_number]->entry.srcip, "0" );
		strcpy( head_static[index_number]->entry.dstip, "0" );
		head_static[index_number]->entry.srcport = 0;
		head_static[index_number]->entry.dstport = 0;
		strcpy( head_static[index_number]->entry.protcol, "0");
//		head_static[index_number]->search_flag = 0;
		//		TimeListInit( head_static[index_number] );
		//値は代入しておくべき？
		//初めは最後のノードを指すポインタも先頭ノードを指しておく

		p_static[index_number] = head_static[index_number];
	}

	analyze = ( node_t * )malloc( sizeof( node_t ) );
	strcpy( analyze->entry.dstip, "0" );
	strcpy( analyze->entry.srcip, "0" );
	strcpy( analyze->entry.protcol, "0" );
	analyze->entry.srcport = 0;
	analyze->entry.dstport = 0;
	analyze->next = NULL;
	analyze->prev = NULL;
	analyze_end = analyze;

	static_search = ( node_t * )malloc( sizeof( node_t ) );
	strcpy( static_search->entry.dstip, "0" );
	strcpy( static_search->entry.srcip, "0" );
	strcpy( static_search->entry.protcol, "0" );
	static_search->entry.srcport = 0;
	static_search->entry.dstport = 0;
	static_search->next = NULL;
	static_search->prev = NULL;
	search_end = static_search;
}

/* listに新しく要素を作成する時に使う, listMake, listAddとかの方が良かったかも */
/* こ関数は統計情報を取るためのリストに要素を追加する関数 		       */
/* 返り値の1は, リストの要素数を1増やした事を示す 			       */
int listInsertStatic( node_t * end, tuple_t x, int number )
{
	node_t *newnode;
	newnode = ( node_t * )malloc( sizeof( node_t ) );

	//5タプルの情報を代入
	listSubstitute( newnode, x );
	newnode->flow_interval = 0;
	newnode->diff_of_time = 0;
	newnode->crcnum = number;
	newnode->crc_flow_interval = 0;

	if ( analyze_end == end )
	{
		//解析用の値の初期化
		analyze_end->next = newnode;

		//ポインタ付け替え
		newnode->next = NULL;
		newnode->prev = analyze_end;
		analyze_end = newnode;
	}
	else if ( search_end == end )
	{
		//解析用の値の初期化
		search_end->next = newnode;

		//ポインタ付け替え
		newnode->next = NULL;
		newnode->prev = search_end;
		search_end = newnode;
	}
	else if ( p_static[number] == end )
	{
		p_static[number]->next = newnode;
		newnode->next = NULL;
		newnode->prev = p_static[number];
		p_static[number] = newnode;
	}
	
//	fprintf( stdout, "listInsertStatic finished\n" );
	//リストの要素の個数を1カウントアップする?
	return 1;
}

/* 代入関数ではないので, 名前を変更する必要がある */
void listStaticSubstitute( node_t * node )
{
	node->flow_interval = 0;
//	newnode->crcnum = number;
	node->crc_flow_interval = 0;
	node->diff_of_time = 0;
}

//////////////////////////////////////////////////
/* 入力xと等しいエントリを持つノードを探す      */
/* 探すリストは, 入力numberの値によって決定する */
/* search_pointerはリストの先頭要素を指す       */
//////////////////////////////////////////////////
void listSearchStatic( tuple_t search_tuple, int number )
{
	node_t * pointer;
	int tmp_crc = 0;
	int tmp_interval = 0;
	int registered = 0;
	double tmp_diff_time;
	double tmp_crc_time;

	if ( p_static[number] == head_static[number] )
	{	//
		listInsertStatic( p_static[number], search_tuple, number );
	}

	pointer = p_static[number];
	while ( pointer != head_static[number] )
	{
		if ( isEqual( search_tuple,  pointer ) == EQUAL )
		{
			//検査用リストに登録されている時
			registered = 1;
	//		fprintf( stdout, "registered\n" );
			tmp_diff_time = search_tuple.reach_time - pointer->entry.reach_time;
//			tmp_crc_time = search_tuple.reach_time - tmp_crc_time;;

			//pointerの要素の削除
			if ( pointer == p_static[number] )
			{	//pointerがリストの最後の要素のIDと同じ場合は, 削除->追加ではなく, 値の入れ替えで終了
				p_static[number]->entry.reach_time = search_tuple.reach_time;
		
			}
			else 
			{
				//リストの最後の要素にこのエントリを追加
				listDeleteStatic( pointer, number );
				listInsertStatic( p_static[number], search_tuple, number );
			}
			break;
		}
		else 
		{
			//登録されていない時
//			fprintf( stdout, "not registered\n" );
			tmp_interval = tmp_interval + 1;
			if ( number == pointer->crcnum )
			{
				tmp_crc = tmp_crc + 1;
				tmp_crc_time = pointer->entry.reach_time;
			}
		}
		pointer = pointer->prev;
	}

	if ( registered == 0 )
	{
		listInsertStatic( p_static[number], search_tuple, number );
		analyze_end->flow_interval = 0;
		analyze_end->crc_flow_interval = 0;
		analyze_end->diff_of_time = 0;
	}
	else if ( registered == 1 )
	{
		analyze_end->flow_interval = tmp_interval;
		analyze_end->diff_of_time = tmp_diff_time;
		analyze_end->crc_flow_interval = tmp_crc;
		analyze_end->diff_of_crc_time = tmp_crc_time;
	}

//	fprintf( stdout, "listSearchStatic finished\n" );
}

//////////////////////////////////////////////////////
/* パケットの到着間隔を保持するリストの初期化を行う */
//////////////////////////////////////////////////////
void TimeListInit( node_t * pointer )
{
	pointer->time_relative = ( time_interval_t * )malloc( sizeof( time_interval_t ) );
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
	time_newnode = ( time_interval_t * )malloc( sizeof( time_interval_t ) );
	
	tmp_time_node = pointer->time_relative; //始めのノードを代入
	while( tmp_time_node->next != NULL )
	{
		tmp_time_node = tmp_time_node->next; //リストの最終端まで行く
	}

	tmp_time_node->next = time_newnode;
	time_newnode->time_interval = interval;
	time_newnode->next = NULL;
}
