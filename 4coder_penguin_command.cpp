#include "4coder_penguin_language.h"

function Rect_f32_Pair
layout_colby_line_number_margin(Application_Links * app,
                                Buffer_ID buffer,
                                Rect_f32 rect,
                                f32 digit_advance)
{
 i64 line_count = buffer_get_line_count(app, buffer);
 i64 line_count_digit_count = digit_count_from_integer(line_count, 10) + 2;
 return(layout_line_number_margin(rect, digit_advance, line_count_digit_count));
}

function void
draw_colby_line_number_margin(Application_Links * app,
                              View_ID view_id, Buffer_ID buffer,
                              Face_ID face_id, Text_Layout_ID text_layout_id,
                              Rect_f32 margin)
{
 Rect_f32 prev_clip = draw_set_clip(app, margin);
 draw_rectangle_fcolor(app, margin, 0.f, fcolor_id(defcolor_line_numbers_back));
 
 Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
 
 FColor line_color = fcolor_id(defcolor_line_numbers_text);
 FColor cursor_line_color =
   fcolor_argb(finalize_color(defcolor_line_numbers_text, 1));
 
 i64 line_count = buffer_get_line_count(app, buffer);
 i64 line_count_digit_count = digit_count_from_integer(line_count, 10);
 
 Scratch_Block scratch(app);
 
 Face_Metrics face_metrics = get_face_metrics(app, face_id);
 f32 digit_advance = face_metrics.decimal_digit_advance;
 
 i64 cursor_position = view_get_cursor_pos(app, view_id);
 Buffer_Cursor cursor =
   view_compute_cursor(app, view_id, seek_pos(cursor_position));
 
 Buffer_Cursor visible_first =
   view_compute_cursor(app, view_id, seek_pos(visible_range.first));
 i64 line_number = visible_first.line;
 for (;visible_first.pos <= visible_range.one_past_last;)
 {
  if (line_number > line_count)
    break;
  
  b32 is_on_cursor_line = (cursor.line == line_number);
  
  FColor draw_color =
    (is_on_cursor_line && global_config.highlight_line_at_cursor) ?
    cursor_line_color : line_color;
  
  Temp_Memory_Block temp(scratch);
  Fancy_String *string = push_fancy_stringf(scratch, 0, draw_color,
                                            "%*lld",
                                            line_count_digit_count,
                                            line_number);
  
  Range_f32 line_y =
    text_layout_line_on_screen(app, text_layout_id, line_number);
  Vec2_f32 p = V2f32(margin.x0 + digit_advance, line_y.min);
  // Comment this conditional if you don't want line_number pushed to the side.
  if (is_on_cursor_line && global_config.highlight_line_at_cursor)
    p.x -= digit_advance;
  
  draw_fancy_string(app, face_id, fcolor_zero(), string, p);
  line_number += 1;
 }
 
 draw_set_clip(app, prev_clip);
}


Parsed_Jump try_language_jump(String_Const_u8 line_str)
{
 Parsed_Jump jump = {};
 for (Language *lang = languages.first;
      lang != 0;
      lang = lang->next)
 {
  jump = lang->parse_jump_location(line_str);
  if (jump.success)
    return jump;
 }
 return jump;
}

internal Sticky_Jump_Array tc_parse_buffer_to_jump_array(Application_Links *app, Arena *arena, Buffer_ID buffer){
 Sticky_Jump_Node *jump_first = 0;;
 Sticky_Jump_Node *jump_last = 0;
 i32 jump_count = 0;
 
 Managed_Scope scope = buffer_get_managed_scope(app, buffer);
 Base_Allocator *managed_allocator = managed_scope_allocator(app, scope);
 Arena managed_arena = make_arena(managed_allocator);
 List_String_Const_u8 *msg_list = scope_attachment(app, scope, buffer_errors, List_String_Const_u8);
 
 for (i32 line = 1;; line += 1){
  b32 output_jump = false;
  i32 colon_index = 0;
  b32 is_sub_error = false;
  Buffer_ID out_buffer_id = 0;
  i64 out_pos = 0;
  {
   Temp_Memory_Block line_auto_closer(arena);
   if (is_valid_line(app, buffer, line)){
    String_Const_u8 line_str = push_buffer_line(app, arena, buffer, line);
    Parsed_Jump parsed_jump = try_language_jump(line_str);
    if (parsed_jump.success){
     Buffer_ID jump_buffer = {};
     if (open_file(app, &jump_buffer, parsed_jump.location.file, false, true)){
      if (buffer_exists(app, jump_buffer)){
       Buffer_Cursor cursor = buffer_compute_cursor(app, jump_buffer, seek_jump(parsed_jump));
       if (cursor.line > 0){
        out_buffer_id = jump_buffer;
        out_pos = cursor.pos;
        output_jump = true;
        // if (parsed_jump.msg.size > 0)
        // {
        //  string_list_push(&managed_arena, msg_list, push_string_copy(&managed_arena, parsed_jump.msg));
        // }
       }
      }
     }
    }
   }
   else{
    break;
   }
  }
  
  if (output_jump){
   Sticky_Jump_Node *jump = push_array(arena, Sticky_Jump_Node, 1);
   sll_queue_push(jump_first, jump_last, jump);
   jump_count += 1;
   jump->jump.list_line = line;
   jump->jump.list_colon_index = colon_index;
   jump->jump.is_sub_error =  is_sub_error;
   jump->jump.jump_buffer_id = out_buffer_id;
   jump->jump.jump_pos = out_pos;
  }
 }
 
 Sticky_Jump_Array result = {};
 result.count = jump_count;
 result.jumps = push_array(arena, Sticky_Jump, result.count);
 i32 index = 0;
 for (Sticky_Jump_Node *node = jump_first;
      node != 0;
      node = node->next){
  result.jumps[index] = node->jump;
  index += 1;
 }
 
 return(result);
}

