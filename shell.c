#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

typedef struct arg{
	
	char* func;
	char ** argv;
};


struct arg* prelucrare_input()
{
	size_t n = 0;
	char * buf = NULL;
	char * token;
	char * delim = " ";
	int i = 0;
	struct arg * a = malloc(sizeof(struct arg));
	a->argv = malloc(sizeof(char)*100);
	
	//citirea argumentelor:
	
	getline(&buf, &n, stdin);
	
	char * input = malloc(sizeof(char) * strlen(buf));
	strcpy(input, buf);
	
	//prelucrarea argumentelor
	
	token = strtok(input, delim);
	a->func = token;
	
	while(token)
	{ 
		token = strtok(NULL, delim);
		a->argv[i] = strtok(token, "\n");	
		i++;
	}
	
	free(buf);
	a->func = strtok(a->func, "\n");
	return a;

}

int main()
{
	//while(1)
	//{
	//	printf("#cel_mai_pacanea_shell# ");
		
		struct arg * v = malloc(sizeof(char)*100);	
		v = prelucrare_input();
		
		printf("functia este: %s \n",v->func);
		printf("%s \n",v->argv[0]);
	

	//}

	return 0;
}

//echo "Salut" | fis.txt > ./a.exe > rezultat.txt
//((echo "Salut" | fis.txt) > ./a.exe ) > rezultat.txt)
//a > ./a.exe
//>Salut
