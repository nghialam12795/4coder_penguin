/**
A file written by:
 _______          .__    .__         .____
 \      \    ____ |  |__ |__|____    |    |   _____    _____
 /   |   \  / ___\|  |  \|  \__  \   |    |   \__  \  /     \
/    |    \/ /_/  >   Y  \  |/ __ \_ |    |___ / __ \|  Y Y  \
\____|__  /\___  /|___|  /__(____  / |_______ (____  /__|_|  /
        \//_____/      \/        \/          \/    \/      \/
--------------------------------------------------------------
License: GNU GENERAL PUBLIC LICENSE Version 3

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

static Face_ID global_styled_title_face = 0;
static Face_ID global_styled_label_face = 0;
static Face_ID global_small_code_face = 0;

static Rect_f32 global_cursor_rect = {0};
static Rect_f32 global_last_cursor_rect = {0};
static Rect_f32 global_mark_rect = {0};
static Rect_f32 global_last_mark_rect = {0};

/////////////////////////////////////////////////////////////////////////////
// STATE                                                                   //
/////////////////////////////////////////////////////////////////////////////

namespace {
    // Bookmark bookmarks[bookmark_max_count] = { };
    // f32 auto_bookmark_time_remaining = bookmark_auto_seconds;
    // i64 bookmark_cursor = 0;
    
    b32 show_function_helper = false;
}
