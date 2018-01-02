CC		= gcc
FLAGS	= -c -g  
COMP = -o
OUT = ngrams
IN = main.o bloom_functions.o st_ngrams.o dy_ngrams.o Job_Scheduler.o
OUT1= exe
IN1=  dy_ngrams.o bloom_functions.o st_ngrams.o Job_Scheduler.o unit_testing.o 
files=small_dynamic_result.txt small_static_result.txt medium_static_result.txt medium_dynamic_result.txt large_static_result.txt  large_dynamic_result.txt 

all: main.o bloom_functions.o st_ngrams.o dy_ngrams.o Job_Scheduler.o #unit_testing.o 
	$(CC) $(COMP) $(OUT) $(IN) -lm -pthread
	#$(CC) $(COMP) $(OUT1) $(IN1) -lcunit -pthread
	
dy_ngrams.o: dy_ngrams.c
	$(CC) $(FLAGS) dy_ngrams.c
	
bloom_functions.o: bloom_functions.c
	$(CC) $(FLAGS) bloom_functions.c	
	
st_ngrams.o: st_ngrams.c
	$(CC) $(FLAGS) st_ngrams.c
	
#unit_testing.o:	unit_testing.c
	#$(CC) $(FLAGS) unit_testing.c 
	
Job_Scheduler.o: Job_Scheduler.c
	$(CC) $(FLAGS) Job_Scheduler.c 
main.o: main.c
	$(CC) $(FLAGS) main.c
# clean up
clean:
	rm -f $(OUT) $(OUT1) $(IN) $(IN1) $(files)