internal void tc_init_marker_list(Application_Links *app, Heap *heap, Buffer_ID buffer, Marker_List *list){
 Scratch_Block scratch(app);
 
 Sticky_Jump_Array jumps = tc_parse_buffer_to_jump_array(app, scratch, buffer);
 Range_i32_Array buffer_ranges = get_ranges_of_duplicate_keys(scratch, &jumps.jumps->jump_buffer_id, sizeof(*jumps.jumps), jumps.count);
 Sort_Pair_i32 *range_index_buffer_id_pairs = push_array(scratch, Sort_Pair_i32, buffer_ranges.count);
 for (i32 i = 0; i < buffer_ranges.count; i += 1){
  range_index_buffer_id_pairs[i].index = i;
  range_index_buffer_id_pairs[i].key = jumps.jumps[buffer_ranges.ranges[i].first].jump_buffer_id;
 }
 sort_pairs_by_key(range_index_buffer_id_pairs, buffer_ranges.count);
 Range_i32_Array scoped_buffer_ranges = get_ranges_of_duplicate_keys(scratch,
                                                                     &range_index_buffer_id_pairs->key,
                                                                     sizeof(*range_index_buffer_id_pairs),
                                                                     buffer_ranges.count);
 
 Sticky_Jump_Stored *stored = push_array(scratch, Sticky_Jump_Stored, jumps.count);
 
 Managed_Scope scope_array[2] = {};
 scope_array[0] = buffer_get_managed_scope(app, buffer);
 
 for (i32 i = 0; i < scoped_buffer_ranges.count; i += 1){
  Range_i32 buffer_range_indices = scoped_buffer_ranges.ranges[i];
  
  u32 total_jump_count = 0;
  for (i32 j = buffer_range_indices.first;
       j < buffer_range_indices.one_past_last;
       j += 1){
   i32 range_index = range_index_buffer_id_pairs[j].index;
   Range_i32 range = buffer_ranges.ranges[range_index];
   total_jump_count += range_size(range);
  }
  
  Temp_Memory marker_temp = begin_temp(scratch);
  Marker *markers = push_array(scratch, Marker, total_jump_count);
  Buffer_ID target_buffer_id = 0;
  u32 marker_index = 0;
  for (i32 j = buffer_range_indices.first;
       j < buffer_range_indices.one_past_last;
       j += 1){
   i32 range_index = range_index_buffer_id_pairs[j].index;
   Range_i32 range = buffer_ranges.ranges[range_index];
   if (target_buffer_id == 0){
    target_buffer_id = jumps.jumps[range.first].jump_buffer_id;
   }
   for (i32 k = range.first; k < range.one_past_last; k += 1){
    markers[marker_index].pos = jumps.jumps[k].jump_pos;
    markers[marker_index].lean_right = false;
    stored[k].list_line        = jumps.jumps[k].list_line;
    stored[k].list_colon_index = jumps.jumps[k].list_colon_index;
    stored[k].is_sub_error     = jumps.jumps[k].is_sub_error;
    stored[k].jump_buffer_id   = jumps.jumps[k].jump_buffer_id;
    stored[k].index_into_marker_array = marker_index;
    marker_index += 1;
   }
  }
  
  scope_array[1] = buffer_get_managed_scope(app, target_buffer_id);
  Managed_Scope scope = get_managed_scope_with_multiple_dependencies(app, scope_array, ArrayCount(scope_array));
  Managed_Object marker_handle = alloc_buffer_markers_on_buffer(app, target_buffer_id, total_jump_count, &scope);
  managed_object_store_data(app, marker_handle, 0, total_jump_count, markers);
  
  end_temp(marker_temp);
  
  Assert(managed_object_get_item_size(app, marker_handle) == sizeof(Marker));
  Assert(managed_object_get_item_count(app, marker_handle) == total_jump_count);
  Assert(managed_object_get_type(app, marker_handle) == ManagedObjectType_Markers);
  
  Managed_Object *marker_handle_ptr = scope_attachment(app, scope, sticky_jump_marker_handle, Managed_Object);
  if (marker_handle_ptr != 0){
   *marker_handle_ptr = marker_handle;
  }
 }
 
 Managed_Object stored_jump_array = alloc_managed_memory_in_scope(app, scope_array[0], sizeof(Sticky_Jump_Stored), jumps.count);
 managed_object_store_data(app, stored_jump_array, 0, jumps.count, stored);
 
 list->jump_array = stored_jump_array;
 list->jump_count = jumps.count;
 list->previous_size = (i32)buffer_get_size(app, buffer);
 list->buffer_id = buffer;
}

internal Marker_List* tc_get_or_make_list_for_buffer(Application_Links *app, Heap *heap, Buffer_ID buffer_id){
 Marker_List *result = get_marker_list_for_buffer(buffer_id);
 if (result != 0){
  i32 buffer_size = (i32)buffer_get_size(app, buffer_id);
  // TODO(allen):  // TODO(allen): // TODO(allen): // TODO(allen): // TODO(allen): When buffers get an "edit sequence number" use that instead.
  if (result->previous_size != buffer_size){
   delete_marker_list(result);
   result = 0;
  }
 }
 if (result == 0){
  result = make_new_marker_list_for_buffer(heap, buffer_id);
  tc_init_marker_list(app, heap, buffer_id, result);
  if (result->jump_count == 0){
   delete_marker_list(result);
   result = 0;
  }
 }
 return(result);
}


CUSTOM_COMMAND_SIG(tc_if_read_only_goto_position)
CUSTOM_DOC("If the buffer in the active view is writable, inserts a character, otherwise performs goto_jump_at_cursor.")
{
 View_ID view = get_active_view(app, Access_ReadVisible);
 Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
 if (buffer == 0){
  buffer = view_get_buffer(app, view, Access_ReadVisible);
  if (buffer != 0){
   tc_goto_jump_at_cursor(app);
   lock_jump_buffer(app, buffer);
  }
 }
 else{
  leave_current_input_unhandled(app);
 }
}

