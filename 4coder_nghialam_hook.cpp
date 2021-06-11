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

function i32 NL_BeginBuffer(Application_Links *app, Buffer_ID buffer_id);
function void NL_Tick(Application_Links *app, Frame_Info frame_info);
function void NL_RenderCaller(Application_Links *app, Frame_Info frame_info, View_ID view_id);
function void NL_WholeScreenRenderCaller(Application_Links *app, Frame_Info frame_info);
function void NL_RenderBuffer(Application_Links *app, View_ID view_id, Face_ID face_id, Buffer_ID buffer, Text_Layout_ID text_layout_id, Rect_f32 rect, Frame_Info frame_info);

//~ NOTE(Nghia Lam): My Implementation
function void NL_SetupCustomHooks(Application_Links *app)
{
  set_custom_hook(app, HookID_Tick, NL_Tick);
  
  set_custom_hook(app, HookID_BeginBuffer, NL_BeginBuffer);
  set_custom_hook(app, HookID_RenderCaller, NL_RenderCaller);
  
  //set_custom_hook(app, HookID_RenderCaller,            F4_Render);
  //set_custom_hook(app, HookID_BeginBuffer,             F4_BeginBuffer);
  set_custom_hook(app, HookID_Layout, F4_Layout);
  set_custom_hook(app, HookID_WholeScreenRenderCaller, F4_WholeScreenRender);
  set_custom_hook(app, HookID_DeltaRule, F4_DeltaRule);
  set_custom_hook(app, HookID_BufferEditRange, F4_BufferEditRange);
  
  set_custom_hook_memory_size(app, HookID_DeltaRule, delta_ctx_size(sizeof(Vec2_f32)));
}

BUFFER_HOOK_SIG(NL_BeginBuffer)
{
  ProfileScope(app, "[Nghia Lam] Begin Buffer");
  Scratch_Block scratch(app);
  b32 treat_as_code = false;
  String_Const_u8 file_name = push_buffer_file_name(app, scratch, buffer_id);
  String_Const_u8 buffer_name = push_buffer_base_name(app, scratch, buffer_id);
  
  if (treat_as_code == false)
  {
    if (file_name.size > 0)
    {
      String_Const_u8 treat_as_code_string = def_get_config_string(scratch, vars_save_string_lit("treat_as_code"));
      String_Const_u8_Array extensions = parse_extension_line_to_extension_list(app, scratch, treat_as_code_string);
      String_Const_u8 ext = string_file_extension(file_name);
      
      for (i32 i = 0; i < extensions.count; ++i)
      {
        if (string_match(ext, extensions.strings[i]))
        {
          treat_as_code = true;
          break;
        }
      }
    }
  }
  
  // NOTE(Nghia Lam): Treat as code for *calc* and *peek* buffers.
  if (treat_as_code == false)
  {
    if (buffer_name.size > 0)
    {
      if (string_match(buffer_name, string_u8_litexpr("*calc*")))
      {
        treat_as_code = true;
      }
      else if (string_match(buffer_name, string_u8_litexpr("*peek*")))
      {
        treat_as_code = true;
      }
    }
  }
  
  // NOTE(Nghia Lam): Treat as code if we've identified the language of a file.
  if (treat_as_code == false)
  {
    F4_Language *language = F4_LanguageFromBuffer(app, buffer_id);
    if (language)
    {
      treat_as_code = true;
    }
  }
  
  // NOTE(Nghia Lam): Change to vim normal map as default when open a file
  String_ID vim_mapid_normal = vars_save_string_lit("keys_vim_normal");
  Managed_Scope scope = buffer_get_managed_scope(app, buffer_id);
  Command_Map_ID *map_id_ptr = scope_attachment(app, scope, buffer_map_id, Command_Map_ID);
  *map_id_ptr = vim_mapid_normal;
  
  // String_ID file_map_id = vars_save_string_lit("keys_file");
  // String_ID code_map_id = vars_save_string_lit("keys_code");
  
  // Command_Map_ID map_id = (treat_as_code) ? (code_map_id) : (file_map_id);
  // Managed_Scope scope = buffer_get_managed_scope(app, buffer_id);
  // Command_Map_ID *map_id_ptr = scope_attachment(app, scope, buffer_map_id, Command_Map_ID);
  // *map_id_ptr = map_id;
  
  Line_Ending_Kind setting = guess_line_ending_kind_from_buffer(app, buffer_id);
  Line_Ending_Kind *eol_setting = scope_attachment(app, scope, buffer_eol_setting, Line_Ending_Kind);
  *eol_setting = setting;
  
  // NOTE(allen): Decide buffer settings
  b32 wrap_lines = true;
  b32 use_lexer = false;
  if (treat_as_code)
  {
    wrap_lines = def_get_config_b32(vars_save_string_lit("enable_code_wrapping"));
    use_lexer = true;
  }
  if (string_match(buffer_name, string_u8_litexpr("*compilation*")))
  {
    wrap_lines = false;
  }
  
  if (use_lexer)
  {
    ProfileBlock(app, "begin buffer kick off lexer");
    Async_Task *lex_task_ptr = scope_attachment(app, scope, buffer_lex_task, Async_Task);
    *lex_task_ptr = async_task_no_dep(&global_async_system, do_full_lex_async, make_data_struct(&buffer_id));
  }
  
  {
    b32 *wrap_lines_ptr = scope_attachment(app, scope, buffer_wrap_lines, b32);
    *wrap_lines_ptr = wrap_lines;
  }
  
  if (use_lexer)
  {
    buffer_set_layout(app, buffer_id, layout_virt_indent_index_generic);
  }
  else
  {
    if (treat_as_code)
    {
      buffer_set_layout(app, buffer_id, layout_virt_indent_literal_generic);
    }
    else
    {
      buffer_set_layout(app, buffer_id, layout_generic);
    }
  }
  
  // No meaning when return
  return 0;
}

