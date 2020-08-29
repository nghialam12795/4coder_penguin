
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

// Credit to Ryan Fleury
// https://github.com/ryanfleury/4coder_fleury/blob/master/4coder_fleury_divider_comments.cpp

//~ NOTE(rjf): Divider Comments

static void penguin_render_divider_comment(Application_Links *app, Buffer_ID buffer, View_ID view, Text_Layout_ID text_layout_id)
{
  ProfileScope(app, "[Fleury] Divider Comments");
  
  String_Const_u8 divider_comment_signifier = string_u8_litexpr("//~");
  
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
          text_layout_character_on_screen(app, text_layout_id, token->pos);
        
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
          // NOTE(rjf): Render divider line.
          Rect_f32 rect =
          {
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

CUSTOM_UI_COMMAND_SIG(projects_lister)
CUSTOM_DOC("List of all your projects")
{
	Scratch_Block scratch(app);
    char *query = "Load your project:";

    Lister_Block lister(app, scratch);
    lister_set_query(lister, query);
    Lister_Handlers handlers = lister_get_default_handlers();
    lister_set_handlers(lister, &handlers);

	// Here are the hardcoded paths that you should change depending on your projects
	String_Const_u8 project1 = string_u8_litexpr("/Users/nghialam/Projects/Ethan/");
	String_Const_u8 project2 = string_u8_litexpr("/Applications/4coder.app/Contents/MacOS/");

	code_index_lock();

	// Here are the names you want to give to each of your projects (optional)

	lister_add_item(lister, project1, string_u8_litexpr("Ethan"), &project1, 0);
	lister_add_item(lister, project2, string_u8_litexpr("4Coder"), &project2, 0);
	code_index_unlock();

	Lister_Result l_result = run_lister(app, lister);
	String_Const_u8 *result = 0;
    if (!l_result.canceled && l_result.user_data != 0){
		result = (String_Const_u8 *)l_result.user_data;
		if (result->str != 0){
			set_hot_directory(app, *result);
			close_all_files(app);
			load_project(app);
		}
	}
}
