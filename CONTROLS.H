#include  <commctrl.h>

/*--------------------------------------------------------- LIST BOX */

#define  LB_CLEAR(elm)               SendMessage(GetDlgItem(hDlg, elm), \
					          LB_RESETCONTENT ,     \
						    0, 0)
#define  LB_ADD_ROW(elm, text)       SendMessage(GetDlgItem(hDlg, elm), \
					           LB_ADDSTRING,        \
						    0, (LPARAM)text)
#define  LB_INS_ROW(elm, num, text)  SendMessage(GetDlgItem(hDlg, elm), \
					           LB_INSERTSTRING,     \
						    (WPARAM)num,        \
						      (LPARAM)text)
#define  LB_SET_ITEM(elm, num, ptr)  SendMessage(GetDlgItem(hDlg, elm), \
					           LB_SETITEMDATA,      \
						    (WPARAM)num,        \
                                                      (LPARAM)ptr)
#define  LB_DEL_ROW(elm, num)        SendMessage(GetDlgItem(hDlg, elm), \
					           LB_DELETESTRING,     \
						    (WPARAM)num, 0)
#define  LB_GET_TEXT(elm, row, text) SendMessage(GetDlgItem(hDlg, elm), \
                                                   LB_GETTEXT,          \
                                                    (WPARAM)row,        \
                                                      (LPARAM)text)
#define  LB_GET_ROW(elm)             SendMessage(GetDlgItem(hDlg, elm), \
                                                   LB_GETCURSEL, 0, 0)
#define  LB_GET_ITEM(elm)            SendMessage(GetDlgItem(hDlg, elm), \
					           LB_GETITEMDATA,      \
                                     SendMessage(GetDlgItem(hDlg, elm), \
                                                   LB_GETCURSEL, 0, 0), \
                                                         0)
#define  LB_GET_COUNT(elm)           SendMessage(GetDlgItem(hDlg, elm), \
                                                   LB_GETCOUNT, 0, 0)
#define  LB_ROW_ITEM(elm, num)       SendMessage(GetDlgItem(hDlg, elm), \
				                   LB_GETITEMDATA,      \
						    (WPARAM)num, 0)
#define  LB_FIND_ROW(elm, text)      SendMessage(GetDlgItem(hDlg, elm), \
				                   LB_FINDSTRING,       \
				                    (WPARAM)-1, (LPARAM)text)
#define  LB_TOP_ROW(elm, row)       SendMessage(GetDlgItem(hDlg, elm), \
					           LB_SETTOPINDEX,     \
						    (WPARAM)row, 0)
#define  LB_SET_SEL(elm, row)       SendMessage(GetDlgItem(hDlg, elm), \
					           LB_SETCURSEL,       \
						    (WPARAM)row, 0)

/*-------------------------------------------------------- COMBO BOX */

#define  CB_CLEAR(elm)                SendMessage(GetDlgItem(hDlg, elm), \
						      CB_RESETCONTENT,	 \
						       0, 0)
#define  CB_FIND_LIST(elm, text)      SendMessage(GetDlgItem(hDlg, elm), \
						      CB_FINDSTRING,	 \
						       (WPARAM)-1,       \
                                                       (LPARAM)text)
#define  CB_ADD_LIST(elm, text)       SendMessage(GetDlgItem(hDlg, elm), \
						      CB_ADDSTRING,	 \
						       0, (LPARAM)text)
#define  CB_DEL_LIST(elm, row)        SendMessage(GetDlgItem(hDlg, elm), \
						      CB_DELETESTRING,	 \
						       (WPARAM)row, 0 )
#define  CB_INS_LIST(elm, row, text)  SendMessage(GetDlgItem(hDlg, elm), \
						      CB_INSERTSTRING,	 \
						       (WPARAM)row,      \
                                                        (LPARAM)text )
#define  CB_GET_LIST(elm, row, text)  SendMessage(GetDlgItem(hDlg, elm), \
						      CB_GETLBTEXT,	 \
						       (WPARAM)row,      \
                                                        (LPARAM)text )
#define  CB_CUR_IDX(elm)              SendMessage(GetDlgItem(hDlg, elm),   \
						       CB_GETCURSEL,0, 0)
#define  CB_CUR_SEL(elm, text)        SendMessage(GetDlgItem(hDlg, elm),     \
						      CB_GETLBTEXT,	     \
                                        SendMessage(GetDlgItem(hDlg, elm),   \
						       CB_GETCURSEL,0, 0  ), \
                                                        (LPARAM)text )

