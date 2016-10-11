/*
 *TODO
 *index$B$rA}$d$9(B ( index$B@8@.$N4X?t(B, 5tapple$B$N$I$N$$$A$r%$%s%G%C%/%9$K$9$k$+$N7hDj(B )
 * aaaaa 
 * */
/* header file */
#include "define.h"
int entry_size = 0; //$B8=:_$N%(%s%H%j?t$r;X$9(B
int INDEX_MAX = ENTRY_MAX / WAY_MAX;

/* $B%U%!%$%k$+$iFI$_<h$C$?(B1$B9T$r6uGr$GJ,3d$79=B$BN$N3F%U%#!<%k%I$KBeF~(B */
tapple_t stringSplit( char *tapple_string )
{
	tapple_t tapple;
	char *cp;
	struct in_addr inp;
	
	cp = strtok( tapple_string, " " );
	memcpy( tapple.srcip, cp, 16 );

	cp = strtok( NULL, " " );
	memcpy( tapple.dstip, cp, 16 );

	cp = strtok( NULL, " " );
	memcpy( tapple.protcol, cp, 3 );

	cp = strtok( NULL, " " );
	tapple.srcport = atoi( cp );

	cp = strtok( NULL, " " );
	tapple.dstport = atoi( cp );

	return tapple;
}

void binaryConvert( tapple_t x, char * bin_tapple )
{
	struct in_addr inp;
	int tcp, position, i, tmp, j;
	char eight_byte[8];
	unsigned long tmp_ip;

	if ( inet_aton( x.srcip, &inp ) == 1 )
	{
		position = 0;
		for ( i = 0 ; i < 4 ; i = i + 1 )
		{
			for ( j = 0 ; j < 8 ; j = j + 1 )
			{
				if( inp.s_addr % 2 == 1 )
				{
					eight_byte[j] = 49;
				}
				else if ( inp.s_addr % 2 == 0 )
				{
					eight_byte[j] = 48;
				}
				inp.s_addr = inp.s_addr / 2;
			}

			for ( j = 0 ; j < 8 ; j = j + 1 )
			{
				bin_tapple[j + 8*i + position] = eight_byte[7 - j];
			}
		}
	}

	if ( inet_aton( x.dstip, &inp ) == 1 )
	{
		position = 32;
		for ( i = 0 ; i < 4 ; i = i + 1 )
		{
			for ( j = 0 ; j < 8 ; j = j + 1 )
			{
				if( inp.s_addr % 2 == 1 )
				{
					eight_byte[j] = 49;
				}
				else if ( inp.s_addr % 2 == 0 )
				{
					eight_byte[j] = 48;
				}
				inp.s_addr = inp.s_addr / 2;
			}

			for ( j = 0 ; j < 8 ; j = j + 1 )
			{
				bin_tapple[j + 8*i + position] = eight_byte[7 - j];
			}
		}

	}

	if ( strcmp( x.protcol, "TCP" ) == 0 )
	{
		position = 71;
		tcp = 6;
		for ( i = 0 ; i < 8 ; i = i + 1 )
		{
			if( tcp % 2 == 1 )
			{
				bin_tapple[position] = 49;
			}
			else if ( tcp % 2 == 0 )
			{
				bin_tapple[position] = 48;
			}
			position = position - 1;
			tcp = tcp / 2;
		}
	}

	position = 87;
	tmp = x.srcport;
	for ( i = 0 ; i < 16 ; i = i + 1 )
	{
		if( tmp % 2 == 1 )
		{
			bin_tapple[position] = 49;
		}
		else if ( tmp % 2 == 0 )
		{
			bin_tapple[position] = 48;
		}
		position = position - 1;
		tmp = tmp / 2;
	}

	position = 103;
	tmp = x.dstport;
	for ( i = 0 ; i < 16 ; i = i + 1 )
	{
		if( tmp % 2 == 1 )
		{
			bin_tapple[position] = 49;
		}
		else if ( tmp % 2 == 0 )
		{
			bin_tapple[position] = 48;
		}
		position = position - 1;
		tmp = tmp / 2;
	}

	bin_tapple[104] = '\0';
}

int main( int argc, char *argv[] )
{
	char fivetapple[200];
	char bin_tapple[105];
	char * tmp;
	tapple_t tapple;
	int i = 1;
	int index = 0;

	listInit();
	if ( ( inputfile = fopen( argv[1], "r" ) ) == NULL )
	{
		fprintf( stdout, "file open error\n" );
		return 0;
	}

	while( fgets( fivetapple, 200, inputfile ) != NULL )
	{
		tapple = stringSplit( fivetapple );
		binaryConvert( tapple, bin_tapple ); //5tapple$B$r(B104$B%S%C%H$N(B2$B?J?t$KJQ49$9$k(B
		index = crcOperation( bin_tapple ); //8$B%S%C%H$N%$%s%G%C%/%9$r:n@.(B

		fprintf( stdout, "NO%d - %s %s %s %d %d  index is %d\n", i, tapple.srcip, tapple.dstip, tapple.protcol, tapple.srcport, tapple.dstport, index );
		listOperation( tapple, index );
//		fprintf( stdout, "%d\n", index );
		printValue();
		i = i + 1;
	}

	fclose( inputfile );

	return 0;
}
