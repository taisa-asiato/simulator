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
		ump_userListIntervalInitAll();
	}

	// blackuserの初期化を行う
//	ump_blackuserListIntervalInit( tuple.reach_time );

	auto user_itr = ump_userlist.find( tuple.srcip );
	if (  user_itr == ump_userlist.end() )
	{ 	// userip がUserListに登録されていない場合
		cout << "NEW USER " << tuple.srcip << endl;
		ump_registUser( tuple );
		ump_registFlow( tuple );
	}
	else 
	{	// userip がUserListに登録されている場合
		// useripが登録されているエントリを優先度が最も高いところにおく
		ump_moveFirstNode( user_itr->second );
		auto flow_itr = user_itr->second->ump_sentflow.find( flow_string );
		if ( flow_itr != user_itr->second->ump_sentflow.end() )
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
			user_itr->second->onepacket_number++;
			user_itr->second->flow_number++;
			
		}

	}

	tmp_node = ump_userlist[tuple.srcip];
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
	auto user_itr = ump_userlist.find( tuple.srcip );
	if ( user_itr != ump_userlist.end() )
	{
		return user_itr->second;
	}

	return NULL;
}

//////////////////////////////////////////////////
/* unordered_mapを用いたuserlistへのエントリ登録*/
//////////////////////////////////////////////////
void ump_registUser( tuple_t tuple )
{
	string tmp_userip;
	ump_user_t tmp_user = ump_initialUserNode( tuple );

	if ( ump_userlist.size() < USER_MAX )
	{
		ump_l_userlist.push_front( tmp_user );
		ump_userlist[tuple.srcip] = ump_l_userlist.begin();
	}
	else 
	{
		ump_deleteUserListLastNode();
		ump_l_userlist.push_front( tmp_user );
		ump_userlist[tuple.srcip] = ump_l_userlist.begin();
	}
}

