#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <errno.h>
#include <pwd.h>
#include <fcntl.h>

#define BUF_SZ 256
#define TRUE 1
#define FALSE 0
#define SIZE 101
#define MAX_PROCESOS 10
#define MAX_HISTORY 10
#define HISTORY_FILE ".hist"
#define SIZE_HELP 18

struct passwd *pw;

char *helps   [SIZE_HELP] = {"my_shell",
                             "cd",
                             "exit",
                             "true",
                             "false",
                             "background",
                             "fg",
                             "jobs",
                             "history",
                             "again",
                             "ctrl+c",
                             "chain",
                             "conditional",
                             "pipes",
                             "redir",
                             "set",
                             "get",
                             "unset",
};
char *commands_help[SIZE_HELP] = {"Primer proyecto de Sistemas Operativos\n\n Integrantes: \nEnzo Rojas D'Toste\nDaniel Abad Fundora\n\n Grupo: C-311\n\n Funcionalidades implementadas:\n\n cd: Se utiliza para cambiar el directorio actual\nexit: Se utiliza para cerrar el shell \n true:Comando que siempre tiene exito\n false: Comando que nunca tiene exito\nbackground: Permite enviar procesos al background\nfg: Permite enviar procesos del background al foreground\njobs: Lista los procesos que ocurren en el background\nhistory: Lista los ultimos 10 comandos ejecutados\n again: Permite volver a ejecutar comandos del history\ncntrl+c:Permite terminar el proceso actual\n chain: Permite crear instrucciones de varios comandos \n conditional: Permite simular instrucciones condicionales en una linea\npipes: Permite redirigir la salida de un comando a la entrada de otro\nredir: Permite redirigir la salida de un comando a un archivo o usar el texto de un archivo como entrada de un comando\nset permite asignarle valores a variables\nget: Permite ver el valor de una variable\n unset: Permite eliminar variables\n\n Para mas informacion acerca de un comando en especifico escribir help <nombre del comando>\n Nota estimada: 10 puntos\n"
                                   ,
"El comando cd permite cambiar de directorio actual\ncd<dir> cambia el directorio actual por el directorio especificado\ncd folder se mueve hacia la carpeta folder dentro del directorio actual\ncd sin parametros te redirecciona a Home\n\n Implementacion:\n Para la implemantacion de esta funcionalidad llamamos a la funcion chdir para cambiar el directorio actual.\nEn caso de no tener argumentos cambiamos la ruta a home/user\n Si el argumento escrito por elusuario esta entre parentesis, seran removidos(Con el objetivo de poder trabajar con carpetas con espacios en el nombre\n\n Implementado en la linea 838\n",
"El comando exit finaliza la ejecucion del shell, utiliza para ello la funcion kill \nSe encuentra implemantado en la linea 627\n",
"El comando true simula un comando que siempre tiene exito\n",
"El comando false simula un comando que siempre fracasa\n",
"Al utilizar el operador & luego de un comando hace que este se ejecute en el background.\nLa implementacion de esto se basa en ejecutar el comando y no utilizar la funcion wait.\n Utilizamos la funcion sendToBackground implementado en la linea 969\n",
"El operador fg <pid> trae el proceso con pid especificado del background hacia el foreground.\n De no tener argumentos, trae el ultimo proceso enviado al background.\n Su implementacion se basa en hacerle wait() al comando requerido. Esta implementado en la linea 1017, en el metodo CallFg\n",
"El comando jobs lista todos los procesos que estan ocurriendo en el background con sus respectivos pids y nombres\nPara ello se reocorre el array de nombres de procesos y de pids y se imprimen\nImplementado en la linea 1059\n",
"El comando history muestra los ultimos 10 comandos ejecutados\nPara ello se lee el archivo .hist donde se guardan los ultimos 10 comandos ejecutados\nImplementado en la linea 1180\n",
"El comando again n hace referencia al n-esimo comando de history.\nPara su implemantacion se sustituye cada aparicion del comando por el comando correspondiente en el history. Luego la linea se ejecuta normalmente\nImplementado en la linea 1287\n",
"El comando ctrl+c permite finalizar la ejecucion de un proceso en foreground\nPara ello se utiliza la funcion kill\nImplementado en la linea 1035\n",
"Los operadores( ; , || , && ) son utilizados para concatenar comandos.\n\nEl operador ; permite ejecutar varios comandos en la misma línea.\n\nEl comando command1 && command2 ejecuta command1 y luego,si este tiene éxito, ejecuta el comando command2\n\nEl comando command1 || command2 ejecuta command1 y si este no tiene éxito ejecuta command2.\n\nAdicionalmente se pueden combinar todos estos comandos en una sola línea.\n\n Los 3 operadores estan implementados dentro del metodo execute (linea 288)\n ",
"Los comandos if, then, else y end nos permiten realizar una operación condicional en una sola línea.\n\nEl comando if <command1> then <command2> else <command3> end, ejecuta el comando <command1>, de tener exito ejecuta <command2> en caso contrario se ejecuta <command3>. \n\nEl operador end se usa para indicar en fin de la operación condicional. Es posible prescindir del comando else en la operacion condicional.\n\n Es posible anidar varias operaciones condicionales\n\n Se encuentra implemantado en el metodo CallIf (linea 1072).\n",
"Al utilizar el operador command1|commad2 redirecciona la salida de command1 a la entrada de command2.\n La implementacion del mismo se basa en crear un nuevo proceso hijo\nSe utiliza el metodo CallCommandWithPipe (linea 667)\n",
"Los operadores < > y >> permiten redireccionar las salidas de nuestros comandos\n El comando command1 < file envia el contenido del archivo file a la entrada de command1\n El comando command1>file sobreescribe el contenido del archivo file con la salida de command1\n El operador >> tiene un funcionamiento similar a > pero en lugar de sobreescribir el archivo, lo escribe al final del mismo.\n\n(Implementado linea 728)\n",
"El comando set <var> <val> le asigna el valor val a la variable del sistema var, de no estar inicializada la variable, la inicializa.\n Como value puede ser colocado un comando entre comillas '' y se ejecutara el comando y su salida se guardara en la variable\nPara la implementacion hemos creado un diccionario en el cual usamos como key el nombre de la variable y su valor, como value\n\n Esta funcionalidad esta implementada en el metodo CallSet (linea 879) \n",
"El comando get var muestra en la consola el valor que tiene asignada la variable a.\n Para su implementacion, revisamos si var se encuentra entre los keys del diccionario creado para la funcion set y, de existir, imprime el value asignado.\n Se utiliza la funcion CallGet (linea 933)\n",
"El comando unset <var> elimina la variable var del sistema\n Para ello revisa si var se encuentra entre las keys del diccionario y de ser asi la elimina junto a su value correspondiente\n Implementado en el metodo GetUnSet(Linea 965)\n"
};



