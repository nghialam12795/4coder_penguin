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

#if !defined(FCODER_NGHIALAM_COMMANDS)
#define FCODER_NGHIALAM_COMMAND

//~ NOTE(Nghia Lam): Main APIs
CUSTOM_COMMAND_SIG(nghialam_startup);             // Custom layer startup.
CUSTOM_COMMAND_SIG(toggle_battery_saver);         // Change between batter saver mode <-> normal mode.
CUSTOM_COMMAND_SIG(toggle_filebar_position);      // Change filebar on top <-> bottom position.
CUSTOM_COMMAND_SIG(windmove_panel_up);            // Move up from the active view.
CUSTOM_COMMAND_SIG(windmove_panel_down);          // Move down from the active view.
CUSTOM_COMMAND_SIG(windmove_panel_left);          // Move left from the active view.
CUSTOM_COMMAND_SIG(windmove_panel_right);         // Move right from the active view.
CUSTOM_COMMAND_SIG(windmove_panel_swap_up);       // Swap the buffer up from the active view.
CUSTOM_COMMAND_SIG(windmove_panel_swap_down);     // Swap the buffer down from the active view.
CUSTOM_COMMAND_SIG(windmove_panel_swap_left);     // Swap the buffer left from the active view.
CUSTOM_COMMAND_SIG(windmove_panel_swap_right);    // Swap the buffer right from the active view.

CUSTOM_COMMAND_SIG(vim_motion_left);                // Move cursor left in normal mode.
CUSTOM_COMMAND_SIG(vim_motion_down);                // Move cursor down in normal mode.
CUSTOM_COMMAND_SIG(vim_motion_up);                  // Move cursor up in normal mode.
CUSTOM_COMMAND_SIG(vim_motion_right);               // Move cursor right in normal mode.
CUSTOM_COMMAND_SIG(vim_motion_word);                // Move cursor to the beginning of next word.
CUSTOM_COMMAND_SIG(vim_motion_word_end);            // Move cursor to the end of next word.
CUSTOM_COMMAND_SIG(vim_motion_word_backward);       // Move cursor to the beginning of previous word.
CUSTOM_COMMAND_SIG(vim_motion_to_blank_line_up);    // Move cursor to the nearest blank line up.
CUSTOM_COMMAND_SIG(vim_motion_to_blank_line_down);  // Move cursor to the nearest blank line down.
CUSTOM_COMMAND_SIG(vim_enter_normal_mode);          // Change to Vim-Editor type Normal Mode.
CUSTOM_COMMAND_SIG(vim_enter_insert_mode);          // Change to Vim-Editor type Insert Mode.
CUSTOM_COMMAND_SIG(vim_start_mouse_select);         // Vim behavior when using mouse left button.
CUSTOM_COMMAND_SIG(vim_write_text_input);           // Vim writing text to files.
CUSTOM_COMMAND_SIG(vim_write_text_and_auto_indent); // Vim writing text to files with indentation.

//~ NOTE(Nghia Lam): My Implementation
CUSTOM_COMMAND_SIG(nghialam_startup)
CUSTOM_DOC("NghiaLam's custom layer startup event") {
  ProfileScope(app, "default startup");
  
  // Init startup code here
}

CUSTOM_COMMAND_SIG(toggle_battery_saver)
CUSTOM_DOC("Toggle battery saver mode") {
  global_battery_saver = !global_battery_saver;
}

CUSTOM_COMMAND_SIG(toggle_filebar_position)
CUSTOM_DOC("Toggle the position of filebar") {
  global_filebar_top = !global_filebar_top;
}

CUSTOM_COMMAND_SIG(windmove_panel_up)
CUSTOM_DOC("Move up from the active view.") {
  NL_WindmoveToPanel(app, WINMOVE_UP, false);
}

CUSTOM_COMMAND_SIG(windmove_panel_down)
CUSTOM_DOC("Move down from the active view.") {
  NL_WindmoveToPanel(app, WINMOVE_DOWN, false);
}

CUSTOM_COMMAND_SIG(windmove_panel_left)
CUSTOM_DOC("Move left from the active view.") {
  NL_WindmoveToPanel(app, WINMOVE_LEFT, false);
}

CUSTOM_COMMAND_SIG(windmove_panel_right)
CUSTOM_DOC("Move right from the active view.") {
  NL_WindmoveToPanel(app, WINMOVE_RIGHT, false);
}

CUSTOM_COMMAND_SIG(windmove_panel_swap_up)
CUSTOM_DOC("Swap buffer up from the active view.") {
  NL_WindmoveToPanel(app, WINMOVE_UP, true);
}

CUSTOM_COMMAND_SIG(windmove_panel_swap_down)
CUSTOM_DOC("Swap buffer down from the active view.") {
  NL_WindmoveToPanel(app, WINMOVE_DOWN, true);
}

CUSTOM_COMMAND_SIG(windmove_panel_swap_left)
CUSTOM_DOC("Swap buffer left from the active view.") {
  NL_WindmoveToPanel(app, WINMOVE_LEFT, true);
}

CUSTOM_COMMAND_SIG(windmove_panel_swap_right)
CUSTOM_DOC("Swap buffer right from the active view.") {
  NL_WindmoveToPanel(app, WINMOVE_RIGHT, true);
}

