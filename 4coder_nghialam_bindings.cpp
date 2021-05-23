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

#if !defined(FCODER_NGHIALAM_BINDINGS)
#define FCODER_NGHIALAM_BINDINGS

//~ NOTE(Nghia Lam): Main APIs
function void NL_SetupDefaultBindings(Mapping *mapping, i64 global_id, i64 file_id, i64 code_id);
function void NL_SetupMacBindings(Mapping *mapping, i64 global_id, i64 file_id, i64 code_id);
function void NL_SetupVimBindings(Mapping *mapping, i64 global_id);

//~ NOTE(Nghia Lam): My Implementation
function void NL_SetupMacBindings(Mapping *mapping, i64 global_id, i64 file_id, i64 code_id) {
  MappingScope();
  SelectMapping(mapping);
  
  SelectMap(global_id);
  Bind(keyboard_macro_start_recording , KeyCode_U, KeyCode_Command);
  Bind(keyboard_macro_finish_recording, KeyCode_U, KeyCode_Command, KeyCode_Shift);
  Bind(keyboard_macro_replay,           KeyCode_U, KeyCode_Control);
  Bind(change_active_panel,           KeyCode_Comma, KeyCode_Command);
  Bind(change_active_panel_backwards, KeyCode_Comma, KeyCode_Command, KeyCode_Shift);
  Bind(interactive_new,               KeyCode_N, KeyCode_Command);
  Bind(interactive_open_or_new,       KeyCode_O, KeyCode_Command);
  Bind(open_in_other,                 KeyCode_O, KeyCode_Control);
  Bind(interactive_kill_buffer,       KeyCode_K, KeyCode_Command);
  Bind(interactive_switch_buffer,     KeyCode_I, KeyCode_Command);
  Bind(project_go_to_root_directory,  KeyCode_H, KeyCode_Command);
  Bind(save_all_dirty_buffers,        KeyCode_S, KeyCode_Command, KeyCode_Shift);
  Bind(change_to_build_panel,         KeyCode_Period, KeyCode_Control);
  Bind(close_build_panel,             KeyCode_Comma, KeyCode_Control);
  Bind(goto_next_jump,                KeyCode_N, KeyCode_Control);
  Bind(goto_prev_jump,                KeyCode_N, KeyCode_Control, KeyCode_Shift);
  Bind(build_in_build_panel,          KeyCode_M, KeyCode_Control);
  Bind(goto_first_jump,               KeyCode_M, KeyCode_Control, KeyCode_Shift);
  Bind(toggle_filebar,                KeyCode_B, KeyCode_Control);
  Bind(execute_any_cli,               KeyCode_Z, KeyCode_Control);
  Bind(execute_previous_cli,          KeyCode_Z, KeyCode_Control, KeyCode_Shift);
  Bind(command_lister,                KeyCode_X, KeyCode_Control);
  Bind(project_command_lister,        KeyCode_X, KeyCode_Control, KeyCode_Shift);
  Bind(quick_swap_buffer,             KeyCode_BackwardSlash, KeyCode_Command);
  Bind(jump_to_last_point,            KeyCode_P, KeyCode_Command);
  Bind(list_all_functions_current_buffer, KeyCode_I, KeyCode_Command, KeyCode_Shift);
  Bind(project_fkey_command, KeyCode_F1);
  Bind(project_fkey_command, KeyCode_F2);
  Bind(project_fkey_command, KeyCode_F3);
  Bind(project_fkey_command, KeyCode_F4);
  Bind(project_fkey_command, KeyCode_F5);
  Bind(project_fkey_command, KeyCode_F6);
  Bind(project_fkey_command, KeyCode_F7);
  Bind(project_fkey_command, KeyCode_F8);
  Bind(project_fkey_command, KeyCode_F9);
  Bind(project_fkey_command, KeyCode_F10);
  Bind(project_fkey_command, KeyCode_F11);
  Bind(project_fkey_command, KeyCode_F12);
  Bind(project_fkey_command, KeyCode_F13);
  Bind(project_fkey_command, KeyCode_F14);
  Bind(project_fkey_command, KeyCode_F15);
  Bind(project_fkey_command, KeyCode_F16);
  Bind(exit_4coder,          KeyCode_F4, KeyCode_Alt);
  
  SelectMap(file_id);
  Bind(delete_char,            KeyCode_Delete);
  Bind(backspace_char,         KeyCode_Backspace);
  Bind(move_up,                KeyCode_Up);
  Bind(move_down,              KeyCode_Down);
  Bind(move_left,              KeyCode_Left);
  Bind(move_right,             KeyCode_Right);
  Bind(seek_end_of_line,       KeyCode_End);
  Bind(seek_beginning_of_line, KeyCode_Home);
  Bind(page_up,                KeyCode_PageUp);
  Bind(page_down,              KeyCode_PageDown);
  Bind(goto_beginning_of_file, KeyCode_PageUp, KeyCode_Command);
  Bind(goto_end_of_file,       KeyCode_PageDown, KeyCode_Command);
  Bind(move_up_to_blank_line_end,        KeyCode_Up, KeyCode_Command);
  Bind(move_down_to_blank_line_end,      KeyCode_Down, KeyCode_Command);
  Bind(move_left_whitespace_boundary,    KeyCode_Left, KeyCode_Command);
  Bind(move_right_whitespace_boundary,   KeyCode_Right, KeyCode_Command);
  Bind(move_line_up,                     KeyCode_Up, KeyCode_Alt);
  Bind(move_line_down,                   KeyCode_Down, KeyCode_Alt);
  Bind(backspace_alpha_numeric_boundary, KeyCode_Backspace, KeyCode_Command);
  Bind(delete_alpha_numeric_boundary,    KeyCode_Delete, KeyCode_Command);
  Bind(snipe_backward_whitespace_or_token_boundary, KeyCode_Backspace, KeyCode_Control);
  Bind(snipe_forward_whitespace_or_token_boundary,  KeyCode_Delete, KeyCode_Control);
  Bind(set_mark,                    KeyCode_Space, KeyCode_Control);
  Bind(set_mark,                    KeyCode_ForwardSlash, KeyCode_Command);
  Bind(replace_in_range,            KeyCode_A, KeyCode_Command);
  Bind(copy,                        KeyCode_C, KeyCode_Command);
  Bind(delete_range,                KeyCode_D, KeyCode_Command);
  Bind(delete_line,                 KeyCode_D, KeyCode_Command, KeyCode_Shift);
  Bind(center_view,                 KeyCode_E, KeyCode_Command);
  Bind(left_adjust_view,            KeyCode_E, KeyCode_Command, KeyCode_Shift);
  Bind(search,                      KeyCode_F, KeyCode_Command);
  Bind(list_all_locations,          KeyCode_F, KeyCode_Command, KeyCode_Shift);
  Bind(list_all_substring_locations_case_insensitive, KeyCode_F, KeyCode_Control);
  Bind(goto_line,                   KeyCode_G, KeyCode_Command);
  Bind(list_all_locations_of_selection,  KeyCode_G, KeyCode_Command, KeyCode_Shift);
  Bind(snippet_lister,              KeyCode_J, KeyCode_Command);
  Bind(kill_buffer,                 KeyCode_K, KeyCode_Command, KeyCode_Shift);
  Bind(duplicate_line,              KeyCode_L, KeyCode_Command);
  Bind(cursor_mark_swap,            KeyCode_M, KeyCode_Command);
  Bind(reopen,                      KeyCode_O, KeyCode_Command, KeyCode_Shift);
  Bind(query_replace,               KeyCode_Q, KeyCode_Command);
  Bind(query_replace_identifier,    KeyCode_Q, KeyCode_Command, KeyCode_Shift);
  Bind(query_replace_selection,     KeyCode_Q, KeyCode_Control);
  Bind(reverse_search,              KeyCode_R, KeyCode_Command);
  Bind(save,                        KeyCode_S, KeyCode_Command);
  Bind(save_all_dirty_buffers,      KeyCode_S, KeyCode_Command, KeyCode_Shift);
  Bind(search_identifier,           KeyCode_T, KeyCode_Command);
  Bind(list_all_locations_of_identifier, KeyCode_T, KeyCode_Command, KeyCode_Shift);
  Bind(paste_and_indent,            KeyCode_V, KeyCode_Command);
  Bind(paste_next_and_indent,       KeyCode_V, KeyCode_Command, KeyCode_Shift);
  Bind(cut,                         KeyCode_X, KeyCode_Command);
  Bind(redo,                        KeyCode_Y, KeyCode_Command);
  Bind(undo,                        KeyCode_Z, KeyCode_Command);
  Bind(view_buffer_other_panel,     KeyCode_1, KeyCode_Command);
  Bind(swap_panels,                 KeyCode_2, KeyCode_Command);
  Bind(if_read_only_goto_position,  KeyCode_Return);
  Bind(if_read_only_goto_position_same_panel, KeyCode_Return, KeyCode_Shift);
  Bind(view_jump_list_with_lister,  KeyCode_Period, KeyCode_Command, KeyCode_Shift);
  
  SelectMap(code_id);
  Bind(move_left_alpha_numeric_boundary,           KeyCode_Left, KeyCode_Command);
  Bind(move_right_alpha_numeric_boundary,          KeyCode_Right, KeyCode_Command);
  Bind(move_left_alpha_numeric_or_camel_boundary,  KeyCode_Left, KeyCode_Control);
  Bind(move_right_alpha_numeric_or_camel_boundary, KeyCode_Right, KeyCode_Control);
  Bind(comment_line_toggle,        KeyCode_Semicolon, KeyCode_Command);
  Bind(word_complete,              KeyCode_Tab);
  Bind(auto_indent_range,          KeyCode_Tab, KeyCode_Command);
  Bind(auto_indent_line_at_cursor, KeyCode_Tab, KeyCode_Shift);
  Bind(word_complete_drop_down,    KeyCode_Tab, KeyCode_Shift, KeyCode_Command);
  Bind(write_block,                KeyCode_R, KeyCode_Control);
  Bind(write_todo,                 KeyCode_T, KeyCode_Control);
  Bind(write_note,                 KeyCode_Y, KeyCode_Control);
  Bind(list_all_locations_of_type_definition,               KeyCode_D, KeyCode_Control);
  Bind(list_all_locations_of_type_definition_of_identifier, KeyCode_T, KeyCode_Control, KeyCode_Shift);
  Bind(open_long_braces,           KeyCode_LeftBracket, KeyCode_Command);
  Bind(open_long_braces_semicolon, KeyCode_LeftBracket, KeyCode_Command, KeyCode_Shift);
  Bind(open_long_braces_break,     KeyCode_RightBracket, KeyCode_Command, KeyCode_Shift);
  Bind(select_surrounding_scope,   KeyCode_LeftBracket, KeyCode_Control);
  Bind(select_surrounding_scope_maximal, KeyCode_LeftBracket, KeyCode_Control, KeyCode_Shift);
  Bind(select_prev_scope_absolute, KeyCode_RightBracket, KeyCode_Control);
  Bind(select_prev_top_most_scope, KeyCode_RightBracket, KeyCode_Control, KeyCode_Shift);
  Bind(select_next_scope_absolute, KeyCode_Quote, KeyCode_Control);
  Bind(select_next_scope_after_current, KeyCode_Quote, KeyCode_Control, KeyCode_Shift);
  Bind(place_in_scope,             KeyCode_ForwardSlash, KeyCode_Control);
  Bind(delete_current_scope,       KeyCode_Minus, KeyCode_Control);
  Bind(if0_off,                    KeyCode_I, KeyCode_Control);
  Bind(open_file_in_quotes,        KeyCode_1, KeyCode_Control);
  Bind(open_matching_file_cpp,     KeyCode_2, KeyCode_Control);
  Bind(write_zero_struct,          KeyCode_0, KeyCode_Command);
  Bind(jump_to_definition_at_cursor, KeyCode_W, KeyCode_Command);
}

