/*********************************************************************/
/*                                                                   */
/*             Утилита обмена с сервисами СМЭВ                       */
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

/*----------------------------------------- Общесистемные переменные */

  typedef struct {
                   int  elem ;
                   int  x ;
                   int  y ;
                   int  xs ;
                   int  ys ;
                 } Elem_pos_list ;

/*----------------------------------------------- Ссылки на картинки */

#ifndef UNIX

  static  HBITMAP  picMarkUn ;
  static  HBITMAP  picMarkEr ;
  static  HBITMAP  picMarkCg ;
  static  HBITMAP  picMarkCb ;
  static  HBITMAP  picMarkCs ;

#endif

/*------------------------------------ Обработчики элементов диалога */

#ifndef UNIX

  union WndProc_par {
                        long            par ;
                     LRESULT (CALLBACK *call)(HWND, UINT, WPARAM, LPARAM) ; 
                    } ;

  static union WndProc_par  Tmp_WndProc ;

#endif

/*------------------------------------ Процедуры обработки сообщений */

/*---------------------------------------------------- Прототипы п/п */

    int  SMEVi_rkn_execute    (void) ;                            /* Исполнение запроса */	
   void  SMEVi_rkn_formrequest(char *) ;                          /* Формирование запроса */	
   void  SMEVi_rkn_formSOAP   (char *, char *, char *, char *) ;  /* Формирование SOAP-пакета */
    int  SMEVi_rkn_xml2csv    (char *, char *, char *,            /* Преобразование XML-файла в CSV-файл */
                               char *, char *, char *,
                               char *, char *, char *,
                               char *, char *         ) ;


/*********************************************************************/
/*                                                                   */
/*	            Обработчик сервиса RKN.GOV.RU                    */	
/*                                                                   */
/*    Значение параметра ACTION:                                     */
/*        CREATE_DIALOG   Создать диалог                             */
/*                                                                   */
/*    Возвращаемые коды:                                             */
/*         0  -  сервис не поддерживается                            */
/*         1  -  сервис успешно обработан                            */
/*        -1  -  ошибка при обработке сервиса                        */

   int  SMEV_rkn_exchange(char *action)
{
#ifndef UNIX
   char  text[1024] ;
#endif

/*------------------------------------------------- Входной контроль */

    if(stricmp(__SMEV_name, "rkn.gov.ru"))  return(0) ;

/*---------------------------------------- Создание диалогового окна */

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
/*--------------------------- Исполнение в режиме консольной команды */
   else
   if(!stricmp(action, "SILENT_MODE"  )) {

                      __silent_rc=SMEVi_rkn_execute() ;

                                         }
/*-------------------------------------------------------------------*/

  return(1) ;
}


/*********************************************************************/
/*								     */
/*           Обработчик сообщений диалогового окна RKN_GOV_RU        */	

