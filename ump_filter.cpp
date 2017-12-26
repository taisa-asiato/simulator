#include "define.h"

using namespace std;
////////////////////////////////////////////////////////////////
/* BlackListの様々な処理を行う関数                            */
////////////////////////////////////////////////////////////////
//TODO: if 文ないのブロックが大きいので分割すべき(　個別の関数に分割すべき )
int ump_UserListOperation( tuple_t tuple )
{
	ump_user_t * tmp_node;
	sent_flow_t * tmp_flow;
	string flow_string = tuple.dstip + " " + tuple.protcol + 
		" " + to_string( tuple.srcport ) + " " + to_string( tuple.dstport );
	tuple_t tmp_tuple;
	int i = 0, count = 0, tmp_count = 1;
	
//	cout << "ump user list size  ; " << ump_userlist.size() << endl;
//	cout << "umpl user list size ; " << ump_l_userlist.size() << endl;
	//一定時間ごとにブラックリストの初期化を行う
	if ( userlist_init_time < tuple.reach_time )
	{	// countにはblackuser数が入る
		// UserListの初期化及び初期化ノードの優先度の変更を行う
		// fprintf( stdout, "UserList Init\n" );
		ump_userListIntervalInit();
		// ump_userListIntervalInitAll();
	}

	// blackuserの初期化を行う
	ump_blackuserListIntervalInit( tuple.reach_time );

	auto user_itr = ump_userlist.find( tuple.srcip );
	if (  user_itr == ump_userlist.end() )
	{ 	// userip がUserListに登録されていない場合
		ump_registUser( tuple );
		ump_registFlow( tuple );
	}
	else 
	{	// userip がUserListに登録されている場合
		// useripが登録されているエントリを優先度が最も高いところにおく
		ump_moveFirstNode( tuple );
		// cout << "move user node to top" << endl;
		// cout << ump_userlist[tuple.srcip]->sentflow.size() << endl;
		auto user_itr = ump_userlist.find( tuple.srcip );
		//auto flow_itr = user_itr->second->ump_sentflow.find( flow_string );
		auto flow_itr = user_itr->second->sentflow.begin();
		for ( ; flow_itr != user_itr->second->sentflow.end() ; flow_itr++ )
		{
			if (
				( flow_itr->flowid.srcip == tuple.srcip ) &&
				( flow_itr->flowid.dstip == tuple.dstip ) &&
				( flow_itr->flowid.protcol == tuple.protcol ) &&
				( flow_itr->flowid.srcport == tuple.srcport ) && 
				( flow_itr->flowid.dstport == tuple.dstport )
			   )
			{
				break;
			}
				
		}
	//	auto fl_itr = user_itr->second->sentflow.find( ump_tmp_flow );

		if ( flow_itr != user_itr->second->sentflow.end() /* flow_itr != user_itr->second->ump_sentflow.end() */ )
		{	//flowが登録されている場合
			//flow_itr->second->count = flow_itr->second->count + 1; 
			flow_itr->count = flow_itr->count + 1;
			if ( flow_itr->count == 2 /* flow_itr->second->count == 2 */)
			{
				user_itr->second->onepacket_number--;
			}
		}
		else
		{	// flowが登録されていない場合
			tmp_count = ump_isSimilarFlow( tuple );
			//cout << " #!#!#!#!#!#! NEW FLOW" << endl;
			ump_registFlow( tuple );
			user_itr->second->onepacket_number += tmp_count;
			user_itr->second->flow_number++;
		}

	}

	//  flowの数がしきい値を超えた場合にはuserlistとする
	if ( ( ump_userlist[tuple.srcip]->onepacket_number >= THRESHOLD ) && ( ump_userlist[tuple.srcip]->isblackuser == 0 ) )
	{	
	//	cout << "user is registered as black" << endl;
		ump_userlist[tuple.srcip]->isblackuser = 1;
		ump_userlist[tuple.srcip]->registered_time = tuple.reach_time;
		// ump_blackuser[tuple.srcip] = ump_userlist[tuple.srcip];
	}

	// cout << "unordered_map " << " ========== " << " list " << endl;
//	cout << "ump size  ; " << ump_userlist[tuple.srcip]->ump_sentflow.size() << endl;
//	cout << "list size ; " << ump_userlist[tuple.srcip]->sentflow.size() << endl;
//	for ( auto itr = ump_userlist[tuple.srcip]->ump_sentflow.begin() ; itr != ump_userlist[tuple.srcip]->ump_sentflow.end() ; itr++ )
//	{
//		cout << itr->first << " ============ " << 
//			itr->second->flowid.dstip << " " << 
//			itr->second->flowid.protcol << " " << 
//			itr->second->flowid.srcport << " " << 
//			itr->second->flowid.dstport << endl;
//	}
//
	ump_printUserList();
	return 0;
}

