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
function void NL_DrawCppTokenColors(Application_Links *app, Text_Layout_ID text_layout_id, Token_Array *array);
function void NL_DrawBraceHighlight(Application_Links *app, Buffer_ID buffer, Text_Layout_ID text_layout_id, i64 position, ARGB_Color *colors, i32 color_count);
function void NL_DrawDividerComment(Application_Links *app, Buffer_ID buffer, View_ID view, Text_Layout_ID text_layout_id);
function void NL_DrawCursorMark(Application_Links *app, View_ID view_id, b32 is_active_view, Buffer_ID buffer, Text_Layout_ID text_layout_id, f32 roundness, f32 outline_thickness, Frame_Info frame_info);

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
  // TODO(Nghia Lam): Draw my own file bar, currently draw default file bar for now.
  draw_file_bar(app, view_id, buffer, face_id, bar);
}

function void NL_DrawCppTokenColors(Application_Links *app,
                                    Text_Layout_ID text_layout_id,
                                    Token_Array *array) {
  // TODO(Nghia Lam): Draw my own cpp token, currently draw default by now.
  draw_cpp_token_colors(app, text_layout_id, array);
}

function void NL_DrawBraceHighlight(Application_Links *app,
                                    Buffer_ID buffer,
                                    Text_Layout_ID text_layout_id,
                                    i64 position,
                                    ARGB_Color *colors,
                                    i32 color_count) {
  ProfileScope(app, "[Nghia Lam] Brace Highlight");
  
  Token_Array token_array = get_token_array_from_buffer(app, buffer);
  if (token_array.tokens != 0) {
    Token_Iterator_Array it = token_iterator_pos(0, &token_array, position);
    Token *token = token_it_read(&it);
    if(token != 0 && token->kind == TokenBaseKind_ScopeOpen) {
      position = token->pos + token->size;
    } else {
      
      if(token_it_dec_all(&it)) {
        token = token_it_read(&it);
        
        if (token->kind == TokenBaseKind_ScopeClose &&
            position == token->pos + token->size) {
          position = token->pos;
        }
      }
    }
  }
  
  draw_enclosures(app,
                  text_layout_id, 
                  buffer,
                  position,
                  FindNest_Scope,
                  RangeHighlightKind_CharacterHighlight,
                  0, 0, 
                  colors, 
                  color_count);
}

function void NL_DrawDividerComment(Application_Links *app,
                                    Buffer_ID buffer,
                                    View_ID view,
                                    Text_Layout_ID text_layout_id) {
  ProfileScope(app, "[Nghia Lam] Divider Comments");
  
  String_Const_u8 divider_comment_signifier = string_u8_litexpr("//~");
  
  Token_Array token_array = get_token_array_from_buffer(app, buffer);
  Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
  Scratch_Block scratch(app);
  
  if(token_array.tokens != 0) {
    i64 first_index         = token_index_from_pos(&token_array, visible_range.first);
    Token_Iterator_Array it = token_iterator_index(0, &token_array, first_index);
    
    Token *token = 0;
    for (;;) {
      token = token_it_read(&it);
      if(token->pos >= visible_range.one_past_last 
         || !token 
         || !token_it_inc_non_whitespace(&it)) {
        break;
      }
      
      if(token->kind == TokenBaseKind_Comment) {
        Rect_f32 comment_first_char_rect = text_layout_character_on_screen(app, 
                                                                           text_layout_id, 
                                                                           token->pos);
        
        Range_i64 token_range = {
          token->pos,
          token->pos + (token->size > (i64)divider_comment_signifier.size
                        ? (i64)divider_comment_signifier.size
                        : token->size),
        };
        
        u8 token_buffer[256] = {0};
        buffer_read_range(app, buffer, token_range, token_buffer);
        String_Const_u8 token_string = { token_buffer, (u64)(token_range.end - token_range.start) };
        
        if(string_match(token_string, divider_comment_signifier)) {
          // NOTE(rjf): Render divider line.
          Rect_f32 rect = {
            comment_first_char_rect.x0,
            comment_first_char_rect.y0-2,
            10000,
            comment_first_char_rect.y0,
          };
          f32 roundness = 4.f;
          draw_rectangle(app, rect, roundness, fcolor_resolve(fcolor_id(defcolor_comment)));
        }
      }
    }
  }
}

