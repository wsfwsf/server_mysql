VPATH=..
source=$(wildcard ../*.cc)
obj=$(patsubst %.cc,%.o,$(source))

all:main Client

Client:../client/client.cc
	g++ $^ -o $@

main:$(obj)
	g++ -o $@ $^ -lmysqlclient

%.o:%.cc
	g++ -o $@ -c $^

%.d:%.cc
	@set -e; rm -f $@; g++ -MM $< $(INCLUDEFLAGS) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(obj:.o=.d)

.PHONY:clean 
clean:
	rm  ../*.o ../*.d.*

