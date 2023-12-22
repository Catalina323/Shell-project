#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//extern int errno;//poate mai tarziu ca sa lansam erori

struct arg{
	char* func;
	char ** argv;
    int nflags;
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
    a->nflags = 0;
	
	//citirea argumentelor:
	
	getline(&buf, &n, stdin);
	
	char * input = malloc(sizeof(char) * strlen(buf));
	strcpy(input, buf);
	
	//prelucrarea argumentelor
	
	token = strtok(input, delim);
	a->func = token;
	
	while(token != NULL)
	{ 
		token = strtok(NULL, delim);
		a->argv[a->nflags++] = token;
		//a->nflags++;
	}
    a->nflags--;
	free(buf);
	a->func = strtok(a->func, "\n");
	return a;
}

int apelare_functie(struct arg* v)
{
    //return -1 daca ceva nu merge bine
    //presupunem initial ca functia este o functie din /usr/bin/ definita deja de sistem
    char* command = malloc(sizeof(char)*100);
    char** argv = malloc(sizeof(char*)*25);
    char* base_path = "/usr/bin";
    strncpy(command, base_path, 8);//copiez path_ul de baza la comanda
    strcat(command, "/");
    strncat(command, v->func, 20);//copiez numele functiei la comanda
    //copiez flagurile in sirul de flaguri

    argv[0] = malloc(sizeof(char)*100);//prin conventie trebuie notat numele programului care ruleaza
    strcpy(argv[0], "shell.c");
    for(int i = 0; i < v->nflags; i++)
    {
        argv[i + 1] = malloc(sizeof(char)*100);
        v->argv[i][strcspn(v->argv[i], "\n")] = 0; // elimina probleme de genul da$'\n'.txt la creare de fisier
        strncpy(argv[i + 1], v->argv[i], 20); 
        if(i < v->nflags - 1)
            strcat(argv[i + 1], " ");     
    }
    argv[v->nflags + 1] = NULL;

    if(!execve(command, argv, NULL))
    {
        printf("A fost rulata o functie din linux\n"); // aici este ok, nu mai trebuie schimbat nimic
        return 0;
    }
    else
    {
        printf("Trebuie rulata o alta functie\n"); // -> punem alt path si alte argumente
    }



    return 0;
}

int main()
{
	struct arg * v = malloc(sizeof(char)*100);	
	while(1)
	{	
        printf("#cel_mai_pacanea_shell# ");	
		v = prelucrare_input();
        pid_t pid = fork();
        if(pid == 0)
            apelare_functie(v);
        wait(NULL); // este necesar sa asteptam altfel are comportament ciudat, precum lag in nano
	}
    //free(v);

	return 0;
}

//echo "Salut" | fis.txt > ./a.exe > rezultat.txt
//((echo "Salut" | fis.txt) > ./a.exe ) > rezultat.txt)
//a > ./a.exe
//>Salut