CUSTOM_COMMAND_SIG(vim_enter_normal_mode)
CUSTOM_DOC("[vim] Change to Vim-Editor type Normal Mode.") {
  NL_VimEnterMode(app, VIMMODE_NORMAL);
}

CUSTOM_COMMAND_SIG(vim_enter_insert_mode)
CUSTOM_DOC("[vim] Change to Vim-Editor type Insert Mode.") {
  NL_VimEnterMode(app, VIMMODE_INSERT);
}

CUSTOM_COMMAND_SIG(vim_start_mouse_select)
CUSTOM_DOC("[vim] Sets the cursor position and mark to the mouse position and enters normal mode.") {
  View_ID view = get_active_view(app, Access_ReadVisible);
  
  Mouse_State mouse = get_mouse_state(app);
  i64 pos = view_pos_from_xy(app, view, V2f32(mouse.p));
  
  vim_enter_normal_mode(app);
  
  view_set_cursor_and_preferred_x(app, view, seek_pos(pos));
  view_set_mark(app, view, seek_pos(pos));
}

CUSTOM_COMMAND_SIG(vim_write_text_and_auto_indent)
CUSTOM_DOC("[vim] Inserts text and auto-indents the line on which the cursor sits if any of the text contains 'layout punctuation' such as ;:{}()[]# and new lines") {
  ProfileScope(app, "[vim] write and auto indent");
  User_Input in = get_current_input(app);
  String_Const_u8 insert = to_writable(&in);
  if (insert.str != 0 && insert.size > 0) {
    b32 do_auto_indent = false;
    for (u64 i = 0; !do_auto_indent && i < insert.size; i += 1) {
      switch (insert.str[i]) {
        case ';': case ':':
        case '{': case '}':
        case '(': case ')':
        case '[': case ']':
        case '#':
        case '\n': case '\t':
        {
          do_auto_indent = true;
        }break;
      }
    }
    if (do_auto_indent) {
      View_ID view = get_active_view(app, Access_ReadWriteVisible);
      Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
      
      Range_i64 pos = {};
      if (view_has_highlighted_range(app, view)) {
        pos = get_view_range(app, view);
      } else {
        pos.min = pos.max = view_get_cursor_pos(app, view);
      }
      
      vim_write_text_input(app);
      
      i64 end_pos = view_get_cursor_pos(app, view);
      pos.min = Min(pos.min, end_pos);
      pos.max = Max(pos.max, end_pos);
      
      auto_indent_buffer(app, buffer, pos, 0);
      move_past_lead_whitespace(app, view, buffer);
    }
    else {
      vim_write_text_input(app);
    }
  }
}

CUSTOM_COMMAND_SIG(vim_write_text_input)
CUSTOM_DOC("[vim] Inserts whatever text was used to trigger this command.") {
  User_Input in = get_current_input(app);
  String_Const_u8 insert = to_writable(&in);
  
  if (sizeof(VIM_ESCAPE_SEQUENCE) >= 3) {
    u8 this_char         = insert.size > 0 ? insert.str[0] : 0;
    u8 prev_char         = global_vim_prev_char;
    global_vim_prev_char = this_char;
    if (this_char &&
        prev_char == VIM_ESCAPE_SEQUENCE[0] &&
        this_char == VIM_ESCAPE_SEQUENCE[1]) {
      backspace_char(app);
      vim_enter_normal_mode(app);
      return;
    }
  }
  
  write_text(app, insert);
}

CUSTOM_COMMAND_SIG(vim_motion_left)
CUSTOM_DOC("[vim] Move cursor left in normal mode.") {
  move_left(app);
  set_mark(app);
}

CUSTOM_COMMAND_SIG(vim_motion_down)
CUSTOM_DOC("[vim] Move cursor down in normal mode.") {
  move_down(app);
  set_mark(app);
}

CUSTOM_COMMAND_SIG(vim_motion_up)
CUSTOM_DOC("[vim] Move cursor up in normal mode.") {
  move_up(app);
  set_mark(app);
}

CUSTOM_COMMAND_SIG(vim_motion_right)
CUSTOM_DOC("[vim] Move cursor right in normal mode.") {
  move_right(app);
  set_mark(app);
}

CUSTOM_COMMAND_SIG(vim_motion_word)
CUSTOM_DOC("[vim] Move cursor to the beginning of next word.") {
  move_right_alpha_numeric_or_camel_boundary(app);
  set_mark(app);
}

CUSTOM_COMMAND_SIG(vim_motion_word_end) 
CUSTOM_DOC("[vim]  Move cursor to the end of next word.") {
  move_right_alpha_numeric_boundary(app);
  set_mark(app);
}

CUSTOM_COMMAND_SIG(vim_motion_word_backward)
CUSTOM_DOC("[vim] Move cursor to the beginning of previous word.") {
  move_left_alpha_numeric_boundary(app);
  set_mark(app);
}

CUSTOM_COMMAND_SIG(vim_motion_to_blank_line_up)
CUSTOM_DOC("[vim] Move cursor to the nearest blank line up.") {
  move_up_to_blank_line_end(app);
  set_mark(app);
}

CUSTOM_COMMAND_SIG(vim_motion_to_blank_line_down)
CUSTOM_DOC("[vim] Move cursor to the nearest blank line down.") {
  move_down_to_blank_line_end(app);
  set_mark(app);
}

#endif // FCODER_NGHIALAM_COMMANDS