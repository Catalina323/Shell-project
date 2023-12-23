#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>

//extern int errno;//poate mai tarziu ca sa lansam erori

#define GIVECHAR 1
#define NOGIVECHAR 0

struct arg{
	char* func;
	char ** argv;
    int nflags;
};

struct ist{
    struct arg* _arg;
    struct ist* next;
    struct ist* prev;
}*first, *last;


char* printeaza_arg(struct arg* v, bool retval)//daca retval e 1 -> imi returneaza si stringul, daca nu, nu il returneaza
{
    printf("%s ", v->func);
    char* command;
    if(retval == 1)
    {
        command = malloc(sizeof(char) * 100);//mai este nevoie de un free in afara functiei pentru asta
        strncat(command, v->func, 20);
        strcat(command, " ");
    }
    
    for(int i = 0; i < v->nflags; i++)
    {
        printf("%s ", v->argv[i]);
        if(retval == 1)
        {
            strncat(command, v->argv[i], 10);
            strcat(command, " ");
        }
    }
    if(retval == 1)
        return command;
    return NULL;
}//printeaza comanda dar o si returneaza sub forma de string pentru a o putea rula din nou din istoric

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

int arata_istoric()
{
    struct ist* current = first;
    while(current->next != NULL)
    {
        printeaza_arg(current->_arg, NOGIVECHAR);
        current = current->next;
    }
    printf("\n");
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
    }
    else
    {
        //nu ne uitam in /usr/bin -> ne uitam in functiile locale

        if(strcmp(v->func, "istoric") == 0)
        {
            arata_istoric();
            return 0;
        }
        else if(strcmp(v->func, "exit") == 0) // NU MERGE BINE LOGICA DE EXIT
        {
            return -1;
        }
        
        printf("Trebuie rulata o alta functie\n"); // -> implementam noi functionalitati
    }


    //eliberare memorie
    free(command);
    for(int i = 0; i <= v->nflags; i++)
    {
        free(argv[i]);
    }
    free(argv);

    return 0;
}



int adauga_istoric(struct arg* v)
{
    struct ist* new_ist = malloc(sizeof(struct ist));
    new_ist->_arg = v;
    if(first == NULL && last == NULL)
    {
        first = new_ist;
        last = new_ist;
        return 0;
    }
    last->next = new_ist;
    new_ist->prev = last;
    last = new_ist;
    return 0;
}

int sterge_istoric()
{
    struct ist* current = first;
    while(current->next != NULL)
    {
        struct ist* aux = current;
        current = current->next;
        free(aux->_arg);
        free(aux);
    }
    free(current->_arg);
    free(current);
}

int main()
{

    bool run = true;
	while(run)
	{		

        struct arg * v = malloc(sizeof(struct arg));
        printf("#cel_mai_pacanea_shell# ");	
		v = prelucrare_input();
        adauga_istoric(v);
        pid_t pid = fork();
        if(pid < 0)
            return errno;
        else if (pid == 0)
        {
            if(apelare_functie(v) == -1)
                run = false;
            return 0;
        }
        else{
            wait(NULL);        
        }
	}
    //free(v);
    sterge_istoric();
	return 0;
}

//echo "Salut" | fis.txt > ./a.exe > rezultat.txt
//((echo "Salut" | fis.txt) > ./a.exe ) > rezultat.txt)
//a > ./a.exe
//>Salut