const char* COMMAND_HISTORY = "history";
const char* COMMAND_EXIT = "exit";
const char* COMMAND_HELP = "help";
const char* COMMAND_TRUE = "true";
const char* COMMAND_FALSE = "false";
const char* COMMAND_CD = "cd";
const char* COMMAND_SET = "set";
const char* COMMAND_GET = "get";
const char* COMMAND_UNSET = "unset";
const char* COMMAND_IN = "<";
const char* COMMAND_OUT = ">";
const char* COMMAND_OUTOUT = ">>";
const char* COMMAND_PIPE = "|";
const char* COMMAND_BACKGROUND = "&";
const char* COMMAND_FG = "fg";
const char* COMMAND_JOBS = "jobs";
const char* COMMAND_IF = "if";
const char* COMMAND_AGAIN = "again";
FILE *file;

enum {
	RESULT_NORMAL,
	ERROR_FORK,
	ERROR_COMMAND,
	ERROR_WRONG_PARAMETER,
	ERROR_WRONG_VARIABLE,
	ERROR_MISS_PARAMETER,
	ERROR_TOO_MANY_PARAMETER,
	ERROR_CD,
	ERROR_SYSTEM,
	ERROR_EXIT,
    ERROR_FALSE,
	ERROR_IF,
	ERROR_AGAIN,
	ERROR_BACKGROUND,

	ERROR_MANY_IN,
	ERROR_MANY_OUT,
	ERROR_MANY_OUTOUT,
	ERROR_FILE_NOT_EXIST,
	
	ERROR_PIPE,
	ERROR_PIPE_MISS_PARAMETER,

	ERROR_FULL_STACK,
	ERROR_PID
};

char username[BUF_SZ];
char hostname[BUF_SZ];
char curPath[BUF_SZ];

int pids [MAX_PROCESOS];
char comandos [MAX_PROCESOS][BUF_SZ];
int n_pids = 0;

int current_pid;
int last_pid;

int print_history();
void save_history(char command[BUF_SZ]);
void load_history();
int isCommandExist(const char* command);
void getUsername();
void getHostname();
int getCurWorkDir();
int splitCommandsSC(char command[BUF_SZ], char commands[BUF_SZ][BUF_SZ]);
int splitCommandsSpace(char command[BUF_SZ], char commands[BUF_SZ][BUF_SZ]);
int execute(int left, int right, char commands[BUF_SZ][BUF_SZ], int recursive);
int splitAndexecute(char command[BUF_SZ]);
int callExit();
int callCommand(int left, int right, char commands[BUF_SZ][BUF_SZ], int recursive);
int callCommandWithPipe(int left, int right, char commands[BUF_SZ][BUF_SZ], int recursive);
int callCommandWithRedi(int left, int right, char commands[BUF_SZ][BUF_SZ], int recursive);
int callCd(int left, int right, char commands[BUF_SZ][BUF_SZ]);
int callSet(int left, int right, char commands[BUF_SZ][BUF_SZ]);
int callGet(int left, int right, char commands[BUF_SZ][BUF_SZ]);
int callUnSet(int left, int right, char commands[BUF_SZ][BUF_SZ]);
int sendToBackground(int left, int right, char commands[BUF_SZ][BUF_SZ]);
int callFg(int left, int right, char commands[BUF_SZ][BUF_SZ]);
int callJobs();
int callIf(int left, int right, char commands[BUF_SZ][BUF_SZ]);
int history_len;
char *history[MAX_HISTORY];
int callHelp(int left, int right, char commands[BUF_SZ][BUF_SZ]);
void replace_again(char command[BUF_SZ]);
struct elemento {
  char* clave;
  char* valor;
  struct elemento* siguiente;
};

static struct elemento* tabla[SIZE];

unsigned hash(char* clave) {
  unsigned hashval;
  for (hashval = 0; *clave != '\0'; clave++)
    hashval = *clave + 31 * hashval;
  return hashval % SIZE;
}

struct elemento* buscar(char* clave) {
  struct elemento* actual;
  for (actual = tabla[hash(clave)]; actual != NULL; actual = actual->siguiente)
    if (strcmp(clave, actual->clave) == 0)
      return actual;
  return NULL;
}

struct elemento* insertar(char* clave, char* valor) {
  struct elemento* actual;
  unsigned hashval;
  if ((actual = buscar(clave)) == NULL) {
    actual = (struct elemento*) malloc(sizeof(*actual));
    if (actual == NULL || (actual->clave = strdup(clave)) == NULL)
      return NULL;
    hashval = hash(clave);
    actual->siguiente = tabla[hashval];
    tabla[hashval] = actual;
  } else
    free((void*) actual->valor);
  if ((actual->valor = strdup(valor)) == NULL)
    return NULL;
  return actual;
}

