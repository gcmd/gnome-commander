/**
 * @file dirlist.cc
 * @copyright (C) 2001-2006 Marcus Bjurman\n
 * @copyright (C) 2007-2012 Piotr Eljasiak\n
 * @copyright (C) 2013-2021 Uwe Scholz\n
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

#include "gnome-cmd-includes.h"
#include "dirlist.h"
#include "gnome-cmd-data.h"
#include "utils.h"

using namespace std;


#define FILES_PER_NOTIFICATION 50
#define LIST_PRIORITY 0

void async_list (GnomeCmdDir *dir);
void sync_list  (GnomeCmdDir *dir);

static void
on_files_listed (GnomeVFSAsyncHandle *handle,
                 GnomeVFSResult result,
                 GList *list,
                 guint entries_read,
                 GnomeCmdDir *dir)
{
    DEBUG('l', "on_files_listed\n");

    if (result != GNOME_VFS_OK && result != GNOME_VFS_ERROR_EOF)
    {
        DEBUG ('l', "Directory listing failed, %s\n", gnome_vfs_result_to_string (result));
        dir->state = GnomeCmdDir::STATE_EMPTY;
        dir->list_result = result;
    }

    if (entries_read > 0 && list != NULL)
    {
        g_list_foreach (list, (GFunc) gnome_vfs_file_info_ref, NULL);
        dir->infolist = g_list_concat (dir->infolist, g_list_copy (list));
        dir->list_counter += entries_read;
        DEBUG ('l', "files listed: %d\n", dir->list_counter);
    }

    if (result == GNOME_VFS_ERROR_EOF)
    {
        dir->state = GnomeCmdDir::STATE_LISTED;
        dir->list_result = GNOME_VFS_OK;
        DEBUG('l', "All files listed\n");
    }
}


static gboolean update_list_progress (GnomeCmdDir *dir)
{
    DEBUG ('l', "Checking list progress...\n");

    if (dir->state == GnomeCmdDir::STATE_LISTING)
    {
        gchar *msg = g_strdup_printf (ngettext ("%d file listed", "%d files listed", dir->list_counter), dir->list_counter);
        gtk_label_set_text (GTK_LABEL (dir->label), msg);
        progress_bar_update (dir->pbar, 50);
        DEBUG('l', "%s\n", msg);
        g_free (msg);
        return TRUE;
    }

    DEBUG ('l', "calling list_done func\n");
    dir->done_func (dir, dir->infolist, dir->list_result);
    return FALSE;
}


void async_list (GnomeCmdDir *dir)
{
    DEBUG('l', "async_list\n");

    GnomeVFSFileInfoOptions infoOpts = (GnomeVFSFileInfoOptions) (GNOME_VFS_FILE_INFO_FOLLOW_LINKS | GNOME_VFS_FILE_INFO_GET_MIME_TYPE);

    GnomeVFSURI *uri = GNOME_CMD_FILE (dir)->get_uri();
    gchar *uri_str = gnome_vfs_uri_to_string (uri, GNOME_VFS_URI_HIDE_PASSWORD);

    DEBUG('l', "async_list: %s\n", uri_str);

    g_free (uri_str);

    gnome_vfs_async_load_directory_uri (&dir->list_handle,
                                        uri,
                                        infoOpts,
                                        FILES_PER_NOTIFICATION,
                                        LIST_PRIORITY,
                                        (GnomeVFSAsyncDirectoryLoadCallback) on_files_listed,
                                        dir);

    g_timeout_add (gnome_cmd_data.gui_update_rate, (GSourceFunc) update_list_progress, dir);
}

static void enumerate_children_callback(GObject *direnum, GAsyncResult *result, gpointer user_data);

void sync_list (GnomeCmdDir *dir)
{
    GError *error = nullptr;
    //GnomeVFSFileInfoOptions infoOpts = (GnomeVFSFileInfoOptions) (GNOME_VFS_FILE_INFO_FOLLOW_LINKS | GNOME_VFS_FILE_INFO_GET_MIME_TYPE);

    gchar *uri_str = GNOME_CMD_FILE (dir)->get_uri_str();
    DEBUG('l', "sync_list: %s\n", uri_str);

    dir->infolist = NULL;
    //dir->list_result = gnome_vfs_directory_list_load (&dir->infolist, uri_str, infoOpts);

    auto gFile = GNOME_CMD_FILE (dir)->gFile;

    auto gFileEnumerator = g_file_enumerate_children (gFile,
                            "*",
                            G_FILE_QUERY_INFO_NONE,
                            nullptr,
                            &error);
    if( error )
    {
        g_critical("Unable to enumerate children, error: %s", error->message);
        g_error_free(error);
        return;
    }

    //GList *gFileInfoList = nullptr;
    g_file_enumerator_next_files_async(gFileEnumerator,
                    2,
                    G_PRIORITY_LOW,
                    nullptr,
                    enumerate_children_callback,
                    dir);

    //enumerate_children_callback(G_OBJECT(gFileEnumerator), nullptr, dir);
    g_object_unref(gFileEnumerator);

    g_free (uri_str);

    dir->state = dir->list_result==GNOME_VFS_OK ? GnomeCmdDir::STATE_LISTED : GnomeCmdDir::STATE_EMPTY;
    //dir->done_func (dir, dir->infolist, dir->list_result);
    //dir->done_func (dir, dir->gFileInfoList, dir->list_result);
}

static void enumerate_children_callback(GObject *direnum, GAsyncResult *result, gpointer user_data)
{
    auto gFileEnumerator = G_FILE_ENUMERATOR(direnum);
    auto dir = GNOME_CMD_DIR(user_data);
    GError *error = nullptr;

    GList *gFileInfosList = g_file_enumerator_next_files_finish( gFileEnumerator,
                    result, &error);
    if( error )
    {
        g_critical("Unable to add files to list, error: %s", error->message);
        g_object_unref(direnum);
        g_error_free(error);
        return;
    }
    else if( gFileInfosList == nullptr )
    {
        /* DONE */
        dir->state = dir->list_result==GNOME_VFS_OK ? GnomeCmdDir::STATE_LISTED : GnomeCmdDir::STATE_EMPTY;
        dir->done_func (dir, dir->gFileInfoList, dir->list_result);
        g_object_unref(direnum);
        return;
    }
    else
    {
        dir->gFileInfoList = g_list_concat (dir->gFileInfoList, g_list_copy (gFileInfosList));
        //g_list_foreach (list, (GFunc) gnome_vfs_file_info_ref, NULL);
        //dir->list_counter += entries_read;
        //DEBUG ('l', "files listed: %d\n", dir->list_counter);

        //GList *node = gFileInfosList;
        //GFileInfo *info;
        //while(node)
        //{
        //    info = (GFileInfo *) node->data;
        //    node = node->next;
        //    g_object_unref(info);
        //}

        g_file_enumerator_next_files_async(G_FILE_ENUMERATOR(direnum),
                        20,
                        G_PRIORITY_LOW,
                        nullptr,
                        enumerate_children_callback,
                        dir);
    }
    g_list_free(gFileInfosList);
}


void dirlist_list (GnomeCmdDir *dir, gboolean visualProgress)
{
    g_return_if_fail (GNOME_CMD_IS_DIR (dir));

    dir->infolist = NULL;
    dir->list_handle = NULL;
    dir->list_counter = 0;
    dir->list_result = GNOME_VFS_OK;
    dir->state = GnomeCmdDir::STATE_LISTING;

    if (!visualProgress)
    {
        sync_list (dir);
        return;
    }

    async_list (dir);
}


void dirlist_cancel (GnomeCmdDir *dir)
{
    dir->state = GnomeCmdDir::STATE_EMPTY;
    dir->list_result = GNOME_VFS_OK;

    DEBUG('l', "Calling async_cancel\n");
    gnome_vfs_async_cancel (dir->list_handle);
}