/////////////////////////////////////////////////
/* unordered_mapを用いたuserlistのエントリ検索 */
/////////////////////////////////////////////////
std::list< ump_user_t >::iterator ump_isUserRegistered( tuple_t tuple )
{
	auto user_itr = ump_userlist.find( tuple.srcip );
	if ( user_itr != ump_userlist.end() )
	{
//		return user_itr;
	}

//	return user_itr;
}

//////////////////////////////////////////////////
/* unordered_mapを用いたuserlistへのエントリ登録*/
//////////////////////////////////////////////////
void ump_registUser( tuple_t tuple )
{
	string tmp_userip;
	ump_user_t tmp_user = ump_initialUserNode( tuple );

	//cout << "USER SIZE IS " << ump_l_userlist.size() << endl;
	//cout << "ump USER SIZE IS " << ump_userlist.size() << endl;
	if ( ump_userlist.size() < USER_MAX )
	{	// UserListに空きがある場合
		// UserListの優先度が一番高い場所にUserを登録する
		ump_l_userlist.push_front( tmp_user );
		ump_userlist[tuple.srcip] = ump_l_userlist.begin();
	}
	else 
	{	// UserListに空きが無い場合
		// UserListに空きノードを作成し. 空きノードにuserを追加する
		ump_deleteUserListLastNode();
		ump_l_userlist.push_front( tmp_user );
		ump_userlist[tuple.srcip] = ump_l_userlist.begin();
	}
}

/////////////////////////////////////////////////////////////
/* unordered_mapで構成されたuserlistのノードの初期化を行う */
/////////////////////////////////////////////////////////////
ump_user_t ump_initialUserNode( tuple_t tuple )
{
	ump_user_t tmp_user;
	tmp_user.userip = tuple.srcip;
	tmp_user.flow_number = 1;
	tmp_user.onepacket_number = 1;
	tmp_user.isblackuser = 0;
	tmp_user.registered_time = 0;

	return tmp_user;
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
	// cout << "init flow" << endl;
//	ump_userlist[str_userip]->blacksentflow = new sent_flow_t;
//	ump_userlist[str_userip]->blacksentflow_end = new sent_flow_t;

//	ump_userlist[str_userip]->blacksentflow->next = ump_userlist[str_userip]->blacksentflow_end;
//	ump_userlist[str_userip]->blacksentflow->prev = NULL;

//	ump_userlist[str_userip]->blacksentflow_end->next = NULL;
//	ump_userlist[str_userip]->blacksentflow_end->prev = ump_userlist[str_userip]->blacksentflow;
}

/////////////////////////////////////////////////////////////
/* unordered_mapで構成したUserlistの最後のノードを削除する */
/////////////////////////////////////////////////////////////
void ump_deleteUserListLastNode()
{
	auto list_itr = ump_l_userlist.end();
	list_itr--;
	string reg_userip = list_itr->userip;

	auto itr = ump_userlist.find( reg_userip );
	if ( itr != ump_userlist.end() )
	{
		// cout << "Delete user node === " << reg_userip << endl;
		ump_l_userlist.pop_back();
		ump_userlist.erase( reg_userip );
	}
}