void eliminar(char* clave) {
  struct elemento* actual;
  struct elemento* anterior;
  unsigned hashval = hash(clave);
  actual = tabla[hashval];
  anterior = NULL;
  while (actual != NULL) {
    if (strcmp(clave, actual->clave) == 0) {
      if (anterior == NULL) {
        tabla[hashval] = actual->siguiente;
      } else {
        anterior->siguiente = actual->siguiente;
      }
      free(actual->clave);
      free(actual->valor);
      free(actual);
      return;
    }
    anterior = actual;
    actual = actual->siguiente;
  }
}

void handler_background() {
	int status;
	
	for (int i = 0; i < n_pids; i++) {
	
		int pid_t = waitpid(pids[i], &status, WNOHANG);

		if(pid_t > 0)
		{
			if(status == EXIT_SUCCESS)
            	printf("\nDone pid %d %s\n",pids[i], comandos[i]);
			else
				printf("\nFail pid %d %s\n",pids[i], comandos[i]);
			for (int j = i; j < n_pids - 1; ++j) {
				pids [j] = pids [j + 1];
				strcpy (comandos [j], comandos [j + 1]);
			}

			i--;
			n_pids--;
		}
		
 	}

}

void sigint_handler (int sig) {
	if (current_pid == -1) {
        printf("\r");
        return;
    }

    if (current_pid == last_pid) {
        kill(current_pid, SIGINT);
    } else {
        kill(current_pid, SIGKILL);
    }

    last_pid = current_pid;

    printf("\r");
}

int main() {
    uid_t uid;

    uid = getuid();
    pw = getpwuid(uid);

	int result = getCurWorkDir();
	if (ERROR_SYSTEM == result) {
		fprintf(stderr, "\e[31;1mError: System error while getting current work directory.\n\e[0m");
		exit(ERROR_SYSTEM);
	}
	getUsername();
	getHostname();

	signal(SIGINT, sigint_handler);
	load_history();

	char argv[BUF_SZ];
	while (TRUE) {
		handler_background();

		current_pid = -1;
        last_pid = -1;

		printf("\e[32;1m%s@%s:%s\e[0m$ ", username, hostname, curPath);
		
		fgets(argv, BUF_SZ, stdin);
        
		int len = strlen(argv);
		if (len != BUF_SZ) {
			argv[len-1] = '\0';
		}
        replace_again(argv);
		if(strcmp(argv, "") != 0) {
			save_history(argv);
			load_history();
			splitAndexecute(argv);
		}
	}
}

