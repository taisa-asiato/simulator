#include "define.h"

/* LRUポリシー. 一部複数ポリシーで統一できる部分がある */
void lruPolicy( tuple_t x, int index )
{
	node_t * tmp;
	if ( ( tmp = isRegistered( x, index ) ) != NULL )
	{ 	//list中に登録されている場合
		hitOrMiss( x, 1 );
		if ( tmp == p[index] )
		{	/* 先頭ノードにある場合には優先度が一番高い事を示しているので何もせずに終了 */
			;
		}
		else if ( tmp != p[index] )
		{
			if( tmp == head[index]->next )
			{
				/* 最終ノードにある場合 */
				head[index]->next = tmp->next; // tmp->nextのノードが最低ノードになるため, head[index]->nextでそのノードを指す
				tmp->next->prev = head[index]; // tmp->nextのノードが最低ノードのため, このノードの前のノードはhead[index]となる
			}
			else if ( tmp != head[index]->next )
			{
				/* 最終ノードでない場合 */
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
		hitOrMiss( x, 0 );
		//list中に登録されていない場合には, 優先度の低いエントリを削除した後,　一番優先度の高い場所にエントリを登録し直す
		listDeleteFirst( index );
		listInsert( x, index );
	}
}

/* SPポリシー */
void spPolicy( tuple_t x, int index )
{
	node_t * tmp;
	node_t * tmp_next;

	if ( ( tmp = isRegistered( x, index ) ) != NULL )
	{	// list中に登録されている場合
		hitOrMiss( x, 1 );
		if ( tmp == p[index] )
		{	/* 先頭ノードにある場合には優先度が一番高い事を示しており何もせずに終了 */
			;
		}
		else if ( tmp != p[index] )
		{
			if ( tmp->next != p[index] )
			{
				/* やってる事は隣のノードとtmpのノードの入れ替え 
				 * すごく読みづらいので, 普通にswap関数とかを作るべきだった */
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
	{	// list中に登録されていない場合
		hitOrMiss( x, 0 );
		/* 一番優先度の低いエントリに登録する */
		listSubstitute( head[index]->next, x );
	}
}