#define  SETc(elm, s)  SendMessage(GetDlgItem(hDlg, elm),		\
					CB_SELECTSTRING, 0, (LPARAM)s)
#define  GETc(elm, s)  SendDlgItemMessage(hDlg, elm,			\
					    WM_GETTEXT, 256, (LPARAM)s)
                                                         
/*------------------------------------------------------------- EDIT */

#define  SETs(elm, s)         SetDlgItemText(hDlg, elm, s)
#define  GETs(elm, s)         GetDlgItemText(hDlg, elm, s, 512)
#define  GETsl(elm, s, len)   GetDlgItemText(hDlg, elm, s, len)
#define  GETs_len(elm)           SendMessage(GetDlgItem(hDlg, elm), \
                                                   WM_GETTEXTLENGTH, 0, 0)

#define  SETi(elm, s)          SetDlgItemInt(hDlg, elm, s, true)
#define  GETi(elm)             GetDlgItemInt(hDlg, elm, NULL, true)

#define  UNDOs(elm)       do {  SendMessage(GetDlgItem(hDlg, elm),             \
                                                   EM_UNDO, 0, 0) ;            \
                                 SendMessage(GetDlgItem(hDlg, elm),            \
                                                   EM_EMPTYUNDOBUFFER, 0, 0) ; \
                             } while(0) 

#define  SELECTs(elm, b, e)    SendMessage(GetDlgItem(hDlg, elm),             \
                                            EM_SETSEL, (WPARAM)b, (LPARAM)e)

/*------------------------------------------------------- IP-ADDRESS */

#define  SETip(elm, s)  SendMessage(GetDlgItem(hDlg, elm),		\
					IPM_SETADDRESS, 0, (LPARAM)s)
#define  GETip(elm, s)  SendDlgItemMessage(hDlg, elm,			\
					IPM_GETADDRESS, 0, (LPARAM)s)
#define  CLRip(elm)     SendMessage(GetDlgItem(hDlg, elm),		\
					IPM_CLEARADDRESS, 0, 0)

/*-------------------------------------------------------- CHECK BOX */

#define  SETx(elm, v) CheckDlgButton(hDlg, elm, (v>0)?BST_CHECKED:BST_UNCHECKED)
#define  GETx(elm)    ((IsDlgButtonChecked(hDlg, elm)==BST_CHECKED)?1:0)

/*----------------------------------------------------------- BUTTON */

#define  BN_TEXT(elm, s)   SendMessage(GetDlgItem(hDlg, elm),		\
		    		         WM_SETTEXT, 0, (LPARAM)s)

/*--------------------------------------------------------- PROGRESS */

#define  PR_RANGE(elm, s)  SendMessage(GetDlgItem(hDlg, elm),		\
                                          PBM_SETRANGE32,               \
                                            0, (LPARAM)(int)s)

#define  SETp(elm, s)      SendMessage(GetDlgItem(hDlg, elm),		\
                                          PBM_SETPOS,                   \
                                            (WPARAM)(int)s, 0)

/*------------------------------------------------------------- USER */

#define  SETu(elm, s)   SendMessage(GetDlgItem(hDlg, elm),		\
		                     WM_SETTEXT, 0, (LPARAM)s)
#define  GETu(elm, s)   SendMessage(GetDlgItem(hDlg, elm),		\
		                     WM_GETTEXT, 512, (LPARAM)s)

/*------------------------------------------------------------ TOOLS */

#define   SETFONT(elm, font)   SendMessage(GetDlgItem(hDlg, elm),        \
                                            WM_SETFONT, (WPARAM)font, 0)

#define    ENABLE(elm)   EnableWindow(GetDlgItem(hDlg, elm), TRUE)
#define   DISABLE(elm)   EnableWindow(GetDlgItem(hDlg, elm), FALSE)

#define     SHOW(elm)       ShowWindow(GetDlgItem(hDlg, elm), SW_SHOW)
#define     HIDE(elm)       ShowWindow(GetDlgItem(hDlg, elm), SW_HIDE)
#define  VISIBLE(elm)  IsWindowVisible(GetDlgItem(hDlg, elm))

#define   REDRAW(elm)    RedrawWindow(GetDlgItem(hDlg, elm), NULL, NULL, 0)

#define     ITEM(elm)  GetDlgItem(hDlg, elm)