int execute(int left, int right, char commands[BUF_SZ][BUF_SZ], int recursive) {
	int result;
	if (strcmp(commands[left], COMMAND_EXIT) == 0) {
		result = callExit();
		if (ERROR_EXIT == result) {
			exit(-1);
		}
	}
    int operadorlogico = -1;

	for(int i = left; i < right; i++)
	{
		if(strcmp(commands[i], "&&") == 0 || strcmp(commands[i], "||") == 0) {
			operadorlogico = i;
			break;
		}
	}

	if(operadorlogico > -1)
	{
		result = execute(left, operadorlogico, commands, FALSE);

		if(result == RESULT_NORMAL && commands[operadorlogico][0] == '&')
			result = execute(operadorlogico + 1, right, commands, FALSE);

		else if(result != RESULT_NORMAL && commands[operadorlogico][0] == '|')
			result = execute(operadorlogico + 1, right, commands, FALSE);
		
		return result;
	}

	if(strcmp(commands[right - 1], COMMAND_BACKGROUND) == 0)
		return sendToBackground(left, right - 1, commands); 

    int tubOrRedi = FALSE;

	for(int i = left; i < right; i++)
	{
		if(strcmp(commands[i], "|") == 0 || strcmp(commands[i], "<") == 0 || strcmp(commands[i], ">") == 0 || strcmp(commands[i], ">>") == 0) {
			tubOrRedi = TRUE;
			break;
		}
	}

	if(tubOrRedi == TRUE) {
		result = callCommand(left, right, commands, FALSE);
		switch (result) {
			case ERROR_FORK:
				fprintf(stderr, "\e[31;1mError: Fork error.\n\e[0m");
				exit(ERROR_FORK);
			case ERROR_COMMAND:
				fprintf(stderr, "\e[31;1mError: Command not exist in myshell.\n\e[0m");
				break;
			case ERROR_MANY_IN:
				fprintf(stderr, "\e[31;1mError: Too many redirection symbol \"%s\".\n\e[0m", COMMAND_IN);
				break;
			case ERROR_MANY_OUT:
				fprintf(stderr, "\e[31;1mError: Too many redirection symbol \"%s\".\n\e[0m", COMMAND_OUT);
				break;
			case ERROR_MANY_OUTOUT:
				fprintf(stderr, "\e[31;1mError: Too many redirection symbol \"%s\".\n\e[0m", COMMAND_OUTOUT);
				break;
			case ERROR_FILE_NOT_EXIST:
				fprintf(stderr, "\e[31;1mError: Input redirection file not exist.\n\e[0m");
				break;
			case ERROR_MISS_PARAMETER:
				fprintf(stderr, "\e[31;1mError: Miss redirect file parameters.\n\e[0m");
				break;
			case ERROR_PIPE:
				fprintf(stderr, "\e[31;1mError: Open pipe error.\n\e[0m");
				break;
			case ERROR_PIPE_MISS_PARAMETER:
				fprintf(stderr, "\e[31;1mError: Miss pipe parameters.\n\e[0m");
				break;
		}
		return result;
	} 
	
	int len = strlen(commands[left]);
			
	if(commands[left][0] == '(' && commands[left][len - 1] == ')') {
		char* nueva_cadena = (char*)malloc(sizeof(char) * (len - 1));
		strncpy(nueva_cadena, commands[left] + 1, len - 2);
		nueva_cadena[len - 2] = '\0';
		
		result = splitAndexecute(nueva_cadena);
		free(nueva_cadena);
		return result;
	}

	if (strcmp(commands[left], COMMAND_TRUE) == 0) {
		result = RESULT_NORMAL;
	} else if (strcmp(commands[left], COMMAND_FALSE) == 0) {
		result = ERROR_FALSE;
	} else if (strcmp(commands[left], COMMAND_CD) == 0) {
		result = callCd(left, right, commands);
		switch (result) {
			case ERROR_MISS_PARAMETER:
				fprintf(stderr, "\e[31;1mError: Miss parameter while using command \"%s\".\n\e[0m"
					, COMMAND_CD);
				break;
			case ERROR_WRONG_PARAMETER:
				fprintf(stderr, "\e[31;1mError: No such path \"%s\".\n\e[0m", commands[left + 1]);
				break;
			case ERROR_TOO_MANY_PARAMETER:
				fprintf(stderr, "\e[31;1mError: Too many parameters while using command \"%s\".\n\e[0m"
					, COMMAND_CD);
				break;
			case RESULT_NORMAL:
				result = getCurWorkDir();
				if (ERROR_SYSTEM == result) {
					fprintf(stderr
						, "\e[31;1mError: System error while getting current work directory.\n\e[0m");
					exit(ERROR_SYSTEM);
				} else {
					break;
				}
		}
	} else if(strcmp(commands[left], COMMAND_SET) == 0 ) {
		result = callSet(left, right, commands);

		switch (result) {
			case ERROR_WRONG_PARAMETER:
				fprintf(stderr, "\e[31;1mError \"%s\".\n\e[0m", commands[left + 1]);
				break;
		}

	} else if(strcmp(commands[left], COMMAND_GET) == 0 ) {
		result = callGet(left, right, commands);

		switch (result) {
			case ERROR_WRONG_VARIABLE:
				fprintf(stderr, "\e[31;1mError Variable don't exist \"%s\".\n\e[0m", commands[left + 1]);
				break;
		}

	} else if(strcmp(commands[left], COMMAND_UNSET) == 0 ) {
		result = callUnSet(left, right, commands);

	} else if(strcmp(commands[left], COMMAND_HELP) == 0 ) {
        result = callHelp(left, right, commands);

    } else if(strcmp(commands[left], COMMAND_FG) == 0 ) {
		result = callFg(left, right, commands);

		switch (result) {
			case ERROR_PID:
				fprintf(stderr, "\e[31;1mError: PID don´t exist.\n\e[0m");
				break;
		}

	} else if(strcmp(commands[left], COMMAND_HISTORY) == 0 ) {
        result = print_history();

    }else if(strcmp(commands[left], COMMAND_JOBS) == 0 ) {
		result = callJobs();

	} else if(strcmp(commands[left], COMMAND_IF) == 0 ) {
		result = callIf(left, right, commands);
	} else if(strcmp(commands[left], COMMAND_AGAIN) == 0 ) {
		result = ERROR_AGAIN;
		fprintf(stderr, "\e[31;1mError: You must specify a correct history index\n\e[0m");
	}
	 else {
		result = callCommand(left, right, commands, recursive);
		switch (result) {
			case ERROR_FORK:
				fprintf(stderr, "\e[31;1mError: Fork error.\n\e[0m");
				exit(ERROR_FORK);
			case ERROR_COMMAND:
				fprintf(stderr, "\e[31;1mError: Command %s not exist in myshell.\n\e[0m", commands[left]);
				break;
			case ERROR_MANY_IN:
				fprintf(stderr, "\e[31;1mError: Too many redirection symbol \"%s\".\n\e[0m", COMMAND_IN);
				break;
			case ERROR_MANY_OUT:
				fprintf(stderr, "\e[31;1mError: Too many redirection symbol \"%s\".\n\e[0m", COMMAND_OUT);
				break;
			case ERROR_MANY_OUTOUT:
				fprintf(stderr, "\e[31;1mError: Too many redirection symbol \"%s\".\n\e[0m", COMMAND_OUTOUT);
				break;
			case ERROR_FILE_NOT_EXIST:
				fprintf(stderr, "\e[31;1mError: Input redirection file not exist.\n\e[0m");
				break;
			case ERROR_MISS_PARAMETER:
				fprintf(stderr, "\e[31;1mError: Miss redirect file parameters.\n\e[0m");
				break;
			case ERROR_PIPE:
				fprintf(stderr, "\e[31;1mError: Open pipe error.\n\e[0m");
				break;
			case ERROR_PIPE_MISS_PARAMETER:
				fprintf(stderr, "\e[31;1mError: Miss pipe parameters.\n\e[0m");
				break;
		}
	}

	return result;
}

int splitAndexecute(char command[BUF_SZ]) {
	int result = RESULT_NORMAL;

	if(strcmp(command, "") != 0) {
		char commandsSC[BUF_SZ][BUF_SZ];
		int commandNumSC = splitCommandsSC(command, commandsSC);

		for(int i = 0; i < commandNumSC; i++)
		{
			char commands[BUF_SZ][BUF_SZ];
			int commandNum = splitCommandsSpace(commandsSC[i], commands);
			result = execute(0, commandNum, commands, FALSE);
		}
	}
	return result;
}

