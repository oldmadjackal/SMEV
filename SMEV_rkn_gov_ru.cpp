/*********************************************************************/
/*                                                                   */
/*             ������� ������ � ��������� ����                       */
/*                                                                   */
/*********************************************************************/

#ifdef UNIX

#include <unistd.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/stat.h>

#else

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <direct.h>
#include <io.h>
#include <time.h>
#include <ctype.h>
#include <process.h>
#include <sys\timeb.h>
#include <sys\types.h>
#include <sys\stat.h>

#endif


#ifdef UNIX

#include "http_tcp.h"

#else

#include "..\grabtp\http_tcp.h"

#include "controls.h"
#include "resource.h"

#endif


#include "SMEV.h"

#pragma warning(disable : 4018)
#pragma warning(disable : 4996)

/*----------------------------------------- ������������� ���������� */

  typedef struct {
                   int  elem ;
                   int  x ;
                   int  y ;
                   int  xs ;
                   int  ys ;
                 } Elem_pos_list ;

/*----------------------------------------------- ������ �� �������� */

#ifndef UNIX

  static  HBITMAP  picMarkUn ;
  static  HBITMAP  picMarkEr ;
  static  HBITMAP  picMarkCg ;
  static  HBITMAP  picMarkCb ;
  static  HBITMAP  picMarkCs ;

#endif

/*------------------------------------ ����������� ��������� ������� */

#ifndef UNIX

  union WndProc_par {
                        long            par ;
                     LRESULT (CALLBACK *call)(HWND, UINT, WPARAM, LPARAM) ; 
                    } ;

  static union WndProc_par  Tmp_WndProc ;

#endif

/*------------------------------------ ��������� ��������� ��������� */

/*---------------------------------------------------- ��������� �/� */

    int  SMEVi_rkn_execute    (void) ;                            /* ���������� ������� */	
   void  SMEVi_rkn_formrequest(char *) ;                          /* ������������ ������� */	
   void  SMEVi_rkn_formSOAP   (char *, char *, char *, char *) ;  /* ������������ SOAP-������ */
    int  SMEVi_rkn_xml2csv    (char *, char *, char *) ;          /* �������������� XML-����� � CSV-���� */


/*********************************************************************/
/*                                                                   */
/*	            ���������� ������� RKN.GOV.RU                    */	
/*                                                                   */
/*    �������� ��������� ACTION:                                     */
/*        CREATE_DIALOG   ������� ������                             */
/*                                                                   */
/*    ������������ ����:                                             */
/*         0  -  ������ �� ��������������                            */
/*         1  -  ������ ������� ���������                            */
/*        -1  -  ������ ��� ��������� �������                        */

   int  SMEV_rkn_exchange(char *action)
{
   char  text[1024] ;

/*------------------------------------------------- ������� �������� */

    if(stricmp(__SMEV_name, "rkn.gov.ru"))  return(0) ;

/*---------------------------------------- �������� ����������� ���� */

   if(!stricmp(action, "CREATE_DIALOG")) {

#ifndef UNIX

           __dialog=CreateDialog(hInst, "IDD_RKN_GOV_RU",
	                                    hFrameWindow, SMEV_rkn_dialog) ;
        if(__dialog==NULL) {
                             sprintf(text, "'rkn.gov.ru' dialog load error %d", GetLastError()) ;
                        SMEV_message(text) ;
                               return(-1) ;
                           }

#endif

                                         }
/*--------------------------- ���������� � ������ ���������� ������� */
   else
   if(!stricmp(action, "SILENT_MODE"  )) {

                      __silent_rc=SMEVi_rkn_execute() ;

                                         }
/*-------------------------------------------------------------------*/

  return(1) ;
}


/*********************************************************************/
/*								     */
/*           ���������� ��������� ����������� ���� RKN_GOV_RU        */	

