/* WinIconPatcher - An icon patcher for Windows
 * Copyright (C) 2022  namazso <admin@namazso.eu>
 * Copyright (C) 2023  ItsProfessional <itsprofessional1234@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef THEMETOOL_INCLUDED
#define THEMETOOL_INCLUDED 1

#include <Windows.h>

#include <string>

EXTERN_C_START

/**
 * @brief Get count of themes, according to Windows.
 * @param count Count of themes
 * @return A HRESULT
 */
void icontool_get_pack_count(PULONG count);

/**
 * @brief Get a specific theme.
 * @param idx Index of theme to get
 * @param theme Output opaque pointer
 * @return A HRESULT
 * @warning This is intentionally an opaque pointer, as it's your job to keep the interface up to date if you wish to
 *          use it. Use the other exported functions for management with guarantees.
 */
void icontool_get_pack(ULONG idx, std::string& pack);

/**
 * @brief Ignore background.
 */
#define THEMETOOL_APPLY_FLAG_IGNORE_BACKGROUND    (ULONG)(1 << 0)
 /**
  * @brief Ignore cursor.
  */
#define THEMETOOL_APPLY_FLAG_IGNORE_CURSOR        (ULONG)(1 << 1)
  /**
   * @brief Ignore desktop icons.
   */
#define THEMETOOL_APPLY_FLAG_IGNORE_DESKTOP_ICONS (ULONG)(1 << 2)
   /**
	* @brief Ignore color accent.
	*/
#define THEMETOOL_APPLY_FLAG_IGNORE_COLOR         (ULONG)(1 << 3)
	/**
	 * @brief Ignore sounds.
	 */
#define THEMETOOL_APPLY_FLAG_IGNORE_SOUND         (ULONG)(1 << 4)
	 /**
	  * @brief Ignore screensaver.
	  */
#define THEMETOOL_APPLY_FLAG_IGNORE_SCREENSAVER   (ULONG)(1 << 5)
	  /**
	   * @brief Unknown, maybe ignore window metrics.
	   */
#define THEMETOOL_APPLY_FLAG_UNKNOWN              (ULONG)(1 << 6)
	   /**
		* @brief Unknown.
		*/
#define THEMETOOL_APPLY_FLAG_UNKNOWN2             (ULONG)(1 << 7)
		/**
		 * @brief Suppress hourglass.
		 */
#define THEMETOOL_APPLY_FLAG_NO_HOURGLASS         (ULONG)(1 << 8)

		 /**
		  * @brief Unknown, seems to suppress hourglass.
		  */
#define THEMETOOL_PACK_FLAG_UNKNOWN1              (ULONG)(1 << 0)
		  /**
		   * @brief Unknown, seems to suppress hourglass.
		   */
#define THEMETOOL_PACK_FLAG_UNKNOWN2              (ULONG)(1 << 1)
		   /**
			* @brief Hides all dialogs and prevents sound.
			*/
#define THEMETOOL_PACK_FLAG_SILENT                (ULONG)(1 << 2)
			/**
			 * @brief Roamed.
			 */
#define THEMETOOL_PACK_FLAG_ROAMED                (ULONG)(1 << 3)


/**
 * @brief Get display name of a theme.
 * @param theme Opaque theme pointer
 * @param out Output buffer
 * @param cch Output buffer length
 * @return A HRESULT
 * @retval HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER) Buffer too small
 */
void icontool_pack_get_display_name(std::string& pack, LPWSTR out, SIZE_T cch);

/**
 * @brief Get visual style path of a theme.
 * @param theme Opaque theme pointer
 * @param out Output buffer
 * @param cch Output buffer length
 * @return A HRESULT
 * @retval HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER) Buffer too small
 */
void icontool_pack_get_vs_path(std::string& pack, LPWSTR out, SIZE_T cch);

EXTERN_C_END

#endif