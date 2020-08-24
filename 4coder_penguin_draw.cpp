function void penguin_draw_hex_color_preview(Application_Links* app, Buffer_ID buffer, Text_Layout_ID text_layout_id, i64 pos) {
    ProfileScope(app, "[Penguin] draw hex color preview");
    
    Scratch_Block scratch(app);
    
    Range_i64 range = enclose_pos_alpha_numeric(app, buffer, pos);
    String_Const_u8 token = push_buffer_range(app, scratch, buffer, range);
    if (token.size == 10) {
        if (token.str[0] == '0' && (token.str[1] == 'x' || token.str[1] == 'X')) {
            b32 is_hex = true;
            for (u32 i = 0; (i < 8) && is_hex; ++i) {
                char c = token.str[i + 2];
                is_hex = ((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') || (c >= '0' && c <= '9'));
            }
            
            if (is_hex) {
                String_Const_u8 hex = string_substring(token, Ii64_size(2, 8));
                
                ARGB_Color hex_color = (u32)string_to_integer(hex, 16);
                draw_character_block(app, text_layout_id, Ii64_size(range.min, 10), 2.0f, hex_color);
                
                ARGB_Color text_color = penguin_calculate_color_brightness(hex_color) < 128 ? 0xFFFFFFFF : 0xFF000000;
                paint_text_color(app, text_layout_id, range, text_color);
            }
        }
    }
}

function void penguin_draw_tokens(Application_Links* app, Buffer_ID buffer, Text_Layout_ID text_layout_id, Token_Array token_array) {
    ProfileScope(app, "[Penguin] paint tokens");
    
    Scratch_Block scratch(app);
    
    Range_i64 function_name_range = { };
    if (token_array.tokens != 0) {
        Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
        i64 first = token_index_from_pos(&token_array, visible_range.first);
        Token_Iterator_Array it = token_iterator_index(0, &token_array, first);
        for (;;) {
            Token* token = token_it_read(&it);
            
            if (token->pos >= visible_range.one_past_last) {
                break;
            }
            
            // paint keyword tokens
            FColor color = penguin_get_token_color_cpp(*token);
            ARGB_Color argb = fcolor_resolve(color);
            paint_text_color(app, text_layout_id, Ii64_size(token->pos, token->size), argb);
            
            // search function tokens
            switch (token->kind) {
                case TokenBaseKind_ParentheticalOpen: {
                    // end function token range
                    if (function_name_range.max == 0) {
                        function_name_range.max = token->pos;
                    }
                }
                break;
                
                case TokenBaseKind_Identifier: {
                    // start function token range
                    if (function_name_range.min == 0 && function_name_range.max == 0) {
                        function_name_range.min = token->pos;
                    }
                    
                    // detect and paint struct tokens
                    if (token->sub_kind == TokenCppKind_Identifier) {
                        String_Const_u8 tokenString = push_token_lexeme(app, scratch, buffer, token);
                        for (Buffer_ID nextBuffer = get_buffer_next(app, 0, Access_Always); nextBuffer != 0; nextBuffer = get_buffer_next(app, nextBuffer, Access_Always)) {
                            Code_Index_File* file = code_index_get_file(nextBuffer);
                            if (file == 0) continue;
                            
                            for (i32 i = 0; i < file->note_array.count; ++i) {
                                Code_Index_Note* note = file->note_array.ptrs[i];
                                switch (note->note_kind) {
                                    case CodeIndexNote_Type: {
                                        if (string_match(note->text, tokenString, StringMatch_Exact)) {
                                            Range_i64 tokenRange = Ii64_size(token->pos, token->size);
                                            paint_text_color(app, text_layout_id, tokenRange, finalize_color(defcolor_keyword, 1));
                                            break;
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
                break;
                
                default: {
                    function_name_range = { };
                }
            }
            
            // paint function tokens
            if (function_name_range.min != 0 && function_name_range.max != 0) {
                paint_text_color(app, text_layout_id, function_name_range, finalize_color(defcolor_int_constant, 1));
                function_name_range = { };
            }
            
            if (!token_it_inc_all(&it)) {
                break;
            }
        }
    }
}

function void penguin_draw_cpp_token_colors(Application_Links *app, Text_Layout_ID text_layout_id, Token_Array array) {
    ProfileScope(app, "Penguin draw cpp token colors");
    
    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
    i64 first_index = token_index_from_pos(&array, visible_range.first);
    Token_Iterator_Array it = token_iterator_index(0, &array, first_index);
    for (;;) {
        Token* token = token_it_read(&it);
        if (token->pos >= visible_range.one_past_last) {
            break;
        }
        FColor color = penguin_get_token_color_cpp(*token);
        ARGB_Color argb = fcolor_resolve(color);
        paint_text_color(app, text_layout_id, Ii64_size(token->pos, token->size), argb);
        if (!token_it_inc_all(&it)) {
            break;
        }
    }
}