int isCommandExist(const char* command) {
	if (command == NULL || strlen(command) == 0) return FALSE;
    
	int result = TRUE;
	
	int fds[2];
	if (pipe(fds) == -1) {
		result = FALSE;
	} else {
		
		int inFd = dup(STDIN_FILENO);
		int outFd = dup(STDOUT_FILENO);

		pid_t pid = vfork();
		if (pid == -1) {
			result = FALSE;
		} else if (pid == 0) {
			
			close(fds[0]);
			dup2(fds[1], STDOUT_FILENO);
			close(fds[1]);

			char tmp[BUF_SZ];
			sprintf(tmp, "command -v %s", command);
			printf("%s", tmp);
			exit(1);
		} else {
			waitpid(pid, NULL, 0);
			
			close(fds[1]);
			dup2(fds[0], STDIN_FILENO);
			close(fds[0]);

			if (getchar() == EOF) {
				result = FALSE;
			}
			
			dup2(inFd, STDIN_FILENO);
			dup2(outFd, STDOUT_FILENO);
		}
	}

	return result;
}

void getUsername() {
	struct passwd* pwd = getpwuid(getuid());
	strcpy(username, pwd->pw_name);
}

void getHostname() {
	gethostname(hostname, BUF_SZ);
}

int getCurWorkDir() {
	char* result = getcwd(curPath, BUF_SZ);
	if (result == NULL)
		return ERROR_SYSTEM;
	else return RESULT_NORMAL;
}

int splitCommandsSC(char command[BUF_SZ], char commands[BUF_SZ][BUF_SZ]) {
	int num = 0;
	int i, j;
	int len = strlen(command);
	int count = 0;
	int count_if = 0;

	for (i=0, j=0; i<len; ++i) {
		if (command[i] == '#')
			break;
		if(command[i] == '(')
			count++;
		if(command[i] == ')')
			count--;
		if(i > 2 && command[i - 3] == 'i' && command[i - 2] == 'f')
			count_if++;
		if(i > 1 && command[i - 2] == 'e' && command[i - 1] == 'n' && command[i] == 'd')
			count_if--;
		if (count_if > 0 || count > 0 || command[i] != ';') {
			commands[num][j++] = command[i];
		} else {
			if (j != 0) {
				commands[num][j] = '\0';
				++num;
				j = 0;
			}
		}
	}
	if (j != 0) {
		commands[num][j] = '\0';
		++num;
	}

	return num;
}

int splitCommandsSpace(char command[BUF_SZ], char commands[BUF_SZ][BUF_SZ]) {
	int num = 0;
	int i, j;
	int len = strlen(command);
    int count = 0;
	int count_if = 0;

	for (i=0, j=0; i<len; ++i) {
		if(command[i] == '(')
			count++;
		if(command[i] == ')')
			count--;
		if(i > 2 && command[i - 3] == 'i' && command[i - 2] == 'f')
			count_if++;
		if(i > 1 && command[i - 2] == 'e' && command[i - 1] == 'n' && command[i] == 'd')
			count_if--;
		if(count_if > 0 || count > 0 || command[i] != ' ') {
			commands[num][j++] = command[i];
		} else {
			if (j != 0) {
				commands[num][j] = '\0';
				++num;
				j = 0;
			}
		}
	}
	if (j != 0) {
		commands[num][j] = '\0';
		++num;
	}

	return num;
}

int callExit() {
	pid_t pid = getpid();
	if (kill(pid, SIGTERM) == -1) 
		return ERROR_EXIT;
	else return RESULT_NORMAL;
}

int callCommand(int left, int right, char commands[BUF_SZ][BUF_SZ], int recursive) {
	pid_t pid = fork();
	if (pid == -1) {
		return ERROR_FORK;
	} else if (pid == 0) {
		int inFds = dup(STDIN_FILENO);
		int outFds = dup(STDOUT_FILENO);

		int result = callCommandWithPipe(left, right, commands, recursive);
		
		dup2(inFds, STDIN_FILENO);
		dup2(outFds, STDOUT_FILENO);
		exit(result);
	} else {
		int status;
		waitpid(pid, &status, 0);
		return WEXITSTATUS(status);
	}
}

int callCommandWithPipe(int left, int right, char commands[BUF_SZ][BUF_SZ], int recursive) {
	if (left >= right) return RESULT_NORMAL;

	int pipeIdx = -1;
	for (int i=left; i<right; ++i) {
		if (strcmp(commands[i], COMMAND_PIPE) == 0) {
			pipeIdx = i;
			break;
		}
	}
	if (pipeIdx == -1) {
		return callCommandWithRedi(left, right, commands, recursive);
	} else if (pipeIdx+1 == right) {
		return ERROR_PIPE_MISS_PARAMETER;
	}

	int fds[2];
	if (pipe(fds) == -1) {
		return ERROR_PIPE;
	}
	int result = RESULT_NORMAL;
	pid_t pid = vfork();
	if (pid == -1) {
		result = ERROR_FORK;
	} else if (pid == 0) {
		close(fds[0]);
		dup2(fds[1], STDOUT_FILENO);
		close(fds[1]);
		
		result = callCommandWithRedi(left, pipeIdx, commands, recursive);
		exit(result);
	} else {
		int status;
		waitpid(pid, &status, 0);
		int exitCode = WEXITSTATUS(status);
		
		if (exitCode != RESULT_NORMAL) {
			char info[4096] = {0};
			char line[BUF_SZ];
			close(fds[1]);
			dup2(fds[0], STDIN_FILENO);
			close(fds[0]);
			while(fgets(line, BUF_SZ, stdin) != NULL) {
				strcat(info, line);
			}
			printf("%s", info);
			
			result = exitCode;
		} else if (pipeIdx+1 < right){
			close(fds[1]);
			dup2(fds[0], STDIN_FILENO);
			close(fds[0]);
			result = callCommandWithPipe(pipeIdx+1, right, commands, recursive);
		}
	}

	return result;
}

