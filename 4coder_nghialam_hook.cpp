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

#if !defined(FCODER_NGHIALAM_HOOKS)
#define FCODER_NGHIALAM_HOOKS

//~ NOTE(Nghia Lam): Main Hooks
function void NL_SetupCustomHooks(Application_Links *app);
function void NL_Tick(Application_Links *app, Frame_Info frame_info);
function void NL_RenderCaller(Application_Links *app, Frame_Info frame_info, View_ID view_id);
function void NL_WholeScreenRenderCaller(Application_Links *app, Frame_Info frame_info);
function void NL_RenderBuffer(Application_Links *app, View_ID view_id, Face_ID face_id, Buffer_ID buffer, Text_Layout_ID text_layout_id, Rect_f32 rect);

//~ NOTE(Nghia Lam): My Implementation
function void NL_SetupCustomHooks(Application_Links *app) {
  set_custom_hook(app, HookID_Tick,                    NL_Tick);
  set_custom_hook(app, HookID_RenderCaller,            NL_RenderCaller);
  set_custom_hook(app, HookID_WholeScreenRenderCaller, NL_WholeScreenRenderCaller);
}

function void NL_Tick(Application_Links *app, Frame_Info frame_info) {
  // Default tick stuffs from Allen
  default_tick(app, frame_info);
  
  // TODO(Nghia Lam): My tick function here
}

function void NL_RenderCaller(Application_Links *app, 
                              Frame_Info frame_info, 
                              View_ID view_id) {
  ProfileScope(app, "[NghiaLam] Render Caller");
  Scratch_Block scratch(app);
  
  View_ID         active_view    = get_active_view(app, Access_Always);
  b32             is_active_view = (active_view == view_id);
  Buffer_ID       buffer         = view_get_buffer(app, view_id, Access_Always);
  String_Const_u8 buffer_name    = push_buffer_base_name(app, scratch, buffer);
  
  // NOTE(Nghia Lam): Render Background
  Rect_f32 region    = NL_DrawBackground(app, view_id, is_active_view, buffer_name);
  Rect_f32 prev_clip = draw_set_clip(app, region);
  
  Face_ID      face_id       = get_face_id(app, buffer);
  Face_Metrics face_metrics  = get_face_metrics(app, face_id);
  f32          line_height   = face_metrics.line_height;
  f32          digit_advance = face_metrics.decimal_digit_advance;
  
  // NOTE(Nghia Lam): Render Filebar
  b64 showing_file_bar = false;
  if(view_get_setting(app, view_id, ViewSetting_ShowFileBar, &showing_file_bar) && showing_file_bar) {
    if (global_filebar_top) {
      Rect_f32_Pair pair = layout_file_bar_on_top(region, line_height);
      NL_DrawFileBar(app, view_id, buffer, face_id, pair.min);
      region = pair.max;
    } else {
      Rect_f32_Pair pair = layout_file_bar_on_bot(region, line_height);
      NL_DrawFileBar(app, view_id, buffer, face_id, pair.max);
      region = pair.min;
    }
  }
  
  Buffer_Scroll scroll = view_get_buffer_scroll(app, view_id);
  Buffer_Point_Delta_Result delta = delta_apply(app, view_id, frame_info.animation_dt, scroll);
  
  if(!block_match_struct(&scroll.position, &delta.point)) {
    block_copy_struct(&scroll.position, &delta.point);
    view_set_buffer_scroll(app, view_id, scroll, SetBufferScroll_NoCursorChange);
  }
  if(delta.still_animating) {
    animate_in_n_milliseconds(app, 0);
  }
  
  // NOTE(Nghia Lam): Render query bar
  // TODO(Nghia Lam): Custom render this with vim query
  {
    Query_Bar *space[32];
    Query_Bar_Ptr_Array query_bars = {};
    query_bars.ptrs                = space;
    if (get_active_query_bars(app, view_id, ArrayCount(space), &query_bars)){
      for (i32 i = 0; i < query_bars.count; i += 1){
        Rect_f32_Pair pair = layout_query_bar_on_top(region, line_height, 1);
        draw_query_bar(app, query_bars.ptrs[i], face_id, pair.min);
        region = pair.max;
      }
    }
  }
  
  // NOTE(Nghia Lam): Render fps hub <- Do we really need this??
  if (show_fps_hud){
    Rect_f32_Pair pair = layout_fps_hud_on_bottom(region, line_height);
    draw_fps_hud(app, frame_info, face_id, pair.max);
    region = pair.min;
    animate_in_n_milliseconds(app, 1000);
  }
  
  // NOTE(Nghia Lam): Layout line number
  Rect_f32 line_number_rect = {};
  if (global_config.show_line_number_margins){
    Rect_f32_Pair pair  = layout_line_number_margin(app, buffer, region, digit_advance);
    line_number_rect    = pair.min;
    region = pair.max;
  }
  
  // NOTE(Nghia Lam): Begin buffer render
  Buffer_Point buffer_point = scroll.position;
  Text_Layout_ID text_layout_id = text_layout_create(app, buffer, region, buffer_point);
  
  // NOTE(Nghia Lam): Render line numbers
  if (global_config.show_line_number_margins){
    draw_line_number_margin(app, view_id, buffer, face_id, text_layout_id, line_number_rect);
  }
  
  // NOTE(Nghia Lam): Render buffer
  NL_RenderBuffer(app, view_id, face_id, buffer, text_layout_id, region);
  
  // NOTE(rjf): Dim inactive rectangle
  if(is_active_view == 0) {
    draw_rectangle(app, region, 0.f, 0x44000000);
  }
  
  // NOTE(Nghia Lam): Cleanup
  text_layout_free(app, text_layout_id);
  draw_set_clip(app, prev_clip);
}

