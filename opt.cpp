#include "define.h"

using namespace std;

void OPT( tuple_t tuple, int index, node_t * tmp )
{
	node_t * tmp_node = head[index]->next;
	node_t * target_node;
	tuple_t tmp_tuple;
	string key_string, target_string;

	// cout << "opt calling" << endl;

	int flownum = 0, tmpnum = 0;
	if ( tmp )
	{	// Cacheのエントリにtupleのフローが登録されている場合
		// Cacheのエントリのフロー番号を変更し終了
		key_string = tmp_node->entry.srcip + " " + tmp_node->entry.dstip + " " + tmp_node->entry.protcol + " " + to_string( tmp_node->entry.srcport ) + " " + to_string( tmp_node->entry.dstport );
		auto itr = opt_list.find( key_string );
		goto POP_LIST;
	}
	else 
	{	// Cacheのエントリにtupleのフローが登録されていない場合
		// Cacheに登録されているフロー及び新たに来たフローのうち, 
		// 次にくるまでに最も時間がかかるフローを探す
		// cout << "not cached flow" << endl;

		// 初期化エントリと同じ値を持つ5タプル構造体を作成
		tmp_tuple.srcip = "0";
		tmp_tuple.dstip = "0";
		tmp_tuple.protcol = "0";
		tmp_tuple.srcport = 0;
		tmp_tuple.dstport = 0;

		// 空きエントリの探索
		while( tmp_node != NULL )
		{
			if ( isEqual( tmp_tuple, tmp_node ) == EQUAL )
			{
				listSubstitute( tmp_node, tuple );
				goto POP_LIST;
			}
			tmp_node = tmp_node->next;
		}

		tmp_node = head[index]->next;
		while ( tmp_node != NULL )
		{	
			key_string = tmp_node->entry.srcip + " " + tmp_node->entry.dstip + 
				" " + tmp_node->entry.protcol + " " + 
				to_string( tmp_node->entry.srcport ) + " " + to_string( tmp_node->entry.dstport );

			auto itr = opt_list.find( key_string );
			if ( itr->second.size() == 0 )
			{	// パケットが来ないフローのエントリがある場合
				listSubstitute( tmp_node, tuple );
				goto POP_LIST;
			}
			else 
			{	// 次のパケットが来るフローのエントリの場合
				auto list_itr = itr->second.begin();
				if ( *list_itr > flownum )
				{	// 次回到着までに一番時間がかかるフローを探索
					target_node = tmp_node;
					flownum = *list_itr;
				}
			}
			tmp_node = tmp_node->next;
		}
		listSubstitute( target_node, tuple );
		goto POP_LIST;
	}
POP_LIST:
	key_string = tuple.srcip + " " + tuple.dstip + " " + tuple.protcol + 
	" " + to_string( tuple.srcport ) + " " + to_string( tuple.dstport );
	opt_list[key_string].pop_front();

	return;
}