function void NL_SetupDefaultBindings(Mapping *mapping, i64 global_id, i64 file_id, i64 code_id) {
  MappingScope();
  SelectMapping(mapping);
  
  SelectMap(global_id);
  Bind(keyboard_macro_start_recording , KeyCode_U, KeyCode_Control);
  Bind(keyboard_macro_finish_recording, KeyCode_U, KeyCode_Control, KeyCode_Shift);
  Bind(keyboard_macro_replay,           KeyCode_U, KeyCode_Alt);
  Bind(change_active_panel,           KeyCode_Comma, KeyCode_Control);
  Bind(change_active_panel_backwards, KeyCode_Comma, KeyCode_Control, KeyCode_Shift);
  Bind(interactive_new,               KeyCode_N, KeyCode_Control);
  Bind(interactive_open_or_new,       KeyCode_O, KeyCode_Control);
  Bind(open_in_other,                 KeyCode_O, KeyCode_Alt);
  Bind(interactive_kill_buffer,       KeyCode_K, KeyCode_Control);
  Bind(interactive_switch_buffer,     KeyCode_I, KeyCode_Control);
  Bind(project_go_to_root_directory,  KeyCode_H, KeyCode_Control);
  Bind(save_all_dirty_buffers,        KeyCode_S, KeyCode_Control, KeyCode_Shift);
  Bind(change_to_build_panel,         KeyCode_Period, KeyCode_Alt);
  Bind(close_build_panel,             KeyCode_Comma, KeyCode_Alt);
  Bind(goto_next_jump,                KeyCode_N, KeyCode_Alt);
  Bind(goto_prev_jump,                KeyCode_N, KeyCode_Alt, KeyCode_Shift);
  Bind(build_in_build_panel,          KeyCode_M, KeyCode_Alt);
  Bind(goto_first_jump,               KeyCode_M, KeyCode_Alt, KeyCode_Shift);
  Bind(toggle_filebar,                KeyCode_B, KeyCode_Alt);
  Bind(execute_any_cli,               KeyCode_Z, KeyCode_Alt);
  Bind(execute_previous_cli,          KeyCode_Z, KeyCode_Alt, KeyCode_Shift);
  Bind(command_lister,                KeyCode_X, KeyCode_Alt);
  Bind(project_command_lister,        KeyCode_X, KeyCode_Alt, KeyCode_Shift);
  Bind(quick_swap_buffer,             KeyCode_BackwardSlash, KeyCode_Alt);
  Bind(jump_to_last_point,            KeyCode_P, KeyCode_Control);
  Bind(list_all_functions_current_buffer_lister, KeyCode_I, KeyCode_Control, KeyCode_Shift);
  Bind(exit_4coder,          KeyCode_F4, KeyCode_Alt);
  Bind(project_fkey_command, KeyCode_F1);
  Bind(project_fkey_command, KeyCode_F2);
  Bind(project_fkey_command, KeyCode_F3);
  Bind(project_fkey_command, KeyCode_F4);
  Bind(project_fkey_command, KeyCode_F5);
  Bind(project_fkey_command, KeyCode_F6);
  Bind(project_fkey_command, KeyCode_F7);
  Bind(project_fkey_command, KeyCode_F8);
  Bind(project_fkey_command, KeyCode_F9);
  Bind(project_fkey_command, KeyCode_F10);
  Bind(project_fkey_command, KeyCode_F11);
  Bind(project_fkey_command, KeyCode_F12);
  Bind(project_fkey_command, KeyCode_F13);
  Bind(project_fkey_command, KeyCode_F14);
  Bind(project_fkey_command, KeyCode_F15);
  Bind(project_fkey_command, KeyCode_F16);
  
  SelectMap(file_id);
  Bind(delete_char,            KeyCode_Delete);
  Bind(backspace_char,         KeyCode_Backspace);
  Bind(move_up,                KeyCode_Up);
  Bind(move_down,              KeyCode_Down);
  Bind(move_left,              KeyCode_Left);
  Bind(move_right,             KeyCode_Right);
  Bind(seek_end_of_line,       KeyCode_End);
  Bind(seek_beginning_of_line, KeyCode_Home);
  Bind(page_up,                KeyCode_PageUp);
  Bind(page_down,              KeyCode_PageDown);
  Bind(goto_beginning_of_file, KeyCode_PageUp, KeyCode_Control);
  Bind(goto_end_of_file,       KeyCode_PageDown, KeyCode_Control);
  Bind(move_up_to_blank_line_end,        KeyCode_Up, KeyCode_Control);
  Bind(move_down_to_blank_line_end,      KeyCode_Down, KeyCode_Control);
  Bind(move_left_whitespace_boundary,    KeyCode_Left, KeyCode_Control);
  Bind(move_right_whitespace_boundary,   KeyCode_Right, KeyCode_Control);
  Bind(move_line_up,                     KeyCode_Up, KeyCode_Alt);
  Bind(move_line_down,                   KeyCode_Down, KeyCode_Alt);
  Bind(backspace_alpha_numeric_boundary, KeyCode_Backspace, KeyCode_Control);
  Bind(delete_alpha_numeric_boundary,    KeyCode_Delete, KeyCode_Control);
  Bind(snipe_backward_whitespace_or_token_boundary, KeyCode_Backspace, KeyCode_Alt);
  Bind(snipe_forward_whitespace_or_token_boundary,  KeyCode_Delete, KeyCode_Alt);
  Bind(set_mark,                    KeyCode_Space, KeyCode_Control);
  Bind(replace_in_range,            KeyCode_A, KeyCode_Control);
  Bind(copy,                        KeyCode_C, KeyCode_Control);
  Bind(delete_range,                KeyCode_D, KeyCode_Control);
  Bind(delete_line,                 KeyCode_D, KeyCode_Control, KeyCode_Shift);
  Bind(center_view,                 KeyCode_E, KeyCode_Control);
  Bind(left_adjust_view,            KeyCode_E, KeyCode_Control, KeyCode_Shift);
  Bind(search,                      KeyCode_F, KeyCode_Control);
  Bind(list_all_locations,          KeyCode_F, KeyCode_Control, KeyCode_Shift);
  Bind(list_all_substring_locations_case_insensitive, KeyCode_F, KeyCode_Alt);
  Bind(goto_line,                   KeyCode_G, KeyCode_Control);
  Bind(list_all_locations_of_selection,  KeyCode_G, KeyCode_Control, KeyCode_Shift);
  Bind(snippet_lister,              KeyCode_J, KeyCode_Control);
  Bind(kill_buffer,                 KeyCode_K, KeyCode_Control, KeyCode_Shift);
  Bind(duplicate_line,              KeyCode_L, KeyCode_Control);
  Bind(cursor_mark_swap,            KeyCode_M, KeyCode_Control);
  Bind(reopen,                      KeyCode_O, KeyCode_Control, KeyCode_Shift);
  Bind(query_replace,               KeyCode_Q, KeyCode_Control);
  Bind(query_replace_identifier,    KeyCode_Q, KeyCode_Control, KeyCode_Shift);
  Bind(query_replace_selection,     KeyCode_Q, KeyCode_Alt);
  Bind(reverse_search,              KeyCode_R, KeyCode_Control);
  Bind(save,                        KeyCode_S, KeyCode_Control);
  Bind(save_all_dirty_buffers,      KeyCode_S, KeyCode_Control, KeyCode_Shift);
  Bind(search_identifier,           KeyCode_T, KeyCode_Control);
  Bind(list_all_locations_of_identifier, KeyCode_T, KeyCode_Control, KeyCode_Shift);
  Bind(paste_and_indent,            KeyCode_V, KeyCode_Control);
  Bind(paste_next_and_indent,       KeyCode_V, KeyCode_Control, KeyCode_Shift);
  Bind(cut,                         KeyCode_X, KeyCode_Control);
  Bind(redo,                        KeyCode_Y, KeyCode_Control);
  Bind(undo,                        KeyCode_Z, KeyCode_Control);
  Bind(view_buffer_other_panel,     KeyCode_1, KeyCode_Control);
  Bind(swap_panels,                 KeyCode_2, KeyCode_Control);
  Bind(if_read_only_goto_position,  KeyCode_Return);
  Bind(if_read_only_goto_position_same_panel, KeyCode_Return, KeyCode_Shift);
  Bind(view_jump_list_with_lister,  KeyCode_Period, KeyCode_Control, KeyCode_Shift);
  
  SelectMap(code_id);
  ParentMap(file_id);
  Bind(move_left_alpha_numeric_boundary,           KeyCode_Left, KeyCode_Control);
  Bind(move_right_alpha_numeric_boundary,          KeyCode_Right, KeyCode_Control);
  Bind(move_left_alpha_numeric_or_camel_boundary,  KeyCode_Left, KeyCode_Alt);
  Bind(move_right_alpha_numeric_or_camel_boundary, KeyCode_Right, KeyCode_Alt);
  Bind(comment_line_toggle,        KeyCode_Semicolon, KeyCode_Control);
  Bind(word_complete,              KeyCode_Tab);
  Bind(auto_indent_range,          KeyCode_Tab, KeyCode_Control);
  Bind(auto_indent_line_at_cursor, KeyCode_Tab, KeyCode_Shift);
  Bind(word_complete_drop_down,    KeyCode_Tab, KeyCode_Shift, KeyCode_Control);
  Bind(write_block,                KeyCode_R, KeyCode_Alt);
  Bind(write_todo,                 KeyCode_T, KeyCode_Alt);
  Bind(write_note,                 KeyCode_Y, KeyCode_Alt);
  Bind(list_all_locations_of_type_definition,               KeyCode_D, KeyCode_Alt);
  Bind(list_all_locations_of_type_definition_of_identifier, KeyCode_T, KeyCode_Alt, KeyCode_Shift);
  Bind(open_long_braces,           KeyCode_LeftBracket, KeyCode_Control);
  Bind(open_long_braces_semicolon, KeyCode_LeftBracket, KeyCode_Control, KeyCode_Shift);
  Bind(open_long_braces_break,     KeyCode_RightBracket, KeyCode_Control, KeyCode_Shift);
  Bind(select_surrounding_scope,   KeyCode_LeftBracket, KeyCode_Alt);
  Bind(select_surrounding_scope_maximal, KeyCode_LeftBracket, KeyCode_Alt, KeyCode_Shift);
  Bind(select_prev_scope_absolute, KeyCode_RightBracket, KeyCode_Alt);
  Bind(select_prev_top_most_scope, KeyCode_RightBracket, KeyCode_Alt, KeyCode_Shift);
  Bind(select_next_scope_absolute, KeyCode_Quote, KeyCode_Alt);
  Bind(select_next_scope_after_current, KeyCode_Quote, KeyCode_Alt, KeyCode_Shift);
  Bind(place_in_scope,             KeyCode_ForwardSlash, KeyCode_Alt);
  Bind(delete_current_scope,       KeyCode_Minus, KeyCode_Alt);
  Bind(if0_off,                    KeyCode_I, KeyCode_Alt);
  Bind(open_file_in_quotes,        KeyCode_1, KeyCode_Alt);
  Bind(open_matching_file_cpp,     KeyCode_2, KeyCode_Alt);
  Bind(write_zero_struct,          KeyCode_0, KeyCode_Control);
  Bind(jump_to_definition_at_cursor, KeyCode_W, KeyCode_Control);
}