/////////////////////////////////////////////////////////////
/* unordered_mapで構成したFlowListの最後のノードを削除する */
/////////////////////////////////////////////////////////////
void ump_deleteFlowListLastNode( tuple_t tuple )
{	
	// cout << "Delete Operation" << endl;
	auto flow_itr = ump_userlist[tuple.srcip]->sentflow.end();
	flow_itr--; // end()の一つ前の要素を指す
	string tmp_flow = flow_itr->flowid.dstip + " " + flow_itr->flowid.protcol + " "
		+ to_string( flow_itr->flowid.srcport ) + " " + to_string( flow_itr->flowid.dstport );

	ump_userlist[tuple.srcip]->sentflow.pop_back();
	auto itr = ump_userlist[tuple.srcip]->ump_sentflow.find( tmp_flow );
	if ( itr != ump_userlist[tuple.srcip]->ump_sentflow.end() )
	{
		//cout << "delete last flow node" << endl;
		ump_userlist[tuple.srcip]->ump_sentflow.erase( itr );
		//cout << "flow is last node" << endl;
	}
}

////////////////////////////////////////
/* 初期化用ノードを作成しリターンする */
////////////////////////////////////////
sent_flow_t initialFlowNode( tuple_t tuple )
{	//TODO:正しくは, ノードに代入する関数なので, 名称変更が必要かも
	sent_flow_t tmp;
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
	// tupleの値をセットした構造体を返す
	sent_flow_t tmp_flow = initialFlowNode( tuple );	

	//cout << "FLOW SIZE IS " << ump_userlist[tuple.srcip]->sentflow.size() << endl; 
	//cout << "ump FLOW SIZE IS " << ump_userlist[tuple.srcip]->ump_sentflow.size() << endl;
	//cout << "MAX FLOW SIZE IS " << FLOW_MAX << endl;
	if ( ump_userlist[tuple.srcip]->sentflow.size() < FLOW_MAX )
	{
		ump_userlist[tuple.srcip]->sentflow.push_front( tmp_flow );
		ump_userlist[tuple.srcip]->ump_sentflow[flow_string] = ump_userlist[tuple.srcip]->sentflow.begin();
	}
	else
	{
		//cout << "OVER MAX FLOW SIZE" << endl;
		ump_deleteFlowListLastNode( tuple );
		ump_userlist[tuple.srcip]->sentflow.push_front( tmp_flow );
		ump_userlist[tuple.srcip]->ump_sentflow[flow_string] = ump_userlist[tuple.srcip]->sentflow.begin();
	}
}

/////////////////////////////////////////////////////////////////////////////////////
/* unordered_mapで構成されたUserListで, 引数のノードを優先度が一番高いところにおく */
/////////////////////////////////////////////////////////////////////////////////////
/*std::unordered_map< 
	std::string, 
	std::list< ump_user_t > 
>::iterator*/
void ump_moveFirstNode( tuple_t tuple )
{
	auto itr = ump_userlist[tuple.srcip];
	auto user_itr = ump_l_userlist.begin();
	ump_user_t tmp_node;

	if ( user_itr == itr )
	{	// 登録されたエントリが一番優先度が高いエントリの場合
		return;
	}
	else 
	{	// 登録されたエントリが一番優先度が高いエントリではない場合
		/* tmp_nodeに値を一時退避 */
		tmp_node.userip = itr->userip;
		tmp_node.flow_number = itr->flow_number;
		tmp_node.onepacket_number = itr->onepacket_number;
		tmp_node.isblackuser = itr->isblackuser;
		tmp_node.registered_time = itr->registered_time;

		/* イテレータ破壊が起きるので,先にコピーコンストラクタを作成し, 
		 * 同じ内容を保持するリストを複製しておく */
		std::list< sent_flow_t > tmp( itr->sentflow );
		tmp_node.sentflow = tmp;

		/* userlsitからuser_itrを削除 */
		ump_l_userlist.erase( itr );
		ump_l_userlist.push_front( tmp_node );
		auto u_itr = ump_l_userlist.begin();
		//ump_userlist[tmp_node.userip] = ump_l_userlist.begin();
		ump_userlist[tmp_node.userip] = u_itr;
	}
}

///////////////////////////////////////////////////////
/* unoredered_mapdで構成されたUserListの初期化を行う */
///////////////////////////////////////////////////////
void ump_initUserList()
{
//	ump_userlist_head->next = ump_userlist_end;
//	ump_userlist_head->prev = NULL;
//	ump_userlist_end->next = NULL;
//	ump_userlist_end->prev = ump_userlist_head;
}