/////////////////////////////////////////////////////////////
/* unordered_mapで構成されたuserlistのノードの初期化を行う */
/////////////////////////////////////////////////////////////
user_list_t ump_initialUserNode( tuple_t tuple )
{
	user_list_t tmp_user;
	tmp_user.userip = tuple.srcip;
	tmp_user.flow_number = 0;
	tmp_user.onepacket_number = 0;
	tmp_user.isblackuser = 0;
	tmp_user.registered_time = 0;
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
	tmp->registered_time = 0.0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/* unordered_mapで構成したUserListのFlowListの先頭(next)および末端(prev)のメモリ確保を行う */
/////////////////////////////////////////////////////////////////////////////////////////////
void ump_initSentFlowList( string str_userip )
{
	cout << "init flow" << endl;
	ump_userlist[str_userip]->blacksentflow = new sent_flow_t;
	ump_userlist[str_userip]->blacksentflow_end = new sent_flow_t;

	ump_userlist[str_userip]->blacksentflow->next = ump_userlist[str_userip]->blacksentflow_end;
	ump_userlist[str_userip]->blacksentflow->prev = NULL;

	ump_userlist[str_userip]->blacksentflow_end->next = NULL;
	ump_userlist[str_userip]->blacksentflow_end->prev = ump_userlist[str_userip]->blacksentflow;
}

/////////////////////////////////////////////////////////////
/* unordered_mapで構成したUserlistの最後のノードを削除する */
/////////////////////////////////////////////////////////////
void ump_deleteUserListLastNode()
{
	auto list_itr = ump_l_userlist.end();
	--list_itr;
	string reg_uerip = list_itr->userip;

	auto itr = ump_userlist.find( reg_userip );
	if ( itr != ump_userlist.end() )
	{
		cout << "Delete user node === " << reg_userip << endl;
		ump_l_userlist.pop_back();
		ump_userlist.erase( reg_userip );
	}
}

/////////////////////////////////////////////////////////////
/* unordered_mapで構成したFlowListの最後のノードを削除する */
/////////////////////////////////////////////////////////////
void ump_deleteFlowListLastNode( tuple_t tuple )
{	
	auto flow_itr = ump_userlist[tuple.srcip]->sentflow.end();
	--flow_itr;
	string tmp_flow = tuple.dstip + " " + tuple.protcol + " "
		+ to_string( tuple.srcport ) + " " + to_string( tuple.dstport);
	auto itr = ump_userlist[tuple.srcip]->ump_sentflow.find( tmp_flow );
	if ( itr != ump_userlist[tuple.srcip]->ump_sentflow.end() )
	{
		cout << "Fine delete node" << endl;
		ump_userlist[tuple.srcip]->sentflow.pop_back( flow_itr );
		ump_userlist[tuple.srcip]->ump_sentflow.erase( itr );
	}
}

////////////////////////////////////////
/* 初期化用ノードを作成しリターンする */
////////////////////////////////////////
sent_flot_t initialFlowNode( tuple_t tuple )
{	//TODO:正しくは, ノードに代入する関数なので, 名称変更が必要かも
	senf_flow_t tmp;
	tmp.count = 1;
	tmp.flowid.srcip = tuple.srcip;
	tmp.flowid.dstip = tuple.dstip;
	tmp.flowid.protcol = tuple.protcol;
	tmp.flowid.srcport = tuple.srcport;
	tmp.flowid.dstport = tuple.dstport;

	return tmp;
}

///////////////////////////////////////////////////////////
/* unordered_mapで構成したUserListのFlowListの操作を行う */
///////////////////////////////////////////////////////////
void ump_registFlow( tuple_t tuple )
{	//TODO : if分の条件文でコード量を半分にできる
	string flow_string = tuple.dstip + " " + tuple.protcol 
		+ " " + to_string( tuple.srcport ) + " " + to_string( tuple.dstport );
	sent_flow_t tmp_flow = initialFlowNode( tuple );	
	if ( ump_userlist[tuple.srcip]->ump_sentflow.size() < FLOW_MAX )
	{
		ump_userlist[tuple.srcip]->sentflow.push_front( tmp_flow );
		ump_userlist[tuple.srcip]->ump_sentflow[flow_string] = ump_userlist[tuple.srcip]->sentflow.begin();
	}
	else
	{
		ump_deleteFlowListLastNode( tuple );
		ump_userlist[tuple.srcip]->sentflow.push_front( tmp_flow );
		ump_userlist[tuple.srcip]->ump_sentflow[flow_string] = ump_userlist[tuple.srcip]->sentflow.begin();
	}
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
	else
	{
		tmp_user->prev->next = tmp_user->next;
		tmp_user->next->prev = tmp_user->prev;
		tmp_user->next = ump_userlist_head->next;
		tmp_user->prev = ump_userlist_head;
		ump_userlist_head->next = tmp_user;
		tmp_user->next->prev = tmp_user;
	}
}

///////////////////////////////////////////////////////
/* unoredered_mapdで構成されたUserListの初期化を行う */
///////////////////////////////////////////////////////
void ump_initUserList()
{
	ump_userlist_head->next = ump_userlist_end;
	ump_userlist_head->prev = NULL;
	ump_userlist_end->next = NULL;
	ump_userlist_end->prev = ump_userlist_head;
}

///////////////////////////////////////////////////////////////////////////
/* unordered_mapで構成されたUserListの初期を行う(初期時間で初期化を行う) */
///////////////////////////////////////////////////////////////////////////
void ump_userListIntervalInitAll()
{
	cout << "Clear ALL User At UserList" << endl;
	user_list_t * tmp;
	ump_userlist.clear();
	ump_initUserList();
	userlist_init_time = userlist_init_time + USERLIST_INIT_INTERVAL;
}


////////////////////////////////////////////////////////////////////
/* ump_userListIntervalInitAll()と同じだが, blackuser以外を初期化 */
////////////////////////////////////////////////////////////////////
void ump_userListIntervalInit()
{
	user_list_t * tmp;
	user_list_t * tmp1;
	tmp = ump_userlist_head->next;

	while ( tmp != ump_userlist_end )
	{
		tmp1 = tmp->next;
		if ( tmp->isblackuser == 0 )
		{	
			tmp1->prev = tmp->prev;
			tmp1->prev->next = tmp1;
			ump_userlist.erase(tmp->userip);
		}
		tmp = tmp1;	
	}
	userlist_init_time = userlist_init_time + USERLIST_INIT_INTERVAL;
}

/////////////////////////////////////
/* blackuserのflowlistのみ削除する */
/////////////////////////////////////
void ump_blackuserListIntervalInit( double now_time )
{
	user_list_t * tmp;
	tmp = ump_userlist_head->next;

	while ( tmp != ump_userlist_end )
	{
		if ( ( tmp->isblackuser == 1 ) && ( now_time - tmp->registered_time > BLACKUSER_INIT_INTERVAL ) )
		{
			ump_userlist[tmp->userip]->ump_sentflow.clear();
			tmp->isblackuser = 0;
			tmp->flow_number = 0;
			tmp->onepacket_number = 0;
			tmp->registered_time = 0.0;
			ump_initSentFlowList( tmp->userip );
		}
		tmp = tmp->next;
		//cout << "Delete BlackUser Spend Time Out Time" << endl;
	}
}

////////////////////////////////////////////////////////
/* unoredered_mapで構成されたuserlistの要素を出力する */
////////////////////////////////////////////////////////
void ump_printUserList( user_list_t * pointer_to_userlist )
{
	user_list_t * tmp = pointer_to_userlist->next;
	int i = 0;

	while ( tmp != ump_userlist_end )
	{
		if ( tmp->isblackuser == 1 )
		{
			fprintf( stdout, "[NO%03d] -- ", i );
			cout << "userip:" << tmp->userip << " flow_number:" << tmp->flow_number << " registered_time:" << tmp->registered_time << endl;
		} 
		else 
		{
			fprintf( stdout, "[NO%03d] -- ", i );
			cout << "userip:" << tmp->userip << " flow_number:" << tmp->flow_number << " registered_time:" << tmp->registered_time << endl;
		}
		ump_printSentFlow( tmp );
		tmp = tmp->next;
		i++;
	}
}

/////////////////////////////////////////////////////////////////
/* unordered_mapで構成されたuserlistのflowlistの要素を出力する */
/////////////////////////////////////////////////////////////////
void ump_printSentFlow( user_list_t * user_node )
{
	sent_flow_t * tmp_sent_node;
	int i = 1;
	tmp_sent_node = user_node->blacksentflow->next;
	while ( tmp_sent_node != user_node->blacksentflow_end )
	{
		fprintf( stdout, "	|	[NO%03d] -- ", i );
		cout <<  tmp_sent_node->flowid.dstip << " " << tmp_sent_node->flowid.protcol << " " << tmp_sent_node->flowid.srcport << " " << tmp_sent_node->flowid.dstport << " " << tmp_sent_node->count << endl;
		tmp_sent_node = tmp_sent_node->next;
		i++;
	}
}

