#include <stdio.h> 
#include <string.h>   
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

//Array of strings to hold path(s)
char** pathVars;
//Used to hold number of path(s)
int i;

//Prints the name of current working directory
void printDir()
{
    char dirName[50];
    getcwd(dirName,sizeof(dirName));
    printf("%s", dirName);

}


//This function use the access() function to check if a file exists.
//it takes in 3 variables. 
//file is the name of the command entered
//paths is the array of strings which holds each file path separetly
//pathNo is the number of paths
//It checks if the file exists in each path, one by one,
// and if yes, then returns "pathname/filename" otherwise NULL
char* fileExists(char* file, char** paths, int pathNo)
{
  int x=0;

   char** filePath = malloc(50*sizeof(char*));
   
  
  while(x<pathNo)
  {
 
    filePath[x] = malloc(sizeof(char) * strlen(paths[x]));
    strcpy(filePath[x], paths[x]);
    strcat(filePath[x], "/");
    strcat(filePath[x], file);

    if(access(filePath[x], F_OK)==0)
      {
         return filePath[x];
      }

        x++;    
  }
  
  return NULL;
}



//sets home directory
void setHome(char* homeInput)
{
  setenv("HOME", homeInput,1);
  printf("New home directory=%s\n", getenv("HOME"));
}



//sets path directory
//Then it updates the global array of strings pathVars and pathNo with the new path variable(s)
void setPath(char* pathInput)
{
  setenv("PATH", pathInput,1);
  printf("New path=%s\n", getenv("PATH"));


    char *token = strtok(pathInput, ":");
    int c = 0;
    while (token != NULL) {

        strcpy(pathVars[c], token);
        token = strtok(NULL, ":");
        c++;
    }

      int d=0;
      while(isspace(pathVars[c-1][d])==0)
      {
        d++;
      }
      pathVars[c-1][d]='\0';
      i=c;
      
}




//This takes in the input entered by the user 
//and stores each word separetly in an array of strings
char** SortInput(char* input)
{
 
   char** prog_params = malloc(50*sizeof(char*));
    char *tok = strtok(input, " ");

    int j = 0;
    while (tok != NULL) {
        prog_params[j] = malloc(sizeof(char) * strlen(tok));
        strcpy(prog_params[j], tok);
        tok = strtok(NULL, " ");
        j++;
    
      }
      prog_params[j]=NULL;

      return prog_params;
    
       
}



//This takes in the command entered by the user as input which has
//already been sorted by the sortInput function.
//It then uses the fileExists function to see if it exists
//If it does, it is executed
//Otherwise an Error messege is given
void execCommand(char** command, char** paths, int pathNo)
{
        char* fileName = malloc(50*sizeof(char));
        fileName = fileExists(command[0], paths, pathNo);

          if(fileName!=NULL)
          {
            pid_t pid=fork();
            if(pid==-1)
            {
             perror("fork");
            }
            if(pid==0)
            {
              if(execv(fileName, command)==-1)
              {
                perror("execv");
              }
              
            }
            waitpid(pid,0,0);
         }
         else
         {
          printf("Error!\n");
         } 

}




//This asks the user for input
//First it checks if the command is to change path or home variables
//if yes, it goes into the respected functions
//Otherwise, it checks if the command is cd. 
//If yes, it performs the right cd function
//Lastly, if its neither, it goes into the execCommand function.
void commandLine()
{
  printDir();
  printf(">");
  char* input = malloc(100*sizeof(char));
    fgets (input, 50, stdin);
    int len = strlen(input);
    input[len-1]='\0';

    if(input[0]=='$' && input[1]=='H' && input[2]=='O' && input[3]=='M' && input[4]=='E' && input[5]=='=')
      {
        input+=6;
        setHome(input);
        return;
      }
      else if(input[0]=='$' && input[1]=='P' && input[2]=='A' && input[3]=='T' && input[4]=='H' && input[5]=='=')
      {
        input+=6;
        setPath(input);
        return;
      }
      else if(input[0]=='c' && input[1]=='d' && strlen(input)==2)
      {
        chdir(getenv("HOME"));

      }
      else if(input[0]=='c' && input[1]=='d' && input[2]==' ')
      {
          if((input[3]=='.' && input[4]=='.') || input[3]=='~')
          {
            chdir("..");
          }
          else
          {
            input+=3;
             char buf[20];
             getcwd(buf, sizeof(buf));
            char array[50];
            strcpy(array, buf);
            if(strlen(buf)!=1)
            {
            strcat(array, "/");
            }
            strcat(array, input);
            if(chdir(array)!=0)
            {
              printf("Error!\n");
            }
            
             //getcwd(buf, sizeof(buf));
             //printf("%s\n", buf );
          }
      }
      else
      {
        execCommand(SortInput(input), pathVars, i);
      }


  }







int main()
{
  
  
 // printDir();

  FILE *ptr_profile;
 //open the input file
//else error is given and program terminates
        ptr_profile =fopen("profile.txt","r");
        if (!ptr_profile)
        {
          printf("Error! Profile doesnt exist\n");
            return 1;
        }
 
        //Environment variables
        char *path = malloc(100*sizeof(char));
        char *home = malloc(100*sizeof(char));
        char *temp = malloc(100*sizeof(char));
      
            //This part assumes the first two lines of profile.txt contains the home and path variables
        //if either the first or second line is blank
        //an error messege is given
        //if either of the lines has nothing after the "="
        //then an error messege is also given

        if(fgets(path,50, ptr_profile)==NULL || (path[0]!='H' && path[0]!='P') || isspace(path[5])!=0)
        {
          printf("The variable(s) are not assigned\n");
          return 0;
        }
        if(fgets(home,50, ptr_profile)==NULL || (home[0]!='H' && home[0]!='P') || isspace(home[5])!=0 || strlen(home)==5)
        {
            printf("The variable(s) are not assigned\n");
            return 0;
        }

       //This checks which line contains the path and which contains the home variable
        //and stores the information in the appropriate array
        if(path[0]=='H')
        {
          strcpy(temp,path);
          strcpy(path, home);
          strcpy(home,temp);
        }
    
      //this gets rid of home= and path=     
    home+=5;
    path+=5;

   // printf("assigning home to %s\n", home);
   // printf("assigning path to %s\n", path);
   
   // printf("Current HOME : \t%s\n", getenv("HOME"));
    setenv("HOME", home,1);
    //printf("New HOME : \t%s\n", getenv("HOME"));


   //printf("Current PATH : \t%s\n", getenv("PATH"));
   setenv("PATH",path,1);
   //printf("New PATH : \t%s\n", getenv("PATH"));



   //this splits the path(s) based on ":"
    pathVars = malloc(50*sizeof(char*));

    char *token = strtok(path, ":");
    i = 0;
    while (token != NULL) {
        pathVars[i] = malloc(sizeof(char) * 20);
        strcpy(pathVars[i], token);
        token = strtok(NULL, ":");
        i++;
    }

      int a=0;
      while(isspace(pathVars[i-1][a])==0)
      {
        a++;
      }
      pathVars[i-1][a]='\0';



      while(1)
      {
        commandLine();
      }
      



    fclose(ptr_profile);


        return 0;

 

}