#include "define.h"

using namespace std;
////////////////////////////////////////////////////////////////
/* BlackListの様々な処理を行う関数                            */
////////////////////////////////////////////////////////////////
//TODO: if 文ないのブロックが大きいので分割すべき(　個別の関数に分割すべき )
int ump_UserListOperation( tuple_t tuple )
{
	user_list_t * tmp_node;
	sent_flow_t * tmp_flow;
	string flow_string = tuple.dstip + " " + tuple.protcol + 
		" " + to_string( tuple.srcport ) + " " + to_string( tuple.dstport );
	tuple_t tmp_tuple;
	int i = 0, count = 0, tmp_count = 1;

	//一定時間ごとにブラックリストの初期化を行う
	if ( userlist_init_time < tuple.reach_time )
	{	// countにはblackuser数が入る
		// UserListの初期化及び初期化ノードの優先度の変更を行う
		// fprintf( stdout, "UserList Init\n" );
		//userListIntervalInit();
		userListIntervalInitAll();
	}

	// blackuserの初期化を行う
//	blackuserIntervalInit( tuple.reach_time );

	auto user_itr = ump_userlist.find( tuple.srcip );
	tmp_node = user_itr->second;
	if (  user_itr == ump_userlist.end() )
	{ 	// userip がUserListに登録されていない場合
		ump_registUser( tuple );
		ump_registFlow( tuple );
	}
	else 
	{	// userip がUserListに登録されている場合
		// useripが登録されているエントリを優先度が最も高いところにおく
		ump_moveFirstNode( tmp_node );
		auto flow_itr = tmp_node->ump_sentflow.find( flow_string );
		if ( flow_itr != tmp_node->ump_sentflow.end() )
		{	//flowが登録されている場合
			tmp_flow = flow_itr->second;
			tmp_flow->count = tmp_flow->count + 1;
			if ( tmp_flow->count == 2 )
			{
				tmp_node->onepacket_number--;
			}
		}
		else
		{	// flowが登録されていない場合
			//tmp_count = isSimilarFlow( tmp_black_node, tuple );
			ump_registFlow( tuple );
			tmp_node->onepacket_number++;
			tmp_node->flow_number++;
			
		}

	}


	//  flowの数がしきい値を超えた場合にはuserlistとする
	if ( ( tmp_node->onepacket_number >= THRESHOLD ) && ( tmp_node->isblackuser == 0 ) )
	{	
		//		fprintf( stdout, "user is registered as blackuser\n" );
		tmp_node->isblackuser = 1;
		tmp_node->registered_time = tuple.reach_time;
	}

	return 0;
}

/////////////////////////////////////////////////
/* unordered_mapを用いたuserlistのエントリ検索 */
/////////////////////////////////////////////////
user_list_t * ump_isUserRegistered( tuple_t tuple )
{
	if ( ump_userlist[tuple.srcip] != NULL )
	{
		return ump_userlist[tuple.srcip];
	}
	else 
	{
		return NULL;
	}
	return NULL;
}

//////////////////////////////////////////////////
/* unordered_mapを用いたuserlistへのエントリ登録*/
//////////////////////////////////////////////////
void ump_registUser( tuple_t tuple )
{
	string tmp_userip;
	if ( ump_userlist.size() < USER_MAX )
	{
		ump_userlist[tuple.srcip] = new user_list_t;
	}
	else 
	{
		ump_deleteUserListLastNode();
		ump_userlist[tuple.srcip] = new user_list_t;
	}

	// ポインタの挿し直し( LRUを想定 )
	ump_userlist[tuple.srcip]->next = ump_userlist_head->next;
	ump_userlist[tuple.srcip]->prev = ump_userlist_head;
	ump_userlist_head->next->prev = ump_userlist[tuple.srcip];
	ump_userlist_head->next = ump_userlist[tuple.srcip];
	ump_substituteUser( ump_userlist[tuple.srcip], tuple );

	// UserList内のsent_flow_tのメモリ確保を行う
	ump_initSentFlowList( tuple );
}

///////////////////////////////////
/* unordered_mapにuserを登録する */
///////////////////////////////////
void ump_substituteUser( user_list_t * tmp, tuple_t tuple )
{
	tmp->userip = tuple.srcip;
	tmp->flow_number = 1;
	tmp->onepacket_number = 1;
	tmp->isblackuser = 0;

}

