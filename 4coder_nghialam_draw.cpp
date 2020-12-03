// ==========================================================================
// A file written by:
//  _______          .__    .__         .____
//  \      \    ____ |  |__ |__|____    |    |   _____    _____
//  /   |   \  / ___\|  |  \|  \__  \   |    |   \__  \  /     \
// /    |    \/ /_/  >   Y  \  |/ __ \_ |    |___ / __ \|  Y Y  \
// \____|__  /\___  /|___|  /__(____  / |_______ (____  /__|_|  /
//    /_____/     \/      \/        \/                \/      \/
// --------------------------------------------------------------
// License: GNU GENERAL PUBLIC LICENSE Version 3
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
// ==========================================================================

#if !defined(FCODER_NGHIALAM_DRAW)
#define FCODER_NGHIALAM_DRAW

//~ NOTE(Nghia Lam): Main drawing API
function Rect_f32 NL_DrawBackground(Application_Links* app, View_ID view, b32 is_active_view, String_Const_u8 buffer_name);
function void NL_DrawFileBar(Application_Links *app, View_ID view_id, Buffer_ID buffer, Face_ID face_id, Rect_f32 bar);

//~ NOTE(Nghia Lam): My Implementation
function Rect_f32 NL_DrawBackground(Application_Links* app, 
                                    View_ID view,
                                    b32 is_active_view,
                                    String_Const_u8 buffer_name) {
  Rect_f32 view_rect =  view_get_screen_rect(app, view);
  // TODO(Nghia Lam): Let the user settings for the width ????
  Rect_f32 region    = rect_inner(view_rect, 1.0f);
  
  ARGB_Color back_argb   = fcolor_resolve(fcolor_id(defcolor_back));
  // ARGB_Color margin_argb = fcolor_resolve(fcolor_id(defcolor_margin));
  
  if (string_match(buffer_name, string_u8_litexpr("*compilation*"))) {
    back_argb = color_blend(back_argb, 0.5f, 0xff000000);
  }
  
  // TODO(Nghia Lam): Investigate this margin color
  draw_rectangle(app, region, 0.f, back_argb);
  draw_margin(app, view_rect, region,back_argb);
  
  return region;
}

function void NL_DrawFileBar(Application_Links *app, 
                             View_ID view_id, 
                             Buffer_ID buffer, 
                             Face_ID face_id, 
                             Rect_f32 bar) {
  // TODO(Nghia Lam): Draw my own file bar, currently draw default file bar for now
  draw_file_bar(app, view_id, buffer, face_id, bar);
}

#endif // FCODER_NGHIALAM_DRAW