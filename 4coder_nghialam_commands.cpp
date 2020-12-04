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
CUSTOM_COMMAND_SIG(nghialam_startup);             // Custom layer startup
CUSTOM_COMMAND_SIG(toggle_battery_saver);         // Change between batter saver mode <-> normal mode
CUSTOM_COMMAND_SIG(toggle_filebar_position);      // Change filebar on top <-> bottom position
CUSTOM_COMMAND_SIG(windmove_panel_up);            // Move up from the active view
CUSTOM_COMMAND_SIG(windmove_panel_down);          // Move down from the active view
CUSTOM_COMMAND_SIG(windmove_panel_left);          // Move left from the active view
CUSTOM_COMMAND_SIG(windmove_panel_right);         // Move right from the active view
CUSTOM_COMMAND_SIG(windmove_panel_swap_up);       // Swap the buffer up from the active view
CUSTOM_COMMAND_SIG(windmove_panel_swap_down);     // Swap the buffer down from the active view
CUSTOM_COMMAND_SIG(windmove_panel_swap_left);     // Swap the buffer left from the active view
CUSTOM_COMMAND_SIG(windmove_panel_swap_right);    // Swap the buffer right from the active view

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

#endif // FCODER_NGHIALAM_COMMANDS