CUSTOM_COMMAND_SIG(tc_if_read_only_goto_position_same_panel)
CUSTOM_DOC("If the buffer in the active view is writable, inserts a character, otherwise performs goto_jump_at_cursor_same_panel.")
{
 View_ID view = get_active_view(app, Access_ReadVisible);
 Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
 if (buffer == 0){
  buffer = view_get_buffer(app, view, Access_ReadVisible);
  if (buffer != 0){
   tc_goto_jump_at_cursor_same_panel(app);
   lock_jump_buffer(app, buffer);
  }
 }
 else{
  leave_current_input_unhandled(app);
 }
}

static void penguin_draw_tooltip_rect(Application_Links *app, Rect_f32 rect) {
 ARGB_Color background_color = fcolor_resolve(fcolor_id(defcolor_back));
 ARGB_Color border_color = fcolor_resolve(fcolor_id(defcolor_margin_active));
 
 background_color &= 0x00ffffff;
 background_color |= 0xd0000000;
 
 border_color &= 0x00ffffff;
 border_color |= 0xd0000000;
 
 draw_rectangle(app, rect, 4.f, background_color);
 draw_rectangle_outline(app, rect, 4.f, 3.f, border_color);
}

static void penguin_pushtooltip(String_Const_u8 string, ARGB_Color color) {
 if(global_tooltip_count < ArrayCount(global_tooltips)) {
  String_Const_u8 string_copy = push_string_copy(&global_frame_arena, string);
  global_tooltips[global_tooltip_count].color = color;
  global_tooltips[global_tooltip_count].string = string_copy;
  global_tooltip_count += 1;
 }
}

function void tc_do_full_lex_async__inner(Async_Context *actx, Buffer_ID buffer_id){
 Application_Links *app = actx->app;
 ProfileScope(app, "async lex");
 Scratch_Block scratch(app);
 
 String_Const_u8 contents = {};
 {
  ProfileBlock(app, "async lex contents (before mutex)");
  acquire_global_frame_mutex(app);
  ProfileBlock(app, "async lex contents (after mutex)");
  contents = push_whole_buffer(app, scratch, buffer_id);
  release_global_frame_mutex(app);
 }
 
 i32 limit_factor = 10000;
 
 Token_List list = {};
 b32 canceled = false;
 Managed_Scope scope = buffer_get_managed_scope(app, buffer_id);
 Language **language = scope_attachment(app, scope, buffer_language, Language*);
 
 // @todo(tyler): Make cancellable
 // list = (*language)->lex_full_input(scratch, contents);
 Generic_Lex_State state = {};
 (*language)->lex_init(scratch, &state, contents);
 for (;;){
  ProfileBlock(app, "async lex block");
  if ((*language)->lex_breaks(scratch, &list, &state, limit_factor)){
   break;
  }
  if (async_check_canceled(actx)){
   canceled = true;
   break;
  }
 }
 if (!canceled){
  ProfileBlock(app, "async lex save results (before mutex)");
  acquire_global_frame_mutex(app);
  ProfileBlock(app, "async lex save results (after mutex)");
  
  if (scope != 0){
   Base_Allocator *allocator = managed_scope_allocator(app, scope);
   Token_Array *tokens_ptr = scope_attachment(app, scope, attachment_tokens, Token_Array);
   base_free(allocator, tokens_ptr->tokens);
   Token_Array tokens = {};
   tokens.tokens = base_array(allocator, Token, list.total_count);
   tokens.count = list.total_count;
   tokens.max = list.total_count;
   token_fill_memory_from_list(tokens.tokens, &list);
   block_copy_struct(tokens_ptr, &tokens);
  }
  buffer_mark_as_modified(buffer_id);
  release_global_frame_mutex(app);
 }
}

function void tc_do_full_lex_async(Async_Context *actx, Data data){
 if (data.size == sizeof(Buffer_ID)){
  Buffer_ID *buffer = (Buffer_ID*)data.data;
  tc_do_full_lex_async__inner(actx, *buffer);
 }
}

function void init_buffer(Application_Links *app, Buffer_ID buffer_id)
{
 Scratch_Block scratch(app);
 
 Managed_Scope scope = buffer_get_managed_scope(app, buffer_id);
 
 b32 treat_as_code = false;
 String_Const_u8 file_name = push_buffer_file_name(app, scratch, buffer_id);
 if (global_config.automatically_load_project && !current_project.loaded){
  load_project(app);
 }
 Language **language = scope_attachment(app, scope, buffer_language, Language*);
 
 if (!*language && file_name.size > 0){
  String_Const_u8_Array extensions = global_config.code_exts;
  String_Const_u8 ext = string_file_extension(file_name);
  *language = language_from_extension(ext);
  
  for (i32 i = 0; !*language && i < extensions.count; ++i) {
   if (string_match(ext, extensions.strings[i])) {
    treat_as_code = true;
   }
   break;
  }
 }
 if (*language) treat_as_code = true;
 
 Command_Map_ID map_id = (treat_as_code)?(mapid_code):(mapid_file);
 Command_Map_ID *map_id_ptr = scope_attachment(app, scope, buffer_map_id, Command_Map_ID);
 *map_id_ptr = map_id;
 
 Line_Ending_Kind setting = guess_line_ending_kind_from_buffer(app, buffer_id);
 Line_Ending_Kind *eol_setting = scope_attachment(app, scope, buffer_eol_setting, Line_Ending_Kind);
 *eol_setting = setting;
 
 // NOTE(allen): Decide buffer settings
 b32 wrap_lines = true;
 b32 use_lexer = false;
 if (treat_as_code){
  wrap_lines = global_config.enable_code_wrapping;
  use_lexer = true;
 }
 
 String_Const_u8 buffer_name = push_buffer_base_name(app, scratch, buffer_id);
 if (string_match(buffer_name, string_u8_litexpr("*compilation*"))){
  wrap_lines = false;
 }
 
 if (use_lexer){
  ProfileBlock(app, "begin buffer kick off lexer");
  Async_Task *lex_task_ptr = scope_attachment(app, scope, buffer_lex_task, Async_Task);
  *lex_task_ptr = async_task_no_dep(&global_async_system, tc_do_full_lex_async, make_data_struct(&buffer_id));
 }
 
 {
  b32 *wrap_lines_ptr = scope_attachment(app, scope, buffer_wrap_lines, b32);
  *wrap_lines_ptr = wrap_lines;
 }
 
 if (use_lexer){
  buffer_set_layout(app, buffer_id, layout_virt_indent_index_generic);
 }
 else{
  if (treat_as_code){
   buffer_set_layout(app, buffer_id, layout_virt_indent_literal_generic);
  }
  else{
   buffer_set_layout(app, buffer_id, layout_generic);
  }
 }
}

