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
function void NL_Tick(Application_Links *app, Frame_Info frame_info);
function void NL_RenderCaller(Application_Links *app, Frame_Info frame_info, View_ID view_id);
function void NL_WholeScreenRenderCaller(Application_Links *app, Frame_Info frame_info);
function void NL_RenderBuffer(Application_Links *app, View_ID view_id, Face_ID face_id, Buffer_ID buffer, Text_Layout_ID text_layout_id, Rect_f32 rect);

//~ NOTE(Nghia Lam): My Implementation
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
    line_number_rect.x1 += 4;
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
  
  // NOTE(rjf): Draw inactive rectangle
  if(is_active_view == 0) {
    draw_rectangle(app, view_rect, 0.f, 0x44000000);
  }
  
  // NOTE(Nghia Lam): Cleanup
  text_layout_free(app, text_layout_id);
  draw_set_clip(app, prev_clip);
}

function void NL_WholeScreenRenderCaller(Application_Links *app, 
                                         Frame_Info frame_info) {
  
}


function void NL_RenderBuffer(Application_Links *app, 
                              View_ID view_id,
                              Face_ID face_id,
                              Buffer_ID buffer,
                              Text_Layout_ID text_layout_id,
                              Rect_f32 rect) {
  // TODO(Nghia Lam): Custom drawing buffer, this is leave default for now
  default_render_buffer(app, view_id, face_id, buffer, text_layout_id, rect);
}

#endif // FCODER_NGHIALAM_HOOKS