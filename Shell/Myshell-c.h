//
//              PROJET SYSTEME 2015-2016 ET3 INFO
//
//                             HILARICUS-RICHARD


#ifndef Myshell_c_h
#define Myshell_c_h

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utime.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdint.h>
#include <fcntl.h>
#include <pwd.h>
#include <dirent.h>
#include <libgen.h>
#include <errno.h>

////////////////////////////////    DECLARATIONS GLOBALES         ///////////////////////////

#define BUFFER_SIZE 1024
#define USERNAME 30
#define HOSTNAME 30
#define MAX_ARGS 20
#define MAX_LENGHT_ARGS 50
#define  MAX_LENGHT_LINE_CMD 30
#define MAX_PATH_ARG 16
#define MAX_LENGHT_SUBPATH 128

/*Variables générales*/
char* cmd_line[MAX_LENGHT_LINE_CMD];
char* cmd_argv[MAX_ARGS+1];
char* tryPath[MAX_PATH_ARG];
char ** history = NULL;

int status;
int historyLen = 0;
int quit = 0;
int archived;
int valid_cmd = 1;
int outred;
int inred;


FILE* stream;

/*Variables gestion path globales*/
extern char **environ;
char* save_path;
char* path;

/*Vararibles informations systeme*/
char hostname[HOSTNAME];
char user[USERNAME];
char cDirectory[BUFFER_SIZE];
char user_entry[BUFFER_SIZE];
const char *homedir;


/*Variables touch and copy*/
struct dirent* direntry;
struct stat dataInfo;

/*Variables Expérimentations Pipes*/
int pipe_1[2];/*Pipe in*/
int pipe_2[2];/*Pipe out*/
int pipe_3[2];/*Pipe Erreur*/

char stdoutFile[BUFFER_SIZE-1];
char stdinFile[BUFFER_SIZE-1];
char stderrFile[BUFFER_SIZE-1];

int std_Out=STDOUT_FILENO;
int std_In=STDIN_FILENO;
int std_Err=STDERR_FILENO;
int stdout_Mode='w';
int stderr_Mode='w';

int pipe_Out=0;
int pipe_In=0;
int pipe_Error=0;


/*Variables utilisees pour le processus de redirection*/
int file_descriptor0;
int file_descriptor1;
int in=0;
int out =0;

int verified=1;

char input[BUFFER_SIZE];
char output[BUFFER_SIZE];


/*Déclarations des prototypes des fonctions*/
void  exec_Cmd();
void read_Cmd();
void cut_Cmd();
void change_Directory(char** cmd_argv);
void touch_Cmd(char** cmd_argv);
int history_Cmd(char** cmd_argv);
int cat_Cmd(char *cmd_argv);
int cat_File(char file);
int cp_Cmd(char* original, char* at);
void cp_Directory(struct stat* dataInfo, char* originalPath, char* atPath);
int cp_File(struct stat* dataInfo, char* originalPath, char* atPath);
int cut_Path(const char* pathEnv);
int gestion_Path(char ** cmd_argv);


#endif /* Myshell_c_h */