//|
//~ NOTE(rjf): Error annotations

static void penguin_render_error_annotation(Application_Links *app, Buffer_ID buffer,
                                            Text_Layout_ID text_layout_id,
                                            Buffer_ID jump_buffer) {
 ProfileScope(app, "[Fleury] Error Annotations");
 
 Heap *heap = &global_heap;
 Scratch_Block scratch(app);
 
 Locked_Jump_State jump_state = {};
 {
  ProfileScope(app, "[Fleury] Error Annotations (Get Locked Jump State)");
  jump_state = get_locked_jump_state(app, heap);
 }
 
 Face_ID face = global_small_code_face;
 Face_Metrics metrics = get_face_metrics(app, face);
 
 if(jump_buffer != 0 && jump_state.view != 0)
 {
  Managed_Scope buffer_scopes[2];
  {
   ProfileScope(app, "[Fleury] Error Annotations (Buffer Get Managed Scope)");
   buffer_scopes[0] = buffer_get_managed_scope(app, jump_buffer);
   buffer_scopes[1] = buffer_get_managed_scope(app, buffer);
  }
  
  Managed_Scope comp_scope = 0;
  {
   ProfileScope(app, "[Fleury] Error Annotations (Get Managed Scope)");
   comp_scope = get_managed_scope_with_multiple_dependencies(app, buffer_scopes, ArrayCount(buffer_scopes));
  }
  
  Managed_Object *buffer_markers_object = 0;
  {
   ProfileScope(app, "[Fleury] Error Annotations (Scope Attachment)");
   buffer_markers_object = scope_attachment(app, comp_scope, sticky_jump_marker_handle, Managed_Object);
  }
  
  // NOTE(rjf): Get buffer markers (locations where jumps point at).
  i32 buffer_marker_count = 0;
  Marker *buffer_markers = 0;
  {
   ProfileScope(app, "[Fleury] Error Annotations (Load Managed Object Data)");
   buffer_marker_count = managed_object_get_item_count(app, *buffer_markers_object);
   buffer_markers = push_array(scratch, Marker, buffer_marker_count);
   managed_object_load_data(app, *buffer_markers_object, 0, buffer_marker_count, buffer_markers);
  }
  
  i64 last_line = -1;
  
  for(i32 i = 0; i < buffer_marker_count; i += 1)
  {
   ProfileScope(app, "[Fleury] Error Annotations (Buffer Loop)");
   
   i64 jump_line_number = get_line_from_list(app, jump_state.list, i);
   i64 code_line_number = get_line_number_from_pos(app, buffer, buffer_markers[i].pos);
   
   if(code_line_number != last_line)
   {
    ProfileScope(app, "[Fleury] Error Annotations (Jump Line)");
    
    String_Const_u8 jump_line = push_buffer_line(app, scratch, jump_buffer, jump_line_number);
    
    // NOTE(rjf): Remove file part of jump line.
    {
     u64 index = string_find_first(jump_line, string_u8_litexpr("error"), StringMatch_CaseInsensitive);
     if(index == jump_line.size)
     {
      index = string_find_first(jump_line, string_u8_litexpr("warning"), StringMatch_CaseInsensitive);
      if(index == jump_line.size)
      {
       index = 0;
      }
     }
     jump_line.str += index;
     jump_line.size -= index;
    }
    
    // NOTE(rjf): Render annotation.
    {
     Range_i64 line_range = Ii64(code_line_number);
     Range_f32 y1 = text_layout_line_on_screen(app, text_layout_id, line_range.min);
     Range_f32 y2 = text_layout_line_on_screen(app, text_layout_id, line_range.max);
     Range_f32 y = range_union(y1, y2);
     Rect_f32 last_character_on_line_rect =
       text_layout_character_on_screen(app, text_layout_id, get_line_end_pos(app, buffer, code_line_number)-1);
     
     if(range_size(y) > 0.f)
     {
      Rect_f32 region = text_layout_region(app, text_layout_id);
      Vec2_f32 draw_position =
      {
       region.x1 - metrics.max_advance*jump_line.size -
         (y.max-y.min)/2 - metrics.line_height/2,
       y.min + (y.max-y.min)/2 - metrics.line_height/2,
      };
      
      if(draw_position.x < last_character_on_line_rect.x1 + 30)
      {
       draw_position.x = last_character_on_line_rect.x1 + 30;
      }
      
      draw_string(app, face, jump_line, draw_position, 0xffff0000);
      
      Mouse_State mouse_state = get_mouse_state(app);
      if(mouse_state.x >= region.x0 && mouse_state.x <= region.x1 &&
         mouse_state.y >= y.min && mouse_state.y <= y.max)
      {
       penguin_pushtooltip(jump_line, 0xffff0000);
      }
     }
    }
   }
   
   last_line = code_line_number;
  }
 }
}

