all:
	touch memory
	touch semakey
	touch indexfile.txt
	g++ consumer.cc -o consumer
	g++ producer.cc -o producer
	
	