function void NL_DrawCursorMark(Application_Links *app, 
                                View_ID view_id, 
                                b32 is_active_view, 
                                Buffer_ID buffer, 
                                Text_Layout_ID text_layout_id, 
                                f32 roundness, 
                                f32 outline_thickness,
                                Frame_Info frame_info) {
  b32 has_highlight_range = draw_highlight_range(app, view_id, buffer, text_layout_id, roundness);
  if (!has_highlight_range) {
    i32 cursor_sub_id       = default_cursor_sub_id();
    i64 cursor_pos          = view_get_cursor_pos(app, view_id);
    i64 mark_pos            = view_get_mark_pos(app, view_id);
    Rect_f32 view_rect      = view_get_screen_rect(app, view_id);
    Rect_f32 clip           = draw_set_clip(app, view_rect);
    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
    
    // NOTE(Nghia Lam): Draw main cursor
    if (is_active_view) {
      Rect_f32 target_cursor = text_layout_character_on_screen(app, text_layout_id, cursor_pos);
      if(cursor_pos < visible_range.start || cursor_pos > visible_range.end) {
        f32 width = target_cursor.x1 - target_cursor.x0;
        target_cursor.x0 = view_rect.x0;
        target_cursor.x1 = target_cursor.x0 + width;
      }
      
      NL_CursorInterpolation(app, frame_info, &global_cursor_rect, &global_last_cursor_rect, target_cursor);
      
      Rect_f32 target_mark = text_layout_character_on_screen(app, text_layout_id, mark_pos);
      if(mark_pos > visible_range.end) {
        target_mark.x0 = 0;
        target_mark.y0 = view_rect.y1;
        target_mark.y1 = view_rect.y1;
      }
      
      if(mark_pos < visible_range.start || mark_pos > visible_range.end) {
        f32 width = target_mark.x1 - target_mark.x0;
        target_mark.x0 = view_rect.x0;
        target_mark.x1 = target_mark.x0 + width;
      }
      
      NL_CursorInterpolation(app, frame_info, &global_mark_rect, &global_last_mark_rect, target_mark);
      
      draw_rectangle(app,
                     global_cursor_rect,
                     roundness, 
                     fcolor_resolve(fcolor_id(defcolor_cursor, cursor_sub_id)));
      paint_text_color_pos(app, 
                           text_layout_id, 
                           cursor_pos, 
                           fcolor_id(defcolor_at_cursor));
      // draw_rectangle_outline(app, global_mark_rect, roundness, outline_thickness, fcolor_resolve(fcolor_id(defcolor_mark)));
      
    } else {
      draw_character_wire_frame(app, 
                                text_layout_id, 
                                mark_pos,
                                roundness, 
                                outline_thickness,
                                fcolor_id(defcolor_mark));
      draw_character_wire_frame(app,
                                text_layout_id,
                                cursor_pos, 
                                roundness, 
                                outline_thickness, 
                                fcolor_id(defcolor_cursor, cursor_sub_id));
    }
    
    draw_set_clip(app, clip);
  }
  
  // NOTE(Nghia Lam): Draw mark highlight in insert mode
  if (is_active_view && NL_IsVimInsertMode(global_vim_mode)) {
    Rect_f32 view_rect = view_get_screen_rect(app, view_id);
    Rect_f32 clip = draw_set_clip(app, view_rect);
    
    f32 lower_bound_y;
    f32 upper_bound_y;
    f32 lower_bound_x;
    f32 upper_bound_x;
    
    if(global_last_cursor_rect.y0 < global_last_mark_rect.y0) {
      lower_bound_y = global_last_cursor_rect.y0;
      upper_bound_y = global_last_mark_rect.y1;
    } else {
      lower_bound_y = global_last_mark_rect.y0;
      upper_bound_y = global_last_cursor_rect.y1;
    }
    
    if (global_last_cursor_rect.x0 < global_last_mark_rect.x0) {
      lower_bound_x = global_last_cursor_rect.x0;
      upper_bound_x = global_last_mark_rect.x1;
    } else {
      lower_bound_x = global_last_mark_rect.x0;
      upper_bound_x = global_last_cursor_rect.x1;
    }
    
    draw_rectangle_outline(app, 
                           Rf32(lower_bound_x, lower_bound_y, upper_bound_x, upper_bound_y), 
                           2.f,
                           3.f,
                           fcolor_resolve(fcolor_change_alpha(fcolor_id(defcolor_comment), 0.5f)));
    draw_set_clip(app, clip);
  }
  
}

#endif // FCODER_NGHIALAM_DRAW