static void penguin_function_helper(Application_Links* app, View_ID view, Buffer_ID buffer, Text_Layout_ID text_layout_id, i64 pos) {
 ProfileScope(app, "[Penguin] function helper");
 
 Token_Array token_array = get_token_array_from_buffer(app, buffer);
 Token_Iterator_Array it;
 Token* token = 0;
 
 Rect_f32 view_rect = view_get_screen_rect(app, view);
 
 Face_ID face = get_face_id(app, buffer);
 Face_Metrics metrics = get_face_metrics(app, face);
 
 if(token_array.tokens != 0) {
  it = token_iterator_pos(0, &token_array, pos);
  token = token_it_read(&it);
  
  if(token != 0 && token->kind == TokenBaseKind_ParentheticalOpen) {
   pos = token->pos + token->size;
  }
  else {
   if (token_it_dec_all(&it)) {
    token = token_it_read(&it);
    if (token->kind == TokenBaseKind_ParentheticalClose && pos == token->pos + token->size) {
     pos = token->pos;
    }
   }
  }
 }
 
 Scratch_Block scratch(app);
 Range_i64_Array ranges = { };
 {
  i64 temp_pos = pos;
  i32 max = 100;
  ranges.ranges = push_array(scratch, Range_i64, max);
  
  for (;;) {
   Range_i64 range = { };
   range.max = temp_pos;
   if (find_nest_side(app, buffer, temp_pos - 1, FindNest_Paren | FindNest_Balanced, Scan_Backward, NestDelim_Open, &range.start)) {
    ranges.ranges[ranges.count] = range;
    ranges.count += 1;
    temp_pos = range.first;
    if (ranges.count >= max) {
     break;
    }
   }
   else {
    break;
   }
  }
 }
 
 for (int range_index = 0; range_index < ranges.count; ++range_index) {
  it = token_iterator_pos(0, &token_array, ranges.ranges[range_index].min);
  token_it_dec_non_whitespace(&it);
  token = token_it_read(&it);
  if(token->kind == TokenBaseKind_Identifier) {
   // highlight function name
   Range_i64 function_name_range = Ii64(token->pos, token->pos+token->size);
   String_Const_u8 function_name = push_buffer_range(app, scratch, buffer, function_name_range);
   ARGB_Color background_color = fcolor_resolve(fcolor_id(defcolor_highlight));
   background_color &= 0x00FFFFFF;
   background_color |= 0x60000000;
   draw_character_block(app, text_layout_id, function_name_range, 4.0f, background_color);
   
   // find active parameter
   int active_parameter_index = 0;
   static int last_active_parameter = -1;
   {
    it = token_iterator_pos(0, &token_array, function_name_range.min);
    int paren_nest = 0;
    for(;token_it_inc_non_whitespace(&it);)
    {
     token = token_it_read(&it);
     if(token->pos + token->size > pos) {
      break;
     }
     
     if(token->kind == TokenBaseKind_ParentheticalOpen) {
      ++paren_nest;
     }
     else if(token->kind == TokenBaseKind_StatementClose) {
      if(paren_nest == 1) {
       ++active_parameter_index;
      }
     }
     else if(token->kind == TokenBaseKind_ParentheticalClose) {
      if(!--paren_nest) {
       break;
      }
     }
    }
   }
   last_active_parameter = active_parameter_index;
   
   for(Buffer_ID buffer_it = get_buffer_next(app, 0, Access_Always); buffer_it != 0; buffer_it = get_buffer_next(app, buffer_it, Access_Always)) {
    Code_Index_File* file = code_index_get_file(buffer_it);
    if(file == 0) continue;
    for(i32 i = 0; i < file->note_array.count; i += 1) {
     Code_Index_Note *note = file->note_array.ptrs[i];
     
     if((note->note_kind == CodeIndexNote_Function || note->note_kind == CodeIndexNote_Macro) && string_match(note->text, function_name)) {
      Range_i64 function_def_range;
      function_def_range.min = note->pos.min;
      function_def_range.max = note->pos.max;
      
      Range_i64 highlight_parameter_range = {0};
      
      Token_Array find_token_array = get_token_array_from_buffer(app, buffer_it);
      it = token_iterator_pos(0, &find_token_array, function_def_range.min);
      
      int paren_nest = 0;
      int param_index = 0;
      for(;token_it_inc_non_whitespace(&it);) {
       token = token_it_read(&it);
       if(token->kind == TokenBaseKind_ParentheticalOpen) {
        if(++paren_nest == 1) {
         if(active_parameter_index == param_index) {
          highlight_parameter_range.min = token->pos+1;
         }
        }
       }
       else if(token->kind == TokenBaseKind_ParentheticalClose) {
        if(!--paren_nest) {
         function_def_range.max = token->pos + token->size;
         if(param_index == active_parameter_index) {
          highlight_parameter_range.max = token->pos;
         }
         break;
        }
       }
       else if(token->kind == TokenBaseKind_StatementClose) {
        if(param_index == active_parameter_index) {
         highlight_parameter_range.max = token->pos;
        }
        
        if(paren_nest == 1) {
         ++param_index;
        }
        
        if(param_index == active_parameter_index) {
         highlight_parameter_range.min = token->pos+1;
        }
       }
      }
      
      if(highlight_parameter_range.min > function_def_range.min && function_def_range.max > highlight_parameter_range.max) {
       String_Const_u8 function_def = push_buffer_range(app, scratch, buffer_it, function_def_range);
       String_Const_u8 highlight_param = push_buffer_range(app, scratch, buffer_it, highlight_parameter_range);
       
       String_Const_u8 pre_highlight_def =
       {
        function_def.str,
        (u64)(highlight_parameter_range.min - function_def_range.min),
       };
       
       String_Const_u8 post_highlight_def =
       {
        function_def.str + highlight_parameter_range.max - function_def_range.min,
        (u64)(function_def_range.max - highlight_parameter_range.max),
       };
       
       Rect_f32 token_rect = text_layout_character_on_screen(app, text_layout_id, function_name_range.min);
       i64 cursor_pos = view_get_cursor_pos(app, view);
       Rect_f32 cursor_rect = text_layout_character_on_screen(app, text_layout_id, cursor_pos);
       
       Rect_f32 tooltip_rect =
       {
        token_rect.x0,
        cursor_rect.y0 - (metrics.line_height * 1.5f),
        token_rect.x0,
        cursor_rect.y0 - (metrics.line_height * 0.5f) ,
       };
       
       tooltip_rect.x1 += get_string_advance(app, face, highlight_param);
       tooltip_rect.x1 += get_string_advance(app, face, pre_highlight_def);
       tooltip_rect.x1 += get_string_advance(app, face, post_highlight_def);
       
       penguin_draw_tooltip_rect(app, tooltip_rect);
       
       Vec2_f32 text_position = { tooltip_rect.x0, tooltip_rect.y0 };
       text_position = draw_string(app, face, pre_highlight_def, text_position, finalize_color(defcolor_comment, 0));
       text_position = draw_string(app, face, highlight_param, text_position, finalize_color(defcolor_comment_pop, 1));
       text_position = draw_string(app, face, post_highlight_def, text_position, finalize_color(defcolor_comment, 0));
       
       goto end_lookup;
      }
     }
    }
   }
   
   end_lookup:;
   break;
  }
 }
}