///////////////////////////////////////////////////////////////////////////
/* unordered_mapで構成されたUserListの初期を行う(初期時間で初期化を行う) */
///////////////////////////////////////////////////////////////////////////
void ump_userListIntervalInitAll()
{
	ump_l_userlist.clear();
	ump_userlist.clear();
	userlist_init_time = userlist_init_time + USERLIST_INIT_INTERVAL;
}


////////////////////////////////////////////////////////////////////
/* ump_userListIntervalInitAll()と同じだが, blackuser以外を初期化 */
////////////////////////////////////////////////////////////////////
void ump_userListIntervalInit()
{
	string srcip;
	for ( auto itr = ump_l_userlist.begin() ; itr != ump_l_userlist.end() ; )
	{
		srcip = itr->userip;
		if ( itr->isblackuser == 0 )
		{	
			auto del = ump_userlist.find( srcip );
			itr = ump_l_userlist.erase( itr );
			ump_userlist.erase( del );
		}
		else 
		{
			itr++;
		}
	}
	userlist_init_time = userlist_init_time + USERLIST_INIT_INTERVAL;
}

/////////////////////////////////////
/* blackuserのflowlistのみ削除する */
/////////////////////////////////////
void ump_blackuserListIntervalInit( double now_time )
{
	for ( auto itr = ump_userlist.begin() ; itr != ump_userlist.end() ; itr++ )
	{
		if ( 	
			( itr->second->isblackuser == 1 ) && 
			( now_time - itr->second->registered_time > BLACKUSER_INIT_INTERVAL ) 
		)
		{
			ump_userlist[itr->second->userip]->sentflow.clear();
			ump_userlist[itr->second->userip]->ump_sentflow.clear();
			ump_userlist[itr->second->userip]->isblackuser = 0;
			ump_userlist[itr->second->userip]->flow_number = 0;
			ump_userlist[itr->second->userip]->onepacket_number = 0;
			ump_userlist[itr->second->userip]->registered_time = 0.0;

			ump_initSentFlowList( itr->second->userip );
			//cout << "Flashed Blackuser" << endl;
		}
	}
}

////////////////////////////////////////////////////////
/* unoredered_mapで構成されたuserlistの要素を出力する */
////////////////////////////////////////////////////////
void ump_printUserList()
{
	int i = 0;
	cout << "====================================================================" << endl;
	for ( auto itr = ump_l_userlist.begin() ; itr != ump_l_userlist.end() ; itr++ )
	{
		fprintf( stdout, "[NO%03d] -- ", i );
		cout << "userip:" << itr->userip << " flow_number:" << itr->flow_number << " registered_time:" << itr->registered_time << endl;
		//ump_printSentFlow( itr->sentflow );
		ump_printSentFlowReverse( itr->sentflow );
		i++;
	}
	cout << "====================================================================" << endl;
}

/////////////////////////////////////////////////////////////////
/* unordered_mapで構成されたuserlistのflowlistの要素を出力する */
/////////////////////////////////////////////////////////////////
void ump_printSentFlow( std::list< sent_flow_t > tmp_sent_flow )
{
	int i = 1;
	for ( auto itr = tmp_sent_flow.begin() ; itr != tmp_sent_flow.end() ; itr++ )
	{
		fprintf( stdout, "	|	[NO%03d] -- ", i );
		cout << itr->flowid.dstip << " " << itr->flowid.protcol << " " << itr->flowid.srcport << " " << itr->flowid.dstport << " " << itr->count << endl;
		i++;
	}
}

/////////////////////////////////////////////////////////////////
/* unordered_mapで構成されたuserlistのflowlistの要素を出力する */
/* 逆向きに出力する					       */
/////////////////////////////////////////////////////////////////
void ump_printSentFlowReverse( std::list< sent_flow_t > tmp_sent_flow )
{
	int i = 1;
	for ( auto itr = tmp_sent_flow.rbegin() ; itr != tmp_sent_flow.rend() ; itr++ )
	{
		fprintf( stdout, "	|	[NO%03d] -- ", i );
		cout << itr->flowid.dstip << " " << itr->flowid.protcol << " " << itr->flowid.srcport << " " << itr->flowid.dstport << " " << itr->count << endl;
		i++;
	}
}

