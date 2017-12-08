#include "define.h"

using namespace std;

int isSimilarFlow( user_list_t * tmp_user, tuple_t tuple )
{
	vector< string > split_dstip, split_dstip_reg;
	user_list_t * tmp_userlist = tmp_user;
	int ret_value = 1;
	split_dstip = split( tuple.dstip, '.' );
	//cout << split_dstip[0] << " " << split_dstip[1] << " " << split_dstip[2] << " " << split_dstip[3] << endl;
	for ( int i = 0 ; i < tmp_userlist->flow_number ; i++ )
	{
		split_dstip_reg = split( tmp_userlist->blacksentflow->flowid.dstip, '.' );
		if ( 
			( split_dstip_reg[0] == split_dstip[0] ) && 
			( split_dstip_reg[1] == split_dstip[1] ) && 
			( split_dstip_reg[2] == split_dstip[2] ) )
		{
			ret_value += 5;
		}
		else if (
				( split_dstip_reg[0] == split_dstip[0] ) && 
				( split_dstip_reg[1] == split_dstip[1] ) )
		{
			ret_value += 5;
		}

		if ( tmp_userlist->blacksentflow->flowid.dstport == tuple.dstport )
		{
			ret_value += 5;
		} 

		tmp_userlist = tmp_userlist->next;
	}

	return ret_value;
}