function void NL_WholeScreenRenderCaller(Application_Links *app, 
                                         Frame_Info frame_info) {
  // TODO(Nghia Lam): Find a way to work with this, maybe render item list and todo list here.
}


function void NL_RenderBuffer(Application_Links *app, 
                              View_ID view_id,
                              Face_ID face_id,
                              Buffer_ID buffer,
                              Text_Layout_ID text_layout_id,
                              Rect_f32 rect) {
  ProfileScope(app, "[NghiaLam] Render Buffer");
  
  View_ID active_view = get_active_view(app, Access_Always);
  b32 is_active_view  = (active_view == view_id);
  Rect_f32 prev_clip  = draw_set_clip(app, rect);
  
  // NOTE(Nghia Lam): Token coloring
  Token_Array token_array = get_token_array_from_buffer(app, buffer);
  if (token_array.tokens != 0){
    // TODO(Nghia Lam): Support other code type token.
    NL_DrawCppTokenColors(app, text_layout_id, &token_array);
    
    // NOTE(Nghia Lam): Scan for TODOs, NOTEs and user name
    if (global_config.use_comment_keyword){
      char user_string_buf[256] = {0};
      String_Const_u8 user_string = {0};
      {
        user_string.data = user_string_buf;
        user_string.size = snprintf(user_string_buf, 
                                    sizeof(user_string_buf), 
                                    "(%.*s)",
                                    string_expand(global_config.user_name));
      }
      Comment_Highlight_Pair pairs[] = {
        {string_u8_litexpr("NOTE"), finalize_color(defcolor_comment_pop, 0)},
        {string_u8_litexpr("TODO"), finalize_color(defcolor_comment_pop, 1)},
        {user_string, 0xffffdd23}, // TODO(Nghia Lam): Use from config file for this color.
      };
      draw_comment_highlights(app, buffer, text_layout_id, &token_array, pairs, ArrayCount(pairs));
    }
  }
  else{
    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
    paint_text_color_fcolor(app, text_layout_id, visible_range, fcolor_id(defcolor_text_default));
  }
  
  i64 cursor_pos = view_correct_cursor(app, view_id);
  view_correct_mark(app, view_id);
  
  // NOTE(Allen): Scope highlight
  if(global_config.use_scope_highlight) {
    Color_Array colors = finalize_color_array(defcolor_back_cycle);
    draw_scope_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
  }
  
  // NOTE(Nghia Lam): Brace Highlight
  // TODO(Nghia Lam): Use global config for this settings
  {
    ARGB_Color colors[] = {
      0xff8ffff2,
    };
    NL_DrawBraceHighlight(app,
                          buffer,
                          text_layout_id,
                          cursor_pos,
                          colors,
                          sizeof(colors)/sizeof(colors[0]));
  }
  
  if (global_config.use_error_highlight || global_config.use_jump_highlight) {
    // NOTE(Allen): Error highlight
    String_Const_u8 name = string_u8_litexpr("*compilation*");
    Buffer_ID compilation_buffer = get_buffer_by_name(app, name, Access_Always);
    if (global_config.use_error_highlight) {
      draw_jump_highlights(app,
                           buffer,
                           text_layout_id,
                           compilation_buffer,
                           fcolor_id(defcolor_highlight_junk));
    }
    
    // NOTE(Allen): Search highlight
    if (global_config.use_jump_highlight) {
      Buffer_ID jump_buffer = get_locked_jump_buffer(app);
      if (jump_buffer != compilation_buffer) {
        draw_jump_highlights(app,
                             buffer,
                             text_layout_id,
                             jump_buffer, 
                             fcolor_id(defcolor_highlight_white));
      }
    }
  }
  
  // NOTE(Allen): Line highlight
  if (global_config.highlight_line_at_cursor && is_active_view){
    i64 line_number = get_line_number_from_pos(app, buffer, cursor_pos);
    draw_line_highlight(app,
                        text_layout_id,
                        line_number,
                        fcolor_id(defcolor_highlight_cursor_line));
  }
  
  // NOTE(Allen): Color parens
  if (global_config.use_paren_helper) {
    Color_Array colors = finalize_color_array(defcolor_text_cycle);
    draw_paren_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
  }
  
  // NOTE(Allen): Cursor
  Face_Metrics metrics = get_face_metrics(app, face_id);
  f32 cursor_roundness = metrics.normal_advance * global_config.cursor_roundness;
  f32 mark_thickness   = (f32) global_config.mark_thickness;
  
  switch (fcoder_mode) {
    case FCoderMode_Original: {
      draw_original_4coder_style_cursor_mark_highlight(app, 
                                                       view_id, 
                                                       is_active_view, 
                                                       buffer, 
                                                       text_layout_id, 
                                                       cursor_roundness, 
                                                       mark_thickness);
    } break;
    case FCoderMode_NotepadLike: {
      draw_notepad_style_cursor_highlight(app, view_id, buffer, text_layout_id, cursor_roundness);
    } break;
  }
  
  // NOTE(Nghia Lam): Divider Comments
  NL_DrawDividerComment(app, buffer, view_id, text_layout_id);
  
  // NOTE(Allen): Fade ranges
  paint_fade_ranges(app, text_layout_id, buffer);
  
  // NOTE(Allen): put the actual text on the actual screen
  draw_text_layout_default(app, text_layout_id);
  
  draw_set_clip(app, prev_clip);
}

#endif // FCODER_NGHIALAM_HOOKS