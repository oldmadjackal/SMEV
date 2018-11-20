/*********************************************************************/
/*                                                                   */
/*             ������� ������ � ��������� ����                       */
/*                                                                   */
/*********************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>
#include <time.h>
#include <ctype.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/stat.h>


#include "resource.h"


#define  __MAIN__

#include "SMEV.h"


/*********************************************************************/
/*                                                                   */
/*	                      MAIN                     	         */	
/*                                                                   */
/*    ����� ��������� ������:                                        */
/*                                                                   */
/*     /config:<path>      ���� � ����� ������������                 */
/*     /silent             ���������� ��� ��������                   */
/*                                                                   */
/*     @<path>             ���� � ����� � ������� ��������� ������   */
/*                                                                   */
/*    ������������ ����:                                             */
/*         0  -  ���������� ����������                               */
/*         1  -  ������ �������������                                */
/*         2  -  ������ �������� ���������� ����                     */
/*         3  -  ������ ���������� ����� ������������                */
/*         4  -  ������ ���������� ����� ������ ��������� ������     */
/*         5  -  ����������� ������ ����                             */
/*       101  -  ������ ��� ������ � ��������                        */
/*       102  -  ��������� ������ ������ �� ������� ��� ��������     */

int cmp_int(const void * p1, const void * p2) {
    int s1 = *(int*)p1; int s2 = *(int*)p2;
    if ((s1 % 2 == 0) && (s2 % 2 == 0)) {
      if (s1 < s2) return -1;
      else if (s1 > s2) return 1;
    }
    
    if(p1>p2) return  1;
              return -1;
}

