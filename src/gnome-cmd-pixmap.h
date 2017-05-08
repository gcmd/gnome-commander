/** 
 * @file gnome-cmd-pixmap.h
 * @copyright (C) 2001-2006 Marcus Bjurman\n
 * @copyright (C) 2007-2012 Piotr Eljasiak\n
 * @copyright (C) 2013-2016 Uwe Scholz\n
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

#ifndef __GNOME_CMD_PIXMAP_H__
#define __GNOME_CMD_PIXMAP_H__

struct GnomeCmdPixmap
{
    GdkPixbuf *pixbuf;
    GdkPixmap *pixmap;
    GdkBitmap *mask;
    gint width;
    gint height;
};


GnomeCmdPixmap *gnome_cmd_pixmap_new_from_file (const gchar *filepath, int width=-1, int height=-1);
GnomeCmdPixmap *gnome_cmd_pixmap_new_from_icon (const gchar *icon_name, gint size, GtkIconLookupFlags flags=(GtkIconLookupFlags) 0);
GnomeCmdPixmap *gnome_cmd_pixmap_new_from_pixbuf (GdkPixbuf *pixbuf);

inline void gnome_cmd_pixmap_free (GnomeCmdPixmap *pixmap)
{
    if (!pixmap)
        return;

    g_return_if_fail (pixmap->pixbuf != NULL);
    g_return_if_fail (pixmap->pixmap != NULL);
    g_return_if_fail (pixmap->mask != NULL);

    g_object_unref (pixmap->pixbuf);
    g_object_unref (pixmap->pixmap);
    g_object_unref (pixmap->mask);

    g_free (pixmap);
}

#endif // __GNOME_CMD_PIXMAP_H__
