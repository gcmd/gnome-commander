/*
    GNOME Commander - A GNOME based file manager 
    Copyright (C) 2001-2004 Marcus Bjurman

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/ 
#ifndef __GNOME_CMD_PREPARE_COPY_DIALOG_H__
#define __GNOME_CMD_PREPARE_COPY_DIALOG_H__

#include "gnome-cmd-file-list.h"
#include "gnome-cmd-file-selector.h"

void
gnome_cmd_prepare_copy_dialog_show (GnomeCmdFileSelector *fs,
									GnomeCmdFileSelector *to);

#endif //__GNOME_CMD_PREPARE_COPY_DIALOG_H__