function u32 penguin_calculate_color_brightness(u32 color) {
 u32 r = ((color >> 16) & 0xFF);
 u32 g = ((color >> 8 ) & 0xFF);
 u32 b = ((color >> 0 ) & 0xFF);
 
 f32 brightness = sqrtf((r * r * 0.241f) + (g * g * 0.691f) + (b * b * 0.068f));
 
 return (u32)(brightness);
}

function FColor penguin_get_token_color_cpp(Token token) {
 FColor result = fcolor_id(defcolor_text_default);
 
 switch (token.kind) {
  case TokenBaseKind_Preprocessor: {
   result = fcolor_id(defcolor_preproc);
  }
  break;
  
  case TokenBaseKind_Keyword: {
   result = fcolor_id(defcolor_keyword);
  }
  break;
  
  case TokenBaseKind_Comment: {
   result = fcolor_id(defcolor_comment);
  }
  break;
  
  case TokenBaseKind_LiteralString: {
   result = fcolor_id(defcolor_str_constant);
  }
  break;
  
  case TokenBaseKind_LiteralInteger: {
   result = fcolor_id(defcolor_int_constant);
  }
  break;
  
  case TokenBaseKind_LiteralFloat: {
   result = fcolor_id(defcolor_float_constant);
  }
  break;
  
  default: {
   switch (token.sub_kind) {
    case TokenCppKind_BraceOp:
    case TokenCppKind_BraceCl:
    case TokenCppKind_ParenOp:
    case TokenCppKind_ParenCl:
    case TokenCppKind_BrackOp:
    case TokenCppKind_BrackCl:
    case TokenCppKind_And:
    case TokenCppKind_AndAnd:
    case TokenCppKind_Or:
    case TokenCppKind_OrOr:
    case TokenCppKind_Colon:
    case TokenCppKind_ColonColon:
    case TokenCppKind_Semicolon:
    case TokenCppKind_Comma:
    case TokenCppKind_Dot:
    case TokenCppKind_DotDotDot:
    case TokenCppKind_Arrow:
    case TokenCppKind_Plus:
    case TokenCppKind_PlusPlus:
    case TokenCppKind_Minus:
    case TokenCppKind_MinusMinus:
    case TokenCppKind_Star:
    case TokenCppKind_Div:
    case TokenCppKind_Mod:
    case TokenCppKind_Ternary:
    case TokenCppKind_Eq:
    case TokenCppKind_PlusEq:
    case TokenCppKind_MinusEq:
    case TokenCppKind_StarEq:
    case TokenCppKind_DivEq:
    case TokenCppKind_ModEq:
    case TokenCppKind_Less:
    case TokenCppKind_LessEq:
    case TokenCppKind_Grtr:
    case TokenCppKind_GrtrEq:
    case TokenCppKind_EqEq:
    case TokenCppKind_NotEq: {
     result = fcolor_id(defcolor_keyword, 2);
    }
    break;
    
    case TokenCppKind_LiteralTrue:
    case TokenCppKind_LiteralFalse: {
     result = fcolor_id(defcolor_bool_constant);
    }
    break;
    
    case TokenCppKind_LiteralCharacter:
    case TokenCppKind_LiteralCharacterWide:
    case TokenCppKind_LiteralCharacterUTF8:
    case TokenCppKind_LiteralCharacterUTF16:
    case TokenCppKind_LiteralCharacterUTF32: {
     result = fcolor_id(defcolor_char_constant);
    }
    break;
    
    case TokenCppKind_PPIncludeFile: {
     result = fcolor_id(defcolor_include);
    }
    break;
   }
  }
 }
 
 return (result);
}

//|
// Credit to Ryan Fleury
// https://github.com/ryanfleury/4coder_fleury/blob/master/4coder_fleury_divider_comments.cpp

// NOTE(rjf): Divider Comments

static void penguin_render_divider_comment(Application_Links *app, Buffer_ID buffer, View_ID view, Text_Layout_ID text_layout_id)
{
 ProfileScope(app, "[Fleury] Divider Comments");
 
 String_Const_u8 divider_comment_signifier = string_u8_litexpr("//|");
 
 Token_Array token_array = get_token_array_from_buffer(app, buffer);
 Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
 Scratch_Block scratch(app);
 
 if(token_array.tokens != 0)
 {
  i64 first_index = token_index_from_pos(&token_array, visible_range.first);
  Token_Iterator_Array it = token_iterator_index(0, &token_array, first_index);
  
  Token *token = 0;
  for(;;)
  {
   token = token_it_read(&it);
   
   if(token->pos >= visible_range.one_past_last || !token || !token_it_inc_non_whitespace(&it))
   {
    break;
   }
   
   if(token->kind == TokenBaseKind_Comment)
   {
    Rect_f32 comment_first_char_rect =
      text_layout_character_on_screen(app, text_layout_id, token->pos + 2);
    
    Range_i64 token_range =
    {
     token->pos,
     token->pos + (token->size > (i64)divider_comment_signifier.size
                   ? (i64)divider_comment_signifier.size
                   : token->size),
    };
    
    u8 token_buffer[256] = {0};
    buffer_read_range(app, buffer, token_range, token_buffer);
    String_Const_u8 token_string = { token_buffer, (u64)(token_range.end - token_range.start) };
    
    if(string_match(token_string, divider_comment_signifier))
    {
     // NOTE(sammynilla): We increment the vertical midpoint by 1 at the end because it feels like the the position is slightly off when interacting with the | character.
     f32 CharacterVerticalMidPoint = comment_first_char_rect.y0 + ((comment_first_char_rect.y1 - comment_first_char_rect.y0)) * 0.5f;
     ++CharacterVerticalMidPoint;
     // NOTE(sammynilla): We're using the horizontal mid-point because were building our line position based on where the | falls.
     f32 CharacterHorizontalMidPoint = comment_first_char_rect.x0 + ((comment_first_char_rect.x1 - comment_first_char_rect.x0)) * 0.5f;
     
     Face_ID FaceID = get_face_id(app, buffer);
     Face_Description FaceDescription = get_face_description(app, FaceID);
     u32 FontSize = FaceDescription.parameters.pt_size;
     
     u32 DividerHeight = round((FontSize*0.1f)-0.05f);
     u32 DividerOffset = (DividerHeight < 1) ? 0 : 1;
     
     Rect_f32 DividerRect = {};
     DividerRect.x0 = CharacterHorizontalMidPoint;
     DividerRect.x1 = 10000;
     DividerRect.y0 = CharacterVerticalMidPoint - DividerOffset;
     DividerRect.y1 = DividerRect.y0 + DividerHeight;
     
     draw_rectangle_fcolor(app, DividerRect, 0, fcolor_id(defcolor_comment));
    }
   }
  }
 }
}

