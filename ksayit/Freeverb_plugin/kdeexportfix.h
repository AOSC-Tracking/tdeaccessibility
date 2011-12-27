/***************************************************** vim:set ts=4 sw=4 sts=4:
  tdelibs < 3.3.2 had a bug in the KDE_EXPORT macro.  This file fixes this
  by undefining it.
  -------------------
  Copyright : (C) 2005 by Gary Cramblitt <garycramblitt@comcast.net>
  -------------------
  Original author: Gary Cramblitt <garycramblitt@comcast.net>
 ******************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 ***************************************************************************/

#include <tdeversion.h>
#if TDE_VERSION < TDE_MAKE_VERSION (3,3,2)
#undef KDE_EXPORT
#define KDE_EXPORT
#endif