int callCommandWithRedi(int left, int right, char commands[BUF_SZ][BUF_SZ], int recursive) {
	if (!isCommandExist(commands[left])) {
		return ERROR_COMMAND;
	}	

	int inNum = 0, outNum = 0, outoutNum = 0;
	char *inFile = NULL, *outFile = NULL, *outoutFile = NULL;
	int endIdx = right;

	for (int i=left; i<right; ++i) {
		if (strcmp(commands[i], COMMAND_IN) == 0) {
			++inNum;
			if (i+1 < right) {
				int len = strlen(commands[i + 1]);
				if(commands[i + 1][0] == '(' && commands[i + 1][len - 1] == ')') {
					for(int j = 0; j < len - 2; j++)
						commands[i + 1][j] = commands[i + 1][j + 1];
					commands[i + 1][len - 2] = '\0';
				}
				inFile = commands[i+1];
			}
			else return ERROR_MISS_PARAMETER;

			if (endIdx == right) endIdx = i;
		} else if (strcmp(commands[i], COMMAND_OUT) == 0) {
			++outNum;
			if (i+1 < right) {
				int len = strlen(commands[i + 1]);
				if(commands[i + 1][0] == '(' && commands[i + 1][len - 1] == ')') {
					for(int j = 0; j < len - 2; j++)
						commands[i + 1][j] = commands[i + 1][j + 1];
					commands[i + 1][len - 2] = '\0';
				}
				outFile = commands[i+1];
			}
			else return ERROR_MISS_PARAMETER;
				
			if (endIdx == right) endIdx = i;
		} else if (strcmp(commands[i], COMMAND_OUTOUT) == 0) {
			++outoutNum;
			if (i+1 < right)
			{
				int len = strlen(commands[i + 1]);
				if(commands[i + 1][0] == '(' && commands[i + 1][len - 1] == ')') {
					for(int j = 0; j < len - 2; j++)
						commands[i + 1][j] = commands[i + 1][j + 1];
					commands[i + 1][len - 2] = '\0';
				}
				outoutFile = commands[i+1];
			}
			else return ERROR_MISS_PARAMETER;
				
			if (endIdx == right) endIdx = i;
		}
	}
	
	if (inNum == 1) {
		FILE* fp = fopen(inFile, "r");
		if (fp == NULL)
			return ERROR_FILE_NOT_EXIST;
		
		fclose(fp);
	}
	
	if (inNum > 1) {
		return ERROR_MANY_IN;
	} else if (outNum > 1) {
		return ERROR_MANY_OUT;
	} else if (outoutNum > 1) {
		return ERROR_MANY_OUTOUT;
	}

	int result = RESULT_NORMAL;
	pid_t pid = vfork();
	if (pid == -1) {
		result = ERROR_FORK;
	} else if (pid == 0) {
		
		if (inNum == 1)
			freopen(inFile, "r", stdin);
		if (outNum == 1)
			freopen(outFile, "w", stdout);
		if (outoutNum == 1)
			freopen(outoutFile, "a", stdout);

		if(recursive == FALSE) {
			result = execute(left, endIdx, commands, TRUE); 
			if(result == RESULT_NORMAL)
				exit(EXIT_SUCCESS);
			exit(0);
		}
		
		char* comm[BUF_SZ];
		for (int i=left; i<endIdx; ++i)
		{
			int len = strlen(commands[i]);
		
			if(commands[i][0] == '(' && commands[i][len - 1] == ')') {
				for(int j = 0; j < len - 2; j++)
					commands[i][j] = commands[i][j + 1];
				commands[i][len - 2] = '\0';
			}
			
			comm[i] = commands[i];
		}
		comm[endIdx] = NULL;

		execvp(comm[left], comm+left);
		fprintf(stderr, "\e[31;1mError: Command %s not exist in myshell.\n\e[0m", commands[left]);
		exit(0);
		
	} else {
		int status;
		waitpid(pid, &status, 0);
		int err = WEXITSTATUS(status);

		if (err) {
			printf("\e[31;1mError: %s\n\e[0m", strerror(err));
		}
	}


	return result;
}

int callCd(int left, int right, char commands[BUF_SZ][BUF_SZ]) {
	int result = RESULT_NORMAL;

	if (right - left < 2) {
		char ruta[7 + BUF_SZ] = "/home/";
		strcat(ruta, username);
		strcat(ruta, "/");
		int ret = chdir(ruta);
		if (ret) result = ERROR_WRONG_PARAMETER;
	} else if (right - left > 2) {
		result = ERROR_TOO_MANY_PARAMETER;
	} else {
		int len = strlen(commands[left + 1]);
		if(commands[left + 1][0] == '(' && commands[left + 1][len - 1] == ')') {
			for(int j = 0; j < len - 2; j++)
				commands[left + 1][j] = commands[left + 1][j + 1];
			commands[left + 1][len - 2] = '\0';
		}
		int ret = chdir(commands[left + 1]);
		if (ret) result = ERROR_WRONG_PARAMETER;
	}

	return result;
}

