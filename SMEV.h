/*********************************************************************/
/*                                                                   */
/*          ������� ������������� �������� ���                       */
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

/*---------------------------------------------- ��������� ��������� */

#define   _PROGRAM_TITLE   "SMEV"

#define   _VERSION   "21.11.2018"
#define   _WINDOW_TITLE       "����� � ��������� SMEV"

/*--------------------------------------------------- ������� ������ */

  _EXTERNAL   int  __exit_flag ;             /* ���� ���������� ������ */

/*------------------------------------------ ����������� ���� ������ */

/*---------------------------------- ���� ���������������� ��������� */

/*---------------------------------------------------- ���� �������� */

#define        _DB_INIT_RC           1
#define    _WINDOW_INIT_RC           2
#define    _CONFIG_READ_RC           3
#define   _COMMAND_READ_RC           4
#define   _SERVICE_UNKNOWN_RC        5
#define      _SMEV_ERROR_RC        101
#define      _SMEV_UPTODATE_RC     102

/*-------------------------------------------------- ���������� ���� */

#ifdef  UNIX
#else

  _EXTERNAL            UINT  TaskBar_Msg ;        /* ��������� ����������� ����� � TaskBar */
  _EXTERNAL  NOTIFYICONDATA  TbIcon ;             /* �������� TaskBar-������ */

  _EXTERNAL       HINSTANCE  hInst ;
  _EXTERNAL        WNDCLASS  FrameWindow ;
  _EXTERNAL            HWND  hFrameWindow ;
  _EXTERNAL            HWND  __only_window ;

  _EXTERNAL            HWND  __dialog ;

#endif

  _EXTERNAL             int  __window_closed ;    /* ���� ������� ���� � ���� */

  _EXTERNAL             int  __silent_process ;   /* ���� ������ � ������ ��������� ������ */
  _EXTERNAL             int  __silent_rc ;        /* ��� �������� � ������ ��������� ������ */

/*------------------------------------------------- ����� ���������� */

  _EXTERNAL     char  __config_path[FILENAME_MAX] ;      /* ���� � ����� ������������ */
  _EXTERNAL     char  __log_path[FILENAME_MAX] ;         /* ���� � ���-����� */

  _EXTERNAL     char  __cookie_path[FILENAME_MAX] ;      /* ���� � ����� ���������� ��������� */
  _EXTERNAL     char  __result_path[FILENAME_MAX] ;      /* ���� � ����� ���������� */

/*--------------------------------------------------- ������� ������ */

#define   _TIMER_0           1

#ifdef  UNIX
#else

  _EXTERNAL       HANDLE  hBackGround_Thread ;
  _EXTERNAL        DWORD  hBackGround_PID ;

            DWORD WINAPI  BackGround_Thread(LPVOID) ;

#endif

/*-------------------------------------------- �������� ������� ���� */

  _EXTERNAL     char  __SMEV_name[FILENAME_MAX] ;         /* ����������������� ��� ������� */
  _EXTERNAL     char  __SMEV_url[FILENAME_MAX] ;          /* URL ������� */

/*-------------------------------- �������������� � ������-��������� */

  _EXTERNAL     char  __crypto_sign_in[FILENAME_MAX] ;    /* ���� ����� ��� ������� */
  _EXTERNAL     char  __crypto_sign_out[FILENAME_MAX] ;   /* ���� ����� � �������� */
  _EXTERNAL     char  __crypto_sign_exec[FILENAME_MAX] ;  /* ��������� ������ ������� ������� ������� */

/*--------------------------------- ��������� ��������� ������������ */

  typedef struct {
                    char  name[64] ;     /* ��� ��������� */
                    char  value[1024] ;  /* �������� ��������� */
                 } ConfigPar ;

#define   _CONFIG_PARS_MAX  20

  _EXTERNAL   ConfigPar  __config_pars[_CONFIG_PARS_MAX] ;     /* ������ ���������� */
  _EXTERNAL         int  __config_pars_cnt ;

/*-------------------------------------------------------- ��������� */

/* SMEV.cpp */
            int  SMEV_silent_process (void) ;                    /* ���������� ������ ��������� ������ */

#ifndef  UNIX
            int  SMEV_system         (HWND) ;                    /* ��������� ��������� ��������� */
#endif

            int  SMEV_message        (char *) ;                  /* ������� ������ ��������� */
            int  SMEV_message        (char *, int) ;
            int  SMEV_log            (char *) ;                  /* ������� ���� */
            int  SMEV_read_command   (char *, char *, int) ;     /* ���������� ����� ��������� ������ */
            int  SMEV_read_config    (void) ;                    /* ���������� ����� ������������ */
           void  SMEV_text_trim      (char *) ;                  /* ������� ��������� � �������� ���������� �������� */
            int  SMEV_create_path    (char *) ;                  /* ������������ ���� � ������� */
            int  SMEV_text_subst     (char *, char *, char *) ;  /* ����������� ����� ������ */
            int  SMEV_load_clipboard (char *) ;                  /* ������ ������ � ClipBoard */
           void  SMEV_show           (char *) ;                  /* ����������� ������ � ���� */	
           char *SMEV_get_parameter  (char *) ;                  /* ��������� ������ �� �������� ��������� */	
            int  SMEV_cookie         (char *, char *, int) ;     /* ������ � ������ ���������� ��������� */
            int  SMEV_result         (char *, int) ;             /* ������ ����� ���������� */
            int  SMEV_form_sign      (char *, char *, int) ;     /* ������������ ������� */
           void  SMEV_toUTF8         (char *, char *) ;          /* ��������� � UTF-8 */
           void  SMEV_fromUTF8       (char *, char *) ;          /* ��������� �� UTF-8 */

/* SMEV_rkn_gov_ru.cpp */
#ifndef  UNIX
  INT_PTR CALLBACK  SMEV_rkn_dialog     (HWND, UINT, WPARAM, LPARAM) ;
#endif
               int  SMEV_rkn_exchange   (char *) ;                  /* ������������� ������ */

