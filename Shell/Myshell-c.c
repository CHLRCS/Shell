//
//              PROJET SYSTEME 2015-2016 ET3 INFO
//
//                      HILARICUS


#include "Myshell-c.h"

int main()
{
    printf(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
    printf(":::::::::::::::::::: WELCOME TO MY SHELL ::::::::::::::::::::\n");
    printf(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");

    getlogin_r(user,USERNAME);
    gethostname(hostname,HOSTNAME-1);
    getcwd(cDirectory, BUFFER_SIZE-1);

    struct passwd *pw = getpwuid(getuid());
    homedir = pw->pw_dir;
    save_path = getenv("PATH");
    while(!quit){

        printf("\n %s@%s:%s >",user,hostname,cDirectory);
        stream = stdout;
        fflush(stdout);
        read_Cmd();
        exec_Cmd();
        int i=0;
        /*Liberation de l'espace memoire allouee*/
        while(cmd_argv[i] != NULL)
        {
            free(cmd_argv[i]);
            cmd_argv[i] = NULL;;
            i++;

        }
        i = 0;
        while(tryPath[i] != NULL)
        {
            free(tryPath[i]);
            tryPath[i] = NULL;;
            i++;
        }
        
        /*Gestion historique*/
        if(verified!=0){
            /*Copie du flux d'entree dans l'historique*/
            archived  = open("history.txt", O_WRONLY|O_APPEND|O_CREAT,S_IWUSR|S_IRUSR);
            if(archived!= -1){
                write(archived,user_entry,strlen(user_entry));
                write(archived,"\n",1);
            }
            /*Fermeture du fichier history.txt contenant l'historique*/
        }
        close(archived);
    }
    return 0;
}

//////////////////////////////////// FONCTION "Read_Cmd" ///////////////////////////////////////
/*
 *  Lecture du flux d'entree (ndlr : saisie de l'utilisateur)
 */
//////////////////////////////////////////////////////////////////////////////////////////////

void read_Cmd(){
  /*Recuperation du flux d'entree et stockage dans user_entry*/
    fgets(user_entry,BUFFER_SIZE,stdin);
  /*Recherche du caractere \n pour harmoniser le flux d'entree*/
    if(strchr(user_entry, '\n'))
        *strchr(user_entry,'\n')=0;

  /*Appel fonction de decoupage du flux stocke*/
    cut_Cmd();
}

//////////////////////////////////// FONCTION "Cut_Cmd" ///////////////////////////////////////
/*
 *  Decoupage du flux d'entree en arguments et stockage de celui ci dans un Tableau de Char
 */
//////////////////////////////////////////////////////////////////////////////////////////////
void cut_Cmd(){
    int i=0,argc=0,traitement=0;
    /*Traitement de la chaine de caractere recue*/
    while(i<strlen(user_entry)){
    /*Reperage et traitement des espace et \0 separant les arguments*/
        while(user_entry[i] == ' ')
        {
            i++;
            traitement++;
        }
        while((user_entry[i] != ' ') && (user_entry[i] != '\0')){
            i++;
        }
        i=traitement;
        cmd_argv[argc]= malloc(MAX_LENGHT_ARGS * sizeof(char));/*Allocation memoire de l'espace necessaire au stockage des arguments coupés en mémoire*/
        while((user_entry[i] != ' ') && (user_entry[i] != '\0')){
            /*Stockage des arguments et de leur position dans cmd_argv*/
            cmd_argv[argc][i-traitement] = user_entry[i];
            i++;
        }
        cmd_argv[argc][i-traitement] = '\0';
        traitement = i;
        argc++;
    }
}

///////////////////////////////////////// FONCTION "cut_Path" ////////////////////////////////////////////////
/* Decoupage du chemin externe en arguments(sous-chemins) et stockage des sous-chemins dans un Tableau de Char
 * Entrée : Chemin externe
 * Sortie : Numero du sous chemin stocké dans TryPath
 */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
int cut_Path(const char* pathEnv)
{
    int i=0,pathNo=0,traitement=0;

    while(i<strlen(pathEnv)){
        while(pathEnv[i] == ':')/*Traitement du chemin externe recu , suppression des caracteres non souhaités*/
        {
            i++;
            traitement++;
        }

        i = traitement;
        tryPath[pathNo] = malloc(MAX_LENGHT_SUBPATH * sizeof(char)); /*Allocation memoire des sous-chemins*/

        while((pathEnv[i] != ':') && pathEnv[i] != '\0'){
            tryPath[pathNo][i-traitement] = pathEnv[i];/*Stockage des sous-chemins et de leur numero*/
            i++;
        }

        tryPath[pathNo][i-traitement] = '\0';
        traitement = i;
        pathNo++;
    }
    return pathNo;
}


//////////////////////////////////// FONCTION "exec_Cmd" ///////////////////////////////////////
/*
 *  Gestion des executions des differentes demandes recues par le Shell
 */
//////////////////////////////////////////////////////////////////////////////////////////////
void  exec_Cmd(){

    int i;

    /*Gestion de la redirection*/
    
    for(i=0;cmd_argv[i];i++){

        if(strcmp(cmd_argv[i],">") == 0 || strcmp(cmd_argv[i],">>") == 0){/*Recherche des ">" ou ">>" signe de redirection dans les arguments recus*/
            cmd_argv[i]=NULL;
            /*Stockage fichier a rediriger dans fichier de destination*/
            stream=fopen(cmd_argv[i+1],"a+");
            strcpy(stdoutFile,cmd_argv[i+1]);
            outred = 2;
        }
    }



    /*Gestion des Pipes (NON FONCTIONNEL - Il s'agit d'experimentations)*/
    /* Ouverture des pipes*/
    if(pipe_Out){
        if(access(stdoutFile,F_OK)==-1){
            creat(stdoutFile,0777);
        }
        if(stdout_Mode=='a'){
            std_Out = open(stdoutFile,O_WRONLY | O_APPEND);
        }else{
            std_Out = open(stdoutFile,O_WRONLY);
        }
    }

    if(pipe_In){
        std_In = pipe_1[0];
    }
    if(pipe_Error){
        if(access(stdoutFile,F_OK)==-1){
            creat(stdoutFile,0666);
        }
        if(stderr_Mode=='a'){
            std_Err = open(stdoutFile,O_WRONLY | O_APPEND);
        }else{
            std_Err = open(stdoutFile,O_WRONLY);
        }

    }
    
    /*Analyse du premier argument et execution de la commande souhaitée*/
    if(cmd_argv[0] == NULL){/*Absence de saisie de la part de l'utilisateur*/
            printf("PAS DE COMMANDE SAISIE");
            verified=0;
        goto end;
    }
    if(!strcmp(cmd_argv[0],"cd")){/*Gestion de l'execution de la commande inter "cd"*/
        change_Directory(cmd_argv);
        goto end;
    } else if(!strcmp(cmd_argv[0],"history")){/*Gestion de l'execution de la commande interne "history"*/
        history_Cmd(cmd_argv);
        verified=1;
        goto end;
    } else  if(!strcmp(cmd_argv[0],"quit") || !strcmp(cmd_argv[0],"exit") || !strcmp(cmd_argv[0],"leave")){/*Gestion de l'execution des commandes "exit" - "quit" - "leave"*/
        quit=1;
        verified=1;
        goto end;
    } else  if(!strcmp(cmd_argv[0],"touch")){/*Gestion de l'execution de la commande interne "touch"*/
        touch_Cmd(cmd_argv);
        verified=1;
        goto end;
    } else  if(!strcmp(cmd_argv[0],"cat")){/*Gestion de l'execution de la commande interne "cat"*/
            for (i = 1; i < 512; i++) {
                cat_Cmd(cmd_argv[i]);/*Tranmission des arguments en ca de concaténation*/
            }
        verified=1;
        goto end;
    }else  if(!strcmp(cmd_argv[0],"cp")){/*Gestion de l'execution de la commande interne "cp"*/
        cp_Cmd(cmd_argv[1],cmd_argv[2]);
        goto end;
    }else{/*Gestion execution de commandes externes - gestion du Path*/
        gestion_Path(cmd_argv);
        goto end;
    }
end:

    if(stream != NULL && stream != stdin && stream != stdout)
    {
        fclose(stream);
        stream = NULL;
    }
}


////////////////////////////////////// FONCTION "gestion_Path" //////////////////////////////////////////////
/* Decoupage du chemin externe en arguments(sous-chemins) et stockage des sous-chemins dans un Tableau de Char
 * Entrée : Arguments stockes
 *  Sortie : 0
 */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
int gestion_Path(char ** cmd_argv){
    int i, n;
    const char* pathEn = getenv("PATH"); /*Recuperation du Path*/
    char* fragPath;

    if(!access(cmd_argv[0],F_OK))   /* Cas ou le fichier est dans le repertoire courant = > Alors on execute celui-ci */
    {
        if(!access(cmd_argv[0],X_OK))
        {
            pid_t pid;
            pid = fork();
            if(pid == 0){

                if(outred){
                    file_descriptor1 = open(stdoutFile,O_WRONLY|O_CREAT,0666);
                    dup2(file_descriptor1, STDOUT_FILENO);
                    fflush(stdout);
                    close(file_descriptor1);
                }

                execve(cmd_argv[0], cmd_argv,environ);

                if(pipe_Out){
                    dup2(pipe_1[1],STDOUT_FILENO);
                    close(pipe_1[1]);
                }else if(pipe_In){
                    dup2(pipe_2[0],STDIN_FILENO);
                    close(pipe_2[0]);
                }else if(pipe_Error){
                    dup2(pipe_3[1],STDERR_FILENO);
                    close(pipe_3[1]);
                }
                outred = 0;

            }else
                wait(EXIT_SUCCESS);
        }
        else
        {
            perror("access()");
        }
    }
    else/*Sinon verification du chemin du Path et verification de l'acces et l'executabilite dans chaque chemins que contient le Path*/
    {

        i = 0;
        n = cut_Path(pathEn);

        while(i<n)
        {
            fragPath = tryPath[i];
            strcat(fragPath, "/");
            strcat(fragPath,cmd_argv[0]);
            strcat(fragPath,"\0");
/*Verification des permissions et si seulement on a les permissions alors on peut gerer le path*/
            if(!access(fragPath,F_OK))
            {
                if(!access(fragPath,X_OK))
                {
                    pid_t pid;
                    pid = fork();

                    if(pid == 0){
                        /*Gestion redirections pour les commandes externes*/
                        if(outred){
                            file_descriptor1 = open(stdoutFile,O_WRONLY|O_CREAT,0666);
                            dup2(file_descriptor1, STDOUT_FILENO);
                            fflush(stdout);
                            close(file_descriptor1);
                        }
                        /*Execution de la commande*/
                        execve(fragPath, cmd_argv,environ);
                        /*Gestion des pipes - Expérimentation non aboutie*/
                        if(pipe_Out){
                            dup2(pipe_1[1],STDOUT_FILENO);
                            close(pipe_1[1]);
                        }else if(pipe_In){
                            dup2(pipe_2[0],STDIN_FILENO);
                            close(pipe_2[0]);
                        }else if(pipe_Error){
                            dup2(pipe_3[1],STDERR_FILENO);
                            close(pipe_3[1]);
                        }
                        outred = 0;

                    }
                    else
                        wait(EXIT_SUCCESS);

                    goto end; /*Si on a trouve le chemin et que l'on a les permissions alors on l'a execute et maintenant on sort de la fonction*/
                }
                else/*Si non on previent l'utilisateur*/
                {
                    perror("access()");
                    goto end;/*On sort de la fonction meme si on a trouve le fichier mais on a pas la permission donc on sort de la fonction de gestion du Path*/
                }
            }
        /*Si non si le fichier n'est pas dans le repertoire courant analyse on passe au chemin suivant */
        i++;
        }
        errno = ENOENT;
        perror("");
    }
end:
    return 0;
}


//////////////////////////////  FONCTION "change_Directory"  //////////////////////////////////
/*
 *  Changement de repertoire courant
 */
//////////////////////////////////////////////////////////////////////////////////////////////
void change_Directory(char** cmd_argv){

  if(strcmp(cmd_argv[1],"~") == 0){/*Saisie du caractere ~ pour un retour a la racine*/
    chdir(homedir); /*Mise a jour du rÈpertoire courant vers la racine*/
    } else {
    chdir(cmd_argv[1]); /*Mise a jour du repertoire courant vers rÈpertoire desire*/
    }
    /*Mise a jour du repertoire courant au niveau de l'information de l'utilisateur du shell*/
    getcwd(cDirectory, BUFFER_SIZE-1);

}

//////////////////////////////  FONCTION "history_Cmd"  //////////////////////////////////
/*
 *  Gestion de l'historique du Shell
 */
//////////////////////////////////////////////////////////////////////////////////////////////
int history_Cmd(){
    FILE* fichier = NULL;
    int i=0;
    char chaine[50] = "";
    fichier = fopen("history.txt", "r");/*Ouverture du fichier contenant l'historique*/
    if (fichier != NULL){
        while (fgets(chaine, 30, fichier) != NULL)
        {
	  printf("%s", chaine);/*Affichage de l'historique*/
        }
        fclose(fichier);/*Fermeture du fichier contenant l'historique*/
    }
    return 0;
}


//////////////////////////////  FONCTION "touch_Cmd"  //////////////////////////////////
/*
 *  Changement de repertoire courant
 */
//////////////////////////////////////////////////////////////////////////////////////////////
//  modification de la date d'un fichier ou creation du fichier is inexistant //
void touch_Cmd(char** user_argv)
{
    char* name[25];
    int numberOfFiles=0,i=0,t_access=1,t_modif=1;
    /*Recuperation des arguments*/
    for(i=1; user_argv[i]; i++)
    {
        if(!strcmp(user_argv[i], "-m") && numberOfFiles == 0)
            t_access = 0;
        else if(user_argv[i][0] != '-')
        {
            name[numberOfFiles] = user_argv[i];
            numberOfFiles++;
        }else{
            printf("Mauvais arguments");
            return;
        }
    }

    for(i=0; i < numberOfFiles; i++)
    {

        if(access(name[i], F_OK) == -1)
            creat(name[i], 0777); /*Creation du fichier si inexistant*/

        else
	  {/*Modification du time acces du fichier*/
            struct utimbuf time;
            time.actime = t_access;
            time.modtime = t_modif;
            utime(name[i], &time);
        }
    }
}


//////////////////////////// FONCTIONS "cat_Cmd" et "cat_File" ////////////////////////////////
/*
 *  Afficher le contenu de fichier - concatener deux fichiers
 */
//////////////////////////////////////////////////////////////////////////////////////////////
int cat_Cmd(char *cmd_argv){
    int file=0;
    int cat=0;
    int i;
    
    if((file=open(cmd_argv,O_RDONLY)) == -1){/*Ouverture du fichier si rien retourne -1*/
        return -1;
    }
    cat = cat_File(file);/*Appel de la fonction de concatenation/Affichage sur sortie pour le fichier ouvert*/
    if(close(file) !=0){
        return -1;
    }
    return cat;
}

int cat_File(char file){
    int n;
    char data[4096];
    while((n=read(file,data,sizeof data))>0){
     int ntotal = 0;
        while(ntotal<n){
         int nread = fprintf(stream,"%s\n",data);/*Affichage du fichier ou des fichiers concantenes*/
            if(nread <1){
                return -1;
            }
                ntotal+=nread;
        }
    }
    return 0;
}

///////////////// FONCTIONS "cp_Cmd" - "cp_File" - "cp_Directory"  /////////////////////////////
/*
 *  Copie de fichiers - dossiers
 */
//////////////////////////////////////////////////////////////////////////////////////////////

int cp_Cmd(char* original, char* at){
    char* fname;
    char* originalAlloc;
    int originalLenght;
    int fnameLenght;
    struct stat source;
    
    if(S_ISDIR(source.st_mode)){/*Verification du type de donnees a copier*/
        fname=basename(original);
        originalLenght = strlen(original);
        fnameLenght = strlen(fname);
        originalAlloc = (char*)malloc((originalLenght+1+fnameLenght+1)*sizeof(char));/*Allocation memoire du fichier d'origine*/
        memcpy(originalAlloc, at, originalLenght);
        if(original[originalLenght-1]!= '/'){
            originalAlloc[originalLenght] = '/';
            memcpy(originalAlloc+originalLenght+1,fname,fnameLenght);
            originalAlloc[originalLenght+1+fnameLenght] = '\0';
        }else{
            memcpy(originalAlloc+originalLenght, fname , fnameLenght);
            originalAlloc[originalLenght+fnameLenght] = '\0';
        }
        at=originalAlloc;
    }
    
    if(S_ISDIR(dataInfo.st_mode)){/*Verification de si il s'agit d'un dossier*/
        cp_Directory(&source,original,at);/*Copie d'un dossier*/
    }else{
        cp_File(&source,original,at);/*Copie d'un fichier*/
    }
    return 0;
}

/* meme procédure pour cp_Directory et cp_File */

int cp_File(struct stat* source, char* originalPath, char* atPath){
    int origin = open(originalPath, O_RDWR);
    int at = open(atPath,O_RDWR | O_CREAT,source->st_mode);
    int lenght=0;
    char seizure[512];
    if(at ==-1){ /*Erreur lors de la création du fichier copié*/
        printf("ERREUR CREATION FICHIER");
    }
    while((lenght = read(origin,seizure,512)) !=0){
        write(at, seizure, lenght);
    }
    /*Fermeture des fichiers ouverts*/
    close(origin);
    close(at);
    return EXIT_SUCCESS;
}

void cp_Directory(struct stat* source, char* originalPath, char* atPath){
    
    char oFreeMemory = 0;
    char atFreeMemory = 0;
    char* atAlloc;
    char* originalAlloc;
    int oLenght = strlen(originalPath);
    int atLenght = strlen(atPath);
    int dirnameLenght;
    DIR* originDir = opendir(originalPath);
    
    if(atPath[atLenght-1]!= '/'){
        atFreeMemory=1;
        atAlloc = (char*)malloc((atLenght+2)*sizeof(char));
        memcpy(atAlloc,atPath,atLenght);
        atPath=atAlloc;
        atLenght++;
    }else if(originalPath[oLenght-1]!= '/'){
        oFreeMemory=1;
        originalAlloc = (char*)malloc((oLenght+2)*sizeof(char));
        memcpy(originalAlloc,originalPath,oLenght);
        originalPath=originalAlloc;
        oLenght++;
    }
    if(originDir == NULL){/*Si pas de répertoire d'origine*/
        printf("Impossible d'ouvrir le repertoire");
    }
    
    
    while((direntry = readdir(originDir)) != NULL){
        if(!strcmp(direntry->d_name, "..") || !strcmp(direntry->d_name,".")){
            continue;
        }
        dirnameLenght = strlen(direntry->d_name);
        originalAlloc = (char*)malloc((oLenght +dirnameLenght+1)*sizeof(char));
        atAlloc = (char*)malloc((atLenght+dirnameLenght+1)*sizeof(char));
        
        memcpy(atAlloc,atPath,atLenght);
        memcpy(atAlloc+atLenght,direntry->d_name,dirnameLenght);
        atAlloc[atLenght+dirnameLenght]='\0';
        
        memcpy(originalAlloc,originalPath,atLenght);
        memcpy(originalAlloc+atLenght,direntry->d_name,dirnameLenght);
        originalAlloc[atLenght+dirnameLenght]='\0';
        
        cp_Cmd(originalAlloc,atAlloc);
        free(atAlloc);
        free(originalAlloc);
    }
    
    closedir(originDir);
}