// Credit to Skytrias
// https://github.com/Skytrias/4files/blob/e254e55d9c310eeaa7f77d6a377ba517d6c1f055/render_highlight.cpp#L462

function void penguin_paint_functions(Application_Links *app, Buffer_ID buffer, Text_Layout_ID text_layout_id) {
 i64 keyword_length = 0;
 i64 start_pos = 0;
 i64 end_pos = 0;
 
 Token_Array array = get_token_array_from_buffer(app, buffer);
 if (array.tokens != 0){
  Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
  i64 first_index = token_index_from_pos(&array, visible_range.first);
  Token_Iterator_Array it = token_iterator_index(0, &array, first_index);
  for (;;){
   Token *token = token_it_read(&it);
   if (token->pos >= visible_range.one_past_last){
    break;
   }
   
   // get pos at paren
   // NOTE(Skytrias): use token->sub_kind == TokenCppKind_ParenOp if only '(' should be used
   if (keyword_length != 0 && token->kind == TokenBaseKind_ParentheticalOpen) {
    end_pos = token->pos;
   }
   
   // search for default text, count up the size
   if (token->kind == TokenBaseKind_Identifier) {
    if (keyword_length == 0) {
     start_pos = token->pos;
    }
    
    keyword_length += 1;
   } else {
    keyword_length = 0;
   }
   
   // color text
   if (start_pos != 0 && end_pos != 0) {
    Range_i64 range = { 0 };
    range.start = start_pos;
    range.end = end_pos;
    
    // NOTE(Skytrias): use your own colorscheme her via fcolor_id(defcolor_*)
    // NOTE(Skytrias): or set the color you'd like to use globally like i do
    paint_text_color(app, text_layout_id, range, fcolor_resolve(fcolor_id(defcolor_special_character)));
    
    end_pos = 0;
    start_pos = 0;
   }
   
   if (!token_it_inc_all(&it)){
    break;
   }
  }
 }
}

global b32 use_relative_line_number_mode = false;

function void draw_relative_line_number_margin(Application_Links *app, View_ID view_id, Buffer_ID buffer, Face_ID face_id, Text_Layout_ID text_layout_id, Rect_f32 margin){
 Rect_f32 prev_clip = draw_set_clip(app, margin);
 draw_rectangle_fcolor(app, margin, 0.f, fcolor_id(defcolor_line_numbers_back));
 
 Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
 Buffer_Cursor cursor_top = view_compute_cursor(app, view_id, seek_pos(visible_range.min));
 i64 top_line = cursor_top.line;
 
 FColor line_color = fcolor_id(defcolor_line_numbers_text);
 FColor current_line_color = fcolor_id(defcolor_cursor);
 
 i64 line_count = buffer_get_line_count(app, buffer);
 i64 line_count_digit_count = digit_count_from_integer(line_count, 10);
 
 Scratch_Block scratch(app);
 
 i64 cursor_line = view_get_cursor_pos(app, view_id);
 Buffer_Cursor cursor_current_line = view_compute_cursor(app, view_id, seek_pos(cursor_line));
 cursor_line = cursor_current_line.line;
 
 Buffer_Cursor cursor = view_compute_cursor(app, view_id, seek_pos(visible_range.first));
 i64 line_number = cursor.line;
 
 i64 below_count = cursor_line - top_line;
 i64 above_count = 1;
 
 for (;cursor.pos <= visible_range.one_past_last;){
  if (line_number > line_count){
   break;
  }
  Range_f32 line_y = text_layout_line_on_screen(app, text_layout_id, line_number);
  Vec2_f32 p = V2f32(margin.x0, line_y.min);
  Temp_Memory_Block temp(scratch);
  Fancy_String *string;
  if(line_number < cursor_line) {
   string = push_fancy_stringf(scratch, 0, line_color, "%*lld",
                               line_count_digit_count, below_count);
   --below_count;
  } else if(line_number > cursor_line) {
   string = push_fancy_stringf(scratch, 0, line_color, "%*lld",
                               line_count_digit_count, above_count);
   ++above_count;
  } else {
   string = push_fancy_stringf(scratch, 0, current_line_color, "%*lld",
                               line_count_digit_count, line_number);
  }
  
  draw_fancy_string(app, face_id, fcolor_zero(), string, p);
  line_number += 1;
 }
 
 draw_set_clip(app, prev_clip);
}

