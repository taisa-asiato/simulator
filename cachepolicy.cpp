#include "define.h"
using namespace std;

/* LRUポリシー */
void lruPolicy( tuple_t x, int index, node_t * tmp )
{
	node_t * newnode;

	if ( tmp )
	{ 	// Cacheのエントリにフローが登録されている場合 
		if ( tmp == p[index] )
		{	/* 優先度が一番高いエントリの場合には何もしない */
			;
		}
		else if ( tmp != p[index] )
		{
			if( tmp == head[index]->next )
			{
				/* 優先度が一番低いエントリに登録されている場合 */
				head[index]->next = tmp->next; // tmp->nextがさすエントリが優先度最低のエントリとなるので, head[index]->nextはそのエントリを指す
				tmp->next->prev = head[index]; // tmp->nextがさすエントリが優先度最低のエントリとなるので, その前のエントリ(ダミー)のhead[index]を指す
			}
			else if ( tmp != head[index]->next )
			{
				/* 優先度が一番低いエントリでない場合 */
				tmp->prev->next = tmp->next;
				tmp->next->prev = tmp->prev;
			}

			p[index]->next = tmp; //LRUなので, 一番優先度の高いエントリに追加
			tmp->next = NULL;
			tmp->prev = p[index];
			p[index] = tmp;
			listSubstitute( tmp, x );
		}
	}
	else
	{	//list中に5タプルが登録されていな場合
		//list中に登録されていない場合には, 優先度の低いエントリを削除した後,　一番優先度の高い場所にエントリを登録し直す
		newnode = head[index]->next;
		head[index]->next = newnode->next;
		newnode->next->prev = head[index];
		newnode->next = NULL;
		newnode->prev = p[index];
		p[index]->next = newnode;
		p[index] = newnode;
		listSubstitute( p[index], x );
	
		//listDeleteFirst( index );
		//cout << "flee last node" << endl;
		//listInsert( x, index );
	}
}

/* SPポリシー */
void spPolicy( tuple_t x, int index, node_t * tmp )
{
	node_t * tmp_next;

	if ( tmp )
	{	// list中に登録されている場合
		if ( tmp == p[index] )
		{	/* 先頭ノードにある場合には優先度が一番高い事を示しており何もせずに終了 */
			;
		}
		else if ( tmp != p[index] )
		{
			if ( tmp->next != p[index] )
			{	// 隣のノードとの入れ替え
				tmp_next = tmp->next->next;
				tmp_next->prev = tmp;
				tmp->next->prev = tmp->prev;
				tmp->prev->next = tmp->next;
				tmp->prev = tmp->next;
				tmp->prev->next = tmp;
				tmp->next = tmp_next;
			}
			else if ( tmp->next == p[index] )
			{
				p[index]->next = tmp;
				p[index]->prev = tmp->prev;
				tmp->next = NULL;
				tmp->prev->next = p[index];
				tmp->prev = p[index];
				p[index] = tmp;
			}
		}

	}
	else 
	{	// listに登録されていない場合
		/* 1番優先度の低いエントリに登録する */
		listSubstitute( head[index]->next, x );
	}
}
