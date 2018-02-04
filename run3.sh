user_max=10
flow_max=10
threshold=100
interval=0.01
ele_threshold=100
nids_rate=1
ON="ON"
debug="NO"
remove="NOT"

	ON="OFF"
	policy="nocache"
	echo "./sim3 $1 $policy $ON $user_max $flow_max $threshold $interval $ele_threshold $debug $remove $nids_rate"
	./sim3 $1 $policy $ON $user_max $flow_max $threshold $interval $ele_threshold $debug $remove $nids_rate

	ON="ON"
	policy="nocache"
	echo "./sim3 $1 $policy $ON $user_max $flow_max $threshold $interval $ele_threshold $debug $remove $nids_rate"
	./sim3 $1 $policy $ON $user_max $flow_max $threshold $interval $ele_threshold $debug $remove $nids_rate

	ON="OFF"
	policy="lru"
	echo "./sim3 $1 $policy $ON $user_max $flow_max $threshold $interval $ele_threshold $debug $remove $nids_rate"
	./sim3 $1 $policy $ON $user_max $flow_max $threshold $interval $ele_threshold $debug $remove $nids_rate

	ON="ON"
	policy="lru"
	echo "./sim3 $1 $policy $ON $user_max $flow_max $threshold $interval $ele_threshold $debug $remove $nids_rate"
	./sim3 $1 $policy $ON $user_max $flow_max $threshold $interval $ele_threshold $debug $remove $nids_rate



#
#	ruby $HOME/postgra/Honda_lab/experiments/caputer/rate_of_1packet.rb \
#		$HOME/postgra/Honda_lab/experiments/caputer/20160501/1packetflow-wide20160501.txt \
#		./skip.out

#for i in 0.6 0.7 0.8 0.9
#do
#	user_max=100
#	nids_rate=$i
#	echo "./sim3 $1 $2 $ON $user_max $flow_max $threshold $interval $ele_threshold $debug $remove $nids_rate"
#	./sim3 $1 $2 $ON $user_max $flow_max $threshold $interval $ele_threshold $debug $remove $nids_rate
#
##
##	ruby $HOME/postgra/Honda_lab/experiments/caputer/rate_of_1packet.rb \
##		$HOME/postgra/Honda_lab/experiments/caputer/20160501/1packetflow-wide20160501.txt \
##		./skip.out
#done
#
#for i in 10 20 30 40 50 60 70 80 90
#do
#	user_max=100
#	threshold=$i
#	echo "./sim3 $1 $2 $ON $user_max $flow_max $threshold $interval $ele_threshold $debug $remove $nids_rate"
#	./sim3 $1 $2 $ON $user_max $flow_max $threshold $interval $ele_threshold $debug $remove $nids_rate
#
##
##	ruby $HOME/postgra/Honda_lab/experiments/caputer/rate_of_1packet.rb \
##		$HOME/postgra/Honda_lab/experiments/caputer/20160501/1packetflow-wide20160501.txt \
##		./skip.out
#done
#
#ON="OFF"
#echo "./sim3 $1 $2 $ON $user_max $flow_max $threshold $interval $ele_threshold $debug $remove $nids_rate"
#./sim3 $1 $2 $ON $user_max $flow_max $threshold $interval $ele_threshold $debug $remove $nids_rate
#
#