CUSTOM_UI_COMMAND_SIG(token_at_cursor)
CUSTOM_DOC("Shows info about the token under the cursor.")
{
 View_ID view = get_active_view(app, Access_Always);
 i64 cursor = view_get_cursor_pos(app, view);
 Buffer_ID buffer = view_get_buffer(app, view, Access_Always);
 
 Token *token = get_token_from_pos(app, buffer, cursor);
 
 Managed_Scope scope = buffer_get_managed_scope(app, buffer);
 Language **language = scope_attachment(app, scope, buffer_language, Language*);
 
 char *base_name = token_base_kind_names[token->kind];
 char *sub_name = (*language)->token_kind_names[token->sub_kind];
 
 char message[512];
 snprintf(message, 512, "Token:\n  Base: %s (%d)\n  Sub: %s (%d)\n", base_name, token->kind, sub_name, token->sub_kind);
 print_message(app, SCu8(message));
}

CUSTOM_UI_COMMAND_SIG(tc_goto_jump_at_cursor)
CUSTOM_DOC("Language specific goto_jump_at_cursor")
{
 Heap *heap = &global_heap;
 
 View_ID view = get_active_view(app, Access_ReadVisible);
 Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
 Marker_List *list = tc_get_or_make_list_for_buffer(app, heap, buffer);
 
 i64 pos = view_get_cursor_pos(app, view);
 Buffer_Cursor cursor = buffer_compute_cursor(app, buffer, seek_pos(pos));
 
 i32 list_index = get_index_exact_from_list(app, list, cursor.line);
 
 if (list_index >= 0){
  ID_Pos_Jump_Location location = {};
  if (get_jump_from_list(app, list, list_index, &location)){
   if (get_jump_buffer(app, &buffer, &location)){
    change_active_panel(app);
    View_ID target_view = get_active_view(app, Access_Always);
    switch_to_existing_view(app, target_view, buffer);
    jump_to_location(app, target_view, buffer, location);
    
   }
  }
 }
}

CUSTOM_COMMAND_SIG(tc_goto_jump_at_cursor_same_panel)
CUSTOM_DOC("Language specific goto_jump_at_cursor_same_panel")
{
 Heap *heap = &global_heap;
 
 View_ID view = get_active_view(app, Access_ReadVisible);
 Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
 
 // @note(tyler): This is the only change
 Marker_List *list = tc_get_or_make_list_for_buffer(app, heap, buffer);
 
 i64 pos = view_get_cursor_pos(app, view);
 Buffer_Cursor cursor = buffer_compute_cursor(app, buffer, seek_pos(pos));
 
 i32 list_index = get_index_exact_from_list(app, list, cursor.line);
 
 if (list_index >= 0){
  ID_Pos_Jump_Location location = {};
  if (get_jump_from_list(app, list, list_index, &location)){
   if (get_jump_buffer(app, &buffer, &location)){
    jump_to_location(app, view, buffer, location);
   }
  }
 }
}

CUSTOM_COMMAND_SIG(toggle_relative_line_number_mode)
CUSTOM_DOC("Toggle relative line numbers.")
{
 use_relative_line_number_mode = !use_relative_line_number_mode;
 if(use_relative_line_number_mode) {
  log_string(app, string_u8_litexpr("Toggle: relative_line_number_mode - on\n"));
 } else {
  log_string(app, string_u8_litexpr("Toggle: relative_line_number_mode - off\n"));
 }
}

CUSTOM_COMMAND_SIG(penguin_toggle_function_helper) {
 show_function_helper = !show_function_helper;
}

bool global_is_expand_panel = false;

CUSTOM_COMMAND_SIG(toggle_expand_panel) {
 View_ID view = get_active_view(app, Access_Always);
 
 if (!global_is_expand_panel) {
  view_set_split_proportion( app, view, 0.9f );
  global_is_expand_panel = true;
 } else {
  view_set_split_proportion( app, view, 0.5f );
  global_is_expand_panel = false;
 }
}

CUSTOM_UI_COMMAND_SIG(even_panel)
CUSTOM_DOC("Make All Windows Even")
{
 View_ID view = get_active_view(app, Access_Always);
 view_set_split_proportion( app, view, 0.5f );
 global_is_expand_panel = false;
}

// NOTE(Nghia Lam): Format:
// <Project_Name>
// <Project_Path>
// ...
// ---------------------- END <- Important
CUSTOM_UI_COMMAND_SIG(projects_lister)
CUSTOM_DOC("List current projects and jump and load to one chosen by the user.")
{
 Scratch_Block scratch(app);
 
 // NOTE(dgl): We currently support up to 20 projects;
 String_Const_u8 Projects[20];
 uint8_t ProjectCount = 0;
 b32 success = false;
 FILE *FileHandle = open_file_try_current_path_then_binary_path(app, "projects.4coder");
 if (FileHandle != 0)
 {
  Data FileContent = dump_file_handle(scratch, FileHandle);
  fclose(FileHandle);
  if (FileContent.data != 0)
  {
   String_Const_u8 *LatestProject = &Projects[ProjectCount++];
   u8 *Source = FileContent.data;
   LatestProject->str = FileContent.data;
   while(Source < (FileContent.data + FileContent.size))
   {
    if(*Source == '\n' && ProjectCount <= ArrayCount(Projects))
    {
     LatestProject->size = Source - LatestProject->str;
     // NOTE(dgl): Switch to new project string
     Source++;
     LatestProject = &Projects[ProjectCount++];
     LatestProject->str = Source;
    }
    else
    {
     Source++;
    }
   }
   success = true;
  }
 }
 
 // TODO(dgl): Remove empty strings
 ProjectCount--;
 
 if (success){
  char *query = "Project:";
  
  Lister_Block lister(app, scratch);
  lister_set_query(lister, query);
  lister_set_default_handlers(lister);
  
  code_index_lock();
  
  for(int Index = 0; Index < ProjectCount; Index+=2)
  {
   String_Const_u8 Name = Projects[Index];
   lister_add_item(lister, Projects[Index + 1], Name, &Projects[Index + 1], 0);
  }
  
  code_index_unlock();
  
  Lister_Result l_result = run_lister(app, lister);
  String_Const_u8 *result = 0;
  
  if(!l_result.canceled && l_result.user_data != 0)
  {
   result = (String_Const_u8 *)l_result.user_data;
   if (result->str != 0){
    set_hot_directory(app, *result);
    close_all_files(app);
    load_project(app);
   }
  }
 }
}