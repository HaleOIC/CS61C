src = $(wildcard ./*.c)
tar = $(patsubst ./%.c, ./%, $(src))

myArgs = -Wall -g


ALL:$(tar)

$(tar):./%:./%.c
	gcc $< -o $@  $(myArgs)

clean:
	-rm -rf $(tar)


.PHONY:clean 
