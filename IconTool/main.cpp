// WinIconPatcher - An icon patcher for Windows
// Copyright (C) 2022  namazso <admin@namazso.eu>
// Copyright (C) 2023  ItsProfessional <itsprofessional1234@gmail.com>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "pch.h"

#include "dlg.h"
#include "MainDialog.h"
#include "utl.h"

#pragma comment(linker, \
  "\"/manifestdependency:type='Win32' "\
  "name='Microsoft.Windows.Common-Controls' "\
  "version='6.0.0.0' "\
  "processorArchitecture='*' "\
  "publicKeyToken='6595b64144ccf1df' "\
  "language='*'\"")


#pragma comment(lib, "ComCtl32.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "ntdll.lib")


int APIENTRY wWinMain(
	_In_ HINSTANCE     instance,
	_In_opt_ HINSTANCE prev_instance,
	_In_ LPWSTR        cmd_line,
	_In_ int           cmd_show
)
{

	// we aren't using these perameters, and if we dont use the below lines, it will give warnings in compile time
	UNREFERENCED_PARAMETER(prev_instance);
	UNREFERENCED_PARAMETER(cmd_line);


	// this is a warning, disabled for the time being to make debugging less annoying
	MessageBoxW(
	  nullptr,
	  ESTRt(L"This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details."),
	  ESTRt(L"Warranty disclaimer"),
	  MB_OK | MB_ICONWARNING
	);



	INITCOMMONCONTROLSEX iccex
	{
	  sizeof(INITCOMMONCONTROLSEX),
	  ICC_WIN95_CLASSES
	};



#define POST_ERROR(...) utl::FormattedMessageBox(nullptr, _T("Error"), MB_OK | MB_ICONERROR, __VA_ARGS__) // Macro for showing errors

	if (!InitCommonControlsEx(&iccex))
		return POST_ERROR(ESTRt(L"InitCommonControlsEx failed, LastError = %08X"), GetLastError());


	const auto dialog = CreateDialogParam(instance, MAKEINTRESOURCE(IDD_MAIN), nullptr, &DlgProcClassBinder<MainDialog>, 0);

	RECT rc;
	GetWindowRect(dialog, &rc);

	const auto hwnd_monitor = MonitorFromWindow(dialog, MONITOR_DEFAULTTONEAREST);
	MONITORINFO monitor_info{};
	monitor_info.cbSize = sizeof(monitor_info);
	GetMonitorInfoW(hwnd_monitor, &monitor_info);

	const auto& monitor_rc = monitor_info.rcMonitor;

	const auto dialog_width = rc.right - rc.left;
	const auto dialog_height = rc.bottom - rc.top;

	const auto x = monitor_rc.left + (monitor_rc.right - monitor_rc.left - dialog_width) / 2;
	const auto y = monitor_rc.top + (monitor_rc.bottom - monitor_rc.top - dialog_height) / 2;

	SetWindowPos(dialog, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER); // Set window position to be both vertically and horizontally centered on the screen.
	ShowWindow(dialog, cmd_show);



	MSG msg{};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!IsDialogMessage(dialog, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}