#include "define.h"

/* リストを生成する際に仮に使用するリスト */
/* 特定のフロー間に, 何種類の要素があるかを調べる必要がある */
/* その時にフロー間の異なるフローのみを要素に登録しておく */
void anotherListInsert( node_t * staticnode, another_node_t * pointer )
{
	another_node_t * newnode;
	another_node_t * tmp;
	newnode = malloc( sizeof( another_node_t ) );

	tmp = pointer;
	while( tmp->next != NULL )
	{
		tmp = tmp->next;
	}

	tmp->next = newnode;
	strcpy( newnode->entry_another.dstip, staticnode->entry.dstip );
	strcpy( newnode->entry_another.srcip, staticnode->entry.srcip );
	strcpy( newnode->entry_another.protcol, staticnode->entry.protcol );
	newnode->entry_another.dstport = staticnode->entry.dstport;
	newnode->entry_another.srcport = staticnode->entry.srcport;
	newnode->packet_count = 1;
	newnode->pointer_for_listStatic = staticnode;
	newnode->next = NULL;
//	fprintf( stdout, "		list insertion %s %s %s %d %d\n", newnode->entry_another.srcip,  newnode->entry_another.dstip, 
//		newnode->entry_another.protcol, newnode->entry_another.srcport, newnode->entry_another.dstport );	

}
/* , リストのどれかに登録されているかどうか */
another_node_t * isRegisteredStaticList( tuple_t inputTapple, another_node_t * pointer )
{
	another_node_t * tmp;

	tmp = pointer;
	while( tmp != NULL )
	{
		if ( isEqualStaticList( inputTapple, tmp ) == EQUAL )
		{
			tmp->packet_count = tmp->packet_count + 1;
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
int isEqualStaticList( tuple_t inputTapple, another_node_t * pointer )
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
	pointer->packet_count = 0;
}

/* 仮のリストの要素を全て削除する関数 */
void deleteAnotherList( another_node_t * pointer )
{
	another_node_t * tmp;
	another_node_t * tmp1;

	tmp1 = pointer;
	tmp = tmp1->next;
//	fprintf( stdout, "	deleteAnotherList started\n" );
	while( tmp != NULL )
	{
		tmp1->next = tmp->next;
		free( tmp );
		tmp = tmp1->next;
	}
//	free( tmp1 );
	pointer->next = NULL;
}

/* 仮のリストの要素を削除しつつ, パケット数が1のものの統計情報も更新する */
void deleteAnotherListAndUpdate( another_node_t * pointer )
{
	another_node_t * tmp;
	another_node_t * tmp1;

	tmp1 = pointer;
	tmp = tmp1->next;
	while( tmp != NULL )
	{
		if ( ( tmp->packet_count == 1 ) && ( tmp->pointer_for_listStatic->search_flag == 0 ) ) 
		{
			//1パケットフローであることを示す
			//define文で明確にしておいた方が良い気がする
			tmp->pointer_for_listStatic->search_flag = 2;
		}
		tmp1->next = tmp->next;
		free( tmp );
		tmp = tmp1->next;
	}
	pointer->next = NULL;
}

void printAnotherList( another_node_t * pointer )
{
//	fprintf( stdout, "	printAnotherList started\n" );
	another_node_t * tmp;
	tmp = pointer;
	while( tmp != NULL )
	{
			fprintf( stdout, "	%s %s %s %d %d\n", tmp->entry_another.srcip,  tmp->entry_another.dstip, 
				tmp->entry_another.protcol, tmp->entry_another.srcport, tmp->entry_another.dstport );	
		tmp = tmp->next;	
	}
}