#ifndef UNIX

  INT_PTR CALLBACK  SMEV_rkn_dialog(  HWND  hDlg,     UINT  Msg, 
                                    WPARAM  wParam, LPARAM  lParam) 
{
       static  int  start_flag=1 ;  /* Флаг запуска */
              HWND  hPrn ;
              RECT  wr ;            /* Габарит окна */
	       int  x_screen ;      /* Габарит экрана */	
               int  y_screen ;
               int  x_shift ;       /* Центрующий сдвиг */	
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
               int  elm ;            /* Идентификатор элемента диалога */
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

     static  HFONT  font ;         /* Шрифт */
 
/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {
/*- - - - - - - - - - - - - - - - Размещение массива позиц.элементов */
             loc_pos=(Elem_pos_list *)calloc(1, sizeof(loc_pos_e)) ;
      memcpy(loc_pos, loc_pos_e, sizeof(loc_pos_e)) ;

          sprintf(loc_pos_ptr, "%p", loc_pos) ;                     /* Сохраняем массив в элементе окна */
             SETs(IDC_ELEM_LIST, loc_pos_ptr) ;
/*- - - - - - - - - - - - - - - - - - Тест координирования элементов */
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
/*- - - - - - - - - - - - -  Фиксация положения и размеров элементов */
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
/*- - - - - - - - - - - - - - - - - - - Подравнивание рамочного окна */
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
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Пропись шрифтов */
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
/*- - - - - - - - - - - - - - - - -  Инициализация значеий элементов */
                    SETs(IDC_VERSION, _VERSION) ;
/*- - - - - - - - - - - - - - - - -  Перехват обработчиков сообщений */
/*- - - - - - - - - - - - - - - - - - - -  Инициализация доступности */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*-------------------------------------- Отработка изменения размера */

    case WM_SIZE: {
                         GETs(IDC_ELEM_LIST, loc_pos_ptr) ;         /* Извлекаем массив позиционирования */ 
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
/*------------------------------------ Отработка внутренних сообений */

    case WM_USER:  {
			  return(FALSE) ;
  			     break ;
  		   }
/*------------------------------------------------ Отработка событий */

    case WM_COMMAND: {

                   sts=HIWORD(wParam) ;
	           elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - Исполнить запрос */
       if(elm==IDC_EXECUTE) {
                                 LB_CLEAR(IDC_LOG) ;

                                SMEVi_rkn_execute() ;
			            return(FALSE) ;
                            }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - -  Скрыть окно */
       if(elm==IDC_HIDE) {
        		    ShowWindow(hFrameWindow, SW_HIDE) ;
//		            ShowWindow(hMainDialog, SW_HIDE) ;
                                           __window_closed=1 ;

			        return(FALSE) ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - Завершить работу */
       if(elm==IDC_TERMINATE) {

           reply=MessageBox(hDlg, "Вы действительно хотите завершить программу?",
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
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {
/*- - - - - - - - - - - - - - - - - - - - - -  Освобождение ресурсов */
                             GETs(IDC_ELEM_LIST, loc_pos_ptr) ;     /* Извлекаем массив позиционирования */ 
                           sscanf(loc_pos_ptr, "%p", &loc_pos) ;
                             free(loc_pos) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			      return(FALSE) ;
			          break ;
			}
/*----------------------------------------------------------- Прочее */

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
/*	                Исполнение запроса             	             */	

   int  SMEVi_rkn_execute(void)
{
    char *request ;
    char *sign ;
    char *SOAP ;
    char *send[2] ;
    char  timestamp_1[1024] ;  /* Время обновления по тэгу lastDumpDate */
    char  timestamp_2[1024] ;  /* Время обновления по тэгу lastDumpDateUrgently */
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

/*------------------------------------------------------- Подготовка */

         request=(char *)calloc(1, _LARGE_BUFF_SIZE) ;
            sign=(char *)calloc(1, _SMALL_BUFF_SIZE) ;
            SOAP=(char *)calloc(1, _LARGE_BUFF_SIZE) ;

/*--------------------------------------------- Главный рабочий блок */

                  rc=_SMEV_ERROR_RC ;

   do {                                                             /* BLOCK */

/*---------------------------------------------- Контрольный запроса */

                      SMEV_show("\nЗАПРОС ДАТЫ ОБНОВЛЕНИЯ\n") ;
/*- - - - - - - - - - - - - - - -  Формирование контрольного запроса */
             SMEVi_rkn_formSOAP(SOAP, "CHECK", NULL, NULL) ;        /* Формирование SOAP-пакета */
                      SMEV_show(SOAP) ;
/*- - - - - - - - - - - - - - - - - -  Отправка контрольного запроса */
            strcpy(request, SOAP) ;                                 /* Сохраняем запрос */

                    send[0]=SOAP ;
                    send[1]=NULL ;

                                 sprintf(text, "\r\nОтправка запроса на %s ...", __SMEV_url) ;
                               SMEV_show(text) ;

          status=HTTP_tcp::SOAP_exchange(__SMEV_url, "anonymous",   /* Передаем запрос */
                                               send, SOAP, _LARGE_BUFF_SIZE-2, text) ;
       if(status) {                                                 /* Если ошибка... */
                      SMEV_log("Ошибка обмена:") ;
                     SMEV_show("Ошибка обмена:") ;
                      SMEV_log( text) ;
                     SMEV_show( text) ;
                  }

       if(!strcmp(request, SOAP))  break ;
/*- - - - - - - - - - - - - - - - - - - - - - -  Обработка редиректа */
     if(strstr(SOAP, " 302 ")!=NULL ||
        strstr(SOAP, " 307 ")!=NULL   ) {

             SMEV_show("\r\nОбнаружено перенаправление") ;
              SMEV_log("\r\nОбнаружено перенаправление") ;

              entry=strstr(SOAP, "Location: ") ;
           if(entry==NULL) {
                              SMEV_show("Адрес перенаправления в ответе не найден:") ;
                              SMEV_log ("Адрес перенаправления в ответе не найден:") ;
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

            strcpy(SOAP, request) ;                                 /* Восстанавливаем запрос */

                                 sprintf(text, "\r\nОтправка запроса на %s ...", __SMEV_url) ;
                               SMEV_show(text) ;

          status=HTTP_tcp::SOAP_exchange(__SMEV_url, "anonymous",   /* Передаем запрос */
                                               send, SOAP, _LARGE_BUFF_SIZE-2, text) ;
       if(status) {                                                 /* Если ошибка... */
                      SMEV_log("Ошибка обмена:") ;
                     SMEV_show("Ошибка обмена:") ;
                      SMEV_log( text) ;
                     SMEV_show( text) ;
                  }

       if(!strcmp(request, SOAP))  break ;

                                        }
/*- - - - - - - - - - - - - - -  Анализ ответа на контрольный запрос */
             SMEV_show("\r\nОтвет сервиса:") ;
              SMEV_log("\r\nОтвет сервиса:") ;

         SMEV_fromUTF8(SOAP, sign) ;

             SMEV_show(SOAP) ;
              SMEV_log(SOAP) ;

                memset(timestamp_1, 0, sizeof(timestamp_1)) ;

          entry=strstr(SOAP, "<lastDumpDate>") ;
       if(entry==NULL) {
                         SMEV_show("Тэг <lastDumpDate> в ответе не найден.") ;
                          SMEV_log("Тэг <lastDumpDate> в ответе не найден.") ;
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
                         SMEV_show("Тэг <lastDumpDateUrgently> в ответе не найден.") ;
                          SMEV_log("Тэг <lastDumpDateUrgently> в ответе не найден.") ;
                             break ;   
                       }

               strncpy(timestamp_2, entry+strlen("<lastDumpDateUrgently>"), sizeof(timestamp_2)-1) ;
          end = strchr(timestamp_2, '<') ;
       if(end!=NULL)  *end=0 ;

                      sprintf(text, "\r\nlastDumpDateUrgently: %s", timestamp_2) ;
                    SMEV_show(text) ;

/*----------------------------------- Проверка необходимости запроса */

                    strcat(timestamp_1, timestamp_2) ;              /* Формируем комбинированную метку */

               SMEV_cookie("READ", text, sizeof(text)-1) ;

         if(!stricmp(timestamp_1, text)) {
                   SMEV_show("\nДанные не требуют обновления\n") ;
                    SMEV_log("\nДанные не требуют обновления\n") ;
                                      rc=_SMEV_UPTODATE_RC ;
                                                 break ;
                                       }
/*------------------------------------------------ Первичный запроса */

                      SMEV_show("\nЗАПРОС ИНДИВИДУАЛЬНОГО КОДА ОТВЕТА\n") ;
/*- - - - - - - - - - - - - - - - -  Формирование первичного запроса */
          SMEVi_rkn_formrequest(request) ;                          /* Формируем запрос */
                      SMEV_show(request) ;

          status=SMEV_form_sign(request, sign, _SMALL_BUFF_SIZE-1); /* Формирование подписи запроса */
#ifndef  UNIX
                    SMEV_system(NULL) ;
#endif
       if(status)  break ;

                         memset(SOAP, 0, sizeof(_LARGE_BUFF_SIZE)) ;
        HTTP_tcp::Base64_incode(request, SOAP, strlen(request)) ;
                         strcpy(request, SOAP) ;

             SMEVi_rkn_formSOAP(SOAP, "REQUEST", request, sign) ;   /* Формирование SOAP-пакета */
                      SMEV_show(SOAP) ;
                       SMEV_log(SOAP) ;
/*- - - - - - - - - - - - - - - - - - -  Отправка первичного запроса */
            strcpy(request, SOAP) ;                                 /* Сохраняем запрос */

                    send[0]=SOAP ;
                    send[1]=NULL ;

                                 sprintf(text, "\r\nОтправка запроса на %s ...", __SMEV_url) ;
                               SMEV_show(text) ;

          status=HTTP_tcp::SOAP_exchange(__SMEV_url, "anonymous",   /* Передаем запрос */
                                               send, SOAP, _LARGE_BUFF_SIZE, text) ;
       if(status) {                                                 /* Если ошибка... */
                      SMEV_log("Ошибка обмена:") ;
                     SMEV_show("Ошибка обмена:") ;
                      SMEV_log( text) ;
                     SMEV_show( text) ;
                  }

       if(!strcmp(request, SOAP))  break ;
/*- - - - - - - - - - - - - - - -  Анализ ответа на первичный запрос */
             SMEV_show("\r\nОтвет сервиса:") ;
              SMEV_log("\r\nОтвет сервиса:") ;
             SMEV_show(SOAP) ;
              SMEV_log(SOAP) ;

                memset(result,  0, sizeof(result )) ;
                memset(comment, 0, sizeof(comment)) ;
                memset(data_id, 0, sizeof(data_id)) ;

          entry=strstr(SOAP, "<result>") ;
       if(entry==NULL) {
                         SMEV_show("Тэг <result> в ответе не найден.") ;
                          SMEV_log("Тэг <result> в ответе не найден.") ;
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
                         SMEV_show("Тэг <code> в ответе не найден.") ;
                          SMEV_log("Тэг <code> в ответе не найден.") ;
                             break ;   
                       }

               strncpy(data_id, entry+strlen("<code>"), sizeof(data_id)-1) ;
          end = strchr(data_id, '<') ;
       if(end!=NULL)  *end=0 ;

                      sprintf(text, "code         : %s", data_id) ;
                    SMEV_show(text) ;

/*---------------------------------- Формирование вторичного запроса */

                      SMEV_show("\nЗАПРОС ОТВЕТА\n") ;

             SMEVi_rkn_formSOAP(SOAP, "HOOK", data_id, NULL) ;      /* Формирование SOAP-пакета */
                      SMEV_show(SOAP) ;

                         strcpy(request, SOAP) ;                    /* Сохраняем запрос */

/*----------------------- Цикл ожидания обработки вторичного запроса */

      while(1) {                                                    /* LOOP */
/*- - - - - - - - - - - - - - - - - - -  Отправка вторичного запроса */
            strcpy(SOAP, request) ;                                 /* Восстанавливаем запрос */

                    send[0]=SOAP ;
                    send[1]=NULL ;

                                 sprintf(text, "\r\nОтправка запроса на %s ...", __SMEV_url) ;
                               SMEV_show(text) ;

          status=HTTP_tcp::SOAP_exchange(__SMEV_url, "anonymous",   /* Передаем запрос */
                                               send, SOAP, _LARGE_BUFF_SIZE, text) ;
       if(status) {                                                 /* Если ошибка... */
                      SMEV_log("Ошибка обмена:") ;
                     SMEV_show("Ошибка обмена:") ;
                      SMEV_log( text) ;
                     SMEV_show( text) ;
                  }

       if(!strcmp(request, SOAP))  break ;
/*- - - - - - - - - - - - - - - -  Анализ ответа на вторичный запрос */
                       text[   0]=SOAP[2048] ;
                       SOAP[2048]= 0 ;

             SMEV_show("\r\nОтвет сервиса:") ;
              SMEV_log("\r\nОтвет сервиса:") ;
             SMEV_show(SOAP) ;
              SMEV_log(SOAP) ;

                       SOAP[2048]=text[0] ;

                memset(result,  0, sizeof(result )) ;
                memset(comment, 0, sizeof(comment)) ;

          entry=strstr(SOAP, "<result>") ;
       if(entry==NULL) {
                         SMEV_show("Тэг <result> в ответе не найден.") ;
                          SMEV_log("Тэг <result> в ответе не найден.") ;
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
/*- - - - - - - - - - - - - - - - - - - - - - - - - Обработка ответа */
       if(!stricmp(result,  "true")) {

              entry=strstr(SOAP, "<registerZipArchive>") ;
           if(entry==NULL) {
                              SMEV_show("Тэг <registerZipArchive> в ответе не найден.") ;
                               SMEV_log("Тэг <registerZipArchive> в ответе не найден.") ;
                                     break ;   
                           }

                           entry+=strlen("<registerZipArchive>") ; 
              end  =strstr(entry, "</registerZipArchive>") ;
           if(end  ==NULL) {
                              SMEV_show("Тэг <registerZipArchive> в ответе не завершен.") ;
                               SMEV_log("Тэг <registerZipArchive> в ответе не завершен.") ;
                                     break ;   
                           }

                                            *end=0 ;  
              status=HTTP_tcp::Base64_decode(entry, request) ;
           if(status<0) {
                              SMEV_show("Данные тэга <registerZipArchive> некорректны.") ;
                               SMEV_log("Данные тэга <registerZipArchive> некорректны.") ;
                                     break ;   
                        }

              status=SMEV_result(request, status) ;
           if(status) {
                              SMEV_show("Ошибка записи файла результата.") ;
                               SMEV_log("Ошибка записи файла результата.") ;
                                     break ;   
                      }

               SMEV_cookie("SAVE", timestamp_1, strlen(timestamp_1)) ;
                                        rc=0 ;
                                           break ;
                                     }

       if(stricmp(result_code, "0"))  break ;
/*- - - - - - - - - - - - - - - - - - - - - -  Технологическая пауза */
              SMEV_show("Повторный запрос через 30 секунд...") ;

                   time_0=time(NULL) ;

           do {               
#ifndef  UNIX
                    SMEV_system(NULL) ;
#endif
                          Sleep(100) ;

              } while(time(NULL)-time_0<30) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
               }                                                    /* END LOOP */
/*--------------------------------------------- Главный рабочий блок */

      } while(0) ;                                                  /* BLOCK */

                 SMEV_show("\r\nОбмен завершен.") ;

/*-------------------------------------------- Освобождение ресурсов */

                      free(request) ;
                      free(sign) ;
                      free(SOAP) ;

/*------------------------------------------ Распаковка файла данных */

                 SMEV_show("\r\nРаспаковка файла данных...") ;

       status=system(__unzip_command) ;
    if(status!=0 || errno!=0) {
                 sprintf(text, "Unzip execute fail (status=%d, errno=%d) : %s", status, errno, __unzip_command) ;
               SMEV_show(text) ;
                SMEV_log(text) ;
                  return(-1) ;
                              }

                 SMEV_show("Распаковка файла данных завершена") ;

/*-------------------------------------- Преобразование файла данных */

                 SMEV_show("\r\nПреобразование файла данных...") ;

       status=SMEVi_rkn_xml2csv("dump.xml", "black_http.csv",      "black_https.csv", 
                                            "black_cyr_http.csv",  "black_cyr_https.csv", 
                                            "black_utf_http.csv", "black_utf_https.csv",
                                            "black_ip4.csv", "black_ip4_rng.csv", "black_ip6.csv", text) ;
    if(status) {
                  SMEV_show(text) ;
                   SMEV_log(text) ;
                      return(-1) ;
              }

                 SMEV_show("\r\nПреобразование файла данных завершено") ;

/*-------------------------------------------------------------------*/

#undef  _SMALL_BUFF_SIZE
#undef  _LARGE_BUFF_SIZE

  return(rc) ;
}


/*********************************************************************/
/*                                                                   */
/*	              Формирование запроса             	             */	

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

/*----------------------------------------------- Подготовка шаблона */

            strcpy(request, request_template) ;

/*------------------------------------- Формирование временной метки */

                     time(&time_stamp) ;
         hhmmss=localtime(&time_stamp) ;

            sprintf(timestamp, "20%02d-%02d-%02dT%02d:%02d:%02d.000+04:00", 
                            hhmmss->tm_year-100, hhmmss->tm_mon+1, hhmmss->tm_mday,
                            hhmmss->tm_hour,     hhmmss->tm_min,   hhmmss->tm_sec  ) ;

        value=SMEV_get_parameter("TIMESTAMP") ;
     if(value==NULL)  value=timestamp ;

                 SMEV_text_subst(request, "$TIMESTAMP$", value) ;

/*------------------------------------------- Подстановка параметров */

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
/*	              Формирование SOAP-пакета         	             */	

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
/*               Преобразование XML-файла в CSV-файл                 */

   int  SMEVi_rkn_xml2csv(char *xml_path, char *www_path, char *ssl_path, 
                                          char *cyr_path, char *cys_path, 
                                          char *u8w_path, char *u8s_path, 
                                          char *ip4_path, char *rn4_path, char *ip6_path, char *error)

{
       FILE *xml_file ;
       FILE *www_file ;
       FILE *ssl_file ;
       FILE *cyr_file ;
       FILE *cys_file ;
       FILE *u8w_file ;
       FILE *u8s_file ;
       FILE *ip4_file ;
       FILE *rn4_file ;
       FILE *ip6_file ;
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
       char *last ;
        int  cut ;
       long  content_cnt ;
       long  www_cnt ;
       long  ssl_cnt ;
       long  cyr_cnt ;
       long  cys_cnt ;
       long  u8w_cnt ;
       long  u8s_cnt ;
       long  ip4_cnt ;
       long  rn4_cnt ;
       long  ip6_cnt ;
       char  text[1024] ;
       char  url_prv[1024] ;

#define   _FRAME_SIZE  256000

/*------------------------------------------------- Входной контроль */

     if(xml_path[0]==0) {
                     sprintf(error, "Не задано имя XML-файла") ;
                           return(-1) ;
                        }
     if(www_path[0]==0) {
                     sprintf(error, "Не задано имя HTTP-файла") ;
                           return(-1) ;
                        }
     if(ssl_path[0]==0) {
                     sprintf(error, "Не задано имя HTTPS-файла") ;
                           return(-1) ;
                        }
     if(cyr_path[0]==0) {
                     sprintf(error, "Не задано имя CYR-HTTP-файла") ;
                           return(-1) ;
                        }
     if(cys_path[0]==0) {
                     sprintf(error, "Не задано имя CYR-HTTPS-файла") ;
                           return(-1) ;
                        }
     if(u8w_path[0]==0) {
                     sprintf(error, "Не задано имя UTF8-HTTP-файла") ;
                           return(-1) ;
                        }
     if(u8s_path[0]==0) {
                     sprintf(error, "Не задано имя UTF8-HTTPS-файла") ;
                           return(-1) ;
                        }
     if(ip4_path[0]==0) {
                     sprintf(error, "Не задано имя IPv4-файла") ;
                           return(-1) ;
                        }
     if(rn4_path[0]==0) {
                     sprintf(error, "Не задано имя IPv4-Range-файла") ;
                           return(-1) ;
                        }
     if(ip6_path[0]==0) {
                     sprintf(error, "Не задано имя IPv6-файла") ;
                           return(-1) ;
                        }
/*-------------------------------------------------- Открытие файлов */

        www_file=fopen(www_path, "wb") ;
     if(www_file==NULL) {
                   sprintf(error, "Ошибка открытия HTTP-файла %d :%s", errno, www_path) ;
                           return(-1) ;
                        }

        ssl_file=fopen(ssl_path, "wb") ;
     if(ssl_file==NULL) {
                   sprintf(error, "Ошибка открытия HTTPS-файла %d :%s", errno, ssl_path) ;
                           return(-1) ;
                        }

        cyr_file=fopen(cyr_path, "wb") ;
     if(cyr_file==NULL) {
                   sprintf(error, "Ошибка открытия CYR-HTTP-файла %d :%s", errno, cyr_path) ;
                           return(-1) ;
                        }

        cys_file=fopen(cys_path, "wb") ;
     if(cys_file==NULL) {
                   sprintf(error, "Ошибка открытия CYR-HTTPS-файла %d :%s", errno, cys_path) ;
                           return(-1) ;
                        }

        u8w_file=fopen(u8w_path, "wb") ;
     if(u8w_file==NULL) {
                   sprintf(error, "Ошибка открытия UTF8-HTTP-файла %d :%s", errno, u8w_path) ;
                           return(-1) ;
                        }

        u8s_file=fopen(u8s_path, "wb") ;
     if(u8s_file==NULL) {
                   sprintf(error, "Ошибка открытия UTF8-HTTPS-файла %d :%s", errno, u8s_path) ;
                           return(-1) ;
                        }

        ip4_file=fopen(ip4_path, "wb") ;
     if(ip4_file==NULL) {
                   sprintf(error, "Ошибка открытия IPv4-файла %d :%s", errno, ip4_path) ;
                           return(-1) ;
                        }

        rn4_file=fopen(rn4_path, "wb") ;
     if(rn4_file==NULL) {
                   sprintf(error, "Ошибка открытия IPv4-файла %d :%s", errno, rn4_path) ;
                           return(-1) ;
                        }

        ip6_file=fopen(ip6_path, "wb") ;
     if(ip6_file==NULL) {
                   sprintf(error, "Ошибка открытия IPv6-файла %d :%s", errno, ip6_path) ;
                           return(-1) ;
                        }

        xml_file=fopen(xml_path, "rb") ;
     if(xml_file==NULL) {
                   sprintf(error, "Ошибка открытия XML-файла %d :%s", errno, xml_path) ;
                           return(-1) ;
                        }
/*----------------------------------------- Преобразование XML-файла */

         buff=(char *)calloc(1, 2*_FRAME_SIZE) ;
        frame=(char *)calloc(1,   _FRAME_SIZE) ;
       record=(char *)calloc(1,   _FRAME_SIZE) ;

                 *error=0 ;

            content_cnt=0 ; 
                www_cnt=0 ; 
                ssl_cnt=0 ; 
                cyr_cnt=0 ;
                cys_cnt=0 ;
                u8w_cnt=0 ;
                u8s_cnt=0 ;
                ip4_cnt=0 ; 
                rn4_cnt=0 ; 
                ip6_cnt=0 ; 

            memset(url_prv, 0, sizeof(url_prv)) ;

    do {
/*- - - - - - - - - - - - - - - - - - -  Считывание следующего кадра */
            memset(frame, 0, _FRAME_SIZE) ;
         cnt=fread(frame, 1, _FRAME_SIZE-1, xml_file) ;

            strcat(buff, frame) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Обработка кадра */
        for(content=buff ; ; content=content_end+1) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - - -  Тег CONTENT */
             content_end=NULL ;

            entry=strstr(content, "<content") ;
         if(entry==NULL)  break ;

            content    =entry ;
            content_end=strstr(content, "</content>") ;
         if(content_end==NULL)  break ;

           *content_end=0 ;

            content_cnt++ ; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Тег URL */
                strcpy(record, content) ; 
            url=strstr(record, "<url") ;
      while(url!=NULL) {
                          url =strstr(url, "![CDATA[")+strlen("![CDATA[") ;
                          end =strchr(url, ']') ;
                         *end = 0 ;

              if(!memcmp(url, "http://", 7))  url=url+7 ;

              if(url[0]=='*' &&                                     /* Обработка префиксной маски */
                 url[1]=='.'   ) {  url[0]='.' ;  url[1]='*' ;  }

            for(c=url, last=NULL, cut=0 ; *c ; c++) {               /* Обработка спец-символов */
              if(*c=='\\')     *c='/' ;
              if(*c=='/' )   last= c ;
              if(*c=='?' ) {   *c= 0 ;  break ;  }
              if(*c=='[' ) {  cut= 1 ;  break ;  }
              if(*c=='(' ) {  cut= 1 ;  break ;  }
              if(*c=='{' ) {  cut= 1 ;  break ;  }
              if(*c=='}' ) {  cut= 1 ;  break ;  }
              if(*c=='%' ) {  cut= 1 ;  break ;  }
                                                    }

         if(*c=='%') {                                              /* Сохранение URL в UTF-кодировке */

           if(strstr(url, "https:")!=NULL) {
                     fwrite( url, 1, strlen( url), u8s_file) ;
                     fwrite("\n", 1, strlen("\n"), u8s_file) ;
                            u8s_cnt++ ; 
                                           }
           else                            {
                     fwrite( url, 1, strlen( url), u8w_file) ;
                     fwrite("\n", 1, strlen("\n"), u8w_file) ;
                            u8w_cnt++ ; 
                                           }
                    }

         if(last!=NULL) {                                           /* Усечка хвоста при наличии спец-символов */
                  if( cut   ==1)  strcpy(last, "\\.*") ; 
                  if(last[1]==0)        *last=0 ;                
                        }

            for(c=url ; *c ; c++)                                   /* Поиск символов кириллицы */
               if(*c<0 || *c>127)  break ;

         if(!strcmp(url_prv, url))  break ;
             strcpy(url_prv, url) ;

         if(*c== 0 ) {

           if(strstr(url, "https:")!=NULL) {
                     fwrite( url, 1, strlen( url), ssl_file) ;
                     fwrite("\n", 1, strlen("\n"), ssl_file) ;
                            ssl_cnt++ ; 
                                           }
           else                            {
                     fwrite( url, 1, strlen( url), www_file) ;
                     fwrite("\n", 1, strlen("\n"), www_file) ;
                            www_cnt++ ; 
                                           }
                     }
         else        {

           if(strstr(url, "https:")!=NULL) {
                     fwrite( url, 1, strlen( url), cys_file) ;
                     fwrite("\n", 1, strlen("\n"), cys_file) ;
                            cys_cnt++ ; 
                                           }
           else                            {
                     fwrite( url, 1, strlen( url), cyr_file) ;
                     fwrite("\n", 1, strlen("\n"), cyr_file) ;
                            cyr_cnt++ ; 
                                           }
                     }

                             break ;
                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - Тег DOMAIN */
       if(url==NULL) {
 
                   strcpy(record, content) ; 
            domain=strstr(record, "<domain") ;
         if(domain!=NULL) {

            domain =strstr(domain, "![CDATA[")+strlen("![CDATA[") ;
               end =strchr(domain, ']') ;
              *end = 0 ;

              if(domain[0]=='*' &&                                  /* Обработка префиксной маски */
                 domain[1]=='.'   ) {  domain[0]='.' ;  domain[1]='*' ;  }

            for(c=domain ; *c ; c++)                                /* Поиск символов кириллицы */
               if(*c<0 || *c>127)  break ;

         if(*c==0) {
                     fwrite(domain, 1, strlen(domain), www_file) ;
                     fwrite(  "\n", 1, strlen(  "\n"), www_file) ;
                            www_cnt++ ; 
                   }
         else      {
                     fwrite(domain, 1, strlen(domain), cyr_file) ;
                     fwrite(  "\n", 1, strlen(  "\n"), cyr_file) ;
                            cyr_cnt++ ; 
                   }

                         }

                     }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Тег IP */
                   strcpy(record, content) ; 

     while(1) {
                  
            ip=strstr(record, "<ip") ;
         if(ip==NULL)  break ;

           *ip =' ' ;
            ip =strchr(ip, '>')+1 ;
           end =strchr(ip, '<') ;
          *end = 0 ;

            for(c=ip ; *c ; c++) if(*c==',')  *c='.' ;

        if(strchr(ip, ':')!=NULL) {
                   fwrite( ip , 1, strlen( ip ), ip6_file) ;
                   fwrite("\n", 1, strlen("\n"), ip6_file) ;
                            ip6_cnt++ ; 
                                  }
        else                      {

             if(strchr(ip, '/')!=NULL) {
                   fwrite( ip , 1, strlen( ip ), rn4_file) ;
                   fwrite("\n", 1, strlen("\n"), rn4_file) ;
                            rn4_cnt++ ; 
                                       } 
             else                      { 
                   fwrite( ip , 1, strlen( ip ), ip4_file) ;
                   fwrite("\n", 1, strlen("\n"), ip4_file) ;
                            ip4_cnt++ ; 
                                       }
                                  }

              }
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Обработка кадра */
                                                    }

              if(*error!=0)  break ;

              if(content_end!=NULL)  memmove(buff, content_end, strlen(content_end)+1) ;
         else if(content    !=buff)  memmove(buff, content,     strlen(content    )+1) ;
/*- - - - - - - - - - - - - - - - - - -  Считывание следующего кадра */
       } while(cnt>0) ;

              free(record) ;
              free(frame) ;
              free(buff) ;

/*-------------------------------------------------- Закрытие файлов */

                    fclose(xml_file) ;
                    fclose(www_file) ;
                    fclose(ssl_file) ;
                    fclose(cyr_file) ;
                    fclose(cys_file) ;
                    fclose(ip4_file) ;
                    fclose(rn4_file) ;
                    fclose(ip6_file) ;

/*-------------------------------------------------------------------*/

         sprintf(text, "Processed %ld records: %ld www, %ld ssl, %ld cyr, %ld cys, %ld u8w, %ld u8s, %ld ip4, %ld ip4 range, %ld ip6",
                                         content_cnt, www_cnt, ssl_cnt, cyr_cnt, cys_cnt, u8w_cnt, u8s_cnt, ip4_cnt, rn4_cnt, ip6_cnt) ;
       SMEV_show(text) ;
        SMEV_log(text) ;

  if(*error!=0)  return(-1) ;

    return(0) ;
}