function void NL_Tick(Application_Links *app, Frame_Info frame_info)
{
  // NOTE(Nghia Lam): Setup from fleury layers
  linalloc_clear(&global_frame_arena);
  global_tooltip_count = 0;
  
  F4_TickColors(app, frame_info);
  F4_Index_Tick(app);
  F4_CLC_Tick(frame_info);
  F4_PowerMode_Tick(app, frame_info);
  F4_UpdateFlashes(app, frame_info);
  
  // Default tick stuffs from Allen
  default_tick(app, frame_info);
}

function void NL_RenderCaller(Application_Links *app, Frame_Info frame_info, View_ID view_id)
{
  F4_RecentFiles_RefreshView(app, view_id);
  
  ProfileScope(app, "[NghiaLam] Render Caller");
  Scratch_Block scratch(app);
  
  View_ID active_view = get_active_view(app, Access_Always);
  b32 is_active_view = (active_view == view_id);
  Buffer_ID buffer = view_get_buffer(app, view_id, Access_Always);
  String_Const_u8 buffer_name = push_buffer_base_name(app, scratch, buffer);
  
  // NOTE(Nghia Lam): Render Background
  Rect_f32 region = NL_DrawBackground(app, view_id, is_active_view, buffer_name);
  Rect_f32 prev_clip = draw_set_clip(app, region);
  
  Face_ID face_id = get_face_id(app, buffer);
  Face_Metrics face_metrics = get_face_metrics(app, face_id);
  f32 line_height = face_metrics.line_height;
  f32 digit_advance = face_metrics.decimal_digit_advance;
  
  // NOTE(Nghia Lam): Render Filebar
  b64 showing_file_bar = false;
  if (view_get_setting(app, view_id, ViewSetting_ShowFileBar, &showing_file_bar) && showing_file_bar)
  {
    if (global_filebar_top)
    {
      Rect_f32_Pair pair = layout_file_bar_on_top(region, line_height);
      NL_DrawFileBar(app, view_id, buffer, face_id, pair.min);
      region = pair.max;
    }
    else
    {
      Rect_f32_Pair pair = layout_file_bar_on_bot(region, line_height);
      NL_DrawFileBar(app, view_id, buffer, face_id, pair.max);
      region = pair.min;
    }
  }
  
  Buffer_Scroll scroll = view_get_buffer_scroll(app, view_id);
  Buffer_Point_Delta_Result delta = delta_apply(app, view_id, frame_info.animation_dt, scroll);
  
  if (!block_match_struct(&scroll.position, &delta.point))
  {
    block_copy_struct(&scroll.position, &delta.point);
    view_set_buffer_scroll(app, view_id, scroll, SetBufferScroll_NoCursorChange);
  }
  if (delta.still_animating)
  {
    animate_in_n_milliseconds(app, 0);
  }
  
  // NOTE(Nghia Lam): Render query bar
  // TODO(Nghia Lam): Custom render this with vim query
  {
    Query_Bar *space[32];
    Query_Bar_Ptr_Array query_bars = {};
    query_bars.ptrs = space;
    if (get_active_query_bars(app, view_id, ArrayCount(space), &query_bars))
    {
      for (i32 i = 0; i < query_bars.count; i += 1)
      {
        Rect_f32_Pair pair = layout_query_bar_on_top(region, line_height, 1);
        draw_query_bar(app, query_bars.ptrs[i], face_id, pair.min);
        region = pair.max;
      }
    }
  }
  
  // NOTE(Nghia Lam): Render fps hub <- Do we really need this??
  if (show_fps_hud)
  {
    Rect_f32_Pair pair = layout_fps_hud_on_bottom(region, line_height);
    draw_fps_hud(app, frame_info, face_id, pair.max);
    region = pair.min;
    animate_in_n_milliseconds(app, 1000);
  }
  
  // NOTE(Nghia Lam): Layout line number
  Rect_f32 line_number_rect = {};
  if (def_get_config_b32(vars_save_string_lit("show_line_number_margins")))
  {
    Rect_f32_Pair pair = layout_line_number_margin(app, buffer, region, digit_advance);
    line_number_rect = pair.min;
    line_number_rect.x1 += 4;
    region = pair.max;
  }
  
  // NOTE(Nghia Lam): Begin buffer render
  Buffer_Point buffer_point = scroll.position;
  if (is_active_view)
  {
    buffer_point.pixel_shift.y += F4_PowerMode_ScreenShake() * 1.f;
  }
  Text_Layout_ID text_layout_id = text_layout_create(app, buffer, region, buffer_point);
  
  // NOTE(Nghia Lam): Render line numbers
  if (def_get_config_b32(vars_save_string_lit("show_line_number_margins")))
  {
    draw_line_number_margin(app, view_id, buffer, face_id, text_layout_id, line_number_rect);
  }
  
  // NOTE(Nghia Lam): Render buffer
  NL_RenderBuffer(app, view_id, face_id, buffer, text_layout_id, region, frame_info);
  
  // NOTE(rjf): Dim inactive rectangle
  if (is_active_view == 0)
  {
    draw_rectangle(app, region, 0.f, 0x44000000);
  }
  
  // NOTE(rjf): Render command server
#if OS_WINDOWS
  CS_render_caller(app, frame_info, view_id);
#endif
  
  // NOTE(Nghia Lam): Cleanup
  text_layout_free(app, text_layout_id);
  draw_set_clip(app, prev_clip);
}

