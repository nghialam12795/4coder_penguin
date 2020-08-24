
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


CUSTOM_COMMAND_SIG(penguin_toggle_function_helper) {
    show_function_helper = !show_function_helper;
}