#ifndef UNIX

  INT_PTR CALLBACK  SMEV_rkn_dialog(  HWND  hDlg,     UINT  Msg, 
                                    WPARAM  wParam, LPARAM  lParam) 
{
       static  int  start_flag=1 ;  /* ���� ������� */
              HWND  hPrn ;
              RECT  wr ;            /* ������� ���� */
	       int  x_screen ;      /* ������� ������ */	
               int  y_screen ;
               int  x_shift ;       /* ���������� ����� */	
               int  y_shift ;
              RECT  Rect_base ;
              RECT  Rect_real ;
              RECT  Rect ;
               int  x0_corr ;
               int  y0_corr ;
               int  x1_corr ;
               int  y1_corr ;
               int  dx ;
               int  dy ;
               int  x_size ;
               int  y_size ;
               int  x ;
               int  y ;
               int  xs ;
               int  ys ;
               int  elm ;            /* ������������� �������� ������� */
               int  sts ;
               int  reply ;
               int  i ; 

                   char  loc_pos_ptr[32] ;
          Elem_pos_list *loc_pos ;
   static Elem_pos_list  loc_pos_e[]={ {IDC_LOG,        0, 0, 1, 1},
                                       {IDC_HIDE,       1, 0, 0, 0},
                                       {IDC_TERMINATE,  1, 0, 0, 0},
                                       {IDC_EXECUTE,    1, 0, 0, 0},
                                       {IDC_VERSION,    1, 1, 0, 0},
                                       {0}                               } ;

     static  HFONT  font ;         /* ����� */
 
/*------------------------------------------------- ������� �������� */

  switch(Msg) {

/*---------------------------------------------------- ������������� */

    case WM_INITDIALOG: {
/*- - - - - - - - - - - - - - - - ���������� ������� �����.��������� */
             loc_pos=(Elem_pos_list *)calloc(1, sizeof(loc_pos_e)) ;
      memcpy(loc_pos, loc_pos_e, sizeof(loc_pos_e)) ;

          sprintf(loc_pos_ptr, "%p", loc_pos) ;                     /* ��������� ������ � �������� ���� */
             SETs(IDC_ELEM_LIST, loc_pos_ptr) ;
/*- - - - - - - - - - - - - - - - - - ���� ��������������� ��������� */
                GetWindowRect(          hDlg,    &Rect_base) ;
                GetWindowRect(ITEM(IDC_TESTPOS), &Rect     ) ;

                      dx     =-(Rect_base.right -Rect.left) ;
                      dy     =-(Rect_base.bottom-Rect.top ) ;
                       x_size=  Rect_base.right -Rect_base.left ;
                       y_size=  Rect_base.bottom-Rect_base.top ;

                 SetWindowPos(ITEM(IDC_TESTPOS), 0,
                               x_size+dx, y_size+dy, 0, 0, 
                                SWP_NOSIZE | SWP_NOZORDER) ;
                GetWindowRect(ITEM(IDC_TESTPOS), &Rect_real) ;

                        x1_corr=Rect.left-Rect_real.left ;
                        y1_corr=Rect.top -Rect_real.top ;

                             dx=Rect.left-Rect_base.left ;
                             dy=Rect.top -Rect_base.top  ;

                 SetWindowPos(ITEM(IDC_TESTPOS), 0,
                                      dx, dy, 0, 0, 
                                SWP_NOSIZE | SWP_NOZORDER) ;
                GetWindowRect(ITEM(IDC_TESTPOS), &Rect_real) ;
                        x0_corr=Rect.left-Rect_real.left ;
                        y0_corr=Rect.top -Rect_real.top ;
/*- - - - - - - - - - - - -  �������� ��������� � �������� ��������� */
     for(i=0 ; loc_pos[i].elem ; i++) {

                GetWindowRect(ITEM(loc_pos[i].elem), &Rect) ;

        if(loc_pos[i].x )  loc_pos[i].x =Rect.left-Rect_base.right+x1_corr ;
        else               loc_pos[i].x =Rect.left-Rect_base.left +x0_corr ;

        if(loc_pos[i].y )  loc_pos[i].y =Rect.top-Rect_base.bottom+y1_corr ;
        else               loc_pos[i].y =Rect.top-Rect_base.top   +y0_corr ;

        if(loc_pos[i].xs)  loc_pos[i].xs= (     Rect.right-Rect.left     ) 
                                         -(Rect_base.right-Rect_base.left) ;
        else               loc_pos[i].xs=Rect.right-Rect.left ;

        if(loc_pos[i].ys)  loc_pos[i].ys= (     Rect.bottom-Rect.top     ) 
                                         -(Rect_base.bottom-Rect_base.top) ;
        else               loc_pos[i].ys=       Rect.bottom-Rect.top ;
                                      }
/*- - - - - - - - - - - - - - - - - - - ������������� ��������� ���� */
       x_screen=GetSystemMetrics(SM_CXSCREEN) ;
       y_screen=GetSystemMetrics(SM_CYSCREEN) ;

           hPrn=GetParent( hDlg) ;
            GetWindowRect( hDlg, &wr) ;

        x_shift=(x_screen-(wr.right-wr.left+1))/2 ;
        y_shift=(y_screen-(wr.bottom-wr.top+1))/2 ;

         AdjustWindowRect(&wr, GetWindowLong(hPrn, GWL_STYLE), false) ;
               MoveWindow( hPrn,  x_shift,
                                  y_shift,
                                 wr.right-wr.left+1,
                                 wr.bottom-wr.top+1, true) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  ������� ������� */
        if(font==NULL)
           font=CreateFont(14, 0, 0, 0, FW_THIN, 
                                 false, false, false,
                                  ANSI_CHARSET,
                                   OUT_DEFAULT_PRECIS,
                                    CLIP_DEFAULT_PRECIS,
                                     DEFAULT_QUALITY,
                                      VARIABLE_PITCH,
                                       "Courier New Cyr") ;
//         SendMessage(ITEM(IDC_LIST_TITLE),    WM_SETFONT, (WPARAM)font, 0) ;
/*- - - - - - - - - - - - - - - - -  ������������� ������� ��������� */
                    SETs(IDC_VERSION, _VERSION) ;
/*- - - - - - - - - - - - - - - - -  �������� ������������ ��������� */
/*- - - - - - - - - - - - - - - - - - - -  ������������� ����������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*-------------------------------------- ��������� ��������� ������� */

    case WM_SIZE: {
                         GETs(IDC_ELEM_LIST, loc_pos_ptr) ;         /* ��������� ������ ���������������� */ 
                       sscanf(loc_pos_ptr, "%p", &loc_pos) ;

                GetWindowRect(hDlg, &Rect_base) ;

                       x_size=Rect_base.right -Rect_base.left ;
                       y_size=Rect_base.bottom-Rect_base.top ;

     for(i=0 ; loc_pos[i].elem ; i++) {

        if(loc_pos[i].x<0)  x =x_size+loc_pos[i].x ;
        else                x =       loc_pos[i].x ;
        
        if(loc_pos[i].y<0)  y =y_size+loc_pos[i].y ;
        else                y =       loc_pos[i].y ;

        if(loc_pos[i].xs<0) xs=x_size+loc_pos[i].xs ;
        else                xs=       loc_pos[i].xs ;

        if(loc_pos[i].ys<0) ys=y_size+loc_pos[i].ys ;
        else                ys=       loc_pos[i].ys ;

           SetWindowPos(ITEM(loc_pos[i].elem),  0,
                                x, y, xs, ys, 
                                  SWP_NOZORDER | SWP_NOCOPYBITS) ;
                                      }

			  return(FALSE) ;
  			     break ;
  		  }
/*------------------------------------ ��������� ���������� �������� */

    case WM_USER:  {
			  return(FALSE) ;
  			     break ;
  		   }
/*------------------------------------------------ ��������� ������� */

    case WM_COMMAND: {

                   sts=HIWORD(wParam) ;
	           elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - ��������� ������ */
       if(elm==IDC_EXECUTE) {
                                 LB_CLEAR(IDC_LOG) ;

                                SMEVi_rkn_execute() ;
			            return(FALSE) ;
                            }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - -  ������ ���� */
       if(elm==IDC_HIDE) {
        		    ShowWindow(hFrameWindow, SW_HIDE) ;
//		            ShowWindow(hMainDialog, SW_HIDE) ;
                                           __window_closed=1 ;

			        return(FALSE) ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - ��������� ������ */
       if(elm==IDC_TERMINATE) {

           reply=MessageBox(hDlg, "�� ������������� ������ ��������� ���������?",
                                    "", MB_YESNO | MB_ICONQUESTION) ;
        if(reply==IDNO)  return(FALSE) ;

			                EndDialog(hDlg, TRUE) ;
  			          PostQuitMessage(0) ;  

                                   __exit_flag=1 ;
                              }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
	             }
/*--------------------------------------------------------- �������� */

    case WM_CLOSE:      {
/*- - - - - - - - - - - - - - - - - - - - - -  ������������ �������� */
                             GETs(IDC_ELEM_LIST, loc_pos_ptr) ;     /* ��������� ������ ���������������� */ 
                           sscanf(loc_pos_ptr, "%p", &loc_pos) ;
                             free(loc_pos) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			      return(FALSE) ;
			          break ;
			}
/*----------------------------------------------------------- ������ */

//  case WM_PAINT:    break ;

    default :        {
			  return(FALSE) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }
/*-------------------------------------------------------------------*/

    return(TRUE) ;
}

#endif


/*********************************************************************/
/*                                                                   */
/*	                ���������� �������             	             */	

   int  SMEVi_rkn_execute(void)
{
    char *request ;
    char *sign ;
    char *SOAP ;
    char *send[2] ;
    char  timestamp_1[1024] ;  /* ����� ���������� �� ���� lastDumpDate */
    char  timestamp_2[1024] ;  /* ����� ���������� �� ���� lastDumpDateUrgently */
    char  result[1024] ;
    char  result_code[1024] ;
    char  comment[1024] ;
    char  data_id[1024] ;
    char  text[1024] ;
    char *entry ;
    char *end ;
     int  status ;
  time_t  time_0 ;
     int  rc ;

#define  _SMALL_BUFF_SIZE      256000
#define  _LARGE_BUFF_SIZE    16000000

/*------------------------------------------------------- ���������� */

         request=(char *)calloc(1, _LARGE_BUFF_SIZE) ;
            sign=(char *)calloc(1, _SMALL_BUFF_SIZE) ;
            SOAP=(char *)calloc(1, _LARGE_BUFF_SIZE) ;

/*--------------------------------------------- ������� ������� ���� */

                  rc=_SMEV_ERROR_RC ;

   do {                                                             /* BLOCK */

/*---------------------------------------------- ����������� ������� */

                      SMEV_show("\n������ ���� ����������\n") ;
/*- - - - - - - - - - - - - - - -  ������������ ������������ ������� */
             SMEVi_rkn_formSOAP(SOAP, "CHECK", NULL, NULL) ;        /* ������������ SOAP-������ */
                      SMEV_show(SOAP) ;
/*- - - - - - - - - - - - - - - - - -  �������� ������������ ������� */
            strcpy(request, SOAP) ;                                 /* ��������� ������ */

                    send[0]=SOAP ;
                    send[1]=NULL ;

                                 sprintf(text, "\r\n�������� ������� �� %s ...", __SMEV_url) ;
                               SMEV_show(text) ;

          status=HTTP_tcp::SOAP_exchange(__SMEV_url, "anonymous",   /* �������� ������ */
                                               send, SOAP, _LARGE_BUFF_SIZE-2, text) ;
       if(status) {                                                 /* ���� ������... */
                      SMEV_log("������ ������:") ;
                     SMEV_show("������ ������:") ;
                      SMEV_log( text) ;
                     SMEV_show( text) ;
                  }

       if(!strcmp(request, SOAP))  break ;
/*- - - - - - - - - - - - - - - - - - - - - - -  ��������� ��������� */
     if(strstr(SOAP, " 302 ")!=NULL ||
        strstr(SOAP, " 307 ")!=NULL   ) {

             SMEV_show("\r\n���������� ���������������") ;
              SMEV_log("\r\n���������� ���������������") ;

              entry=strstr(SOAP, "Location: ") ;
           if(entry==NULL) {
                              SMEV_show("����� ��������������� � ������ �� ������:") ;
                              SMEV_log ("����� ��������������� � ������ �� ������:") ;
                              SMEV_show(SOAP) ;
                              SMEV_log (SOAP) ;
                                break ;
                           }

                  memset(result, 0, sizeof(result)) ;
                 strncpy(result, entry+strlen("Location: "), sizeof(result)-1) ;
              end=strchr(result, ' ') ;
           if(end!=NULL)  *end=0 ;
              end=strchr(result, '\r') ;
           if(end!=NULL)  *end=0 ;
              end=strchr(result, '\n') ;
           if(end!=NULL)  *end=0 ;

               SMEV_text_subst(__SMEV_url, "/services/OperatorRequest/", result) ;
               SMEV_text_subst(   request, "/services/OperatorRequest/", result) ;

            strcpy(SOAP, request) ;                                 /* ��������������� ������ */

                                 sprintf(text, "\r\n�������� ������� �� %s ...", __SMEV_url) ;
                               SMEV_show(text) ;

          status=HTTP_tcp::SOAP_exchange(__SMEV_url, "anonymous",   /* �������� ������ */
                                               send, SOAP, _LARGE_BUFF_SIZE-2, text) ;
       if(status) {                                                 /* ���� ������... */
                      SMEV_log("������ ������:") ;
                     SMEV_show("������ ������:") ;
                      SMEV_log( text) ;
                     SMEV_show( text) ;
                  }

       if(!strcmp(request, SOAP))  break ;

                                        }
/*- - - - - - - - - - - - - - -  ������ ������ �� ����������� ������ */
             SMEV_show("\r\n����� �������:") ;
              SMEV_log("\r\n����� �������:") ;

         SMEV_fromUTF8(SOAP, sign) ;

             SMEV_show(SOAP) ;
              SMEV_log(SOAP) ;

                memset(timestamp_1, 0, sizeof(timestamp_1)) ;

          entry=strstr(SOAP, "<lastDumpDate>") ;
       if(entry==NULL) {
                         SMEV_show("��� <lastDumpDate> � ������ �� ������.") ;
                          SMEV_log("��� <lastDumpDate> � ������ �� ������.") ;
                             break ;   
                       }

               strncpy(timestamp_1, entry+strlen("<lastDumpDate>"), sizeof(timestamp_1)-1) ;
          end = strchr(timestamp_1, '<') ;
       if(end!=NULL)  *end=0 ;

                      sprintf(text, "\r\nlastDumpDate: %s", timestamp_1) ;
                    SMEV_show(text) ;

                memset(timestamp_2, 0, sizeof(timestamp_2)) ;

          entry=strstr(SOAP, "<lastDumpDateUrgently>") ;
       if(entry==NULL) {
                         SMEV_show("��� <lastDumpDateUrgently> � ������ �� ������.") ;
                          SMEV_log("��� <lastDumpDateUrgently> � ������ �� ������.") ;
                             break ;   
                       }

               strncpy(timestamp_2, entry+strlen("<lastDumpDateUrgently>"), sizeof(timestamp_2)-1) ;
          end = strchr(timestamp_2, '<') ;
       if(end!=NULL)  *end=0 ;

                      sprintf(text, "\r\nlastDumpDateUrgently: %s", timestamp_2) ;
                    SMEV_show(text) ;

/*----------------------------------- �������� ������������� ������� */

                    strcat(timestamp_1, timestamp_2) ;              /* ��������� ��������������� ����� */

               SMEV_cookie("READ", text, sizeof(text)-1) ;

         if(!stricmp(timestamp_1, text)) {
                   SMEV_show("\n������ �� ������� ����������\n") ;
                    SMEV_log("\n������ �� ������� ����������\n") ;
                                      rc=_SMEV_UPTODATE_RC ;
                                                 break ;
                                       }
/*------------------------------------------------ ��������� ������� */

                      SMEV_show("\n������ ��������������� ���� ������\n") ;
/*- - - - - - - - - - - - - - - - -  ������������ ���������� ������� */
          SMEVi_rkn_formrequest(request) ;                          /* ��������� ������ */
                      SMEV_show(request) ;

          status=SMEV_form_sign(request, sign, _SMALL_BUFF_SIZE-1); /* ������������ ������� ������� */
#ifndef  UNIX
                    SMEV_system(NULL) ;
#endif
       if(status)  break ;

                         memset(SOAP, 0, sizeof(_LARGE_BUFF_SIZE)) ;
        HTTP_tcp::Base64_incode(request, SOAP, strlen(request)) ;
                         strcpy(request, SOAP) ;

             SMEVi_rkn_formSOAP(SOAP, "REQUEST", request, sign) ;   /* ������������ SOAP-������ */
                      SMEV_show(SOAP) ;
                       SMEV_log(SOAP) ;
/*- - - - - - - - - - - - - - - - - - -  �������� ���������� ������� */
            strcpy(request, SOAP) ;                                 /* ��������� ������ */

                    send[0]=SOAP ;
                    send[1]=NULL ;

                                 sprintf(text, "\r\n�������� ������� �� %s ...", __SMEV_url) ;
                               SMEV_show(text) ;

          status=HTTP_tcp::SOAP_exchange(__SMEV_url, "anonymous",   /* �������� ������ */
                                               send, SOAP, _LARGE_BUFF_SIZE, text) ;
       if(status) {                                                 /* ���� ������... */
                      SMEV_log("������ ������:") ;
                     SMEV_show("������ ������:") ;
                      SMEV_log( text) ;
                     SMEV_show( text) ;
                  }

       if(!strcmp(request, SOAP))  break ;
/*- - - - - - - - - - - - - - - -  ������ ������ �� ��������� ������ */
             SMEV_show("\r\n����� �������:") ;
              SMEV_log("\r\n����� �������:") ;
             SMEV_show(SOAP) ;
              SMEV_log(SOAP) ;

                memset(result,  0, sizeof(result )) ;
                memset(comment, 0, sizeof(comment)) ;
                memset(data_id, 0, sizeof(data_id)) ;

          entry=strstr(SOAP, "<result>") ;
       if(entry==NULL) {
                         SMEV_show("��� <result> � ������ �� ������.") ;
                          SMEV_log("��� <result> � ������ �� ������.") ;
                             break ;   
                       }

               strncpy(result, entry+strlen("<result>"), sizeof(result)-1) ;
          end = strchr(result, '<') ;
       if(end!=NULL)  *end=0 ;

                      sprintf(text, "\r\nresult       : %s", result) ;
                    SMEV_show(text) ;

          entry=strstr(SOAP, "<resultComment>") ;
       if(entry!=NULL) {
                 strncpy(comment, entry+strlen("<resultComment>"), sizeof(comment)-4) ;
            end = strchr(comment, '<') ;
         if(end!=NULL)  *end=0 ;

           SMEV_fromUTF8(comment, sign) ;
                       }
                          
                      sprintf(text, "resultComment: %s", comment) ;
                    SMEV_show(text) ;

       if(stricmp(result, "true"))  break ;
     
          entry=strstr(SOAP, "<code>") ;
       if(entry==NULL) {
                         SMEV_show("��� <code> � ������ �� ������.") ;
                          SMEV_log("��� <code> � ������ �� ������.") ;
                             break ;   
                       }

               strncpy(data_id, entry+strlen("<code>"), sizeof(data_id)-1) ;
          end = strchr(data_id, '<') ;
       if(end!=NULL)  *end=0 ;

                      sprintf(text, "code         : %s", data_id) ;
                    SMEV_show(text) ;

/*---------------------------------- ������������ ���������� ������� */

                      SMEV_show("\n������ ������\n") ;

             SMEVi_rkn_formSOAP(SOAP, "HOOK", data_id, NULL) ;      /* ������������ SOAP-������ */
                      SMEV_show(SOAP) ;

                         strcpy(request, SOAP) ;                    /* ��������� ������ */

/*----------------------- ���� �������� ��������� ���������� ������� */

      while(1) {                                                    /* LOOP */
/*- - - - - - - - - - - - - - - - - - -  �������� ���������� ������� */
            strcpy(SOAP, request) ;                                 /* ��������������� ������ */

                    send[0]=SOAP ;
                    send[1]=NULL ;

                                 sprintf(text, "\r\n�������� ������� �� %s ...", __SMEV_url) ;
                               SMEV_show(text) ;

          status=HTTP_tcp::SOAP_exchange(__SMEV_url, "anonymous",   /* �������� ������ */
                                               send, SOAP, _LARGE_BUFF_SIZE, text) ;
       if(status) {                                                 /* ���� ������... */
                      SMEV_log("������ ������:") ;
                     SMEV_show("������ ������:") ;
                      SMEV_log( text) ;
                     SMEV_show( text) ;
                  }

       if(!strcmp(request, SOAP))  break ;
/*- - - - - - - - - - - - - - - -  ������ ������ �� ��������� ������ */
                       text[   0]=SOAP[2048] ;
                       SOAP[2048]= 0 ;

             SMEV_show("\r\n����� �������:") ;
              SMEV_log("\r\n����� �������:") ;
             SMEV_show(SOAP) ;
              SMEV_log(SOAP) ;

                       SOAP[2048]=text[0] ;

                memset(result,  0, sizeof(result )) ;
                memset(comment, 0, sizeof(comment)) ;

          entry=strstr(SOAP, "<result>") ;
       if(entry==NULL) {
                         SMEV_show("��� <result> � ������ �� ������.") ;
                          SMEV_log("��� <result> � ������ �� ������.") ;
                             break ;   
                       }

               strncpy(result, entry+strlen("<result>"), sizeof(result)-1) ;
          end = strchr(result, '<') ;
       if(end!=NULL)  *end=0 ;

                      sprintf(text, "\r\nresult       : %s", result) ;
                    SMEV_show(text) ;

          entry=strstr(SOAP, "<resultComment>") ;
       if(entry!=NULL) {
                 strncpy(comment, entry+strlen("<resultComment>"), sizeof(comment)-4) ;
            end = strchr(comment, '<') ;
         if(end!=NULL)  *end=0 ;

           SMEV_fromUTF8(comment, sign) ;
                       }
                          
                      sprintf(text, "resultComment: %s", comment) ;
                    SMEV_show(text) ;

          entry=strstr(SOAP, "<resultCode>") ;
       if(entry!=NULL) {
                 strncpy(result_code, entry+strlen("<resultCode>"), sizeof(result_code)-1) ;
            end = strchr(result_code, '<') ;
         if(end!=NULL)  *end=0 ;
                       }
                          
                      sprintf(text, "resultCode   : %s", result_code) ;
                    SMEV_show(text) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - ��������� ������ */
       if(!stricmp(result,  "true")) {

              entry=strstr(SOAP, "<registerZipArchive>") ;
           if(entry==NULL) {
                              SMEV_show("��� <registerZipArchive> � ������ �� ������.") ;
                               SMEV_log("��� <registerZipArchive> � ������ �� ������.") ;
                                     break ;   
                           }

                           entry+=strlen("<registerZipArchive>") ; 
              end  =strstr(entry, "</registerZipArchive>") ;
           if(end  ==NULL) {
                              SMEV_show("��� <registerZipArchive> � ������ �� ��������.") ;
                               SMEV_log("��� <registerZipArchive> � ������ �� ��������.") ;
                                     break ;   
                           }

                                            *end=0 ;  
              status=HTTP_tcp::Base64_decode(entry, request) ;
           if(status<0) {
                              SMEV_show("������ ���� <registerZipArchive> �����������.") ;
                               SMEV_log("������ ���� <registerZipArchive> �����������.") ;
                                     break ;   
                        }

              status=SMEV_result(request, status) ;
           if(status) {
                              SMEV_show("������ ������ ����� ����������.") ;
                               SMEV_log("������ ������ ����� ����������.") ;
                                     break ;   
                      }

               SMEV_cookie("SAVE", timestamp_1, strlen(timestamp_1)) ;
                                        rc=0 ;
                                           break ;
                                     }

       if(stricmp(result_code, "0"))  break ;
/*- - - - - - - - - - - - - - - - - - - - - -  ��������������� ����� */
              SMEV_show("��������� ������ ����� 30 ������...") ;

                   time_0=time(NULL) ;

           do {               
#ifndef  UNIX
                    SMEV_system(NULL) ;
#endif
                          Sleep(100) ;

              } while(time(NULL)-time_0<30) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
               }                                                    /* END LOOP */
/*--------------------------------------------- ������� ������� ���� */

      } while(0) ;                                                  /* BLOCK */

                 SMEV_show("\r\n����� ��������.") ;

/*-------------------------------------------- ������������ �������� */

                      free(request) ;
                      free(sign) ;
                      free(SOAP) ;

/*------------------------------------------ ���������� ����� ������ */

                 SMEV_show("\r\n���������� ����� ������...") ;

       status=system(__unzip_command) ;
    if(status!=0 || errno!=0) {
                 sprintf(text, "Unzip execute fail (status=%d, errno=%d) : %s", status, errno, __unzip_command) ;
               SMEV_show(text) ;
                SMEV_log(text) ;
                  return(-1) ;
                              }

                 SMEV_show("���������� ����� ������ ���������") ;

/*-------------------------------------- �������������� ����� ������ */

                 SMEV_show("\r\n�������������� ����� ������...") ;

       status=SMEVi_rkn_xml2csv("dump.xml", "blacklist.csv", text) ;
    if(status) {
                  SMEV_show(text) ;
                   SMEV_log(text) ;
                      return(-1) ;
              }

                 SMEV_show("\r\n�������������� ����� ������ ���������") ;

/*-------------------------------------------------------------------*/

#undef  _SMALL_BUFF_SIZE
#undef  _LARGE_BUFF_SIZE

  return(status) ;
}


/*********************************************************************/
/*                                                                   */
/*	              ������������ �������             	             */	

   void  SMEVi_rkn_formrequest(char *request)
{
     struct tm *hhmmss ;
        time_t  time_stamp ;
          char  timestamp[32] ;
          char *value ;
  static  char *request_template=
"<?xml version=\"1.0\" encoding=\"windows-1251\"?>\r\n"
"<request>\r\n"
"<requestTime>$TIMESTAMP$</requestTime>\r\n"
"<operatorName>$OPERATOR_NAME$</operatorName>\r\n"
"<inn>$INN$</inn>\r\n"
"<ogrn>$OGRN$</ogrn>\r\n"
"<email>$EMAIL$</email>\r\n"
"</request>\r\n\r\n" ;

/*----------------------------------------------- ���������� ������� */

            strcpy(request, request_template) ;

/*------------------------------------- ������������ ��������� ����� */

                     time(&time_stamp) ;
         hhmmss=localtime(&time_stamp) ;

            sprintf(timestamp, "20%02d-%02d-%02dT%02d:%02d:%02d.000+04:00", 
                            hhmmss->tm_year-100, hhmmss->tm_mon+1, hhmmss->tm_mday,
                            hhmmss->tm_hour,     hhmmss->tm_min,   hhmmss->tm_sec  ) ;

        value=SMEV_get_parameter("TIMESTAMP") ;
     if(value==NULL)  value=timestamp ;

                 SMEV_text_subst(request, "$TIMESTAMP$", value) ;

/*------------------------------------------- ����������� ���������� */

        value=SMEV_get_parameter("OPERATOR_NAME") ;
                 SMEV_text_subst(request, "$OPERATOR_NAME$", value) ;

        value=SMEV_get_parameter("INN") ;
                 SMEV_text_subst(request, "$INN$", value) ;

        value=SMEV_get_parameter("OGRN") ;
                 SMEV_text_subst(request, "$OGRN$", value) ;

        value=SMEV_get_parameter("EMAIL") ;
                 SMEV_text_subst(request, "$EMAIL$", value) ;

/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*	              ������������ SOAP-������         	             */	

   void  SMEVi_rkn_formSOAP(char *SOAP, char *action, char *request, char *sign)
{
  static  char *REQUEST_template=
"<?xml version=\"1.0\" encoding=\"windows-1251\"?> \r\n"
"<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:oper=\"http://vigruzki.rkn.gov.ru/OperatorRequest/\"> \r\n"
"   <soapenv:Header/> \r\n"
"   <soapenv:Body> \r\n"
"      <oper:sendRequest> \r\n"
"         <requestFile>$REQUEST$></requestFile> \r\n"
"         <signatureFile>$SIGN$</signatureFile> \r\n"
"         <dumpFormatVersion>2.0</dumpFormatVersion> \r\n"
"      </oper:sendRequest> \r\n"
"   </soapenv:Body> \r\n"
"</soapenv:Envelope> \r\n" ;
  static  char *HOOK_template=
"<?xml version=\"1.0\" encoding=\"windows-1251\"?> \r\n"
"<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:oper=\"http://vigruzki.rkn.gov.ru/OperatorRequest/\"> \r\n"
"   <soapenv:Header/> \r\n"
"   <soapenv:Body> \r\n"
"      <oper:getResult> \r\n"
"         <code>$DATA_ID$</code> \r\n"
"      </oper:getResult> \r\n"
"   </soapenv:Body> \r\n"
"</soapenv:Envelope> \r\n" ;
  static  char *CHECK_template=
"<?xml version=\"1.0\" encoding=\"windows-1251\"?> \r\n"
"<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:oper=\"http://vigruzki.rkn.gov.ru/OperatorRequest/\"> \r\n"
"   <soapenv:Header/> \r\n"
"   <soapenv:Body> \r\n"
"      <oper:getLastDumpDateEx> \r\n"
"      </oper:getLastDumpDateEx> \r\n"
"   </soapenv:Body> \r\n"
"</soapenv:Envelope> \r\n" ;


   if(!stricmp(action, "CHECK"  )) {
                                        strcpy(SOAP, CHECK_template) ;
                                   }
   else
   if(!stricmp(action, "REQUEST")) {
                                        strcpy(SOAP, REQUEST_template) ;

                               SMEV_text_subst(SOAP, "$REQUEST$", request) ;
                               SMEV_text_subst(SOAP, "$SIGN$",    sign   ) ;
                                   }
   else
   if(!stricmp(action, "HOOK"   )) {
                                        strcpy(SOAP, HOOK_template) ;

                               SMEV_text_subst(SOAP, "$DATA_ID$", request) ;
                                   }

}


/*********************************************************************/
/*								     */
/*               �������������� XML-����� � CSV-����                 */

   int  SMEVi_rkn_xml2csv(char *xml_path, char *csv_path, char *error)

{
       FILE *xml_file ;
       FILE *csv_file ;
       char *buff ;
       char *frame ;
       char *record ;
        int  cnt ;
       char *entry ;
       char *content ;
       char *content_end ;
       char *url ;
       char *domain ;
       char *ip ; 
       char *end ;
       char *c ;
       long  content_cnt ;
       long  ip_cnt ;
       char  text[128] ;

#define   _FRAME_SIZE  256000

/*------------------------------------------------- ������� �������� */

     if(xml_path[0]==0) {
                     sprintf(error, "�� ������ ��� XML-�����") ;
                           return(-1) ;
                        }
     if(csv_path[0]==0) {
                     sprintf(error, "�� ������ ��� CSV-�����") ;
                           return(-1) ;
                        }
/*-------------------------------------------------- �������� ������ */

        csv_file=fopen(csv_path, "wb") ;
     if(csv_file==NULL) {
                   sprintf(error, "������ �������� SCV-����� %d :%s", errno, csv_path) ;
                           return(-1) ;
                        }
        xml_file=fopen(xml_path, "rb") ;
     if(xml_file==NULL) {
                   sprintf(error, "������ �������� XML-����� %d :%s", errno, xml_path) ;
                           return(-1) ;
                        }
/*----------------------------------------- �������������� XML-����� */

         buff=(char *)calloc(1, 2*_FRAME_SIZE) ;
        frame=(char *)calloc(1,   _FRAME_SIZE) ;
       record=(char *)calloc(1,   _FRAME_SIZE) ;

                 *error=0 ;

            content_cnt=0 ; 
                 ip_cnt=0 ; 

    do {
/*- - - - - - - - - - - - - - - - - - -  ���������� ���������� ����� */
            memset(frame, 0, _FRAME_SIZE) ;
         cnt=fread(frame, 1, _FRAME_SIZE-1, xml_file) ;

            strcat(buff, frame) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  ��������� ����� */
        for(content=buff ; ; content=content_end+1) {

                 *record=  0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - -  ��� CONTENT */
             content_end=NULL ;

            entry=strstr(content, "<content") ;
         if(entry==NULL)  break ;

            content    =entry ;
            content_end=strstr(content, "</content>") ;
         if(content_end==NULL)  break ;

           *content_end=0 ;

            content_cnt++ ; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - -  ��� URL */
            url=strstr(content, "<url") ;
         if(url!=NULL) {
                          url =strstr(url, "![CDATA[")+strlen("![CDATA[") ;
                          end =strchr(url, ']') ;
                         *end = 0 ;
                    strcat(record, url) ;
                    strcat(record, ";") ;
                         *end =' ' ;
                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ��� DOMAIN */
            domain=strstr(content, "<domain") ;
         if(domain==NULL) {
                             strcat(record, ";;") ;
                          }
         else             {

            domain =strstr(domain, "![CDATA[")+strlen("![CDATA[") ;
               end =strchr(domain, ']') ;
              *end = 0 ;

           if(url==NULL) {
                             strcat(record, domain) ;
                             strcat(record, ";") ;
                         }

                             strcat(record, domain) ;
                             strcat(record, ";") ;
                          }

                         *end =' ' ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ��� IP */
     while(1) {
                  
            ip=strstr(content, "<ip") ;
         if(ip==NULL)  break ;

           *ip =' ' ;
            ip =strchr(ip, '>')+1 ;
           end =strchr(ip, '<') ;
          *end = 0 ;

         for(c=ip ; *c ; c++) if(*c==',')  *c='.' ;

                 strcat(record, ip) ;
                 strcat(record, ";") ;

                     *end=' ' ;
                   ip_cnt++ ; 
              }

                 strcat(record, "\n") ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ������ CSV */
            fwrite(record, 1, strlen(record), csv_file) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  ��������� ����� */
                                                    }

              if(*error!=0)  break ;

              if(content_end!=NULL)  memmove(buff, content_end, strlen(content_end)+1) ;
         else if(content    !=buff)  memmove(buff, content,     strlen(content    )+1) ;
/*- - - - - - - - - - - - - - - - - - -  ���������� ���������� ����� */
       } while(cnt>0) ;

              free(record) ;
              free(frame) ;
              free(buff) ;

/*-------------------------------------------------- �������� ������ */

                    fclose(xml_file) ;
                    fclose(csv_file) ;

/*-------------------------------------------------------------------*/

         sprintf(text, "Processed: %ld records, %ld ip", content_cnt, ip_cnt) ;
       SMEV_show(text) ;
        SMEV_log(text) ;

  if(*error!=0)  return(-1) ;

    return(0) ;
}