function void NL_WholeScreenRenderCaller(Application_Links *app,
                                         Frame_Info frame_info)
{
  // TODO(Nghia Lam): Find a way to work with this, maybe render item list and todo list here.
}

function void NL_RenderBuffer(Application_Links *app,
                              View_ID view_id,
                              Face_ID face_id,
                              Buffer_ID buffer,
                              Text_Layout_ID text_layout_id,
                              Rect_f32 rect,
                              Frame_Info frame_info)
{
  Scratch_Block scratch(app);
  ProfileScope(app, "[NghiaLam] Render Buffer");
  
  View_ID active_view = get_active_view(app, Access_Always);
  b32 is_active_view = (active_view == view_id);
  Rect_f32 prev_clip = draw_set_clip(app, rect);
  
  // NOTE(Nghia Lam): Token coloring
  Token_Array token_array = get_token_array_from_buffer(app, buffer);
  if (token_array.tokens != 0)
  {
    F4_SyntaxHighlight(app, text_layout_id, &token_array);
    
    // NOTE(Nghia Lam): Scan for TODOs, NOTEs and user name
    b32 use_comment_keywords = def_get_config_b32(vars_save_string_lit("use_comment_keywords"));
    if (use_comment_keywords)
    {
      Comment_Highlight_Pair pairs[] =
      {
        {str8_lit("NOTE"), finalize_color(defcolor_comment_pop, 0)},
        {str8_lit("TODO"), finalize_color(defcolor_comment_pop, 1)},
        {def_get_config_string(scratch, vars_save_string_lit("user_name")), finalize_color(fleury_color_comment_user_name, 0)},
      };
      draw_comment_highlights(app, buffer, text_layout_id, &token_array, pairs, ArrayCount(pairs));
    }
  }
  else
  {
    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
    paint_text_color_fcolor(app, text_layout_id, visible_range, fcolor_id(defcolor_text_default));
  }
  
  i64 cursor_pos = view_correct_cursor(app, view_id);
  view_correct_mark(app, view_id);
  
  // NOTE(Allen): Scope highlight
  b32 use_scope_highlight = def_get_config_b32(vars_save_string_lit("use_scope_highlight"));
  if (use_scope_highlight)
  {
    Color_Array colors = finalize_color_array(defcolor_back_cycle);
    draw_scope_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
  }
  
  // NOTE(rjf): Brace highlight
  {
    Color_Array colors = finalize_color_array(fleury_color_brace_highlight);
    if (colors.count >= 1 && F4_ARGBIsValid(colors.vals[0]))
    {
      F4_Brace_RenderHighlight(app, buffer, text_layout_id, cursor_pos,
                               colors.vals, colors.count);
    }
  }
  
  // NOTE(allen): Line highlight
  {
    b32 highlight_line_at_cursor = def_get_config_b32(vars_save_string_lit("highlight_line_at_cursor"));
    String_Const_u8 name = string_u8_litexpr("*compilation*");
    Buffer_ID compilation_buffer = get_buffer_by_name(app, name, Access_Always);
    if (highlight_line_at_cursor && (is_active_view || buffer == compilation_buffer))
    {
      i64 line_number = get_line_number_from_pos(app, buffer, cursor_pos);
      draw_line_highlight(app, text_layout_id, line_number,
                          fcolor_id(defcolor_highlight_cursor_line));
    }
  }
  
  // NOTE(rjf): Error/Search Highlight
  {
    b32 use_error_highlight = def_get_config_b32(vars_save_string_lit("use_error_highlight"));
    b32 use_jump_highlight = def_get_config_b32(vars_save_string_lit("use_jump_highlight"));
    if (use_error_highlight || use_jump_highlight)
    {
      // NOTE(allen): Error highlight
      String_Const_u8 name = string_u8_litexpr("*compilation*");
      Buffer_ID compilation_buffer = get_buffer_by_name(app, name, Access_Always);
      if (use_error_highlight)
      {
        draw_jump_highlights(app, buffer, text_layout_id, compilation_buffer,
                             fcolor_id(defcolor_highlight_junk));
      }
      
      // NOTE(allen): Search highlight
      if (use_jump_highlight)
      {
        Buffer_ID jump_buffer = get_locked_jump_buffer(app);
        if (jump_buffer != compilation_buffer)
        {
          draw_jump_highlights(app, buffer, text_layout_id, jump_buffer,
                               fcolor_id(defcolor_highlight_white));
        }
      }
    }
  }
  
  // NOTE(rjf): Error annotations
  {
    String_Const_u8 name = string_u8_litexpr("*compilation*");
    Buffer_ID compilation_buffer = get_buffer_by_name(app, name, Access_Always);
    F4_RenderErrorAnnotations(app, buffer, text_layout_id, compilation_buffer);
  }
  
  // NOTE(jack): Token Occurance Highlight
  if (!def_get_config_b32(vars_save_string_lit("f4_disable_cursor_token_occurance")))
  {
    ProfileScope(app, "[Fleury] Token Occurance Highlight");
    
    // NOTE(jack): Get the active cursor's token string
    Buffer_ID active_cursor_buffer = view_get_buffer(app, active_view, Access_Always);
    i64 active_cursor_pos = view_get_cursor_pos(app, active_view);
    Token_Array active_cursor_buffer_tokens = get_token_array_from_buffer(app, active_cursor_buffer);
    Token_Iterator_Array active_cursor_it = token_iterator_pos(0, &active_cursor_buffer_tokens, active_cursor_pos);
    Token *active_cursor_token = token_it_read(&active_cursor_it);
    
    String_Const_u8 active_cursor_string = string_u8_litexpr("");
    if (active_cursor_token)
    {
      active_cursor_string = push_buffer_range(app, scratch, active_cursor_buffer, Ii64(active_cursor_token));
      
      // Loop the visible tokens
      Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
      i64 first_index = token_index_from_pos(&token_array, visible_range.first);
      Token_Iterator_Array it = token_iterator_index(0, &token_array, first_index);
      for (;;)
      {
        Token *token = token_it_read(&it);
        if (!token || token->pos >= visible_range.one_past_last)
        {
          break;
        }
        
        if (token->kind == TokenBaseKind_Identifier)
        {
          Range_i64 token_range = Ii64(token);
          String_Const_u8 token_string = push_buffer_range(app, scratch, buffer, token_range);
          
          // NOTE(jack) If this is the buffers cursor token, highlight it with an Underline
          if (range_contains(token_range, view_get_cursor_pos(app, view_id)))
          {
            F4_RenderRangeHighlight(app, view_id, text_layout_id,
                                    token_range, F4_RangeHighlightKind_Underline,
                                    fcolor_resolve(fcolor_id(fleury_color_token_highlight)));
          }
          // NOTE(jack): If the token matches the active buffer token. highlight it with a Minor Underline
          else if (active_cursor_token->kind == TokenBaseKind_Identifier &&
                   string_match(token_string, active_cursor_string))
          {
            F4_RenderRangeHighlight(app, view_id, text_layout_id,
                                    token_range, F4_RangeHighlightKind_MinorUnderline,
                                    fcolor_resolve(fcolor_id(fleury_color_token_minor_highlight)));
          }
        }
        
        if (!token_it_inc_non_whitespace(&it))
        {
          break;
        }
      }
    }
  }
  // NOTE(jack): if "f4_disable_cursor_token_occurance" is set, just highlight the cusror
  else
  {
    ProfileScope(app, "[Fleury] Token Highlight");
    
    Token_Iterator_Array it = token_iterator_pos(0, &token_array, cursor_pos);
    Token *token = token_it_read(&it);
    if (token && token->kind == TokenBaseKind_Identifier)
    {
      F4_RenderRangeHighlight(app, view_id, text_layout_id,
                              Ii64(token->pos, token->pos + token->size),
                              F4_RangeHighlightKind_Underline,
                              fcolor_resolve(fcolor_id(fleury_color_token_highlight)));
    }
  }
  
  // NOTE(rjf): Flashes
  {
    F4_RenderFlashes(app, view_id, text_layout_id);
  }
  
  // NOTE(Allen): Color parens
  if (def_get_config_b32(vars_save_string_lit("use_paren_helper")))
  {
    Color_Array colors = finalize_color_array(defcolor_text_cycle);
    draw_paren_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
  }
  
  // NOTE(Nghia Lam): Cursor
  Face_Metrics metrics = get_face_metrics(app, face_id);
  u64 cursor_roundness_100 = def_get_config_u64(app, vars_save_string_lit("cursor_roundness"));
  f32 cursor_roundness = metrics.normal_advance * cursor_roundness_100 * 0.01f;
  f32 mark_thickness = (f32)def_get_config_u64(app, vars_save_string_lit("mark_thickness"));
  
  switch (fcoder_mode)
  {
    case FCoderMode_Original:
    {
      NL_DrawCursorMark(app,
                        view_id,
                        is_active_view,
                        buffer,
                        text_layout_id,
                        cursor_roundness,
                        mark_thickness,
                        frame_info);
    }
    break;
    case FCoderMode_NotepadLike:
    {
      draw_notepad_style_cursor_highlight(app, view_id, buffer, text_layout_id, cursor_roundness);
    }
    break;
  }
  
  // NOTE(Nghia Lam): Divider Comments
  NL_DrawDividerComment(app, buffer, view_id, text_layout_id);
  
  // NOTE(rjf): Brace annotations
  F4_Brace_RenderCloseBraceAnnotation(app, buffer, text_layout_id, cursor_pos);
  
  // NOTE(rjf): Brace lines
  F4_Brace_RenderLines(app, buffer, view_id, text_layout_id, cursor_pos);
  
  // NOTE(Allen): Fade ranges
  paint_fade_ranges(app, text_layout_id, buffer);
  
  // NOTE(Allen): put the actual text on the actual screen
  draw_text_layout_default(app, text_layout_id);
  
  // NOTE(rjf): Interpret buffer as calc code, if it's the calc buffer.
  Buffer_ID calc_buffer_id = get_buffer_by_name(app, string_u8_litexpr("*calc*"), AccessFlag_Read);
  if (calc_buffer_id == buffer)
  {
    F4_CLC_RenderBuffer(app, buffer, view_id, text_layout_id, frame_info);
  }
  
  // NOTE(rjf): Draw calc comments.
  F4_CLC_RenderComments(app, buffer, view_id, text_layout_id, frame_info);
  
  draw_set_clip(app, prev_clip);
  
  // NOTE(rjf): Draw tooltips and stuff.
  if (active_view == view_id)
  {
    // NOTE(rjf): Position context helper
    {
      F4_PosContext_Render(app, view_id, buffer, text_layout_id, cursor_pos);
    }
    
    // NOTE(rjf): Draw tooltip list.
    {
      Mouse_State mouse = get_mouse_state(app);
      
      Rect_f32 view_rect = view_get_screen_rect(app, view_id);
      
      Face_ID tooltip_face_id = global_small_code_face;
      Face_Metrics tooltip_face_metrics = get_face_metrics(app, tooltip_face_id);
      
      Rect_f32 tooltip_rect =
      {
        (f32)mouse.x + 16,
        (f32)mouse.y + 16,
        (f32)mouse.x + 16,
        (f32)mouse.y + 16 + tooltip_face_metrics.line_height + 8,
      };
      
      for (int i = 0; i < global_tooltip_count; ++i)
      {
        String_Const_u8 string = global_tooltips[i].string;
        tooltip_rect.x1 = tooltip_rect.x0;
        tooltip_rect.x1 += get_string_advance(app, tooltip_face_id, string) + 4;
        
        if (tooltip_rect.x1 > view_rect.x1)
        {
          f32 difference = tooltip_rect.x1 - view_rect.x1;
          tooltip_rect.x1 = (float)(int)(tooltip_rect.x1 - difference);
          tooltip_rect.x0 = (float)(int)(tooltip_rect.x0 - difference);
        }
        
        F4_DrawTooltipRect(app, tooltip_rect);
        
        draw_string(app, tooltip_face_id, string,
                    V2f32(tooltip_rect.x0 + 4,
                          tooltip_rect.y0 + 4),
                    global_tooltips[i].color);
      }
    }
  }
  
  // NOTE(rjf): Draw inactive rectangle
  if (is_active_view == 0)
  {
    Rect_f32 view_rect = view_get_screen_rect(app, view_id);
    ARGB_Color color = fcolor_resolve(fcolor_id(fleury_color_inactive_pane_overlay));
    if (F4_ARGBIsValid(color))
    {
      draw_rectangle(app, view_rect, 0.f, color);
    }
  }
  
  // NOTE(rjf): Render code peek.
  {
    if (!view_get_is_passive(app, view_id) && !is_active_view)
    {
      F4_CodePeek_Render(app, view_id, face_id, buffer, frame_info);
    }
  }
  
  // NOTE(rjf): Draw power mode.
  {
    F4_PowerMode_RenderBuffer(app, view_id, face_id, frame_info);
  }
}

#endif // FCODER_NGHIALAM_HOOKS