int callSet(int left, int right, char commands[BUF_SZ][BUF_SZ]) {
	int result = RESULT_NORMAL;

    if(right - left > 1) {
		int size = 0;
		for(int i = left + 2; i < right; i++)
		{
			size += strlen(commands[i]);
		}
		size += right - left - 2;

		char* newValor = malloc(size);

		if(right - left > 2)
			sprintf(newValor, "%s", commands[left + 2]);
		 else
		 {
			sprintf(newValor, "%s", "");
		// 	free(newValor);
		// 	newValor = malloc(BUF_SZ);
		// 	fgets(newValor, BUF_SZ, stdin);

		// 	int len = strlen(newValor);
		// 	if (len != BUF_SZ) {
		// 		newValor[len-1] = '\0';
		// 	}
		 }

		for(int i = left + 3; i < right; i++)
		{
			sprintf(newValor, "%s %s", newValor, commands[i]);
		}

		struct elemento* e = insertar(commands[left + 1], newValor);
		free(newValor);

		if(e == NULL)
			result = ERROR_WRONG_PARAMETER;
	}

	else {
		struct elemento* actual;
		for (int i = 0; i < SIZE; i++) {
			actual = tabla[i];
			while (actual != NULL) {
				printf("%s=%s\n", actual->clave, actual->valor);
				actual = actual->siguiente;
			}
		}
	}

    return result;
}

int callGet(int left, int right, char commands[BUF_SZ][BUF_SZ]) {
	int result = RESULT_NORMAL;

	if(right - left > 2)
		result = ERROR_TOO_MANY_PARAMETER;
	
	else if(right - left == 2) {
		struct elemento* e = buscar(commands[left + 1]);

		if(e == NULL)
			result = ERROR_WRONG_PARAMETER;
		else {
			int len = strlen(e->valor);
			
			if((char)e->valor[0] == '(' && (char)e->valor[len - 1] == ')') {
				char* nueva_cadena = (char*)malloc(sizeof(char) * (len - 1));
				strncpy(nueva_cadena, e->valor + 1, len - 2);
				nueva_cadena[len - 2] = '\0';
				
				result = splitAndexecute(nueva_cadena);
				free(nueva_cadena);
			}

			else
				printf("%s\n", e->valor);
		}
	}

    return result;
}

int callUnSet(int left, int right, char commands[BUF_SZ][BUF_SZ]) {
	int result = RESULT_NORMAL;

	if(right - left > 2)
		result = ERROR_TOO_MANY_PARAMETER;
	
	else if(right - left == 2)
  		eliminar(commands[left + 1]);

    return result;
}

int sendToBackground(int left, int right, char commands[BUF_SZ][BUF_SZ]) {
  
  int result = RESULT_NORMAL;
  int pid = fork ();
  
  if (pid == 0) {
	setpgid(0, 0);
	result = execute(left, right, commands, FALSE); 
	if(result == RESULT_NORMAL)
		exit(EXIT_SUCCESS);
	exit(EXIT_FAILURE);

  } else if (pid > 0) {
	setpgid(pid, pid);
	if (n_pids < MAX_PROCESOS) {
		pids [n_pids] = pid;

		int size = 0;
		for(int i = left; i < right; i++)
		{
			size += strlen(commands[i]);
		}
		size += right - left;

		char* newValor = malloc(size);

		sprintf(newValor, "%s", commands[left]);

		for(int i = left + 1; i < right; i++)
		{
			sprintf(newValor, "%s %s", newValor, commands[i]);
		}

		strcpy (comandos [n_pids], newValor);
		free(newValor);
		n_pids++;
    } else {
		result = ERROR_FULL_STACK;
		fprintf(stderr, "\e[31;1mError: Too many proccesses at background\n\e[0m");
    }

  } else {
    perror ("fork");
  }
  return result;
}

int callFg(int left, int right, char commands[BUF_SZ][BUF_SZ]) {
	
	int status;
	int r;

	int temp = current_pid;

	if(n_pids == 0)
		return ERROR_PID;

	if(right - left == 1)
		current_pid = pids[n_pids - 1];	
	else
		current_pid = atoi(commands[left + 1]);
	
	tcsetpgrp(0, current_pid);
	setpgid(0, current_pid);
	kill(current_pid, SIGCONT);
	do {
			r = waitpid(current_pid, &status, WUNTRACED);	
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));

	if(r == -1)
	{
		current_pid = temp;
		return ERROR_PID;
	}

	int found = FALSE;

	for (int i = 0; i < n_pids; i++) {
		if(pids[i] == current_pid || found == TRUE)
		{
			found = TRUE;
			if(i + 1 < n_pids)
			{
				pids[i] = pids[i + 1];
				strcpy (comandos[i], comandos[i + 1]);
			}
		}
	}

	if(found == TRUE)
		n_pids--;

	return RESULT_NORMAL;
}

int callJobs() {
	for (int i = 0; i < n_pids; i++) {
		printf ("[%d] %s\n", pids [i], comandos[i]);
	}

	return RESULT_NORMAL;
}