function void NL_SetupVimBindings(Mapping *mapping, i64 global_id) {
  
  String_ID vim_mapid_normal = vars_save_string_lit("keys_vim_normal");
  //String_ID vim_mapid_visual = vars_save_string_lit("keys_vim_visual");
  String_ID vim_mapid_yank = vars_save_string_lit("keys_vim_yank");
  String_ID vim_mapid_goto = vars_save_string_lit("keys_goto");
  String_ID vim_mapid_view = vars_save_string_lit("keys_vim_view");
  String_ID vim_mapid_delete = vars_save_string_lit("keys_vim_delete");
  String_ID vim_mapid_leader = vars_save_string_lit("keys_vim_leader");
  String_ID vim_mapid_leader_buffer = vars_save_string_lit("keys_vim_leader_buffer");
  String_ID vim_mapid_leader_window = vars_save_string_lit("keys_vim_leader_window");
  
  MappingScope();
  SelectMapping(mapping);
  
  // NOTE(Nghia Lam): Vim Normal map
  SelectMap(vim_mapid_normal);
  ParentMap(global_id);
  
  BindMouse(vim_start_mouse_select, MouseCode_Left);
  BindMouseRelease(click_set_cursor, MouseCode_Left);
  BindCore(click_set_cursor_and_mark, CoreCode_ClickActivateView);
  BindMouseMove(click_set_cursor_if_lbutton);
  
  Bind(undo,                                      KeyCode_U);
  Bind(delete_char,                               KeyCode_X);
  Bind(paste_and_indent,                          KeyCode_P);
  Bind(command_lister,                            KeyCode_Semicolon,     KeyCode_Shift);
  Bind(seek_beginning_of_textual_line,            KeyCode_0);
  Bind(seek_end_of_textual_line,                  KeyCode_4,             KeyCode_Shift);
  Bind(seek_beginning_of_textual_line,            KeyCode_H,             KeyCode_Shift);
  Bind(seek_end_of_textual_line,                  KeyCode_L,             KeyCode_Shift);
  Bind(if_read_only_goto_position,                KeyCode_Return);
  Bind(if_read_only_goto_position_same_panel,     KeyCode_Return,        KeyCode_Shift);
  Bind(vim_motion_to_end_of_file,                 KeyCode_G,             KeyCode_Shift);
  Bind(vim_motion_up,                             KeyCode_K);
  Bind(vim_motion_down,                           KeyCode_J);
  Bind(vim_motion_left,                           KeyCode_H);
  Bind(vim_motion_right,                          KeyCode_L);
  Bind(vim_motion_word,                           KeyCode_W);
  Bind(vim_motion_word_end,                       KeyCode_E);
  Bind(vim_motion_word_backward,                  KeyCode_B);
  Bind(vim_motion_to_blank_line_up,               KeyCode_LeftBracket,   KeyCode_Shift);
  Bind(vim_motion_to_blank_line_down,             KeyCode_RightBracket,  KeyCode_Shift);
  Bind(vim_motion_to_blank_line_up,               KeyCode_K,             KeyCode_Control);
  Bind(vim_motion_to_blank_line_down,             KeyCode_J,             KeyCode_Control);
  Bind(vim_new_line_below,                        KeyCode_O);
  Bind(vim_new_line_above,                        KeyCode_O,             KeyCode_Shift);
  Bind(vim_enter_insert_mode_bol,                 KeyCode_I,             KeyCode_Shift);
  Bind(vim_enter_insert_mode_eol,                 KeyCode_A,             KeyCode_Shift);
  Bind(vim_enter_insert_mode,                     KeyCode_I);
  Bind(vim_enter_goto_mode,                       KeyCode_G);
  Bind(vim_enter_yank_mode,                       KeyCode_Y);
  Bind(vim_enter_view_mode,                       KeyCode_Z);
  Bind(vim_enter_append_mode,                     KeyCode_A);
  Bind(vim_enter_delete_mode,                     KeyCode_D);
  Bind(vim_enter_leader_mode,                     KeyCode_Space);
  
  // NOTE(Nghia Lam): Vim Delete map
  SelectMap(vim_mapid_delete);
  Bind(vim_enter_normal_mode,         KeyCode_Escape);
  Bind(vim_delete_line,               KeyCode_D);
  Bind(vim_delete_up,                 KeyCode_K);
  Bind(vim_delete_down,               KeyCode_J);
  Bind(vim_delete_word_end,           KeyCode_E);
  Bind(vim_delete_word_backward,      KeyCode_B);
  
  // NOTE(Nghia Lam): Vim Yank map
  SelectMap(vim_mapid_yank);
  Bind(vim_enter_normal_mode,         KeyCode_Escape);
  Bind(vim_yank_line,                 KeyCode_Y);
  
  // NOTE(Nghia Lam): Vim View map
  SelectMap(vim_mapid_view);
  Bind(vim_enter_normal_mode,         KeyCode_Escape);
  Bind(vim_view_center,               KeyCode_Z);
  
  // NOTE(Nghia Lam): Vim Goto map
  SelectMap(vim_mapid_goto);
  Bind(vim_enter_normal_mode,          KeyCode_Escape);
  Bind(vim_motion_to_begin_of_file,    KeyCode_G);
  Bind(vim_jump_to_defition_at_cursor, KeyCode_D);
  
  // NOTE(Nghia Lam): Vim Leader map
  SelectMap(vim_mapid_leader);
  Bind(vim_enter_normal_mode,         KeyCode_Escape);
  Bind(vim_enter_leader_buffer_mode,  KeyCode_B);
  Bind(vim_enter_leader_window_mode,  KeyCode_W);
  
  // NOTE(Nghia Lam): Vim Leader Window map
  SelectMap(vim_mapid_leader_window);
  Bind(vim_enter_normal_mode,         KeyCode_Escape);
  Bind(vim_window_vsplit,             KeyCode_V);
  Bind(vim_window_hsplit,             KeyCode_S);
  Bind(vim_window_panel_move_up,      KeyCode_K);
  Bind(vim_window_panel_move_down,    KeyCode_J);
  Bind(vim_window_panel_move_left,    KeyCode_H);
  Bind(vim_window_panel_move_right,   KeyCode_L);
  Bind(vim_window_panel_swap_up,      KeyCode_K,        KeyCode_Shift);
  Bind(vim_window_panel_swap_down,    KeyCode_J,        KeyCode_Shift);
  Bind(vim_window_panel_swap_left,    KeyCode_H,        KeyCode_Shift);
  Bind(vim_window_panel_swap_right,   KeyCode_L,        KeyCode_Shift);
  
  // NOTE(Nghia Lam): Vim Leader Buffer map
  SelectMap(vim_mapid_leader_buffer);
  Bind(vim_enter_normal_mode,         KeyCode_Escape);
  Bind(vim_buffer_save,               KeyCode_S);
  Bind(vim_buffer_kill,               KeyCode_K);
}

#endif // FCODER_NGHIALAM_BINDINGS