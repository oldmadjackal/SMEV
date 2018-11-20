/*********************************************************************/
/*                                                                   */
/*          Система маршрутизации запросов ФНС                       */
/*                                                                   */
/*********************************************************************/

#ifdef  __MAIN__
#define  _EXTERNAL
#else
#define  _EXTERNAL  extern
#endif

#include <time.h>
#include <sys/timeb.h>
#include <sys/types.h>

#ifdef  UNIX

#define  stricmp  strcasecmp
#define  Sleep(p)     sleep(p/1000)

#else


#endif

/*---------------------------------------------- Параметры генерации */

#define   _PROGRAM_TITLE   "SMEV"

#define   _VERSION   "21.11.2018"
#define   _WINDOW_TITLE       "Обмен с сервисами SMEV"

/*--------------------------------------------------- Система команд */

  _EXTERNAL   int  __exit_flag ;             /* Флаг завершения работы */

/*------------------------------------------ Специальные типы данных */

/*---------------------------------- Коды пользовательских сообщений */

/*---------------------------------------------------- Коды возврата */

#define        _DB_INIT_RC           1
#define    _WINDOW_INIT_RC           2
#define    _CONFIG_READ_RC           3
#define   _COMMAND_READ_RC           4
#define   _SERVICE_UNKNOWN_RC        5
#define      _SMEV_ERROR_RC        101
#define      _SMEV_UPTODATE_RC     102

/*-------------------------------------------------- Диалоговые окна */

#ifdef  UNIX
#else

  _EXTERNAL            UINT  TaskBar_Msg ;        /* Сообщение активизации иконы в TaskBar */
  _EXTERNAL  NOTIFYICONDATA  TbIcon ;             /* Описание TaskBar-иконки */

  _EXTERNAL       HINSTANCE  hInst ;
  _EXTERNAL        WNDCLASS  FrameWindow ;
  _EXTERNAL            HWND  hFrameWindow ;
  _EXTERNAL            HWND  __only_window ;

  _EXTERNAL            HWND  __dialog ;

#endif

  _EXTERNAL             int  __window_closed ;    /* Флаг скрытия окна в трей */

  _EXTERNAL             int  __silent_process ;   /* Флаг работы в режиме командной строки */
  _EXTERNAL             int  __silent_rc ;        /* Код возврата в режиме командной строки */

/*------------------------------------------------- Общие переменные */

  _EXTERNAL     char  __config_path[FILENAME_MAX] ;      /* Путь к файлу конфигурации */
  _EXTERNAL     char  __log_path[FILENAME_MAX] ;         /* Путь к лог-файлу */

  _EXTERNAL     char  __cookie_path[FILENAME_MAX] ;      /* Путь к файлу сохранения состояния */
  _EXTERNAL     char  __result_path[FILENAME_MAX] ;      /* Путь к файлу результата */

/*--------------------------------------------------- Фоновые потоки */

#define   _TIMER_0           1

#ifdef  UNIX
#else

  _EXTERNAL       HANDLE  hBackGround_Thread ;
  _EXTERNAL        DWORD  hBackGround_PID ;

            DWORD WINAPI  BackGround_Thread(LPVOID) ;

#endif

/*-------------------------------------------- Описание сервиса СМЭВ */

  _EXTERNAL     char  __SMEV_name[FILENAME_MAX] ;         /* Идентификационное имя сервиса */
  _EXTERNAL     char  __SMEV_url[FILENAME_MAX] ;          /* URL сервиса */

/*-------------------------------- Взаимодействие с крипто-утилитами */

  _EXTERNAL     char  __crypto_sign_in[FILENAME_MAX] ;    /* Путь файла для подписи */
  _EXTERNAL     char  __crypto_sign_out[FILENAME_MAX] ;   /* Путь файла с подписью */
  _EXTERNAL     char  __crypto_sign_exec[FILENAME_MAX] ;  /* Командная строка запуска утилиты подписи */

/*--------------------------------- Свободные параметры конфигурации */

  typedef struct {
                    char  name[64] ;     /* Имя параметра */
                    char  value[1024] ;  /* Значение параметра */
                 } ConfigPar ;

#define   _CONFIG_PARS_MAX  20

  _EXTERNAL   ConfigPar  __config_pars[_CONFIG_PARS_MAX] ;     /* Список параметров */
  _EXTERNAL         int  __config_pars_cnt ;

/*-------------------------------------------------------- Прототипы */

/* SMEV.cpp */
            int  SMEV_silent_process (void) ;                    /* Обработчик режима командной строки */

#ifndef  UNIX
            int  SMEV_system         (HWND) ;                    /* Обработка системных сообщений */
#endif

            int  SMEV_message        (char *) ;                  /* Система выдачи сообщений */
            int  SMEV_message        (char *, int) ;
            int  SMEV_log            (char *) ;                  /* Ведение лога */
            int  SMEV_read_command   (char *, char *, int) ;     /* Считывание файла командных ключей */
            int  SMEV_read_config    (void) ;                    /* Считывание файла конфигурации */
           void  SMEV_text_trim      (char *) ;                  /* Отсечка начальных и конечных пробельных символов */
            int  SMEV_create_path    (char *) ;                  /* Формирование пути к разделу */
            int  SMEV_text_subst     (char *, char *, char *) ;  /* Подстановка полей данных */
            int  SMEV_load_clipboard (char *) ;                  /* Выдача текста в ClipBoard */
           void  SMEV_show           (char *) ;                  /* Отображение текста в логе */	
           char *SMEV_get_parameter  (char *) ;                  /* Получение ссылки на значение параметра */	
            int  SMEV_cookie         (char *, char *, int) ;     /* Работа с файлом сохранения контекста */
            int  SMEV_result         (char *, int) ;             /* Запись файла результата */
            int  SMEV_form_sign      (char *, char *, int) ;     /* Формирование подписи */
           void  SMEV_toUTF8         (char *, char *) ;          /* Кодировка в UTF-8 */
           void  SMEV_fromUTF8       (char *, char *) ;          /* Кодировка из UTF-8 */

/* SMEV_rkn_gov_ru.cpp */
#ifndef  UNIX
  INT_PTR CALLBACK  SMEV_rkn_dialog     (HWND, UINT, WPARAM, LPARAM) ;
#endif
               int  SMEV_rkn_exchange   (char *) ;                  /* Осуществление обмена */