int callIf(int left, int right, char commands[BUF_SZ][BUF_SZ]) {

	if(right - left == 1)
		return ERROR_IF;

	int result = RESULT_NORMAL;

	int then_index = -1;
	int count = 0;

	int len = strlen(commands[left + 1]);

	for(int i = 0; i < len - 3; i++)
	{
		if(commands[left + 1][i] == 'i' && commands[left + 1][i + 1] == 'f')
			count++;
		else if(commands[left + 1][i] == 'e' && commands[left + 1][i + 1] == 'n' && commands[left + 1][i + 2] == 'd')
			count--;
		else if(count == 0 && commands[left + 1][i] == 't' && commands[left + 1][i + 1] == 'h' && commands[left + 1][i + 2] == 'e' && commands[left + 1][i + 3] == 'n')
		{
			then_index = i;
			break;
		}
	}

	if(then_index == -1)
		return ERROR_IF;

	char* nueva_cadena = (char*)malloc(sizeof(char) * (then_index));
	strncpy(nueva_cadena, commands[left + 1], then_index - 1);
	nueva_cadena[then_index - 1] = '\0';
	
	result = splitAndexecute(nueva_cadena);
	free(nueva_cadena);

	if(result == RESULT_NORMAL)
	{
		int endOrelse = -1;
		int count = 0;

		for(int i = then_index + 3; i < len - 2; i++)
		{
			if(commands[left + 1][i] == 'i' && commands[left + 1][i + 1] == 'f')
				count++;
			if(count == 0 && ((i < len - 3 && commands[left + 1][i] == 'e' && commands[left + 1][i + 1] == 'l' && commands[left + 1][i + 2] == 's' && commands[left + 1][i + 3] == 'e') || (commands[left + 1][i] == 'e' && commands[left + 1][i + 1] == 'n' && commands[left + 1][i + 2] == 'd')))
			{
				endOrelse = i;
				break;
			}
			else if(commands[left + 1][i] == 'e' && commands[left + 1][i + 1] == 'n' && commands[left + 1][i + 2] == 'd')
				count--;
		}

		if(endOrelse == -1)
			return ERROR_IF;

		char* new_cadena = (char*)malloc(sizeof(char) * (endOrelse - then_index - 3));
		strncpy(new_cadena, commands[left + 1] + then_index + 4, endOrelse - then_index - 4);
		new_cadena[endOrelse - then_index - 4] = '\0';
		
		result = splitAndexecute(new_cadena);
		free(new_cadena);
	}

	else
	{

		int eelse = -1;
		int count = 0;

		for(int i = then_index + 3; i < len - 3; i++)
		{
			if(commands[left + 1][i] == 'i' && commands[left + 1][i + 1] == 'f')
				count++;
			else if(commands[left + 1][i] == 'e' && commands[left + 1][i + 1] == 'n' && commands[left + 1][i + 2] == 'd')
				count--;
			else if(count == 0 && commands[left + 1][i] == 'e' && commands[left + 1][i + 1] == 'l' && commands[left + 1][i + 2] == 's' && commands[left + 1][i + 3] == 'e')
			{
				eelse = i;
				break;
			}
		}

		if(eelse == -1)
			return ERROR_IF;

		if(commands[left + 1][len - 3] == 'e' && commands[left + 1][len - 2] == 'n' && commands[left + 1][len - 1] == 'd')
		{
			char* new_cadena = (char*)malloc(sizeof(char) * (len - 3 - eelse - 3));
			strncpy(new_cadena, commands[left + 1] + eelse + 4, len - 3 - eelse - 4);
			new_cadena[len - 3 - eelse - 4] = '\0';
			
			result = splitAndexecute(new_cadena);
			free(new_cadena);
		}
		else
			return ERROR_IF;

	}

	return result;

}
char *path_history() {
    char *path = (char *) malloc(sizeof(char) * (strlen(pw->pw_dir) + strlen(HISTORY_FILE) + 1));

    strcpy(path, pw->pw_dir);
    strcat(path, "/");
    strcat(path, HISTORY_FILE);

    return path;
}
void load_history() {
    for(int i=0;i< 10 ;i++){
        history[i]=(char *) malloc(1024);
    }

    char *path = path_history();

    int status = 0;
    FILE *file;
    file = fopen(path, "r");
    int i = 0;
    if (file != NULL) {
        while (status != -1) {
            char *line = NULL;
            size_t buf_size = 0;
            status = (int) getline(&line, &buf_size, file);
            if (status == -1) {
                free(line);
                continue;
            }
            if (i == 10) break;

            strcpy(history[i], line);
            free(line);
            i++;
        }
        history_len = i;
        fclose(file);
    }
    else{
        file = fopen(path, "w");
        history_len = 0;
        fclose(file);
    }

    free(path);
}
int print_history() {
    for (int j = 0; j < history_len; j++) {
        printf("%d: %s", j + 1, history[j]);
    }
    return 0;
}
int callHelp(int left, int right, char commands[BUF_SZ][BUF_SZ]) {
    if (right == left + 1 || commands[left + 1] == NULL) {
        printf("\n%s\n", commands_help[0]);

        return 0;
    }
    else {

        for (int i = 0; i < SIZE_HELP; i++) {
            if (strcmp(helps[i], commands[left + 1]) == 0) {
                printf("%s", commands_help[i]);
                return 0;
            }
        }

        fprintf(stderr, "%s: command not found\n", commands[left + 1]);

        return 1;
    }
}
void save_history(char command[BUF_SZ]) {
    if (history_len < 0){
        history_len = 0;
    }
    if (history_len == 10) {
        char *aux = history[0];
        for (int i = 1; i < history_len; i++) {
            history[i - 1] = history[i];
        }
        history_len--;

        history[history_len] = aux;
    }

    strcpy(history[history_len++], command);

    char *path = path_history();
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    for (int i = 0; i < history_len; i++) {
        write(fd, history[i], strlen(history[i]));
    }
    write(fd, "\n", 1);
    close(fd);
}

void replace(char *str, size_t len, const char *with, int wlen)
{
  if (wlen < len)
  {
    memmove(str + wlen, str + len, strlen(str + len) + 1);
  }
  else if (wlen > len)
  {
    memmove(str + wlen, str + len, strlen(str + len) + 1);
  }
  memcpy(str, with, wlen);
}

void replace_again(char command[BUF_SZ]) {
	char *again;
	again = strstr(command, "again ");
    
	while(again != NULL)
	{
		int index;
		int k;
		int result = sscanf(again + 6, "%d%n", &index, &k);

		if (result == 1 && index >= 1 && index <= history_len)
		{
			size_t pos = again - command;
			size_t len = 6 + k;

			replace(command + pos, len, history[index - 1], strlen(history[index - 1]) - 1);

			again = strstr(command, "again ");
		}
		else
			again = strstr(again + 6, "again ");
		
	}
	
}


