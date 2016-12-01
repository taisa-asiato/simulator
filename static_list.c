#include "define.h"

/* リストを生成する際に仮に使用するリスト */
/* 特定のフロー間に, 何種類の要素があるかを調べる必要がある */
/* その時にフロー間の異なるフローのみを要素に登録しておく */
void anotherListInsert( tapple_t x, another_node_t * pointer )
{
	another_node_t * newnode;
	another_node_t * tmp;
	newnode = malloc( sizeof( another_node_t ) );

	tmp = pointer;
	while( tmp != NULL )
	{
		tmp = tmp->next;
	}

	tmp = newnode;
	strcpy( newnode->entry_another.dstip, x.dstip );
	strcpy( newnode->entry_another.srcip, x.srcip );
	strcpy( newnode->entry_another.protcol, x.protcol );
	newnode->entry_another.dstport = x.dstport;
	newnode->entry_another.srcport = x.srcport;
	newnode->next = NULL;
}
/* inputTappleが, リストのどれかに登録されているかどうか */
another_node_t * isRegisteredStaticList( tapple_t inputTapple, another_node_t * pointer )
{
	another_node_t * tmp;
	tmp = pointer;
	while( tmp != NULL )
	{
		if ( isEqualStaticList( inputTapple, tmp ) == 1 )
		{
			return tmp;
		}
		else
		{
			tmp = tmp->next;
		}
	}

	return NULL;
}

/* pointerのentry_anotherとinputTappleが等しいか判定する関数 */
/* 等しい場合にはEQUAL( 1 ), 等しくない場合にはNOTEQUAL( 0 ) を返す*/
int isEqualStaticList( tapple_t inputTapple, another_node_t * pointer )
{
	if (
			( strcmp( inputTapple.srcip, pointer->entry_another.srcip ) == 0 ) &&
			( strcmp( inputTapple.dstip, pointer->entry_another.dstip ) == 0 ) &&
			( strcmp( inputTapple.protcol, pointer->entry_another.protcol ) == 0 ) &&
			( inputTapple.srcport == pointer->entry_another.srcport ) &&
			( inputTapple.dstport == pointer->entry_another.dstport ) 
	  )
	{
		return EQUAL;
	}
	else 
	{
		return NOTEQUAL;
	}
}

/* 仮のリストの初期化を行う関数 */
void anotherListInit( another_node_t * pointer )
{
	pointer->next = NULL;
	strcpy( pointer->entry_another.dstip, "0" );
	strcpy( pointer->entry_another.srcip, "0" );
	strcpy( pointer->entry_another.protcol, "0" );
	pointer->entry_another.srcport = 0;
	pointer->entry_another.dstport = 0;
}

/* 仮のリストの要素を全て削除する関数 */
void deleteAnotherList( another_node_t * pointer )
{
	another_node_t * tmp;
	another_node_t * tmp1;

	tmp1 = pointer;
	tmp = tmp1->next;
	while( tmp != NULL )
	{
		free( tmp1 );
		tmp1 = tmp;
		tmp = tmp1->next;
	}
	free( tmp1 );
}