/////////////////////////////////////////////////////////////////////////////////////////////
/* unordered_mapで構成したUserListのFlowListの先頭(next)および末端(prev)のメモリ確保を行う */
/////////////////////////////////////////////////////////////////////////////////////////////
void ump_initSentFlowList( tuple_t tuple )
{
	ump_userlist[tuple.srcip]->blacksentflow = new sent_flow_t;
	ump_userlist[tuple.srcip]->blacksentflow_end = new sent_flow_t;

	ump_userlist[tuple.srcip]->blacksentflow->next = ump_userlist[tuple.srcip]->blacksentflow_end;
	ump_userlist[tuple.srcip]->blacksentflow->prev = NULL;

	ump_userlist[tuple.srcip]->blacksentflow_end->next = NULL;
	ump_userlist[tuple.srcip]->blacksentflow_end->prev = ump_userlist[tuple.srcip]->blacksentflow;
}

/////////////////////////////////////////////////////////////
/* unordered_mapで構成したUserlistの最後のノードを削除する */
/////////////////////////////////////////////////////////////
void ump_deleteUserListLastNode()
{
	string tmp_userip;
	tmp_userip = ump_userlist_end->prev->userip;
	
	ump_userlist_end = ump_userlist_end->prev->prev->next;
	ump_userlist_end->prev = ump_userlist_end->prev->prev;

	auto itr = ump_userlist.find( tmp_userip );
	if ( itr != ump_userlist.end() )
	{
		cout << "Find delete node" << endl;
		ump_userlist.erase( itr );
	}
}
/////////////////////////////////////////////////////////////
/* unordered_mapで構成したFlowListの最後のノードを削除する */
/////////////////////////////////////////////////////////////
void ump_deleteFlowListLastNode( tuple_t tuple  )
{	// 引数なしでも消去できる
	string tmp_flow = tuple.dstip + " " + tuple.protcol + " " + to_string( tuple.srcport ) + " " + to_string( tuple.dstport);
	auto itr = ump_userlist[tuple.srcip]->ump_sentflow.find( tmp_flow );
	if ( itr != ump_userlist[tuple.srcip]->ump_sentflow.end() )
	{
		cout << "Fine delete node" << endl;
		ump_userlist[tuple.srcip]->ump_sentflow.erase( itr );
	}
}

///////////////////////////////////////////////////////////
/* unordered_mapで構成したUserListのFlowListの操作を行う */
///////////////////////////////////////////////////////////
void ump_registFlow( tuple_t tuple )
{	//TODO : if分の条件文でコード量を半分にできる
	string flow_string = tuple.dstip + " " + tuple.protcol 
		+ " " + to_string( tuple.srcport ) + " " + to_string( tuple.dstport );
	
	if ( ump_userlist[tuple.srcip]->ump_sentflow.size() < FLOW_MAX )
	{
		ump_userlist[tuple.srcip]->ump_sentflow[flow_string] = new sent_flow_t;
	}
	else
	{
		ump_deleteFlowListLastNode( tuple );
		ump_userlist[tuple.srcip]->ump_sentflow[flow_string] = new sent_flow_t;
	}
	// ポインタの挿し直し ( LRU )
	ump_userlist[tuple.srcip]->ump_sentflow[flow_string]->next = ump_userlist[tuple.srcip]->blacksentflow->next;
	ump_userlist[tuple.srcip]->ump_sentflow[flow_string]->prev = ump_userlist[tuple.srcip]->blacksentflow;
	ump_userlist[tuple.srcip]->blacksentflow->next->prev = ump_userlist[tuple.srcip]->ump_sentflow[flow_string];
	ump_userlist[tuple.srcip]->blacksentflow->next = ump_userlist[tuple.srcip]->ump_sentflow[flow_string];
	substituteFlow( ump_userlist[tuple.srcip]->ump_sentflow[flow_string], tuple );
}

/////////////////////////////////////////////////////////////////////////////////////
/* unordered_mapで構成されたUserListで, 引数のノードを優先度が一番高いところにおく */
/////////////////////////////////////////////////////////////////////////////////////
void ump_moveFirstNode( user_list_t * tmp_user )
{
	user_list_t * tmp1, tmp2;
	if ( tmp_user == ump_userlist_head->next )
	{	// 一番優先度が高いところにある場合には何もしない
		return;
	}
	else if ( tmp_user == ump_userlist_end->prev )
	{
		tmp_user->prev->next = ump_userlist_end;
		ump_userlist_end->prev = tmp_user->prev;
	}
	
	tmp1 = ump_userlist_head->next;
	tmp1->prev = tmp_user;
	tmp_user->next = tmp1;
	tmp_user->prev = ump_userlist_head;
	ump_userlist_head->next = tmp_user;
}
