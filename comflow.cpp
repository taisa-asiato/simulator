#include "define.h"

using namespace std;

int isSimilarFlow( user_list_t * tmp_user, tuple_t tuple )
{
	vector< string > split_dstip, split_dstip_reg;
	user_list_t * tmp_userlist = tmp_user;
	int ret_value = 1;
	split_dstip = split( tuple.dstip, '.' );
	//cout << split_dstip[0] << " " << split_dstip[1] << " " << split_dstip[2] << " " << split_dstip[3] << endl;
	//for ( int i = 0 ; i < tmp_userlist->flow_number ; i++ )
	while( tmp_userlist != NULL )
	{
		split_dstip_reg = split( tmp_userlist->blacksentflow->flowid.dstip, '.' );
		if ( 
			( split_dstip_reg[0] == split_dstip[0] ) && 
			( split_dstip_reg[1] == split_dstip[1] ) && 
			( split_dstip_reg[2] == split_dstip[2] ) )
		{
			ret_value += 5;
		}
		/*else if (
				( split_dstip_reg[0] == split_dstip[0] ) && 
				( split_dstip_reg[1] == split_dstip[1] ) )
		{
			ret_value += 5;
		}
		*/
		if ( tmp_userlist->blacksentflow->flowid.dstport == tuple.dstport )
		{
			ret_value += 5;
		} 

		tmp_userlist = tmp_userlist->next;
	}

	return ret_value;
}


int ump_isSimilarFlow( tuple_t tuple )
{
	vector< string > split_dstip, split_dstip_reg;
	int ret_value = 1;
	split_dstip = split( tuple.dstip, '.' );
	std::list< sent_flow_t > tmp_sentflow = ump_userlist[tuple.srcip]->sentflow;
	//cout << split_dstip[0] << " " << split_dstip[1] << " " << split_dstip[2] << " " << split_dstip[3] << endl;
	for ( auto itr = tmp_sentflow.rbegin() ; itr != tmp_sentflow.rend() ; itr++ )
	{
		split_dstip_reg = split( itr->flowid.dstip, '.' );
		if ( 
			( split_dstip_reg[0] == split_dstip[0] ) && 
			( split_dstip_reg[1] == split_dstip[1] ) && 
			( split_dstip_reg[2] == split_dstip[2] ) )
		{
			ret_value += 5;
		}
		/*else if (
				( split_dstip_reg[0] == split_dstip[0] ) && 
				( split_dstip_reg[1] == split_dstip[1] ) )
		{
			ret_value += 5;
		}
		*/
		if ( itr->flowid.dstport == tuple.dstport )
		{
			ret_value += 5;
		} 
	}

	return ret_value;
}
