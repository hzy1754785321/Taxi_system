
DC=./driver_client 
OC=../order_client
SE=../server
MAKE=make
system:
	cd $(DC) && $(MAKE) && cd $(OC) && $(MAKE) && cd $(SE) && $(MAKE)

