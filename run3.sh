user_max=100
flow_max=100
threshold=20
interval=0.1
ON="ON"
debug="NO"
remove="NOT"
for i in 10 20 30 40 50 60 70 80 90 100
do
	threshold=$i
       echo "./sim3 $1 $2 $ON $user_max $flow_max $threshold $interval $debug $remove"
       ./sim3 $1 $2 $ON $user_max $flow_max $threshold $interval $debug $remove
#	ruby $HOME/postgra/Honda_lab/experiments/caputer/rate_of_1packet.rb \
#		$HOME/postgra/Honda_lab/experiments/caputer/20160501/1packetflow-wide20160501.txt \
#		./skip.out
done
