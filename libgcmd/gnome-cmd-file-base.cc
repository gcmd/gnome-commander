/**
 * @file gnome-cmd-file-base.cc
 * @copyright (C) 2001-2006 Marcus Bjurman\n
 * @copyright (C) 2007-2012 Piotr Eljasiak\n
 * @copyright (C) 2013-2020 Uwe Scholz\n
 *
 * @copyright This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * @copyright This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * @copyright You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <config.h>

#include "libgcmd-deps.h"
#include "gnome-cmd-file-base.h"

using namespace std;


G_DEFINE_TYPE (GnomeCmdFileBase, gnome_cmd_file_base, G_TYPE_OBJECT)


static void gnome_cmd_file_base_init (GnomeCmdFileBase *self)
{
}


static void gnome_cmd_file_base_finalize (GObject *object)
{
    GnomeCmdFileBase *self = GNOME_CMD_FILE_BASE (object);

    gnome_vfs_file_info_unref (self->gnomeVfsFileInfo);
    if (self->uri)
        gnome_vfs_uri_unref (self->uri);
    if (self->gFile)
        g_object_unref(self->gFile);

    G_OBJECT_CLASS (gnome_cmd_file_base_parent_class)->finalize (object);
}


static void gnome_cmd_file_base_class_init (GnomeCmdFileBaseClass *klass)
{
    gnome_cmd_file_base_parent_class = (GObjectClass *) g_type_class_peek_parent (klass);

    GObjectClass *object_class = (GObjectClass *) klass;

    object_class->finalize = gnome_cmd_file_base_finalize;
}