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
function void NL_SetupVimBindings(Mapping *mapping);

//~ NOTE(Nghia Lam): My Implementation
function void NL_SetupVimBindings(Mapping *mapping) {
  String_ID global_map_id = vars_save_string_lit("keys_global");
  
  MappingScope();
  SelectMapping(mapping);
  
  // NOTE(Nghia Lam): Vim Normal map
  SelectMap(vim_mapid_normal);
  ParentMap(global_map_id);
  
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