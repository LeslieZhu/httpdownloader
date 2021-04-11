/*
	HTTP Downloader can download files through HTTP(S), FTP(S), and SFTP connections.
	Copyright (C) 2015-2021 Eric Kutcher

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "options.h"
#include "utilities.h"
#include "folder_browser.h"
#include "lite_gdi32.h"

#define BTN_DOWNLOAD_HISTORY		1000
#define BTN_QUICK_ALLOCATION		1001
#define BTN_SET_FILETIME			1002
#define BTN_UPDATE_REDIRECTED		1003
#define BTN_USE_ONE_INSTANCE		1004
#define BTN_PREVENT_STANDBY			1005
#define BTN_RESUME_DOWNLOADS		1006

#define CB_DRAG_AND_DROP_ACTION		1007

#define CB_PROMPT_LAST_MODIFIED		1008
#define CB_PROMPT_RENAME			1009
#define CB_PROMPT_FILE_SIZE			1010
#define EDIT_MAX_FILE_SIZE			1011

#define CB_SHUTDOWN_ACTION			1012

#define BTN_DEFAULT_DOWNLOAD_DIRECTORY	1013

#define BTN_USE_TEMP_DOWNLOAD_DIRECTORY	1014
#define BTN_TEMP_DOWNLOAD_DIRECTORY		1015

#define EDIT_THREAD_COUNT			1016

// Advanced Tab
HWND g_hWnd_chk_download_history = NULL;
HWND g_hWnd_chk_quick_allocation = NULL;
HWND g_hWnd_chk_set_filetime = NULL;
HWND g_hWnd_chk_update_redirected = NULL;
HWND g_hWnd_chk_use_one_instance = NULL;
HWND g_hWnd_chk_prevent_standby = NULL;
HWND g_hWnd_chk_resume_downloads = NULL;

HWND g_hWnd_drag_and_drop_action = NULL;

HWND g_hWnd_prompt_last_modified = NULL;
HWND g_hWnd_prompt_rename = NULL;
HWND g_hWnd_max_file_size = NULL;
HWND g_hWnd_prompt_file_size = NULL;

HWND g_hWnd_shutdown_action = NULL;

HWND g_hWnd_default_download_directory = NULL;
HWND g_hWnd_btn_default_download_directory = NULL;

HWND g_hWnd_chk_temp_download_directory = NULL;
HWND g_hWnd_temp_download_directory = NULL;
HWND g_hWnd_btn_temp_download_directory = NULL;

HWND g_hWnd_thread_count = NULL;
HWND g_hWnd_ud_thread_count = NULL;

wchar_t *t_default_download_directory = NULL;
wchar_t *t_temp_download_directory = NULL;

wchar_t file_size_tooltip_text[ 32 ];
HWND g_hWnd_file_size_tooltip = NULL;

LRESULT CALLBACK AdvancedTabWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch ( msg )
	{
		case WM_CREATE:
		{
			RECT rc;
			_GetClientRect( hWnd, &rc );

			g_hWnd_chk_download_history = _CreateWindowW( WC_BUTTON, ST_V_Enable_download_history, BS_AUTOCHECKBOX | WS_CHILD | WS_TABSTOP | WS_VISIBLE, 0, 0, rc.right, 20, hWnd, ( HMENU )BTN_DOWNLOAD_HISTORY, NULL, NULL );
			g_hWnd_chk_quick_allocation = _CreateWindowW( WC_BUTTON, ST_V_Enable_quick_file_allocation, BS_AUTOCHECKBOX | WS_CHILD | WS_TABSTOP | WS_VISIBLE, 0, 20, rc.right, 20, hWnd, ( HMENU )BTN_QUICK_ALLOCATION, NULL, NULL );
			g_hWnd_chk_set_filetime = _CreateWindowW( WC_BUTTON, ST_V_Set_date_and_time_of_file, BS_AUTOCHECKBOX | WS_CHILD | WS_TABSTOP | WS_VISIBLE, 0, 40, rc.right, 20, hWnd, ( HMENU )BTN_SET_FILETIME, NULL, NULL );
			g_hWnd_chk_update_redirected = _CreateWindowW( WC_BUTTON, ST_V_Update_redirected_URL_s__in_download_list, BS_AUTOCHECKBOX | WS_CHILD | WS_TABSTOP | WS_VISIBLE, 0, 60, rc.right, 20, hWnd, ( HMENU )BTN_UPDATE_REDIRECTED, NULL, NULL );
			g_hWnd_chk_use_one_instance = _CreateWindowW( WC_BUTTON, ST_V_Allow_only_one_instance, BS_AUTOCHECKBOX | WS_CHILD | WS_TABSTOP | WS_VISIBLE, 0, 80, rc.right, 20, hWnd, ( HMENU )BTN_USE_ONE_INSTANCE, NULL, NULL );

			g_hWnd_chk_prevent_standby = _CreateWindowW( WC_BUTTON, ST_V_Prevent_system_standby, BS_AUTOCHECKBOX | WS_CHILD | WS_TABSTOP | WS_VISIBLE, 0, 100, rc.right, 20, hWnd, ( HMENU )BTN_PREVENT_STANDBY, NULL, NULL );
			g_hWnd_chk_resume_downloads = _CreateWindowW( WC_BUTTON, ST_V_Resume_previously_downloading, BS_AUTOCHECKBOX | WS_CHILD | WS_TABSTOP | WS_VISIBLE, 0, 120, rc.right, 20, hWnd, ( HMENU )BTN_RESUME_DOWNLOADS, NULL, NULL );

			HWND hWnd_static_drag_and_drop_action = _CreateWindowW( WC_STATIC, ST_V_Drag_and_drop_URL_s__action_, WS_CHILD | WS_VISIBLE, 0, 149, rc.right - 155, 15, hWnd, NULL, NULL, NULL );
			g_hWnd_drag_and_drop_action = _CreateWindowExW( WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL, CBS_AUTOHSCROLL | CBS_DROPDOWNLIST | WS_CHILD | WS_TABSTOP | WS_VSCROLL | WS_VISIBLE, rc.right - 150, 145, 150, 23, hWnd, ( HMENU )CB_DRAG_AND_DROP_ACTION, NULL, NULL );
			_SendMessageW( g_hWnd_drag_and_drop_action, CB_ADDSTRING, 0, ( LPARAM )ST_V_None );
			_SendMessageW( g_hWnd_drag_and_drop_action, CB_ADDSTRING, 0, ( LPARAM )ST_V_Download_immediately );
			_SendMessageW( g_hWnd_drag_and_drop_action, CB_ADDSTRING, 0, ( LPARAM )ST_V_Add_in_Stopped_state );

			_SendMessageW( g_hWnd_drag_and_drop_action, CB_SETCURSEL, cfg_drag_and_drop_action, 0 );


			HWND hWnd_static_prompt_last_modified = _CreateWindowW( WC_STATIC, ST_V_When_a_file_has_been_modified_, WS_CHILD | WS_VISIBLE, 0, 177, rc.right - 155, 15, hWnd, NULL, NULL, NULL );
			g_hWnd_prompt_last_modified = _CreateWindowExW( WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL, CBS_AUTOHSCROLL | CBS_DROPDOWNLIST | WS_CHILD | WS_TABSTOP | WS_VSCROLL | WS_VISIBLE, rc.right - 150, 173, 150, 23, hWnd, ( HMENU )CB_PROMPT_LAST_MODIFIED, NULL, NULL );
			_SendMessageW( g_hWnd_prompt_last_modified, CB_ADDSTRING, 0, ( LPARAM )ST_V_Display_Prompt );
			_SendMessageW( g_hWnd_prompt_last_modified, CB_ADDSTRING, 0, ( LPARAM )ST_V_Continue_Download );
			_SendMessageW( g_hWnd_prompt_last_modified, CB_ADDSTRING, 0, ( LPARAM )ST_V_Restart_Download );
			_SendMessageW( g_hWnd_prompt_last_modified, CB_ADDSTRING, 0, ( LPARAM )ST_V_Skip_Download );

			_SendMessageW( g_hWnd_prompt_last_modified, CB_SETCURSEL, cfg_prompt_last_modified, 0 );


			HWND hWnd_static_prompt_rename = _CreateWindowW( WC_STATIC, ST_V_When_a_file_already_exists_, WS_CHILD | WS_VISIBLE, 0, 205, rc.right - 155, 15, hWnd, NULL, NULL, NULL );
			g_hWnd_prompt_rename = _CreateWindowExW( WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL, CBS_AUTOHSCROLL | CBS_DROPDOWNLIST | WS_CHILD | WS_TABSTOP | WS_VSCROLL | WS_VISIBLE, rc.right - 150, 201, 150, 23, hWnd, ( HMENU )CB_PROMPT_RENAME, NULL, NULL );
			_SendMessageW( g_hWnd_prompt_rename, CB_ADDSTRING, 0, ( LPARAM )ST_V_Display_Prompt );
			_SendMessageW( g_hWnd_prompt_rename, CB_ADDSTRING, 0, ( LPARAM )ST_V_Rename_File );
			_SendMessageW( g_hWnd_prompt_rename, CB_ADDSTRING, 0, ( LPARAM )ST_V_Overwrite_File );
			_SendMessageW( g_hWnd_prompt_rename, CB_ADDSTRING, 0, ( LPARAM )ST_V_Skip_Download );

			_SendMessageW( g_hWnd_prompt_rename, CB_SETCURSEL, cfg_prompt_rename, 0 );


			HWND hWnd_static_prompt_file_size = _CreateWindowW( WC_STATIC, ST_V_When_a_file_is_greater_than_or_equal_to_, WS_CHILD | WS_VISIBLE, 0, 233, rc.right - 260, 15, hWnd, NULL, NULL, NULL );

			g_hWnd_max_file_size = _CreateWindowExW( WS_EX_CLIENTEDGE, WC_EDIT, NULL, ES_AUTOHSCROLL | ES_CENTER | ES_NUMBER | WS_CHILD | WS_TABSTOP | WS_VISIBLE, rc.right - 255, 229, 100, 23, hWnd, ( HMENU )EDIT_MAX_FILE_SIZE, NULL, NULL );

			_SendMessageW( g_hWnd_max_file_size, EM_LIMITTEXT, 20, 0 );

			g_hWnd_file_size_tooltip = _CreateWindowExW( WS_EX_TOPMOST, TOOLTIPS_CLASS, 0, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, 0, 0, 0, 0, hWnd, NULL, NULL, NULL );

			file_size_tooltip_text[ 0 ] = 0;

			TOOLINFO ti;
			_memzero( &ti, sizeof( TOOLINFO ) );
			ti.cbSize = sizeof( TOOLINFO );
			ti.uFlags = TTF_SUBCLASS;
			ti.hwnd = g_hWnd_max_file_size;
			ti.lpszText = file_size_tooltip_text;

			_GetClientRect( hWnd, &ti.rect );
			_SendMessageW( g_hWnd_file_size_tooltip, TTM_ADDTOOL, 0, ( LPARAM )&ti );


			char value[ 21 ];
			_memzero( value, sizeof( char ) * 21 );
			__snprintf( value, 21, "%I64u", cfg_max_file_size );
			_SendMessageA( g_hWnd_max_file_size, WM_SETTEXT, 0, ( LPARAM )value );


			g_hWnd_prompt_file_size = _CreateWindowExW( WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL, CBS_AUTOHSCROLL | CBS_DROPDOWNLIST | WS_CHILD | WS_TABSTOP | WS_VSCROLL | WS_VISIBLE, rc.right - 150, 229, 150, 23, hWnd, ( HMENU )CB_PROMPT_FILE_SIZE, NULL, NULL );
			_SendMessageW( g_hWnd_prompt_file_size, CB_ADDSTRING, 0, ( LPARAM )ST_V_Display_Prompt );
			_SendMessageW( g_hWnd_prompt_file_size, CB_ADDSTRING, 0, ( LPARAM )ST_V_Continue_Download );
			_SendMessageW( g_hWnd_prompt_file_size, CB_ADDSTRING, 0, ( LPARAM )ST_V_Skip_Download );

			_SendMessageW( g_hWnd_prompt_file_size, CB_SETCURSEL, cfg_prompt_file_size, 0 );


			HWND hWnd_static_shutdown_action = _CreateWindowW( WC_STATIC, ST_V_System_shutdown_action_, WS_CHILD | WS_VISIBLE, 0, 261, rc.right - 155, 15, hWnd, NULL, NULL, NULL );
			g_hWnd_shutdown_action = _CreateWindowExW( WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL, CBS_AUTOHSCROLL | CBS_DROPDOWNLIST | WS_CHILD | WS_TABSTOP | WS_VSCROLL | WS_VISIBLE, rc.right - 150, 257, 150, 23, hWnd, ( HMENU )CB_SHUTDOWN_ACTION, NULL, NULL );

			_SendMessageW( g_hWnd_shutdown_action, CB_ADDSTRING, 0, ( LPARAM )ST_V_None );
			_SendMessageW( g_hWnd_shutdown_action, CB_ADDSTRING, 0, ( LPARAM )ST_V_Exit_program );
			_SendMessageW( g_hWnd_shutdown_action, CB_ADDSTRING, 0, ( LPARAM )ST_V_Log_off );
			_SendMessageW( g_hWnd_shutdown_action, CB_ADDSTRING, 0, ( LPARAM )ST_V_Lock );
			_SendMessageW( g_hWnd_shutdown_action, CB_ADDSTRING, 0, ( LPARAM )ST_V_Restart_system );
			_SendMessageW( g_hWnd_shutdown_action, CB_ADDSTRING, 0, ( LPARAM )ST_V_Sleep );
			_SendMessageW( g_hWnd_shutdown_action, CB_ADDSTRING, 0, ( LPARAM )ST_V_Hibernate );
			_SendMessageW( g_hWnd_shutdown_action, CB_ADDSTRING, 0, ( LPARAM )ST_V_Shut_down );
			if ( g_is_windows_8_or_higher )
			{
				_SendMessageW( g_hWnd_shutdown_action, CB_ADDSTRING, 0, ( LPARAM )ST_V_Hybrid_shut_down );
			}

			_SendMessageW( g_hWnd_shutdown_action, CB_SETCURSEL, cfg_shutdown_action, 0 );



			HWND hWnd_static_default_download_directory = _CreateWindowW( WC_STATIC, ST_V_Default_download_directory_, WS_CHILD | WS_VISIBLE, 0, 287, rc.right, 15, hWnd, NULL, NULL, NULL );
			g_hWnd_default_download_directory = _CreateWindowExW( WS_EX_CLIENTEDGE, WC_EDIT, cfg_default_download_directory, ES_AUTOHSCROLL | ES_READONLY | WS_CHILD | WS_TABSTOP | WS_VISIBLE, 0, 302, rc.right - 40, 23, hWnd, NULL, NULL, NULL );
			g_hWnd_btn_default_download_directory = _CreateWindowW( WC_BUTTON, ST_V_BTN___, WS_CHILD | WS_TABSTOP | WS_VISIBLE, rc.right - 35, 302, 35, 23, hWnd, ( HMENU )BTN_DEFAULT_DOWNLOAD_DIRECTORY, NULL, NULL );


			g_hWnd_chk_temp_download_directory = _CreateWindowW( WC_BUTTON, ST_V_Use_temporary_download_directory_, BS_AUTOCHECKBOX | WS_CHILD | WS_TABSTOP | WS_VISIBLE, 0, 330, rc.right - 10, 20, hWnd, ( HMENU )BTN_USE_TEMP_DOWNLOAD_DIRECTORY, NULL, NULL );
			g_hWnd_temp_download_directory = _CreateWindowExW( WS_EX_CLIENTEDGE, WC_EDIT, cfg_temp_download_directory, ES_AUTOHSCROLL | ES_READONLY | WS_CHILD | WS_TABSTOP | WS_VISIBLE, 0, 350, rc.right - 40, 23, hWnd, NULL, NULL, NULL );
			g_hWnd_btn_temp_download_directory = _CreateWindowW( WC_BUTTON, ST_V_BTN___, WS_CHILD | WS_TABSTOP | WS_VISIBLE, rc.right - 35, 350, 35, 23, hWnd, ( HMENU )BTN_TEMP_DOWNLOAD_DIRECTORY, NULL, NULL );


			HWND hWnd_static_thread_count = _CreateWindowW( WC_STATIC, ST_V_Thread_pool_count_, WS_CHILD | WS_VISIBLE, 0, 384, 190, 15, hWnd, NULL, NULL, NULL );
			g_hWnd_thread_count = _CreateWindowExW( WS_EX_CLIENTEDGE, WC_EDIT, NULL, ES_AUTOHSCROLL | ES_CENTER | ES_NUMBER | WS_CHILD | WS_TABSTOP | WS_VISIBLE, rc.right - 100, 380, 100, 23, hWnd, ( HMENU )EDIT_THREAD_COUNT, NULL, NULL );

			g_hWnd_ud_thread_count = _CreateWindowW( UPDOWN_CLASS, NULL, UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_NOTHOUSANDS | UDS_SETBUDDYINT | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd, NULL, NULL, NULL );

			_SendMessageW( g_hWnd_thread_count, EM_LIMITTEXT, 10, 0 );
			_SendMessageW( g_hWnd_ud_thread_count, UDM_SETBUDDY, ( WPARAM )g_hWnd_thread_count, 0 );
			_SendMessageW( g_hWnd_ud_thread_count, UDM_SETBASE, 10, 0 );
			_SendMessageW( g_hWnd_ud_thread_count, UDM_SETRANGE32, 1, g_max_threads );
			_SendMessageW( g_hWnd_ud_thread_count, UDM_SETPOS, 0, cfg_thread_count );

			RECT rc_spinner;
			_GetClientRect( g_hWnd_ud_thread_count, &rc_spinner );
			int spinner_width = rc_spinner.right - rc_spinner.left;

			_SetWindowPos( g_hWnd_thread_count, HWND_TOP, rc.right - ( 100 + spinner_width ), 380, 100, 23, SWP_NOZORDER );
			_SetWindowPos( g_hWnd_ud_thread_count, HWND_TOP, rc.right - spinner_width, 380, 0, 0, SWP_NOZORDER | SWP_NOSIZE );


			_SendMessageW( g_hWnd_chk_download_history, WM_SETFONT, ( WPARAM )g_hFont, 0 );
			_SendMessageW( g_hWnd_chk_quick_allocation, WM_SETFONT, ( WPARAM )g_hFont, 0 );
			_SendMessageW( g_hWnd_chk_set_filetime, WM_SETFONT, ( WPARAM )g_hFont, 0 );
			_SendMessageW( g_hWnd_chk_update_redirected, WM_SETFONT, ( WPARAM )g_hFont, 0 );
			_SendMessageW( g_hWnd_chk_use_one_instance, WM_SETFONT, ( WPARAM )g_hFont, 0 );
			_SendMessageW( g_hWnd_chk_prevent_standby, WM_SETFONT, ( WPARAM )g_hFont, 0 );
			_SendMessageW( g_hWnd_chk_resume_downloads, WM_SETFONT, ( WPARAM )g_hFont, 0 );

			_SendMessageW( hWnd_static_drag_and_drop_action, WM_SETFONT, ( WPARAM )g_hFont, 0 );
			_SendMessageW( g_hWnd_drag_and_drop_action, WM_SETFONT, ( WPARAM )g_hFont, 0 );

			_SendMessageW( hWnd_static_prompt_last_modified, WM_SETFONT, ( WPARAM )g_hFont, 0 );
			_SendMessageW( g_hWnd_prompt_last_modified, WM_SETFONT, ( WPARAM )g_hFont, 0 );

			_SendMessageW( hWnd_static_prompt_rename, WM_SETFONT, ( WPARAM )g_hFont, 0 );
			_SendMessageW( g_hWnd_prompt_rename, WM_SETFONT, ( WPARAM )g_hFont, 0 );

			_SendMessageW( hWnd_static_prompt_file_size, WM_SETFONT, ( WPARAM )g_hFont, 0 );
			_SendMessageW( g_hWnd_max_file_size, WM_SETFONT, ( WPARAM )g_hFont, 0 );
			_SendMessageW( g_hWnd_prompt_file_size, WM_SETFONT, ( WPARAM )g_hFont, 0 );

			_SendMessageW( hWnd_static_shutdown_action, WM_SETFONT, ( WPARAM )g_hFont, 0 );
			_SendMessageW( g_hWnd_shutdown_action, WM_SETFONT, ( WPARAM )g_hFont, 0 );

			_SendMessageW( hWnd_static_default_download_directory, WM_SETFONT, ( WPARAM )g_hFont, 0 );
			_SendMessageW( g_hWnd_default_download_directory, WM_SETFONT, ( WPARAM )g_hFont, 0 );
			_SendMessageW( g_hWnd_btn_default_download_directory, WM_SETFONT, ( WPARAM )g_hFont, 0 );

			_SendMessageW( g_hWnd_chk_temp_download_directory, WM_SETFONT, ( WPARAM )g_hFont, 0 );
			_SendMessageW( g_hWnd_temp_download_directory, WM_SETFONT, ( WPARAM )g_hFont, 0 );
			_SendMessageW( g_hWnd_btn_temp_download_directory, WM_SETFONT, ( WPARAM )g_hFont, 0 );

			_SendMessageW( hWnd_static_thread_count, WM_SETFONT, ( WPARAM )g_hFont, 0 );
			_SendMessageW( g_hWnd_thread_count, WM_SETFONT, ( WPARAM )g_hFont, 0 );


			_SendMessageW( g_hWnd_chk_download_history, BM_SETCHECK, ( cfg_enable_download_history ? BST_CHECKED : BST_UNCHECKED ), 0 );
			_SendMessageW( g_hWnd_chk_quick_allocation, BM_SETCHECK, ( cfg_enable_quick_allocation ? BST_CHECKED : BST_UNCHECKED ), 0 );
			_SendMessageW( g_hWnd_chk_set_filetime, BM_SETCHECK, ( cfg_set_filetime ? BST_CHECKED : BST_UNCHECKED ), 0 );
			_SendMessageW( g_hWnd_chk_update_redirected, BM_SETCHECK, ( cfg_update_redirected ? BST_CHECKED : BST_UNCHECKED ), 0 );
			_SendMessageW( g_hWnd_chk_use_one_instance, BM_SETCHECK, ( cfg_use_one_instance ? BST_CHECKED : BST_UNCHECKED ), 0 );
			_SendMessageW( g_hWnd_chk_prevent_standby, BM_SETCHECK, ( cfg_prevent_standby ? BST_CHECKED : BST_UNCHECKED ), 0 );
			_SendMessageW( g_hWnd_chk_resume_downloads, BM_SETCHECK, ( cfg_resume_downloads ? BST_CHECKED : BST_UNCHECKED ), 0 );

			if ( cfg_use_temp_download_directory )
			{
				_SendMessageW( g_hWnd_chk_temp_download_directory, BM_SETCHECK, BST_CHECKED, 0 );
				_EnableWindow( g_hWnd_temp_download_directory, TRUE );
				_EnableWindow( g_hWnd_btn_temp_download_directory, TRUE );
			}
			else
			{
				_SendMessageW( g_hWnd_chk_temp_download_directory, BM_SETCHECK, BST_UNCHECKED, 0 );
				_EnableWindow( g_hWnd_temp_download_directory, FALSE );
				_EnableWindow( g_hWnd_btn_temp_download_directory, FALSE );
			}

			if ( cfg_default_download_directory != NULL )
			{
				t_default_download_directory = GlobalStrDupW( cfg_default_download_directory );
			}

			if ( cfg_sound_file_path != NULL )
			{
				t_sound_file_path = GlobalStrDupW( cfg_sound_file_path );
			}

			if ( cfg_temp_download_directory != NULL )
			{
				t_temp_download_directory = GlobalStrDupW( cfg_temp_download_directory );
			}

			return 0;
		}
		break;

		case WM_COMMAND:
		{
			switch ( LOWORD( wParam ) )
			{
				case BTN_DOWNLOAD_HISTORY:
				case BTN_QUICK_ALLOCATION:
				case BTN_SET_FILETIME:
				case BTN_UPDATE_REDIRECTED:
				case BTN_USE_ONE_INSTANCE:
				case BTN_PREVENT_STANDBY:
				case BTN_RESUME_DOWNLOADS:
				{
					options_state_changed = true;
					_EnableWindow( g_hWnd_options_apply, TRUE );
				}
				break;

				case EDIT_MAX_FILE_SIZE:
				{
					if ( HIWORD( wParam ) == EN_UPDATE )
					{
						DWORD sel_start;

						char value[ 21 ];
						_SendMessageA( ( HWND )lParam, WM_GETTEXT, 21, ( LPARAM )value );
						unsigned long long num = strtoull( value );

						if ( num == 0xFFFFFFFFFFFFFFFF )
						{
							_SendMessageA( ( HWND )lParam, EM_GETSEL, ( WPARAM )&sel_start, NULL );

							_SendMessageA( ( HWND )lParam, WM_SETTEXT, 0, ( LPARAM )"18446744073709551615" );

							_SendMessageA( ( HWND )lParam, EM_SETSEL, sel_start, sel_start );
						}
						else if ( num == 0 )
						{
							num = 1;

							_SendMessageA( ( HWND )lParam, EM_GETSEL, ( WPARAM )&sel_start, NULL );

							_SendMessageA( ( HWND )lParam, WM_SETTEXT, 0, ( LPARAM )"1" );

							_SendMessageA( ( HWND )lParam, EM_SETSEL, sel_start, sel_start );
						}

						unsigned int length = FormatSizes( file_size_tooltip_text, 32, SIZE_FORMAT_AUTO, num );
						file_size_tooltip_text[ length ] = 0;

						TOOLINFO ti;
						_memzero( &ti, sizeof( TOOLINFO ) );
						ti.cbSize = sizeof( TOOLINFO );
						ti.hwnd = g_hWnd_max_file_size;
						ti.lpszText = file_size_tooltip_text;
						_SendMessageW( g_hWnd_file_size_tooltip, TTM_UPDATETIPTEXT, 0, ( LPARAM )&ti );

						if ( num != cfg_max_file_size )
						{
							options_state_changed = true;
							_EnableWindow( g_hWnd_options_apply, TRUE );
						}
					}
				}
				break;

				case CB_DRAG_AND_DROP_ACTION:
				case CB_PROMPT_RENAME:
				case CB_PROMPT_FILE_SIZE:
				case CB_PROMPT_LAST_MODIFIED:
				case CB_SHUTDOWN_ACTION:
				{
					if ( HIWORD( wParam ) == CBN_SELCHANGE )
					{
						options_state_changed = true;
						_EnableWindow( g_hWnd_options_apply, TRUE );
					}
				}
				break;

				case BTN_DEFAULT_DOWNLOAD_DIRECTORY:
				{
					wchar_t *directory = NULL;

					_BrowseForFolder( hWnd, ST_V_Select_the_default_download_directory, &directory );

					if ( directory != NULL )
					{
						GlobalFree( t_default_download_directory );
						t_default_download_directory = directory;

						_SendMessageW( g_hWnd_default_download_directory, WM_SETTEXT, 0, ( LPARAM )t_default_download_directory );

						options_state_changed = true;
						_EnableWindow( g_hWnd_options_apply, TRUE );
					}
				}
				break;

				case BTN_USE_TEMP_DOWNLOAD_DIRECTORY:
				{
					bool show_dialog = false;

					if ( _SendMessageW( g_hWnd_chk_temp_download_directory, BM_GETCHECK, 0, 0 ) == BST_CHECKED )
					{
						if ( _SendMessageW( g_hWnd_temp_download_directory, WM_GETTEXTLENGTH, 0, 0 ) == 0 )
						{
							show_dialog = true;
						}

						_EnableWindow( g_hWnd_temp_download_directory, TRUE );
						_EnableWindow( g_hWnd_btn_temp_download_directory, TRUE );
					}
					else
					{
						_EnableWindow( g_hWnd_temp_download_directory, FALSE );
						_EnableWindow( g_hWnd_btn_temp_download_directory, FALSE );
					}

					// Fall through if we haven't chosen a file yet.
					if ( !show_dialog )
					{
						options_state_changed = true;
						_EnableWindow( g_hWnd_options_apply, TRUE );

						break;
					}
				}

				case BTN_TEMP_DOWNLOAD_DIRECTORY:
				{
					wchar_t *directory = NULL;

					_BrowseForFolder( hWnd, ST_V_Select_the_temporary_download_directory, &directory );

					if ( directory != NULL )
					{
						GlobalFree( t_temp_download_directory );
						t_temp_download_directory = directory;

						_SendMessageW( g_hWnd_temp_download_directory, WM_SETTEXT, 0, ( LPARAM )t_temp_download_directory );

						options_state_changed = true;
						_EnableWindow( g_hWnd_options_apply, TRUE );
					}
					else
					{
						if ( _SendMessageW( g_hWnd_chk_temp_download_directory, BM_GETCHECK, 0, 0 ) == BST_CHECKED &&
							 _SendMessageW( g_hWnd_temp_download_directory, WM_GETTEXTLENGTH, 0, 0 ) == 0 )
						{
							_SendMessageW( g_hWnd_chk_temp_download_directory, BM_SETCHECK, BST_UNCHECKED, 0 );
							_EnableWindow( g_hWnd_temp_download_directory, FALSE );
							_EnableWindow( g_hWnd_btn_temp_download_directory, FALSE );
						}
					}
				}
				break;

				case EDIT_THREAD_COUNT:
				{
					if ( HIWORD( wParam ) == EN_UPDATE )
					{
						DWORD sel_start;

						char value[ 11 ];
						_SendMessageA( ( HWND )lParam, WM_GETTEXT, 11, ( LPARAM )value );
						unsigned long num = _strtoul( value, NULL, 10 );

						if ( num > g_max_threads )
						{
							_SendMessageA( ( HWND )lParam, EM_GETSEL, ( WPARAM )&sel_start, NULL );

							__snprintf( value, 11, "%lu", g_max_threads );
							_SendMessageA( ( HWND )lParam, WM_SETTEXT, 0, ( LPARAM )value );

							_SendMessageA( ( HWND )lParam, EM_SETSEL, sel_start, sel_start );
						}
						else if ( num == 0 )
						{
							_SendMessageA( ( HWND )lParam, EM_GETSEL, ( WPARAM )&sel_start, NULL );

							_SendMessageA( ( HWND )lParam, WM_SETTEXT, 0, ( LPARAM )"1" );

							_SendMessageA( ( HWND )lParam, EM_SETSEL, sel_start, sel_start );
						}

						if ( num != cfg_thread_count )
						{
							options_state_changed = true;
							_EnableWindow( g_hWnd_options_apply, TRUE );
						}
					}
				}
				break;
			}

			return 0;
		}
		break;

		case WM_DESTROY:
		{
			if ( t_default_download_directory != NULL )
			{
				GlobalFree( t_default_download_directory );
				t_default_download_directory = NULL;
			}

			if ( t_temp_download_directory != NULL )
			{
				GlobalFree( t_temp_download_directory );
				t_temp_download_directory = NULL;
			}

			return 0;
		}
		break;

		default:
		{
			return _DefWindowProcW( hWnd, msg, wParam, lParam );
		}
		break;
	}
	//return TRUE;
}