int main(int argc, char **argv)
{
           char  cmd_line[2048] ;
            int  status ;
           char  text[2048] ;
           char *key ;
           char *end ;
            int  i ;

/*---------------------------------------------------- ������������� */

            printf("SMEV tools, version 20.10.2018\n") ;

/*------------------------------------------ ������ ��������� ������ */

       memset(cmd_line, 0, sizeof(cmd_line)) ;

     for(i=1 ; i<argc ; i++)  {

       if(*cmd_line!=0)  strcat(cmd_line, " ") ;
                         strcat(cmd_line, argv[i]) ;
                              }
/*- - - - - - - - - - - - - - - - - - -  ���������� ��������� ������ */
          memset(text, 0, sizeof(text)) ;

             status=SMEV_read_command(cmd_line, text, sizeof(text)-1) ;
          if(status) {
                        printf("Bad command line\n") ;
                        return(_COMMAND_READ_RC) ;
                     }
/*- - - - - - - - - - - - - - - - - - - - -  ������ ��������� ������ */
       key=strstr(text, "/config:") ;
    if(key!=NULL) {
                    strncpy(__config_path, key+strlen("/config:"), sizeof(__config_path)-1) ;
                 end=strchr(__config_path, '\n') ;
              if(end!=NULL)  *end=0 ;
                  }
/*------------------------------- ���������� ���������������� ������ */

       status=SMEV_read_config() ;
    if(status)   return(_CONFIG_READ_RC) ;

/*----------------------------------- ���������� � ���������� ������ */

          status=SMEV_silent_process() ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*								     */
/*		���������� ������ ��������� ������                   */

  int  SMEV_silent_process(void)
{
   int  status ;
  char  text[1024] ;


                      __silent_rc=0 ;

   do {
            status=SMEV_rkn_exchange("SILENT_MODE") ;
         if(status>0)  break ;

                   sprintf(text, "Unknown SMEV service: %s", __SMEV_name) ;
                  SMEV_log(text) ;
                    return(_SERVICE_UNKNOWN_RC) ;  

      } while(0) ;

    return(__silent_rc) ;
}


/********************************************************************/
/*                                                                  */
/*                       ������� ������ ���������                   */

   int  SMEV_message(char *text)
{
     return( SMEV_message(text, 0) ) ;
}

   int  SMEV_message(char *text, int  icon)
{

                            printf("%s\n", text) ;
   if(__log_path[0]!=0 )  SMEV_log(text) ;

   return(0) ;
}


/*********************************************************************/
/*								     */
/*	                         ������� ����                        */

  int  SMEV_log(char *text)

{
     time_t  time_abs ;
  struct tm *hhmmss ;
       char  path[FILENAME_MAX] ;
       FILE *file ;
       char  prefix[512] ;
       char  error[1024] ;
       char *entry ;
 
/*------------------------------------------------- ������� �������� */

    if(__log_path[0]==0) {
//                           SMEV_message("�� ����� ���� ����� ����") ;
                                return(-1) ;
                         }
/*------------------------------------------ ������������ ����� ���� */

                     strcpy(path, __log_path) ;

                          entry=strrchr(path, '/') ;
         if(entry==NULL)  entry=        path ;
                          entry=strrchr(path, '.') ;
/*
         if(entry==NULL) {
                             sprintf(prefix, ".%s", __user) ;
                              strcat(path, prefix) ;
                         }
         else            {
                             sprintf(prefix, ".%s.", __user) ;
                     SMEV_text_subst(entry, ".", prefix) ;
                         }
*/
/*---------------------------------------------- �������� ����� ���� */

       file=fopen(path, "at") ;
    if(file==NULL) {
                          sprintf(error, "������ �������� ����� ���� %d : %s", errno, path) ;
                     SMEV_message(error) ;
                       return(-1) ;
                   }
/*-------------------------------------------- �������� ����� ������ */

/* 
        status=_heapchk() ;
     if(status!=_HEAPOK    && 
        status!=_HEAPEMPTY   ) {

          sprintf(prefix, "Heap crash!!!") ;
           fwrite(prefix, 1, strlen(prefix), file) ;

                               } 

*/
/*------------------------------------------------------ ������ ���� */

    if(text!=NULL) {

               time_abs=     time( NULL) ;
                 hhmmss=localtime(&time_abs) ;

          sprintf(prefix, "%02d.%02d %02d:%02d:%02d>>",
                                    hhmmss->tm_mday,
                                    hhmmss->tm_mon+1,
                                    hhmmss->tm_hour,
                                    hhmmss->tm_min,    
                                    hhmmss->tm_sec  ) ;

           fwrite(prefix, 1, strlen(prefix), file) ;
           fwrite(text,   1, strlen(text),   file) ;
           fwrite("\n",   1, strlen("\n"),   file) ;

                  }
/*---------------------------------------------- �������� ����� ���� */

           fclose(file) ;

/*-------------------------------------------------------------------*/
                       
   return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*	              ����������� ������ � ����       	             */	

   void  SMEV_show(char *text)
{
   char *entry ;
   char *end ;
   char  row[256] ;
    int  cnt ;


    for(entry=text ; *entry!=0 ; entry+=strlen(row)) {

             memset(row, 0, sizeof(row)) ;
            strncpy(row, entry, sizeof(row)-1) ;
         end=strchr(row, '\n') ;
      if(end!=NULL) {  end[0]=' ' ;  end[1]=0 ; }
         end=strchr(row, '\r') ;
      if(end!=NULL)    end[0]=' ' ;

                 printf("%s\n", row) ;
                                                     }

}


/*********************************************************************/
/*								     */
/*           ���������� ����� ��������� ������                       */	

  int  SMEV_read_command(char *command, char *buff, int  size)

{
       FILE *file ;
       char  message[1024] ;
       char *tmp ;

/*---------------------------------------------- ���� ��������� ���� */

     if(command[0]=='@') {

        file=fopen(command+1, "rb") ;                               /* �������� ����� */
     if(file==NULL) {
                        sprintf(message, "Command file open error %d :%s", errno, command+1) ;
                   SMEV_message(message) ;
                          return(-1) ;
                    }

         fread(buff, 1, size, file) ;                               /* ���������� ����� */
        fclose(file) ;                                              /* �������� ����� */

                         }
/*-------------------------------------------- ���� ��������� ������ */
     else                {

                 strncpy(buff, command, size) ;

                         }
/*------------------------------------------------- ��������� ������ */

    for(tmp=buff ; *tmp ; tmp++)                                    /* ������ ����-�������� */
            if(*tmp=='\t') *tmp=' ' ;
       else if(*tmp=='\r') *tmp='\n' ;

        SMEV_text_subst(buff, " /", "\n/") ;

/*-------------------------------------------------------------------*/

  return(0) ;
}

/*********************************************************************/
/*								     */
/*                   ���������� ����� ������������                   */

   int  SMEV_read_config(void)

{
       FILE *file ;
       char  root_path[1024] ;
       char  text[1024] ;
       char  message[1024] ;
       char *entry ;
       char *end ;
        int  i ;

 static  struct {
                  char *key ;
                  void *value ;     /* ��������!!! ��� ������, ���������� ���� ������ ����� ����� �� ����� FILENAME_MAX */
                  char  type ;
                   int  size ;
                }  pars[]={
                           { "$ROOT$="          ,    root_path,        'C', sizeof(         root_path) },
                           { "Log_path="        ,   __log_path,        'C', sizeof(        __log_path) },
                           { "Service_name="    ,  __SMEV_name,        'C', sizeof(       __SMEV_name) },
                           { "Service_url="     ,  __SMEV_url,         'C', sizeof(       __SMEV_url ) },
                           { "Crypto_sign_in="  ,  __crypto_sign_in,   'C', sizeof(__crypto_sign_in  ) },
                           { "Crypto_sign_out=" ,  __crypto_sign_out,  'C', sizeof(__crypto_sign_out ) },
                           { "Crypto_sign_exec=",  __crypto_sign_exec, 'C', sizeof(__crypto_sign_exec) },
                           { "Cookie_path="     ,  __cookie_path,      'C', sizeof(     __cookie_path) },
                           { "Result_path="     ,  __result_path,      'C', sizeof(     __result_path) },
                           {  NULL }
                          } ;

/*---------------------------------------------------- ������������� */

         memset(root_path, 0, sizeof(root_path)) ;

/*--------------------------------------------------- �������� ����� */

        file=fopen(__config_path, "rb") ;
     if(file==NULL) {
                        sprintf(message, "Config file open error %d :%s", errno, __config_path) ;
                   SMEV_message(message) ;
                          return(-1) ;
                    }
/*------------------------------------------------- ���������� ����� */

                      memset(text, 0, sizeof(text)) ;

     while(1) {                                                     /* CIRCLE.1 - ��������� ������ ���� */
/*- - - - - - - - - - - - - - - - - - -  ���������� ��������� ������ */
                   end=fgets(text, sizeof(text)-1, file) ;          /* ��������� ������ */
                if(end==NULL)  break ;

         if(text[0]==';')  continue ;                               /* �������� ����������� */

            end=strchr(text, '\n') ;                                /* ������� ������ ����� ������ */
         if(end!=NULL)  *end=0 ;
            end=strchr(text, '\r') ;
         if(end!=NULL)  *end=0 ;

             SMEV_text_trim(text) ;                                 /* ��������� ������� ������� */
         if(text[0]== 0 )  continue ;                               /* ���������� ������ ������ */
/*- - - - - - - - - - - - - - - - - - - ���������� �����-����������� */
     if(root_path[0]!=0)  
              SMEV_text_subst(text, "$ROOT$", root_path) ;
/*- - - - - - - - - - - - - - - - - - - - - - ����� "Parameter:...=" */
         if(!memcmp(text, "Parameter:",
                   strlen("Parameter:"))) {

              entry=text+strlen("Parameter:") ;

           if(__config_pars_cnt>=_CONFIG_PARS_MAX) {
                                   sprintf(message, "Configuration parameterslist overflow") ;
                              SMEV_message(message) ;
                                      return(-1) ;
                                                   }

           end=strchr(entry, '=') ;
        if(end==NULL) {
                                   sprintf(message, "Separator = is missed in specification: %s", text) ;
                              SMEV_message(message) ;
                                      return(-1) ;
                      }

                                                            *end=0 ;
             strncpy(__config_pars[__config_pars_cnt].name,  entry, sizeof(__config_pars->name )) ; 
             strncpy(__config_pars[__config_pars_cnt].value, end+1, sizeof(__config_pars->value)) ; 
                                   __config_pars_cnt++ ;

                                                continue ;
                                          }
/*- - - - - - - - - - - - - - - - - - - -  �������� ��������� ������ */
       for(i=0 ; pars[i].key!=NULL ; i++)                           /* �������.����� */
         if(!memcmp(text, pars[i].key,
                   strlen(pars[i].key ))) {

             if(pars[i].type=='I')  *(int *)pars[i].value=atoi(text+strlen(pars[i].key)) ;
             else           strncpy((char *)pars[i].value, text+strlen(pars[i].key), pars[i].size-1) ;

                                                break ;
                                          }

         if(pars[i].key!=NULL)  continue ;                          /* ���� ������ ���������������� - */
                                                                    /*     ��������� � ���������      */
               sprintf(message, "Unknown key in config file: %s", text) ;
          SMEV_message(message) ;
                 return(-1) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
              }                                                     /* CONTINUE.1 */
/*--------------------------------------------------- �������� ����� */

                   fclose(file) ;

/*---------------------------- �������������� ������������� �������� */

/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*          ������� ��������� � �������� ���������� ��������         */

  void  SMEV_text_trim(char *text)

{
  char *beg ;
  char *end ;


    for(end=text+strlen(text)-1 ; 
        end>=text && (*end==' ' || *end=='\t'
                                || *end=='\n'
                                || *end=='\r') ; end--)  *end=0 ;

    for(beg=text ;    *beg==' ' || *beg=='\t'
                                || *beg=='\n'
                                || *beg=='\r'  ; beg++)  ;
  

        strcpy(text, beg) ;
}


/*********************************************************************/
/*                                                                   */
/*                 ������������ ���� � �������                       */
/*                                                                   */
/*  ����������: 0 ��� ��� ������                                     */

  int  SMEV_create_path(char *folder)

{
  char  path[FILENAME_MAX] ;
  char *cut ;
   int  status ;

/*------------------------------------------------------- ���������� */

               strncpy(path, folder, sizeof(path)-1) ;

/*-------------------------------------------------- ������ �������� */

       if(path[1]==':') {                                           /* ������ �������� �������� */
         if(path[0]=='a' || path[0]=='�')  path[0]='A' ;
         if(path[0]=='�' || path[0]=='�')  path[0]='B' ;
         if(path[0]=='�' || path[0]=='�')  path[0]='C' ;
                        }
/*-------------------------------------- �������� ������������� ���� */

        status=access(path, 0x00) ;
     if(status==0)  return(0) ;

/*------------------------------------------------ ������������ ���� */

        for(cut=path ; ; cut=cut+1) {
                        
              cut=strchr(cut, '/') ;
           if(cut!=NULL) *cut=0 ;

                       status=access(path, 0x00) ;
           if(status)  status= mkdir(path, 0777) ;
           if(status)   return(errno) ;

           if(cut!=NULL) *cut='/' ;
           if(cut==NULL)  break ;
                                    }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*                 ����������� ����� ������                         */

   int  SMEV_text_subst(char *buff, char *name, char *value)

{
   char *entry ;
    int  i ;


    if(value==NULL)  value="" ;

             entry=buff ;

     for(i=0 ; i<100 ; i++) {

             entry=strstr(entry, name) ;
          if(entry==NULL)  break ;
                         
               memmove(entry+strlen(value), entry+strlen(name), 
                                       strlen(entry+strlen(name))+1) ;
                memcpy(entry, value, strlen(value)) ;

                       entry+=strlen(value) ;

                            } ;

  return(i) ;
}


/*********************************************************************/
/*                                                                   */
/*	         ��������� ������ �� �������� ���������       	     */	

   char *SMEV_get_parameter(char *name)
{
  int  i ;


     for(i=0 ; i<__config_pars_cnt ; i++)
       if(!stricmp(name, __config_pars[i].name))  return(__config_pars[i].value) ;

  return(NULL) ;
}


/*********************************************************************/
/*								     */
/*               ������ � ������ ���������� ���������                */

   int  SMEV_cookie(char *oper, char *data, int  size)

{
       FILE *file ;
       char  text[1024] ;

/*------------------------------------------------------- ���������� */

   if(!stricmp(oper, "READ" )) {
                                  memset(data, 0, size) ;
                               }
/*------------------------------------------------- ������� �������� */

      if(__cookie_path[0]==0)  return(0) ;

/*---------------------------------------------------- ������ ������ */

   if(!stricmp(oper, "READ")) {

        file=fopen(__cookie_path, "rb") ;                           /* �������� ����� */
     if(file==NULL) {
                        sprintf(text, "Cookie file open error %d :%s", errno, __cookie_path) ;
                       SMEV_log(text) ;
                          return(-1) ;
                    }

                     fread(data, 1, size, file) ;                   /* ���������� ����� */
                    fclose(file) ;                                  /* �������� ����� */

                               }
/*---------------------------------------------------- ������ ������ */

   if(!stricmp(oper, "SAVE")) {

        file=fopen(__cookie_path, "wb") ;                           /* �������� ����� */
     if(file==NULL) {
                        sprintf(text, "Cookie file open error %d :%s", errno, __cookie_path) ;
                       SMEV_log(text) ;
                          return(-1) ;
                    }

                    fwrite(data, 1, size, file) ;                   /* ������ ����� */
                    fclose(file) ;                                  /* �������� ����� */

                              }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*								     */
/*                    ������ ����� ����������                        */

   int  SMEV_result(char *data, int  size)

{
       FILE *file ;
       char  text[1024] ;

/*------------------------------------------------- ������� �������� */

     if(__result_path[0]==0)  return(0) ;

/*---------------------------------------------------- ������ ������ */

        file=fopen(__result_path, "wb") ;                           /* �������� ����� */
     if(file==NULL) {
                        sprintf(text, "Result file open error %d :%s", errno, __result_path) ;
                       SMEV_log(text) ;
                          return(-1) ;
                    }

                    fwrite(data, 1, size, file) ;                   /* ������ ����� */
                    fclose(file) ;                                  /* �������� ����� */

/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                      ������������ �������                         */	

   int  SMEV_form_sign(char *data, char *sign, int sign_size)

{
  FILE *file ;
  char  text[1024] ;
   int  cnt ;
  char *entry ;
  char *end ;

/*--------------------------------------------- ������ ������ � ���� */

         file=fopen(__crypto_sign_in, "wb") ;
      if(file==NULL) {
                          sprintf(text, "ERROR -- Signer input file open error %d : %s", errno, __crypto_sign_in) ;
                        SMEV_show(text) ;
                         SMEV_log(text) ;
                            return(-1) ;
                     }

         cnt=fwrite(data, 1, strlen(data), file) ;
             fclose(file) ;

      if(cnt!=strlen(data)) {
                               sprintf(text, "ERROR -- Signer input file write error %d : %s", errno, __crypto_sign_in) ;
                             SMEV_show(text) ;
                              SMEV_log(text) ;
                                 return(-1) ;
                            }
/*------------------------------------------- �������� ����� ������� */

         cnt=unlink(__crypto_sign_out) ;
      if(cnt==-1 && errno!=ENOENT) {
                    sprintf(text, "ERROR -- Signer output file unlink error %d : %s", errno, __crypto_sign_out) ;
                  SMEV_show(text) ;
                   SMEV_log(text) ;
                      return(-1) ;
                                   }
/*-------------------------------------------- ����� ������� ������� */

         cnt=system(__crypto_sign_exec) ;
      if(cnt!=0) {
                    sprintf(text, "ERROR -- Signer execute fail: return %d, error %d : %s", cnt, errno, __crypto_sign_exec) ;
                  SMEV_show(text) ;
                   SMEV_log(text) ;
                      return(-1) ;
                 }
/*------------------------------------------ ������ ������� �� ����� */

         file=fopen(__crypto_sign_out, "rb") ;
      if(file==NULL) {
                          sprintf(text, "ERROR -- Signer output file open error %d : %s", errno, __crypto_sign_out) ;
                        SMEV_show(text) ;
                         SMEV_log(text) ;
                            return(-1) ;
                     }

            memset(sign, 0, sign_size) ;
         cnt=fread(sign, 1, sign_size, file) ;
            fclose(file) ;

      if(cnt==sign_size) {
                               sprintf(text, "ERROR -- Signer output file too big : %s", __crypto_sign_out) ;
                             SMEV_show(text) ;
                              SMEV_log(text) ;
                                 return(-1) ;
                         }
/*------------------------------------------------ ��������� ������� */

        for(entry=sign, end=sign ; *end!=0 ; ) {

              end=strchr(entry, '\n') ;
           if(end==NULL)  end=entry+strlen(entry) ;
              end++ ;

           if(*entry=='-')  strcpy(entry, end) ;
           else                    entry=end ;
                                               }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                         ��������� � UTF-8                         */
/*                         ��������� �� UTF-8                        */

  void  SMEV_toUTF8(char *data, char *work)

{
   int  cnt ;

#ifdef UNIX

#else
     
      cnt=MultiByteToWideChar(CP_ACP,  0,         data, strlen(data), (LPWSTR)work, strlen(data)) ;
      cnt=WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)work,        cnt,           data, strlen(data)*2, NULL, NULL) ;

                data[cnt]=0 ;

#endif
}

  void  SMEV_fromUTF8(char *data, char *work)

{
   int  cnt ;

#ifdef UNIX

#else
     
      cnt=MultiByteToWideChar(CP_UTF8, 0,         data, strlen(data), (LPWSTR)work, strlen(data)) ;
      cnt=WideCharToMultiByte(CP_ACP,  0, (LPWSTR)work,        cnt,           data, strlen(data), NULL, NULL) ;

                data[cnt]=0 ;

#endif
}
